/**
  @file CImage.cpp
  @author Morgan McGuire, morgan@graphics3d.com
  @created 2002-05-27
  @edited  2003-05-23
 */
#include "G3D/CImage.h"
#include "G3D/debug.h"

extern "C" {
#ifdef _WIN32
	#include "../IJG/jconfig.h"
	#include "../IJG/cdjpeg.h"
#else
	#include <jconfig.h>
	#include <jpeglib.h>
#endif
}
#include <sys/stat.h>
#include <assert.h>
#include <sys/types.h>

#ifndef _WIN32
/**
 This is used by the Windows bitmap I/O.
 */
static const int BI_RGB = 0;
#endif

const int jpegQuality = 90;

/**
 The IJG library needs special setup for compress/decompressing
 from memory.  These classes provide them.  
 
 The format of this class is defined by the IJG library; do not
 change it.
 */ 
class memory_destination_mgr {
public:
	struct jpeg_destination_mgr pub;
	JOCTET*                     buffer;
	int                         size;
	int                         count;
};

typedef memory_destination_mgr* mem_dest_ptr;

/**
 Signature dictated by IJG.
 */
static void init_destination (
    j_compress_ptr              cinfo) {

	mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = dest->size;
	dest->count=0;
}

/**
 Signature dictated by IJG.
 */
static boolean empty_output_buffer (
    j_compress_ptr              cinfo) {

	mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;

	dest->pub.next_output_byte = dest->buffer;
	dest->pub.free_in_buffer = dest->size;

	return TRUE;
}

/**
 Signature dictated by IJG.
 */
static void term_destination (
    j_compress_ptr              cinfo) {

	mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;
	dest->count = dest->size - dest->pub.free_in_buffer;
}

/**
 Signature dictated by IJG.
 */
static void jpeg_memory_dest (
    j_compress_ptr              cinfo,
    JOCTET*                     buffer,
    int                         size) {

	mem_dest_ptr dest;

	if (cinfo->dest == NULL) {
        // First time for this JPEG object; call the
        // IJG allocator to get space.
		cinfo->dest = (struct jpeg_destination_mgr*)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, 
                                        JPOOL_PERMANENT,
                       				    sizeof(memory_destination_mgr));
	}

	dest                            = (mem_dest_ptr) cinfo->dest;
	dest->size                      = size;
	dest->buffer                    = buffer;
	dest->pub.init_destination      = init_destination;
	dest->pub.empty_output_buffer   = empty_output_buffer;
	dest->pub.term_destination      = term_destination;
}
  
////////////////////////////////////////////////////////////////////////////////////////

#define INPUT_BUF_SIZE  4096

/**
 Structure dictated by IJG.
 */
class memory_source_mgr {
public:
    struct jpeg_source_mgr  pub;
	int                     source_size;
	unsigned char*          source_data;
	boolean                 start_of_data;
	JOCTET*                 buffer;
};


typedef memory_source_mgr* mem_src_ptr;


/**
 Signature dictated by IJG.
 */
static void init_source(
    j_decompress_ptr        cinfo) {

    mem_src_ptr src = (mem_src_ptr) cinfo->src;

    src->start_of_data = TRUE;
}


/**
 Signature dictated by IJG.
 */
static boolean fill_input_buffer(
    j_decompress_ptr        cinfo) {

	mem_src_ptr src = (mem_src_ptr) cinfo->src;

	size_t bytes_read = 0;

	if (src->source_size > INPUT_BUF_SIZE)
		bytes_read = INPUT_BUF_SIZE;
	else
		bytes_read = src->source_size;

	memcpy (src->buffer, src->source_data, bytes_read);

	src->source_data += bytes_read;
	src->source_size -= bytes_read;

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = bytes_read;
	src->start_of_data = FALSE;


	return TRUE;
}


/**
 Signature dictated by IJG.
 */
static void skip_input_data(
    j_decompress_ptr        cinfo,
    long                    num_bytes) {

	mem_src_ptr src = (mem_src_ptr)cinfo->src;

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			boolean s = fill_input_buffer(cinfo);
            debugAssert(s); (void)s;
		}

		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}


/**
 Signature dictated by IJG.
 */
static void term_source (
    j_decompress_ptr        cinfo) {
	// Intentionally empty
}


/**
 Signature dictated by IJG.
 */
static void jpeg_memory_src (
    j_decompress_ptr        cinfo,
    JOCTET*                 buffer,
    int                     size) {

	mem_src_ptr src;

	if (cinfo->src == NULL) {
        // First time for this JPEG object
		cinfo->src = (struct jpeg_source_mgr*)
			(*cinfo->mem->alloc_small)(
                (j_common_ptr) cinfo,
                JPOOL_PERMANENT,
				sizeof(memory_source_mgr));
		
        src = (mem_src_ptr)cinfo->src;
		
        src->buffer = (JOCTET*)
			(*cinfo->mem->alloc_small)(
                (j_common_ptr) cinfo,
                JPOOL_PERMANENT,
				INPUT_BUF_SIZE * sizeof(JOCTET));
	}

	src = (mem_src_ptr)cinfo->src;
	src->pub.init_source        = init_source;
	src->pub.fill_input_buffer  = fill_input_buffer;
	src->pub.skip_input_data    = skip_input_data;

    // use default method
	src->pub.resync_to_restart  = jpeg_resync_to_restart;
	src->pub.term_source        = term_source;
	src->source_data            = buffer;
	src->source_size            = size;

    // forces fill_input_buffer on first read
	src->pub.bytes_in_buffer    = 0;
    
    // until buffer loaded
    src->pub.next_input_byte = NULL; 
}

//////////////////////////////////////////////////////////////////////////////////////////////

namespace G3D {
   
void RGBtoRGBA(
    const uint8*    in,
    uint8*          out,
    int                     numPixels) {

    for (int i = 0; i < numPixels; ++i) {
        int i3 = i * 3;
        int i4 = i3 + i;

        out[i4 + 0] = in[i3 + 0]; 
        out[i4 + 1] = in[i3 + 1]; 
        out[i4 + 2] = in[i3 + 2]; 
        out[i4 + 3] = 255; 
    }
}


void RGBtoBGRA(
    const uint8*    in,
    uint8*          out,
    int                     numPixels) {

    for (int i = 0; i < numPixels; ++i) {
        int i3 = i * 3;
        int i4 = i3 + i;

        out[i4 + 2] = in[i3 + 0]; 
        out[i4 + 1] = in[i3 + 1]; 
        out[i4 + 0] = in[i3 + 2]; 
        out[i4 + 3] = 255; 
    }
}


void RGBtoBGR(
    const uint8*    in,
    uint8*          out,
    int                     numPixels) {

    for (int i = 0; i < numPixels; ++i) {
        int i3 = i * 3;

        out[i3 + 2] = in[i3 + 0]; 
        out[i3 + 1] = in[i3 + 1]; 
        out[i3 + 0] = in[i3 + 2];
    }
}



void RGBxRGBtoRGBA(
    const uint8*            colorRGB,
    const uint8*            alphaRGB,
    uint8*                  out,
    int                     numPixels) {

    for (int i = numPixels - 1; i >= 0; --i) {
        int i3 = i * 3;
        int i4 = i3 + i;

        out[i4 + 0] = colorRGB[i3 + 0];
        out[i4 + 1] = colorRGB[i3 + 1];
        out[i4 + 2] = colorRGB[i3 + 2];
        out[i4 + 3] = alphaRGB[i3 + 0];
    }
}

void RGBtoARGB(
    const uint8*            in,
    uint8*                  out,
    int                     numPixels) {

    for (int i = 0; i < numPixels; ++i) {
        int i3 = i * 3;
        int i4 = i3 + i;

        out[i4 + 0] = 255; 
        out[i4 + 1] = in[i3 + 0]; 
        out[i4 + 2] = in[i3 + 1]; 
        out[i4 + 3] = in[i3 + 2];
    }
}


void flipRGBVertical(
    const uint8*            in,
    uint8*                  out,
    int                     width,
    int                     height) {

    // Allocate a temp row so the operation
    // is still safe if in == out
    uint8* temp = (uint8*) malloc(width * 3);

    for (int i = 0; i < height / 2; ++i) {
        int oneRow = width * 3;
        int topOff = i * oneRow;
        int botOff = (height - i - 1) * oneRow;
        memcpy(temp,         in + topOff, oneRow);
        memcpy(out + topOff, in + botOff, oneRow);
        memcpy(out + botOff, temp,        oneRow);
    }

    free(temp);
}

////////////////////////////////////////////////////////////////////////////////////////

void CImage::encodeBMP(
    BinaryOutput&       out) {

    debugAssert(channels == 3);
    out.setEndian(G3D_LITTLE_ENDIAN);

    uint8 red;
    uint8 green;
    uint8 blue;
    int pixelBufferSize = width * height * 3;
    int fileHeaderSize = 14;
    int infoHeaderSize = 40;
    int BMScanWidth;
    int BMPadding;

    // First write the BITMAPFILEHEADER
    //
    //  WORD    bfType; 
    //  DWORD   bfSize; 
    //  WORD    bfReserved1; 
    //  WORD    bfReserved2; 
    //  DWORD   bfOffBits; 

    // Type
    out.writeUInt8('B');
    out.writeUInt8('M');

    // File size
    out.writeUInt32(fileHeaderSize + infoHeaderSize + pixelBufferSize);

    // Two reserved fields set to zero
    out.writeUInt16(0);
    out.writeUInt16(0);

	// The offset, in bytes, from the BITMAPFILEHEADER structure
    // to the bitmap bits.
    out.writeUInt32(infoHeaderSize + fileHeaderSize);

    // Now the BITMAPINFOHEADER
    //
    //  DWORD  biSize; 
    //  LONG   biWidth; 
    //  LONG   biHeight; 
    //  WORD   biPlanes; 
    //  WORD   biBitCount 
    //  DWORD  biCompression; 
    //  DWORD  biSizeImage; 
    //  LONG   biXPelsPerMeter; 
    //  LONG   biYPelsPerMeter; 
    //  DWORD  biClrUsed; 
    //  DWORD  biClrImportant; 

    // Size of the info header
    out.writeUInt32(infoHeaderSize);
 
    // Width and height of the image
	out.writeUInt32(width);
    out.writeUInt32(height);

    // Planes ("must be set to 1")
    out.writeUInt16(1);

    // BitCount and CompressionType
    out.writeUInt16(24);
    out.writeUInt32(BI_RGB);

    // Image size ("may be zero for BI_RGB bitmaps")
    out.writeUInt32(0);

    // biXPelsPerMeter
    out.writeUInt32(0);
    // biYPelsPerMeter
    out.writeUInt32(0);

    // biClrUsed
    out.writeUInt32(0); 

    // biClrImportant
    out.writeUInt32(0); 
    
    BMScanWidth = width * 3;

    if (BMScanWidth & 3) {
        BMPadding = 4 - (BMScanWidth & 3);
    } else {
        BMPadding = 0;
    }

    int hStart = height - 1;
    int hEnd   = -1;
    int hDir   = -1;
    int dest;

    // Write the pixel data
    for (int h = hStart; h != hEnd; h += hDir) {
        dest = channels * h * width;
        for (int w = 0; w < width; ++w) {

            red   = _byte[dest];
            green = _byte[dest + 1];
            blue  = _byte[dest + 2];

            out.writeUInt8(blue);
            out.writeUInt8(green);
            out.writeUInt8(red);

            dest += 3;
        }

        if (BMPadding) {
            out.skip(2);
        }
    }
}


void CImage::encodeTGA(
    BinaryOutput&       out) {

    out.setEndian(G3D_LITTLE_ENDIAN);

    // ID length
    out.writeUInt8(0);

    // Color map Type
    out.writeUInt8(0);

    // Type
    out.writeUInt8(2);

    // Color map
    out.skip(5);

    // x, y offsets
    out.writeUInt16(0);
    out.writeUInt16(0);

    // Width & height
    out.writeUInt16(width);
    out.writeUInt16(height);

    // Color depth
    out.writeUInt8(8 * channels);

    // Image descriptor
    out.writeUInt8(0);

    // Image ID (zero length)

    if (channels == 3) {
        // Pixels are upside down in BGR format.
        for (int y = height - 1; y >= 0; y--) {
            for (int x = 0; x < width; x++) {
                uint8* p = &(_byte[3 * (y * width + x)]);
                out.writeUInt8(p[2]);
                out.writeUInt8(p[1]);
                out.writeUInt8(p[0]);
            }
        }
    } else {
        // Pixels are upside down in BGRA format.
        for (int y = height - 1; y >= 0; y--) {
            for (int x = 0; x < width; x++) {
                uint8* p = &(_byte[4 * (y * width + x)]);
                out.writeUInt8(p[2]);
                out.writeUInt8(p[1]);
                out.writeUInt8(p[0]);
                out.writeUInt8(p[3]);
            }
        }
    }

    // Write "TRUEVISION-XFILE " 18 bytes from the end 
    // (with null termination)
    out.writeString("TRUEVISION-XFILE ");
}


void CImage::encodeJPEG(
    BinaryOutput&           out) {
    debugAssert(channels == 3);
    out.setEndian(G3D_LITTLE_ENDIAN);

    // Allocate and initialize a compression object
    jpeg_compress_struct    cinfo;
    jpeg_error_mgr          jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

    // Specify the destination for the compressed data.
    // (Overestimate the size)
    int buffer_size = width * height * 3 + 200;
	JOCTET* compressed_data = (JOCTET*)malloc(buffer_size);
	jpeg_memory_dest(&cinfo, compressed_data, buffer_size);

    // Set parameters for compression, including image size & colorspace
    cinfo.image_width       = width;
    cinfo.image_height      = height;

	// # of color components per pixel
    cinfo.input_components  = 3;
    
    // colorspace of input image
    cinfo.in_color_space    = JCS_RGB; 
    
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, jpegQuality, false);

    // Initialize the compressor
    jpeg_start_compress(&cinfo, TRUE);

    // Iterate over all scanlines from top to bottom
	// pointer to a single row
    JSAMPROW row_pointer[1];
    
    // JSAMPLEs per row in image_buffer
    int row_stride = cinfo.image_width * 3;
    while (cinfo.next_scanline < cinfo.image_height) {
	    row_pointer[0] = &(_byte[cinfo.next_scanline * row_stride]);
	    jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // Shut down the compressor
    jpeg_finish_compress(&cinfo);
    
    // Figure out how big the result was.
    int outLength = ((mem_dest_ptr)cinfo.dest)->count;

    //	Release the JPEG compression object
    jpeg_destroy_compress(&cinfo);

    // Copy into an appropriately sized output buffer.
    out.writeBytes(compressed_data, outLength);

    // Free the conservative buffer.
    free(compressed_data);
    compressed_data = NULL;
}


void CImage::decode(
    BinaryInput&        input,
    Format              format) {

    switch (format) {
    case JPEG:
        decodeJPEG(input);
        break;

    case TGA:
        decodeTGA(input);
        break;

    case BMP:
        decodeBMP(input);
        break;

    default:
        debugAssert(false);
    }

    debugAssert(width >= 0);
    debugAssert(height >= 0);
    debugAssert(channels == 3 || channels == 4);
    debugAssert(_byte != NULL);
}


void CImage::decodeTGA(
    BinaryInput&        input) {

    // This is a simple TGA loader that can handle uncompressed
    // truecolor TGA files (TGA type 2). 
    // Verify this is a TGA file by looking for the TRUEVISION tag.
    input.setPosition(input.size() - 18);
    std::string tag = input.readString(16);
    if (tag != "TRUEVISION-XFILE") {
        throw Error("Not a TGA file", input.getFilename());
    }

    input.reset();

    int IDLength     = input.readUInt8();
    int colorMapType = input.readUInt8();
    int imageType    = input.readUInt8();

    (void)colorMapType;
	
    // 2 is the type supported by this routine.
    if (imageType != 2) {
        throw Error("TGA images must be type 2 (Uncompressed truecolor)", input.getFilename());
    }
	
    // Color map specification
    input.skip(5);

    // Image specification

    // Skip x and y offsets
    input.skip(4); 

    width = input.readInt16();
    height = input.readInt16();

    int colorDepth = input.readUInt8();

    if ((colorDepth != 24) && (colorDepth != 32)) {
        throw Error("TGA files must be 24 or 32 bit.", input.getFilename());
    }

    if (colorDepth == 32) {
        channels = 4;
    } else {
        channels = 3;
    }

    // Image descriptor contains overlay data as well
    // as data indicating where the origin is
    int imageDescriptor = input.readUInt8();
    (void)imageDescriptor;
	
    // Image ID
    input.skip(IDLength);

    _byte = (uint8*)malloc(width * height * channels);
    debugAssert(_byte);
	
    // Pixel data
    int x;
    int y;

    if (channels == 3) {
        for (y = height - 1; y >= 0; y--) {
          for (x = 0; x < width; x++) {
            int b = input.readUInt8();
            int g = input.readUInt8();
            int r = input.readUInt8();
		    
            int i = (x + y * width) * 3;
            _byte[i + 0] = r;
            _byte[i + 1] = g;
            _byte[i + 2] = b;
          }
        }
    } else {
        for (y = height - 1; y >= 0; y--) {
          for (x = 0; x < width; x++) {
            int b = input.readUInt8();
            int g = input.readUInt8();
            int r = input.readUInt8();
            int a = input.readUInt8();
		    
            int i = (x + y * width) * 4;
            _byte[i + 0] = r;
            _byte[i + 1] = g;
            _byte[i + 2] = b;
            _byte[i + 3] = a;
          }
        }
    }
}


void CImage::decodeBMP(
    BinaryInput&            input) {

    // The BMP decoding uses these flags.
    static const uint16 PICTURE_NONE               = 0x0000;
    static const uint16 PICTURE_BITMAP             = 0x1000;

    // Compression Flags
    static const uint16 PICTURE_UNCOMPRESSED       = 0x0100;
    static const uint16 PICTURE_MONOCHROME         = 0x0001;
    static const uint16 PICTURE_4BIT               = 0x0002;
    static const uint16 PICTURE_8BIT               = 0x0004;
    static const uint16 PICTURE_16BIT              = 0x0008;
    static const uint16 PICTURE_24BIT              = 0x0010;
    static const uint16 PICTURE_32BIT              = 0x0020;

    // This is a simple BMP loader that can handle uncompressed BMP files.
    // Verify this is a BMP file by looking for the BM tag.
    input.reset();
    std::string tag = input.readString(2);
    if (tag != "BM") {
        throw Error("Not a BMP file", input.getFilename());
    }

    channels = 3;
	// Skip to the BITMAPINFOHEADER's width and height
	input.skip(16);

    width  = input.readUInt32();
    height = input.readUInt32();

	// Skip to the bit count and compression type
	input.skip(2);

    uint16 bitCount        = input.readUInt16();
    uint32 compressionType = input.readUInt32();

    uint8 red;
    uint8 green;
    uint8 blue;
    uint8 blank;

	// Only uncompressed bitmaps are supported by this code
    if ((int32)compressionType != BI_RGB) {
        throw Error("BMP images must be uncompressed", input.getFilename());
    }

    uint8* palette = NULL;

	// Create the palette if needed
    if (bitCount <= 8) {

        int numColors = 1 << bitCount;

        palette = (uint8*)malloc(numColors * 3);
        debugAssert(palette);

        // Skip past the end of the header to the palette info
        input.skip(20);

        int c;
        for(c = 0; c < numColors * 3; c += 3) {
            // Palette information in bitmaps is stored in BGR_ format.
            // That means it's blue-green-red-blank, for each entry.
            blue  = input.readUInt8();
            green = input.readUInt8();
            red   = input.readUInt8();
            blank = input.readUInt8();

            palette[c]     = red;
            palette[c + 1] = green;
            palette[c + 2] = blue;
        }
	}

    int hStart = 0;
    int hEnd   = 0;
    int hDir   = 0;

    if (height < 0) {
        height = -height;
        hStart = 0;
        hEnd   = height;
        hDir   = 1;
    } else {
        //height = height;
        hStart = height - 1;
        hEnd   = -1;
        hDir   = -1;
    }

    _byte = (uint8*)malloc(width * height * 3);
    debugAssert(_byte);

    int BMScanWidth;
    int BMPadding;
    uint8 BMGroup;
    uint8 BMPixel8;
    int currPixel;
    int dest;
    int flags = PICTURE_NONE;

    if (bitCount == 1) {
        // Note that this file is not necessarily grayscale, since it's possible
        // the palette is blue-and-white, or whatever. But of course most image
        // programs only write 1-bit images if they're black-and-white.
        flags = PICTURE_BITMAP | PICTURE_UNCOMPRESSED | PICTURE_MONOCHROME;

        // For bitmaps, each scanline is dword-aligned.
        BMScanWidth = (width + 7) >> 3;
        if (BMScanWidth & 3) {
            BMScanWidth += 4 - (BMScanWidth & 3);
        }

        // Powers of 2
        int pow2[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

        for (int h = hStart; h != hEnd; h += hDir) {

            currPixel = 0;
            dest = 3 * h * width;

            for (int w = 0; w < BMScanWidth; ++w) {

                BMGroup = input.readUInt8();

                // Now we read the pixels. Usually there are eight pixels per byte,
                // since each pixel is represented by one bit, but if the width
                // is not a multiple of eight, the last byte will have some bits
                // set, with the others just being extra. Plus there's the
                // dword-alignment padding. So we keep checking to see if we've
                // already read "width" number of pixels.
                for (int i = 7; i >= 0; --i) {
                    if (currPixel < width) {
                        int src  = 3 * ((BMGroup & pow2[i]) >> i);
                    
                        _byte[dest]     = palette[src];
                        _byte[dest + 1] = palette[src + 1];
                        _byte[dest + 2] = palette[src + 2];
                    
                        ++currPixel;
                        dest += 3;
                    }
                }
            }
        }

	} else if (bitCount == 4) {

        flags = PICTURE_BITMAP | PICTURE_UNCOMPRESSED | PICTURE_4BIT;

        // For bitmaps, each scanline is dword-aligned.
        int BMScanWidth = (width+1) >> 1;
        if (BMScanWidth & 3) {
            BMScanWidth += 4 - (BMScanWidth & 3);
        }

        for (int h = hStart; h != hEnd; h += hDir) {

            currPixel = 0;
            dest = 3 * h * width;

            for (int w = 0; w < BMScanWidth; w++) {

                BMGroup = input.readUInt8();
                int src[2];
                src[0] = 3 * ((BMGroup & 0xF0) >> 4);
                src[1] = 3 * (BMGroup & 0x0F);

                // Now we read the pixels. Usually there are two pixels per byte,
                // since each pixel is represented by four bits, but if the width
                // is not a multiple of two, the last byte will have only four bits
                // set, with the others just being extra. Plus there's the
                // dword-alignment padding. So we keep checking to see if we've
                // already read "Width" number of pixels.

                for (int i = 0; i < 2; ++i) {
                    if (currPixel < width) {
                        int tsrc  = src[i];
                    
                        _byte[dest]     = palette[tsrc];
                        _byte[dest + 1] = palette[tsrc + 1];
                        _byte[dest + 2] = palette[tsrc + 2];

                        ++currPixel;
                        dest += 3;
                    }
                }
            }
        }

	} else if (bitCount == 8) {
        
        flags = PICTURE_BITMAP | PICTURE_UNCOMPRESSED | PICTURE_8BIT;

        // For bitmaps, each scanline is dword-aligned.
        BMScanWidth = width;
        if (BMScanWidth & 3) {
            BMScanWidth += 4 - (BMScanWidth & 3);
        }

        for (int h = hStart; h != hEnd; h += hDir) {

            currPixel = 0;

            for (int w = 0; w < BMScanWidth; ++w) {

                BMPixel8 = input.readUInt8();

                if (currPixel < width) {
                    dest = 3 * ((h * width) + currPixel);
                    int src  = 3 * BMPixel8;
                    
                    _byte[dest]     = palette[src];
                    _byte[dest + 1] = palette[src + 1];
                    _byte[dest + 2] = palette[src + 2];
                    
                    ++currPixel;
                }
            }
        }

    } else if (bitCount == 16) {

    	throw Error("16-bit bitmaps not supported", input.getFilename());

	} else if (bitCount == 24) {
        input.skip(20);

        flags = PICTURE_BITMAP | PICTURE_UNCOMPRESSED | PICTURE_24BIT;

        // For bitmaps, each scanline is dword-aligned.
        BMScanWidth = width * 3;

        if (BMScanWidth & 3) {
            BMPadding = 4 - (BMScanWidth & 3);
        } else {
            BMPadding = 0;
        }

        for (int h = hStart; h != hEnd; h += hDir) {
            dest = 3 * h * width;
            for (int w = 0; w < width; ++w) {

                blue  = input.readUInt8();
                green = input.readUInt8();
                red   = input.readUInt8();

                _byte[dest]     = red;
                _byte[dest + 1] = green;
                _byte[dest + 2] = blue;

                dest += 3;
            }

            if (BMPadding) {
                input.skip(2);
            }
        }

	} else if (bitCount == 32) {

    	throw Error("32 bit bitmaps not supported", input.getFilename());
    
    } else {
        // We support all possible bit depths, so if the
        //     code gets here, it's not even a real bitmap.
        free(_byte);
        _byte = NULL;
        throw Error("Not a bitmap!", input.getFilename());
	}

}


void CImage::decodeJPEG(
    BinaryInput&                input) {

	struct jpeg_decompress_struct   cinfo;
	struct jpeg_error_mgr           jerr;
    int                             loc = 0;

    channels = 3;
    // We have to set up the error handler, in case initialization fails.
	cinfo.err = jpeg_std_error(&jerr);

    // Initialize the JPEG decompression object.
	jpeg_create_decompress(&cinfo);

	// Specify data source (eg, a file, for us, memory)
	jpeg_memory_src(&cinfo, const_cast<uint8*>(input.getCArray()), input.size());

	// Read the parameters with jpeg_read_header()
	jpeg_read_header(&cinfo, TRUE);

	// Set parameters for decompression
	// (We do nothing here since the defaults are fine)

	// Start decompressor
	jpeg_start_decompress(&cinfo);

	// Get and set the values of interest to this object
	this->width     = cinfo.output_width;
	this->height    = cinfo.output_height;

	// Prepare the pointer object for the pixel data
	_byte = (uint8*)malloc(width * height * 3);

 	// JSAMPLEs per row in output buffer
    int bpp         = cinfo.output_components;
    int row_stride  = cinfo.output_width * bpp;

	// Make a one-row-high sample array that will go away when done with image
    JSAMPARRAY temp = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    // Read data on a scanline by scanline basis
	while (cinfo.output_scanline < cinfo.output_height) {

        // We may need to adjust the output based on the
        // number of channels it has.
        switch (bpp) {
	    case 1:
            // Grayscale; decompress to temp.
    		jpeg_read_scanlines(&cinfo, temp, 1);

            // Expand to three channels
            {
                uint8* scan     = &(_byte[width * cinfo.output_scanline * 3]);
                uint8* endScan  = scan + (width * 3);
                uint8* t        = *temp;

                while (scan < endScan) {
                    uint8 value = t[0];

                    // Spread the value 3x.
                    scan[0] = value;
                    scan[1] = value;
                    scan[2] = value;

                    scan    += 3;
                    t       += 1;
                }
            }
		    break;

	    case 3:
            // Read directly into the array
            {
                // Need one extra level of indirection.
                uint8*     scan = _byte + loc;
                JSAMPARRAY ptr  = &scan;
    		    jpeg_read_scanlines(&cinfo, ptr, 1);
            }
		    break;

	    case 4:
            // RGBA; decompress to temp.
    		jpeg_read_scanlines(&cinfo, temp, 1);

            // Drop the 3rd channel
            {
                uint8* scan     = &(_byte[width * cinfo.output_scanline * 3]);
                uint8* endScan  = scan + width * 3;
                uint8* t        = *temp;

                while (scan < endScan) {
                    scan[0] = t[0];
                    scan[1] = t[1];
                    scan[2] = t[2];
                    
                    scan    += 3;
                    t       += 4;
                }
            }
		    break;

	    default:
		    throw Error("Unexpected number of channels.", input.getFilename());
	    }

		loc += row_stride;
	}

	// Finish decompression
	jpeg_finish_decompress(&cinfo);

	// Release JPEG decompression object
	jpeg_destroy_decompress(&cinfo);
}


CImage::Format CImage::resolveFormat(
    const std::string&  filename,
    const uint8*        data,
    int                 dataLen,
    Format              maybeFormat) {

    // Return the provided format if it is specified.
    if (maybeFormat != AUTODETECT) {
        return maybeFormat;
    }

    std::string extension;

    // Try to detect from the filename's extension
    if (filename.size() > 5) {
        // Search backwards for the "."
        for (int i = 1; i <= 5; ++i) {
            if (filename[filename.size() - i] == '.') {
                // Upper case
                extension = toUpper(filename.substr(filename.size() - i + 1));
                break;
            }
        }
    }

    Format tmp = stringToFormat(extension);
    if ((tmp != AUTODETECT) && (tmp != UNKNOWN)) {
        return tmp;
    }

    // Try and autodetect from the file itself by looking at the first
    // character.

    // We can't look at the character if it is null.
    debugAssert(data != NULL);              

    if ((dataLen > 0) && (data[0] == 'B')) {
        return BMP;
    }

    int j = memcmp(&data[6], "JFIF", 4);

    if ((dataLen > 11) && (data[0] == 0xFF) &&
        (memcmp(&data[6], "JFIF", 4) == 0)) {
        return JPEG;
    }

    if (memcmp(&data[dataLen - 18], "TRUEVISION-XFILE", 16) == 0) {
        return TGA;
    }

    return UNKNOWN;
}


CImage::CImage(
    const std::string&  filename,
    Format              format) : width(0), height(0), channels(0), _byte(NULL) {
    
    load(filename, format);
}


void CImage::load(
    const std::string&  filename,
    Format              format) {

    clear();

    BinaryInput b = BinaryInput(filename, G3D_LITTLE_ENDIAN);
    if (b.size() <= 0) {
        throw Error("File not found.", filename);
    }

    decode(b, resolveFormat(filename, b.getCArray(), b.size(), format));
}


CImage::CImage(
    const uint8*        data,
    int                 length,
    Format              format) {

    BinaryInput b = BinaryInput(data, length, G3D_LITTLE_ENDIAN);
    // It is safe to cast away the const because we
    // know we don't corrupt the data.

    decode(b, resolveFormat("", data, length, format));
}


CImage::CImage(
    int                 width,
    int                 height,
    int                 channels) {
    
    debugAssert(width >= 0);
    debugAssert(height >= 0);
    debugAssert(channels >= 1);

    this->width = width;
    this->height = height;
    this->channels = channels;
    _byte = (uint8*)calloc(width * height * channels, sizeof(uint8));
    debugAssert(isValidHeapPointer(_byte));
}


void CImage::_copy(
    const CImage&       other) {

    clear();

    width  = other.width;
    height = other.height;
    channels = other.channels;
    int s  = width * height * channels * sizeof(uint8);
    _byte  = (uint8*)malloc(s);
    debugAssert(isValidHeapPointer(_byte));
    memcpy(_byte, other._byte, s);
}


CImage::CImage(
    const CImage&        other) : _byte(NULL) {

    _copy(other);
}


CImage::~CImage() {
    clear();
}


void CImage::clear() {
    width = 0;
    height = 0;
    free(_byte);
    _byte = NULL;
}

CImage& CImage::operator=(const CImage& other) {
    _copy(other);
    return *this;
}


bool CImage::supportedFormat(
    const std::string& format) {

    return (stringToFormat(format) != UNKNOWN);
}


CImage::Format CImage::stringToFormat(
    const std::string& format) {

    std::string extension = toUpper(format);

    if ((extension == "JPG") || (extension == "JPEG")) {
        return JPEG;
    } else if (extension == "TGA") {
        return TGA;
    } else if (extension == "BMP") {
        return BMP;
    } else {
        return UNKNOWN;
    }
}


void CImage::save(
    const std::string& filename,
    Format             format) {

    BinaryOutput b = BinaryOutput(filename, G3D_LITTLE_ENDIAN);
    encode(resolveFormat(filename, NULL, 0, format), b);
    b.commit();
}


void CImage::encode(
    Format              format,
    uint8*&             outData,
    int&                outLength) {

    BinaryOutput out = BinaryOutput();

    encode(format, out);

    outData = (uint8*)malloc(out.size());
    debugAssert(outData);
    outLength = out.size();

    out.commit(outData);
}

void CImage::encode(
    Format              format,
    BinaryOutput&       out) {

    switch (format) {
    case JPEG:
        encodeJPEG(out);
        break;

    case BMP:
        encodeBMP(out);
        break;

    case TGA:
        encodeTGA(out);
        break;

    default:
        debugAssert(false);
    }
}

CImage CImage::insertRedAsAlpha(const CImage& alpha) const {
    debugAssert(alpha.width == width);
    debugAssert(alpha.height == height);

    CImage out(width, height, 4);

    for (int i = 0; i < width * height; ++i) {
        out.byte()[i * 4 + 0] = byte()[i * channels + 0];
        out.byte()[i * 4 + 1] = byte()[i * channels + 1];
        out.byte()[i * 4 + 2] = byte()[i * channels + 2];
        out.byte()[i * 4 + 3] = alpha.byte()[i * alpha.channels];
    }

    return out;
}


CImage CImage::stripAlpha() const {
    CImage out(width, height, 3);

    for (int i = 0; i < width * height; ++i) {
        out.byte()[i * 3 + 0] = byte()[i * channels + 0];
        out.byte()[i * 3 + 1] = byte()[i * channels + 1];
        out.byte()[i * 3 + 2] = byte()[i * channels + 2];
    }

    return out;
}

};

