/**
 @file NetworkDevice.h

 These classes abstract networking from the socket level to a
 serialized messaging style that is more appropriate for games.  The
 performance has been tuned for sending many small messages.  The
 message protocol contains a header that prevents them from being used
 with raw UDP/TCP (e.g. connecting to an HTTP server). 

 LightweightConduit and ReliableConduits have different interfaces
 because they have different semantics.  You would never want to
 interchange them without rewriting the surrounding code.

 NetworkDevice creates conduits because they need access to a global
 log pointer and because I don't want non-reference counted conduits
 being created.

 Be careful with threads and reference counting.  The reference
 counters are not threadsafe, and are also not updated correctly if a
 thread is explicitly killed.  Since the conduits will be passed by
 const XConduitRef& most of the time this doesn't appear as a major
 problem.  With non-blocking conduits, you should need few threads
 anyway.

 LightweightConduits preceed each message with a 4-byte host order
 unsigned integer that is the message type.  This does not appear in
 the message serialization/deserialization.

 ReliableConduits preceed each message with two 4-byte host order
 unsigned integers.  The first is the message type and the second
 indicates the length of the rest of the data.  The size does not
 include the size of the header itself.  The minimum message is 9
 bytes, a 4-byte types, a 4-byte header of "1" and one byte of data.

 @maintainer Morgan McGuire, morgan@graphics3d.com
 @created 2002-11-22
 @edited  2004-04-02
 */

#ifndef NETWORKDEVICE_H
#define NETWORKDEVICE_H

#include "G3D/platform.h"
#include <string>
#include <iostream>
#include "G3D/g3dmath.h"
#ifdef _WIN32
    #include <winsock.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #define SOCKADDR_IN struct sockaddr_in
    #define SOCKET int
#endif
#include "G3D/ReferenceCount.h"
#include "G3D/Array.h"

namespace G3D {

class NetAddress {
private:
    friend class NetworkDevice;
    friend class LightweightConduit;
    friend class ReliableConduit;

    /** Host byte order */
    void init(uint32 host, uint16 port);
    void init(const std::string& hostname, uint16 port);
    NetAddress(const SOCKADDR_IN& a);
    NetAddress(const struct in_addr& addr, uint16 port = 0);

    SOCKADDR_IN                 addr;

public:
    /**
     In host byte order
     */
    NetAddress(uint32 host, uint16 port = 0);

    /**
     Port is in host byte order.
     */
    NetAddress(const std::string& hostname, uint16 port = 0);

    /**
    String must be in the form "hostname:port"
     */
    NetAddress(const std::string& hostnameAndPort);

    /**
     For use with a lightweight conduit.
     */
    static NetAddress broadcastAddress(uint16 port);

    NetAddress();

    void serialize(class BinaryOutput& b) const;
    void deserialize(class BinaryInput& b);

    /** Returns true if this is not an illegal address. */
    bool ok() const;

    /** Returns a value in host format */
    inline uint32 ip() const {
        return ntohl(addr.sin_addr.s_addr);
        //return ntohl(addr.sin_addr.S_un.S_addr);
    }

    inline uint16 port() const {
        return ntohs(addr.sin_port);
    }

    std::string ipString() const;
    std::string toString() const;

};

std::ostream& operator<<(std::ostream& os, const NetAddress&);

inline unsigned int hashCode(const NetAddress& a) {
	return a.ip() + ((uint32)a.port() << 16);
}


/**
 Two addresses may point to the same computer but be != because
 they have different IP's.
 */
inline bool operator==(const NetAddress& a, const NetAddress& b) {
	return (a.ip() == b.ip()) && (a.port() == b.port());
}


inline bool operator!=(const NetAddress& a, const NetAddress& b) {
    return !(a == b);
}


/**
 Interface for data sent through a conduit.
 NetMessages automatically serialize and deserialize themselves when
 sent through ReliableConduit and LightweightConduit.  An application
 contains many subclasses of NetMessage, one for each kind of message
 (e.g. SignOnMessage, CreatePlayer, ChatMessage, PlaySoundMessage, 
 SignOffMessage).  Because the specific messages needed depend on the
 application, they are not part of G3D.
 */
class NetMessage {
public:
    virtual ~NetMessage() {}

    /** This must return a value even for an uninitalized instance.
       Create an enumeration for your message types and return
       one of those values.  It will be checked on both send and
       receive as a form of runtime type checking. 
    
       Values less than 1000 are reserved for the system.*/
    virtual uint32 type() const = 0;
    virtual void serialize(class BinaryOutput& b) const = 0;
    virtual void deserialize(class BinaryInput& b) = 0;
};


class Conduit : public ReferenceCountedObject {
protected:
    friend class NetworkDevice;
    friend class NetListener;

    uint64                          mSent;
    uint64                          mReceived;
    uint64                          bSent;
    uint64                          bReceived;

    class NetworkDevice*            nd;
    SOCKET                          sock;

    Conduit(class NetworkDevice* _nd);

public:

    virtual ~Conduit();
    uint64 bytesSent() const;
    uint64 messagesSent() const;
    uint64 bytesReceived() const;
    uint64 messagesReceived() const;

    /**
     If true, receive will return true.
     */
    virtual bool messageWaiting() const;

    /**
     Returns the type of the waiting message (i.e. the type supplied
     with send).  The return value is zero when there is no message
     waiting.

     One way to use this is to have a Table mapping message types to
     pre-allocated NetMessage subclasses so receiving looks like:

     <PRE>
         // My base class for messages.
         class Message : public NetMessage {
             virtual void process() = 0;
         };

         Message* m = table[conduit->waitingMessageType()];
         conduit->receive(m);
         m->process();
     </PRE>

      Another is to simply SWITCH on the message type.
     */
    virtual uint32 waitingMessageType() = 0;

    /** Returns true if the connection is ok. */
    bool ok() const;
};

typedef ReferenceCountedPointer<class ReliableConduit> ReliableConduitRef;


// Messaging and stream APIs must be supported on a single class because
// sometimes an application will switch modes on a single socket.  For
// example, when transferring 3D level geometry during handshaking with
// a game server.
/**
 A conduit that guarantees messages will arrive, intact and in order.
 Create on the client using NetworkDevice::createReliableConduit and
 on the server using NetListener::waitForConnection.  Set the reference
 counted pointer to NULL to disconnect.

 To construct a ReliableConduit:
 <OL>
   <LI> Create a G3D::NetworkDevice (if you are using G3D::GApp, it creates 
        one for you) on the client and on the server.
   <LI> On the server, create a G3D::NetListener using 
        G3D::NetworkDevice::createListener
   <LI> On the server, invoke G3D::NetListener::waitForConnection.
   <LI> On the client, call G3D::NetworkDevice::createReliableConduit.  
        You will need the server's G3D::NetAddress.  Consider using
        G3D::DiscoveryClient to find it via broadcasting.
 </OL>

 */
class ReliableConduit : public Conduit {
private:
    friend class NetworkDevice;
    friend class NetListener;

    NetAddress                      addr;
    /**
     True when the messageType has been read but the
     packet has not been read.
     */
    bool                            alreadyReadType;
    
    /**
     Type of the incoming message.
     */
    uint32                          messageType;

    ReliableConduit(class NetworkDevice* _nd, const NetAddress& addr);

    ReliableConduit(class NetworkDevice* _nd, 
                    const SOCKET& sock, 
                    const NetAddress& addr);

    /**
     ReliableConduit messages are serialized with the message size
     (since TCP may divide it across packets) and the message type.
     */
    static void serializeMessage(const NetMessage* m, BinaryOutput& b);

    void sendBuffer(const BinaryOutput& b);

public:

    /** Closes the socket. */
    ~ReliableConduit();

    /**
     Serializes the message and schedules it to be sent as soon as possible,
     then returns immediately.

     The actual data sent across the network is preceeded by the
     message type and the size of the serialized message as a 32-bit
     integer.  The size is sent because TCP is a stream protocol and
     doesn't have a concept of discrete messages.
     */
    void send(const NetMessage* m);

    /** Send the same message to a number of conduits.  Useful for sending
        data from a server to many clients (only serializes once). */
    static void multisend(const Array<ReliableConduitRef>& array, 
                          const NetMessage* m);

    inline static void multisend(const Array<ReliableConduitRef>& array, 
                                 const NetMessage& m) {
        multisend(array, &m);
    }

    inline static void multisend(const Array<ReliableConduitRef>& array) {
        multisend(array, NULL);
    }

    inline void send(const NetMessage& m) {
        send(&m);
    }

    inline void send() {
        send(NULL);
    }

    virtual uint32 waitingMessageType();

    /** If a message is waiting, deserializes the waiting message into
        m and returns true, otherwise returns false.
        
        If m == NULL, the message is pulled from the conduit and discarded.
        
        If a message is incoming but was split across multipled TCP
        packets in transit, this will block for up to .25 seconds
        waiting for all packets to arrive.  For short messages (less
        than 5k) this is extremely unlikely to occur.*/
    bool receive(NetMessage* m);

    inline bool receive(NetMessage& m) {
        return receive(&m);
    }

    NetAddress address() const;
};


/**
 Provides fast but unreliable transfer of messages.  On a LAN,
 LightweightConduit will probably never drop messages but you
 <I>might</I> get your messages out of order.  On an internet
 connection it might drop messages altogether.  Messages are never
 corrupted, however.  LightweightConduit requires a little less setup
 and overhead than ReliableConduit.  ReliableConduit guarantees
 message delivery and order but requires a persistent connection.
 
 To set up a LightweightConduit (assuming you have already made
 subclasses of G3D::NetMessage based on your application's
 pcommunication protocol):

[Server Side]
<OL>
<LI> Create a G3D::NetworkDevice on program startup (if you use G3D::GApp, 
it will do this for you)

<LI> Call G3D::NetworkDevice::createLightweightConduit(port, true, false), 
where port is the port on which you will receive messages.

<LI> Poll G3D::LightWeightcontuit::messageWaiting from your main loop.  When 
it is true (or, equivalently, when G3D::LightWeightcontuit::waitingMessageType
is non-zero) there is an incoming message.

<LI> To read the incoming message, call G3D::LightWeightconduit::receive with 
the appropriate subclass of G3D::NetMessage. 
G3D::LightWeightcontuit::waitingMessageType tells you what subclass is 
needed (you make up your own message constants for your program; numbers 
under 1000 are reserved for G3D's internal use).

<LI> When done, simply set the G3D::LightweightConduitRef to NULL or let 
it go out of scope and the conduit cleans itself up automatically.
</OL>

[Client Side]
<OL>

<LI> Create a G3D::NetworkDevice on program startup (if you use G3D::GApp, 
it will do this for you)

<LI> Call G3D::NetworkDevice::createLightweightConduit().  If you will 
broadcast to all servers on a LAN, set the third optional argument to 
true (the default is false for no broadcast).  You can also set up the
receive port as if it was a server to send and receive from a single 
LightweightConduit.

<LI> To send, call G3D::LightweightConduit::send with the target address 
and a pointer to an instance of the message you want to send.

<LI> When done, simply set the G3D::LightweightConduitRef to NULL or let 
it go out of scope and the conduit cleans itself up automatically.

</OL>
 */
class LightweightConduit : public Conduit {
private:
    friend class NetworkDevice;

    /**
     True when waitingForMessageType has read the message
     from the network into messageType/messageStream.
     */
    bool                    alreadyReadMessage;

    /**
     Origin of the received message.
     */
    NetAddress              messageSender;

    /**
     The type of the last message received.
     */
    uint32                  messageType;

    /**
     The message received (the type has already been read off).
     */
    Array<uint8>            messageBuffer;

    LightweightConduit(class NetworkDevice* _nd, uint16 receivePort, 
                       bool enableReceive, bool enableBroadcast);

    
    /**
     LightweightConduit messages are serialized with the message type
     (the size unnecessary because UDP is not allowed to divide messages).
     */
    void serializeMessage(const NetMessage* m, BinaryOutput& b) const;

    void sendBuffer(const NetAddress& a, BinaryOutput& b);

    /** Maximum transmission unit (packet size in bytes) for this socket.
        May vary between sockets. */
    int                    MTU;

public:
    class PacketSizeException {
    public:
        std::string            message;
        int                    serializedPacketSize;
        int                    maxMessageSize;

        inline PacketSizeException(const std::string& m, int s, int b) :
            message(m),
            serializedPacketSize(s),
            maxMessageSize(b) {}
    };

    /** Closes the socket. */
    ~LightweightConduit();

    /** The maximum length of a message that can be sent
        (G3D places a small header at the front of each UDP packet;
        this is already taken into account by the value returned).
     */
    inline int maxMessageSize() const {
        return MTU - 4;
    }

    /** Serializes and sends the message immediately. Data may not
        arrive and may arrive out of order, but individual messages
        are guaranteed to not be corrupted.  If the message is null,
        an empty message is still sent.

        Throws PacketSizeException if the serialized message exceeds
        maxMessageSize. */
    void send(const NetAddress& a, const NetMessage* m);

    /** Send the same message to multiple addresses (only serializes once).
        Useful when server needs to send to a known list of addresses
        (unlike direct UDP broadcast to all addresses on the subnet) */
    void send(const Array<NetAddress>& a, const NetMessage* m);

    inline void send(const Array<NetAddress>& a, const NetMessage& m) {
        send(a, &m);
    }

    inline void send(const Array<NetAddress>& a) {
        send(a, NULL);
    }

    inline void send(const NetAddress& a, const NetMessage& m) {
        send(a, &m);
    }

    inline void send(const NetAddress& a) {
        send(a, NULL);
    }

    /** If data is waiting, deserializes the waiting message into m,
        puts the sender's address in addr and returns true, otherwise
        returns false.  If m is NULL, the message is consumed but not
        deserialized.
    */
    bool receive(NetMessage* m, NetAddress& sender);

    inline bool receive(NetMessage& m, NetAddress& sender) {
        return receive(&m, sender);
    }

    inline bool receive(NetAddress& sender) {
        return receive(NULL, sender);
    }

    virtual uint32 waitingMessageType();

    virtual bool messageWaiting() const;
};

typedef ReferenceCountedPointer<class LightweightConduit> LightweightConduitRef;

///////////////////////////////////////////////////////////////////////////////

/**
 Runs on the server listening for clients trying to make reliable connections.
 */
class NetListener : public ReferenceCountedObject {
private:

    friend class NetworkDevice;

    class NetworkDevice*            nd;
    SOCKET                          sock;

    /** Port is in host byte order. */
    NetListener(class NetworkDevice* _nd, uint16 port);

public:

    ~NetListener();

    /** Block until a connection is received.  Returns NULL if 
        something went wrong. */
    ReliableConduitRef waitForConnection();

    /** True if a client is waiting (i.e. waitForConnection will
        return immediately). */
    bool clientWaiting() const;

    bool ok() const;
};

typedef ReferenceCountedPointer<class NetListener> NetListenerRef;

///////////////////////////////////////////////////////////////////////////////

/**
 An abstraction over sockets that provides a message based network
 infrastructure optimized for sending many small (>500 byte) messages.
 All functions always return immediately.
 <P>
 Create only one NetworkDevice per-process (WinSock restriction).
 <P>
 NetworkDevice is technically not thread safe.  However, as long as you
 use different conduits on different threads, you will encounter no
 problems sharing the single NetworkDevice across multiple threads.
 This assumes that the underlying WinSock/BSD sockets implementation
 is thread safe.  That is not guaranteed, but in practice seems
 to always be true (e.g.
 http://tangentsoft.net/wskfaq/intermediate.html#threadsafety)

 */
class NetworkDevice {
private:
    friend class Conduit;
    friend class LightweightConduit;
    friend class ReliableConduit;
    friend class NetListener;

    class Log*                  debugLog;

    bool                        initialized;

    /** Utility method. */
    void closesocket(SOCKET& sock) const;

    /** Utility method. Returns true on success.*/
    bool bind(SOCKET sock, const NetAddress& addr) const;

public:

    NetworkDevice();

    /**
     Returns the log this was initialized with.
     */
    Log* log() const {
        return debugLog;
    }

    /** Returns the name of this computer */
    std::string localHostName() const;

    /** There is often more than one address for the local host. This
        returns all of them. */
    void localHostAddresses(Array<NetAddress>& array) const;

    /**
     Returns false if there was a problem initializing the network.
     */
    bool init(class Log* log = NULL);

    /**
     Shuts down the network device.
     */
    void cleanup();

    /**
     If receivePort is specified and enableReceive is true, the conduit can 
     receive as well as send.
     @param receivePort host byte order
     */
    LightweightConduitRef createLightweightConduit(
        uint16 receivePort = 0, 
        bool enableReceive = false, 
        bool enableBroadcast = false);

    /**
     Client invokes this to connect to a server.  The call blocks until the 
     conduit is opened.  The conduit will not be ok() if it fails.
     */
    ReliableConduitRef createReliableConduit(const NetAddress& address);

    /**
     Call this on the server side to create an object that listens for
     connections.
     */
    NetListenerRef createListener(const uint16 port);
};

}

#ifndef _WIN32
#undef SOCKADDR_IN
#undef SOCKET
#endif

#endif
