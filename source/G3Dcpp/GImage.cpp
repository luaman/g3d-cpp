/**
  @file GImage.cpp
  @author Morgan McGuire, morgan@graphics3d.com
  @created 2002-05-27
  @edited  2006-02-10
 */
#include "G3D/platform.h"
#include "G3D/GImage.h"
#include "G3D/debug.h"
#include "G3D/TextInput.h"
#include "G3D/TextOutput.h"
#include "G3D/stringutils.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"

/**
 Pick up libjpeg headers locally on Windows, but from the system on all other platforms.
*/
extern "C" {
#ifdef G3D_WIN32
	#include "../IJG/jconfig.h"
	#include "../IJG/cdjpeg.h"
#else
	#include <jconfig.h>
	#include <jpeglib.h>
#endif
}

#include "../png/png.h"
#include "G3D/Log.h"
#include <sys/stat.h>
#include <assert.h>
#include <sys/types.h>

#ifndef G3D_WIN32
/**
 This is used by the Windows bitmap I/O.
 */
static const int BI_RGB = 0;
#endif

const int jpegQuality = 96;

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
    (void)cinfo;
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
   
void GImage::RGBtoRGBA(
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


void GImage::RGBtoBGRA(
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


void GImage::RGBtoBGR(
    const uint8*    in,
    uint8*          out,
    int             numPixels) {

    for (int i = 0; i < numPixels; ++i) {
        int i3 = i * 3;

        int r = in[i3 + 0];
        int g = in[i3 + 1];
        int b = in[i3 + 2];

        out[i3 + 2] = r; 
        out[i3 + 1] = g; 
        out[i3 + 0] = b;
    }
}


void GImage::RGBxRGBtoRGBA(
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


void GImage::RGBtoARGB(
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


void GImage::flipRGBVertical(
    const uint8*            in,
    uint8*                  out,
    int                     width,
    int                     height) {

    
    // Allocate a temp row so the operation
    // is still safe if in == out
    uint8* temp = (uint8*)System::malloc(width * 3);
    alwaysAssertM(temp != NULL, "Out of memory"); 

    int oneRow = width * 3;
    int N = height / 2;

    // if height is an odd value, don't swap odd middle row
    for (int i = 0; i < N; ++i) {
        int topOff = i * oneRow;
        int botOff = (height - i - 1) * oneRow;
        System::memcpy(temp,         in + topOff, oneRow);
        System::memcpy(out + topOff, in + botOff, oneRow);
        System::memcpy(out + botOff, temp,        oneRow);
    }

    System::free(temp);
}


void GImage::flipRGBAVertical(
    const uint8*            in,
    uint8*                  out,
    int                     width,
    int                     height) {

    
    // Allocate a temp row so the operation
    // is still safe if in == out
    uint8* temp = (uint8*)System::malloc(width * 4);
    alwaysAssertM(temp != NULL, "Out of memory");

    int oneRow = width * 4;

    // if height is an odd value, don't swap odd middle row
    for (int i = 0; i < height / 2; ++i) {
        int topOff = i * oneRow;
        int botOff = (height - i - 1) * oneRow;
        System::memcpy(temp,         in + topOff, oneRow);
        System::memcpy(out + topOff, in + botOff, oneRow);
        System::memcpy(out + botOff, temp,        oneRow);
    }

    System::free(temp);
}

////////////////////////////////////////////////////////////////////////////////////////

void GImage::encodeBMP(
    BinaryOutput&       out) const {

    debugAssert(channels == 1 || channels == 3);
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

            if (channels == 3) {
                red   = _byte[dest];
                green = _byte[dest + 1];
                blue  = _byte[dest + 2];
            } else {
                red   = _byte[dest];
                green = _byte[dest];
                blue  = _byte[dest];
            }

            out.writeUInt8(blue);
            out.writeUInt8(green);
            out.writeUInt8(red);

            dest += channels;
        }

        if (BMPadding) {
            out.skip(2);
        }
    }
}


void GImage::encodeTGA(
    BinaryOutput&       out) const {

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
    if (channels == 3) {
        // 0 alpha bits
        out.writeUInt8(0);
    }
    else {
        // 8 alpha bits
        out.writeUInt8(8);
    }

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


void GImage::encodeJPEG(
    BinaryOutput&           out) const {

	if (channels != 3) {
		// Convert to three channel
		GImage tmp = *this;
		tmp.convertToRGB();
		tmp.encodeJPEG(out);
		return;
	}

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
    JOCTET* compressed_data = (JOCTET*)System::malloc(buffer_size);
	jpeg_memory_dest(&cinfo, compressed_data, buffer_size);


    cinfo.image_width       = width;
    cinfo.image_height      = height;

	// # of color components per pixel
    cinfo.input_components  = 3;

    // colorspace of input image
    cinfo.in_color_space    = JCS_RGB; 
    cinfo.input_gamma       = 1.0;
    
    // Set parameters for compression, including image size & colorspace
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, jpegQuality, false);
    cinfo.smoothing_factor = 0;
    cinfo.optimize_coding = TRUE;
//    cinfo.dct_method = JDCT_FLOAT;
    cinfo.dct_method = JDCT_ISLOW;
    cinfo.jpeg_color_space = JCS_YCbCr;

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
    System::free(compressed_data);
    compressed_data = NULL;
}

//libpng required function signature
void png_write_data(png_structp png_ptr,
    png_bytep data,
    png_size_t length) {

    debugAssert( png_ptr->io_ptr != NULL );
    debugAssert( data != NULL );

    ((BinaryOutput*)png_ptr->io_ptr)->writeBytes(data, length);
}

//libpng required function signature
void png_flush_data(
    png_structp png_ptr) {
    (void)png_ptr;
    //Do nothing.
}

//libpng required function signature
void png_error(
    png_structp png_ptr,
    png_const_charp error_msg) {
    
    (void)png_ptr;
    debugAssert( error_msg != NULL );
    throw GImage::Error(error_msg, "PNG"); 
}

//libpng required function signature
void png_warning(
    png_structp png_ptr,
    png_const_charp warning_msg) {

    (void)png_ptr;
    debugAssert( warning_msg != NULL );
    Log::common()->println(warning_msg);
}

void GImage::encodePNG(
    BinaryOutput&           out) const {

    debugAssert( channels == 1 || channels == 3 || channels == 4 );

    if (this->height > (int)(PNG_UINT_32_MAX/png_sizeof(png_bytep)))
        throw GImage::Error("Unsupported PNG height.", out.getFilename());

    out.setEndian(G3D_LITTLE_ENDIAN);

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, png_error, png_warning);
    if (!png_ptr)
        throw GImage::Error("Unable to initialize PNG encoder.", out.getFilename());

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        throw GImage::Error("Unable to initialize PNG encoder.", out.getFilename());
    }

    //setup libpng write handler so can use BinaryOutput
    png_set_write_fn(png_ptr, (void*)&out, png_write_data, png_flush_data);

    if (channels == 3) {
        png_set_IHDR(png_ptr, info_ptr, this->width, this->height, 8, PNG_COLOR_TYPE_RGB,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    }
    else if (channels == 4) {
        png_set_IHDR(png_ptr, info_ptr, this->width, this->height, 8, PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    }
    else if (channels == 1) {
        png_set_IHDR(png_ptr, info_ptr, this->width, this->height, 8, PNG_COLOR_TYPE_GRAY,
            PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    }
    else {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        throw GImage::Error("Unsupported number of channels for PNG.", out.getFilename());
    }

    png_color_8_struct sig_bit;
    sig_bit.red = 8;
    sig_bit.green = 8;
    sig_bit.blue = 8;
    if (channels == 4)
        sig_bit.alpha = 8;
    else
        sig_bit.alpha = 0;
    png_set_sBIT(png_ptr, info_ptr, &sig_bit);

    //write the png header
    png_write_info(png_ptr, info_ptr);

    png_bytepp row_pointers = new png_bytep[this->height];

    for (int i=0; i < this->height; ++i) {
        row_pointers[i] = (png_bytep)&this->_byte[(this->width * this->channels * i)];
    }

    png_write_image(png_ptr, row_pointers);

    png_write_end(png_ptr, info_ptr);

    delete[] row_pointers;

    png_destroy_write_struct(&png_ptr, &info_ptr);
}

void GImage::encodePPMASCII(
    BinaryOutput&       out) const {

    debugAssert(channels == 3);

    TextOutput::Options ppmOptions;
    ppmOptions.convertNewlines = false;
    ppmOptions.numColumns = 70;
    ppmOptions.wordWrap = TextOutput::Options::WRAP_WITHOUT_BREAKING;
    TextOutput ppm(ppmOptions);
    // Always write out a full-color ppm
    ppm.printf("P3\n%d %d\n255\n", width, height);
    
    const Color3uint8* c = this->pixel3();
    for (uint32 i = 0; i < (uint32)(width * height); ++i) {
        ppm.printf("%d %d %d%c", c[i].r, c[i].g, c[i].b, 
            ((i % ((width * 3) - 1)) == 0) ?
            '\n' : ' '); 
    }

    out.writeString(ppm.commitString());
}

void GImage::encodePPM(
    BinaryOutput&       out) const {

    // http://netpbm.sourceforge.net/doc/ppm.html
    debugAssert(channels == 3);

    std::string header = format("P6 %d %d 255 ", width, height);

    out.writeBytes(header.c_str(), header.size());

    out.writeBytes(this->pixel3(), width * height * 3);
}


void GImage::decode(
    BinaryInput&        input,
    Format              format) {

    switch (format) {
    case PPM_ASCII:
        decodePPMASCII(input);
        break;

    case PPM:
        decodePPM(input);
        break;

    case PNG:
        decodePNG(input);
        break;

    case JPEG:
        decodeJPEG(input);
        break;

    case TGA:
        decodeTGA(input);
        break;

    case BMP:
        decodeBMP(input);
        break;

    case ICO:
        decodeICO(input);
        break;

    case PCX:
        decodePCX(input);
        break;

    default:
        debugAssert(false);
    }

    debugAssert(width >= 0);
    debugAssert(height >= 0);
    debugAssert(channels == 1 || channels == 3 || channels == 4);
    debugAssert(_byte != NULL);
}


void GImage::decodeTGA(
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

    _byte = (uint8*)System::malloc(width * height * channels);
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


void GImage::decodeICO(
    BinaryInput&            input) {

	// Header
	uint16 r = input.readUInt16();
	debugAssert(r == 0);
	r = input.readUInt16();
	debugAssert(r == 1);

	// Read the number of icons, although we'll only load the
	// first one.
	int count = input.readUInt16();

	channels = 4;

	debugAssert(count > 0);

    const uint8* headerBuffer = input.getCArray() + input.getPosition();
    int maxWidth = 0, maxHeight = 0;
    int maxHeaderNum = 0;
    for (int currentHeader = 0; currentHeader < count; ++currentHeader) {
        
        const uint8* curHeaderBuffer = headerBuffer + (currentHeader * 16);
        int tmpWidth = curHeaderBuffer[0];
        int tmpHeight = curHeaderBuffer[1];
        // Just in case there is a non-square icon, checking area
        if ((tmpWidth * tmpHeight) > (maxWidth * maxHeight)) {
            maxWidth = tmpWidth;
            maxHeight = tmpHeight;
            maxHeaderNum = currentHeader;
        }
    }

    input.skip(maxHeaderNum * 16);

	width = input.readUInt8();
	height = input.readUInt8();
	int numColors = input.readUInt8();
	
    _byte = (uint8*)System::malloc(width * height * channels);
    debugAssert(_byte);

	// Bit mask for packed bits
	int mask = 0;

	int bitsPerPixel = 8;

	switch (numColors) {
	case 2:
		mask      = 0x01;
		bitsPerPixel = 1;
		break;

	case 16:
		mask      = 0x0F;
		bitsPerPixel = 4;
		break;

	case 0:
		numColors = 256;
		mask      = 0xFF;
		bitsPerPixel = 8;
		break;
    default:
    	throw Error("Unsupported ICO color count.", input.getFilename());
	}

	input.skip(5);
	// Skip 'size' unused
        input.skip(4);

	int offset = input.readUInt32();

	// Skip over any other icon descriptions
    input.setPosition(offset);

	// Skip over bitmap header; it is redundant
	input.skip(40);

	Array<Color4uint8> palette;
    palette.resize(numColors, true);
	for (int c = 0; c < numColors; ++c) {
		palette[c].b = input.readUInt8();
		palette[c].g = input.readUInt8();
		palette[c].r = input.readUInt8();
		palette[c].a = input.readUInt8();
	}

	// The actual image and mask follow

	// The XOR Bitmap is stored as 1-bit, 4-bit or 8-bit uncompressed Bitmap 
	// using the same encoding as BMP files. The AND Bitmap is stored in as 
	// 1-bit uncompressed Bitmap.
	// 
	// Pixels are stored bottom-up, left-to-right. Pixel lines are padded 
	// with zeros to end on a 32bit (4byte) boundary. Every line will have the 
	// same number of bytes. Color indices are zero based, meaning a pixel color 
	// of 0 represents the first color table entry, a pixel color of 255 (if there
	// are that many) represents the 256th entry.
/*
	int bitsPerRow  = width * bitsPerPixel;
	int bytesPerRow = iCeil((double)bitsPerRow / 8);
	// Rows are padded to 32-bit boundaries
	bytesPerRow += bytesPerRow % 4;

	// Read the XOR values into the color channel
	for (int y = height - 1; y >= 0; --y) {
		int x = 0;
		// Read the row
		for (int i = 0; i < bytesPerRow; ++i) {
			uint8 byte = input.readUInt8();
			for (int j = 0; (j < 8) && (x < width); ++x, j += bitsPerPixel) {
				int bit = ((byte << j) >> (8 - bitsPerPixel)) & mask;
				pixel4(x, y) = colorTable[bit];
			}
		}
	}
*/
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

    int BMScanWidth;
    uint8 BMGroup;
    uint8 BMPixel8;
    int currPixel;
    int dest;

    if (bitsPerPixel == 1) {
        // Note that this file is not necessarily grayscale, since it's possible
        // the palette is blue-and-white, or whatever. But of course most image
        // programs only write 1-bit images if they're black-and-white.

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
                        int src  = ((BMGroup & pow2[i]) >> i);
                    
                        _byte[dest]     = palette[src].r;
                        _byte[dest + 1] = palette[src].g;
                        _byte[dest + 2] = palette[src].b;
                    
                        ++currPixel;
                        dest += 4;
                    }
                }
            }
        }

	} else if (bitsPerPixel == 4) {

        // For bitmaps, each scanline is dword-aligned.
        int BMScanWidth = (width+1) >> 1;
        if (BMScanWidth & 3) {
            BMScanWidth += 4 - (BMScanWidth & 3);
        }

        for (int h = hStart; h != hEnd; h += hDir) {

            currPixel = 0;
            dest = 4 * h * width;

            for (int w = 0; w < BMScanWidth; w++) {

                BMGroup = input.readUInt8();
                int src[2];
                src[0] = ((BMGroup & 0xF0) >> 4);
                src[1] = (BMGroup & 0x0F);

                // Now we read the pixels. Usually there are two pixels per byte,
                // since each pixel is represented by four bits, but if the width
                // is not a multiple of two, the last byte will have only four bits
                // set, with the others just being extra. Plus there's the
                // dword-alignment padding. So we keep checking to see if we've
                // already read "Width" number of pixels.

                for (int i = 0; i < 2; ++i) {
                    if (currPixel < width) {
                        int tsrc  = src[i];
                    
                        _byte[dest]     = palette[tsrc].r;
                        _byte[dest + 1] = palette[tsrc].g;
                        _byte[dest + 2] = palette[tsrc].b;

                        ++currPixel;
                        dest += 4;
                    }
                }
            }
        }

	} else if (bitsPerPixel == 8) {
        
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
                    dest = 4 * ((h * width) + currPixel);
                    int src  = BMPixel8;
                    
                    _byte[dest]     = palette[src].r;
                    _byte[dest + 1] = palette[src].g;
                    _byte[dest + 2] = palette[src].b;
                    
                    ++currPixel;
                }
            }
        }
    }

	// Read the mask into the alpha channel
	int bitsPerRow  = width;
	int bytesPerRow = iCeil((double)bitsPerRow / 8);

    // For bitmaps, each scanline is dword-aligned.
    //BMScanWidth = (width + 1) >> 1;
    if (bytesPerRow & 3) {
        bytesPerRow += 4 - (bytesPerRow & 3);
    }
    
    for (int y = height - 1; y >= 0; --y) {
		int x = 0;
		// Read the row
		for (int i = 0; i < bytesPerRow; ++i) {
			uint8 byte = input.readUInt8();
			for (int j = 0; (j < 8) && (x < width); ++x, ++j) {
				int bit = (byte >> (7 - j)) & 0x01;
				pixel4(x, y).a = (1 - bit) * 0xFF;
			}
		}
	}

}


void GImage::decodeBMP(
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

    (void)PICTURE_16BIT;
    (void)PICTURE_32BIT;

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

        // Skip to the palette color count in the header
        input.skip(12);

        int numColors = input.readUInt32();

        palette = (uint8*)System::malloc(numColors * 3);
        debugAssert(palette);

        // Skip past the end of the header to the palette info
        input.skip(4);

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

    _byte = (uint8*)System::malloc(width * height * 3);
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

        System::free(_byte);
        _byte = NULL;
        System::free(palette); 
        palette = NULL;
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

        System::free(_byte);
        _byte = NULL;
        System::free(palette); 
        palette = NULL;
    	throw Error("32 bit bitmaps not supported", input.getFilename());
    
    } else {
        // We support all possible bit depths, so if the
        //     code gets here, it's not even a real bitmap.
        System::free(_byte);
        _byte = NULL;
        throw Error("Not a bitmap!", input.getFilename());
	}

    System::free(palette); 
    palette = NULL;
}


void GImage::decodeJPEG(
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
    _byte = (uint8*)System::malloc(width * height * 3);

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
                uint8* scan     = &(_byte[loc * 3]);
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
                uint8* scan     = &(_byte[loc * 3]);
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
		    throw Error("Unexpected number6 of channels.", input.getFilename());
	    }

		loc += row_stride;
	}

	// Finish decompression
	jpeg_finish_decompress(&cinfo);

	// Release JPEG decompression object
	jpeg_destroy_decompress(&cinfo);
}



void GImage::decodePCX(
    BinaryInput&                input) {

    uint8  manufacturer = input.readUInt8();
    uint8  version      = input.readUInt8();
    uint8  encoding     = input.readUInt8();
    uint8  bitsPerPixel = input.readUInt8();

    uint16 xmin         = input.readUInt16();
    uint16 ymin         = input.readUInt16();
    uint16 xmax         = input.readUInt16();
    uint16 ymax         = input.readUInt16();

    uint16 horizDPI     = input.readUInt16();
    uint16 vertDPI      = input.readUInt16();

    Color3uint8 colorMap[16];
    input.readBytes(48, colorMap);

    input.skip(1);

    uint8  planes       = input.readUInt8();
    uint16 bytesPerLine = input.readUInt16();
    uint16 paletteType  = input.readUInt16();
    input.skip(4 + 54);

    (void)bytesPerLine;

    width  = xmax - xmin + 1;
    height = ymax - ymin + 1;
    channels = 3;

    if ((manufacturer != 0x0A) || (encoding != 0x01)) {
        throw GImage::Error("PCX file is corrupted", input.getFilename());
    }

    (void)version;
    (void)vertDPI;
    (void)horizDPI;

    if ((bitsPerPixel != 8) || ((planes != 1) && (planes != 3))) {
        throw GImage::Error("Only 8-bit paletted and 24-bit PCX files supported.", input.getFilename());
    }

	// Prepare the pointer object for the pixel data
    _byte = (uint8*)System::malloc(width * height * 3);

    if ((paletteType == 1) && (planes == 3)) {

        Color3uint8* pixel = pixel3();

        // Iterate over each scan line
        for (int row = 0; row < height; ++row) {
            // Read each scan line once per plane
            for (int plane = 0; plane < planes; ++plane) {
                int p = row * width;
                int p1 = p + width;
                while (p < p1) {
                    uint8 value = input.readUInt8();
                    int length = 1;
            
                    if (value >= 192) {
                        // This is the length, not the value.  Mask off
                        // the two high bits and read the true index.
                        length = value & 0x3F;
                        value = input.readUInt8();
                    }

                    // Set the whole run
                    for (int i = length - 1; i >= 0; --i, ++p) {
                        debugAssert(p < width * height);
                        pixel[p][plane] = value;
                    }
                }
            }
        }

    } else if (planes == 1) {

        Color3uint8 palette[256];

        int imageBeginning   = input.getPosition();
        int paletteBeginning = input.getLength() - 769;

        input.setPosition(paletteBeginning);

        uint8 dummy = input.readUInt8();

        if (dummy != 12) {
            Log::common()->println("\n*********************");
            Log::common()->printf("Warning: Corrupted PCX file (palette marker byte was missing) \"%s\"\nLoading anyway\n\n", input.getFilename().c_str());
        }

        input.readBytes(sizeof(palette), palette);
        input.setPosition(imageBeginning);
        

        Color3uint8* pixel = pixel3();
        
        // The palette indices are run length encoded.
        int p = 0;
        while (p < width * height) {
            uint8 index  = input.readUInt8();
            uint8 length = 1;

            if (index >= 192) {
                // This is the length, not the index.  Mask off
                // the two high bits and read the true index.
                length = index & 0x3F;
                index  = input.readUInt8();
            }

            Color3uint8 color = palette[index];

            // Set the whole run
            for (int i = length - 1; i >= 0; --i, ++p) {
                debugAssert(p < width * height);
                pixel[p] = color;
            }

        }

    } else {
        throw GImage::Error("Unsupported PCX file type.", input.getFilename());
    }
}

//libpng required function signature
void png_read_data(
    png_structp png_ptr,
    png_bytep data,
    png_size_t length) {


    debugAssert( png_ptr->io_ptr != NULL );
    debugAssert( length >= 0 );
    debugAssert( data != NULL );

    ((BinaryInput*)png_ptr->io_ptr)->readBytes(length, data);
}

void GImage::decodePNG(
    BinaryInput&        input) {

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, png_error, png_warning);
    if (!png_ptr)
        throw GImage::Error("Unable to initialize PNG decoder.", input.getFilename());

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        throw GImage::Error("Unable to initialize PNG decoder.", input.getFilename());
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
        throw GImage::Error("Unable to initialize PNG decoder.", input.getFilename());
    }

    //now that the libpng structures are setup, change the error handlers and read routines
    //to use G3D functions so that BinaryInput can be used.

    png_set_read_fn(png_ptr, (png_voidp)&input, png_read_data);
    
    //read in sequentially so that three copies of the file are not in memory at once
    png_read_info(png_ptr, info_ptr);

    uint32 png_width, png_height;
    int bit_depth, color_type, interlace_type;
    //this will validate the data it extracts from info_ptr
    png_get_IHDR(png_ptr, info_ptr, (png_uint_32*)&png_width, (png_uint_32*)&png_height, &bit_depth, &color_type,
       &interlace_type, int_p_NULL, int_p_NULL);

    if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        throw GImage::Error("Unsupported PNG color type - PNG_COLOR_TYPE_GRAY_ALPHA.", input.getFilename());
    }

    this->width = png_width;
    this->height = png_height;

    //swap bytes of 16 bit files to least significant byte first
    png_set_swap(png_ptr);

    png_set_strip_16(png_ptr);

    //Expand paletted colors into true RGB triplets
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
    }

    //Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
        png_set_gray_1_2_4_to_8(png_ptr);
    }

    //Expand paletted or RGB images with transparency to full alpha channels
    //so the data will be available as RGBA quartets.
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }

    // Fix sub-8 bit_depth to 8bit
    if (bit_depth < 8) {
        png_set_packing(png_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_RGBA) {
        this->channels = 4;
        this->_byte = (uint8*)System::malloc(width * height * 4);
    } else if ((color_type == PNG_COLOR_TYPE_RGB) || 
               (color_type == PNG_COLOR_TYPE_PALETTE)) {
        this->channels = 3;
        this->_byte = (uint8*)System::malloc(width * height * 3);
    } else if (color_type == PNG_COLOR_TYPE_GRAY) {
        this->channels = 1;
        this->_byte = (uint8*)System::malloc(width * height);
    } else {
        throw GImage::Error("Unsupported PNG bit-depth or type.", input.getFilename());
    }

    //since we are reading row by row, required to handle interlacing
    uint32 number_passes = png_set_interlace_handling(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    for (uint32 pass = 0; pass < number_passes; ++pass) {
        for (uint32 y = 0; y < (uint32)height; ++y) {
            png_bytep rowPointer = &this->_byte[width * this->channels * y]; 
            png_read_rows(png_ptr, &rowPointer, png_bytepp_NULL, 1);
        }
    }

    png_read_end(png_ptr, info_ptr);

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
}

void GImage::decodePPMASCII(
    BinaryInput&        input) {

    int ppmWidth;
    int ppmHeight;

    double maxColor;

    // Create a TextInput object to parse ascii format
    // Mixed binary/ascii formats will require more 

    const std::string inputStr = input.readString();

    TextInput::Options ppmOptions;
    ppmOptions.cppComments = false;
    ppmOptions.otherCommentCharacter = '#';
    ppmOptions.signedNumbers = true;
    ppmOptions.singleQuotedStrings = false;

    TextInput ppmInput(TextInput::FROM_STRING, inputStr, ppmOptions);

    //Skip first line in header P#
    std::string ppmType = ppmInput.readSymbol();

    ppmWidth = (int)ppmInput.readNumber();
    ppmHeight = (int)ppmInput.readNumber();

    // Everything but a PBM will have a max color value
    if (ppmType != "P2") {
        maxColor = ppmInput.readNumber();
    } else {
        maxColor = 255;
    }

    if ((ppmWidth < 0) ||
        (ppmHeight < 0) ||
        (maxColor <= 0)) {
        throw GImage::Error("Invalid PPM Header.", input.getFilename());
    }

    // I don't think it's proper to scale values less than 255
    if (maxColor <= 255.0) {
        maxColor = 255.0;
    }

    this->width = ppmWidth;
    this->height = ppmHeight;
    this->channels = 3;
    // always scale down to 1 byte per channel
    this->_byte = (uint8*)System::malloc(width * height * 3);

    // Read in the image data.  I am not validating if the values match the maxColor
    // requirements.  I only scale if needed to fit within the byte available.
    for (uint32 i = 0; i < (uint32)(width * height); ++i) {
        // read in color and scale to max pixel defined in header
        // A max color less than 255 might need to be left alone and not scaled.
        Color3uint8& curPixel = *(this->pixel3() + i);

        if (ppmType == "P3") {
            curPixel.r = (uint8)(ppmInput.readNumber() * (255.0 / maxColor));
            curPixel.g = (uint8)(ppmInput.readNumber() * (255.0 / maxColor));
            curPixel.b = (uint8)(ppmInput.readNumber() * (255.0 / maxColor));
        } else if (ppmType == "P2") {
            uint8 pixel = (uint8)(ppmInput.readNumber() * (255.0 / maxColor));
            curPixel.r = pixel;
            curPixel.g = pixel;
            curPixel.b = pixel;
        } else if (ppmType == "P1") {
            int pixel = (uint8)(ppmInput.readNumber() * maxColor);
            curPixel.r = pixel;
            curPixel.g = pixel;
            curPixel.b = pixel;
        }
    }
}

/** Consumes whitespace up to and including a number, but not the following character */
static int scanUInt(BinaryInput& input) {
    char c = input.readUInt8();
    while (isWhiteSpace(c)) {
        c = input.readUInt8();
    }

    std::string s;
    s += c;
    c = input.readUInt8();
    while (!isWhiteSpace(c)) {
        s += c;
        c = input.readUInt8();
    }

    // Back up one to avoid consuming the last character
    input.setPosition(input.getPosition() - 1);

    int x;
    sscanf(s.c_str(), "%d", &x);
    return x;
}

void GImage::decodePPM(
    BinaryInput&        input) {

    char head[2];
    int w, h;

    input.readBytes(head, 2);
    if (head[0] != 'P' || head[1] != '6') {
        throw GImage::Error("Invalid PPM Header.", input.getFilename());
    }

    w = scanUInt(input);
    h = scanUInt(input);

    // Skip the max color specifier
    scanUInt(input);

    if ((w < 0) ||
        (h < 0) ||
        (w > 100000) ||
        (h > 100000)) {
        throw GImage::Error("Invalid PPM size in header.", input.getFilename());
    }

    // Trailing whitespace
    input.readUInt8();

    resize(w, h, 3);

    input.readBytes(_byte, width * height * 3);
}


GImage::Format GImage::resolveFormat(
    const std::string&  filename,
    const uint8*        data,
    int                 dataLen,
    Format              maybeFormat) const {

    // Return the provided format if it is specified.
    if (maybeFormat != AUTODETECT) {
        return maybeFormat;
    }

    std::string extension;

    // Try to detect from the filename's extension
    if (filename.size() >= 5) {
        int n = iMax(filename.size() - 1, 5);
        // Search backwards for the "."
        for (int i = 1; i <= n; ++i) {
            if (filename[filename.size() - i] == '.') {
                // Upper case
                extension = toUpper(filename.substr(filename.size() - i + 1));
                break;
            }
        }
    }

    if (extension == "PPM") {
        // There are two PPM formats; we handle them differently
        if (dataLen > 3) {
            if (!memcmp(data, "P6", 2)) {
                return PPM;
            } else {
                return PPM_ASCII;
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

    if ((dataLen > 3) && (!memcmp(data, "P3", 2) || !memcmp(data, "P2", 2) || !memcmp(data, "P1", 2))) {
        return PPM_ASCII;
    }

    if ((dataLen > 3) && !memcmp(data, "P6", 2)) {
        return PPM;
    }

    if (dataLen > 8) {
        if (!png_sig_cmp((png_bytep)data, 0, 8))
            return PNG;
    }

    if ((dataLen > 0) && (data[0] == 'B')) {
        return BMP;
    }

    if (dataLen > 10) {
        if ((dataLen > 11) && (data[0] == 0xFF) &&
            (memcmp(&data[6], "JFIF", 4) == 0)) {
            return JPEG;
        }
    }

    if (dataLen > 40) {
        if (memcmp(&data[dataLen - 18], "TRUEVISION-XFILE", 16) == 0) {
            return TGA;
        }
    }

    if ((dataLen > 4) && (data[0] == 0) && (data[1] == 0) && (data[2] == 0) && (data[3] == 1)) {
        return ICO;
    }

    if ((dataLen > 0) && (data[0] == 10)) {
        return PCX;
    }

    return UNKNOWN;
}


GImage::GImage(
    const std::string&  filename,
    Format              format) : 
    _byte(NULL), 
    width(0),
    height(0), 
    channels(0){
    
    load(filename, format);
}


void GImage::load(
    const std::string&  filename,
    Format              format) {

    clear();

    try {
        BinaryInput b(filename, G3D_LITTLE_ENDIAN);
        if (b.size() <= 0) {
            throw Error("File not found.", filename);
        }

        decode(b, resolveFormat(filename, b.getCArray(), b.size(), format));
    } catch (const std::string& error) {
        throw Error(error, filename);
    }
}


GImage::GImage(
    const uint8*        data,
    int                 length,
    Format              format) {

    BinaryInput b(data, length, G3D_LITTLE_ENDIAN);
    // It is safe to cast away the const because we
    // know we don't corrupt the data.

    decode(b, resolveFormat("", data, length, format));
}


GImage::GImage(
    int                 width,
    int                 height,
    int                 channels) {
    
    _byte = NULL;
    resize(width, height, channels);
}


void GImage::resize(
    int                 width,
    int                 height,
    int                 channels) {
    debugAssert(width >= 0);
    debugAssert(height >= 0);
    debugAssert(channels >= 1);

    clear();

    this->width = width;
    this->height = height;
    this->channels = channels;
    size_t sz = width * height * channels;

    _byte = (uint8*)System::calloc(sz, sizeof(uint8));
    debugAssert(isValidHeapPointer(_byte));
}


void GImage::_copy(
    const GImage&       other) {

    clear();

    width  = other.width;
    height = other.height;
    channels = other.channels;
    int s  = width * height * channels * sizeof(uint8);
    _byte  = (uint8*)System::malloc(s);
    debugAssert(isValidHeapPointer(_byte));
    memcpy(_byte, other._byte, s);
}


GImage::GImage(
    const GImage&        other) : _byte(NULL) {

    _copy(other);
}


GImage::~GImage() {
    clear();
}


void GImage::clear() {
    width = 0;
    height = 0;
    System::free(_byte);
    _byte = NULL;
}


GImage& GImage::operator=(const GImage& other) {
    _copy(other);
    return *this;
}


bool GImage::copySubImage(
    GImage & dest, const GImage & src,
    int srcX, int srcY, int srcWidth, int srcHeight) {
    if ((src.width < srcX + srcWidth) ||
        (src.height < srcY + srcHeight) ||
        (srcY < 0) ||
        (srcX < 0)) {

        return false;
    }

    dest.resize(srcWidth, srcHeight, src.channels);
    
    bool ret;
    ret = pasteSubImage(dest, src, 0, 0, srcX, srcY, srcWidth, srcHeight);
    debugAssert(ret);

    return true;
}


bool GImage::pasteSubImage(
    GImage & dest, const GImage & src,
    int destX, int destY,
    int srcX, int srcY, int srcWidth, int srcHeight) {
    if ((src.width < srcX + srcWidth) ||
        (src.height < srcY + srcHeight) ||
        (dest.width < destX + srcWidth) ||
        (dest.height < destY + srcHeight) ||
        (srcY < 0) ||
        (srcX < 0) ||
        (destY < 0) ||
        (destX < 0) ||
        (src.channels != dest.channels)) {

        return false;
    }

    for (int i = 0; i < srcHeight; i++) {
        const uint8* srcRow = src.byte() +
            ((i + srcY) * src.width + srcX) * src.channels;
        uint8* destRow = dest.byte() +
            ((i + destY) * dest.width + destX) * dest.channels;
        memcpy(destRow, srcRow, srcWidth * src.channels);
    }

    return true;
}


bool GImage::supportedFormat(
    const std::string& format) {

    return (stringToFormat(format) != UNKNOWN);
}


GImage::Format GImage::stringToFormat(
    const std::string& format) {

    std::string extension = toUpper(format);

    if ((extension == "JPG") || (extension == "JPEG")) {
        return JPEG;
    } else if (extension == "TGA") {
        return TGA;
    } else if (extension == "BMP") {
        return BMP;
    } else if (extension == "PCX") {
        return PCX;
    } else if (extension == "ICO") {
        return ICO;
    } else if (extension == "PNG") {
        return PNG;
    } else if (extension == "PPM") {
        return PPM;
    } else {
        return UNKNOWN;
    }
}


void GImage::save(
    const std::string& filename,
    Format             format) const {

    BinaryOutput b(filename, G3D_LITTLE_ENDIAN);
    encode(resolveFormat(filename, NULL, 0, format), b);
    b.commit(false);
}


void GImage::encode(
    Format              format,
    uint8*&             outData,
    int&                outLength) const {

    BinaryOutput out;

    encode(format, out);

    outData = (uint8*)System::malloc(out.size());
    debugAssert(outData);
    outLength = out.size();

    out.commit(outData);
}


void GImage::encode(
    Format              format,
    BinaryOutput&       out) const {

    switch (format) {
    case PPM_ASCII:
        encodePPMASCII(out);
        break;

    case PPM:
        encodePPM(out);
        break;

    case PNG:
        encodePNG(out);
        break;

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

GImage GImage::insertRedAsAlpha(const GImage& alpha) const {
    debugAssert(alpha.width == width);
    debugAssert(alpha.height == height);

    GImage out(width, height, 4);

    for (int i = 0; i < width * height; ++i) {
        out.byte()[i * 4 + 0] = byte()[i * channels + 0];
        out.byte()[i * 4 + 1] = byte()[i * channels + 1];
        out.byte()[i * 4 + 2] = byte()[i * channels + 2];
        out.byte()[i * 4 + 3] = alpha.byte()[i * alpha.channels];
    }

    return out;
}


GImage GImage::stripAlpha() const {
    GImage out(width, height, 3);

    for (int i = 0; i < width * height; ++i) {
        out.byte()[i * 3 + 0] = byte()[i * channels + 0];
        out.byte()[i * 3 + 1] = byte()[i * channels + 1];
        out.byte()[i * 3 + 2] = byte()[i * channels + 2];
    }

    return out;
}


int GImage::sizeInMemory() const {
	return sizeof(GImage) + width * height * channels;
}


void GImage::computeNormalMap(
    const GImage&       bump,
    GImage&             normal,
    bool                lowPassBump,
    bool                scaleHeightByNz) {

    const int w = bump.width;
    const int h = bump.height;
    const int stride = bump.channels;

    normal.resize(w, h, 4);

    const uint8* const B = bump.byte();
    Color4uint8* const N = normal.pixel4();

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            // Index into normal map pixel
            int i = x + y * w;

            // Index into bump map *byte*
            int j = stride * i;


            Vector3 delta;

            // Get a value from B (with wrapping lookup) relative to (x, y)
            // and divide by 255
            #define height(DX, DY) ((B[(((DX + x + w) % w) + \
                                        ((DY + y + h) % h) * w) * stride]) / 255.0)


            // Sobel filter to compute the normal.  
            //
            // Y Filter (X filter is the transpose)
            //  [ -1 -2 -1 ]
            //  [  0  0  0 ]
            //  [  1  2  1 ]

            // Write the Y value directly into the x-component so we don't have
            // to explicitly compute a cross product at the end.
            delta.y = -(height(-1, -1) *  1 + height( 0, -1) *  2 + height( 1, -1) *  1 +
                        height(-1,  1) * -1 + height( 0,  1) * -2 + height( 1,  1) * -1);

            delta.x = -(height(-1, -1) * -1 + height( 1, -1) * 1 + 
                        height(-1,  0) * -2 + height( 1,  0) * 2 + 
                        height(-1,  1) * -1 + height( 1,  1) * 1);

            delta.z = 1.0;


            delta = delta.direction();

            // Copy over the bump value into the alpha channel.
            float H = B[j] / 255.0;

            if (lowPassBump) {
                H = (height(-1, -1) + height( 0, -1) + height(1, -1) +
                        height(-1,  0) + height( 0,  0) + height(1,  0) +
                        height(-1,  1) + height( 0,  1) + height(1,  1)) / 9.0;
            }
            #undef height

            if (scaleHeightByNz) {
                // delta.z can't possibly be negative, so we avoid actually
                // computing the absolute value.
                H *= delta.z;
            }

            N[i].a = iRound(H * 255.0);

            // Pack into byte range
            delta = delta * 127.5 + Vector3(127.5, 127.5, 127.5);
            N[i].r = iClamp(iRound(delta.x), 0, 255);
            N[i].g = iClamp(iRound(delta.y), 0, 255);
            N[i].b = iClamp(iRound(delta.z), 0, 255);
        }
    }
}


void GImage::BAYER_G8B8_R8G8_to_Quarter_R8G8B8(int width, int height, const uint8* in, uint8* out) {
    debugAssert(in != out);

    int halfHeight = height / 2;
    int halfWidth  = width / 2;

    int dst_off = 0;
    for (int y = 0; y < halfHeight; ++y) {
        for (int x = 0; x < halfWidth; ++x) {
            // GBRG
            int src_off = x*2 + y*2*width;
            out[dst_off] = in[src_off+width]; // red
            out[dst_off+1] = ((int)in[src_off] + (int)in[src_off+width+1])/2; // green
            out[dst_off+2] = in[src_off+1]; // blue            

            dst_off = dst_off + 3;
        }
    }
}


void GImage::Quarter_R8G8B8_to_BAYER_G8B8_R8G8(int inWidth, int inHeight, const uint8* in, uint8* out) {
    // Undo quarter-size Bayer as best we can.  This code isn't very efficient, but it
    // also isn't used very frequently.

    debugAssert(out != in);

    int outWidth  = 2 * inWidth;
    int outHeight = 2 * inHeight;

    for (int y = 0; y < outHeight; ++y) {
        for (int x = 0; x < outWidth; ++x) {
            const Color3uint8* inp = ((const Color3uint8*)in) + ((x/2) + (y/2)* inWidth);
            uint8* outp = out + x + y * outWidth;

            if (isEven(y)) {
                // GB row
                if (isEven(x)) {
                    // Green
                    *outp = inp->g;
                } else {
                    // Blue
                    *outp = inp->b;
                }
            } else {
                // RG row
                if (isEven(x)) {
                    // Red
                    *outp = inp->r;
                } else {
                    // Green
                    *outp = inp->g;
                }
            }
        }
    }
}


/** Applies a 5x5 filter to monochrome image I (wrapping at the boundaries) */
static uint8 applyFilter(
    const uint8*    I,
    int             x,
    int             y,
    int             w,
    int             h,
    const float     filter[5][5]) {

    debugAssert(isEven(w));
    debugAssert(isEven(h));

    float sum = 0.0;
    float denom = 0.0;

    for (int dy = 0; dy < 5; ++dy) {
        int offset = ((y + dy + h - 2) % h) * w;

        for (int dx = 0; dx < 5; ++dx) {
            float f = filter[dy][dx];
            sum += f * I[((x + dx + w - 2) % w) + offset];
            denom += f;
        }
    }

    return (uint8)iClamp(iRound(sum / denom), 0, 255);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Bayer conversions
//

// There are two kinds of rows (GR and BG).
// In each row, there are two kinds of pixels (G/R, B/G).
// We express the four kinds of INPUT pixels as:
//    GRG, GRG, BGB, BGG
//
// There are three kinds of OUTPUT pixels: R, G, B.
// Thus there are nominally 12 different I/O combinations, 
// but several are impulses because needed output at that 
// location *is* the input (e.g., G_GRG and G_BGG).
//
// The following 5x5 row-major filters are named as output_input.

// Green
static const float G_GRR[5][5] =
{{     0.0f,      0.0f,     -1.0f,      0.0f,      0.0f},
{     0.0f,      0.0f,      2.0f,      0.0f,      0.0f},
{    -1.0f,      2.0f,      4.0f,      2.0f,     -1.0f},
{     0.0f,      0.0f,      2.0f,      0.0f,      0.0f},
{     0.0f,      0.0f,     -1.0f,      0.0f,      0.0f}};

static const float G_BGB[5][5] =
{{     0.0f,      0.0f,     -1.0f,      0.0f,      0.0f},
{     0.0f,      0.0f,      2.0f,      0.0f,      0.0f},
{    -1.0f,      2.0f,      4.0f,      2.0f,     -1.0f},
{     0.0f,      0.0f,      2.0f,      0.0f,      0.0f},
{     0.0f,      0.0f,     -1.0f,      0.0f,      0.0f}};

// Red 
//(the caption in the paper is wrong for this case:
// "R row B column really means R row G column"
static const float R_GRG[5][5] =
{{     0.0,      0.0,      0.5,      0.0,      0.0},
{     0.0,     -1.0,      0.0,     -1.0,      0.0},
{    -1.0,      4.0,      5.0,      4.0,     -1.0},
{     0.0,     -1.0,      0.0,     -1.0,      0.0},
{     0.0,      0.0,      0.5,      0.0,      0.0}};

static const float R_BGG[5][5] =
{{     0.0,      0.0,     -1.0,      0.0,      0.0},
{     0.0,     -1.0,      4.0,     -1.0,      0.0},
{     0.5,      0.0,      5.0,      0.0,      0.5},
{     0.0,     -1.0,      4.0,     -1.0,      0.0},
{     0.0,      0.0,     -1.0,      0.0,      0.0}};

static const float R_BGB[5][5] =
{{     0.0,      0.0, -3.0/2.0,      0.0,      0.0},
{     0.0,      2.0,      0.0,      2.0,      0.0},
{-3.0/2.0,      0.0,      6.0,      0.0, -3.0/2.0},
{     0.0,      2.0,      0.0,      2.0,      0.0},
{     0.0,      0.0, -3.0/2.0,      0.0,      0.0}};


// Blue 
//(the caption in the paper is wrong for this case:
// "B row R column really means B row G column")
#define B_BGG R_GRG
#define B_GRG R_BGG
#define B_GRR R_BGB


void GImage::BAYER_R8G8_G8R8_to_R8G8B8_MHC(int w, int h, const uint8* in, uint8* _out) {
    debugAssert(in != _out);

    Color3uint8* out = (Color3uint8*)_out;

    for (int y = 0; y < h; ++y) {

        // Row beginning in the input array.
        int offset = y * w;

        // RG row
        for (int x = 0; x < w; ++x, ++out) {
            // R pixel
            {
                out->r = in[x + offset];
                out->g = applyFilter(in, x, y, w, h, G_GRR);
                out->b = applyFilter(in, x, y, w, h, B_GRR);
            }
            ++x; ++out;

            // G pixel
            {
                out->r = applyFilter(in, x, y, w, h, R_GRG);
                out->g = in[x + offset];
                out->b = applyFilter(in, x, y, w, h, B_GRG);
            }
        }

        ++y;
        offset += w;

        // GB row
        for (int x = 0; x < w; ++x, ++out) {
            // G pixel
            {
                out->r = applyFilter(in, x, y, w, h, R_BGG);
                out->g = in[x + offset];
                out->b = applyFilter(in, x, y, w, h, B_BGG);
            }
            ++x; ++out;

            // B pixel
            {
                out->r = applyFilter(in, x, y, w, h, R_BGB);
                out->g = applyFilter(in, x, y, w, h, G_BGB);
                out->b = in[x + offset];
            }
        }
    }
}


void GImage::BAYER_G8B8_R8G8_to_R8G8B8_MHC(int w, int h, const uint8* in, uint8* _out) {

    debugAssert(in != _out);

    Color3uint8* out = (Color3uint8*)_out;

    for (int y = 0; y < h; ++y) {

        // Row beginning in the input array.
        int offset = y * w;

        // GB row
        for (int x = 0; x < w; ++x, ++out) {
            // G pixel
            {
                out->r = applyFilter(in, x, y, w, h, R_BGG);
                out->g = in[x + offset];
                out->b = applyFilter(in, x, y, w, h, B_BGG);
            }
            ++x; ++out;

            // B pixel
            {
                out->r = applyFilter(in, x, y, w, h, R_BGB);
                out->g = applyFilter(in, x, y, w, h, G_BGB);
                out->b = in[x + offset];
            }
        }

        ++y;
        offset += w;

        // RG row
        for (int x = 0; x < w; ++x, ++out) {
            // R pixel
            {
                out->r = in[x + offset];
                out->g = applyFilter(in, x, y, w, h, G_GRR);
                out->b = applyFilter(in, x, y, w, h, B_GRR);
            }
            ++x; ++out;

            // G pixel
            {
                out->r = applyFilter(in, x, y, w, h, R_GRG);
                out->g = in[x + offset];
                out->b = applyFilter(in, x, y, w, h, B_GRG);
            }
        }
    }

}

#undef B_BGG
#undef B_GRG
#undef B_GRR
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GImage::convertToL8() {
    switch(channels) {
    case 1:
        return;

    case 3:
        {            
            // Average
            Color3uint8* src = (Color3uint8*)_byte;
            _byte = NULL;
            resize(width, height, 1);
            for (int i = width * height - 1; i >= 0; --i) {
                const Color3uint8   s = src[i];
                uint8&              d = _byte[i]; 
                d = ((int)s.r + (int)s.g + (int)s.b) / 3;
            }
            System::free(src);
        }
        break;

    case 4:
        {            
            // Average
            Color4uint8* src = (Color4uint8*)_byte;
            _byte = NULL;
            resize(width, height, 1);
            for (int i = width * height - 1; i >= 0; --i) {
                const Color4uint8   s = src[i];
                uint8&              d = _byte[i]; 
                d = ((int)s.r + (int)s.g + (int)s.b) / 3;
            }
            System::free(src);
        }
        return;

    default:
        alwaysAssertM(false, "Bad number of channels in input image");
    }
}


void GImage::convertToRGBA() {
    switch(channels) {
    case 1:
        {            
            // Spread
            uint8* old = _byte;
            _byte = NULL;
            resize(width, height, 4);
            for (int i = width * height - 1; i >= 0; --i) {
                const uint8  s = old[i];
                Color4uint8& d = ((Color4uint8*)_byte)[i]; 
                d.r = d.g = d.b = s;
                d.a = 255;
            }
            System::free(_byte);
        }
        break;

    case 3:
        {            
            // Add alpha
            Color3uint8* old = (Color3uint8*)_byte;
            _byte = NULL;
            resize(width, height, 4);
            for (int i = width * height - 1; i >= 0; --i) {
                const Color3uint8   s = old[i];
                Color4uint8&        d = ((Color4uint8*)_byte)[i]; 
                d.r = s.r;
                d.g = s.g;
                d.b = s.b;
                d.a = 255;
            }
            System::free(old);
        }
        break;

    case 4:
        // Already RGBA
        return;

    default:
        alwaysAssertM(false, "Bad number of channels in input image");
    }
}


void GImage::convertToRGB() {
    switch(channels) {
    case 1:
        {            
            // Spread
            uint8* old = _byte;
            _byte = NULL;
            resize(width, height, 3);
            for (int i = width * height - 1; i >= 0; --i) {
                const uint8  s = old[i];
                Color3uint8& d = ((Color3uint8*)_byte)[i]; 
                d.r = d.g = d.b = s;
            }
            System::free(old);
        }
        break;

    case 3:
		return;

    case 4:
		// Strip alpha
        {            
            Color4uint8* old = (Color4uint8*)_byte;
            _byte = NULL;
            resize(width, height, 3);
            for (int i = width * height - 1; i >= 0; --i) {
                const Color4uint8   s = old[i];
                Color3uint8&        d = ((Color3uint8*)_byte)[i]; 
                d.r = s.r;
                d.g = s.g;
                d.b = s.b;
            }
            System::free(old);
        }
        break;

    default:
        alwaysAssertM(false, "Bad number of channels in input image");
    }
}


void GImage::R8G8B8_to_Y8U8V8(int width, int height, const uint8* _in, uint8* _out) {
    const Color3uint8* in = reinterpret_cast<const Color3uint8*>(_in);
    Color3uint8* out = reinterpret_cast<Color3uint8*>(_out);

    Color3uint8 p;
    for (int i = width * height - 1; i >= 0; --i) {
        p.r = iClamp(iRound(in->r *  0.229 + in->g *  0.587 + in->b *  0.114), 0, 255);
        p.g = iClamp(iRound(in->r * -0.147 + in->g * -0.289 + in->b *  0.436) + 127, 0, 255);
        p.b = iClamp(iRound(in->r *  0.615 + in->g * -0.515 + in->b * -0.100) + 127, 0, 255);
        *out = p;
        ++in;
        ++out;
    }
}



void GImage::Y8U8V8_to_R8G8B8(int width, int height, const uint8* _in, uint8* _out) {
    const Color3uint8* in = reinterpret_cast<const Color3uint8*>(_in);
    Color3uint8* out = reinterpret_cast<Color3uint8*>(_out);

    Color3uint8 p;
    for (int i = width * height - 1; i >= 0; --i) {
        p.r = iClamp(iRound(in->r *  1.0753 +                   (in->b - 127) *  1.2256), 0, 255);
        p.g = iClamp(iRound(in->r *  1.0753 + (in->g - 127) * -0.3946 + (in->b - 127) * -0.4947), 0, 255);
        p.b = iClamp(iRound(in->r *  1.0753 + (in->g - 127) *  2.0320 + (in->b - 127) *  0.0853), 0, 255);
        *out = p;
        ++in;
        ++out;
    }
}

}

