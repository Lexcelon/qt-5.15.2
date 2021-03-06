/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include "qcommonsequencetypes_p.h"
#include "qstaticbaseuricontext_p.h"

#include "qstaticbaseuristore_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

StaticBaseURIStore::StaticBaseURIStore(const QUrl &baseURI,
                                       const Expression::Ptr &operand) : SingleContainer(operand)
                                                                       , m_baseURI(baseURI)
{
}

Expression::Ptr StaticBaseURIStore::typeCheck(const StaticContext::Ptr &context,
                                              const SequenceType::Ptr &reqType)
{
    const StaticContext::Ptr newContext(new StaticBaseURIContext(context->baseURI().resolved(m_baseURI),
                                                                 context));
    return m_operand->typeCheck(newContext, reqType);
}

SequenceType::Ptr StaticBaseURIStore::staticType() const
{
    return m_operand->staticType();
}

SequenceType::List StaticBaseURIStore::expectedOperandTypes() const
{
    SequenceType::List ops;
    ops.append(CommonSequenceTypes::ZeroOrMoreItems);
    return ops;
}

ExpressionVisitorResult::Ptr StaticBaseURIStore::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
