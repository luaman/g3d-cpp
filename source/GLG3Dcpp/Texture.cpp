/**
 @file Texture.cpp

 @author Morgan McGuire, morgan@blueaxion.com

 @created 2001-02-28
 @edited  2003-05-24
*/

#include "GLG3D/glcalls.h"
#include "graphics3D.h"
#include "GLG3D/TextureFormat.h"
#include "GLG3D/Texture.h"
#include "GLG3D/getOpenGLState.h"

namespace G3D {

/**
 Pushes all OpenGL texture state.
 */
static void glStatePush() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);
    glActiveTextureARB(GL_TEXTURE0_ARB);
}

/**
 Pops all OpenGL texture state.
 */
static void glStatePop() {
    glPopClientAttrib();
    glPopAttrib();
}


static GLenum dimensionToTarget(Texture::Dimension d) {
    switch (d) {
    case Texture::DIM_2D:
        return GL_TEXTURE_2D;

    case Texture::DIM_2D_RECT:
        return GL_TEXTURE_RECTANGLE_NV;

    default:
        debugAssert(false);
        return GL_TEXTURE_2D;
    }
}


static void createMipMapTexture(    
    GLenum          target,
    const uint8*    bytes,
    int             bytesFormat,
    int             width,
    int             height,
    GLenum          textureFormat) {

    switch (target) {
    case GL_TEXTURE_2D:
        {
            int r = gluBuild2DMipmaps(target, textureFormat, width, height, bytesFormat, GL_UNSIGNED_BYTE, bytes);
            debugAssertM(r == 0, (const char*)gluErrorString(r));
            break;
        }

    default:
        debugAssertM(false, "Mipmaps not supported for this texture target");
    }
}


static void createTexture(
    GLenum          target,
    const uint8*    bytes,
    GLenum          bytesFormat,
    int             width,
    int             height,
    GLenum          textureFormat) {

    switch (target) {
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE_NV:
        // 2D texture, level of detail 0 (normal), internal format, x size from image, y size from image, 
        // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
        glTexImage2D(target, 0, textureFormat, width, height, 0, bytesFormat, GL_UNSIGNED_BYTE, bytes);
        break;

    default:
        debugAssertM(false, "Fell through switch");
    }

}


/**
 Overrides the current wrap and interpolation parameters for the
 current texture.
 */
static void setTexParameters(
    GLenum                          target,
    Texture::WrapMode               wrap,
    Texture::InterpolateMode        interpolate) {

    // Set the wrap and interpolate state
    switch (wrap) {
    case Texture::TILE:
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
        break;

    case Texture::CLAMP:
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        break;

    case Texture::TRANSPARENT_BORDER:
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_SGIS);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_SGIS);
        glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER_SGIS);
        {
            Color4 black(0,0,0,0);
            glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, black);
        }
        break;

    default:
        debugAssert(false);
    }


    switch (interpolate) {
    case Texture::TRILINEAR_MIPMAP:
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;

    case Texture::BILINEAR_NO_MIPMAP:
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        break;

    case Texture::NO_INTERPOLATION:
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;

    default:
        debugAssert(false);
    }
}

/////////////////////////////////////////////////////////////////////////////


Texture::Texture(
    const std::string&      _name,
    GLuint                  _textureID,
    Dimension               _dimension,
    const TextureFormat*    _format,
    InterpolateMode         _interpolate,
    WrapMode                _wrap,
    bool                    __opaque) :
    
    format(_format),
    textureID(_textureID),
    dimension(_dimension),
    _opaque(__opaque) {


    glStatePush();

        GLenum target = dimensionToTarget(_dimension);
        glBindTexture(target, _textureID);

        name = _name;

        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);
        depth = 1;

        invertY = false;
        
        interpolate         = _interpolate;
        wrap                = _wrap;

        setTexParameters(target, wrap, interpolate);

    glStatePop();
}


TextureRef Texture::fromGLTexture(
    const std::string&      name,
    GLuint                  textureID,
    const TextureFormat*    textureFormat,
    WrapMode                wrap,
    InterpolateMode         interpolate,
    Dimension               dimension,
    bool                    opaque) {

    return new Texture(name, textureID, dimension, textureFormat, interpolate, wrap, opaque);
}


TextureRef Texture::fromFile(
    const std::string&      filename,
    const TextureFormat*    desiredFormat,
    WrapMode                wrap,
    InterpolateMode         interpolate,
    Dimension               dimension) {

    CImage data(filename);

    const TextureFormat* format = TextureFormat::RGB8;
    bool opaque = true;

    if (data.channels == 4) {
        format = TextureFormat::RGBA8;
        opaque = false;
    }

    TextureRef t =
        Texture::fromMemory(filename, data.byte(), format, data.width, data.height, 1, 
            desiredFormat, wrap, interpolate, dimension);

    return t;
}


TextureRef Texture::fromTwoFiles(
    const std::string&      filename,
    const std::string&      alphaFilename,
    const TextureFormat*    desiredFormat,
    WrapMode                wrap,
    InterpolateMode         interpolate,
    Dimension               dimension) {

    // Compose the two images to a single RGBA

    CImage color(filename);
    CImage alpha(alphaFilename);
    uint8* data = NULL;

    if (color.channels == 4) {
        data = color.byte();
        // Write the data inline
        for (int i = 0; i < color.width * color.height; ++i) {
            data[i * 4 + 3] = alpha.byte()[i * alpha.channels];
        }
    } else {
        debugAssert(color.channels == 3);
        data = new uint8[color.width * color.height * 4];
        // Write the data inline
        for (int i = 0; i < color.width * color.height; ++i) {
            data[i * 4 + 0] = color.byte()[i * 3 + 0];
            data[i * 4 + 1] = color.byte()[i * 3 + 1];
            data[i * 4 + 2] = color.byte()[i * 3 + 2];
            data[i * 4 + 3] = alpha.byte()[i * alpha.channels];
        }
    }

    TextureRef t =
        Texture::fromMemory(filename, data, TextureFormat::RGBA8, color.width, color.height, 1, 
            desiredFormat, wrap, interpolate, dimension);

    if (color.channels == 3) {
        // Delete the data if it was dynamically allocated
        delete[] data;
    }
    data = NULL;

    return t;
}


TextureRef Texture::fromMemory(
    const std::string&      name,
    const uint8*            bytes,
    const TextureFormat*    bytesFormat,
    int                     width,
    int                     height,
    int                     depth,
    const TextureFormat*    desiredFormat,
    WrapMode                wrap,
    InterpolateMode         interpolate,
    Dimension               dimension) {

    // Create the texture
    GLuint textureID = newGLTextureID();
    GLenum target = dimensionToTarget(dimension);

    glStatePush();

        glEnable(target);
        glBindTexture(target, textureID);
        if (interpolate == TRILINEAR_MIPMAP) {
            createMipMapTexture(target, bytes, bytesFormat->OpenGLBaseFormat, width, height, desiredFormat->OpenGLFormat);
        } else {
            createTexture(target, bytes, bytesFormat->OpenGLBaseFormat, width, height, desiredFormat->OpenGLFormat);
        }

    glStatePop();

    return fromGLTexture(name, textureID, desiredFormat, wrap, interpolate, dimension, bytesFormat->opaque);
}


Texture::~Texture() {
	glDeleteTextures(1, &textureID);
	textureID = 0;
}


unsigned int Texture::newGLTextureID() {
    unsigned int t;
    glGenTextures(1, &t);
    return t;
}


void Texture::copyFromScreen(int x, int y, int width, int height, int windowHeight, Dimension dim, bool useBackBuffer) {
    glStatePush();

    if (useBackBuffer) {
        glReadBuffer(GL_BACK);
    } else {
        glReadBuffer(GL_FRONT);
    }

    
    dimension = dim;
    GLenum target = dimensionToTarget(dim);

    // Set up new state
    this->width   = width;
    this->height  = height;
    this->depth   = 1;
    
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
    glEnable(target);

    glBindTexture(target, textureID);
    glCopyTexImage2D(target, 0, format->OpenGLFormat, x, windowHeight - (y + height), width, height, 0);

    setTexParameters(target, wrap, interpolate);

    debugAssert(glGetError() == GL_NO_ERROR);
    glDisable(target);

    invertY = true;

    glStatePop();
 
}




int Texture::sizeInMemory() const {

    int base = (width * height * depth * format->bitsPerTexel) / 8;

    int total = 0;

    if (interpolate == TRILINEAR_MIPMAP) {
        int w = width;
        int h = height;

        while ((w > 2) && (h > 2)) {
            total += base;
            base /= 4;
            w /= 2;
            h /= 2;
        }

    } else {
        total = base;
    }

    return total;
}

unsigned int Texture::getOpenGLTextureTarget() const {
    switch (dimension) {
    case DIM_2D:
        return GL_TEXTURE_2D;

    case Texture::DIM_2D_RECT:
        return GL_TEXTURE_RECTANGLE_NV;

    default:
        debugAssertM(false, "Fell through switch");
    }
    return 0;
}

} // G3D
