/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QT3DRENDER_QSHADERNODE_P_H
#define QT3DRENDER_QSHADERNODE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DRender/private/qt3drender_global_p.h>

#include <Qt3DRender/private/qshaderformat_p.h>
#include <Qt3DRender/private/qshadernodeport_p.h>

#include <QtCore/quuid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender
{
class QShaderNode
{
public:
    enum Type : char {
        Invalid,
        Input,
        Output,
        Function
    };

    class Rule
    {
    public:
        Q_3DRENDERSHARED_PRIVATE_EXPORT Rule(const QByteArray &substitution = QByteArray(), const QByteArrayList &headerSnippets = QByteArrayList()) noexcept;

        QByteArray substitution;
        QByteArrayList headerSnippets;
    };

    Q_3DRENDERSHARED_PRIVATE_EXPORT Type type() const noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QUuid uuid() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setUuid(const QUuid &uuid) noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QStringList layers() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setLayers(const QStringList &layers) noexcept;

    Q_3DRENDERSHARED_PRIVATE_EXPORT QVector<QShaderNodePort> ports() const noexcept;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void addPort(const QShaderNodePort &port);
    Q_3DRENDERSHARED_PRIVATE_EXPORT void removePort(const QShaderNodePort &port);

    Q_3DRENDERSHARED_PRIVATE_EXPORT QStringList parameterNames() const;
    Q_3DRENDERSHARED_PRIVATE_EXPORT QVariant parameter(const QString &name) const;
    Q_3DRENDERSHARED_PRIVATE_EXPORT void setParameter(const QString &name, const QVariant &value);
    Q_3DRENDERSHARED_PRIVATE_EXPORT void clearParameter(const QString &name);

    Q_3DRENDERSHARED_PRIVATE_EXPORT void addRule(const QShaderFormat &format, const Rule &rule);
    Q_3DRENDERSHARED_PRIVATE_EXPORT void removeRule(const QShaderFormat &format);

    Q_3DRENDERSHARED_PRIVATE_EXPORT QVector<QShaderFormat> availableFormats() const;
    Q_3DRENDERSHARED_PRIVATE_EXPORT Rule rule(const QShaderFormat &format) const;

private:
    QUuid m_uuid;
    QStringList m_layers;
    QVector<QShaderNodePort> m_ports;
    QHash<QString, QVariant> m_parameters;
    QVector<QPair<QShaderFormat, QShaderNode::Rule>> m_rules;
};

Q_3DRENDERSHARED_PRIVATE_EXPORT bool operator==(const QShaderNode::Rule &lhs, const QShaderNode::Rule &rhs) noexcept;

inline bool operator!=(const QShaderNode::Rule &lhs, const QShaderNode::Rule &rhs) noexcept
{
    return !(lhs == rhs);
}

}

Q_DECLARE_TYPEINFO(Qt3DRender::QShaderNode, Q_MOVABLE_TYPE);
Q_DECLARE_TYPEINFO(Qt3DRender::QShaderNode::Rule, Q_MOVABLE_TYPE);
QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QShaderNode)
Q_DECLARE_METATYPE(Qt3DRender::QShaderNode::Rule)

#endif // QT3DRENDER_QSHADERNODE_P_H
