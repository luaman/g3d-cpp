/**
  @file Network_Demo/Server.cpp

  @author Morgan McGuire, matrix@graphics3d.com
  @created 2004-03-28
  @edited  2004-05-03
 */

#include "Server.h"
#include "App.h"
#include "messages.h"

Server::Server(App* app) : app(app) {
    app->debugLog->printf("Entering Server::Server\n");
    advertisement.name = app->networkDevice->localHostName();
    discoveryServer.init(app->networkDevice, &app->discoverySettings, &advertisement);

    app->debugLog->printf("SERVER --------------------------\n");
    listener = app->networkDevice->createListener(GAME_PORT);
    app->debugLog->printf("---------------------------------\n");
    app->debugLog->printf("Leaving Server::Server\n\n");
}


ID Server::newID() {
    static ID lastID = 0;
    return ++lastID;
}


void Server::doNetwork() {
    discoveryServer.doNetwork();

    if (listener->clientWaiting()) {
        acceptIncomingClient();
    }

    // Check for messages from clients
    for (int c = 0; c < clientProxyArray.size(); ++c) {
        ClientProxy& clientProxy = clientProxyArray[c];
        ReliableConduitRef& net =  clientConduitArray[c];

        // TODO: check if client is ok, tell others if not

        switch (net->waitingMessageType()) {
        case NO_MSG:
            break;

        case EntityStateMessage_MSG:
            {
                EntityStateMessage msg;
                net->receive(&msg);
                if (msg.id == clientProxy.id) {
                    Entity& entity = entityTable[msg.id];

                    // Update the controls of this entity
                    entity.controls = msg.controls;
                    app->debugPrintf("SERVER: receive from %s", entity.name.c_str());
                    
                    // Send to other clients, but don't trust the client's state
                    // beyond the controls.
                    entity.makeStateMessage(msg);
                    ReliableConduit::multisend(clientConduitArray, msg);
                } else {
                    app->debugLog->printf("SERVER: Client sent EntityStateMessage with wrong ID\n\n");
                }
            }
            break;

        default: 
            app->debugLog->printf("SERVER: Ignored unknown message type %d\n",
                net->waitingMessageType());
            net->receive(NULL);
        }
    }
}


static std::string randomName() {
    static const std::string nameArray[11] =
    {"Red Baron", "Ace", "Maverick", "Goose", "Iceman", "Angel",
     "Skywalker", "Darth Vader", "Han Solo", "Kirk", "Picard"};

    return nameArray[iRandom(0, 10)];
}


void Server::acceptIncomingClient() {
    app->debugLog->printf("SERVER --------------------------\n");
    app->debugLog->printf("Incoming client detected\n");

    ClientProxy client;
    ReliableConduitRef net;

    net = listener->waitForConnection();
    if (! net.isNull()) {
        client.id = newID();

        // Create a new object for the client
        Entity entity;
        entity.id                   = client.id;
        entity.color                = Color3::wheelRandom();
        entity.modelFilename        = "ifs/p51-mustang.ifs";
        entity.frame.translation    = Vector3::random() * 10;
        entity.frame.yaw            = random(0, G3D_TWO_PI);
        entity.name                 = randomName();

        entityTable.set(entity.id, entity);

        // Tell the client their ID
        SignOnMessage msg(entity.id);
        net->send(&msg);

        // Tell the client about objects in the world
        // (A drawback of the single-threaded listen server
        // design used in this demo is that we might overflow the
        // network here.  The server should really be running on
        // a separate thread.)

        {
            EntityTable::Iterator end = entityTable.end();
            CreateEntityMessage msg;
            for (EntityTable::Iterator e = entityTable.begin(); e != end; ++e) {
                msg.entity = &e->value;
                net->send(&msg);
            }

            // Tell the other clients about this new entity
            msg.entity = &entity;
            ReliableConduit::multisend(clientConduitArray, msg);
        }

        // Add this client to our list
        addClient(client, net);

    } else {
        app->debugLog->printf("Client failed to connect\n");
    }

    app->debugLog->printf("---------------------------------\n");
}


void Server::doSimulation(SimTime dt) {
    simulateEntities(entityTable, dt);
}


bool Server::ok() const {
    return discoveryServer.ok() && listener->ok();
}


Server::~Server() {
    listener = NULL;
    discoveryServer.cleanup();
}


void Server::addClient(ClientProxy& p, ReliableConduitRef& r) {
    clientProxyArray.append(p);
    clientConduitArray.append(r);
}


void Server::fastRemoveClient(int i) {
    clientProxyArray.fastRemove(i);
    clientConduitArray.fastRemove(i);
}
