/**
 @file BinaryOutput.cpp
 
 @author Morgan McGuire, graphics3d.com
 
 @created 2002-02-20
 @edited  2003-02-06
 */

#include "G3D/BinaryOutput.h"
#include "G3D/fileutils.h"
#include "G3D/stringutils.h"
#ifdef _WIN32
  #include "zlib/zlib.h"
#else
  #include <zlib.h>
#endif


namespace G3D {

BinaryOutput::BinaryOutput() {
    swapBytes = false;
    pos       = 0;
    filename  = "<memory>";
}


BinaryOutput::BinaryOutput(
    const std::string&  filename,
    G3DEndian           fileEndian) {

    pos = 0;
    setEndian(fileEndian);
    this->filename = filename;
}


BinaryOutput::~BinaryOutput() {
    // Nothing to do
}


void BinaryOutput::setEndian(
    G3DEndian fileEndian) {

    // Figure out if this machine is little or big endian.
    G3DEndian machineEndian;
    
    int32 a = 1;
    if (*(uint8*)&a == 1) {
        machineEndian = G3D_LITTLE_ENDIAN;
    } else {
        machineEndian = G3D_BIG_ENDIAN;
    }

    swapBytes = (fileEndian != machineEndian);
}


void BinaryOutput::compress() {
    // Old buffer size
    uint32 L       = buffer.size();
    uint8* convert = (uint8*)&L;

    // Zlib requires the output buffer to be this big
    uint8* temp = (uint8*)malloc(iCeil(buffer.size() * 1.01) + 12);
    uLongf newSize;
    int result = compress2 (temp, &newSize, buffer.getCArray(), buffer.size(), 9); 

    debugAssert(result == Z_OK);


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
    buffer.resize(newSize + 4);
    memcpy(buffer.getCArray() + 4, temp, newSize);
    pos = buffer.size();

    free(temp);
}


void BinaryOutput::commit() {
    // Make sure the directory exists.
    std::string root, base, ext, path;
    Array<std::string> pathArray;
    parseFilename(filename, root, pathArray, base, ext); 

    path = root + stringJoin(pathArray, '/');
    if (! fileExists(path)) {
        createDirectory(path);
    }

    FILE* file = fopen(filename.c_str(), "wb");

    debugAssert(file);


    fwrite(buffer.getCArray(), buffer.size(), 1, file);
    fclose(file);
    file = NULL;
}


void BinaryOutput::commit(
    uint8*                  out) {

    memcpy(out, buffer.getCArray(), buffer.size());
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

    reserveBytes(len);
    memcpy(buffer.getCArray() + pos, s, len);
    pos += len;
}


void BinaryOutput::writeStringEven(const char* s) {
    // +1 is because strlen doesn't count the null
    int len = strlen(s) + 1;

    reserveBytes(len);
    memcpy(buffer.getCArray() + pos, s, len);
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

}
