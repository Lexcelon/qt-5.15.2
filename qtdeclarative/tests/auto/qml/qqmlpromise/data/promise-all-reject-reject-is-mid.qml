/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQml module of the Qt Toolkit.
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
import QtQuick 2.4

QtObject {
    // TODO!

    property int resolveValue: 5
    property int rejectValue: 10
    property int resultValue: rejectValue
    property bool wasTestSuccessful: false


    property var delayedEvent: Timer {
        interval: 0
        property var handler: null
        onTriggered: {
            if (handler) {
                handler();
            }
        }
    }

    function postEvent(event, value) {
        delayedEvent.handler = function() { event(value) }
        delayedEvent.restart();
    }

    property var promise1: Promise.resolve(resolveValue);
    property int promise2: resolveValue
    property var promise3: Promise.reject(rejectValue)
    property var promise4: new Promise(function (resolve, reject) {
        postEvent(resolve, resolveValue)
    })

    Component.onCompleted: {
        Promise.all([promise1, promise2, promise3, promise4]).then(function() {
            throw new Error("Should never be called")
        }, function(value) {
            if (value !== rejectValue) {
                return;
            }

            wasTestSuccessful = true
        })
    }
}
