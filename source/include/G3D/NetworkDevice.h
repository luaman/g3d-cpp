/**
 @file NetworkDevice.h

 These classes abstract networking from the socket level to a serialized messaging
 style that is more appropriate for games.  The performance has been tuned for 
 sending many small messages.

 LightweightConduit and ReliableConduits have different interfaces because
 they have different semantics.  You would never want to interchange them without
 rewriting the surrounding code.

 NetworkDevice creates conduits because they need access to a global log pointer
 and because I don't want non-reference counted conduits being created.

 Be careful with threads and reference counting.  The reference counters are not
 threadsafe, and are also not updated correctly if a thread is explicitly killed.
 Since the conduits will be passed by const XConduitRef& most of the time this
 doesn't appear as a major problem.  With non-blocking conduits, you should need
 few threads anyway.

 ReliableConduits preceed each message with a 4-byte host order unsigned integer
 indicating the length of the rest of the data.  That is, this header does not
 include the size of the header itself.  The minimum message is 5 bytes, a 4-byte
 header of "1" and one byte of data.

 @maintainer Morgan McGuire, morgan@graphics3d.com
 @created 2002-11-22
 @edited  2003-01-03
 */

#ifndef NETWORKDEVICE_H
#define NETWORKDEVICE_H

#include <string>
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

    /** Returns true if this is not an illegal address. */
    bool ok() const;

    /** Returns a value in host format */
    uint32 ip() const {
        return ntohl(addr.sin_addr.s_addr);
        //return ntohl(addr.sin_addr.S_un.S_addr);
    }

    uint16 port() const {
        return ntohs(addr.sin_port);
    }

    std::string ipString() const;
    std::string toString() const;
};



/**
 Interface for data sent through a conduit.
 */
class NetMessage {
public:
    virtual ~NetMessage() {}
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
    virtual ~Conduit();

public:

    uint64 bytesSent() const;
    uint64 messagesSent() const;
    uint64 bytesReceived() const;
    uint64 messagesReceived() const;

    /**
     If true, receive will return true.
     */
    bool messageWaiting() const;

    /** Returns true if the connection is ok. */
    bool ok() const;
};


class ReliableConduit : public Conduit {
private:
    friend class NetworkDevice;
    friend class NetListener;

    NetAddress                      addr;

    ReliableConduit(class NetworkDevice* _nd, const NetAddress& addr);

    ReliableConduit(class NetworkDevice* _nd, const SOCKET& sock, const NetAddress& addr);

    /** Closes the socket. */
    ~ReliableConduit();

public:

    /**
     Serializes the message and schedules it to be
     sent as soon as possible, then returns immediately.
     */
    void send(const NetMessage* m);
    
    /** If data is waiting, deserializes the waiting message into m and
        returns true, otherwise returns false.*/
    bool receive(NetMessage* m);

    NetAddress address() const;
};


typedef ReferenceCountedPointer<class ReliableConduit> ReliableConduitRef;


/**
 Provides fast but unreliable transfer of messages.  LightweightConduits
 are implemented using UDP.
 */
class LightweightConduit : public Conduit {
private:
    friend class NetworkDevice;
    
    LightweightConduit(class NetworkDevice* _nd, uint16 receivePort, bool enableReceive, bool enableBroadcast);

    /** Closes the socket. */
    ~LightweightConduit();

public:

    /** Serializes and sends the message immediately. Data may not arrive and may
        arrive out of order, but individual messages are guaranteed to not be
        corrupted. */
    void send(const NetAddress& a, const NetMessage* m);

    /** If data is waiting, deserializes the waiting message into m, puts the
        sender's address in addr and
        returns true, otherwise returns false.
        @param maxSize Maximum size of the message, in bytes.  Must be less than 8k. */
    bool receive(NetMessage* m, NetAddress& sender, int maxSize = 512);
};

typedef ReferenceCountedPointer<class LightweightConduit> LightweightConduitRef;

//////////////////////////////////////////////////////////////////////////////////

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
    ~NetListener();

public:

    /** Block until a connection is received.  Returns NULL if 
        something went wrong. */
    ReliableConduitRef waitForConnection();

    /** True if a client is waiting (i.e. waitForConnection will return immediately). */
    bool clientWaiting() const;

    bool ok() const;
};

typedef ReferenceCountedPointer<class NetListener> NetListenerRef;

//////////////////////////////////////////////////////////////////////////////////

/**
 An abstraction over sockets that provides a message based network infrastructure
 optimized for sending many small (>500 byte) messages.  All functions always return
 immediately.
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

    /** Utility method. */
    void bind(SOCKET sock, const NetAddress& addr) const;

public:

    NetworkDevice();

    /** Returns the name of this computer */
    std::string localHostName() const;

    /** There is often more than one address for the local host. This returns all of them. */
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
    LightweightConduitRef createLightweightConduit(uint16 receivePort = 0, bool enableReceive = false, bool enableBroadcast = false);

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
