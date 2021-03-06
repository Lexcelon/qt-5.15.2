//
// Copyright (c) 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef COMPILER_TRANSLATOR_REMOVEINVARIANTDECLARATION_H_
#define COMPILER_TRANSLATOR_REMOVEINVARIANTDECLARATION_H_

class TIntermNode;
namespace sh
{

void RemoveInvariantDeclaration(TIntermNode *root);

}  // namespace sh

#endif  // COMPILER_TRANSLATOR_REMOVEINVARIANTDECLARATION_H_
