package com.graphics3d.g3d;

import java.lang.IndexOutOfBoundsException;
import java.lang.Long;
import java.net.*;
import java.io.*;
import java.math.BigInteger;
import java.nio.ByteOrder;

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
    private ByteOrder          byteOrder;

    /** Resize dataSize/data as necessary so that there are numBytes of space after the current position */
    private void reserveBytes(int numBytes) {
        if (position + numBytes > data.length) {
            byte temp[] = data;

            // Overestimate the amount of space needed
            data = new byte[(position + numBytes) * 2];
            System.arraycopy(temp, 0, data, 0, temp.length);
        }

        dataSize = Math.max(position + numBytes, dataSize);
    }

    public BinaryOutput(ByteOrder byteOrder) {
        this.byteOrder = byteOrder;
        dataSize = 0;
        filename = "";
        position = 0;
        data = new byte[128];
    }
    
    public BinaryOutput(String filename, ByteOrder byteOrder) {
        this.byteOrder = byteOrder;
        this.filename = filename;
        dataSize = 0;
        position = 0;
        data = new byte[128];
    }

    byte[] getCArray() {
        return data;
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
        data[position++] = (byte)v;
    }

    public void writeInt8(int v) {
        reserveBytes(1);
        data[position++] = (byte)v;
    }

    public void writeBool8(boolean v) {
        writeInt8(v ? 1 : 0);
    }

    public void writeUInt16(int v) {
        if (byteOrder == ByteOrder.LITTLE_ENDIAN) {
            writeUInt8(v & 0xFF);
            writeUInt8(v >> 8);
        } else {
            writeUInt8(v >> 8);
            writeUInt8(v & 0xFF);
        }
    }

    public void writeInt32(int v) {
        if (byteOrder == ByteOrder.LITTLE_ENDIAN) {
            writeInt8(v & 0xFF); 
            writeInt8(v >> 8); 
            writeInt8(v >> 16); 
            writeInt8(v >> 24);
        } else {
            writeInt8(v >> 24); 
            writeInt8(v >> 16); 
            writeInt8(v >> 8); 
            writeInt8(v & 0xFF);
        }
    }
    
    public void writeUInt32(int v) {
        if (byteOrder == ByteOrder.LITTLE_ENDIAN) {
            writeUInt8(v & 0xFF); 
            writeUInt8(v >> 8); 
            writeUInt8(v >> 16); 
            writeUInt8(v >> 24);
        } else {
            writeUInt8(v >> 24); 
            writeUInt8(v >> 16); 
            writeUInt8(v >> 8); 
            writeUInt8(v & 0xFF);
        }
    }

    /** Skips ahead n bytes. */
    public void skip(int numBytes) {
        position += numBytes;
    }

    public boolean hasMore() {
        return position < data.length;
    }
    
    public void commit() {
        try {
            FileOutputStream output = new FileOutputStream(filename);

            try {
                output.write(data, 0, dataSize);        
                output.flush();
            } catch (IOException e) {
                // couldn't write data or flush
            }
        } catch (FileNotFoundException e) {
            // Invalid or missing filename.
        }
    }

}
