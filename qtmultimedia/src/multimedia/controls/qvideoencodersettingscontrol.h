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

#ifndef QVIDEOENCODERSETTINGSCONTROL_H
#define QVIDEOENCODERSETTINGSCONTROL_H

#include <QtMultimedia/qmediacontrol.h>
#include <QtMultimedia/qmediarecorder.h>

#include <QtCore/qpair.h>
#include <QtCore/qsize.h>

QT_BEGIN_NAMESPACE

class QByteArray;
class QStringList;
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

// Required for QDoc workaround
class QString;

class Q_MULTIMEDIA_EXPORT QVideoEncoderSettingsControl : public QMediaControl
{
    Q_OBJECT

public:
    virtual ~QVideoEncoderSettingsControl();

    virtual QList<QSize> supportedResolutions(const QVideoEncoderSettings &settings,
                                              bool *continuous = nullptr) const = 0;

    virtual QList<qreal> supportedFrameRates(const QVideoEncoderSettings &settings,
                                             bool *continuous = nullptr) const = 0;

    virtual QStringList supportedVideoCodecs() const = 0;
    virtual QString videoCodecDescription(const QString &codec) const = 0;

    virtual QVideoEncoderSettings videoSettings() const = 0;
    virtual void setVideoSettings(const QVideoEncoderSettings &settings) = 0;

protected:
    explicit QVideoEncoderSettingsControl(QObject *parent = nullptr);
};

#define QVideoEncoderSettingsControl_iid "org.qt-project.qt.videoencodersettingscontrol/5.0"
Q_MEDIA_DECLARE_CONTROL(QVideoEncoderSettingsControl, QVideoEncoderSettingsControl_iid)

QT_END_NAMESPACE


#endif
