//
// Copyright (c) 2014 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// FramebufferAttachment.h: Defines the wrapper class gl::FramebufferAttachment, as well as the
// objects and related functionality. [OpenGL ES 2.0.24] section 4.4.3 page 108.

#ifndef LIBANGLE_FRAMEBUFFERATTACHMENT_H_
#define LIBANGLE_FRAMEBUFFERATTACHMENT_H_

#include "angle_gl.h"
#include "common/angleutils.h"
#include "libANGLE/angletypes.h"
#include "libANGLE/Error.h"
#include "libANGLE/ImageIndex.h"
#include "libANGLE/signal_utils.h"

namespace egl
{
class Surface;
}

namespace rx
{
// An implementation-specific object associated with an attachment.

class FramebufferAttachmentRenderTarget : angle::NonCopyable
{
  public:
    FramebufferAttachmentRenderTarget() {}
    virtual ~FramebufferAttachmentRenderTarget() {}
};

class FramebufferAttachmentObjectImpl;
}

namespace gl
{
class FramebufferAttachmentObject;
struct Format;
class Renderbuffer;
class Texture;

enum class InitState
{
    MayNeedInit,
    Initialized,
};

using OnAttachmentDirtyBinding  = angle::ChannelBinding<size_t, InitState>;
using OnAttachmentDirtyChannel  = angle::BroadcastChannel<size_t, InitState>;
using OnAttachmentDirtyReceiver = angle::SignalReceiver<size_t, InitState>;

// FramebufferAttachment implements a GL framebuffer attachment.
// Attachments are "light" containers, which store pointers to ref-counted GL objects.
// We support GL texture (2D/3D/Cube/2D array) and renderbuffer object attachments.
// Note: Our old naming scheme used the term "Renderbuffer" for both GL renderbuffers and for
// framebuffer attachments, which confused their usage.

class FramebufferAttachment final
{
  public:
    FramebufferAttachment();

    FramebufferAttachment(const Context *context,
                          GLenum type,
                          GLenum binding,
                          const ImageIndex &textureIndex,
                          FramebufferAttachmentObject *resource);

    FramebufferAttachment(FramebufferAttachment &&other);
    FramebufferAttachment &operator=(FramebufferAttachment &&other);

    ~FramebufferAttachment();

    void detach(const Context *context);
    void attach(const Context *context,
                GLenum type,
                GLenum binding,
                const ImageIndex &textureIndex,
                FramebufferAttachmentObject *resource,
                GLsizei numViews,
                GLuint baseViewIndex,
                GLenum multiviewLayout,
                const GLint *viewportOffsets);

    // Helper methods
    GLuint getRedSize() const;
    GLuint getGreenSize() const;
    GLuint getBlueSize() const;
    GLuint getAlphaSize() const;
    GLuint getDepthSize() const;
    GLuint getStencilSize() const;
    GLenum getComponentType() const;
    GLenum getColorEncoding() const;

    bool isTextureWithId(GLuint textureId) const { return mType == GL_TEXTURE && id() == textureId; }
    bool isRenderbufferWithId(GLuint renderbufferId) const { return mType == GL_RENDERBUFFER && id() == renderbufferId; }

    GLenum getBinding() const { return mTarget.binding(); }
    GLuint id() const;

    // These methods are only legal to call on Texture attachments
    const ImageIndex &getTextureImageIndex() const;
    GLenum cubeMapFace() const;
    GLint mipLevel() const;
    GLint layer() const;
    GLsizei getNumViews() const;
    GLenum getMultiviewLayout() const;
    GLint getBaseViewIndex() const;
    const std::vector<Offset> &getMultiviewViewportOffsets() const;

    // The size of the underlying resource the attachment points to. The 'depth' value will
    // correspond to a 3D texture depth or the layer count of a 2D array texture. For Surfaces and
    // Renderbuffers, it will always be 1.
    Extents getSize() const;
    const Format &getFormat() const;
    GLsizei getSamples() const;
    GLenum type() const { return mType; }
    bool isAttached() const { return mType != GL_NONE; }

    Renderbuffer *getRenderbuffer() const;
    Texture *getTexture() const;
    const egl::Surface *getSurface() const;
    FramebufferAttachmentObject *getResource() const;
    InitState initState() const;
    Error initializeContents(const Context *context);
    void setInitState(InitState initState) const;

    // "T" must be static_castable from FramebufferAttachmentRenderTarget
    template <typename T>
    gl::Error getRenderTarget(const Context *context, T **rtOut) const
    {
        static_assert(std::is_base_of<rx::FramebufferAttachmentRenderTarget, T>(),
                      "Invalid RenderTarget class.");
        return getRenderTargetImpl(
            context, reinterpret_cast<rx::FramebufferAttachmentRenderTarget **>(rtOut));
    }

    bool operator==(const FramebufferAttachment &other) const;
    bool operator!=(const FramebufferAttachment &other) const;

    static std::vector<Offset> GetDefaultViewportOffsetVector();
    static const GLsizei kDefaultNumViews;
    static const GLenum kDefaultMultiviewLayout;
    static const GLint kDefaultBaseViewIndex;
    static const GLint kDefaultViewportOffsets[2];

  private:
    gl::Error getRenderTargetImpl(const Context *context,
                                  rx::FramebufferAttachmentRenderTarget **rtOut) const;

    // A framebuffer attachment points to one of three types of resources: Renderbuffers,
    // Textures and egl::Surface. The "Target" struct indicates which part of the
    // object an attachment references. For the three types:
    //   - a Renderbuffer has a unique renderable target, and needs no target index
    //   - a Texture has targets for every image and uses an ImageIndex
    //   - a Surface has targets for Color and Depth/Stencil, and uses the attachment binding
    class Target
    {
      public:
        Target();
        Target(GLenum binding, const ImageIndex &imageIndex);
        Target(const Target &other);
        Target &operator=(const Target &other);

        GLenum binding() const { return mBinding; }
        const ImageIndex &textureIndex() const { return mTextureIndex; }

      private:
        GLenum mBinding;
        ImageIndex mTextureIndex;
    };

    GLenum mType;
    Target mTarget;
    FramebufferAttachmentObject *mResource;
    GLsizei mNumViews;
    GLenum mMultiviewLayout;
    GLint mBaseViewIndex;
    std::vector<Offset> mViewportOffsets;
};

// A base class for objects that FBO Attachments may point to.
class FramebufferAttachmentObject
{
  public:
    FramebufferAttachmentObject();
    virtual ~FramebufferAttachmentObject();

    virtual Extents getAttachmentSize(const ImageIndex &imageIndex) const = 0;
    virtual const Format &getAttachmentFormat(GLenum binding,
                                              const ImageIndex &imageIndex) const = 0;
    virtual GLsizei getAttachmentSamples(const ImageIndex &imageIndex) const      = 0;

    virtual void onAttach(const Context *context) = 0;
    virtual void onDetach(const Context *context) = 0;
    virtual GLuint getId() const = 0;

    // These are used for robust resource initialization.
    virtual InitState initState(const ImageIndex &imageIndex) const = 0;
    virtual void setInitState(const ImageIndex &imageIndex, InitState initState) = 0;

    Error getAttachmentRenderTarget(const Context *context,
                                    GLenum binding,
                                    const ImageIndex &imageIndex,
                                    rx::FramebufferAttachmentRenderTarget **rtOut) const;

    Error initializeContents(const Context *context, const ImageIndex &imageIndex);

    OnAttachmentDirtyChannel *getDirtyChannel();

  protected:
    virtual rx::FramebufferAttachmentObjectImpl *getAttachmentImpl() const = 0;

    OnAttachmentDirtyChannel mDirtyChannel;
};

inline Extents FramebufferAttachment::getSize() const
{
    ASSERT(mResource);
    return mResource->getAttachmentSize(mTarget.textureIndex());
}

inline const Format &FramebufferAttachment::getFormat() const
{
    ASSERT(mResource);
    return mResource->getAttachmentFormat(mTarget.binding(), mTarget.textureIndex());
}

inline GLsizei FramebufferAttachment::getSamples() const
{
    ASSERT(mResource);
    return mResource->getAttachmentSamples(mTarget.textureIndex());
}

inline gl::Error FramebufferAttachment::getRenderTargetImpl(
    const Context *context,
    rx::FramebufferAttachmentRenderTarget **rtOut) const
{
    ASSERT(mResource);
    return mResource->getAttachmentRenderTarget(context, mTarget.binding(), mTarget.textureIndex(),
                                                rtOut);
}

} // namespace gl

#endif // LIBANGLE_FRAMEBUFFERATTACHMENT_H_
