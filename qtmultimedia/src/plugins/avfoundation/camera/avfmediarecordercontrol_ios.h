/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Toolkit.
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

#ifndef AVFMEDIARECORDERCONTROL_IOS_H
#define AVFMEDIARECORDERCONTROL_IOS_H

#include "avfmediaassetwriter.h"
#include "avfstoragelocation.h"
#include "avfcamerautility.h"

#include <QtMultimedia/qmediarecordercontrol.h>
#include <private/qvideooutputorientationhandler_p.h>

#include <QtCore/qglobal.h>
#include <QtCore/qurl.h>

#include <AVFoundation/AVFoundation.h>

QT_BEGIN_NAMESPACE

class AVFCameraService;
class QString;
class QUrl;

class AVFMediaRecorderControlIOS : public QMediaRecorderControl
{
    Q_OBJECT
public:
    AVFMediaRecorderControlIOS(AVFCameraService *service, QObject *parent = nullptr);
    ~AVFMediaRecorderControlIOS() override;

    QUrl outputLocation() const override;
    bool setOutputLocation(const QUrl &location) override;

    QMediaRecorder::State state() const override;
    QMediaRecorder::Status status() const override;

    qint64 duration() const override;

    bool isMuted() const override;
    qreal volume() const override;

    void applySettings() override;
    void unapplySettings();

public Q_SLOTS:
    void setState(QMediaRecorder::State state) override;
    void setMuted(bool muted) override;
    void setVolume(qreal volume) override;

private:

    Q_INVOKABLE void assetWriterStarted();
    Q_INVOKABLE void assetWriterFinished();

private Q_SLOTS:
    void captureModeChanged(QCamera::CaptureModes);
    void cameraStatusChanged(QCamera::Status newStatus);

private:
    void stopWriter();

    AVFCameraService *m_service;
    AVFScopedPointer<QT_MANGLE_NAMESPACE(AVFMediaAssetWriter)> m_writer;

    QUrl m_outputLocation;
    AVFStorageLocation m_storageLocation;

    QMediaRecorder::State m_state;
    QMediaRecorder::Status m_lastStatus;

    NSDictionary *m_audioSettings;
    NSDictionary *m_videoSettings;
    QVideoOutputOrientationHandler m_orientationHandler;
};

QT_END_NAMESPACE

#endif // AVFMEDIARECORDERCONTROL_IOS_H
