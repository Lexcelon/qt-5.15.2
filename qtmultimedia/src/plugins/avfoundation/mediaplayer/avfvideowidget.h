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

#ifndef AVFVIDEOWIDGET_H
#define AVFVIDEOWIDGET_H

#include <QtWidgets/QWidget>

@class AVPlayerLayer;
#if defined(Q_OS_OSX)
@class NSView;
#else
@class UIView;
#endif

QT_BEGIN_NAMESPACE

class AVFVideoWidget : public QWidget
{
public:
    AVFVideoWidget(QWidget *parent);
    virtual ~AVFVideoWidget();

    QSize sizeHint() const override;
    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode mode);
    void setPlayerLayer(AVPlayerLayer *layer);

protected:
    void resizeEvent(QResizeEvent *) override;
    void paintEvent(QPaintEvent *) override;

private:
    void updateAspectRatio();
    void updatePlayerLayerBounds(const QSize &size);

    QSize m_nativeSize;
    Qt::AspectRatioMode m_aspectRatioMode;
    AVPlayerLayer *m_playerLayer;
#if defined(Q_OS_OSX)
    NSView *m_nativeView;
#else
    UIView *m_nativeView;
#endif
};

QT_END_NAMESPACE

#endif // AVFVIDEOWIDGET_H
