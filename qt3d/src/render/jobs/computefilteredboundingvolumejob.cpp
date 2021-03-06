/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "computefilteredboundingvolumejob_p.h"

#include <Qt3DRender/private/entity_p.h>
#include <Qt3DRender/private/renderlogging_p.h>
#include <Qt3DRender/private/sphere_p.h>
#include <Qt3DRender/private/job_common_p.h>
#include <Qt3DRender/private/managers_p.h>
#include <Qt3DRender/private/nodemanagers_p.h>

#include <QThread>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

namespace {

void expandWorldBoundingVolume(NodeManagers *manager,
                               Qt3DRender::Render::Sphere *sphere,
                               Qt3DRender::Render::Entity *node,
                               Qt3DRender::Render::Entity *excludeSubTree)
{
    Qt3DRender::Render::Sphere childSphere(*node->worldBoundingVolume());
    // Go to the nodes that have the most depth
    const auto childrenHandles = node->childrenHandles();
    for (const HEntity &handle : childrenHandles) {
        Entity *c = manager->renderNodesManager()->data(handle);
        if (c && c != excludeSubTree)
            expandWorldBoundingVolume(manager, &childSphere, c, excludeSubTree);
    }
    sphere->expandToContain(childSphere);
}

} // namespace

class ComputeFilteredBoundingVolumeJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    ComputeFilteredBoundingVolumeJobPrivate(ComputeFilteredBoundingVolumeJob *job) : Qt3DCore::QAspectJobPrivate(), m_job(job) {}
    ~ComputeFilteredBoundingVolumeJobPrivate() {}

    void postFrame(Qt3DCore::QAspectManager *aspectManager) override;

    ComputeFilteredBoundingVolumeJob *m_job;
    Qt3DRender::Render::Sphere m_sphere;
};

ComputeFilteredBoundingVolumeJob::ComputeFilteredBoundingVolumeJob()
    : Qt3DCore::QAspectJob(*new ComputeFilteredBoundingVolumeJobPrivate(this))
    , m_root(nullptr)
    , m_ignoreSubTree(nullptr)
    , m_manager(nullptr)
{
    SET_JOB_RUN_STAT_TYPE(this, JobTypes::ExpandBoundingVolume, 0)
}

void ComputeFilteredBoundingVolumeJob::setRoot(Entity *root)
{
    m_root = root;
}

void ComputeFilteredBoundingVolumeJob::setManagers(NodeManagers *manager)
{
    m_manager = manager;
}

void ComputeFilteredBoundingVolumeJob::ignoreSubTree(Entity *node)
{
    m_ignoreSubTree = node;
}

void ComputeFilteredBoundingVolumeJob::run()
{
    qCDebug(Jobs) << "Entering" << Q_FUNC_INFO << QThread::currentThread();
    Q_D(ComputeFilteredBoundingVolumeJob);
    d->m_sphere = {};

    if (!m_root)
        return;
    if (!m_ignoreSubTree) {
        d->m_sphere = *m_root->worldBoundingVolumeWithChildren();
        return;
    }

    bool isFilterChildOfRoot = false;
    Entity *parent = m_ignoreSubTree->parent();
    while (parent) {
        if (parent == m_root) {
            isFilterChildOfRoot = true;
            break;
        }
        parent = parent->parent();
    }
    if (!isFilterChildOfRoot) {
        d->m_sphere = *m_root->worldBoundingVolumeWithChildren();
        return;
    }

    expandWorldBoundingVolume(m_manager, &d->m_sphere, m_root, m_ignoreSubTree);

    qCDebug(Jobs) << "Exiting" << Q_FUNC_INFO << QThread::currentThread();
}

void ComputeFilteredBoundingVolumeJob::finished(Qt3DCore::QAspectManager *aspectManager, const Qt3DRender::Render::Sphere &sphere)
{
    Q_UNUSED(aspectManager)
    Q_UNUSED(sphere)
}

void ComputeFilteredBoundingVolumeJobPrivate::postFrame(Qt3DCore::QAspectManager *aspectManager)
{
    m_job->finished(aspectManager, m_sphere);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
