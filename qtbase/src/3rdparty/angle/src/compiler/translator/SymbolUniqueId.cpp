//
// Copyright (c) 2017 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// SymbolUniqueId.cpp: Encapsulates a unique id for a symbol.

#include "compiler/translator/SymbolUniqueId.h"

#include "compiler/translator/SymbolTable.h"

namespace sh
{

TSymbolUniqueId::TSymbolUniqueId(TSymbolTable *symbolTable) : mId(symbolTable->nextUniqueIdValue())
{
}

TSymbolUniqueId::TSymbolUniqueId(const TSymbol &symbol) : mId(symbol.getUniqueId().get())
{
}

int TSymbolUniqueId::get() const
{
    return mId;
}

}  // namespace sh
