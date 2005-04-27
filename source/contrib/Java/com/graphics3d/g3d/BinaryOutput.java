package com.graphics3d.g3d;

import java.lang.IndexOutOfBoundsException;
import java.lang.Long;
import java.net.*;
import java.io.*;
import java.math.BigInteger;

/**
 Sequential or random access byte-order independent binary file
 access. 

 Differences from the C++ version:
 <UL>
  <LI> No huge files
  <LI> No compressed files
  <LI> No bit-reading
  <LI> No Vector, Color read methods
 </UL> 

 @author Morgan McGuire
 */
public class BinaryOutput {

    /**
     Index of the next byte in data to be used.
     */
    private int               position;

    /** Size of the bytes of data that are actually used, as opposed to preallocated */
    private int               dataSize;
    private byte              data[];
    private String            filename;

    private final static BigInteger maxUInt64 = BigInteger.valueOf(Long.MAX_VALUE).add(BigInteger.ONE).pow(2).subtract(BigInteger.ONE);
    public ByteOrder          byteOrder;

    /** Resize dataSize/data as necessary so that there are numBytes of space after the current position */
    private void reserveBytes(int numBytes) {
        if (position + numBytes < data.length) {
            byte temp[] = data;

            // Overestimate the amount of space needed
            data = new byte[(position + numBytes) * 2];
            System.arraycopy(temp, 0, temp.length, data, 0);
        }

        dataSize = Math.max(position + numBytes, dataSize);
    }

    private BinaryInput(ByteOrder byteOrder) {
        this.byteOrder = byteOrder;
        dataSize = 0;
        filename = "";
        position = 0;
        data = new byte[128];
    }
    
    public BinaryInput(String filename, int byteOrder) {
        this(byteOrder);
        this.filename = filename;
    }

    /**
     * Returns the length of the file in bytes.
     */
    public int getLength() {
        return dataSize;
    }

    public int size() {
        return getLength();
    }

    public int getPosition() {
        return position;
    }

    public void setPosition(int position) {
        if (position < 0) {
            throw new IllegalArgumentException
                ("Can't set position below 0");
        }
        
        if (position > dataSize) {
            int p = position;
            position = dataSize;
            reserveBytes(dataSize - p);
        }

        this.position = position;
    }

    /** Goes back to the beginning of the file.  */
    public void reset() {
        setPosition(0);
    }

    public void writeUInt8(int v) {
        reserveBytes(1);
        data[position++] = v;
    }

    public void writeInt8(int v) {
        reserveBytes(1);
        data[position++] = v;
    }

    public void writeBool8(boolean v) {
        writeInt8(v ? 1 : 0);
    }

    public void writeUInt16(int v) {
        if (byteOrder == Constants.G3D_LITTLE_ENDIAN) {
            writeUInt8(v & 0xFF);
            writeUInt8(v >> 8);
        } else {
            writeUInt8(v >> 8);
            writeUInt8(v & 0xFF);
        }
    }

    public long readUInt32() {
        if (byteOrder == Constants.G3D_LITTLE_ENDIAN) {
            return 
                readUInt8() + 
                (readUInt8() << 8) + 
                (readUInt8() << 16) + 
                (readUInt8() << 24);
        } else {
            return 
                (readUInt8() << 24) + 
                (readUInt8() << 16) + 
                (readUInt8() << 8) + 
                readUInt8();
        }
    }

    public int readInt8() {
        return (int)data[position++];
    }

    public short readInt16() {
        int i = readUInt16();
        if (i > 32767) {
            i -= 65536;
        }
        return (short)i;
    }

    public int readInt32() {

        long i = readUInt32();
        if (i > 2147483648L) {
            i += 4294967296L;
        }
        return (int)i;
    }

    private BigInteger readInt64BigInteger() {
        byte val[] = new byte[8];        
        if (byteOrder == Constants.G3D_LITTLE_ENDIAN) {
            // Reverse the order
            for (int i = 0; i < 8; ++i) {
                val[i] = data[position + 7 - i];
            }
        } else {
            System.arraycopy(data, position, val, 0, 8);
        }

        setPosition(position + 8);

        return new BigInteger(val);
    }

    /** Java does not support unsigned types, so we must return a BigInteger here. 
        Use .longValue to extract a useful value when it is known to be within the
        signed long range. */
    public BigInteger readUInt64() {
        BigInteger big = readInt64BigInteger();
        
        if (big.compareTo(BigInteger.ZERO) == -1) {            
            // The value was negative, so shift up by long value
            return big.add(maxUInt64);
        } else {
            return big;
        }
    }

    public long readInt64() {
        return readInt64BigInteger().longValue();
    }

    public float readFloat32() {
        return Float.intBitsToFloat(readInt32());
    }

    public double readFloat64() {
        return Double.longBitsToDouble(readInt64());
    }

    /** Reads until NULL or the end of the file is encountered. */
    public String readString() {
        int length = 0;
        while (data[position + length] != '\0') {
            ++length;
        }

        return readString(length + 1);
    }
    
    public String readString32() {
        return readString((int)readUInt32());
    }

    public String readString(int length) {
        String s = new String(data, position, length);
        position += length;
        return s;
    }

    /** Reads until NULL or the end of the file is encountered. If the string has odd length 
        (including NULL), reads another byte. */
	public String readStringEven() {
        int length = 0;
        while (data[position + length] != '\0') {
            ++length;
        }

        return readString(length + 2 - (length % 2));
    } 

    /** Skips ahead n bytes. */
    public void skip(int numBytes) {
        position += numBytes;
    }

    public void readBytes(byte bytes[], int n) {
        System.arraycopy(data, position, bytes, 0, n);
        skip(n);
    }

    public boolean hasMore() {
        return position < data.length;
    }

}
