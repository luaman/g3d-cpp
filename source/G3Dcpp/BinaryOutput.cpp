/**
 @file BinaryOutput.cpp
 
 @author Morgan McGuire, graphics3d.com
 
 @created 2002-02-20
 @edited  2005-01-15
 */

#include "G3D/platform.h"
#include "G3D/BinaryOutput.h"
#include "G3D/fileutils.h"
#include "G3D/stringutils.h"
#include "G3D/Array.h"
#ifdef G3D_WIN32
  #include "zlib/zlib.h"
#else
  #include <zlib.h>
#endif


namespace G3D {

void BinaryOutput::reallocBuffer(size_t bytes, size_t oldBufferLen) {
    maxBufferLen = (int)(bufferLen * 1.5) + 100;
    uint8* newBuffer = (uint8*)realloc(buffer, maxBufferLen);

    if ((newBuffer == NULL) && (bytes > 0)) {
        // Realloc failed; we're probably out of memory.
        bufferLen = oldBufferLen;
        reserveBytesWhenOutOfMemory(bytes);
    } else {
        buffer = newBuffer;
        debugAssert(isValidHeapPointer(buffer));
    }
}


void BinaryOutput::reserveBytesWhenOutOfMemory(size_t bytes) {
    if (filename == "<memory>") {
        throw "Out of memory while writing to memory in BinaryOutput (no RAM left).";
    }else if (bytes > maxBufferLen) {
        throw "Out of memory while writing to disk in BinaryOutput (could not create a large enough buffer).";
    } else {
        // Dump the contents to disk.  In order to enable seeking, we keep the last
        // 10 MB in memory.

        size_t writeBytes = bufferLen - 10 * 1024 * 1024;

        if (writeBytes < bufferLen / 3) {
            // We're going to write less than 1/3 of the file;
            // give up and just write the whole thing.
            writeBytes = bufferLen;
        }

        FILE* file = fopen(filename.c_str(), "ab");
        debugAssert(file);

        size_t count = fwrite(buffer, 1, writeBytes, file);
        debugAssert(count == writeBytes);

        fclose(file);
        file = NULL;

        // Record that we saved this data.
        alreadyWritten += writeBytes;
        bufferLen -= writeBytes;
        pos -= writeBytes;

        // Shift the unwritten data back appropriately in the buffer.
        debugAssert(isValidHeapPointer(buffer));
        System::memcpy(buffer, buffer + writeBytes, bufferLen);
        debugAssert(isValidHeapPointer(buffer));
    }
}


BinaryOutput::BinaryOutput() {
    alreadyWritten = 0;
    swapBytes = false;
    pos       = 0;
    filename  = "<memory>";
    buffer = NULL;
    bufferLen = 0;
    maxBufferLen = 0;
    beginEndBits = 0;
    bitString = 0;
    bitPos = 0;
    committed = false;
}


BinaryOutput::BinaryOutput(
    const std::string&  filename,
    G3DEndian           fileEndian) {

    pos = 0;
    alreadyWritten = 0;
    setEndian(fileEndian);
    this->filename = filename;
    buffer = NULL;
    bufferLen = 0;
    maxBufferLen = 0;
    beginEndBits = 0;
    bitString = 0;
    bitPos = 0;
    committed = false;
}


void BinaryOutput::reset() {
    debugAssert(beginEndBits == 0);
    alwaysAssertM(filename == "<memory>", 
        "Can only reset a BinaryOutput that writes to memory.");

    // Do not reallocate, just clear the size of the buffer.
    pos = 0;
    alreadyWritten = 0;
    bufferLen = 0;
    beginEndBits = 0;
    bitString = 0;
    bitPos = 0;
    committed = false;
}


BinaryOutput::~BinaryOutput() {
    debugAssert(isValidHeapPointer(buffer));
    free(buffer);
    buffer = NULL;
    bufferLen = 0;
    maxBufferLen = 0;
}


void BinaryOutput::setEndian(G3DEndian fileEndian) {
    swapBytes = (fileEndian != System::machineEndian());
}


void BinaryOutput::compress() {
    if (alreadyWritten > 0) {
        throw "Cannot compress huge files (part of this file has already been written to disk).";
    }

    // Old buffer size
    uint32 L       = bufferLen;
    uint8* convert = (uint8*)&L;

    // Zlib requires the output buffer to be this big
    uint8* temp = (uint8*)malloc(iCeil(bufferLen * 1.01) + 12);
    int newSize;
    int result = compress2(temp, (unsigned long*)&newSize, buffer, bufferLen, 9); 

    debugAssert(result == Z_OK); (void)result;

    // Write the header
    if (swapBytes) {
        buffer[0] = convert[3];
        buffer[1] = convert[2];
        buffer[2] = convert[1];
        buffer[3] = convert[0];
    } else {
        buffer[0] = convert[0];
        buffer[1] = convert[1];
        buffer[2] = convert[2];
        buffer[3] = convert[3];
    }

    // Write the data
    if (newSize + 4 > maxBufferLen) {
        maxBufferLen = newSize + 4;
        buffer = (uint8*)realloc(buffer, maxBufferLen);
    }
    bufferLen = newSize + 4;
    System::memcpy(buffer + 4, temp, newSize);
    pos = bufferLen;

    free(temp);
}


void BinaryOutput::commit(bool flush) {
    debugAssertM(! committed, "Cannot commit twice");
    committed = true;
    debugAssertM(beginEndBits == 0, "Missing endBits before commit");

    // Make sure the directory exists.
    std::string root, base, ext, path;
    Array<std::string> pathArray;
    parseFilename(filename, root, pathArray, base, ext); 

    path = root + stringJoin(pathArray, '/');
    if (! fileExists(path)) {
        createDirectory(path);
    }

    char* mode = (alreadyWritten > 0) ? "ab" : "wb";

    FILE* file = fopen(filename.c_str(), mode);
    debugAssert(file);

    alreadyWritten += bufferLen;

    fwrite(buffer, bufferLen, 1, file);
    if (flush) {
        fflush(file);
    }
    fclose(file);
    file = NULL;
}


void BinaryOutput::commit(
    uint8*                  out) {
    debugAssertM(! committed, "Cannot commit twice");
    committed = true;

    System::memcpy(out, buffer, bufferLen);
}


void BinaryOutput::writeUInt16(uint16 u) {
    reserveBytes(2);

    uint8* convert = (uint8*)&u;

    if (swapBytes) {
        buffer[pos]     = convert[1];
        buffer[pos + 1] = convert[0];
    } else {
        buffer[pos]     = convert[0];
        buffer[pos + 1] = convert[1];
    }

    pos += 2;
}


void BinaryOutput::writeUInt32(uint32 u) {
    reserveBytes(4);

    uint8* convert = (uint8*)&u;

    debugAssert(beginEndBits == 0);

    if (swapBytes) {
        buffer[pos]     = convert[3];
        buffer[pos + 1] = convert[2];
        buffer[pos + 2] = convert[1];
        buffer[pos + 3] = convert[0];
    } else {
        buffer[pos]     = convert[0];
        buffer[pos + 1] = convert[1];
        buffer[pos + 2] = convert[2];
        buffer[pos + 3] = convert[3];
    }

    pos += 4;
}


void BinaryOutput::writeUInt64(uint64 u) {
    reserveBytes(8);

    uint8* convert = (uint8*)&u;

    if (swapBytes) {
        buffer[pos]     = convert[7];
        buffer[pos + 1] = convert[6];
        buffer[pos + 2] = convert[5];
        buffer[pos + 3] = convert[4];
        buffer[pos + 4] = convert[3];
        buffer[pos + 5] = convert[2];
        buffer[pos + 6] = convert[1];
        buffer[pos + 7] = convert[0];
    } else {
        buffer[pos]     = convert[0];
        buffer[pos + 1] = convert[1];
        buffer[pos + 2] = convert[2];
        buffer[pos + 3] = convert[3];
        buffer[pos + 4] = convert[4];
        buffer[pos + 5] = convert[5];
        buffer[pos + 6] = convert[6];
        buffer[pos + 7] = convert[7];
    }

    pos += 8;
}


void BinaryOutput::writeString(const char* s) {
    // +1 is because strlen doesn't count the null
    int len = strlen(s) + 1;

    debugAssert(beginEndBits == 0);
    reserveBytes(len);
    System::memcpy(buffer + pos, s, len);
    pos += len;
}


void BinaryOutput::writeStringEven(const char* s) {
    // +1 is because strlen doesn't count the null
    int len = strlen(s) + 1;

    reserveBytes(len);
    System::memcpy(buffer + pos, s, len);
    pos += len;

    // Pad with another NULL
    if ((len % 2) == 1) {
        writeUInt8(0);
    }
}


void BinaryOutput::writeString32(const char* s) {
    writeUInt32(strlen(s) + 1);
    writeString(s);
}


void BinaryOutput::writeVector4(const Vector4& v) {
    writeFloat32(v.x);
    writeFloat32(v.y);
    writeFloat32(v.z);
    writeFloat32(v.w);
}


void BinaryOutput::writeVector3(const Vector3& v) {
    writeFloat32(v.x);
    writeFloat32(v.y);
    writeFloat32(v.z);
}


void BinaryOutput::writeVector2(const Vector2& v) {
    writeFloat32(v.x);
    writeFloat32(v.y);
}


void BinaryOutput::writeColor4(const Color4& v) {
    writeFloat32(v.r);
    writeFloat32(v.g);
    writeFloat32(v.b);
    writeFloat32(v.a);
}


void BinaryOutput::writeColor3(const Color3& v) {
    writeFloat32(v.r);
    writeFloat32(v.g);
    writeFloat32(v.b);
}


void BinaryOutput::beginBits() {
    debugAssertM(beginEndBits == 0, "Already in beginBits...endBits");
    bitString = 0x00;
    bitPos = 0;
    beginEndBits = 1;
}


void BinaryOutput::writeBits(uint32 value, int numBits) {

    while (numBits > 0) {
        // Extract the current bit of value and
        // insert it into the current byte
        bitString |= (value & 1) << bitPos;
        ++bitPos;
        value = value >> 1;
        --numBits;

        if (bitPos > 7) {
            // We've reached the end of this byte
            writeUInt8(bitString);
            bitString = 0x00;
            bitPos = 0;
        }
    }
}


void BinaryOutput::endBits() {
    debugAssertM(beginEndBits == 1, "Not in beginBits...endBits");
    if (bitPos > 0) {
        writeUInt8(bitString);
    }
    bitString = 0;
    bitPos = 0;
    beginEndBits = 0;
}

}
