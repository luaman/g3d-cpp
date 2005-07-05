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

    public void sendBuffer(BinaryOutput b) {
        if (sock.isConnected()) {
            ByteBuffer tmpBuffer = ByteBuffer.allocate(b.size());
            tmpBuffer.put(b.getCArray(), 0, b.size());
            try {
                int written = sock.write(tmpBuffer);
                System.out.printf("%d bytes written.\n", written);
            } catch (IOException e) {
                System.out.printf("Error sending through conduit - %s\n", e.getMessage());
            }
        }
    }

    /** Accumulates whatever part of the message (not the header) is still waiting
        on the socket into the receiveBuffer during state = RECEIVING mode.  
        Closes the socket if anything goes wrong.
        When receiveBufferUsedSize == messageSize, the entire message has arrived. */
    private void receiveIntoBuffer() {
    
        if (sock.isConnected()) {
            try {
                int recieved = sock.read(receiveBuffer);
                if (receiveBuffer.remaining() == 0) {
                    state = State.HOLDING;
                    receiveBufferUsedSize = messageSize;
                } else {
                    receiveBufferUsedSize += messageSize;
                }
            } catch (IOException e) {
            }
        }
    }

    /** Receives the messageType and messageSize from the socket. */
    private void receiveHeader() {
        ByteBuffer headerBuffer = ByteBuffer.allocate(8);
        
        receiveBuffer.clear();
        
        if (sock.isConnected()) {
            try {
                sock.read(headerBuffer);
                if (headerBuffer.remaining() == 0) {
                    messageType = headerBuffer.getInt(0);
                    messageSize = headerBuffer.getInt(4);
                    receiveBuffer.limit(messageSize);
                } else {
                    state = State.NO_MESSAGE;
                    sock.close();
                }
            } catch (IOException e) {
            }
        }
    }

    public ReliableConduit(InetSocketAddress addr){
        receiveBuffer = ByteBuffer.allocate(0);
        try {
            sock = SocketChannel.open(addr);
        } catch (IOException e) {
            System.out.printf("Couldn't open address\n");
        }
    }

    // The message is actually copied from the socket to an internal buffer during
    // this call.  Receive only deserializes.
    public boolean messageWaiting() {
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
    public void send(int type, BinarySerializable message) {
        BinaryOutput binaryOutput = new BinaryOutput(ByteOrder.BIG_ENDIAN);
        binaryOutput.writeInt32(type);
        binaryOutput.writeInt32(0);
        message.serialize(binaryOutput);
        binaryOutput.setPosition(4);
        binaryOutput.writeInt32(binaryOutput.size() - 8);
        sendBuffer(binaryOutput);
    }
    
    /** Send the same message to a number of conduits.  Useful for sending
        data from a server to many clients (only serializes once). */
    static void multisend(
        ReliableConduit[]           array, 
        int                         type,
        BinarySerializable          m) {
        
        if (array.length > 0) {
            BinaryOutput binaryOutput = new BinaryOutput(ByteOrder.BIG_ENDIAN);
            binaryOutput.reset();
            binaryOutput.writeUInt32(type);
            binaryOutput.writeUInt32(0);
            m.serialize(binaryOutput);
            binaryOutput.setPosition(4);
            binaryOutput.writeUInt32(binaryOutput.size() - 8);
        
            for (int i = 0; i < array.length; ++i) {
                array[i].sendBuffer(binaryOutput);
            }
        }
    }

    public int waitingMessageType() {
        if (messageWaiting()) {
            return messageType;
        } else {
            return 0;
        }
    }

    /** 
        If a message is waiting, deserializes the waiting message into
        message and returns true, otherwise returns false.  You can
        determine the type of the message (and therefore, the class
        of message) using G3D::ReliableConduit::waitingMessageType().
     */
    public boolean receive(BinaryDeserializable message) {

        if (! messageWaiting()) {
            return false;
        }

        // Deserialize
        BinaryInput b = new BinaryInput(receiveBuffer.array(), 0, ByteOrder.BIG_ENDIAN, false);
        message.deserialize(b);
        
        // Don't let anyone read this message again.  We leave the buffer
        // allocated for the next caller, however.
        receiveBufferUsedSize = 0;
        state = State.NO_MESSAGE;
        messageType = 0;
        messageSize = 0;

        // Potentially read the next message.
        messageWaiting();

        return true;
    }

    /** Removes the current message from the queue. */
    public void receive() {
        if (! messageWaiting()) {
            return;
        }
        receiveBufferUsedSize = 0;
        state = State.NO_MESSAGE;
        messageType = 0;
        messageSize = 0;

        // Potentially read the next message.
        messageWaiting();
    }

    public InetSocketAddress address() {
        return addr;
    }
}