/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef DPICHOOSER_H
#define DPICHOOSER_H

#include <QtWidgets/qwidget.h>

QT_BEGIN_NAMESPACE

class QSpinBox;
class QComboBox;

namespace qdesigner_internal {

struct DPI_Entry;

/* Let the user choose a DPI settings */
class DPI_Chooser : public QWidget {
    Q_DISABLE_COPY_MOVE(DPI_Chooser)
    Q_OBJECT

public:
    explicit DPI_Chooser(QWidget *parent = nullptr);
    ~DPI_Chooser();

    void getDPI(int *dpiX, int *dpiY) const;
    void setDPI(int dpiX, int dpiY);

private slots:
    void syncSpinBoxes();

private:
    void setUserDefinedValues(int dpiX, int dpiY);

    struct DPI_Entry *m_systemEntry;
    QComboBox *m_predefinedCombo;
    QSpinBox *m_dpiXSpinBox;
    QSpinBox *m_dpiYSpinBox;
};
}

QT_END_NAMESPACE

#endif // DPICHOOSER_H
