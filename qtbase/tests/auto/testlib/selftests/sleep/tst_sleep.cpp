/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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


#include <QtCore/QCoreApplication>
#include <QtCore/QElapsedTimer>
#include <QtTest/QtTest>

class tst_Sleep: public QObject
{
    Q_OBJECT

private slots:
    void sleep();
    void wait();
};

void tst_Sleep::sleep()
{
    QElapsedTimer t;
    t.start();

    QTest::qSleep(100);
    QVERIFY(t.elapsed() > 90);

    QTest::qSleep(1000);
    QVERIFY(t.elapsed() > 1000);

    QTest::qSleep(1000 * 10); // 10 seconds
    QVERIFY(t.elapsed() > 1000 * 10);
}

void tst_Sleep::wait()
{
    QElapsedTimer t;
    t.start();

    QTest::qWait(1);
    QVERIFY(t.elapsed() >= 1);

    QTest::qWait(10);
    QVERIFY(t.elapsed() >= 11);

    QTest::qWait(100);
    QVERIFY(t.elapsed() >= 111);

    QTest::qWait(1000);
    QVERIFY(t.elapsed() >= 1111);
}

QTEST_MAIN(tst_Sleep)

#include "tst_sleep.moc"
