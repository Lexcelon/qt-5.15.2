/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
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

#ifndef QT3DRENDER_QATTRIBUTE_P_H
#define QT3DRENDER_QATTRIBUTE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <private/qnode_p.h>
#include <private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAttribute;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QAttributePrivate : public Qt3DCore::QNodePrivate
{
public:
    Q_DECLARE_PUBLIC(QAttribute)

    QAttributePrivate();

    QBuffer *m_buffer;
    QString m_name;
    QAttribute::VertexBaseType m_vertexBaseType;
    uint m_vertexSize;
    uint m_count;
    uint m_byteStride;
    uint m_byteOffset;
    uint m_divisor;
    QAttribute::AttributeType m_attributeType;
};

struct QAttributeData
{
    Qt3DCore::QNodeId bufferId;
    QString name;
    QAttribute::VertexBaseType vertexBaseType;
    uint vertexSize;
    uint count;
    uint byteStride;
    uint byteOffset;
    uint divisor;
    QAttribute::AttributeType attributeType;
};

} // Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QATTRIBUTE_P_H
