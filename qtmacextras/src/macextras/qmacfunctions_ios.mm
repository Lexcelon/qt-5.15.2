/****************************************************************************
**
** Copyright (C) 2016 Petroules Corporation.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtMacExtras module of the Qt Toolkit.
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

#include "qmacfunctions.h"
#include "qmacfunctions_p.h"

#import <UIKit/UIKit.h>

QT_BEGIN_NAMESPACE

namespace QtMac
{

#if QT_DEPRECATED_SINCE(5, 12)
CGContextRef currentCGContext()
{
    return UIGraphicsGetCurrentContext();
}

/*!
    \fn void setApplicationIconBadgeNumber(int number)
    \obsolete Use \c {UIApplication.sharedApplication.applicationIconBadgeNumber} instead.

    Sets the value shown on the application icon a.k.a badge to \a number.

    Unlike its \macos counterpart, only numbers can be used.

    \sa applicationIconBadgeNumber(), setBadgeLabelText()
*/
void setApplicationIconBadgeNumber(int number)
{
    [[UIApplication sharedApplication] setApplicationIconBadgeNumber:number];
}

/*!
    \fn int applicationIconBadgeNumber()
    \obsolete Use \c {UIApplication.sharedApplication.applicationIconBadgeNumber} instead.

    Returns the value of the application icon a.k.a badge.

    \sa setApplicationIconBadgeNumber(), badgeLabelText()
*/
int applicationIconBadgeNumber()
{
    return [[UIApplication sharedApplication] applicationIconBadgeNumber];
}
#endif

} // namespace QtMac

QT_END_NAMESPACE
