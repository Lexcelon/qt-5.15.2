/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWaylandCompositor module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QWAYLANDTOUCH_H
#define QWAYLANDTOUCH_H

#include <QtWaylandCompositor/QWaylandCompositorExtension>

#include <QtCore/QObject>
#include <QtGui/QTouchEvent>

struct wl_resource;

QT_BEGIN_NAMESPACE

class QWaylandTouch;
class QWaylandTouchPrivate;
class QWaylandSeat;
class QWaylandView;
class QWaylandClient;
class QWaylandSurface;

class Q_WAYLAND_COMPOSITOR_EXPORT QWaylandTouch : public QWaylandObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QWaylandTouch)
public:
    QWaylandTouch(QWaylandSeat *seat, QObject *parent = nullptr);

    QWaylandSeat *seat() const;
    QWaylandCompositor *compositor() const;

    virtual uint sendTouchPointEvent(QWaylandSurface *surface, int id, const QPointF &position, Qt::TouchPointState state);
    virtual void sendFrameEvent(QWaylandClient *client);
    virtual void sendCancelEvent(QWaylandClient *client);
    virtual void sendFullTouchEvent(QWaylandSurface *surface, QTouchEvent *event);

    virtual void addClient(QWaylandClient *client, uint32_t id, uint32_t version);
};

QT_END_NAMESPACE

#endif  /*QWAYLANDTOUCH_H*/
