/**
 @file Texture.cpp

 @author Morgan McGuire, morgan@blueaxion.com

 Notes:
 <UL>
 <LI>http://developer.apple.com/opengl/extensions/ext_texture_rectangle.html
 </UL>

 @created 2001-02-28
 @edited  2004-09-03
*/

#include "GLG3D/glcalls.h"
#include "graphics3D.h"
#include "GLG3D/TextureFormat.h"
#include "GLG3D/Texture.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/GLCaps.h"

namespace G3D {

static const char* cubeMapString[] = {"ft", "bk", "up", "dn", "rt", "lf"};

/**
 Returns true if the system supports automatic MIP-map generation.
 */
static bool hasAutoMipMap() {
    static bool initialized = false;
    static bool ham = false;

    if (! initialized) {
        initialized = true;
        std::string ext = (char*)glGetString(GL_EXTENSIONS);
        ham = (ext.find("GL_SGIS_generate_mipmap") != std::string::npos);
    }

    return ham;
}

static void disableAllTextures() {
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
    glDisable(GL_TEXTURE_RECTANGLE_EXT);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
}

/**
 Pushes all OpenGL texture state.
 */
static void glStatePush() {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

    if (GLCaps::supports_GL_ARB_multitexture()) {
        glActiveTextureARB(GL_TEXTURE0_ARB);
    }
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
        return GL_TEXTURE_RECTANGLE_EXT;

    default:
        debugAssert(false);
        return GL_TEXTURE_2D;
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
            
            // Supported formats as defined by: http://developer.3dlabs.com/openGL/glu_man_pages.pdf
            alwaysAssertM((bytesFormat == GL_COLOR_INDEX) ||
                (bytesFormat == GL_STENCIL_INDEX) ||
                (bytesFormat == GL_DEPTH_COMPONENT) ||
                (bytesFormat == GL_RED) ||
                (bytesFormat == GL_GREEN) ||
                (bytesFormat == GL_BLUE) ||
                (bytesFormat == GL_ALPHA) ||
                (bytesFormat == GL_RGB) ||
                (bytesFormat == GL_RGBA) ||
                (bytesFormat == GL_LUMINANCE) ||
                (bytesFormat == GL_LUMINANCE_ALPHA), "Invalid bytesFormat for gluScaleImage in createTexture.");

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

            freeBytes = true;
        }

        // Intentionally fall through for power of 2 case

    case GL_TEXTURE_RECTANGLE_EXT:

        // Supported formats as defined by: http://developer.3dlabs.com/GLmanpages/glteximage2d.htm
        // textureFormat should throw a proper GL_INVALID_ENUM error, but might need checking also
        alwaysAssertM((bytesFormat == GL_COLOR_INDEX) ||
            (bytesFormat == GL_RED) ||
            (bytesFormat == GL_GREEN) ||
            (bytesFormat == GL_BLUE) ||
            (bytesFormat == GL_ALPHA) ||
            (bytesFormat == GL_RGB) ||
            (bytesFormat == GL_RGBA) ||
            (bytesFormat == GL_LUMINANCE) ||
            (bytesFormat == GL_LUMINANCE_ALPHA) ||
            (bytesFormat == GL_BGR_EXT) ||
            (bytesFormat == GL_BGRA_EXT) ||
            (bytesFormat == GL_422_EXT) ||
            (bytesFormat == GL_422_REV_EXT) ||
            (bytesFormat == GL_422_AVERAGE_EXT) ||
            (bytesFormat == GL_422_REV_AVERAGE_EXT), "Invalid bytesFormat for glTexImage2D in createTexture.");

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
            // Supported formats as defined by: http://developer.3dlabs.com/openGL/glu_man_pages.pdf
            alwaysAssertM((bytesFormat == GL_COLOR_INDEX) ||
                (bytesFormat == GL_STENCIL_INDEX) ||
                (bytesFormat == GL_DEPTH_COMPONENT) ||
                (bytesFormat == GL_RED) ||
                (bytesFormat == GL_GREEN) ||
                (bytesFormat == GL_BLUE) ||
                (bytesFormat == GL_ALPHA) ||
                (bytesFormat == GL_RGB) ||
                (bytesFormat == GL_RGBA) ||
                (bytesFormat == GL_LUMINANCE) ||
                (bytesFormat == GL_LUMINANCE_ALPHA), "Invalid bytesFormat for gluScaleImage in createTexture.");

            int r = gluBuild2DMipmaps(target, textureFormat, width, height, bytesFormat, GL_UNSIGNED_BYTE, bytes);
            debugAssertM(r == 0, (const char*)gluErrorString(r)); (void)r;
            break;
        }

    default:
        debugAssertM(false, "Mipmaps not supported for this texture target");
    }
}



/**
 Overrides the current wrap and interpolation parameters for the
 current texture.
 */
static void setTexParameters(
    GLenum                          target,
    Texture::WrapMode               wrap,
    Texture::InterpolateMode        interpolate,
    Texture::DepthReadMode          depthRead) {

    debugAssert(
        target == GL_TEXTURE_2D ||
        target == GL_TEXTURE_RECTANGLE_EXT ||
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

        if (hasAutoMipMap()) {  
            glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }
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


    if (depthRead == Texture::DEPTH_NORMAL) {
        glTexParameteri(target, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE);
    } else {
        glTexParameteri(target, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);

        glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC_ARB, 
            (depthRead == Texture::DEPTH_LEQUAL) ? GL_LEQUAL : GL_GEQUAL);
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
    bool                    __opaque,
    DepthReadMode           __dr) :
    
    textureID(_textureID),
    dimension(_dimension),
    format(_format),
    _opaque(__opaque),
    _depthRead(__dr) {

    debugAssert(_format);

    glStatePush();

        GLenum target = dimensionToTarget(_dimension);
        glBindTexture(target, _textureID);

        name = _name;

        if (_dimension != DIM_CUBE_MAP) {
            glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);
        } else {
            width = height = 1;
        }

        depth = 1;
        invertY = false;
        
        interpolate         = _interpolate;
        wrap                = _wrap;
        setTexParameters(target, wrap, interpolate, _depthRead);
    glStatePop();
}


TextureRef Texture::fromGLTexture(
    const std::string&      name,
    GLuint                  textureID,
    const TextureFormat*    textureFormat,
    WrapMode                wrap,
    InterpolateMode         interpolate,
    Dimension               dimension,
    DepthReadMode           depthRead) {

    debugAssert(textureFormat);

    return new Texture(name, textureID, dimension, textureFormat, interpolate, wrap, textureFormat->opaque, depthRead);
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
    const std::string               filename[6],
    const class TextureFormat*      desiredFormat,
    WrapMode                        wrap,
    InterpolateMode                 interpolate,
    Dimension                       dimension,
    double                          brighten,
    DepthReadMode                   depthRead) {

    std::string realFilename[6];

    const TextureFormat* format = TextureFormat::RGB8;
    bool opaque = true;

    // The six cube map faces, or the one texture and 5 dummys.
    GImage image[6];
    const uint8* array[6];
    for (int i = 0; i < 6; ++i) {
        array[i] = NULL;
    }

    int numFaces = (dimension == DIM_CUBE_MAP) ? 6 : 1;

    if (dimension == DIM_CUBE_MAP) {
        if (filename[1] == "") {
            // Wildcard format
            // Parse the filename into a base name and extension
            std::string filenameBase, filenameExt;
            splitFilenameAtWildCard(filename[0], filenameBase, filenameExt);
            for (int f = 0; f < 6; ++f) {
                realFilename[f] = filenameBase + cubeMapString[f] + filenameExt;
            }
        } else {
            // Separate filenames have been provided
            realFilename[0] = filename[0];
            for (int f = 1; f < 6; ++f) {
                debugAssert(filename[f] != "");
                realFilename[f] = filename[f];
            }
        }
    } else {
        debugAssertM(filename[1] == "",
            "Can't specify more than one filename unless loading a cube map");
        realFilename[0] = filename[0];
    }

    for (int f = 0; f < numFaces; ++f) {

        image[f].load(realFilename[f]);

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
        Texture::fromMemory(filename[0], array, format,
            image[0].width, image[0].height, 1,
            desiredFormat, wrap, interpolate, dimension,
            depthRead);

    return t;
}


TextureRef Texture::fromFile(
    const std::string&      filename,
    const TextureFormat*    desiredFormat,
    WrapMode                wrap,
    InterpolateMode         interpolate,
    Dimension               dimension,
    double                  brighten,
    DepthReadMode           depthRead) {

    std::string f[6];
    f[0] = filename;
    f[1] = "";
    f[2] = "";
    f[3] = "";
    f[4] = "";
    f[5] = "";

    return fromFile(f, desiredFormat, wrap, interpolate, dimension, brighten, depthRead);
}


TextureRef Texture::fromTwoFiles(
    const std::string&      filename,
    const std::string&      alphaFilename,
    const TextureFormat*    desiredFormat,
    WrapMode                wrap,
    InterpolateMode         interpolate,
    Dimension               dimension,
    DepthReadMode           depthRead) {

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
        splitFilenameAtWildCard(filename, filenameBase, filenameExt);
    }
    
    GImage color[6];
    GImage alpha[6];
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
            desiredFormat, wrap, interpolate, dimension, depthRead);

    if (color[0].channels == 3) {
        // Delete the data if it was dynamically allocated
        for (int f = 0; f < numFaces; ++f) {
            delete[] const_cast<unsigned char*>(array[f]);
        }
    }

    } catch (const GImage::Error& e) {
        Log::common()->printf("\n**************************\n\n"
            "Loading \"%s\" failed. %s\n", e.filename.c_str(),
            e.reason.c_str());
    }

    return t;
}

static const GLenum cubeFaceTarget[] =
    {GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
     GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
     GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
     GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
     GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
     GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB};

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
    Dimension               dimension,
    DepthReadMode           depthRead) {

    debugAssert(bytesFormat);

    // Create the texture
    GLuint textureID = newGLTextureID();
    GLenum target = dimensionToTarget(dimension);

    if (desiredFormat == TextureFormat::AUTO) {
        desiredFormat = bytesFormat;
    }

    glStatePush();

        glEnable(target);
        glBindTexture(target, textureID);
        if ((interpolate == TRILINEAR_MIPMAP) && hasAutoMipMap()) {
            // Enable hardware MIP-map genera
            glTexParameteri(target, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
        }


        int numFaces = (dimension == DIM_CUBE_MAP) ? 6 : 1;
        
        for (int f = 0; f < numFaces; ++f) {
            if (dimension == DIM_CUBE_MAP) {
                // Choose the appropriate face target
                target = cubeFaceTarget[f];
            }

            if ((interpolate == TRILINEAR_MIPMAP) && ! hasAutoMipMap()) {
                createMipMapTexture(target, bytes[f],
                              bytesFormat->OpenGLBaseFormat,
                              width, height, desiredFormat->OpenGLFormat);
            } else {
                createTexture(target, bytes[f], bytesFormat->OpenGLBaseFormat,
                              width, height, desiredFormat->OpenGLFormat, 
                              bytesFormat->packedBitsPerTexel / 8);
            }
        }
        debugAssertGLOk();
    glStatePop();

    if (dimension != DIM_2D_RECT) {
        width  = ceilPow2(width);
        height = ceilPow2(height);
    }

    debugAssertGLOk();
    TextureRef t = fromGLTexture(name, textureID, desiredFormat, wrap, interpolate, dimension, depthRead);

    t->width = width;
    t->height = height;
    return t;
}


TextureRef Texture::fromGImage(
    const std::string&              name,
    const GImage&                   image,
    const class TextureFormat*      desiredFormat,
    WrapMode                        wrap,
    InterpolateMode                 interpolate,
    Dimension                       dimension,
    DepthReadMode                   depthRead) {

    const TextureFormat* format = TextureFormat::RGB8;
    bool opaque = true;

    // The six cube map faces, or the one texture and 5 dummys.
    const uint8* array[1];

    if (image.channels == 4) {
        format = TextureFormat::RGBA8;
        opaque = false;
    }

    if (desiredFormat == NULL) {
        desiredFormat = format;
    }

    array[0] = image.byte();

    TextureRef t =
        Texture::fromMemory(name, array, format,
            image.width, image.height, 1,
            desiredFormat, wrap, interpolate, 
            dimension, depthRead);

    return t;
}


TextureRef Texture::createEmpty(
    int                              w,
    int                              h,
    const std::string&               name,
    const TextureFormat*             desiredFormat,
    Texture::WrapMode                wrap,
    Texture::InterpolateMode         interpolate,
    Texture::Dimension               dimension,
    Texture::DepthReadMode           depthRead) {

    debugAssertM(desiredFormat, "desiredFormat may not be TextureFormat::AUTO");

    // We must pretend the input is in the desired format otherwise 
    // OpenGL might refuse to negotiate formats for us.
    Array<uint8> data(w * h * desiredFormat->packedBitsPerTexel / 8);
    const uint8* bytes[6];
    for (int i = 0; i < 6; ++i) {
        bytes[i] = data.getCArray();
    }

    return Texture::fromMemory(name, bytes, desiredFormat, w, h, 1, desiredFormat, wrap, interpolate, dimension, depthRead);
}


void Texture::splitFilenameAtWildCard(
    const std::string&  filename,
    std::string&        filenameBase,
    std::string&        filenameExt) {

    const std::string splitter = "*";

    int i = filename.rfind(splitter);
    if (i != -1) {
        filenameBase = filename.substr(0, i);
        filenameExt  = filename.substr(i + 1, filename.size() - i - splitter.length()); 
    } else {
        throw GImage::Error("Cube map filenames must contain \"*\" as a "
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
    debugAssert(this->dimension == DIM_2D || this->dimension == DIM_2D_RECT);

    if (GLCaps::supports_GL_ARB_multitexture()) {
        glActiveTextureARB(GL_TEXTURE0_ARB);
    }
    disableAllTextures();
    GLenum target = dimensionToTarget(dimension);
    glEnable(target);

    glBindTexture(target, textureID);
    int e = glGetError();
    alwaysAssertM(e == GL_NONE, 
        std::string("Error encountered during glBindTexture: ") + GLenumToString(e));

    double viewport[4];
    glGetDoublev(GL_VIEWPORT, viewport);
    double viewportHeight = viewport[3];
    debugAssertGLOk();
    
    glCopyTexImage2D(target, 0, format->OpenGLFormat,
                     iRound(rect.x0()), 
                     iRound(viewportHeight - rect.y1()), 
                     iRound(rect.width()), iRound(rect.height()), 
                     0);

    debugAssertGLOk();
    // Reset the original properties
    setTexParameters(target, wrap, interpolate, _depthRead);

    debugAssertGLOk();
    glDisable(target);

    // Once copied from the screen, the direction will be reversed.
    invertY = true;

    glStatePop();
}


void Texture::copyFromScreen(
    const Rect2D&       rect,
    CubeFace            face,
    bool                useBackBuffer) {

    glStatePush();

    if (useBackBuffer) {
        glReadBuffer(GL_BACK);
    } else {
        glReadBuffer(GL_FRONT);
    }    

    // Set up new state
    debugAssertM(width == rect.width(), "Cube maps require all six faces to have the same dimensions");
    debugAssertM(height == rect.height(), "Cube maps require all six faces to have the same dimensions");
    debugAssert(this->dimension == DIM_CUBE_MAP);
    debugAssert(face >= 0);
    debugAssert(face < 6);

    if (GLCaps::supports_GL_ARB_multitexture()) {
        glActiveTextureARB(GL_TEXTURE0_ARB);
    }
    disableAllTextures();

    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, textureID);

    GLenum target = cubeFaceTarget[(int)face];

    int e = glGetError();
    alwaysAssertM(e == GL_NONE, 
        std::string("Error encountered during glBindTexture: ") + GLenumToString(e));

    double viewport[4];
    glGetDoublev(GL_VIEWPORT, viewport);
    double viewportHeight = viewport[3];
    debugAssertGLOk();

    glCopyTexImage2D(target, 0, format->OpenGLFormat,
                     iRound(rect.x0()), 
                     iRound(viewportHeight - rect.y1()), 
                     iRound(rect.width()), 
                     iRound(rect.height()), 0);

    debugAssertGLOk();
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
    glStatePop();
}


void Texture::getCameraRotation(CubeFace face, Matrix3& outMatrix) {
    switch (face) {
    case CUBE_POS_X:
        outMatrix = Matrix3::fromEulerAnglesYXZ(G3D_HALF_PI, G3D_PI, 0);
        break;

    case CUBE_NEG_X:
        outMatrix = Matrix3::fromEulerAnglesYXZ(-G3D_HALF_PI, G3D_PI, 0);
        break;

    case CUBE_POS_Y:
        outMatrix = Matrix3::fromEulerAnglesXYZ(G3D_HALF_PI, 0, 0);
        break;

    case CUBE_NEG_Y:
        outMatrix = Matrix3::fromEulerAnglesXYZ(-G3D_HALF_PI, 0, 0);
        break;

    case CUBE_POS_Z:
        outMatrix = Matrix3::fromEulerAnglesYZX(G3D_PI, G3D_PI, 0);
        break;

    case CUBE_NEG_Z:
        outMatrix = Matrix3::fromAxisAngle(Vector3::unitZ(), G3D_PI);
        break;
    }
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
        return GL_TEXTURE_RECTANGLE_EXT;

    default:
        debugAssertM(false, "Fell through switch");
    }
    return 0;
}

} // G3D
