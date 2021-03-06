/****************************************************************************
**
** Copyright (C) 2019 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

//![file]
import QtQuick 2.12
import QtQuick.Window 2.12
import Qt.labs.qmlmodels 1.0

Window {
    width: 400
    height: 400
    visible: true

    TableView {
        anchors.fill: parent
        columnSpacing: 1
        rowSpacing: 1
        boundsBehavior: Flickable.StopAtBounds

        model: TableModel {
            TableModelColumn {
                display: function(modelIndex) { return rows[modelIndex.row][0].checked }
                setDisplay: function(modelIndex, cellData) { rows[modelIndex.row][0].checked = cellData }
            }
            TableModelColumn {
                display: function(modelIndex) { return rows[modelIndex.row][1].amount }
                setDisplay: function(modelIndex, cellData) { rows[modelIndex.row][1].amount = cellData }
            }
            TableModelColumn {
                display: function(modelIndex) { return rows[modelIndex.row][2].fruitType }
                setDisplay: function(modelIndex, cellData) { rows[modelIndex.row][2].fruitType = cellData }
            }
            TableModelColumn {
                display: function(modelIndex) { return rows[modelIndex.row][3].fruitName }
                setDisplay: function(modelIndex, cellData) { rows[modelIndex.row][3].fruitName = cellData }
            }
            TableModelColumn {
                display: function(modelIndex) { return rows[modelIndex.row][4].fruitPrice }
                setDisplay: function(modelIndex, cellData) { rows[modelIndex.row][4].fruitPrice = cellData }
            }

            // Each row is one type of fruit that can be ordered
//![rows]
            rows: [
                [
                    // Each object (line) is one cell/column.
                    { checked: false, checkable: true },
                    { amount: 1 },
                    { fruitType: "Apple" },
                    { fruitName: "Granny Smith" },
                    { fruitPrice: 1.50 }
                ],
                [
                    { checked: true, checkable: true },
                    { amount: 4 },
                    { fruitType: "Orange" },
                    { fruitName: "Navel" },
                    { fruitPrice: 2.50 }
                ],
                [
                    { checked: false, checkable: false },
                    { amount: 1 },
                    { fruitType: "Banana" },
                    { fruitName: "Cavendish" },
                    { fruitPrice: 3.50 }
                ]
            ]
//![rows]
        }
//![delegate]
        delegate:  TextInput {
            text: model.display
            padding: 12
            selectByMouse: true

            onAccepted: model.display = text

            Rectangle {
                anchors.fill: parent
                color: "#efefef"
                z: -1
            }
        }
//![delegate]
    }
}
//![file]
