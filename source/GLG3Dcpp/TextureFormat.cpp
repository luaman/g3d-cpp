/**
 @file TextureFormat.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2003-05-23
 @edited  2006-01-11
 */

#include "GLG3D/TextureFormat.h"
#include "GLG3D/glheaders.h"
#include "GLG3D/glcalls.h"

namespace G3D {
static bool INT = false;
static bool FLOAT = true;
static bool OPAQUEx = true;
 
const TextureFormat* TextureFormat::L8        = new TextureFormat(1, false, GL_LUMINANCE8, GL_LUMINANCE, 8, 0, 0, 0, 0, 0, 8, 8, OPAQUEx, INT);

const TextureFormat* TextureFormat::L16       = new TextureFormat(1, false, GL_LUMINANCE16, GL_LUMINANCE, 16, 0, 0, 0, 0, 0, 16, 16, OPAQUEx, INT);

const TextureFormat* TextureFormat::L16F      = new TextureFormat(1, false, GL_LUMINANCE16F_ARB, GL_LUMINANCE, 16, 0, 0, 0, 0, 0, 16, 16, OPAQUEx, FLOAT);

const TextureFormat* TextureFormat::L32F      = new TextureFormat(1, false, GL_LUMINANCE32F_ARB, GL_LUMINANCE, 32, 0, 0, 0, 0, 0, 32, 32, OPAQUEx, FLOAT);

const TextureFormat* TextureFormat::A8        = new TextureFormat(1, false, GL_ALPHA8, GL_ALPHA, 0, 8, 0, 0, 0, 0, 8, 8, !OPAQUEx, INT);

const TextureFormat* TextureFormat::A16       = new TextureFormat(1, false, GL_ALPHA16, GL_ALPHA, 0, 16, 0, 0, 0, 0, 16, 16, !OPAQUEx, INT);

const TextureFormat* TextureFormat::A16F      = new TextureFormat(1, false, GL_ALPHA16F_ARB, GL_ALPHA, 0, 16, 0, 0, 0, 0, 16, 16, !OPAQUEx, FLOAT);

const TextureFormat* TextureFormat::A32F      = new TextureFormat(1, false, GL_ALPHA32F_ARB, GL_ALPHA, 0, 32, 0, 0, 0, 0, 32, 32, !OPAQUEx, FLOAT);

const TextureFormat* TextureFormat::LA4       = new TextureFormat(2, false, GL_LUMINANCE4_ALPHA4, GL_LUMINANCE_ALPHA, 4, 4, 0, 0, 0, 0, 8, 8, !OPAQUEx, INT);
 
const TextureFormat* TextureFormat::LA8       = new TextureFormat(2, false, GL_LUMINANCE8_ALPHA8, GL_LUMINANCE_ALPHA, 8, 8, 0, 0, 0, 0, 16, 16, !OPAQUEx, INT);

const TextureFormat* TextureFormat::LA16      = new TextureFormat(2, false, GL_LUMINANCE16_ALPHA16, GL_LUMINANCE_ALPHA, 16, 16, 0, 0, 0, 0, 16*2, 16*2, !OPAQUEx, INT);

const TextureFormat* TextureFormat::LA16F     = new TextureFormat(2, false, GL_LUMINANCE_ALPHA16F_ARB, GL_LUMINANCE_ALPHA, 16, 16, 0, 0, 0, 0, 16*2, 16*2, !OPAQUEx, FLOAT);

const TextureFormat* TextureFormat::LA32F     = new TextureFormat(2, false, GL_LUMINANCE_ALPHA32F_ARB, GL_LUMINANCE_ALPHA, 32, 32, 0, 0, 0, 0, 32*2, 32*2, !OPAQUEx, FLOAT);

const TextureFormat* TextureFormat::RGB5      = new TextureFormat(3, false, GL_RGB5, GL_RGBA, 0, 0, 5, 5, 5, 0, 16, 16, OPAQUEx, INT);

const TextureFormat* TextureFormat::RGB5A1    = new TextureFormat(4, false, GL_RGB5_A1, GL_RGBA, 0, 1, 5, 5, 5, 0, 16, 16, OPAQUEx, INT);

const TextureFormat* TextureFormat::RGB8      = new TextureFormat(3, false, GL_RGB8, GL_RGB, 0, 0, 8, 8, 8, 0, 24, 32, OPAQUEx, INT);

const TextureFormat* TextureFormat::RGB16     = new TextureFormat(3, false, GL_RGB16, GL_RGB, 0, 0, 16, 16, 16, 0, 16*3, 16*3, OPAQUEx, INT);

const TextureFormat* TextureFormat::RGB16F    = new TextureFormat(3, false, GL_RGB16F_ARB, GL_RGB, 0, 0, 16, 16, 16, 0, 16*3, 16*3, OPAQUEx, FLOAT);

const TextureFormat* TextureFormat::RGB32F    = new TextureFormat(3, false, GL_RGB32F_ARB, GL_RGB, 0, 0, 32, 32, 32, 0, 32*3, 32*3, OPAQUEx, FLOAT);

const TextureFormat* TextureFormat::RGBA8     = new TextureFormat(4, false, GL_RGBA8, GL_RGBA, 0, 8, 8, 8, 8, 0, 32, 32, false, INT);

const TextureFormat* TextureFormat::RGBA16    = new TextureFormat(4, false, GL_RGBA16, GL_RGBA, 0, 16, 16, 16, 16, 0, 16*4, 16*4, false, INT);

const TextureFormat* TextureFormat::RGBA16F   = new TextureFormat(4, false, GL_RGBA16F_ARB, GL_RGBA, 0, 16, 16, 16, 16, 0, 16*4, 16*4, false, FLOAT);

const TextureFormat* TextureFormat::RGBA32F   = new TextureFormat(4, false, GL_RGBA32F_ARB, GL_RGBA, 0, 32, 32, 32, 32, 0, 32*4, 32*4, false, FLOAT);

const TextureFormat* TextureFormat::RGB_DXT1  = new TextureFormat(3, true, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_RGB, 0, 0, 0, 0, 0, 0, 64, 64, OPAQUEx, INT);

const TextureFormat* TextureFormat::RGBA_DXT1 = new TextureFormat(4, true, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_RGBA, 0, 0, 0, 0, 0, 0, 64, 64, !OPAQUEx, INT);

const TextureFormat* TextureFormat::RGBA_DXT3 = new TextureFormat(4, true, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_RGBA, 0, 0, 0, 0, 0, 0, 128, 128, !OPAQUEx, INT);

const TextureFormat* TextureFormat::RGBA_DXT5 = new TextureFormat(4, true, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_RGBA, 0, 0, 0, 0, 0, 0, 128, 128, !OPAQUEx, INT);

const TextureFormat* TextureFormat::DEPTH16   = new TextureFormat(1, false, GL_DEPTH_COMPONENT16_ARB, GL_DEPTH_COMPONENT, 0, 0, 0, 0, 0, 16, 16, 16, !OPAQUEx, INT);

const TextureFormat* TextureFormat::DEPTH24   = new TextureFormat(1, false, GL_DEPTH_COMPONENT24_ARB, GL_DEPTH_COMPONENT, 0, 0, 0, 0, 0, 24, 32, 24, !OPAQUEx, INT);

const TextureFormat* TextureFormat::DEPTH32   = new TextureFormat(1, false, GL_DEPTH_COMPONENT32_ARB, GL_DEPTH_COMPONENT, 0, 0, 0, 0, 0, 32, 32, 32, !OPAQUEx, INT);

const TextureFormat* TextureFormat::AUTO      = NULL;

// These formats are for use with Renderbuffers only!
const TextureFormat* TextureFormat::STENCIL1 = new TextureFormat(1, false, GL_STENCIL_INDEX1_EXT, GL_STENCIL_INDEX_EXT, 0, 0, 0, 0, 0, 0, 1, 1, !OPAQUEx, INT);
	
const TextureFormat* TextureFormat::STENCIL4 = new TextureFormat(1, false, GL_STENCIL_INDEX4_EXT, GL_STENCIL_INDEX_EXT, 0, 0, 0, 0, 0, 0, 4, 4, !OPAQUEx, INT);
	
const TextureFormat* TextureFormat::STENCIL8 = new TextureFormat(1, false, GL_STENCIL_INDEX8_EXT, GL_STENCIL_INDEX_EXT, 0, 0, 0, 0, 0, 0, 8, 8, !OPAQUEx, INT);
	
const TextureFormat* TextureFormat::STENCIL16 = new TextureFormat(1, false, GL_STENCIL_INDEX16_EXT, GL_STENCIL_INDEX_EXT, 0, 0, 0, 0, 0, 0, 16, 16, !OPAQUEx, INT);


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

const TextureFormat* TextureFormat::stencil(int bits) {

    if (bits == SAME_AS_SCREEN) {
        // Detect screen depth
        bits = glGetInteger(GL_STENCIL_BITS);
    }

    switch (bits) {
    case 1:
        return STENCIL1;

    case 4:
        return STENCIL4;

    case 8:
        return STENCIL8;

    case 16:
        return STENCIL16;

    default:
        debugAssertM(false, "Stencil must be 1, 4, 8 or 16.");
        return STENCIL16;
    }
}

}
