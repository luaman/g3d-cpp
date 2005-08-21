/**
  @file Mocap/main.cpp

  @author Morgan McGuire, matrix@graphics3d.com
 */

#include <G3DAll.h>
#if G3D_VER < 60700
    #error Requires G3D 6.07
#endif

#include "World.h"
#include "App.h"

#include "../ArticulatedModel/ArticulatedModel.cpp"
#include "../ArticulatedModel/ArticulatedModel_pose.cpp"
#include "../ArticulatedModel/SuperShader.cpp"


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*          app;

    Demo(App* app);

    virtual ~Demo() {}

    virtual void init();

    virtual void doLogic();

	virtual void doNetwork();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();

    virtual void cleanup();

};

Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(2, 0, -2));
    app->debugCamera.lookAt(Vector3(0, -0.75, 0));
}


void Demo::cleanup() {
    // Called when Demo::run() exits
}


void Demo::doNetwork() {
	// Poll net messages here
}


void Demo::doSimulation(SimTime dt) {
	// Add physical simulation here
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

	// Add other key handling here
}


void Demo::doGraphics() {

    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    app->world.render(app->renderDevice);
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    loadScene();
    
    applet->run();
}


void App::loadScene() {
    world.init();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}

App* app = NULL;

int main(int argc, char** argv) {
    
    GAppSettings settings;
    
    settings.useNetwork = false;
    settings.window.stencilBits = 8;
    settings.window.alphaBits = 0;
    
    app = new App(settings);
    app->run();
    delete app;
    app = NULL;

    return 0;
}
