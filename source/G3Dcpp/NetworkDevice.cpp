/**
 @file NetworkDevice.cpp

 @maintainer Morgan McGuire, morgan@cs.brown.edu
 @created 2002-11-22
 @edited  2003-06-29
 */

#include <stdlib.h>
#include <time.h>
#include "G3D/platform.h"

#ifdef G3D_WIN32
    #include <malloc.h>
#endif

#include "G3D/NetworkDevice.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/Log.h"
#include "G3D/G3DGameUnits.h"
#include "G3D/stringutils.h"
#include "G3D/debug.h"

namespace G3D {

#if defined(G3D_LINUX) || defined(G3D_OSX)
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/tcp.h>
    #define _alloca alloca
    /** Define an error code for non-windows platforms. */
    int WSAGetLastError() {
        return -1;
    }

    #define SOCKET_ERROR -1

    static std::string windowsErrorCode(int code) {
        return "";
    }

    static std::string windowsErrorCode() {
        return "";
    }

    static const int WSAEWOULDBLOCK = -100;

    typedef int SOCKET;
    typedef struct sockaddr_in SOCKADDR_IN;

#else 

    // Windows
    static std::string windowsErrorCode(int code) {
        LPTSTR formatMsg = NULL;

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_IGNORE_INSERTS |
                      FORMAT_MESSAGE_FROM_SYSTEM,
                        NULL,
                        code,
                        0,
                        (LPTSTR)&formatMsg,
                        0,
                        NULL);

        return format("CODE %d: %s\n", code, formatMsg);
    }

    static std::string windowsErrorCode() {
        return windowsErrorCode(GetLastError());
    }

#endif


#if defined(G3D_WIN32) || defined(G3D_OSX)
    typedef int socklen_t;
#endif


static void logSocketInfo(Log* debugLog, const SOCKET& sock) {
    uint32 val;
    socklen_t sz = 4;
    int ret;

    ret = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&val, (socklen_t*)&sz);
    debugLog->printf("SOL_SOCKET/SO_RCVBUF = %d\n", val);

    ret = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&val, (socklen_t*)&sz);
    debugLog->printf("SOL_SOCKET/SO_SNDBUF = %d\n", val);

    // Note: timeout = 0 means no timeout
    ret = getsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&val, (socklen_t*)&sz);
    debugLog->printf("SOL_SOCKET/SO_RCVTIMEO = %d\n", val);

    ret = getsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&val, (socklen_t*)&sz);
    debugLog->printf("SOL_SOCKET/SO_SNDTIMEO = %d\n", val);
}


////////////////////////////////////////////////////////////////////////////////////////////////////

/** Invokes select on one socket */
static int selectOneReadSocket(const SOCKET& sock) {
    // 0 time timeout is specified to poll and return immediately
    struct timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;

    // Create a set that contains just this one socket
    fd_set socketSet;
    FD_ZERO(&socketSet); 
    FD_SET(sock, &socketSet);

    return select(1, &socketSet, NULL, NULL, &timeout);
}


/** Returns true if the socket has a read pending */
static bool readWaiting(Log* debugLog, const SOCKET& sock) {
    int ret = selectOneReadSocket(sock);

    switch (ret) {
    case SOCKET_ERROR:
        if (debugLog) {
            debugLog->println("ERROR: selectOneReadSocket returned SOCKET_ERROR in readWaiting().");
            debugLog->println(windowsErrorCode());
        }
        return true;

    case 0:
        return false;

    case 1:
        return true;

    default:
        if (debugLog) {
            debugLog->printf("WARNING: selectOneReadSocket returned %d in readWaitingg().\n", ret);
        }
        return true;
    }
}


/** Invokes select on one socket */
static int selectOneWriteSocket(const SOCKET& sock) {
    // 0 time timeout is specified to poll and return immediately
    struct timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;

    // Create a set that contains just this one socket
    fd_set socketSet;
    FD_ZERO(&socketSet); 
    FD_SET(sock, &socketSet);

    return select(1, NULL, &socketSet, NULL, &timeout);
}

////////////////////////////////////////////////////////////////////////////////////////////////

NetAddress::NetAddress() {
    memset(&addr, 0, sizeof(addr));
}


void NetAddress::init(uint32 host, uint16 port) {
    if ((host != 0) || (port != 0)) {
        addr.sin_family      = AF_INET;
        addr.sin_port        = htons(port);
        if (host == 0) {
            host = INADDR_ANY;
        }
        addr.sin_addr.s_addr = htonl(host);
    } else {
        memset(&addr, 0, sizeof(addr));
    }
}


NetAddress::NetAddress(
    const std::string&          hostname,
    uint16                      port) {
    init(hostname, port);
}


void NetAddress::init(
    const std::string&          hostname,
    uint16                      port) {

    uint32 addr;
    
    if (hostname == "") {
        addr = INADDR_NONE;
    } else {
        addr = inet_addr(hostname.c_str());
    }

	// The address wasn't in numeric form, resolve it
    if (addr == INADDR_NONE) {
        // Get the IP address of the server and store it in host
        struct hostent* host = gethostbyname(hostname.c_str());

        if (host == NULL) {
            //debugLog->printf("Unable to resolve host: %s\n", hostname.c_str());
            //debugLog->printf("%d\n", WSAGetLastError());
            init(0, 0);
            return;
        }

        uint32 a;
        memcpy(&a, host->h_addr_list[0], host->h_length);
        addr = htonl(a);
    }

    init(addr, port);
}


NetAddress::NetAddress(uint32 hostip, uint16 port) {
    init(hostip, port);
}


NetAddress NetAddress::broadcastAddress(uint16 port) {
    return NetAddress(0xFFFFFFFF, port);
}


NetAddress::NetAddress(const std::string& hostnameAndPort) {

    Array<std::string> part = stringSplit(hostnameAndPort, ':');

    debugAssert(part.length() == 2);
    init(part[0], atoi(part[1].c_str()));
}


NetAddress::NetAddress(const SOCKADDR_IN& a) {
    addr = a;
}


NetAddress::NetAddress(const struct in_addr& addr, uint16 port) {
#ifdef G3D_WIN32
    init(ntohl(addr.S_un.S_addr), port);
#else
    init(htonl(addr.s_addr), port);
#endif
}


void NetAddress::serialize(class BinaryOutput& b) const {
    b.writeUInt32(ip());
    b.writeUInt16(port());
}


void NetAddress::deserialize(class BinaryInput& b) {
    uint32 i;
    uint16 p;

    i = b.readUInt32();
    p = b.readUInt16();

    init(i, p);
}


bool NetAddress::ok() const {
    return addr.sin_family != 0;
}


std::string NetAddress::ipString() const {
    return format("%s", inet_ntoa(*(in_addr*)&(addr.sin_addr)));
}

std::string NetAddress::toString() const {
    return ipString() + format(":%d", ntohs(addr.sin_port));
}

////////////////////////////////////////////////////////////////////////////////////////////////

NetworkDevice::NetworkDevice() {
    initialized     = false;
    debugLog        = NULL;
}


std::string NetworkDevice::localHostName() const {
    struct hostent* host = gethostbyname("localhost");
    return host->h_name;
}


bool NetworkDevice::init(class Log* _log) {
    debugAssert(!initialized);
    debugLog = _log;

    #ifdef G3D_WIN32
        if (debugLog) {
            debugLog->section("Network Startup");
            debugLog->println("Starting WinSock networking.\n");
        }
        WSADATA wsda;		    
        WSAStartup(MAKEWORD(1,1), &wsda);

        if (debugLog) {
            std::string machine = localHostName();
            std::string addr    = NetAddress(machine, 0).ipString();
            debugLog->printf(
                "Network:\n"
                "  localhost = %s (%s)\n"
                "  %s\n"
                "  Status: %s\n"
                "  Loaded winsock specification version %d (%d is the highest available)\n"
                "  %d sockets available\n"
                "  Largest UDP datagram packet size is %d bytes\n\n",
                machine.c_str(), addr.c_str(),
                wsda.szDescription,
                wsda.szSystemStatus,
                wsda.wVersion,
                wsda.wHighVersion,
                wsda.iMaxSockets,
                wsda.iMaxUdpDg);
        }
    #endif


    if (debugLog) {debugLog->section("Testing Network");}

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (debugLog) {debugLog->print("Open Socket                  ");}
    if (sock == SOCKET_ERROR) {
        if (debugLog) {
            debugLog->println("FAIL");
            debugLog->println(windowsErrorCode());
        }
        return false;
    }
    if (debugLog) { debugLog->println("Ok"); }

    int TR = true;
    int ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&TR, sizeof(TR));

    if (debugLog) {debugLog->print("Enable UDP Broadcast         ");}
    if (ret != 0) {
        if (debugLog) {
            debugLog->println("FAIL");
            debugLog->println(windowsErrorCode());
        }
        return false;
    }
    if (debugLog) {debugLog->println("Ok");}

    if (debugLog) {debugLog->print("Testing UDP Broadcast        ");}
    SOCKADDR_IN addr;
    int32 x;
    addr = NetAddress(0xFFFFFFFF, 23).addr;
    ret = sendto(sock, (const char*)&x, sizeof(x), 0, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == SOCKET_ERROR) {
        if (debugLog) {
            debugLog->println("FAIL");
            debugLog->println(windowsErrorCode());
        }
        return false;
    }
    if (debugLog) {debugLog->println("Ok");}

    if (debugLog) {debugLog->section("");}
    initialized = true;

    return true;
}


void NetworkDevice::cleanup() {
    debugAssert(initialized);

    #ifdef G3D_WIN32
        if (debugLog) {debugLog->section("Network Cleanup");}
        WSACleanup();
        if (debugLog) {debugLog->println("Network cleaned up.");}
    #endif
}


LightweightConduitRef NetworkDevice::createLightweightConduit(
    uint16                      receivePort,
    bool                        enableReceive,
    bool                        enableBroadcast) {
    
    return new LightweightConduit(this, receivePort, enableReceive, enableBroadcast);
}


ReliableConduitRef NetworkDevice::createReliableConduit(
    const NetAddress&           address) {

    return new ReliableConduit(this, address);
}


NetListenerRef NetworkDevice::createListener(const uint16 port) {
    return new NetListener(this, port);
}
 

void NetworkDevice::bind(SOCKET sock, const NetAddress& addr) const {
    if (debugLog) { debugLog->printf("Binding socket %d on port %d    ", sock, htons(addr.addr.sin_port));}

    if (::bind(sock, (struct sockaddr*)&(addr.addr), sizeof(addr.addr)) == SOCKET_ERROR) {
        if (debugLog) {
            debugLog->println("FAIL");
            debugLog->println(windowsErrorCode());
        }
        closesocket(sock);
        return;
    }

    if (debugLog) {debugLog->println("Ok");}
}


void NetworkDevice::closesocket(SOCKET& sock) const {
    if (sock != 0) {
        #ifdef G3D_WIN32
                ::closesocket(sock);
        #else
	        close(sock);
        #endif

        if (debugLog) {debugLog->printf("Closed socket %d\n", sock);}
        sock = 0;
    }
}


void NetworkDevice::localHostAddresses(Array<NetAddress>& array) const {
    array.resize(0);

    char ac[128];

    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
        if (debugLog) {debugLog->printf("Error while getting local host name\n");}
        return;
    }

    struct hostent* phe = gethostbyname(ac);
    if (phe == 0) {
        if (debugLog) {debugLog->printf("Error while getting local host address\n");}
        return;
    }

    for (int i = 0; (phe->h_addr_list[i] != 0); ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        array.append(NetAddress(addr));
    }    
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conduit::Conduit(NetworkDevice* _nd) {
    sock                = 0;
    nd                  = _nd;
    mSent               = 0;
    mReceived           = 0;
    bSent               = 0;
    bReceived           = 0;
}


Conduit::~Conduit() {
    nd->closesocket(sock);
}


uint64 Conduit::bytesSent() const {
    return bSent;
}


uint64 Conduit::bytesReceived() const {
    return bReceived;
}


uint64 Conduit::messagesSent() const {
    return mSent;
}


uint64 Conduit::messagesReceived() const {
    return mReceived;
}


bool Conduit::ok() const {
    return sock != 0;
}


bool Conduit::messageWaiting() const {
    return readWaiting(nd->debugLog, sock);
}

////////////////////////////////////////////////////////////////////////////////////////////////////


ReliableConduit::ReliableConduit(NetworkDevice* _nd, const NetAddress& _addr) : Conduit(_nd) {

    messageType         = 0;
    alreadyReadType     = false;

    addr = _addr;
    if (nd->debugLog) {nd->debugLog->print("Creating a TCP socket       ");}
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    
    if (sock == SOCKET_ERROR) {
        if (nd->debugLog) {
            nd->debugLog->println("FAIL");
            nd->debugLog->println(windowsErrorCode());
        }
        nd->closesocket(sock);
        return;
    }
    if (nd->debugLog) {
        nd->debugLog->println("Ok");
    }

    // Disable Nagle's algorithm (we send lots of small packets)
    const int T = true;
    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&T, sizeof(T)) == SOCKET_ERROR) {
        if (nd->debugLog) {
            nd->debugLog->println("WARNING: Disabling Nagel's algorithm failed.");
            nd->debugLog->println(windowsErrorCode());
        }
    }

    // Ideally, we'd like to specify IPTOS_LOWDELAY as well.

    if (nd->debugLog) {
        logSocketInfo(nd->debugLog, sock);
    }

    // Increase the buffer size; the default (8192) is too easy to overflow when the network
    // latency is high.
    {
        uint32 val = 262144;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(val)) == SOCKET_ERROR) {
            if (nd->debugLog) {
                nd->debugLog->printf("WARNING: Increasing socket receive buffer to %d failed.\n", val);
                nd->debugLog->println(windowsErrorCode());
            }
        }

        if (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val)) == SOCKET_ERROR) {
            if (nd->debugLog) {
                nd->debugLog->printf("WARNING: Increasing socket send buffer to %d failed.\n", val);
                nd->debugLog->println(windowsErrorCode());
            }
        }
    }

    if (nd->debugLog) {nd->debugLog->printf("Created TCP socket %d\n", sock);}

    std::string x = addr.toString();
    if (nd->debugLog) {nd->debugLog->printf("Connecting to %s on TCP socket %d   ", x.c_str(), sock);}

    int ret = connect(sock, (struct sockaddr *) &(addr.addr), sizeof(addr.addr));

    if (ret == WSAEWOULDBLOCK) {

        RealTime t = System::getTick() + 5;
        // Non-blocking; we must wait in select
        while ((selectOneWriteSocket(sock) == 0) && (System::getTick() < t)) {
            #ifdef G3D_WIN32
                Sleep(2);
            #endif
        }

    } else if (ret != 0) {
        if (nd->debugLog) {
            nd->debugLog->println("FAIL");
            nd->debugLog->println(windowsErrorCode());
        }
        return;
    }

    if (nd->debugLog) {nd->debugLog->println("Ok");}
}


ReliableConduit::ReliableConduit(class NetworkDevice* _nd, const SOCKET& _sock, const NetAddress& _addr) : Conduit(_nd) {
    sock                = _sock;
    addr                = _addr;

    messageType         = 0;
    alreadyReadType     = false;
}


ReliableConduit::~ReliableConduit() {
}



uint32 ReliableConduit::waitingMessageType() {
    if (! messageWaiting()) {
        return 0;
    }
    
    if (! alreadyReadType) {
        // Read the type
        uint32 tmp;
        int ret = recv(sock, (char*)&tmp, sizeof(tmp), 0);
        alreadyReadType = true;

        // The type is the first four bytes.  It is little endian.
        if (System::machineEndian() == G3D_LITTLE_ENDIAN) {
            messageType = tmp;
        } else {
            // Swap the byte order
            for (int i = 0; i < 4; ++i) {
                ((char*)&messageType)[i] = ((char*)&tmp)[3 - i];
            }
        }

        if ((ret == SOCKET_ERROR) || (ret != sizeof(messageType))) {
            if (nd->debugLog) {
                nd->debugLog->printf("Call to recv failed.  ret = %d, sizeof(messageType) = %d\n", ret, sizeof(messageType));
                nd->debugLog->println(windowsErrorCode());
            }
            nd->closesocket(sock);
            messageType = 0;
        }
    }

    return messageType;
}


void ReliableConduit::send(const NetMessage* m) {

    BinaryOutput b("<memory>", G3D_LITTLE_ENDIAN);

    if (m == NULL) {
        b.writeUInt32(1);
    } else {
        debugAssert(m->type() != 0);
        b.writeUInt32(m->type());
    }

    // Reserve space for the 4 byte size header
    b.writeUInt32(0);

    if (m != NULL) {
        m->serialize(b);
    } else {
        // We need to send at least one byte because receive assumes that
        // a zero length packet is an error.
        b.writeUInt8(-1);
    }
    
    uint32 len = b.getLength() - 8;
    
    // We send the length first to tell recv how much data to read.
    // Here we abuse BinaryOutput a bit and write directly into
    // its buffer, violating the abstraction.
    ((uint32*)b.getCArray())[1] = htonl(len);

    int ret = ::send(sock, (const char*)b.getCArray(), b.getLength(), 0);
    
    if (ret == SOCKET_ERROR) {
        if (nd->debugLog) {
            nd->debugLog->println("Error occured while sending message.");
            nd->debugLog->println(windowsErrorCode());
        }
        nd->closesocket(sock);
        return;
    }

    ++mSent;
    bSent += b.getLength();

    // Verify the packet was actually sent
    debugAssert(ret == b.getLength());
}


NetAddress ReliableConduit::address() const {
    return addr;
}


bool ReliableConduit::receive(NetMessage* m) {

    static const RealTime timeToWaitForFragmentedPacket = 0.25;

    // This both checks to ensure that a message was waiting and
    // actively consumes the message type if it has not been read
    // yet.
    uint32 t = waitingMessageType();
    if (t == 0) {
        return false;
    }

    if (m != NULL) {
        debugAssert(t == m->type());
    }

    // Reset the waiting message tracker.
    messageType = 0;
    alreadyReadType = false;

    uint8* buffer        = NULL;
    bool freeBuffer      = true;

    // Read the size
    uint32 len;
    int ret = recv(sock, (char*)&len, sizeof(len), 0);

    if ((ret == SOCKET_ERROR) || (ret != sizeof(len))) {
        if (nd->debugLog) {
            nd->debugLog->printf("Call to recv failed.  ret = %d, sizeof(len) = %d\n", ret, sizeof(len));
            nd->debugLog->println(windowsErrorCode());
        }
        nd->closesocket(sock);
        return false;
    }

    len = ntohl(len);
    debugAssert(len >= 0);
    debugAssert(len < 100000);

    // Allocate space for the packet
    if (len < 512) {
        // Allocate on the stack
        buffer = (uint8*)_alloca(len);
        freeBuffer = false;
    } else {
        buffer = (uint8*)malloc(len);
    }

    if (buffer == NULL) {
        if (nd->debugLog) {
            nd->debugLog->println("Could not allocate a memory buffer during receivePacket.");
        }
        nd->closesocket(sock);
        return false;
    }

    // Read the data itself
    ret = 0;
    int left = len;
    int count = 0;
    while ((ret != SOCKET_ERROR) && (left > 0) && (count < 4)) {

        ret = recv(sock, ((char*)buffer) + (len - left), left, 0);

        // Sometimes we get only part of a packet
        if (ret > 0) {
            left -= ret;

            if (left > 0) {
                if (nd->debugLog) nd->debugLog->printf("WARNING: recv() on partial packet of length %d bytes, attempt #%d.  Read %d bytes this time, %d bytes remaining\n", len, count + 1, ret, left);
                ++count;

                // Give the machine a chance to read more data, but don't wait forever
                RealTime t = System::getTick() + timeToWaitForFragmentedPacket;
                while (! messageWaiting() && (System::getTick() < t)) {
                    #ifdef G3D_WIN32
                        Sleep(5);
                    #endif
                }
            }
        } else if (ret == 0) {
            break;
        }
    }

    if ((ret == 0) || (ret == SOCKET_ERROR) || (left != 0)) {
        if (freeBuffer) {
            free(buffer);
            buffer = NULL;
        }

        if (nd->debugLog) {
            if (ret == SOCKET_ERROR) {
                nd->debugLog->printf("Call to recv failed.  ret = %d, sizeof(len) = %d\n", ret, len);
                nd->debugLog->println(windowsErrorCode());
            } else {
                nd->debugLog->printf("Expected %d bytes from recv and got %d.", len, len - left);
            }
        }
        nd->closesocket(sock);
        return false;
    }

    ++mReceived;
    bReceived += len + 4;

    // Deserialize
    if (m != NULL) {
        BinaryInput b(buffer, ret, G3D_LITTLE_ENDIAN, BinaryInput::NO_COPY);
        m->deserialize(b);
    }

    if (freeBuffer) {
        free(buffer);
        buffer = NULL;
    }
    
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

LightweightConduit::LightweightConduit(NetworkDevice* _nd, uint16 port, bool enableReceive, bool enableBroadcast) : Conduit(_nd) {

    if (nd->debugLog) {nd->debugLog->print("Creating a UDP socket        ");}
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (sock == SOCKET_ERROR) {
        sock = 0;
        if (nd->debugLog) {
            nd->debugLog->println("FAIL");
            nd->debugLog->println(windowsErrorCode());
        }
        return;
    }
    if (nd->debugLog) {nd->debugLog->println("Ok");}

    if (enableReceive) {
        debugAssert(port != 0);
        nd->bind(sock, NetAddress(0, port));
    }

    if (enableBroadcast) {
        int TR = true;
        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&TR, sizeof(TR)) != 0) {
            if (nd->debugLog) {
                nd->debugLog->println("Call to setsockopt failed");
                nd->debugLog->println(windowsErrorCode());
            }
            nd->closesocket(sock);
            return;
        }
    }

    if (nd->debugLog) {nd->debugLog->printf("Done creating UDP socket %d\n", sock);}

    alreadyReadMessage = false;
}


LightweightConduit::~LightweightConduit() {
}


void LightweightConduit::send(const NetAddress& a, const NetMessage* m) {

    BinaryOutput b("<memory>", G3D_LITTLE_ENDIAN);

    if (m != NULL) {
        debugAssert(m->type() != 0);
        b.writeUInt32(m->type());
        m->serialize(b);
    } else {
        b.writeUInt32(1);
    }

    if (sendto(sock, (const char*)b.getCArray(), b.getLength(), 0, (struct sockaddr *) &(a.addr), sizeof(a.addr)) == SOCKET_ERROR) {
        if (nd->debugLog) {
            nd->debugLog->printf("Error occured while sending packet to %s\n", inet_ntoa(a.addr.sin_addr));
            nd->debugLog->println(windowsErrorCode());
        }
        nd->closesocket(sock);
    } else {
        ++mSent;
        bSent += b.getLength();
    }
}


bool LightweightConduit::messageWaiting() const {
    // We may have already pulled the message off the network stream
    return alreadyReadMessage || Conduit::messageWaiting();
}


uint32 LightweightConduit::waitingMessageType() {
    if (! messageWaiting()) {
        return 0;
    } 

    if (! alreadyReadMessage) {
        messageBuffer.resize(8192);

        SOCKADDR_IN remote_addr;
        int iRemoteAddrLen = sizeof(sockaddr);

        int ret = recvfrom(sock, (char*)messageBuffer.getCArray(), messageBuffer.size(), 0, (struct sockaddr *) &remote_addr, (socklen_t*)&iRemoteAddrLen);

        if (ret == SOCKET_ERROR) {
            if (nd->debugLog) {
                nd->debugLog->println("Error: recvfrom failed in LightweightConduit::waitingMessageType().");
                nd->debugLog->println(windowsErrorCode());
            }
            nd->closesocket(sock);
            messageBuffer.resize(0);
            messageSender = NetAddress();
            messageType = 0;
            return 0;
        }

        messageSender = NetAddress(remote_addr);

        ++mReceived;
        bReceived += ret;

        messageBuffer.resize(ret, DONT_SHRINK_UNDERLYING_ARRAY);

        // The type is the first four bytes.  It is little endian.
        if (System::machineEndian() == G3D_LITTLE_ENDIAN) {
            messageType = *((uint32*)messageBuffer.getCArray());
        } else {
            // Swap the byte order
            for (int i = 0; i < 4; ++i) {
                ((char*)&messageType)[i] = messageBuffer[3 - i];
            }
        }

        alreadyReadMessage = true;
    }

    return messageType;
}


bool LightweightConduit::receive(NetMessage* m, NetAddress& sender) {

    // This both checks to ensure that a message was waiting and
    // actively consumes the message from the network stream if
    // it has not been read yet.
    uint32 t = waitingMessageType();
    if (t == 0) {
        return false;
    }

    if (m != NULL) {
        debugAssert(m->type() == t);
    }

    sender = messageSender;
    alreadyReadMessage = false;

    if (messageBuffer.size() < 4) {
        // Something went wrong
        return false;
    }

    if (m != NULL) {
        BinaryInput b((messageBuffer.getCArray() + 4), messageBuffer.size() - 4, G3D_LITTLE_ENDIAN, BinaryInput::NO_COPY);
        m->deserialize(b);
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////////////

NetListener::NetListener(NetworkDevice* _nd, uint16 port) {
    nd = _nd;

    // Start the listener socket
    if (nd->debugLog) {nd->debugLog->print("Creating a listener        ");}
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    
    if (sock == SOCKET_ERROR) {
        if (nd->debugLog) {
            nd->debugLog->printf("FAIL");
            nd->debugLog->println(windowsErrorCode());
        }
        nd->closesocket(sock);
        return;
    }
    if (nd->debugLog) {nd->debugLog->println("Ok");}
    
    nd->bind(sock, NetAddress(0, port));

    if (nd->debugLog) {nd->debugLog->printf("Listening on port %5d   ", port);}
    if (listen(sock, 100) == SOCKET_ERROR) {
        if (nd->debugLog) {
            nd->debugLog->println("FAIL");
            nd->debugLog->println(windowsErrorCode());
        }
        nd->closesocket(sock);
        return;
    }
    if (nd->debugLog) {
        nd->debugLog->println("Ok");
        nd->debugLog->printf("Listening on socket %d\n\n", sock);
    }
}


NetListener::~NetListener() {
    nd->closesocket(sock);
}


ReliableConduitRef NetListener::waitForConnection() {

    // The address of the connecting host
    SOCKADDR_IN    remote_addr;
    int iAddrLen = sizeof(remote_addr);

    if (nd->debugLog) {nd->debugLog->println("Blocking in NetListener::waitForConnection().");}
    SOCKET sClient = accept(sock, (struct sockaddr*) &remote_addr, (socklen_t*)&iAddrLen);

    if (sClient == SOCKET_ERROR) {
        if (nd->debugLog) {
            nd->debugLog->println("Error in NetListener::acceptConnection.");
            nd->debugLog->println(windowsErrorCode());
        }
        nd->closesocket(sock);
        return NULL;
    }

    if (nd->debugLog) {nd->debugLog->printf("%s connected, transferred to socket %d.\n", inet_ntoa(remote_addr.sin_addr), sClient);}

#ifndef G3D_WIN32
    return new ReliableConduit(nd, sClient, NetAddress(htonl(remote_addr.sin_addr.s_addr), ntohs(remote_addr.sin_port)));
#else
    return new ReliableConduit(nd, sClient, NetAddress(ntohl(remote_addr.sin_addr.S_un.S_addr), ntohs(remote_addr.sin_port)));
#endif
}


bool NetListener::ok() const {
    return sock != 0;
}


bool NetListener::clientWaiting() const {
    return readWaiting(nd->debugLog, sock);
}


} // namespace

