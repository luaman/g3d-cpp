/**
 @file BinaryOutput.h
 
 @maintainer Morgan McGuire, graphics3d.com
 
 @created 2001-08-09
 @edited  2005-01-13

 Copyright 2000-2005, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_BINARYOUTPUT_H
#define G3D_BINARYOUTPUT_H

#include <assert.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include "G3D/Color4.h"
#include "G3D/Color3.h"
#include "G3D/Vector4.h"
#include "G3D/Vector3.h"
#include "G3D/Vector2.h"
#include "G3D/g3dmath.h"
#include "G3D/debug.h"
#include "G3D/BinaryInput.h"
#include "G3D/System.h"

namespace G3D {

/**
 Sequential or random access byte-order independent binary file access.

 The compress() call can be used to compress with zlib.
 */
class BinaryOutput {
private:
    std::string     filename;

    // True if the file endianess does not match the machine endian
    bool            swapBytes;

    uint8*          buffer;
    
    /** Size of the elements used */
    size_t          bufferLen;

    /** Underlying size of memory allocaded */
    size_t          maxBufferLen;

    // Next byte in file
    size_t          pos;

    // is this initialized?
    bool            init;

    /**
     Make sure at least bytes can be written, resizing if
     necessary.
     */
    void reserveBytes(size_t bytes) {
        bufferLen = iMax(bufferLen, pos + bytes);
        if (bufferLen >= maxBufferLen) {
            maxBufferLen = (size_t)(bufferLen * 1.5) + 100;
            buffer = (uint8*)realloc(buffer, maxBufferLen);
        }

    }

public:

    /**
     You must call setEndian() if you use this (memory) constructor.
     */
    BinaryOutput();

    /**
     Doesn't actually open the file; commit() does that.
     Use "<memory>" as the filename if you're going to commit
     to memory.
     */
    BinaryOutput(
        const std::string&  filename,
        G3DEndian           fileEndian);

    ~BinaryOutput();
    
    /** Compresses the data in the buffer in place, 
        preceeding it with a little-endian uint32 indicating 
        the uncompressed size.
        Call immediately before commit().*/
    void compress();

    /**
     Returns a pointer to the internal memory buffer.
     */
    inline const uint8* getCArray() const {
        return buffer;
    }

    void setEndian(G3DEndian fileEndian);

    std::string getFilename() const {
        return filename;
    }

    /**
     Write the bytes to disk.  It is ok to call this 
     multiple times; it will just overwrite the previous file.

     Parent directories are created as needed if they do
     not exist.

     <B>Not</B> called from the destructor; you must call
     it yourself.

     @param flush If true (default) the file is ready for reading when the method returns, otherwise 
      the method returns immediately and writes the file in the background.
    */
    void commit(bool flush = true);

    /**
     Write the bytes to memory (which must be of
     at least size() bytes).
     */
    void commit(uint8*);

    /**
      A memory BinaryOutput may be reset so that it can be written to again
      without allocating new memory.  The underlying array will not be deallocated,
      but the reset structure will act like a newly intialized one.
     */
    void reset();

    /**
     Returns the length of the file in bytes.
     @deprecated use BinaryOutput.size
     */
    inline int getLength() const {
        return bufferLen;
    }

    inline int length() const {
        return bufferLen;
    }

    inline int size() const {
        return bufferLen;
    }

    /**
     Sets the length of the file to n, padding
     with 0's past the current end.  Does not
     change the position of the next byte to be
     written unless n < size().
     */
    inline void setLength(size_t n) {
        if (n < bufferLen) {
            pos = n;
        }
        if (n > bufferLen) {
            reserveBytes(n - bufferLen);
        }
    }

    /**
     Returns the current byte position in the file,
     where 0 is the beginning and getLength() - 1 is the end.
     */
    inline int getPosition() const {
        return pos;
    }

    /**
     Sets the position.  Can set past length, in which case
     the file is padded with zeros up to one byte before the
     next to be written.
     */
    inline void setPosition(size_t p) {
        if (p > bufferLen) {
            setLength(p);
        }
        pos = p;
    }

    void writeBytes(
        const void*        b,
        int                 count) {

        reserveBytes(count);
        System::memcpy(buffer + pos, b, count);
        pos += count;
    }

    /**
     Writes a signed 8-bit integer to the current position.
     */
    inline void writeInt8(int8 i) {
        reserveBytes(1);
        buffer[pos] = *(uint8*)&i;
        pos++;
    }

    inline void writeBool8(bool b) {
        writeInt8(b ? 1 : 0);
    }

    inline void writeUInt8(int8 i) {
        reserveBytes(1);
        buffer[pos] = i;
        pos++;
    }

    void writeUInt16(uint16 u);

    inline void writeInt16(int16 i) {
        writeUInt16(*(uint16*)&i);
    }

    void writeUInt32(uint32 u);

    inline void writeInt32(int32 i) {
        writeUInt32(*(uint32*)&i);
    }

    void writeUInt64(uint64 u);

    inline void writeInt64(int64 i) {
        writeUInt64(*(uint64*)&i);
    }

    inline void writeFloat32(float32 f) {
        writeUInt32(*(uint32*)&f);
    }

    inline void writeFloat64(float64 f) {
        writeUInt64(*(uint64*)&f);
    }

    /**
     Write a string with NULL termination.
     */
    inline void writeString(const std::string& s) {
        writeString(s.c_str());
    }

    void writeString(const char* s);

    /**
     Write a string, ensuring that the total length
     including NULL is even.
     */
    void writeStringEven(const std::string& s) {
        writeStringEven(s.c_str());
    }

    void writeStringEven(const char* s);


    void writeString32(const char* s);

    /**
     Write a string with a 32-bit length field in front
     of it.
     */
    void writeString32(const std::string& s) {
        writeString32(s.c_str());
    }

    void writeVector4(const Vector4& v);

    void writeVector3(const Vector3& v);

    void writeVector2(const Vector2& v);

    void writeColor4(const Color4& v);

    void writeColor3(const Color3& v);

    /**
     Skips ahead n bytes.
     */
    inline void skip(size_t n) {
        if (pos + n > bufferLen) {
            setLength(pos + n);
        }
        pos += n;
    }
};

}
#endif

