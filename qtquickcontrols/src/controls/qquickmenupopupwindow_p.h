/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls module of the Qt Toolkit.
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

#ifndef QQUICKMENUPOPUPWINDOW_H
#define QQUICKMENUPOPUPWINDOW_H

#include "qquickpopupwindow_p.h"

QT_BEGIN_NAMESPACE

class QQuickMenu1;
class QQuickMenuBar1;

class QQuickMenuPopupWindow1 : public QQuickPopupWindow1
{
    Q_OBJECT
public:
    QQuickMenuPopupWindow1(QQuickMenu1 *menu);

    void setItemAt(QQuickItem *menuItem);
    void setParentWindow(QWindow *effectiveParentWindow, QQuickWindow *parentWindow);
    void setGeometry(int posx, int posy, int w, int h);

    void setParentItem(QQuickItem *) override;

    QQuickMenu1 *menu() const;
public Q_SLOTS:
    void setToBeDeletedLater();

protected Q_SLOTS:
    void updateSize();
    void updatePosition();

Q_SIGNALS:
    void willBeDeletedLater();

protected:
    void focusInEvent(QFocusEvent *) override;
    void exposeEvent(QExposeEvent *) override;
    bool shouldForwardEventAfterDismiss(QMouseEvent *) const override;

private:
    QQuickItem *m_itemAt;
    QPointF m_oldItemPos;
    QPointF m_initialPos;
    QPointer<QQuickWindow> m_logicalParentWindow;
    QQuickMenu1 *m_menu;
};

QT_END_NAMESPACE

#endif // QQUICKMENUPOPUPWINDOW_H
