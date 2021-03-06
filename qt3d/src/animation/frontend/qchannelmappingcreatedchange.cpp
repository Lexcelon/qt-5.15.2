/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qchannelmappingcreatedchange_p.h"
#include "qchannelmappingcreatedchange_p_p.h"
#include <Qt3DAnimation/private/qabstractchannelmapping_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QChannelMappingCreatedChangeBasePrivate::QChannelMappingCreatedChangeBasePrivate(const QAbstractChannelMapping *mapping)
    : Qt3DCore::QNodeCreatedChangeBasePrivate(mapping)
    , m_mappingType(QAbstractChannelMappingPrivate::get(mapping)->m_mappingType)
{
}

/*!
    \class Qt3DAnimation::QChannelMappingCreatedChangeBase
    \inmodule Qt3DCore
    \brief Base class for handling creation changes for QAbstractSkeleton sub-classes.
*/
QChannelMappingCreatedChangeBase::QChannelMappingCreatedChangeBase(const QAbstractChannelMapping *mapping)
    : Qt3DCore::QNodeCreatedChangeBase(*new QChannelMappingCreatedChangeBasePrivate(mapping), mapping)
{
}

/*! \internal */
QChannelMappingCreatedChangeBase::~QChannelMappingCreatedChangeBase()
{
}

/*! \internal */
QChannelMappingCreatedChangeBase::MappingType QChannelMappingCreatedChangeBase::type() const
{
    Q_D(const QChannelMappingCreatedChangeBase);
    return d->m_mappingType;
}

} // namespace Qt3DAnimation

QT_END_NAMESPACE
