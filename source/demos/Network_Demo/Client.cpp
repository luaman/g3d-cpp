/**
  @file Network_Demo/Client.cpp

  @author Morgan McGuire, matrix@graphics3d.com
  @created 2004-03-28
  @edited  2004-03-31
 */

#include "Client.h"
#include "Server.h"
#include "App.h"
#include "messages.h"

// Windows.h defines DEFAULT_PITCH, which we use as a variable name
#ifdef DEFAULT_PITCH
#undef DEFAULT_PITCH
#endif

Client::Client(App* _app) : GApplet(_app), app(_app) {
    serverProxy = ServerProxy(_app, this);
}


void Client::init()  {
    // Called before Client::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));

    localID = NO_ID;

    // Connect
    serverProxy.connect(selectedServer.address);

    camera.setPosition(Vector3::ZERO);
    camera.lookAt(-Vector3::UNIT_Z);
}


void Client::cleanup() {
    // Called when Client::run() exits
    entityTable.clear();
}


void Client::doNetwork() {
	// Poll net messages here
    if (app->hostingServer) {
        app->hostingServer->doNetwork();
    }

    switch (serverProxy.net->waitingMessageType()) {
    case NO_MSG:
        // No message waiting
        break;

    case SignOnMessage_MSG:
        {
            SignOnMessage msg;
            serverProxy.net->receive(&msg);
            localID = msg.id;
        }
        break;

    case CreateEntityMessage_MSG:
        {
            Entity entity;
            CreateEntityMessage msg(&entity);
            serverProxy.net->receive(&msg);
            entityTable.set(entity.id, entity);

            if (localID == entity.id) {
                // This is the client's own entity.  Set the
                // window caption.
                app->renderDevice->setCaption(entity.name + " - G3D Network Demo");
            }
            app->debugLog->printf("CLIENT: Created entity \"%s\"\n\n", 
                entity.name.c_str());
        }
        break;

    case EntityStateMessage_MSG:
        {
            EntityStateMessage msg;
            serverProxy.net->receive(&msg);
            if (entityTable.containsKey(msg.id)) {
                entityTable[msg.id].clientUpdateFromStateMessage(msg, localID);
            }
        }
        break;

    default:
        app->debugLog->printf("CLIENT: Ignored unknown message type %d\n",
            serverProxy.net->waitingMessageType());
        serverProxy.net->receive(NULL);
    }
}


void Client::simulateCamera(SimTime dt) {
    if (entityTable.containsKey(localID)) {
        const Entity& me = entityTable[localID];

        double FOLLOW_DISTANCE = 20;
        double FOLLOW_HEIGHT = 4;

        const CoordinateFrame cframe = me.smoothCoordinateFrame();

        // Move the camera to follow the plane
        camera.setPosition(cframe.translation 
                + cframe.rotation.getColumn(2) * FOLLOW_DISTANCE
                + Vector3::UNIT_Y * FOLLOW_HEIGHT);
        camera.lookAt(cframe.translation + Vector3::UNIT_Y * FOLLOW_HEIGHT / 2);
    }
}


void Client::doSimulation(SimTime dt) {
    if (app->hostingServer) {
        app->hostingServer->doSimulation(dt);
    }

    simulateEntities(entityTable, dt);

    simulateCamera(dt);
}


/**
 Allow other applications some time.
 */
static void manageFrameRate() {
    const double targetFrameRate = 60;

    // Sleep if we have more performance than needed
    static RealTime last = 0;
    RealTime now = System::getTick();
    double actualFrameTime = now - last;
    RealTime desiredFrameTime = 1.0 / targetFrameRate;
    RealTime delta = desiredFrameTime - actualFrameTime;
    if (delta > 0.0) {
        System::sleep(min(delta, 0.1));
    }
    last = now;
}


void Client::doLogic() {

//    if (! app->hostingServer) {
        manageFrameRate();
//    }

    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Quit back to main menu
        endApplet = true;
    }

    if (entityTable.containsKey(localID) && ! app->debugController.active()) {
        Entity& entity = entityTable[localID];
        Controls newControls = entity.controls;

        const int THROTTLE_FORWARD_KEY1  = 'w';
        const int THROTTLE_FORWARD_KEY2  = SDLK_UP;
        const int THROTTLE_BACKWARD_KEY1 = 's';
        const int THROTTLE_BACKWARD_KEY2 = SDLK_DOWN;

        const int YAW_LEFT_KEY1          = 'a';
        const int YAW_LEFT_KEY2          = SDLK_LEFT;
        const int YAW_RIGHT_KEY1         = 'd';
        const int YAW_RIGHT_KEY2         = SDLK_RIGHT;

        const int PITCH_UP_KEY1          = ' ';
        const int PITCH_UP_KEY2          = SDLK_BACKSPACE;
        const int PITCH_DOWN_KEY1        = 'z';
        const int PITCH_DOWN_KEY2        = SDLK_LCTRL;

        const double FORWARD_THROTTLE    =  1.0;
        const double DEFAULT_THROTTLE    =  0.0;
        const double BACKWARD_THROTTLE   = -0.5;

        const double LEFT_YAW            =  1.0;
        const double DEFAULT_YAW         =  0.0;
        const double RIGHT_YAW           = -1.0;

        const double UP_PITCH            =  1.0;
        const double DEFAULT_PITCH       =  0.0;
        const double DOWN_PITCH          = -1.0;

        // See which way the throttle is tipped
        if (app->userInput->keyDown(THROTTLE_FORWARD_KEY1) ||
            app->userInput->keyDown(THROTTLE_FORWARD_KEY2)) {

            newControls.throttle = FORWARD_THROTTLE;

        } else if (app->userInput->keyDown(THROTTLE_BACKWARD_KEY1) ||
                   app->userInput->keyDown(THROTTLE_BACKWARD_KEY2)) {

            newControls.throttle = BACKWARD_THROTTLE;

        } else {

            newControls.throttle = DEFAULT_THROTTLE;
        }

        
        // See which way the yaw control points
        if (app->userInput->keyDown(YAW_LEFT_KEY1) ||
            app->userInput->keyDown(YAW_LEFT_KEY2)) {

            newControls.yaw = LEFT_YAW;

        } else if (app->userInput->keyDown(YAW_RIGHT_KEY1) ||
                   app->userInput->keyDown(YAW_RIGHT_KEY2)) {

            newControls.yaw = RIGHT_YAW;

        } else {

            newControls.yaw = DEFAULT_YAW;
        }


        // See which way the pitch control points
        if (app->userInput->keyDown(PITCH_UP_KEY1) ||
            app->userInput->keyDown(PITCH_UP_KEY2)) {

            newControls.pitch = UP_PITCH;

        } else if (app->userInput->keyDown(PITCH_DOWN_KEY1) ||
                   app->userInput->keyDown(PITCH_DOWN_KEY2)) {

            newControls.pitch = DOWN_PITCH;

        } else {

            newControls.pitch = DEFAULT_PITCH;
        }


        if (entity.controls != newControls) {
            // Tell the server our new controls
            app->debugPrintf("CLIENT: send");
            entity.controls = newControls;
            EntityStateMessage msg;
            entity.makeStateMessage(msg);
            serverProxy.net->send(&msg);
        }
    }

	// Add other key handling here
}


void Client::renderEntity(const Entity& entity) {
    const std::string modelFilename = entity.modelFilename;

    if (! app->modelManager.containsKey(modelFilename)) {
        // Load a model the first time it is used.
        std::string filename = app->dataDir + modelFilename;
        app->debugLog->printf("Loading \"%s\"....", filename.c_str());
        app->modelManager.set(modelFilename,
            IFSModel::create(filename, 10));
        app->debugLog->printf("Done.\n\n");
    }

    // Get the model
    IFSModelRef model = app->modelManager[modelFilename];

    // Lerp between old and new values
    CoordinateFrame cframe = entity.smoothCoordinateFrame();

    // Pose it
    GMaterial material;
    material.color = entity.color;
    material.specularCoefficient = 1.0;
    PosedModelRef posedModel = model->pose(cframe, material);

    // Render the model
    posedModel->render(app->renderDevice);
}


void Client::renderEntities() {
    EntityTable::Iterator end = entityTable.end();
    for (EntityTable::Iterator e = entityTable.begin(); e != end; ++e) {
        renderEntity(e->value);
    }
}


void Client::doGraphics() {

    const GCamera& cam = app->debugController.active() ? app->debugCamera : camera;

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(cam);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (! app->sky.isNull()) {
        app->sky->render(lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
        // Splash light
        app->renderDevice->setLight(1, GLight::directional(-Vector3::UNIT_Y, Color3::WHITE * 0.20, false));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);

        renderEntities();

        for (int x = 0; x < 10; ++x) {
            for (int z = 0; z < 10; ++z) {
                Vector3 v((x - 5) * 10, -10, (z - 5) * 10); 
                Draw::box(AABox(v, v + Vector3(1,.1,1)), app->renderDevice, Color3::RED, Color3::BLACK);
            }
        }

    app->renderDevice->disableLighting();

    if (! app->sky.isNull()) {
        app->sky->renderLensFlare(lighting);
    }

    app->renderDevice->push2D();

        // Entity names
        {
            EntityTable::Iterator end = entityTable.end();
            for (EntityTable::Iterator e = entityTable.begin(); e != end; ++e) {
                const Entity& entity = e->value;
                Vector3 pos = cam.project(entity.smoothCoordinateFrame().translation,
                    app->renderDevice->getViewport());

                if (pos.isFinite()) {
                    app->font->draw2D(entity.name, pos.xy(), 16, Color3::WHITE, Color3::BLACK,
                        GFont::XALIGN_CENTER);
                }
            }
        }

        // Server crash messages
        if (! serverProxy.ok()) {
            app->font->draw2D("Lost connection to server.", Vector2(5, 5), 14,
                Color3(1.0, 0.4, 0.4), Color3::BLACK);
        }

        if (app->hostingServer && ! app->hostingServer->ok()) {
            app->font->draw2D("(Local Server Crashed)", Vector2(5, 25), 14,
                Color3(1.0, 0.4, 0.4), Color3::BLACK);
        }
    app->renderDevice->pop2D();
}


///////////////////////////////////////////////////////////////////////////

ServerProxy::ServerProxy(App* app, Client* client) : app(app), client(client) {
}


void ServerProxy::connect(const NetAddress& address) {
    app->debugLog->printf("\nCLIENT: --------------------------\n");
    app->debugLog->printf("\nConnecting to \"%s\"\n\n", address.toString().c_str());

    net = app->networkDevice->createReliableConduit(address);

    if ((net == (ReliableConduitRef)NULL) || ! net->ok()) {
        app->debugLog->printf("\nConnect failed\n\n");
    } else {
        app->debugLog->printf("\nConnected to server\n\n");
    }

    app->debugLog->printf("--------------------------------\n");
}


bool ServerProxy::ok() const {
    return (net != NULL) && (net->ok());
}
