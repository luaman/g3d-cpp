/**
 @file BinaryInput.cpp
 
 @author Morgan McGuire, graphics3d.com
 
 @created 2001-08-09
 @edited  2003-06-24


  <PRE>
    {    
    BinaryOutput b = BinaryOutput("c:/tmp/test.b", BinaryOutput::LITTLE_ENDIAN);

    float f = 3.1415926;
    int i = 1027221;
    std::string s = "Hello World!";

    b.writeFloat32(f);
    b.writeInt32(i);
    b.writeString(s);
    b.commit();
    

    BinaryInput in = BinaryInput("c:/tmp/test.b", BinaryInput::LITTLE_ENDIAN);

    debugAssert(f == in.readFloat32());
    int ii = in.readInt32();
    debugAssert(i == ii);
    debugAssert(s == in.readString());
    }
  </PRE>
 */

#include "G3D/platform.h"
#include "G3D/BinaryInput.h"
#include "G3D/fileutils.h"
#ifdef G3D_WIN32
  #include "zlib/zlib.h"
#else
  #include <zlib.h>
#endif

namespace G3D {

const bool BinaryInput::NO_COPY = false;
    
static bool needSwapBytes(G3DEndian fileEndian) {
    return (fileEndian != System::machineEndian());
}


/** Helper used by the constructors for decompression */
static uint32 readUInt32(const uint8* data, bool swapBytes) {
    if (swapBytes) {
        uint8 out[4];
        out[0] = data[3];
        out[1] = data[2];
        out[2] = data[1];
        out[3] = data[0];
        return *((uint32*)out);
    } else {
        return *((uint32*)data);
    }
}

BinaryInput::BinaryInput(
    const uint8*        data,
    int                 dataLen,
    G3DEndian           dataEndian,
    bool                compressed,
    bool                copyMemory) {

    debugAssert(!compressed || !copyMemory);

    freeBuffer = copyMemory || compressed;

    this->fileEndian = dataEndian;
    this->filename = "<memory>";
	pos = 0;
    swapBytes = needSwapBytes(fileEndian);

    if (compressed) {
        // Read the decompressed size from the first 4 bytes
        length = G3D::readUInt32(data, swapBytes);

      	buffer = (uint8*)malloc(length);

        unsigned long L;
        // Decompress with zlib
        int result = uncompress(buffer, &L, data + 4, dataLen - 4);
        length = L;
        debugAssert(result == Z_OK); (void)result;

    } else {
	    length = dataLen;
        if (! copyMemory) {
            buffer = const_cast<uint8*>(data);
        } else {
	        buffer = (uint8*)malloc(length);
            memcpy(buffer, data, dataLen);
        }
    }
}


BinaryInput::BinaryInput(
    const std::string&  filename,
    G3DEndian           fileEndian,
    bool                compressed) {

    freeBuffer = true;
    this->fileEndian = fileEndian;
    this->filename = filename;
	buffer = NULL;
	length = 0;
	pos = 0;

    swapBytes = needSwapBytes(fileEndian);

    // Figure out how big the file is and verify that it exists.
    length = fileLength(filename);

    // Read the file into memory
    FILE* file = fopen(filename.c_str(), "rb");

	if (! file || (length == -1)) {
        throw format("File not found: \"%s\"", filename.c_str());
		return;
	}

    buffer = (uint8*) malloc(length);
    debugAssert(buffer);
    
    fread(buffer, length, sizeof(int8), file);
    fclose(file);
    file = NULL;

    pos = 0;

    if (compressed) {
        // Decompress
        // Use the existing buffer as the source, allocate
        // a new buffer to use as the destination.

        int tempLength = length;
        length = G3D::readUInt32(buffer, swapBytes);

        uint8* tempBuffer = buffer;
        buffer = (uint8*)malloc(length);

        debugAssert(isValidHeapPointer(tempBuffer));
        debugAssert(isValidHeapPointer(buffer));

        unsigned long L = length;
        int result = uncompress(buffer, &L, tempBuffer + 4, tempLength - 4);
        length = L;

        debugAssert(result == Z_OK); (void)result;

        free(tempBuffer);
    }
}


BinaryInput::~BinaryInput() {

    if (freeBuffer) {
        free(buffer);
    }
    buffer = NULL;
}


void BinaryInput::readBytes(size_t n, void* bytes) {
    debugAssertM(pos + n <= length, "Read past end of file");
    debugAssert(isValidPointer(bytes));

    memcpy(bytes, buffer + pos, n);
    pos += n;
}


uint64 BinaryInput::readUInt64() {
    debugAssertM(pos + 8 <= length, "Read past end of file");
    uint8 out[8];

    if (swapBytes) {
        out[0] = buffer[pos + 7];
        out[1] = buffer[pos + 6];
        out[2] = buffer[pos + 5];
        out[3] = buffer[pos + 4];
        out[4] = buffer[pos + 3];
        out[5] = buffer[pos + 2];
        out[6] = buffer[pos + 1];
        out[7] = buffer[pos + 0];
    } else {
        out[0] = buffer[pos + 0];
        out[1] = buffer[pos + 1];
        out[2] = buffer[pos + 2];
        out[3] = buffer[pos + 3];
        out[4] = buffer[pos + 4];
        out[5] = buffer[pos + 5];
        out[6] = buffer[pos + 6];
        out[7] = buffer[pos + 7];
    }

    pos += 8;
    return *(uint64*)out;
}


std::string BinaryInput::readString(size_t n) {
    debugAssertM(pos + n <= length, "Read past end of file");
    
    char *s = (char*)malloc(n + 1);
    assert(s != NULL);

    memcpy(s, buffer + pos, n);
    // There may not be a null, so make sure
    // we add one.
    s[n] = '\0';

    std::string out = s;
    free(s);
    s = NULL;

    pos += n;

    return out;

}


std::string BinaryInput::readString() {
    int n = 0;

    while ((pos + n < length - 1) && (buffer[pos + n] != '\0')) {
        n++;
    }

    n++;

    return readString(n);
}


std::string BinaryInput::readStringEven() {
    std::string x = readString();
    if (hasMore() && (G3D::isOdd(x.length() + 1))) {
        skip(1);
    }
    return x;
}


std::string BinaryInput::readString32() {
    int len = readUInt32();
    return readString(len);
}


Vector4 BinaryInput::readVector4() {
    double x = readFloat32();
    double y = readFloat32();
    double z = readFloat32();
    double w = readFloat32();
    return Vector4(x, y, z, w);
}


Vector3 BinaryInput::readVector3() {
    double x = readFloat32();
    double y = readFloat32();
    double z = readFloat32();
    return Vector3(x, y, z);
}


Vector2 BinaryInput::readVector2() {
    double x = readFloat32();
    double y = readFloat32();
    return Vector2(x, y);
}


Color4 BinaryInput::readColor4() {
    double r = readFloat32();
    double g = readFloat32();
    double b = readFloat32();
    double a = readFloat32();
    return Color4(r, g, b, a);
}


Color3 BinaryInput::readColor3() {
    double r = readFloat32();
    double g = readFloat32();
    double b = readFloat32();
    return Color3(r, g, b);
}

}
