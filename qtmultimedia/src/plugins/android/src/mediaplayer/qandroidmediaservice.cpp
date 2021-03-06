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

#include "qandroidmediaservice.h"

#include "qandroidmediaplayercontrol.h"
#include "qandroidmetadatareadercontrol.h"
#include "qandroidaudiorolecontrol.h"
#include "qandroidcustomaudiorolecontrol.h"
#include "qandroidmediaplayervideorenderercontrol.h"

QT_BEGIN_NAMESPACE

QAndroidMediaService::QAndroidMediaService(QObject *parent)
    : QMediaService(parent)
    , mAudioRoleControl(nullptr)
    , mCustomAudioRoleControl(nullptr)
    , mVideoRendererControl(0)
{
    mMediaControl = new QAndroidMediaPlayerControl;
    mMetadataControl = new QAndroidMetaDataReaderControl;
    mAudioRoleControl = new QAndroidAudioRoleControl;
    mCustomAudioRoleControl = new QAndroidCustomAudioRoleControl;
    connect(mAudioRoleControl, &QAndroidAudioRoleControl::audioRoleChanged,
            mMediaControl, &QAndroidMediaPlayerControl::setAudioRole);
    connect(mCustomAudioRoleControl, &QAndroidCustomAudioRoleControl::customAudioRoleChanged,
            mMediaControl, &QAndroidMediaPlayerControl::setCustomAudioRole);
    connect(mMediaControl, SIGNAL(mediaChanged(QMediaContent)),
            mMetadataControl, SLOT(onMediaChanged(QMediaContent)));
    connect(mMediaControl, SIGNAL(metaDataUpdated()),
            mMetadataControl, SLOT(onUpdateMetaData()));
}

QAndroidMediaService::~QAndroidMediaService()
{
    delete mVideoRendererControl;
    delete mCustomAudioRoleControl;
    delete mAudioRoleControl;
    delete mMetadataControl;
    delete mMediaControl;
}

QMediaControl *QAndroidMediaService::requestControl(const char *name)
{
    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        return mMediaControl;

    if (qstrcmp(name, QMetaDataReaderControl_iid) == 0)
        return mMetadataControl;

    if (qstrcmp(name, QAudioRoleControl_iid) == 0)
        return mAudioRoleControl;

    if (qstrcmp(name, QCustomAudioRoleControl_iid) == 0)
        return mCustomAudioRoleControl;

    if (qstrcmp(name, QVideoRendererControl_iid) == 0) {
        if (!mVideoRendererControl) {
            mVideoRendererControl = new QAndroidMediaPlayerVideoRendererControl(mMediaControl);
            return mVideoRendererControl;
        }
    }

    return 0;
}

void QAndroidMediaService::releaseControl(QMediaControl *control)
{
    if (control == mVideoRendererControl) {
        delete mVideoRendererControl;
        mVideoRendererControl = 0;
    }
}

QT_END_NAMESPACE
