/**
 @file Texture.cpp

 @author Morgan McGuire, morgan@blueaxion.com

 @created 2001-02-28
 @edited  2003-11-13
*/

#include "GLG3D/glcalls.h"
#include "graphics3D.h"
#include "GLG3D/TextureFormat.h"
#include "GLG3D/Texture.h"
#include "GLG3D/getOpenGLState.h"

namespace G3D {

static const char* cubeMapString[] = {"ft", "bk", "up", "dn", "rt", "lf"};

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
    case Texture::DIM_CUBE_MAP:
        return GL_TEXTURE_CUBE_MAP_ARB;

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
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:

        {
            int r = gluBuild2DMipmaps(target, textureFormat, width, height, bytesFormat, GL_UNSIGNED_BYTE, bytes);
            debugAssertM(r == 0, (const char*)gluErrorString(r)); (void)r;
            break;
        }

    default:
        debugAssertM(false, "Mipmaps not supported for this texture target");
    }
}


static void createTexture(
    GLenum          target,
    const uint8*    rawBytes,
    GLenum          bytesFormat,
    int             width,
    int             height,
    GLenum          textureFormat,
    int             bytesPerPixel) {

    uint8* bytes = const_cast<uint8*>(rawBytes);

    // If true, we're supposed to free the byte array at the end of
    // the function.
    bool   freeBytes = false; 

    switch (target) {
    case GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB:
    case GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB:
    case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB:
    case GL_TEXTURE_2D:
        if (! isPow2(width) || ! isPow2(height)) {

            int oldWidth = width;
            int oldHeight = height;
            width  = ceilPow2(width);
            height = ceilPow2(height);

            bytes = new uint8[width * height * bytesPerPixel];

            // Rescale the image to a power of 2
            gluScaleImage(
                bytesFormat,
                oldWidth,
                oldHeight,
                GL_UNSIGNED_BYTE,
                rawBytes,
                width,
                height,
                GL_UNSIGNED_BYTE,
                bytes);
        }

        // Intentionally fall through

    case GL_TEXTURE_RECTANGLE_NV:
        // 2D texture, level of detail 0 (normal), internal format, x size from image, y size from image, 
        // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
        glTexImage2D(target, 0, textureFormat, width, height, 0, bytesFormat, GL_UNSIGNED_BYTE, bytes);
        break;

    default:
        debugAssertM(false, "Fell through switch");
    }

    if (freeBytes) {
        // Texture was resized; free the temporary.
        delete bytes;
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

    debugAssert(
        target == GL_TEXTURE_2D ||
        target == GL_TEXTURE_RECTANGLE_NV ||
        target == GL_TEXTURE_CUBE_MAP_ARB);

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
    
    textureID(_textureID),
    dimension(_dimension),
    format(_format),
    _opaque(__opaque) {

    debugAssert(_format);

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

    debugAssert(textureFormat);

    return new Texture(name, textureID, dimension, textureFormat, interpolate, wrap, opaque);
}


/**
 Scales the intensity up or down of an entire image.
 @param skipAlpha 0 if there is no alpha channel, 1 if there is 
 */
static void brightenImage(uint8* byte, int n, double brighten, int skipAlpha) {

    // Make a lookup table
    uint8 bright[256];
    for (int i = 0; i < 256; ++i) {
        bright[i] = iClamp(iRound(i * brighten), 0, 255);
    }

    for (int i = 0; i < n; i += skipAlpha) {
        for (int c = 0; c < 3; ++c, ++i) {
            byte[i] = bright[byte[i]];
        }
    }
}

TextureRef Texture::fromFile(
    const std::string&      filename,
    const TextureFormat*    desiredFormat,
    WrapMode                wrap,
    InterpolateMode         interpolate,
    Dimension               dimension,
    double                  brighten) {

    const TextureFormat* format = TextureFormat::RGB8;
    bool opaque = true;

    // The six cube map faces, or the one texture and 5 dummys.
    CImage image[6];
    const uint8* array[6];
    for (int i = 0; i < 6; ++i) {
        array[i] = NULL;
    }

    int numFaces = (dimension == DIM_CUBE_MAP) ? 6 : 1;

    // Parse the filename into a base name and extension
    std::string filenameBase = filename;
    std::string filenameExt;

    if (dimension == DIM_CUBE_MAP) {
        splitFilename(filename, filenameBase, filenameExt);
    }

    for (int f = 0; f < numFaces; ++f) {

        std::string fn = filename;

        if (dimension == DIM_CUBE_MAP) {
            fn = filenameBase + cubeMapString[f] + filenameExt;
        }

        image[f].load(fn);

        if (image[f].channels == 4) {
            format = TextureFormat::RGBA8;
            opaque = false;
        }

        if (desiredFormat == NULL) {
            desiredFormat = format;
        }

        array[f] = image[f].byte();
    }


    if (brighten != 1.0) {
        for (int f = 0; f < numFaces; ++f) {
            brightenImage(
                image[f].byte(),
                image[f].width * image[f].height * image[f].channels,
                brighten,
                image[f].channels - 3);
        }
    }

    TextureRef t =
        Texture::fromMemory(filename, array, format,
            image[0].width, image[0].height, 1,
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

    debugAssert(desiredFormat);

    // The six cube map faces, or the one texture and 5 dummys.
    const uint8* array[6];
    for (int i = 0; i < 6; ++i) {
        array[i] = NULL;
    }

    int numFaces = (dimension == DIM_CUBE_MAP) ? 6 : 1;

    // Parse the filename into a base name and extension
    std::string filenameBase = filename;
    std::string filenameExt;
    std::string alphaFilenameBase = alphaFilename;
    std::string alphaFilenameExt;

    if (dimension == DIM_CUBE_MAP) {
        splitFilename(filename, filenameBase, filenameExt);
    }
    
    CImage color[6];
    CImage alpha[6];
    TextureRef t;

    try {
    for (int f = 0; f < numFaces; ++f) {

        std::string fn = filename;
        std::string an = alphaFilename;

        if (dimension == DIM_CUBE_MAP) {
            fn = filenameBase + cubeMapString[f] + filenameExt;
            an = alphaFilenameBase + cubeMapString[f] + alphaFilenameExt;
        }

        // Compose the two images to a single RGBA
        color[f].load(fn);
        alpha[f].load(an);
        uint8* data = NULL;

        if (color[f].channels == 4) {
            data = color[f].byte();
            // Write the data inline
            for (int i = 0; i < color[f].width * color[f].height; ++i) {
                data[i * 4 + 3] = alpha[f].byte()[i * alpha[f].channels];
            }
        } else {
            debugAssert(color[f].channels == 3);
            data = new uint8[color[f].width * color[f].height * 4];
            // Write the data inline
            for (int i = 0; i < color[f].width * color[f].height; ++i) {
                data[i * 4 + 0] = color[f].byte()[i * 3 + 0];
                data[i * 4 + 1] = color[f].byte()[i * 3 + 1];
                data[i * 4 + 2] = color[f].byte()[i * 3 + 2];
                data[i * 4 + 3] = alpha[f].byte()[i * alpha[f].channels];
            }
        }

        array[f] = data;
    }

    t = Texture::fromMemory(filename, array, TextureFormat::RGBA8,
            color[0].width, color[0].height, 1, 
            desiredFormat, wrap, interpolate, dimension);

    if (color[0].channels == 3) {
        // Delete the data if it was dynamically allocated
        for (int f = 0; f < numFaces; ++f) {
            delete[] const_cast<unsigned char*>(array[f]);
        }
    }

    } catch (const CImage::Error& e) {
        Log::common()->printf("\n**************************\n\n"
            "Loading \"%s\" failed. %s\n", e.filename.c_str(),
            e.reason.c_str());
    }

    return t;
}


TextureRef Texture::fromMemory(
    const std::string&      name,
    const uint8**           bytes,
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

        int numFaces = (dimension == DIM_CUBE_MAP) ? 6 : 1;
        
        for (int f = 0; f < numFaces; ++f) {
            if (dimension == DIM_CUBE_MAP) {
                // Choose the appropriate face target
                static const GLenum cubeFaceTarget[] =
                    {GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
                    GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
                    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
                    GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
                    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB};

                target = cubeFaceTarget[f];
            }

            if (interpolate == TRILINEAR_MIPMAP) {
                createMipMapTexture(target, bytes[f],
                              bytesFormat->OpenGLBaseFormat,
                              width, height, desiredFormat->OpenGLFormat);
            } else {
                createTexture(target, bytes[f], bytesFormat->OpenGLBaseFormat,
                              width, height, desiredFormat->OpenGLFormat, 
                              bytesFormat->packedBitsPerTexel / 8);
            }
        }

    glStatePop();

    if (dimension != DIM_2D_RECT) {
        width  = ceilPow2(width);
        height = ceilPow2(height);
    }

    return fromGLTexture(name, textureID, desiredFormat, wrap,
         interpolate, dimension, bytesFormat->opaque);
}


void Texture::splitFilename(
    const std::string&  filename,
    std::string&        filenameBase,
    std::string&        filenameExt) {

    const std::string splitter = "*";

    int i = filename.rfind(splitter);
    if (i != -1) {
        filenameBase = filename.substr(0, i);
        filenameExt  = filename.substr(i + 1, filename.size() - i - splitter.length()); 
    } else {
        throw CImage::Error("Cube map filenames must contain \"*\" as a "
                            "placeholder for up/lf/rt/bk/ft/dn", filename);
    }
}


Texture::~Texture() {
	glDeleteTextures(1, &textureID);
	textureID = 0;
}


unsigned int Texture::newGLTextureID() {
    unsigned int t;
    glGenTextures(1, &t);

    alwaysAssertM(glGetError() != GL_INVALID_OPERATION, 
         "GL_INVALID_OPERATION: Probably caused by invoking "
         "glGenTextures between glBegin and glEnd.");

    return t;
}


void Texture::copyFromScreen(
    const Rect2D& rect,
    bool useBackBuffer) {

    glStatePush();

    if (useBackBuffer) {
        glReadBuffer(GL_BACK);
    } else {
        glReadBuffer(GL_FRONT);
    }    

    // Set up new state
    this->width   = (int)rect.width();
    this->height  = (int)rect.height();
    this->depth   = 1;
    this->dimension = DIM_2D;
    
    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_3D);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, textureID);
    int e = glGetError();
    alwaysAssertM(e == GL_NONE, 
        std::string("Error encountered during glBindTexture: ") + GLenumToString(e));

    double viewport[4];
    glGetDoublev(GL_VIEWPORT, viewport);
    double viewportHeight = viewport[3];
        
    glCopyTexImage2D(GL_TEXTURE_2D, 0, format->OpenGLFormat, rect.x0(), viewportHeight - rect.y1(), rect.width(), rect.height(), 0);

    setTexParameters(GL_TEXTURE_2D, wrap, interpolate);

    debugAssert(glGetError() == GL_NO_ERROR);
    glDisable(GL_TEXTURE_2D);

    invertY = true;

    glStatePop();
}


int Texture::sizeInMemory() const {

    int base = (width * height * depth * format->hardwareBitsPerTexel) / 8;

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

    if (dimension == DIM_CUBE_MAP) {
        total *= 6;
    }

    return total;
}


unsigned int Texture::getOpenGLTextureTarget() const {
    switch (dimension) {
    case DIM_CUBE_MAP:
        return GL_TEXTURE_CUBE_MAP_ARB;

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
