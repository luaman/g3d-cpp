/**
  @file Network_Demo/Server.h

  @author Morgan McGuire, matrix@graphics3d.com
  @created 2004-03-28
  @edited  2004-03-28
 */

#ifndef SERVER_H
#define SERVER_H

#include <G3DAll.h>
#include "ServerAd.h"
#include "Entity.h"

/**
 Server-side representation of a client.
 */
class ClientProxy {
public:
    /**
     The ID of the object associated with this client.
     */
    ID                                  id;

    /**
     1/2 the ping time.
     */
    RealTime                            oneWayLatency;

    ClientProxy() {}
    ClientProxy(ID _id) : id(_id), oneWayLatency(0) {}
};


/**
 The server.  This could be used for a dedicated server;
 the program is currently set up to use it only as a listen
 server (client and server on same machine).
 */
class Server {
private:

    class App*              app;

    DiscoveryServer         discoveryServer;

    // Servers send all information over reliable network connections
    // so that the LAN and internet structure of the game is identical.
    // Only matchmaking uses unreliable connections.

    /** Array parallel to clientProxyArray.  Add/remove entries through
        addClient/fastRemoveClient*/
    Array<ReliableConduitRef>   clientConduitArray;

    Array<ClientProxy>      clientProxyArray;

    void addClient(ClientProxy& p, ReliableConduitRef& r);

    void fastRemoveClient(int i);

    /**
     Listens for new clients.  Handled in doNetwork.
     */
    NetListenerRef          listener;
    
    EntityTable             entityTable;

    /** Generates a fresh ID */
    ID newID();

    /** Called from doNetwork when listener has an incoming client */
    void acceptIncomingClient();

public:

    /** The advertisement for this server */
    ServerAd                advertisement;

    Server(class App* app);

    ~Server();

    bool ok() const;

    /** Called from Client::doNetwork */
    void doNetwork();

    /** Called from Client::dosimulation */
    void doSimulation(SimTime dt);
};

#endif
