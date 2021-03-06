/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the config.tests of the Qt Toolkit.
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

#ifndef QWAYLANDXDGSURFACEV5_P_H
#define QWAYLANDXDGSURFACEV5_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qwayland-xdg-shell-unstable-v5_p.h"

#include <QtWaylandClient/qtwaylandclientglobal.h>
#include <QtWaylandClient/private/qwaylandshellsurface_p.h>

#include <QtCore/QSize>
#include <QtCore/QMargins>

QT_BEGIN_NAMESPACE

class QWindow;

namespace QtWaylandClient {

class QWaylandWindow;
class QWaylandInputDevice;
class QWaylandExtendedSurface;
class QWaylandXdgShellV5;

class Q_WAYLAND_CLIENT_EXPORT QWaylandXdgSurfaceV5 : public QWaylandShellSurface
        , public QtWayland::xdg_surface_v5
{
    Q_OBJECT
public:
    QWaylandXdgSurfaceV5(QWaylandXdgShellV5 *shell, QWaylandWindow *window);
    ~QWaylandXdgSurfaceV5() override;

    using QtWayland::xdg_surface_v5::resize;
    static resize_edge convertToResizeEdges(Qt::Edges edges);
    bool resize(QWaylandInputDevice *inputDevice, Qt::Edges edges) override;

    using QtWayland::xdg_surface_v5::move;
    bool move(QWaylandInputDevice *inputDevice) override;
    bool showWindowMenu(QWaylandInputDevice *seat) override;

    void setTitle(const QString &title) override;
    void setAppId(const QString &appId) override;

    void raise() override;
    void lower() override;
    void setContentOrientationMask(Qt::ScreenOrientations orientation) override;
    void setWindowFlags(Qt::WindowFlags flags) override;
    void sendProperty(const QString &name, const QVariant &value) override;

    void applyConfigure() override;
    void requestWindowStates(Qt::WindowStates states) override;
    bool wantsDecorations() const override;

private:
    void updateTransientParent(QWaylandWindow *parent);

private:
    QWaylandWindow *m_window = nullptr;
    QWaylandXdgShellV5* m_shell = nullptr;
    struct {
        Qt::WindowStates states = Qt::WindowNoState;
        bool isResizing = false;
        QSize size = {0, 0};
        uint serial = 0;
    } m_acked, m_pending;
    QSize m_normalSize;
    QMargins m_margins;
    QWaylandExtendedSurface *m_extendedWindow = nullptr;

    void xdg_surface_configure(int32_t width,
                               int32_t height,
                               struct wl_array *states,
                               uint32_t serial) override;
    void xdg_surface_close() override;

    friend class QWaylandWindow;
};

QT_END_NAMESPACE

}

#endif // QWAYLANDXDGSURFACEV5_P_H
