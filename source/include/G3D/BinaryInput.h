/**
 @file BinaryInput.h
 
 @maintainer Morgan McGuire, graphics3d.com
 
 @created 2001-08-09
 @edited  2005-02-24

 Copyright 2000-2005, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_BINARYINPUT_H
#define G3D_BINARYINPUT_H

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
#include "G3D/System.h"

namespace G3D {

#if defined(G3D_WIN32) || defined(G3D_LINUX)
    // Allow writing of integers to non-word aligned locations.
    // This is legal on x86, but not on other platforms.
    #define G3D_ALLOW_UNALIGNED_WRITES
#endif

/**
 Sequential or random access byte-order independent binary file access.
 Files compressed with zlib and beginning with an unsigned 32-bit int
 size are transparently decompressed when the compressed = true flag is
 specified to the constructor.

 Most classes define serialize/deserialize methods that use BinaryInput,
 BinaryOutput, TextInput, and TextOutput.  There are text serializer 
 functions for primitive types (e.g. int, std::string, float, double) but not 
 binary serializers-- you <B>must</b> call the BinaryInput::readInt32 or
 other appropriate function.  This is because it would be very hard to 
 debug the error sequence: <CODE>serialize(1.0, bo); ... float f; deserialize(f, bi);</CODE>
 in which a double is serialized and then deserialized as a float. 
 */
class BinaryInput {
private:
    /**
     is the file big or little endian
     */
    G3DEndian       fileEndian;
    std::string     filename;

    bool            swapBytes;

    /** Next position to read from in bitString during readBits. */
    int             bitPos;

    /** Bits currently being read by readBits.  
        Contains at most 8 (low) bits.  Note that
        beginBits/readBits actually consumes one extra byte, which
        will be restored by writeBits.*/
    uint32          bitString;

    /** 1 when between beginBits and endBits, 0 otherwise. */
    int             beginEndBits;

    /**
     Length of file, in bytes
     */
    int             length;
    uint8*          buffer;

    /**
     Next byte in file
     */
    int             pos;

    /**
     When true, the buffer is freed in the deconstructor.
     */
    bool            freeBuffer;

public:

    /** false, constant to use with the copyMemory option */
    static const bool      NO_COPY;

	/**
	 If the file cannot be opened, a zero length buffer is presented.
	 */
    BinaryInput(
        const std::string&  filename,
        G3DEndian           fileEndian,
        bool                compressed = false);

    /**
     Creates input stream from an in memory source.
     Unless you specify copyMemory = false, the data is copied
     from the pointer, so you may deallocate it as soon as the
     object is constructed.  It is an error to specify copyMemory = false
     and compressed = true.

     To decompress part of a file, you can follow the following paradigm:

     <PRE>
        BinaryInput master(...);

        // read from master to point where compressed data exists.

        BinaryInput subset(master.getCArray() + master.getPosition(), 
                           master.length() - master.getPosition(),
                           true, true);

        // Now read from subset (it is ok for master to go out of scope)
     </PRE>
     */
    BinaryInput(
        const uint8*        data,
        int                 dataLen,
        G3DEndian           dataEndian,
        bool                compressed = false,
        bool                copyMemory = true);

    virtual ~BinaryInput();


    std::string getFilename() const {
        return filename;
    }

    /**
     returns a pointer to the internal memory buffer.
     */
    const uint8* getCArray() const {
        return buffer;
    }

    /**
     Performs bounds checks in debug mode.  [] are relative to
     the start of the file, not the current position.
     Seeks to the new position before reading.
     */
    inline const uint8 operator[](int n) {
        setPosition(n);
        return readUInt8();
    }

    /**
     Returns the length of the file in bytes.
     */
    inline int getLength() const {
        return length;
    }

    inline int size() const {
        return getLength();
    }

    /**
     Returns the current byte position in the file,
     where 0 is the beginning and getLength() - 1 is the end.
     */
    inline int getPosition() const {
        return pos;
    }

    /**
     Sets the position.  Cannot set past length.
     */
    inline void setPosition(int p) {
        debugAssertM(p <= length, "Read past end of file");
        pos = p;
    }

    /**
     Goes back to the beginning of the file.
     */
    inline void reset() {
        setPosition(0);
    }

    inline int8 readInt8() {
        debugAssertM(pos + 1 <= length, "Read past end of file");
        return buffer[pos++];
    }

    inline bool readBool8() {
        return (readInt8() != 0);
    }

    inline uint8 readUInt8() {
        debugAssertM(pos + 1 <= length, "Read past end of file");
        return ((uint8*)buffer)[pos++];
    }

    uint16 inline readUInt16() {
        debugAssertM(pos + 2 <= length, "Read past end of file");

        pos += 2;
        if (swapBytes) {
            uint8 out[2];
            out[0] = buffer[pos - 2];
            out[1] = buffer[pos - 1];
            return *(uint16*)out;
        } else {
            #ifdef G3D_ALLOW_UNALIGNED_WRITES
                return *(uint16*)(&buffer[pos - 2]);
            #else
                uint8 out[2];
                out[0] = buffer[pos - 1];
                out[1] = buffer[pos - 2];
                return *(uint16*)out;
            #endif
        }

    }

    inline int16 readInt16() {
        uint16 a = readUInt16();
        return *(int16*)&a;
    }

    inline uint32 readUInt32() {
        debugAssertM((pos + 4) <= length, "Read past end of file");

        pos += 4;
        if (swapBytes) {
            uint8 out[4];
            out[0] = buffer[pos - 1];
            out[1] = buffer[pos - 2];
            out[2] = buffer[pos - 3];
            out[3] = buffer[pos - 4];
            return *(uint32*)out;
        } else {
            #ifdef G3D_ALLOW_UNALIGNED_WRITES
                return *(uint32*)(&buffer[pos - 4]);
            #else
                uint8 out[4];
                out[0] = buffer[pos - 4];
                out[1] = buffer[pos - 3];
                out[2] = buffer[pos - 2];
                out[3] = buffer[pos - 1];
                return *(uint32*)out;
            #endif
        }
    }


    inline int32 readInt32() {
        uint32 a = readUInt32();
        return *(int32*)&a;
    }

    uint64 readUInt64();

    inline int64 readInt64() {
        uint64 a = readUInt64();
        return *(int64*)&a;
    }

    inline float32 readFloat32() {
        uint32 a = readUInt32();
        return *(float32*)&a;
    }

    inline float64 readFloat64() {
        uint64 a = readUInt64();
        return *(float64*)&a;
    }

    /**
     Returns the data in bytes.
     @deprecated Use readBytes(void*, int).
     */
    void readBytes(int n, void* bytes);

    /**
     Returns the data in bytes.
     */
    inline void readBytes(void* bytes, int n) {
        readBytes(n, bytes);
    }

    /**
     Reads an n character string.  The string is not
     required to end in NULL in the file but will
     always be a proper std::string when returned.
     */
    std::string readString(int n);

    /**
     Reads until NULL or the end of the file is encountered.
     */
    std::string readString();

    /**
     Reads until NULL or the end of the file is encountered.
     If the string has odd length (including NULL), reads 
     another byte.
     */
    std::string readStringEven();


    std::string readString32();

    Vector4 readVector4();
    Vector3 readVector3();
    Vector2 readVector2();

    Color4 readColor4();
    Color3 readColor3();

    /**
     Skips ahead n bytes.
     */
    inline void skip(int n) {
        debugAssertM((pos + n) <= length, "Read past end of file");
        pos += n;
    }

	/**
	 Returns true if the position is not at the end of the file
	 */
	inline bool hasMore() const {
		return pos < length;
	}

    /** Prepares for bit reading via readBits.  Only readBits can be
        called between beginBits and endBits without corrupting the
        data stream. */
    void beginBits();

    /** Can only be called between beginBits and endBits */
    uint32 readBits(int numBits);

    /** Ends bit-reading. */
    void endBits();
};


}

#endif

