//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// Image9.cpp: Implements the rx::Image9 class, which acts as the interface to
// the actual underlying surfaces of a Texture.

#include "libANGLE/renderer/d3d/d3d9/Image9.h"
#include "libANGLE/renderer/d3d/d3d9/renderer9_utils.h"
#include "libANGLE/renderer/d3d/d3d9/formatutils9.h"
#include "libANGLE/renderer/d3d/d3d9/Renderer9.h"
#include "libANGLE/renderer/d3d/d3d9/RenderTarget9.h"
#include "libANGLE/renderer/d3d/d3d9/TextureStorage9.h"
#include "libANGLE/formatutils.h"
#include "libANGLE/Framebuffer.h"
#include "libANGLE/FramebufferAttachment.h"
#include "libANGLE/Renderbuffer.h"
#include "common/utilities.h"

namespace rx
{

Image9::Image9(Renderer9 *renderer)
{
    mSurface  = nullptr;
    mRenderer = nullptr;

    mD3DPool = D3DPOOL_SYSTEMMEM;
    mD3DFormat = D3DFMT_UNKNOWN;

    mRenderer = renderer;
}

Image9::~Image9()
{
    SafeRelease(mSurface);
}

gl::Error Image9::generateMip(IDirect3DSurface9 *destSurface, IDirect3DSurface9 *sourceSurface)
{
    D3DSURFACE_DESC destDesc;
    HRESULT result = destSurface->GetDesc(&destDesc);
    ASSERT(SUCCEEDED(result));
    if (FAILED(result))
    {
        return gl::OutOfMemory()
               << "Failed to query the source surface description for mipmap generation, "
               << gl::FmtHR(result);
    }

    D3DSURFACE_DESC sourceDesc;
    result = sourceSurface->GetDesc(&sourceDesc);
    ASSERT(SUCCEEDED(result));
    if (FAILED(result))
    {
        return gl::OutOfMemory()
               << "Failed to query the destination surface description for mipmap generation, "
               << gl::FmtHR(result);
    }

    ASSERT(sourceDesc.Format == destDesc.Format);
    ASSERT(sourceDesc.Width == 1 || sourceDesc.Width / 2 == destDesc.Width);
    ASSERT(sourceDesc.Height == 1 || sourceDesc.Height / 2 == destDesc.Height);

    const d3d9::D3DFormat &d3dFormatInfo = d3d9::GetD3DFormatInfo(sourceDesc.Format);
    ASSERT(d3dFormatInfo.info().mipGenerationFunction != nullptr);

    D3DLOCKED_RECT sourceLocked = {0};
    result                      = sourceSurface->LockRect(&sourceLocked, nullptr, D3DLOCK_READONLY);
    ASSERT(SUCCEEDED(result));
    if (FAILED(result))
    {
        return gl::OutOfMemory() << "Failed to lock the source surface for mipmap generation, "
                                 << gl::FmtHR(result);
    }

    D3DLOCKED_RECT destLocked = {0};
    result                    = destSurface->LockRect(&destLocked, nullptr, 0);
    ASSERT(SUCCEEDED(result));
    if (FAILED(result))
    {
        sourceSurface->UnlockRect();
        return gl::OutOfMemory() << "Failed to lock the destination surface for mipmap generation, "
                                 << gl::FmtHR(result);
    }

    const uint8_t *sourceData = reinterpret_cast<const uint8_t*>(sourceLocked.pBits);
    uint8_t *destData = reinterpret_cast<uint8_t*>(destLocked.pBits);

    ASSERT(sourceData && destData);

    d3dFormatInfo.info().mipGenerationFunction(sourceDesc.Width, sourceDesc.Height, 1, sourceData,
                                               sourceLocked.Pitch, 0, destData, destLocked.Pitch,
                                               0);

    destSurface->UnlockRect();
    sourceSurface->UnlockRect();

    return gl::NoError();
}

gl::Error Image9::generateMipmap(Image9 *dest, Image9 *source)
{
    IDirect3DSurface9 *sourceSurface = nullptr;
    ANGLE_TRY(source->getSurface(&sourceSurface));

    IDirect3DSurface9 *destSurface = nullptr;
    ANGLE_TRY(dest->getSurface(&destSurface));

    ANGLE_TRY(generateMip(destSurface, sourceSurface));

    dest->markDirty();

    return gl::NoError();
}

gl::Error Image9::copyLockableSurfaces(IDirect3DSurface9 *dest, IDirect3DSurface9 *source)
{
    D3DLOCKED_RECT sourceLock = {0};
    D3DLOCKED_RECT destLock = {0};

    HRESULT result;

    result = source->LockRect(&sourceLock, nullptr, 0);
    if (FAILED(result))
    {
        return gl::OutOfMemory() << "Failed to lock source surface for copy, " << gl::FmtHR(result);
    }

    result = dest->LockRect(&destLock, nullptr, 0);
    if (FAILED(result))
    {
        source->UnlockRect();
        return gl::OutOfMemory() << "Failed to lock source surface for copy, " << gl::FmtHR(result);
    }

    ASSERT(sourceLock.pBits && destLock.pBits);

    D3DSURFACE_DESC desc;
    source->GetDesc(&desc);

    const d3d9::D3DFormat &d3dFormatInfo = d3d9::GetD3DFormatInfo(desc.Format);
    unsigned int rows = desc.Height / d3dFormatInfo.blockHeight;

    unsigned int bytes = d3d9::ComputeBlockSize(desc.Format, desc.Width, d3dFormatInfo.blockHeight);
    ASSERT(bytes <= static_cast<unsigned int>(sourceLock.Pitch) &&
           bytes <= static_cast<unsigned int>(destLock.Pitch));

    for(unsigned int i = 0; i < rows; i++)
    {
        memcpy((char*)destLock.pBits + destLock.Pitch * i, (char*)sourceLock.pBits + sourceLock.Pitch * i, bytes);
    }

    source->UnlockRect();
    dest->UnlockRect();

    return gl::NoError();
}

// static
gl::Error Image9::CopyImage(const gl::Context *context,
                            Image9 *dest,
                            Image9 *source,
                            const gl::Rectangle &sourceRect,
                            const gl::Offset &destOffset,
                            bool unpackFlipY,
                            bool unpackPremultiplyAlpha,
                            bool unpackUnmultiplyAlpha)
{
    IDirect3DSurface9 *sourceSurface = nullptr;
    ANGLE_TRY(source->getSurface(&sourceSurface));

    IDirect3DSurface9 *destSurface = nullptr;
    ANGLE_TRY(dest->getSurface(&destSurface));

    D3DSURFACE_DESC destDesc;
    HRESULT result = destSurface->GetDesc(&destDesc);
    ASSERT(SUCCEEDED(result));
    if (FAILED(result))
    {
        return gl::OutOfMemory()
               << "Failed to query the source surface description for mipmap generation, "
               << gl::FmtHR(result);
    }
    const d3d9::D3DFormat &destD3DFormatInfo = d3d9::GetD3DFormatInfo(destDesc.Format);

    D3DSURFACE_DESC sourceDesc;
    result = sourceSurface->GetDesc(&sourceDesc);
    ASSERT(SUCCEEDED(result));
    if (FAILED(result))
    {
        return gl::OutOfMemory()
               << "Failed to query the destination surface description for mipmap generation, "
               << gl::FmtHR(result);
    }
    const d3d9::D3DFormat &sourceD3DFormatInfo = d3d9::GetD3DFormatInfo(sourceDesc.Format);

    D3DLOCKED_RECT sourceLocked = {0};
    result                      = sourceSurface->LockRect(&sourceLocked, nullptr, D3DLOCK_READONLY);
    ASSERT(SUCCEEDED(result));
    if (FAILED(result))
    {
        return gl::OutOfMemory() << "Failed to lock the source surface for CopyImage, "
                                 << gl::FmtHR(result);
    }

    D3DLOCKED_RECT destLocked = {0};
    result                    = destSurface->LockRect(&destLocked, nullptr, 0);
    ASSERT(SUCCEEDED(result));
    if (FAILED(result))
    {
        sourceSurface->UnlockRect();
        return gl::OutOfMemory() << "Failed to lock the destination surface for CopyImage, "
                                 << gl::FmtHR(result);
    }

    const uint8_t *sourceData = reinterpret_cast<const uint8_t *>(sourceLocked.pBits) +
                                sourceRect.x * sourceD3DFormatInfo.pixelBytes +
                                sourceRect.y * sourceLocked.Pitch;
    uint8_t *destData = reinterpret_cast<uint8_t *>(destLocked.pBits) +
                        destOffset.x * destD3DFormatInfo.pixelBytes +
                        destOffset.y * destLocked.Pitch;
    ASSERT(sourceData && destData);

    CopyImageCHROMIUM(sourceData, sourceLocked.Pitch, sourceD3DFormatInfo.pixelBytes,
                      sourceD3DFormatInfo.info().colorReadFunction, destData, destLocked.Pitch,
                      destD3DFormatInfo.pixelBytes, destD3DFormatInfo.info().colorWriteFunction,
                      gl::GetUnsizedFormat(dest->getInternalFormat()),
                      destD3DFormatInfo.info().componentType, sourceRect.width, sourceRect.height,
                      unpackFlipY, unpackPremultiplyAlpha, unpackUnmultiplyAlpha);

    destSurface->UnlockRect();
    sourceSurface->UnlockRect();

    return gl::NoError();
}

bool Image9::redefine(GLenum target, GLenum internalformat, const gl::Extents &size, bool forceRelease)
{
    // 3D textures are not supported by the D3D9 backend.
    ASSERT(size.depth <= 1);

    // Only 2D and cube texture are supported by the D3D9 backend.
    ASSERT(target == GL_TEXTURE_2D || target == GL_TEXTURE_CUBE_MAP);

    if (mWidth != size.width ||
        mHeight != size.height ||
        mDepth != size.depth ||
        mInternalFormat != internalformat ||
        forceRelease)
    {
        mWidth = size.width;
        mHeight = size.height;
        mDepth = size.depth;
        mInternalFormat = internalformat;

        // compute the d3d format that will be used
        const d3d9::TextureFormat &d3d9FormatInfo = d3d9::GetTextureFormatInfo(internalformat);
        mD3DFormat = d3d9FormatInfo.texFormat;
        mRenderable = (d3d9FormatInfo.renderFormat != D3DFMT_UNKNOWN);

        SafeRelease(mSurface);
        mDirty = (d3d9FormatInfo.dataInitializerFunction != nullptr);

        return true;
    }

    return false;
}

gl::Error Image9::createSurface()
{
    if (mSurface)
    {
        return gl::NoError();
    }

    IDirect3DTexture9 *newTexture = nullptr;
    IDirect3DSurface9 *newSurface = nullptr;
    const D3DPOOL poolToUse = D3DPOOL_SYSTEMMEM;
    const D3DFORMAT d3dFormat = getD3DFormat();

    if (mWidth != 0 && mHeight != 0)
    {
        int levelToFetch = 0;
        GLsizei requestWidth = mWidth;
        GLsizei requestHeight = mHeight;
        d3d9::MakeValidSize(true, d3dFormat, &requestWidth, &requestHeight, &levelToFetch);

        IDirect3DDevice9 *device = mRenderer->getDevice();

        HRESULT result = device->CreateTexture(requestWidth, requestHeight, levelToFetch + 1, 0,
                                               d3dFormat, poolToUse, &newTexture, nullptr);

        if (FAILED(result))
        {
            ASSERT(result == D3DERR_OUTOFVIDEOMEMORY || result == E_OUTOFMEMORY);
            return gl::OutOfMemory() << "Failed to create image surface, " << gl::FmtHR(result);
        }

        newTexture->GetSurfaceLevel(levelToFetch, &newSurface);
        SafeRelease(newTexture);

        const d3d9::TextureFormat &d3dFormatInfo = d3d9::GetTextureFormatInfo(mInternalFormat);
        if (d3dFormatInfo.dataInitializerFunction != nullptr)
        {
            RECT entireRect;
            entireRect.left = 0;
            entireRect.right = mWidth;
            entireRect.top = 0;
            entireRect.bottom = mHeight;

            D3DLOCKED_RECT lockedRect;
            result = newSurface->LockRect(&lockedRect, &entireRect, 0);
            ASSERT(SUCCEEDED(result));
            if (FAILED(result))
            {
                return gl::OutOfMemory() << "Failed to lock image surface, " << gl::FmtHR(result);
            }

            d3dFormatInfo.dataInitializerFunction(mWidth, mHeight, 1, reinterpret_cast<uint8_t*>(lockedRect.pBits),
                                                  lockedRect.Pitch, 0);

            result = newSurface->UnlockRect();
            ASSERT(SUCCEEDED(result));
            if (FAILED(result))
            {
                return gl::OutOfMemory() << "Failed to unlock image surface, " << gl::FmtHR(result);
            }
        }
    }

    mSurface = newSurface;
    mDirty = false;
    mD3DPool = poolToUse;

    return gl::NoError();
}

gl::Error Image9::lock(D3DLOCKED_RECT *lockedRect, const RECT &rect)
{
    gl::Error error = createSurface();
    if (error.isError())
    {
        return error;
    }

    if (mSurface)
    {
        HRESULT result = mSurface->LockRect(lockedRect, &rect, 0);
        ASSERT(SUCCEEDED(result));
        if (FAILED(result))
        {
            return gl::OutOfMemory() << "Failed to lock image surface, " << gl::FmtHR(result);
        }

        mDirty = true;
    }

    return gl::NoError();
}

void Image9::unlock()
{
    if (mSurface)
    {
        HRESULT result = mSurface->UnlockRect();
        ASSERT(SUCCEEDED(result));
    }
}

D3DFORMAT Image9::getD3DFormat() const
{
    // this should only happen if the image hasn't been redefined first
    // which would be a bug by the caller
    ASSERT(mD3DFormat != D3DFMT_UNKNOWN);

    return mD3DFormat;
}

bool Image9::isDirty() const
{
    // Make sure to that this image is marked as dirty even if the staging texture hasn't been created yet
    // if initialization is required before use.
    return (mSurface ||
            d3d9::GetTextureFormatInfo(mInternalFormat).dataInitializerFunction != nullptr) &&
           mDirty;
}

gl::Error Image9::getSurface(IDirect3DSurface9 **outSurface)
{
    gl::Error error = createSurface();
    if (error.isError())
    {
        return error;
    }

    *outSurface = mSurface;
    return gl::NoError();
}

gl::Error Image9::setManagedSurface2D(const gl::Context *context,
                                      TextureStorage *storage,
                                      int level)
{
    IDirect3DSurface9 *surface = nullptr;
    TextureStorage9 *storage9  = GetAs<TextureStorage9>(storage);
    gl::Error error = storage9->getSurfaceLevel(context, GL_TEXTURE_2D, level, false, &surface);
    if (error.isError())
    {
        return error;
    }
    return setManagedSurface(surface);
}

gl::Error Image9::setManagedSurfaceCube(const gl::Context *context,
                                        TextureStorage *storage,
                                        int face,
                                        int level)
{
    IDirect3DSurface9 *surface = nullptr;
    TextureStorage9 *storage9 = GetAs<TextureStorage9>(storage);
    gl::Error error = storage9->getSurfaceLevel(context, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                                                level, false, &surface);
    if (error.isError())
    {
        return error;
    }
    return setManagedSurface(surface);
}

gl::Error Image9::setManagedSurface(IDirect3DSurface9 *surface)
{
    D3DSURFACE_DESC desc;
    surface->GetDesc(&desc);
    ASSERT(desc.Pool == D3DPOOL_MANAGED);

    if ((GLsizei)desc.Width == mWidth && (GLsizei)desc.Height == mHeight)
    {
        if (mSurface)
        {
            gl::Error error = copyLockableSurfaces(surface, mSurface);
            SafeRelease(mSurface);
            if (error.isError())
            {
                return error;
            }
        }

        mSurface = surface;
        mD3DPool = desc.Pool;
    }

    return gl::NoError();
}

gl::Error Image9::copyToStorage(const gl::Context *context,
                                TextureStorage *storage,
                                const gl::ImageIndex &index,
                                const gl::Box &region)
{
    gl::Error error = createSurface();
    if (error.isError())
    {
        return error;
    }

    TextureStorage9 *storage9 = GetAs<TextureStorage9>(storage);

    IDirect3DSurface9 *destSurface = nullptr;

    if (index.type == GL_TEXTURE_2D)
    {
        error =
            storage9->getSurfaceLevel(context, GL_TEXTURE_2D, index.mipIndex, true, &destSurface);
        if (error.isError())
        {
            return error;
        }
    }
    else
    {
        ASSERT(gl::IsCubeMapTextureTarget(index.type));
        error = storage9->getSurfaceLevel(context, index.type, index.mipIndex, true, &destSurface);
        if (error.isError())
        {
            return error;
        }
    }

    error = copyToSurface(destSurface, region);
    SafeRelease(destSurface);
    return error;
}

gl::Error Image9::copyToSurface(IDirect3DSurface9 *destSurface, const gl::Box &area)
{
    ASSERT(area.width > 0 && area.height > 0 && area.depth == 1);
    ASSERT(destSurface);

    IDirect3DSurface9 *sourceSurface = nullptr;
    gl::Error error = getSurface(&sourceSurface);
    if (error.isError())
    {
        return error;
    }

    ASSERT(sourceSurface && sourceSurface != destSurface);

    RECT rect;
    rect.left = area.x;
    rect.top = area.y;
    rect.right = area.x + area.width;
    rect.bottom = area.y + area.height;

    POINT point = {rect.left, rect.top};

    IDirect3DDevice9 *device = mRenderer->getDevice();

    if (mD3DPool == D3DPOOL_MANAGED)
    {
        D3DSURFACE_DESC desc;
        sourceSurface->GetDesc(&desc);

        IDirect3DSurface9 *surf = 0;
        HRESULT result = device->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format,
                                                             D3DPOOL_SYSTEMMEM, &surf, nullptr);
        if (FAILED(result))
        {
            return gl::OutOfMemory()
                   << "Internal CreateOffscreenPlainSurface call failed, " << gl::FmtHR(result);
        }

        auto err = copyLockableSurfaces(surf, sourceSurface);
        result = device->UpdateSurface(surf, &rect, destSurface, &point);
        SafeRelease(surf);
        ANGLE_TRY(err);
        ASSERT(SUCCEEDED(result));
        if (FAILED(result))
        {
            return gl::OutOfMemory() << "Internal UpdateSurface call failed, " << gl::FmtHR(result);
        }
    }
    else
    {
        // UpdateSurface: source must be SYSTEMMEM, dest must be DEFAULT pools
        HRESULT result = device->UpdateSurface(sourceSurface, &rect, destSurface, &point);
        ASSERT(SUCCEEDED(result));
        if (FAILED(result))
        {
            return gl::OutOfMemory() << "Internal UpdateSurface call failed, " << gl::FmtHR(result);
        }
    }

    return gl::NoError();
}

// Store the pixel rectangle designated by xoffset,yoffset,width,height with pixels stored as format/type at input
// into the target pixel rectangle.
gl::Error Image9::loadData(const gl::Context *context,
                           const gl::Box &area,
                           const gl::PixelUnpackState &unpack,
                           GLenum type,
                           const void *input,
                           bool applySkipImages)
{
    // 3D textures are not supported by the D3D9 backend.
    ASSERT(area.z == 0 && area.depth == 1);

    const gl::InternalFormat &formatInfo = gl::GetSizedInternalFormatInfo(mInternalFormat);
    GLuint inputRowPitch                 = 0;
    ANGLE_TRY_RESULT(
        formatInfo.computeRowPitch(type, area.width, unpack.alignment, unpack.rowLength),
        inputRowPitch);
    ASSERT(!applySkipImages);
    ASSERT(unpack.skipPixels == 0);
    ASSERT(unpack.skipRows == 0);

    const d3d9::TextureFormat &d3dFormatInfo = d3d9::GetTextureFormatInfo(mInternalFormat);
    ASSERT(d3dFormatInfo.loadFunction != nullptr);

    RECT lockRect =
    {
        area.x, area.y,
        area.x + area.width, area.y + area.height
    };

    D3DLOCKED_RECT locked;
    gl::Error error = lock(&locked, lockRect);
    if (error.isError())
    {
        return error;
    }

    d3dFormatInfo.loadFunction(area.width, area.height, area.depth,
                               reinterpret_cast<const uint8_t *>(input), inputRowPitch, 0,
                               reinterpret_cast<uint8_t *>(locked.pBits), locked.Pitch, 0);

    unlock();

    return gl::NoError();
}

gl::Error Image9::loadCompressedData(const gl::Context *context,
                                     const gl::Box &area,
                                     const void *input)
{
    // 3D textures are not supported by the D3D9 backend.
    ASSERT(area.z == 0 && area.depth == 1);

    const gl::InternalFormat &formatInfo = gl::GetSizedInternalFormatInfo(mInternalFormat);
    GLsizei inputRowPitch                = 0;
    ANGLE_TRY_RESULT(formatInfo.computeRowPitch(GL_UNSIGNED_BYTE, area.width, 1, 0), inputRowPitch);
    GLsizei inputDepthPitch = 0;
    ANGLE_TRY_RESULT(formatInfo.computeDepthPitch(area.height, 0, inputDepthPitch),
                     inputDepthPitch);

    const d3d9::TextureFormat &d3d9FormatInfo = d3d9::GetTextureFormatInfo(mInternalFormat);

    ASSERT(area.x % d3d9::GetD3DFormatInfo(d3d9FormatInfo.texFormat).blockWidth == 0);
    ASSERT(area.y % d3d9::GetD3DFormatInfo(d3d9FormatInfo.texFormat).blockHeight == 0);

    ASSERT(d3d9FormatInfo.loadFunction != nullptr);

    RECT lockRect =
    {
        area.x, area.y,
        area.x + area.width, area.y + area.height
    };

    D3DLOCKED_RECT locked;
    gl::Error error = lock(&locked, lockRect);
    if (error.isError())
    {
        return error;
    }

    d3d9FormatInfo.loadFunction(area.width, area.height, area.depth,
                                reinterpret_cast<const uint8_t*>(input), inputRowPitch, inputDepthPitch,
                                reinterpret_cast<uint8_t*>(locked.pBits), locked.Pitch, 0);

    unlock();

    return gl::NoError();
}

// This implements glCopyTex[Sub]Image2D for non-renderable internal texture formats and incomplete textures
gl::Error Image9::copyFromRTInternal(const gl::Offset &destOffset,
                                     const gl::Rectangle &sourceArea,
                                     RenderTargetD3D *source)
{
    ASSERT(source);

    // ES3.0 only behaviour to copy into a 3d texture
    ASSERT(destOffset.z == 0);

    RenderTarget9 *renderTarget = GetAs<RenderTarget9>(source);

    IDirect3DSurface9 *surface = renderTarget->getSurface();
    ASSERT(surface);

    IDirect3DDevice9 *device = mRenderer->getDevice();

    IDirect3DSurface9 *renderTargetData = nullptr;
    D3DSURFACE_DESC description;
    surface->GetDesc(&description);

    HRESULT result = device->CreateOffscreenPlainSurface(description.Width, description.Height,
                                                         description.Format, D3DPOOL_SYSTEMMEM,
                                                         &renderTargetData, nullptr);

    if (FAILED(result))
    {
        SafeRelease(surface);
        return gl::OutOfMemory() << "Could not create matching destination surface, "
                                 << gl::FmtHR(result);
    }

    result = device->GetRenderTargetData(surface, renderTargetData);

    if (FAILED(result))
    {
        SafeRelease(renderTargetData);
        SafeRelease(surface);
        return gl::OutOfMemory() << "GetRenderTargetData unexpectedly failed, "
                                 << gl::FmtHR(result);
    }

    int width = sourceArea.width;
    int height = sourceArea.height;

    RECT sourceRect = { sourceArea.x, sourceArea.y, sourceArea.x + width, sourceArea.y + height };
    RECT destRect = { destOffset.x, destOffset.y, destOffset.x + width, destOffset.y + height };

    D3DLOCKED_RECT sourceLock = {0};
    result = renderTargetData->LockRect(&sourceLock, &sourceRect, 0);

    if (FAILED(result))
    {
        SafeRelease(renderTargetData);
        SafeRelease(surface);
        return gl::OutOfMemory()
               << "Failed to lock the source surface (rectangle might be invalid), "
               << gl::FmtHR(result);
    }

    D3DLOCKED_RECT destLock = {0};
    gl::Error error = lock(&destLock, destRect);
    if (error.isError())
    {
        renderTargetData->UnlockRect();
        SafeRelease(renderTargetData);
        SafeRelease(surface);
        return error;
    }

    ASSERT(destLock.pBits && sourceLock.pBits);

    unsigned char *sourcePixels = (unsigned char*)sourceLock.pBits;
    unsigned char *destPixels = (unsigned char*)destLock.pBits;

    switch (description.Format)
    {
      case D3DFMT_X8R8G8B8:
      case D3DFMT_A8R8G8B8:
        switch (getD3DFormat())
        {
          case D3DFMT_X8R8G8B8:
          case D3DFMT_A8R8G8B8:
            for (int y = 0; y < height; y++)
            {
                memcpy(destPixels, sourcePixels, 4 * width);
                sourcePixels += sourceLock.Pitch;
                destPixels += destLock.Pitch;
            }
            break;
          case D3DFMT_L8:
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    destPixels[x] = sourcePixels[x * 4 + 2];
                }
                sourcePixels += sourceLock.Pitch;
                destPixels += destLock.Pitch;
            }
            break;
          case D3DFMT_A8L8:
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    destPixels[x * 2 + 0] = sourcePixels[x * 4 + 2];
                    destPixels[x * 2 + 1] = sourcePixels[x * 4 + 3];
                }
                sourcePixels += sourceLock.Pitch;
                destPixels += destLock.Pitch;
            }
            break;
          default:
            UNREACHABLE();
        }
        break;
      case D3DFMT_R5G6B5:
        switch (getD3DFormat())
        {
          case D3DFMT_X8R8G8B8:
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    unsigned short rgb = ((unsigned short*)sourcePixels)[x];
                    unsigned char red = static_cast<unsigned char>((rgb & 0xF800) >> 8);
                    unsigned char green = static_cast<unsigned char>((rgb & 0x07E0) >> 3);
                    unsigned char blue = static_cast<unsigned char>((rgb & 0x001F) << 3);
                    destPixels[x + 0] = blue | (blue >> 5);
                    destPixels[x + 1] = green | (green >> 6);
                    destPixels[x + 2] = red | (red >> 5);
                    destPixels[x + 3] = 0xFF;
                }
                sourcePixels += sourceLock.Pitch;
                destPixels += destLock.Pitch;
            }
            break;
          case D3DFMT_L8:
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    unsigned char red = sourcePixels[x * 2 + 1] & 0xF8;
                    destPixels[x] = red | (red >> 5);
                }
                sourcePixels += sourceLock.Pitch;
                destPixels += destLock.Pitch;
            }
            break;
          default:
            UNREACHABLE();
        }
        break;
      case D3DFMT_A1R5G5B5:
        switch (getD3DFormat())
        {
          case D3DFMT_X8R8G8B8:
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    unsigned short argb = ((unsigned short*)sourcePixels)[x];
                    unsigned char red = static_cast<unsigned char>((argb & 0x7C00) >> 7);
                    unsigned char green = static_cast<unsigned char>((argb & 0x03E0) >> 2);
                    unsigned char blue = static_cast<unsigned char>((argb & 0x001F) << 3);
                    destPixels[x + 0] = blue | (blue >> 5);
                    destPixels[x + 1] = green | (green >> 5);
                    destPixels[x + 2] = red | (red >> 5);
                    destPixels[x + 3] = 0xFF;
                }
                sourcePixels += sourceLock.Pitch;
                destPixels += destLock.Pitch;
            }
            break;
          case D3DFMT_A8R8G8B8:
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    unsigned short argb = ((unsigned short*)sourcePixels)[x];
                    unsigned char red = static_cast<unsigned char>((argb & 0x7C00) >> 7);
                    unsigned char green = static_cast<unsigned char>((argb & 0x03E0) >> 2);
                    unsigned char blue = static_cast<unsigned char>((argb & 0x001F) << 3);
                    unsigned char alpha = (signed short)argb >> 15;
                    destPixels[x + 0] = blue | (blue >> 5);
                    destPixels[x + 1] = green | (green >> 5);
                    destPixels[x + 2] = red | (red >> 5);
                    destPixels[x + 3] = alpha;
                }
                sourcePixels += sourceLock.Pitch;
                destPixels += destLock.Pitch;
            }
            break;
          case D3DFMT_L8:
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    unsigned char red = sourcePixels[x * 2 + 1] & 0x7C;
                    destPixels[x] = (red << 1) | (red >> 4);
                }
                sourcePixels += sourceLock.Pitch;
                destPixels += destLock.Pitch;
            }
            break;
          case D3DFMT_A8L8:
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    unsigned char red = sourcePixels[x * 2 + 1] & 0x7C;
                    destPixels[x * 2 + 0] = (red << 1) | (red >> 4);
                    destPixels[x * 2 + 1] = (signed char)sourcePixels[x * 2 + 1] >> 7;
                }
                sourcePixels += sourceLock.Pitch;
                destPixels += destLock.Pitch;
            }
            break;
          default:
            UNREACHABLE();
        }
        break;
      default:
        UNREACHABLE();
    }

    unlock();
    renderTargetData->UnlockRect();

    SafeRelease(renderTargetData);
    SafeRelease(surface);

    mDirty = true;
    return gl::NoError();
}

gl::Error Image9::copyFromTexStorage(const gl::Context *context,
                                     const gl::ImageIndex &imageIndex,
                                     TextureStorage *source)
{
    RenderTargetD3D *renderTarget = nullptr;
    gl::Error error               = source->getRenderTarget(context, imageIndex, &renderTarget);
    if (error.isError())
    {
        return error;
    }

    gl::Rectangle sourceArea(0, 0, mWidth, mHeight);
    return copyFromRTInternal(gl::Offset(), sourceArea, renderTarget);
}

gl::Error Image9::copyFromFramebuffer(const gl::Context *context,
                                      const gl::Offset &destOffset,
                                      const gl::Rectangle &sourceArea,
                                      const gl::Framebuffer *source)
{
    const gl::FramebufferAttachment *srcAttachment = source->getReadColorbuffer();
    ASSERT(srcAttachment);

    RenderTargetD3D *renderTarget = nullptr;
    gl::Error error               = srcAttachment->getRenderTarget(context, &renderTarget);
    if (error.isError())
    {
        return error;
    }

    ASSERT(renderTarget);
    return copyFromRTInternal(destOffset, sourceArea, renderTarget);
}

}  // namespace rx
