/**
 @file Texture.cpp

 @author Morgan McGuire, morgan@blueaxion.com

 @created 2001-02-28
 @edited  2003-04-11
*/

#include "GLG3D/glcalls.h"
#include "graphics3D.h"
#include "GLG3D/Texture.h"
#include "GLG3D/getOpenGLState.h"
namespace G3D {

/**
 Used while creating the texture;
 */
class Data {
public:
    int         width;
    int         height;

    /**
     GL_RGB, GL_RGBA
     */
    int         format;

    /**
     In bytes; negative numbers are bits
     */
    int         pixelSize;
    
    uint8*      pixel;
    
    Data();

    /**
      Frees the pixel array.
     */
    ~Data();
};


Data::Data() {
    width  = 0;
    height = 0;
    pixel  = NULL;
}


Data::~Data() {
    width  = 0;
    height = 0;
    free(pixel);
    pixel  = NULL;
}

/////////////////////////////////////////////////////////////////////////////
Texture::Texture(
    const std::string&          _name,
    int                         _width,
    int                         _height,
    int                         _depth,
    unsigned int                _textureID,
    InterpolateMode             _interpolate,
    WrapMode                    _wrap,
    Dimension                   _dimension,
    int                         internalColorChannelBits,
    int                         internalAlphaChannelBits,
    bool                        internalCompress)  : 
    
    name(_name),
    filename(""),
    alphaFilename(""),
    interpolate(_interpolate),
    wrap(_wrap),
    dimension(_dimension),
    width(_width),
    height(_height),
    depth(_depth),
    compressed(internalCompress),
    colorChannelBits(internalColorChannelBits),
    alphaChannelBits(internalAlphaChannelBits),
    textureID(_textureID),
    invertY(false) {

    // TODO: don't trash current texture state!
    int active;
    glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &active);
    glActiveTextureARB(GL_TEXTURE0_ARB);

    GLenum t = getOpenGLTextureTarget();

    if (compressed) {
        // Verify compression worked
        int p;
        glGetTexLevelParameteriv(t, 0, GL_TEXTURE_COMPRESSED_ARB, &p);
        compressed = (p != 0);
    }
    
    if (alphaChannelBits == 0) {
        this->channels = RGB;
    } else if (colorChannelBits == 0) {
        this->channels = ALPHA;
    } else {
        this->channels = RGBA;
    }

    reload();


    switch (interpolate) {
    case TRILINEAR_MIPMAP:
        glTexParameteri(t, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(t, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;

    case BILINEAR_NO_MIPMAP:
        glTexParameteri(t, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glTexParameteri(t, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        break;

    default:
        glTexParameteri(t, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
        glTexParameteri(t, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    }

    glBindTexture(t, 0);
    glActiveTextureARB(active);
}

 
Texture::Texture(
    const std::string&  name,
    const std::string&  filename,
    const std::string&  alphaFilename,
    InterpolateMode     interpolate,
    WrapMode            wrap,
    Dimension           dimension,
    int                 colorChannelBits,
    int                 alphaChannelBits,
    bool                _compress,
    int                 depth) : 
    
    name(name),
    filename(filename),
    alphaFilename(alphaFilename),
    depth(depth),
    compressed(_compress),
    colorChannelBits(colorChannelBits),
    alphaChannelBits(alphaChannelBits),
    invertY(false) {

    width  = 0;
    height = 0;

    // TODO: don't trash current texture state!
    int active;
    glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &active);
    glActiveTextureARB(GL_TEXTURE0_ARB);

    if ((alphaChannelBits == -1)) {
        if (alphaFilename != "") {
            this->alphaChannelBits = this->colorChannelBits;
            alphaChannelBits = this->alphaChannelBits;
        } else {
            alphaChannelBits = 0;
            this->alphaChannelBits = 0;
        }
    }

    debugAssert((alphaChannelBits == 0) || (alphaChannelBits == 1) || (alphaChannelBits == 8));
    debugAssert((colorChannelBits == 5) || (colorChannelBits == 8) || (colorChannelBits == 0));

    if (alphaChannelBits == 0) {
        channels = RGB;
    } else if (colorChannelBits == 0) {
        channels = ALPHA;
    } else {
        channels = RGBA;
    }

    this->interpolate   = interpolate;
    this->wrap          = wrap;
    this->dimension     = dimension;

    // Only supported formats
    debugAssert(channels == RGB || channels == RGBA || channels == ALPHA);
    debugAssert(dimension == DIM_2D || dimension == DIM_2D_RECT);

    glGenTextures(1, &textureID);

    reload();

    GLenum t = getOpenGLTextureTarget();

    if (compressed) {
        // Verify compression worked
        int p;
        glGetTexLevelParameteriv(t, 0, GL_TEXTURE_COMPRESSED_ARB, &p);
        compressed = (p != 0);
    }

    reload();

    glBindTexture(t, 0);
    glActiveTextureARB(active);
}


Texture::Texture(
    const std::string&  name,
    int                 width,
    int                 height,
    int                 depth,
    uint8*              pixels,
    InterpolateMode     interpolate,
    WrapMode            wrap,
    Channels            channels,
    Dimension           dimension,
    int                 colorChannelBits,
    int                 alphaChannelBits,
    bool                _compress) : compressed(_compress), invertY(false) {

    this->name          = name;
    filename            = "";
    alphaFilename       = "";

    if ((alphaFilename != "") && (alphaChannelBits == -1)) {
        this->alphaChannelBits = this->colorChannelBits;
        alphaChannelBits = this->alphaChannelBits;
    }

    debugAssert((colorChannelBits == 5) || (colorChannelBits == 8) || (colorChannelBits == 0));

    if (alphaChannelBits == -1) {
        if (channels == RGB) {
            alphaChannelBits = 0;
        } else {
            alphaChannelBits = 8;
        }
    }

    debugAssert((alphaChannelBits == 0) || (alphaChannelBits == 1) || (alphaChannelBits == 4) || (alphaChannelBits == 8));

    this->width  = width;
    this->height = height;
    this->depth  = depth;
    this->alphaChannelBits = alphaChannelBits;
    this->colorChannelBits = colorChannelBits;

    int active;
    // TODO: don't trash current texture state!
    glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &active);
    glActiveTextureARB(GL_TEXTURE0_ARB);

    debugAssert(pixels != NULL);

    this->interpolate   = interpolate;
    this->wrap          = wrap;
    this->channels      = channels;
    this->dimension     = dimension;

    glGenTextures(1, &textureID);

    reload();

    debugAssert((channels == RGB) || (channels == RGBA) || (channels == ALPHA));

    Data d;
    d.width             = width;
    d.height            = height / depth;
    switch (channels) {
    case RGB:
        d.format     = GL_RGB;
        d.pixelSize  = 3;
        break;

    case RGBA:
        d.format     = GL_RGBA;
        d.pixelSize  = 4;
        break;
    
    case ALPHA:
        if (alphaChannelBits == 8) {
            d.format     = GL_ALPHA;
            d.pixelSize  = 1;
        } else if (alphaChannelBits == 4) {
            d.format     = GL_ALPHA4;
            d.pixelSize  = -4;
        }
        break;
    }

    d.pixel             = pixels;

    switch (interpolate) {
    case TRILINEAR_MIPMAP:
		createMipMapTexture(d);
        break;

    case BILINEAR_NO_MIPMAP:
    case NO_INTERPOLATION:
		createTexture(d);
        break;
    }

    // Prevent destructor from deallocating the memory passed in
    d.pixel = NULL;

    GLenum t = getOpenGLTextureTarget();

    if (compressed) {
        // Verify compression worked
        int p;
        glGetTexLevelParameteriv(t, 0, GL_TEXTURE_COMPRESSED_ARB, &p);
        compressed = (p != 0);
    }

    glBindTexture(t, 0);
    glActiveTextureARB(active);
}


Texture::~Texture() {
	glDeleteTextures(1, &textureID);
	textureID = 0;
}


void Texture::reload() {
    GLenum t = getOpenGLTextureTarget();
    
    glBindTexture(t, textureID);

    switch (wrap) {
    case TILE:
        glTexParameteri(t, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(t, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(t, GL_TEXTURE_WRAP_R, GL_REPEAT);
        break;

    case CLAMP:
        glTexParameteri(t, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(t, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(t, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        break;

    case TRANSPARENT_BORDER:
        glTexParameteri(t, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER_SGIS);
        glTexParameteri(t, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER_SGIS);
        glTexParameteri(t, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER_SGIS);
        {
            Color4 black(0,0,0,0);
            glTexParameterfv(t, GL_TEXTURE_BORDER_COLOR, black);
        }
        break;

    default:
        debugAssert(false);
    }

    if ((filename != "") || (alphaFilename != "")) {
        Data d;
        getData(d);
        width  = d.width;
        height = d.height;
        switch (interpolate) {
        case TRILINEAR_MIPMAP:
		    createMipMapTexture(d);
            break;
    
        case BILINEAR_NO_MIPMAP:
        case NO_INTERPOLATION:
		    createTexture(d);
            break;
        }
    }
}


void Texture::getData(
    Data&       d) {

    try {
        if (filename != "") {
            CImage im = CImage(filename);
            debugAssert((G3D::isPow2(im.width) && G3D::isPow2(im.height)) || (dimension == DIM_2D_RECT));
            d.width  = im.width;
            d.height = im.height;

            if (alphaFilename != "") {
                CImage aim = CImage(alphaFilename);

                debugAssert(aim.width == im.width);
                debugAssert(aim.height == im.height);
        
                d.format    = GL_RGBA;
                d.pixelSize = 4;
                d.pixel     = (uint8*)malloc(d.width * d.height * d.pixelSize);
                debugAssert(d.pixel);

                RGBxRGBtoRGBA(im.pixel, aim.pixel, d.pixel, im.width * im.height);
            } else {
                // Avoid copying the data by stealing it from the CImage.
                d.format    = GL_RGB;
                d.pixelSize = 3;
                d.pixel     = im.pixel;
                // Don't let the destructor dealloc the data on us.
                im.pixel = NULL;
            }
        } else {
            // Alpha only texture
            if (alphaFilename != "") {
                CImage aim = CImage(alphaFilename);

                d.width     = aim.width;
                d.height    = aim.height;
        
                d.format    = GL_ALPHA;
                d.pixelSize = 1;
                d.pixel     = (uint8*)malloc(d.width * d.height * d.pixelSize);
                debugAssert(d.pixel);

                for (int i = 0; i < d.width * d.height; ++i) {
                    d.pixel[i] = aim.pixel[3 * i];
                }
            }
        }
    } catch (CImage::Error e) {
        std::string msg = std::string("Error while loading '") + e.filename + "': " + e.reason;
        
        // Log the error
        debugAssertM(false, msg);
    }

}


unsigned int Texture::newGLTextureID() {
    unsigned int t;
    glGenTextures(1, &t);
    return t;
}


void Texture::createMipMapTexture(const Data& d) {

    GLint t = getOpenGLTextureTarget();
    // Trilinear
    glTexParameteri(t, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(t, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    switch (t) {
    case GL_TEXTURE_2D:
        {
            int r = gluBuild2DMipmaps(t, externalFormat(), d.width, d.height, d.format, GL_UNSIGNED_BYTE, d.pixel);
            debugAssertM(r == 0, (const char*)gluErrorString(r));
            break;
        }

    default:
        debugAssertM(false, "Mipmaps not supported for this texture target");
      }
}


void Texture::createTexture(const Data& d) {
    GLint t = getOpenGLTextureTarget();

    if (interpolate == BILINEAR_NO_MIPMAP) {
        // Bilinear interpolation
        glTexParameteri(t, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glTexParameteri(t, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    } else {
        glTexParameteri(t, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
        glTexParameteri(t, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    }


    switch (t) {
    case GL_TEXTURE_2D:
    case GL_TEXTURE_RECTANGLE_NV:
        // 2D texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
        // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
        glTexImage2D(t, 0, externalFormat(), d.width, d.height, 0, d.format, GL_UNSIGNED_BYTE, d.pixel);
        break;

    default:
        debugAssertM(false, "Fell through switch");
    }
}


void Texture::copyFromScreen(int x, int y, int width, int height, int windowHeight, bool useBackBuffer) {
    // Store old texture state
    int active = glGetInteger(GL_ACTIVE_TEXTURE_ARB);
    int t1d    = glGetInteger(GL_TEXTURE_1D);
    int t2d    = glGetInteger(GL_TEXTURE_2D);
    int t3d    = glGetInteger(GL_TEXTURE_3D);
    int t2dr   = glGetInteger(GL_TEXTURE_RECTANGLE_NV);

    if (useBackBuffer) {
        glReadBuffer(GL_BACK);
    } else {
        glReadBuffer(GL_FRONT);
    }

    GLint target;
    
    if (! isPow2(width) || ! isPow2(height)) {
        target    = GL_TEXTURE_RECTANGLE_NV;
        dimension = DIM_2D_RECT;
    } else {
        target = GL_TEXTURE_2D;
        dimension = DIM_2D;
    }

    // Set up new state
    filename      = "";
    alphaFilename = "";
    this->width   = width;
    this->height  = height;
    this->depth   = 1;
    compressed    = false;
    
    if (textureID == -1) {
        // This texture has never been allocated
        glGenTextures(1, &textureID);
    }

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
    glDisable(GL_TEXTURE_RECTANGLE_NV);
    glEnable(target);

    glBindTexture(target, textureID);
    glCopyTexImage2D(target, 0, getInternalGLFormat(), x, windowHeight - (y + height), width, height, 0);        

    if ((interpolate == BILINEAR_NO_MIPMAP) || (interpolate == TRILINEAR_MIPMAP)) {
        // Bilinear interpolation
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        interpolate = BILINEAR_NO_MIPMAP;
    } else {
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    }
    
    debugAssert(glGetError() == GL_NO_ERROR);
    glDisable(target);

    glReadBuffer(GL_BACK); 

    invertY = true;

    // Restore texture state
    glActiveTextureARB(active);
    if (t1d) { glEnable(GL_TEXTURE_1D); }
    if (t2d) { glEnable(GL_TEXTURE_1D); }
    if (t3d) { glEnable(GL_TEXTURE_1D); }
    if (t2dr) { glEnable(GL_TEXTURE_RECTANGLE_NV); }
}



GLenum Texture::getInternalGLFormat() const {

    if (compressed) {
        if (colorChannelBits == 5) {
            if (alphaChannelBits == 1) {
                return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            } else {
                return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            }
        } else {
            // 8-bit color channels
            if (alphaChannelBits == 8) {
                return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            } else {
                return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            }
        }
    } else {
        // Uncompressed
        if (colorChannelBits == 5) {
            if (alphaChannelBits == 1) {
                return GL_RGB5_A1;
            } else {
                return GL_RGB5;
            }
        } else if (colorChannelBits == 8) {
            if (alphaChannelBits == 8) {
                return GL_RGBA8;
            } else {
                return GL_RGB8;
            }
        } else if (colorChannelBits == 0) {
            if (alphaChannelBits == 8) {
                return GL_ALPHA;
            } else if (alphaChannelBits == 4) {
                return GL_ALPHA4;
            }
        }
    }

    debugAssertM(false, "Illegal internal texture format");
    return 0;
}


int Texture::sizeInMemory() const {
    // bits per pixel
    int bpp;

    switch (getInternalGLFormat()) {
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
        bpp = 4;
        break;

    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        bpp = 8;
        break;

    case GL_ALPHA:
    case GL_ALPHA8:
        bpp = 8;
        break;

    case GL_ALPHA4:
        bpp = 4;
        break;

    case GL_RGB5:
    case GL_RGB5_A1:
        bpp = 16;
        break;

    case 3:
    case GL_RGB8:
        bpp = 24;
        break;

    case 4:
    case GL_RGBA8:
        bpp = 32;
        break;

    default:
        debugAssertM(false, "G3D internal error: Fell through switch");
        bpp = 4;
    }

    int base = (width * height * depth * bpp) / 8;

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


unsigned int Texture::externalFormat() const {

    switch (getInternalGLFormat()) {
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB:
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
    case 3:
        return GL_RGB;

    case GL_RGB5_A1:
    case GL_RGBA8:
    case GL_RGBA:
    case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
        return GL_RGBA;
        break;

    case GL_ALPHA:
    case GL_ALPHA8:
    case GL_ALPHA4:
        return GL_ALPHA;
        break;
    }

    debugAssertM(false, "Fell through switch");
    return 0;
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
