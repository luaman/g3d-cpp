/**
 @file Collision_Demo/main.cpp

  To run this program:
    1. Install OpenGL and SDL (www.libsdl.org)
    2. Compile and run
    3. Press ESC to exit

 <P>

 This demo uses the G3D collision detection routines to implement a simple physics
 system.  Shadow maps are rendered using the OpenGL SGIX_SHADOW extension to
 show how to use non-G3D calls with G3D.  A Matrix4 skeleton is provided as well.
 <P>

 This is not the greatest collision simulation ever written-- see OPCODE for something much nicer.
 The intent of the demo is to show how to use non-G3D OpenGL calls, test the collision
 detection routines (which are really intended for character motion and ray tracing,
 not physics), and show a good way of structuring a 3D application.
  
 <P>
 Requires:
   OpenGL
   <A HREF="http://www.libsdl.org">SDL</A>
   G3D
   GLG3D

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2003-02-07
 @edited  2003-05-24
 */

#include <G3DAll.h>
#include "Model.h"
#include "Object.h"
#include "Scene.h"

/**
 The path to the data directory from this program's directory.
 */
std::string DATA_DIR("data/");

/** The same bit depth is used for the shadow map and the screen */
int                     depthBits       = 24;

Log*                    debugLog        = NULL;
RenderDevice*           renderDevice    = NULL;
CFont*                  font            = NULL;
UserInput*              userInput       = NULL;
VARArea*                varDynamic      = NULL;
VARArea*                varStatic       = NULL;
Camera*                 camera          = NULL;
Scene*                  scene           = NULL;
ManualCameraController* controller      = NULL;

double                  maxFrameRate    = 30;

/** World time */
GameTime                gameTime        = 0;

bool                    endProgram      = false;


RealTime getTime() {
    return SDL_GetTicks() / 1000.0;
}

void buildScene();
void doSimulation(GameTime timeStep);
void doGraphics();
void doUserInput();


int main(int argc, char** argv) {
    // Search for the data
    for (int count = 0; (count < 3) && (! fileExists(DATA_DIR + "ifs/p51-mustang.ifs")); ++count) {
        DATA_DIR = std::string("../") + DATA_DIR;
    }

    // Initialize
    debugLog     = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(800, 600, debugLog, 1.0, false,
                       1024 * 1024 * 2, true, 8, 0, depthBits, 0);
    camera       = new Camera(renderDevice);

    // Allocate the two VARAreas used in this demo
    varStatic  = renderDevice->createVARArea(1024 * 1024);
    debugAssert(varStatic);
    varDynamic = renderDevice->createVARArea(1024 * 1);
    debugAssert(varDynamic);

    font         = new CFont(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    controller   = new ManualCameraController(renderDevice);
    controller->setMoveRate(2);

    controller->setPosition(Vector3(15, 20, 15));
    controller->lookAt(Vector3(-2,3,-5));

    scene        = new Scene();

    buildScene();

    renderDevice->resetState();
    renderDevice->setColorClearValue(Color3(.1, .5, 1));

    gameTime     = toSeconds(10, 00, 00, AM);
    
    RealTime now = getTime() - 0.001, lastTime;

    // Main loop
    do {
        RealTime timeStep;
        lastTime = now;

        // Lock the frame rate
        do {
            now = getTime();
            timeStep = now - lastTime;
        } while (timeStep <= 1 / (maxFrameRate + 0.5));

        doUserInput();

        doSimulation(timeStep);

        doGraphics();
   
    } while (! endProgram);

    varStatic->reset();


    debugLog->printf("Static VAR peak size was  %d bytes.\n",
                     varStatic->peakAllocatedSize());
    debugLog->printf("Dynamic VAR peak size was %d bytes.\n", 
                     varDynamic->peakAllocatedSize());

    // Cleanup
    Model::freeModels();
    delete font;
    delete userInput;
    delete controller;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;
    delete scene;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////


void doSimulation(GameTime timeStep) {
    // Simulation
    controller->doSimulation(max(0.1, min(0, timeStep)), *userInput);
    camera->setCoordinateFrame(controller->getCoordinateFrame());

    scene->simulate(1 / maxFrameRate);
}


void doGraphics() {
    // Stats that will be updated periodically
    static RealTime lastFrameRateUpdate = getTime() - 0.9;
    static RealTime lastTime;
    static int frameRate = 0;
    static int numTris = 0;
    static int triRate = 0;
    RealTime now = getTime();

    // Compute the frame rate
    if (lastFrameRateUpdate + 1 < now) {
        frameRate = iRound(renderDevice->getFrameRate());
        numTris = iRound(renderDevice->getTrianglesPerFrame());
        triRate = iRound(renderDevice->getTriangleRate() / 1000);
        lastFrameRateUpdate = now;
    }

    renderDevice->beginFrame();
        renderDevice->pushState();
                
            camera->setProjectionAndCameraMatrix(renderDevice->getWidth(), 
                                                 renderDevice->getHeight());

            LightingParameters lighting(gameTime);
            lighting.lightDirection = (lighting.lightDirection + Vector3(0,0,.4)).direction();
            lighting.ambient = lighting.ambient + Color3(.3,.3,.4);

            scene->render(lighting);
      
                renderDevice->push2D();

                font->draw2DString(
                    format("%d fps", frameRate),
                    10, 10, 28, Color3::WHITE, Color3::BLACK);

                font->draw2DString(
                    format("%d tris", numTris),
                    10, 72, 20, Color3::WHITE, Color3::BLACK);

                font->draw2DString(
                    format("%d ktri/s", triRate),
                    10, 100, 20, Color3::WHITE, Color3::BLACK);

            renderDevice->pop2D();
        renderDevice->popState();
        
    renderDevice->endFrame();
    varDynamic->reset();
}


//////////////////////////////////////////////////////////////////////////////

void doUserInput() {

    userInput->beginEvents();

    // Event handling
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
        endProgram = true;
        break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                endProgram = true;
                break;

            // Add other key handlers here

            }
            break;

            // Add other event handlers here
        }

        userInput->processEvent(event);
    }

    userInput->endEvents();
}



