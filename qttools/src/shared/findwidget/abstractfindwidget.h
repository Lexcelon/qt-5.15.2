/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef ABSTRACTFINDWIDGET_H
#define ABSTRACTFINDWIDGET_H

#include <QtGui/QIcon>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class QAction;
class QCheckBox;
class QEvent;
class QKeyEvent;
class QLabel;
class QLineEdit;
class QObject;
class QToolButton;

class AbstractFindWidget : public QWidget
{
    Q_OBJECT

public:
    enum FindFlag {
        /// Use a layout that is roughly half as wide and twice as high as the regular one.
        NarrowLayout = 1,
        /// Do not show the "Whole words" checkbox.
        NoWholeWords = 2,
        /// Do not show the "Case sensitive" checkbox.
        NoCaseSensitive = 4
    };
    Q_DECLARE_FLAGS(FindFlags, FindFlag)

    explicit AbstractFindWidget(FindFlags flags = FindFlags(), QWidget *parent = 0);
    ~AbstractFindWidget() override;

    bool eventFilter(QObject *object, QEvent *e) override;

    static QIcon findIconSet();
    QAction *createFindAction(QObject *parent);

public slots:
    void activate();
    virtual void deactivate();
    void findNext();
    void findPrevious();
    void findCurrentText();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateButtons();

protected:
    virtual void find(const QString &textToFind, bool skipCurrent, bool backward, bool *found, bool *wrapped) = 0;

    bool caseSensitive() const;
    bool wholeWords() const;

private:
    void findInternal(const QString &textToFind, bool skipCurrent, bool backward);

    QLineEdit *m_editFind;
    QLabel *m_labelWrapped;
    QToolButton *m_toolNext;
    QToolButton *m_toolClose;
    QToolButton *m_toolPrevious;
    QCheckBox *m_checkCase;
    QCheckBox *m_checkWholeWords;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractFindWidget::FindFlags)

QT_END_NAMESPACE

#endif  // ABSTRACTFINDWIDGET_H
