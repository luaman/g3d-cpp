/**
  @file TextureFormat.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-05-23
  @edited  2003-05-23
*/

#ifndef GLG3D_TEXTUREFORMAT_H
#define GLG3D_TEXTUREFORMAT_H

#include "graphics3D.h"
#include "GLG3D/glheaders.h"

namespace G3D {

/**
 Used to describe texture formats to the G3D::Texture class.
 Don't construct these; use the static constants provided.
 */
class TextureFormat {
public:

    int                 numComponents;
    bool                compressed;
    
    /**
     The GL format equivalent to this one.
     */
    GLenum              OpenGLFormat;
    GLenum              OpenGLBaseFormat;

    int                 luminanceBits;

    /**
     Number of bits per texel storage for alpha values; Zero for compressed textures.
     */
    int                 alphaBits;
    
    /**
     Number of bits per texel storage for red values; Zero for compressed textures.
     */
    int                 redBits;

    /**
     Number of bits per texel storage for green values; Zero for compressed textures.
     */
    int                 greenBits;
    
    /**
     Number of bits per texel storage for blue values; Zero for compressed textures.
     */
    int                 blueBits;
    
    /**
     True if there is no alpha channel for this texture.
     */
    bool                opaque;

    /**
     This may be greater than the sum of the per-channel bits
     because graphics cards need to pad to the nearest 1, 2, or
     4 bytes.
     */
    int                 bitsPerTexel;

private:

    TextureFormat(
        int             _numComponents,
        bool            _compressed,
        GLenum          _glFormat,
        GLenum          _glBaseFormat,
        int             _luminanceBits,
        int             _alphaBits,
        int             _redBits,
        int             _greenBits,
        int             _blueBits,
        int             _bitsPerTexel,
        bool            _opaque) : 
        numComponents(_numComponents),
        compressed(_compressed),
        OpenGLFormat(_glFormat),
        OpenGLBaseFormat(_glBaseFormat),
        luminanceBits(_luminanceBits),
        alphaBits(_alphaBits),
        redBits(_redBits),
        greenBits(_greenBits),
        blueBits(_blueBits),
        bitsPerTexel(_bitsPerTexel),
        opaque(_opaque) {
    }

public:

    static const TextureFormat* L8;

    static const TextureFormat* A8;

    static const TextureFormat* LA8;

    static const TextureFormat* RGB5;

    static const TextureFormat* RGB5A1;

    static const TextureFormat* RGB8;

    static const TextureFormat* RGBA8;
    
    static const TextureFormat* RGB_DXT1;

    static const TextureFormat* RGBA_DXT1;

    static const TextureFormat* RGBA_DXT3;

    static const TextureFormat* RGBA_DXT5;

};

}

#endif
