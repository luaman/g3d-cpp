package com.graphics3d.g3d;

import java.lang.IndexOutOfBoundsException;
import java.lang.reflect.Array;
import java.lang.Long;
import java.net.*;
import java.io.*;

/**
 * Random Access Input object
 * @author Morgan McGuire & Max McGuire
 */
public class BinaryInput {

    /**
     * Index of the next byte to be used.
     */
    private int position;
    private byte data[];

    public int byteOrder = Constants.littleEndian;

    /** Copies the data by default */
    public BinaryInput(byte data[], int byteOrder) {
        BinaryInput(data, byteOrder, true);
    }

    public BinaryInput(byte data[], int byteOrder, boolean copyMemory) {
        this.byteOrder = byteOrder;
        if (copyMemory) {
            this.data = new byte[data.length];
            System.arraycopy(data, 0, this.data, 0, data.length); 
        } else {
            this.data = data;
        }
    }

    public BinaryInput(URL url, int byteOrder) throws IOException {
        this(url.openStream(), byteOrder);
    }

    public BinaryInput(String filename, int byteOrder) 
        throws FileNotFoundException, IOException {
        this(new FileInputStream(filename), byteOrder);
    }

    /**
     * Clones the data array.
     */
    public BinaryInput(byte data[], int offset, int length) {
        this(data, offset, length, Constants.G3D_LITTLE_ENDIAN);
    }

    /**
     * Clones the data array.
     */
    public BinaryInput(byte data[], int offset, int length, int byteOrder) {
        this.data = new byte[length];
        System.arraycopy(data, offset, this.data, 0, length);
    }

    /**
     * Closes the stream when initialization is done.
     */
    public BinaryInput(InputStream stream) throws IOException {
        this(stream, Constants.G3D_LITTLE_ENDIAN);
    }

    /**
     * Closes the stream when initialization is done.
     */
    public BinaryInput(InputStream stream, int byteOrder) throws IOException {
        this.byteOrder = byteOrder;

        // Read whatever is immediately available
        int dataSize = stream.available();
        byte data[] = new byte[dataSize];
        int numBytes = 0;

        numBytes = stream.read(data, numBytes, dataSize);

        int buffer;
        buffer = stream.read();

        // read returns -1 when stream is empty
        while (buffer > -1) {
            if (numBytes == dataSize) {
                // resize the array
                int newDataSize = dataSize * 2;
                byte newData[] = new byte[newDataSize];
                for (int d = 0; d < dataSize; d++) {
                    newData[d] = data[d];
                }
                data = newData;
                dataSize = newDataSize;
            }
            data[numBytes++] = (byte)buffer;
            buffer = stream.read();
        }

        stream.close();

        if (numBytes == dataSize) {
            this.data = data;
        } else {
            // resize the array
            this.data = new byte[numBytes];
            for (int d = 0; d < numBytes; d++) {
                this.data[d] = data[d];
            }
        }

        position = 0;
    }

    public void close() {
        // Don't really need to do anything here, but it is good form
        // to maintain stream semantics and provide a close method.
    }

    /**
     * Returns the length of the file in bytes.
     */
    public int getLength() {
        return Array.getLength(data);
    }

    public int getPosition() {
        return position;
    }

    public void setPosition(int position) {
        if (position > Array.getLength(data)) {
            throw new IllegalArgumentException(
                                               "Can't set position past 1 + end of file (file length = " + 
                                               Array.getLength(data) + ")");
        }
        this.position = position;
    }

    public int readUInt8() {
        int i = readInt8();
        if (i < 0) {
            i += 256;
        }
        return i;
    }

    public int readUInt16() {
        if (byteOrder == Constants.G3D_LITTLE_ENDIAN) {
            return readUInt8() + (readUInt8() << 8);
        } else {
            return (readUInt8() << 8) + readUInt8();
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

    public int readInt16() {
        int i = readUInt16();
        if (i > 32767) {
            i -= 65536;
        }
        return i;
    }

    public int readInt32() {

        long i = readUInt32();
        if (i > 2147483648L) {
            i += 4294967296L;
        }
        return (int)i;
    }

    public float readFloat32() {
        return Float.intBitsToFloat(readInt32());
    }

    public String readString(int length) {
        String s = new String(data, position, length);
        position += length;
        return s;
    }


    public void skip(int numBytes) {
        position += numBytes;
    }
}



