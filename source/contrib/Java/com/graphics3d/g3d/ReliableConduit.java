package com.graphics3d.g3d;

import java.lang.IndexOutOfBoundsException;
import java.net.*;
import java.io.*;
import java.nio.*;
import java.nio.channels.*;

/**
 Java implementation of ReliableConduit.
 
 Differences from the C++ version:
 <UL>
 </UL> 

 */
public class ReliableConduit {

    private static enum State {RECEIVING, HOLDING, NO_MESSAGE};
    
    // From Conduit
    long                          mSent;
    long                          mReceived;
    long                          bSent;
    long                          bReceived;
    SocketChannel                 sock;
    
    private State state;

    private InetSocketAddress               addr;
    
    /**
     Type of the incoming message.
     */
    private int                             messageType;

    /** 
     Total size of the incoming message (read from the header).
     */
    private int                             messageSize;

    /** Shared buffer for receiving messages. */
    private ByteBuffer                      receiveBuffer;

    /** Total size of the receiveBuffer. */
    private int                             receiveBufferTotalSize;

    /** Size occupied by the current message... so far.  This will be
        equal to messageSize when the whole message has arrived. 
      */
    private int                            receiveBufferUsedSize;

    public void sendBuffer(BinaryOutput b) throws IOException {
        if (sock.isConnected()) {
            ByteBuffer tmpBuffer = ByteBuffer.allocate(b.size());
            tmpBuffer.put(b.getCArray(), 0, b.size());
            tmpBuffer.rewind();
            sock.write(tmpBuffer);
        }
    }

    /** Accumulates whatever part of the message (not the header) is still waiting
        on the socket into the receiveBuffer during state = RECEIVING mode.  
        Closes the socket if anything goes wrong.
        When receiveBufferUsedSize == messageSize, the entire message has arrived. */
    private void receiveIntoBuffer() throws IOException {
    
        if (sock.isConnected()) {
            int recieved = sock.read(receiveBuffer);

            if (receiveBuffer.remaining() == 0) {
                state = State.HOLDING;
                receiveBufferUsedSize = messageSize;
            } else {
                receiveBufferUsedSize += messageSize;
            }
        }
    }

    /** Receives the messageType and messageSize from the socket. */
    private void receiveHeader() throws IOException {
        ByteBuffer headerBuffer = ByteBuffer.allocate(8);
        headerBuffer.order(ByteOrder.LITTLE_ENDIAN);
        
        if (sock.isConnected()) {
            int numRead = sock.read(headerBuffer);
            if (numRead > 0) {
                if (headerBuffer.remaining() == 0) {
                    messageType = headerBuffer.getInt(0);
                    // Size is in network byte order
                    byte sizeArray[] = headerBuffer.array();
                    messageSize = ((int)sizeArray[4] << 24) + 
                        ((int)sizeArray[5] << 16) +
                        ((int)sizeArray[6] << 8) + 
                        (int)sizeArray[7];
                    receiveBuffer = ByteBuffer.allocate(messageSize);
                    state = State.RECEIVING;
                } else {
                    state = State.NO_MESSAGE;
                    sock.close();
                }
            }
        }
    }

    public ReliableConduit(InetSocketAddress addr) throws IOException {
        receiveBuffer = ByteBuffer.allocate(0);
        receiveBufferUsedSize = 0;
        messageType = 0;
        messageSize = 0;
        state = State.NO_MESSAGE;
        sock = SocketChannel.open(addr);
    }

    // The message is actually copied from the socket to an internal buffer during
    // this call.  Receive only deserializes.
    public boolean messageWaiting() throws IOException {
        if (state == State.HOLDING) {
            return true;
        } else if (state == State.RECEIVING) {
            receiveIntoBuffer();
            
            if (receiveBufferUsedSize == messageSize) {
                state = State.HOLDING;
                return true;
            }
        } else if (state == State.NO_MESSAGE) {
            
            receiveHeader();
            
            if (messageSize > 0) {
                return messageWaiting();
            } else {
                return false;
            }
        }
        
        return false;
    }

    /**
     Serializes the message and schedules it to be sent as soon as possible,
     and then returns immediately.  The message can be any <B>class</B> with
     a serialize and deserialize method.  On the receiving side,
     use G3D::ReliableConduit::waitingMessageType() to detect the incoming
     message and then invoke G3D::ReliableConduit::receive(msg) where msg
     is of the same class as the message that was sent.

     The actual data sent across the network is preceeded by the
     message type and the size of the serialized message as a 32-bit
     integer.  The size is sent because TCP is a stream protocol and
     doesn't have a concept of discrete messages.
     */
    public void send(int type, BinarySerializable message) throws IOException {
        BinaryOutput binaryOutput = new BinaryOutput(ByteOrder.LITTLE_ENDIAN);
        
        // Write header
        binaryOutput.writeInt32(type);
        binaryOutput.writeInt32(0);
        
        // Serialize message
        message.serialize(binaryOutput);
        
        // Fix message size
        int size = binaryOutput.size() - 8;
        byte output[] = binaryOutput.getCArray();
        output[4] = (byte)(size >> 24); 
        output[5] = (byte)(size >> 16); 
        output[6] = (byte)(size >> 8); 
        output[7] = (byte)(size & 0xFF);
        
        sendBuffer(binaryOutput);
    }
    
    /** Send the same message to a number of conduits.  Useful for sending
        data from a server to many clients (only serializes once). */
    static void multisend(
        ReliableConduit[]           array, 
        int                         type,
        BinarySerializable          m) throws IOException {
        
        if (array.length > 0) {
            BinaryOutput binaryOutput = new BinaryOutput(ByteOrder.LITTLE_ENDIAN);

            // Write header
            binaryOutput.writeInt32(type);
            binaryOutput.writeInt32(0);

            // Serialize message
            m.serialize(binaryOutput);
        
            // Fix message size
            int size = binaryOutput.size() - 8;
            byte output[] = binaryOutput.getCArray();
            output[4] = (byte)(size >> 24); 
            output[5] = (byte)(size >> 16); 
            output[6] = (byte)(size >> 8); 
            output[7] = (byte)(size & 0xFF);
        
            for (int i = 0; i < array.length; ++i) {
                array[i].sendBuffer(binaryOutput);
            }
        }
    }

    public int waitingMessageType() throws IOException {
        if (messageWaiting()) {
            return messageType;
        } else {
            return 0;
        }
    }

    public boolean ok() {
        return (sock != null) && sock.isConnected();
    }

    /** 
        If a message is waiting, deserializes the waiting message into
        message and returns true, otherwise returns false.  You can
        determine the type of the message (and therefore, the class
        of message) using G3D::ReliableConduit::waitingMessageType().
     */
    public boolean receive(BinaryDeserializable message) throws IOException {

        if (! messageWaiting()) {
            return false;
        }

        // Deserialize
        BinaryInput b = new BinaryInput(receiveBuffer.array(), 0, ByteOrder.LITTLE_ENDIAN, false);
        message.deserialize(b);
        
        // Don't let anyone read this message again.  We leave the buffer
        // allocated for the next caller, however.
        receiveBufferUsedSize = 0;
        state = State.NO_MESSAGE;
        messageType = 0;
        messageSize = 0;
        receiveBuffer.clear();

        // Potentially read the next message.
        messageWaiting();

        return true;
    }

    /** Removes the current message from the queue. */
    public void receive() throws IOException {
        if (! messageWaiting()) {
            return;
        }
        receiveBufferUsedSize = 0;
        state = State.NO_MESSAGE;
        messageType = 0;
        messageSize = 0;
        receiveBuffer.clear();

        // Potentially read the next message.
        messageWaiting();
    }

    public InetSocketAddress address() {
        return addr;
    }
}
