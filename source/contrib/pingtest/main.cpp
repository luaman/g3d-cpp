/**
 @file pingtest/main.cpp
 
 A simple demo of LightweightConduit using a 1-way ping scheme.
 This does not use the protocol of the 'ping' program, but the
 idea is the same (there is no return message from the server,
 however).

 Two protocols are supported.  When reliable is true, the client
 makes a ReliableConduit then drops it.  When reliable is false,
 the client sends a PingMessage across a LightweightConduit.
 
 @author Morgan McGuire, matrix@graphics3d.com
 */
#include <G3DAll.h>

/** Change this constant to build a test for ReliableConduit */
static const bool reliable = true;

enum {PING_PORT=1201};
NetworkDevice  networkDevice;
void reliableServer();
void server();
void client(const std::string& address);
void reliableClient(const std::string& address);
void printHelp();

int main(int argc, char** argv) {
    Log debugLog("log.txt");
    networkDevice.init(&debugLog);
    if ((argc == 2) && (std::string("--server") == argv[1])) {
        if (reliable) {
            reliableServer();
        } else {
            server();
        }
    } else if ((argc == 2) && ! beginsWith(argv[1], "--")) {
        if (reliable) {
            reliableClient(argv[1]);
        } else {
            client(argv[1]);
        }
    } else {
        printHelp();
    }
    networkDevice.cleanup();
    return 0;
}

// Arbitrary constant greater than 1000 to identify our messages
enum {PingMessage_MSG = 1008};
class PingMessage : public NetMessage {
public:
    virtual uint32 type() const {
        return PingMessage_MSG;
    }

    virtual void serialize(BinaryOutput& b) const {
        // No data payload
    }
 
    virtual void deserialize(BinaryInput& b) {
        // No data payload
    }
};

void server() {
    // Print our network address
    printf("Running lightweight G3D pingtest server on %s\n", 
           networkDevice.localHostName().c_str());

    printf(" Press any key to quit.\n\n");
 
    LightweightConduitRef conduit =
        networkDevice.createLightweightConduit(PING_PORT, true);

    if (conduit.isNull()) {
        printf("Could not create conduit.  Shutting down server.\n");
        return;
    }

    while (! System::consoleKeyPressed()) {
        NetAddress clientAddress;
        switch (conduit->waitingMessageType()) {
        case 0:
            // No message
            System::sleep(0.05);
            break;

        case PingMessage_MSG:
            {
                PingMessage msg;
                conduit->receive(&msg, clientAddress);
                printf("  Received ping from %s\n",
                       clientAddress.toString().c_str());
            }
            break;

        default:
            // Ignore the message
            conduit->receive(NULL, clientAddress);
            printf("  Unknown message type received: %d\n",
                   conduit->waitingMessageType());
        }
    }
}

 #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/tcp.h>

void reliableServer() {
    // Print our network address
    printf("Running reliable G3D pingtest server on %s\n",
           networkDevice.localHostName().c_str());

    printf(" Press any key to quit.\n\n");
 
    NetListenerRef listener =
        networkDevice.createListener(PING_PORT);

    while (! System::consoleKeyPressed()) {
        NetAddress clientAddress;

        if (listener.isNull() || ! listener->ok()) {
            printf("Listener crashed.  Shutting down server.\n");
            return;
        }

        if (listener->clientWaiting()) {
            ReliableConduitRef conduit =
                listener->waitForConnection();

            printf("  Received connection from %s\n",
                   conduit->address().toString().c_str());

            // Let the conduit go out of scope, so it 
            // is automatically freed.
        } else {
            System::sleep(0.05);
        }
    }
}

void client(const std::string& server) {
    LightweightConduitRef conduit =
        networkDevice.createLightweightConduit();

    NetAddress serverAddress(server, PING_PORT);
    PingMessage msg;

    printf("  Sending ping to %s\n", serverAddress.toString().c_str());
    conduit->send(serverAddress, &msg);
}


void reliableClient(const std::string& server) {

    NetAddress serverAddress(server, PING_PORT);

    printf("  Trying to connect to %s\n", serverAddress.toString().c_str());

    ReliableConduitRef conduit =
        networkDevice.createReliableConduit(serverAddress);
    
    if (conduit.isNull() || ! conduit->ok()) {
        printf("  Unable to connect.\n");
    } else {
        printf("  Connected successfully.  Disconnecting.\n");
    }
}


void printHelp() {
    printf("pingtest {machine|--server}\n");
    printf("\n");
    printf("  machine   Host name of the pingtest server.\n");
    printf("  --server  Run a server on this machine.\n");
    printf("\n");
}
