/**
 @file Collision_Demo/main.cpp

 <P>
 This demo uses the G3D collision detection routines to implement a simple physics
 system.  
 <P>

 This is not the greatest collision simulation ever written-- see OPCODE for
 better collision detection and Guendelman et. al's SIGGRAPH 2003 paper for better
 simulation.
 
 G3D's collision detection routines are intended for convenient bounding volume
 tests and ray casting, not high-performance collision detection, so this is 
 really a stress test.
  
 <P>
 Requires:
   OpenGL
   <A HREF="http://www.libsdl.org">SDL</A>
   G3D
   GLG3D

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2003-02-07
 @edited  2004-06-20
 */
#include <G3DAll.h>
#include "Model.h"
#include "Object.h"
#include "Scene.h"
#include "Demo.h"

GApp*                app            = NULL;
VARAreaRef           varStatic      = NULL;

class App : public GApp {
protected:
    void main();
public:
    App(const GAppSettings& settings);
};


const double                  maxFrameRate    = 30.5;


Demo::Demo(App* _app) : GApplet(_app), app(_app), gameTime(0) {
    // Allocate the two VARAreas used in this demo
    ::varStatic    = VARArea::create(1024 * 1024);
    debugAssert(::varStatic.notNull());
}


void Demo::init()  {
    app->debugCamera.setPosition(Vector3(15, 20, 15));
    app->debugCamera.lookAt(Vector3(-2,3,-5));
    app->debugController.setActive(false);

    buildScene();

    gameTime     = G3D::toSeconds(10, 00, 00, AM); 

    simStartTime = System::time();
}


Demo::~Demo() {
    app->debugLog->printf("Static VAR peak size was  %d bytes.\n",
                     varStatic->peakAllocatedSize());
    ::varStatic = NULL;
}

void Demo::doSimulation(SimTime timeStep) {
    if (simStartTime + 10 < System::time()) {
        scene.clear();
        buildScene();
        simStartTime = System::time();
    }

    scene.simulate(timeStep);
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }
}


void Demo::doGraphics() {
    app->renderDevice->pushState();
            
        app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

        LightingParameters lighting(gameTime);
        lighting.lightDirection = (lighting.lightDirection + Vector3(0,0,.4)).direction();
        lighting.ambient = lighting.ambient + Color3(.3,.3,.4);

        debugAssertGLOk();
        scene.render(lighting);
  
    app->renderDevice->popState();        
}


void App::main() {
    setDebugMode(true);
    Demo(this).run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
}


int main(int argc, char** argv) {

    app = new App(GAppSettings());
    
    app->run();

    Model::freeModels();

    delete app;

    return 0;
}
