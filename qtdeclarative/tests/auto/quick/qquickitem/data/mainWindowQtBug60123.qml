/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
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
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

Rectangle {
    width: 400
    height: 200
    color: "white"
    property string lastEvent: ""

    Rectangle {
        id: buttonRect
        width: 50
        height: 50
        color: "black"

        MouseArea {
            anchors.fill: parent
            onPressed: {
                lastEvent = "pressed"
                buttonRect.color = "yellow"
            }
            onReleased: {
                lastEvent = "released"
                buttonRect.color = "black"
            }
            onCanceled: {
                lastEvent = "canceled"
                buttonRect.color = "green"
            }
        }
    }
}
