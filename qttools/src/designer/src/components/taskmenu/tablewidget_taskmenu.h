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

#ifndef TABLEWIDGET_TASKMENU_H
#define TABLEWIDGET_TASKMENU_H

#include <qdesigner_taskmenu_p.h>
#include <extensionfactory_p.h>

#include <QtWidgets/qtablewidget.h>
#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

class QLineEdit;
class QDesignerFormWindowInterface;

namespace qdesigner_internal {

class TableWidgetTaskMenu: public QDesignerTaskMenu
{
    Q_OBJECT
public:
    explicit TableWidgetTaskMenu(QTableWidget *button, QObject *parent = nullptr);
    ~TableWidgetTaskMenu() override;

    QAction *preferredEditAction() const override;
    QList<QAction*> taskActions() const override;

private slots:
    void editItems();
    void updateSelection();

private:
    QTableWidget *m_tableWidget;
    QPointer<QDesignerFormWindowInterface> m_formWindow;
    QPointer<QLineEdit> m_editor;
    mutable QList<QAction*> m_taskActions;
    QAction *m_editItemsAction;
};

using TableWidgetTaskMenuFactory = ExtensionFactory<QDesignerTaskMenuExtension, QTableWidget, TableWidgetTaskMenu>;
}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // TABLEWIDGET_TASKMENU_H
