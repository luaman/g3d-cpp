/**
  @file Network_Demo/main.cpp

  Optional command line argument is the name of the log file (useful when running
  two clients on the same machine).

  This demo shows how to set up network discovery (server browsing) and a client
  server program.  It also contains a "listen server" that runs in parallel with
  a client within a single thread.

  In-game keys:
    ESC     - Quit
    W/S     - Forward/backward
    A/D     - Left/right
    Z/space - Down/up

  @author Morgan McGuire, matrix@graphics3d.com
  @created 2004-03-28
  @edited  2004-05-03
 */

#include "App.h"
#include "Browse.h"
#include "Client.h"
#include "Server.h"

App* app;

int main(int argc, char** argv) {
    GAppSettings settings;

    if (argc > 1) {
        settings.logFilename = argv[1];
    }

    settings.window.fsaaSamples = 4;
    settings.window.width       = 640;
    settings.window.height      = 480;

    App(settings).run();
    return 0;
}


App::App(const GAppSettings& settings) : 
    GApp(settings),
    discoverySettings("Demo Discovery Protocol", 1),
    hostingServer(NULL) {

    ::app = this;

    renderDevice->setCaption("G3D Network Demo");
}

    
void App::main() {
	setDebugMode(true);
	debugController.setActive(false);
	debugShowRenderingStats = false;
    debugQuitOnEscape = false;

    // Load objects here
    sky  = Sky::create(renderDevice, dataDir + "sky/");
    font = GFont::fromFile(renderDevice, dataDir + "font/dominant.fnt");

    Browse browse(this);
    Client client(this);

    while (! endProgram) {
        // First browse for a server (or start our own).
        browse.run();

        if (endProgram) {
            return;
        }

        if (browse.host) {
            // Start a server
            hostingServer = new Server(this);

            // Connect to that server
            client.selectedServer = hostingServer->advertisement;

            Array<NetAddress> myAddresses;
            networkDevice->localHostAddresses(myAddresses);
            client.selectedServer.address = NetAddress(myAddresses[0].ip(), GAME_PORT);
        } else {
            // Overwrite the port
            client.selectedServer = browse.selectedServer;
            client.selectedServer.address = NetAddress(client.selectedServer.address.ip(), GAME_PORT);
        }

        // Now enter the world as a client (our own server runs in the background)
        client.run();

        debugController.setActive(false);

        // If we were hosting, delete the server
        if (hostingServer) {
            delete hostingServer;
            hostingServer = NULL;
        }
    }
}
