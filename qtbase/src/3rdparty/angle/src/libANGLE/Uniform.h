//
// Copyright (c) 2010-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef LIBANGLE_UNIFORM_H_
#define LIBANGLE_UNIFORM_H_

#include <string>
#include <vector>

#include "angle_gl.h"
#include "common/debug.h"
#include "common/MemoryBuffer.h"
#include "compiler/translator/blocklayout.h"
#include "libANGLE/angletypes.h"

namespace gl
{
struct UniformTypeInfo;

struct StaticallyUsed
{
    StaticallyUsed();
    StaticallyUsed(const StaticallyUsed &rhs);
    virtual ~StaticallyUsed();

    StaticallyUsed &operator=(const StaticallyUsed &rhs);

    void setStaticUse(GLenum shaderType, bool used);
    void unionReferencesWith(const StaticallyUsed &other);

    bool vertexStaticUse;
    bool fragmentStaticUse;
    bool computeStaticUse;
};

// Helper struct representing a single shader uniform
struct LinkedUniform : public sh::Uniform, public StaticallyUsed
{
    LinkedUniform();
    LinkedUniform(GLenum type,
                  GLenum precision,
                  const std::string &name,
                  const std::vector<unsigned int> &arraySizes,
                  const int binding,
                  const int offset,
                  const int location,
                  const int bufferIndex,
                  const sh::BlockMemberInfo &blockInfo);
    LinkedUniform(const sh::Uniform &uniform);
    LinkedUniform(const LinkedUniform &uniform);
    LinkedUniform &operator=(const LinkedUniform &uniform);
    ~LinkedUniform() override;

    bool isSampler() const;
    bool isImage() const;
    bool isAtomicCounter() const;
    bool isInDefaultBlock() const;
    bool isField() const;
    size_t getElementSize() const;
    size_t getElementComponents() const;

    const UniformTypeInfo *typeInfo;

    // Identifies the containing buffer backed resource -- interface block or atomic counter buffer.
    int bufferIndex;
    sh::BlockMemberInfo blockInfo;
};

struct BufferVariable : public sh::ShaderVariable, public StaticallyUsed
{
    BufferVariable();
    BufferVariable(GLenum type,
                   GLenum precision,
                   const std::string &name,
                   const std::vector<unsigned int> &arraySizes,
                   const int bufferIndex,
                   const sh::BlockMemberInfo &blockInfo);
    ~BufferVariable() override;

    int bufferIndex;
    sh::BlockMemberInfo blockInfo;

    int topLevelArraySize;
};

// Parent struct for atomic counter, uniform block, and shader storage block buffer, which all
// contain a group of shader variables, and have a GL buffer backed.
struct ShaderVariableBuffer : public StaticallyUsed
{
    ShaderVariableBuffer();
    ShaderVariableBuffer(const ShaderVariableBuffer &other);
    ~ShaderVariableBuffer() override;
    int numActiveVariables() const;

    int binding;
    unsigned int dataSize;
    std::vector<unsigned int> memberIndexes;
};

using AtomicCounterBuffer = ShaderVariableBuffer;

// Helper struct representing a single shader interface block
struct InterfaceBlock : public ShaderVariableBuffer
{
    InterfaceBlock();
    InterfaceBlock(const std::string &nameIn,
                   const std::string &mappedNameIn,
                   bool isArrayIn,
                   unsigned int arrayElementIn,
                   int bindingIn);

    std::string nameWithArrayIndex() const;
    std::string mappedNameWithArrayIndex() const;

    std::string name;
    std::string mappedName;
    bool isArray;
    unsigned int arrayElement;
};

}

#endif   // LIBANGLE_UNIFORM_H_
