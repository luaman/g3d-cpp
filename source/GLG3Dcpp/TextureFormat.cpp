/**
 @file TextureFormat.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2003-05-23
 @edited  2004-02-13
 */

#include "GLG3D/TextureFormat.h"
#include "GLG3D/glheaders.h"
#include "GLG3D/glcalls.h"

namespace G3D {

const TextureFormat* TextureFormat::L8        = new TextureFormat(1, false, GL_LUMINANCE8, GL_LUMINANCE, 8, 0, 0, 0, 0, 0, 8, 8, true);

const TextureFormat* TextureFormat::A8        = new TextureFormat(1, false, GL_ALPHA8, GL_ALPHA, 0, 8, 0, 0, 0, 0, 8, 8, false);
 
const TextureFormat* TextureFormat::LA8       = new TextureFormat(2, false, GL_LUMINANCE4_ALPHA4, GL_LUMINANCE_ALPHA, 8, 8, 0, 0, 0, 0, 16, 16, false);

const TextureFormat* TextureFormat::RGB5      = new TextureFormat(3, false, GL_RGB5, GL_RGBA, 0, 0, 5, 5, 5, 0, 16, 16, true);

const TextureFormat* TextureFormat::RGB5A1    = new TextureFormat(4, false, GL_RGB5_A1, GL_RGBA, 0, 1, 5, 5, 5, 0, 16, 16, false);

const TextureFormat* TextureFormat::RGB8      = new TextureFormat(3, false, GL_RGB8, GL_RGB, 0, 0, 8, 8, 8, 0, 24, 32, true);

const TextureFormat* TextureFormat::RGBA8     = new TextureFormat(4, false, GL_RGBA8, GL_RGBA, 0, 8, 8, 8, 8, 0, 32, 32, false);

const TextureFormat* TextureFormat::RGB_DXT1  = new TextureFormat(3, true, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_RGB, 0, 0, 0, 0, 0, 0, 64, 64, true);

const TextureFormat* TextureFormat::RGBA_DXT1 = new TextureFormat(4, true, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_RGBA, 0, 0, 0, 0, 0, 0, 64, 64, false);

const TextureFormat* TextureFormat::RGBA_DXT3 = new TextureFormat(4, true, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_RGBA, 0, 0, 0, 0, 0, 0, 128, 128, false);

const TextureFormat* TextureFormat::RGBA_DXT5 = new TextureFormat(4, true, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_RGBA, 0, 0, 0, 0, 0, 0, 128, 128, false);

const TextureFormat* TextureFormat::DEPTH16   = new TextureFormat(1, false, GL_DEPTH_COMPONENT16_ARB, GL_DEPTH_COMPONENT, 0, 0, 0, 0, 0, 16, 16, 16, true);

const TextureFormat* TextureFormat::DEPTH24   = new TextureFormat(1, false, GL_DEPTH_COMPONENT24_ARB, GL_DEPTH_COMPONENT, 0, 0, 0, 0, 0, 24, 32, 24, true);

const TextureFormat* TextureFormat::DEPTH32   = new TextureFormat(1, false, GL_DEPTH_COMPONENT32_ARB, GL_DEPTH_COMPONENT, 0, 0, 0, 0, 0, 32, 32, 32, true);

const TextureFormat* TextureFormat::AUTO      = NULL;

const TextureFormat* TextureFormat::depth(int depthBits) {

    if (depthBits == SAME_AS_SCREEN) {
        // Detect screen depth
        depthBits = glGetInteger(GL_DEPTH_BITS);
    }

    switch (depthBits) {
    case 16:
        return DEPTH16;

    case 24:
        return DEPTH24;

    case 32:
        return DEPTH32;

    default:
        debugAssertM(false, "Depth must be 16, 24, or 32.");
        return DEPTH32;
    }
}

}
