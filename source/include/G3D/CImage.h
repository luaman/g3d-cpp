/**
  @file CImage.h

  See G3D::CImage for details.

  @cite JPEG compress/decompressor is the <A HREF="http://www.ijg.org/files/">IJG library</A>, used in accordance with their license.
  @cite JPG code by John Chisholm, using the IJG Library
  @cite TGA code by Morgan McGuire
  @cite BMP code by John Chishold, based on code by Edward "CGameProgrammer" Resnick <A HREF="mailto:cgp@gdnmail.net">mailto:cgp@gdnmail.net</A> at <A HREF="ftp://ftp.flipcode.com/cotd/LoadPicture.txt">ftp://ftp.flipcode.com/cotd/LoadPicture.txt</A>

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2002-05-27
  @edited  2003-05-23

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.

 */

#ifndef G3D_CIMAGE_H
#define G3D_CIMAGE_H

#include <string>
#include "G3D/Array.h"
#include "G3D/g3dmath.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/stringutils.h"
#include "G3D/Color3uint8.h"

namespace G3D {
   
/**
 @param in        RGB buffer of numPixels * 3 bytes
 @param out       Buffer of numPixels * 4 bytes
 @param numPixels Number of RGB pixels to convert
 */
void RGBtoRGBA(
    const uint8*            in,
    uint8*                  out,
    int                     numPixels);

void RGBtoBGR(
    const uint8*            in,
    uint8*                  out,
    int                     numPixels);

/**
 Win32 32-bit HDC format.
 */
void RGBtoBGRA(
    const uint8*            in,
    uint8*                  out,
    int                     numPixels);

void RGBtoARGB(
    const uint8*            in,
    uint8*                  out,
    int                     numPixels);


/**
 Uses the red channel of the second image as an alpha channel.
 */
void RGBxRGBtoRGBA(
    const uint8*            colorRGB,
    const uint8*            alphaRGB,
    uint8*                  out,
    int                     numPixels);
    
/**
 Flips the image along the vertical axis.
 Safe for in == out.
 */
void flipRGBVertical(
    const uint8*            in,
    uint8*                  out,
    int                     width,
    int                     height);


/**
 Interface to image compression & file formats.

  Supported formats (decode and encode): JPEG, TGA 24, TGA 32, BMP 1, BMP 4, BMP 8, BMP 24

  Sample usage:

  <PRE>
    #include "graphics3D.h"

    // Loading from disk:
    G3D::CImage im1 = G3D::Image("test.jpg");
    
    // Loading from memory:
    G3D::CImage im2 = G3D::CImage(data, length);

    // im.pixel is a pointer to RGB color data.  If you want
    // an alpha channel, call RGBtoRGBA or RGBtoARGB for
    // conversion.

    // Saving to memory:
    G3D::CImage im3 = G3D::CImage(width, height);
    // (Set the pixels of im3...) 
    uint8* data2;
    int    len2;
    im3.encode(G3D::CImage::JPEG, data2, len2);

    // Saving to disk
    im3.save("out.jpg");
  </PRE>
 */
class CImage {
private:
    uint8*                _byte;

public:

    class Error {
    public:
        Error(
            const std::string& reason,
            const std::string& filename = "") :
        reason(reason), filename(filename) {}
        
        std::string reason;
        std::string filename;
    };

    enum Format {JPEG, BMP, TGA, AUTODETECT, UNKNOWN};

    /**
     The number of channels; either 3 (RGB) or 4 (RGBA)
     */
    int                     channels;
    int                     width;
    int                     height;

    const uint8* byte() const {
        return _byte;
    }

    const Color3uint8* pixel3() const {
        return (Color3uint8*)_byte;
    }

    const Color4uint8* pixel4() const {
        return (Color4uint8*)_byte;
    }

    uint8* byte() {
        return _byte;
    }

    Color3uint8* pixel3() {
        return (Color3uint8*)_byte;
    }

    Color4uint8* pixel4() {
        return (Color4uint8*)_byte;
    }

private:

    void encodeBMP(
        BinaryOutput&       out);


    /**
     The TGA file will be either 24- or 32-bit depending
     on the number of channels.
     */
    void encodeTGA(
        BinaryOutput&       out);

    /**
     Converts this image into a JPEG
     */
    void encodeJPEG(
        BinaryOutput&       out);

    /**
     Decodes the buffer into this image.
     @format Guaranteed correct format.
     */
    void decode(
        BinaryInput&        input,
        Format              format);

    void decodeTGA(
        BinaryInput&        input);

    void decodeBMP(
        BinaryInput&        input);

    void decodeJPEG(
        BinaryInput&        input);

    /**
     Given [maybe] a filename, memory buffer, and [maybe] a format, 
     returns the most likely format of this file.
     */
    Format resolveFormat(
        const std::string&  filename,
        const uint8*        data,
        int                 dataLen,
        Format              maybeFormat);

    void _copy(
        const CImage&       other);

public:

    CImage() {
        width = height = channels = 0;
        _byte = NULL;
    }

    /**
     Load an encoded image from disk and decode it.
     Throws CImage::Error if something goes wrong.
     */
    CImage(
        const std::string&  filename,
        Format              format = AUTODETECT);

    /**
     Decodes an image stored in a buffer.
    */
    CImage(
        const unsigned char*data,
        int                 length,
        Format              format = AUTODETECT);

    /**
     Create an empty image of the given size.
     */
    CImage(
        int                 width,
        int                 height,
        int                 channels = 3);

    CImage(
        const CImage&       other);

    CImage& operator=(const CImage& other);

    /**
     Returns a new CImage that has 4 channels.  RGB is
     taken from this CImage and the alpha from the red
     channel of the second image.
     */ 
    CImage insertRedAsAlpha(const CImage& alpha) const;

    /**
     Returns a new CImage with 3 channels, removing
     the alpha channel if there is one.
     */
    CImage stripAlpha() const;

    /**
     Loads an image from disk (clearing the old one first).
     */
    void load(
        const std::string&  filename,
        Format              format = AUTODETECT);

    /**
     Frees memory and resets to a 0x0 image.
     */
    void clear();
        
    /**
     Deallocates the pixels.
     */
    virtual ~CImage();

    /**
      Returns true if format is supported.  Format
      should be an extension string (e.g. "BMP").
     */
    static bool supportedFormat(
        const std::string& format);

    /**
     Converts a string to an enum, returns UNKNOWN if not recognized.
     */
    static Format stringToFormat(
        const std::string& format);

    /**
     Encode and save to disk.
     */
    void save(
        const std::string& filename,
        Format             format = AUTODETECT);
   
    /**
     The caller must delete the returned buffer.
     */
    void encode(
        Format              format,
        uint8*&             outData,
        int&                outLength);

    /**
     Does not commit the BinaryOutput when done.
     */
    void CImage::encode(
        Format              format,
        BinaryOutput&       out);
};

}

#endif

