/****************************************************************************
**
** Copyright (C) 2018 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebEngine module of the Qt Toolkit.
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

#include <QtTest/QtTest>

#include "httpserver.h"
#include "../util.h"
#include "qdebug.h"
#include "qwebenginepage.h"
#include "qwebengineprofile.h"
#include "qwebenginesettings.h"
#include "qwebengineview.h"

class tst_LoadSignals : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    void initTestCase();
    void init();

private Q_SLOTS:
    void monotonicity();
    void loadStartedAndFinishedCount_data();
    void loadStartedAndFinishedCount();
    void secondLoadForError_WhenErrorPageEnabled_data();
    void secondLoadForError_WhenErrorPageEnabled();
    void loadAfterInPageNavigation_qtbug66869();
    void fileDownloadDoesNotTriggerLoadSignals_qtbug66661();

private:
    QWebEngineProfile profile;
    QWebEnginePage page{&profile};
    QWebEngineView view;
    QSignalSpy loadStartedSpy{&page, &QWebEnginePage::loadStarted};
    QSignalSpy loadProgressSpy{&page, &QWebEnginePage::loadProgress};
    QSignalSpy loadFinishedSpy{&page, &QWebEnginePage::loadFinished};
};

void tst_LoadSignals::initTestCase()
{
    view.setPage(&page);
    view.resize(1024,768);
    view.show();
}

void tst_LoadSignals::init()
{
    loadStartedSpy.clear();
    loadProgressSpy.clear();
    loadFinishedSpy.clear();
}

/**
  * Test that we get the expected number of loadStarted and loadFinished signals
  */
void tst_LoadSignals::loadStartedAndFinishedCount_data()
{
    QTest::addColumn<QUrl>("url");
    QTest::addColumn<int>("expectedLoadCount");
    QTest::newRow("Normal") << QUrl("qrc:///resources/page1.html") << 1;
    QTest::newRow("WithAnchor") << QUrl("qrc:///resources/page2.html#anchor") << 1;

    // In this case, we get an unexpected additional loadStarted, but no corresponding
    // loadFinished, so expectedLoadCount=2 would also not work. See also QTBUG-65223
    QTest::newRow("WithAnchorClickedFromJS") << QUrl("qrc:///resources/page3.html") << 1;
}

void tst_LoadSignals::loadStartedAndFinishedCount()
{
    QFETCH(QUrl, url);
    QFETCH(int, expectedLoadCount);

    view.load(url);
    QTRY_COMPARE(loadFinishedSpy.size(), expectedLoadCount);
    QVERIFY(loadFinishedSpy[0][0].toBool());

    // Wait for 10 seconds (abort waiting if another loadStarted or loadFinished occurs)
    QTRY_LOOP_IMPL((loadStartedSpy.size() != expectedLoadCount)
                || (loadFinishedSpy.size() != expectedLoadCount), 10000, 100);

    // No further loadStarted should have occurred within this time
    QCOMPARE(loadStartedSpy.size(), expectedLoadCount);
    QCOMPARE(loadFinishedSpy.size(), expectedLoadCount);
}

/**
  * Test monotonicity of loadProgress signals
  */
void tst_LoadSignals::monotonicity()
{
    HttpServer server;
    server.setResourceDirs({ TESTS_SHARED_DATA_DIR });
    connect(&server, &HttpServer::newRequest, [] (HttpReqRep *) {
         QTest::qWait(250); // just add delay to trigger some progress for every sub resource
    });
    QVERIFY(server.start());

    view.load(server.url("/loadprogress/main.html"));
    QTRY_COMPARE(loadFinishedSpy.size(), 1);
    QVERIFY(loadFinishedSpy[0][0].toBool());

    // first loadProgress should have 0% progress
    QCOMPARE(loadProgressSpy.takeFirst()[0].toInt(), 0);

    // every loadProgress should have more progress than the one before
    int progress = 0;
    for (const auto &item : loadProgressSpy) {
        QVERIFY(progress < item[0].toInt());
        progress = item[0].toInt();
    }

    // last loadProgress should have 100% progress
    QCOMPARE(loadProgressSpy.last()[0].toInt(), 100);
}

/**
  * Test that we get a second loadStarted and loadFinished signal
  * for error-pages (unless error-pages are disabled)
  */
void tst_LoadSignals::secondLoadForError_WhenErrorPageEnabled_data()
{
    QTest::addColumn<bool>("enabled");
    // in this case, we get no second loadStarted and loadFinished, although we had
    // agreed on making the navigation to an error page an individual load
    QTest::newRow("ErrorPageEnabled") << true;
    QTest::newRow("ErrorPageDisabled") << false;
}

void tst_LoadSignals::secondLoadForError_WhenErrorPageEnabled()
{
    QFETCH(bool, enabled);
    view.settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, enabled);
    int expectedLoadCount = (enabled ? 2 : 1);

    // RFC 2606 guarantees that this will never become a valid domain
    view.load(QUrl("http://nonexistent.invalid"));
    QTRY_COMPARE_WITH_TIMEOUT(loadFinishedSpy.size(), expectedLoadCount, 10000);
    QVERIFY(!loadFinishedSpy[0][0].toBool());
    if (enabled)
        QVERIFY(loadFinishedSpy[1][0].toBool());

    // Wait for 10 seconds (abort waiting if another loadStarted or loadFinished occurs)
    QTRY_LOOP_IMPL((loadStartedSpy.size() != expectedLoadCount)
                || (loadFinishedSpy.size() != expectedLoadCount), 10000, 100);

    // No further loadStarted should have occurred within this time
    QCOMPARE(loadStartedSpy.size(), expectedLoadCount);
    QCOMPARE(loadFinishedSpy.size(), expectedLoadCount);
}

/**
  * Test that a second load after an in-page navigation receives its expected loadStarted and
  * loadFinished signal.
  */
void tst_LoadSignals::loadAfterInPageNavigation_qtbug66869()
{
    view.load(QUrl("qrc:///resources/page3.html"));
    QTRY_COMPARE(loadFinishedSpy.size(), 1);
    QVERIFY(loadFinishedSpy[0][0].toBool());

    // page3 does an in-page navigation after 500ms
    QTest::qWait(2000);
    loadFinishedSpy.clear();
    loadProgressSpy.clear();
    loadStartedSpy.clear();

    // second load
    view.load(QUrl("qrc:///resources/page1.html"));
    QTRY_COMPARE(loadFinishedSpy.size(), 1);
    QVERIFY(loadFinishedSpy[0][0].toBool());
    // loadStarted and loadFinished should have been signalled
    QCOMPARE(loadStartedSpy.size(), 1);

    // reminder that we still need to solve the core issue
    QFAIL("https://codereview.qt-project.org/#/c/222112/ only hides the symptom, the core issue still needs to be solved");
}

/**
  * Test that file-downloads don't trigger loadStarted or loadFinished signals.
  * See QTBUG-66661
  */
void tst_LoadSignals::fileDownloadDoesNotTriggerLoadSignals_qtbug66661()
{
    view.load(QUrl("qrc:///resources/page4.html"));
    QTRY_COMPARE(loadFinishedSpy.size(), 1);
    QVERIFY(loadFinishedSpy[0][0].toBool());

    // allow the download
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    QWebEngineDownloadItem::DownloadState downloadState = QWebEngineDownloadItem::DownloadRequested;
    ScopedConnection sc1 =
            connect(&profile, &QWebEngineProfile::downloadRequested,
                    [&downloadState, &tempDir](QWebEngineDownloadItem *item) {
                        connect(item, &QWebEngineDownloadItem::stateChanged,
                                [&downloadState](QWebEngineDownloadItem::DownloadState newState) {
                                    downloadState = newState;
                                });
                        item->setDownloadDirectory(tempDir.path());
                        item->accept();
                    });

    // trigger the download link that becomes focused on page4
    QTest::qWait(1000);
    QTest::sendKeyEvent(QTest::Press, view.focusProxy(), Qt::Key_Return, QString("\r"), Qt::NoModifier);
    QTest::sendKeyEvent(QTest::Release, view.focusProxy(), Qt::Key_Return, QString("\r"), Qt::NoModifier);

    // Wait for 10 seconds (abort waiting if another loadStarted or loadFinished occurs)
    QTRY_LOOP_IMPL((loadStartedSpy.size() != 1)
                || (loadFinishedSpy.size() != 1), 10000, 100);

    // Download must have occurred
    QTRY_COMPARE(downloadState, QWebEngineDownloadItem::DownloadCompleted);

    // No further loadStarted should have occurred within this time
    QCOMPARE(loadStartedSpy.size(), 1);
    QCOMPARE(loadFinishedSpy.size(), 1);
}

QTEST_MAIN(tst_LoadSignals)
#include "tst_loadsignals.moc"
