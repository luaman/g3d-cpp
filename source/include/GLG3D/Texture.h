/**
  @file Texture.h

  @maintainer Morgan McGuire, morgan@blueaxion.com

  @created 2001-02-28
  @edited  2002-12-14
*/

#ifndef GLG3D_TEXTURE_H
#define GLG3D_TEXTURE_H

#include "../../include/graphics3D.h"
#include "../../include/GLG3D/glheaders.h"

namespace G3D {

/**
 Abstraction of OpenGL textures.  This class can be used with raw OpenGL, 
 without RenderDevice or SDL.

 If you use TextureRef instead of Texture*, the texture memory will be
 garbage collected.

 If you enable texture compression, textures will be compressed on the fly.

 DIM_2D_RECT requires the NV_texture_rectangle extension.
 Texture compression requires the EXT_texture_compression_s3tc extions.

 <PRE>
  TextureRef texture = new Texture("Logo", "logo.jpg");

  ...
    
  GLint u = texture->getOpenGLTextureTarget();
  glEnable(u);
  glBindTexture(u, texture->getTextureID());
 </PRE>

  3D MIP Maps are not supported because gluBuild3DMipMaps is not in all GLU implementations.
 */
class Texture : public ReferenceCountedObject {
public:

    enum Channels        {LUMINANCE = 1, RGB = 3, RGBA = 4, ALPHA = 5};
    enum Dimension       {DIM_2D = 2, DIM_2D_RECT = 4};
    enum WrapMode        {TILE = 1, CLAMP = 0};
    enum InterpolateMode {TRILINEAR_MIPMAP = 3, BILINEAR_NO_MIPMAP = 2, NO_INTERPOLATION = 1};

private:

    std::string                     name;
    std::string                     filename;
    std::string                     alphaFilename;
    InterpolateMode                 interpolate;
    WrapMode                        wrap;
    Channels                        channels;
    Dimension                       dimension;
    int                             width;
    int                             height;
    int                             depth;
    bool                            compressed;

    /**
     Number of bits to allocate for each color channel.
     */
    int                             colorChannelBits;

    /**
     Number of bits for the alpha channel.
     */
    int                             alphaChannelBits;

    /**
     Loads filename (and maybe alphaFilename) and inserts
     the contents into d.
     */
    void getData(class Data& d);
	void createMipMapTexture(const class Data& d);
    void createTexture(const class Data& d);

    /**
     Converts the colorChannelBits and alphaChannelBits values
     into a GL internal texture format.  Used to set internalGLFormat.
     */
    GLenum getInternalGLFormat() const;

    /** OpenGL texture ID */
	unsigned int textureID;

    unsigned int externalFormat() const;

public:

    /**
     Returns a new OpenGL texture ID.
     */
    static unsigned int newGLTextureID();

    /**
     Construct a texture from an image file.  The internal* parameters specify how the
     texture should be stored in video memory.

     @param filename      Complete path to load file from.  Supports TGA, BMP, JPG formats.
     @param alphaFilename If a non-empty string, an alpha channel is created from the red channel
                          of this image.
     @param dimension     If DIM_3D, you must also specify the depth value

     @param internalAlphaChannelBits If -1, this will be set equal to the color channel bits when the 
     alphaFilename != ""
     @param combineMode   In the 4.00 release, this functionality will move to RenderDevice.
     @param internalCompress If the graphics card driver does not support compression, this will revert
     to the corresponding uncompressed format.
     @param depth 3D textures are stored in images such that there are depth images, each of size
            width x height, stacked on top of each other vertically.  Because the image stores the
            width and height, but not the depth, you must explicitly provide it.
     */
    Texture(
        const std::string&          name,
        const std::string&          filename,
        const std::string&          alphaFilename            = "",
        InterpolateMode             interpolate              = TRILINEAR_MIPMAP,
        WrapMode                    wrap                     = TILE,
        Dimension                   dimension                = DIM_2D,
        int                         internalColorChannelBits = 8,
        int                         internalAlphaChannelBits = -1,
        bool                        internalCompress         = false,
        int                         depth                    = 1);

    /**
     Construct from a series of bits already in memory.
     The internal* parameters specify how the
     texture should be stored in video memory.  It is safe to free pixels after the constructur
     returns-- the data is copied into video memory.

     @param alphaChannelBits If -1, this will be set equal to the color channel bits when the 
     channels value is RGBA.
     */
    Texture(
        const std::string&          name,
        int                         width,
        int                         height,
        int                         depth,
        uint8*                      pixels,
        InterpolateMode             interpolate   = TRILINEAR_MIPMAP,
        WrapMode                    wrap          = TILE,
        Channels                    channels      = RGB,
        Dimension                   dimension     = DIM_2D,
        int                         internalColorChannelBits = 8,
        int                         internalAlphaChannelBits = -1,
        bool                        internalCompress      = false);

    /**
     Construct an empty texture for future use with copyFromScreen() or
     construct a texture from an existing OpenGL texture ID (which will
     be released when this texture is garbage collected). 
     */
    Texture(
        const std::string&          name          = "",
        int                         width         = 1,
        int                         height        = 1,
        int                         depth         = 1,
        uint32                      textureID     = newGLTextureID(),
        InterpolateMode             interpolate   = TRILINEAR_MIPMAP,
        WrapMode                    wrap          = TILE,
        Dimension                   dimension     = DIM_2D,
        int                         internalColorChannelBits = 8,
        int                         internalAlphaChannelBits = -1,
        bool                        internalCompress      = false);    

    /**
     Copies data from screen into an existing texture (replacing whatever was
     previously there).  The dimensions must be powers of two or a texture 
     rectangle will be created (not supported on some cards).

     The (x, y) coordinates are in real screen pixels.  (0, 0) is the top left
     of the screen.

     The texture dimensions will be updated.  The previous wrap mode will be preserved.
     The interpolation mode will be preserved (unless it required a mipmap,
     in which case it will be set to BILINEAR_NO_MIPMAP).  The previous color depth
     and alpha depth will be preserved.  Texture compression is not supported for
     textures copied from the screen.

     @param useBackBuffer If true, the texture is created from the back buffer.
     If false, the texture is created from the front buffer.

     @param windowHeight renderDevice->getHeight().
     */
    void copyFromScreen(int x, int y, int width, int height, int windowHeight, bool useBackBuffer = true);

    /**
     When true, the y texture coordinate should be inverted.  If used with RenderDevice,
     this is performed automatically.
     */
    bool invertY;

    /**
     How much (texture) memory this texture occupies.
     */
    int sizeInMemory() const;

    inline unsigned int getOpenGLID() const {
        return textureID;
    }

    inline const int getTexelWidth() const {
        return width;
    }

    inline const int getTexelHeight() const {
        return height;
    }

    inline const int getTexelDepth() const {
        return depth;
    }

    inline const std::string& getName() const {
        return name;
    }

    inline const std::string& getFilename() const {
        return filename;
    }

    inline const std::string& getAlphaFilename() const {
        return alphaFilename;
    }

    inline InterpolateMode getInterpolateMode() const {
        return interpolate;
    }

    inline WrapMode getWrapMode() const {
        return wrap;
    }

    inline Channels getChannels() const {
        return channels;
    }
    
    inline Dimension getDimension() const {
        return dimension;
    }

    /**
     Reload this texture (if it came from a file).
     */
    void reload();

    /**
     Deallocates the OpenGL texture.
     */
    virtual ~Texture();

    /**
     The OpenGL texture target this binds (e.g. GL_TEXTURE_2D)
     */
    unsigned int getOpenGLTextureTarget() const;

};

typedef ReferenceCountedPointer<class Texture> TextureRef;

} // namespace

#endif
