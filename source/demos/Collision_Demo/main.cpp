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
 @edited  2003-11-07
 */
#include <G3DAll.h>
#include "Model.h"
#include "Object.h"
#include "Scene.h"

#if G3D_VER != 060007
    #error Requires G3D 6.00 b7
#endif


/**
 The path to the data directory from this program's directory.
 */
std::string DATA_DIR("data/");

/** The same bit depth is used for the shadow map and the screen */
int                     depthBits       = 24;

Log*                    debugLog        = NULL;
RenderDevice*           renderDevice    = NULL;
CFontRef                font            = NULL;
UserInput*              userInput       = NULL;
VARArea*                varDynamic      = NULL;
VARArea*                varStatic       = NULL;
Camera*                 camera          = NULL;
Scene*                  scene           = NULL;
ManualCameraController* controller      = NULL;

double                  maxFrameRate    = 30.5;

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
    DATA_DIR = demoFindData();

    // Initialize
    debugLog     = new Log();
    renderDevice = new RenderDevice();
    RenderDeviceSettings settings;
    settings.depthBits = depthBits;
    renderDevice->init(settings, debugLog);
    camera       = new Camera(renderDevice);

    // Verify we have the shadow extension
    if (! renderDevice->supportsOpenGLExtension("GL_ARB_shadow")) {
        error("Critical Error", "This demo requires a graphics card with the ARB_shadow extension.", true);
        exit(-1);
    }

    // Allocate the two VARAreas used in this demo
    varStatic  = renderDevice->createVARArea(1024 * 1024);
    debugAssert(varStatic);
    varDynamic = renderDevice->createVARArea(1024 * 1);
    debugAssert(varDynamic);

    font         = CFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    controller   = new ManualCameraController(renderDevice, userInput);
    controller->setMoveRate(4);

    controller->setPosition(Vector3(15, 20, 15));
    controller->lookAt(Vector3(-2,3,-5));
    controller->setActive(true);

    scene        = new Scene();

    buildScene();

    renderDevice->resetState();
    renderDevice->setColorClearValue(Color3(.1, .5, 1));

    gameTime     = G3D::toSeconds(10, 00, 00, AM);
    
    RealTime now = getTime() - 0.001;

    // Main loop
    RealTime lastTime;
    do {
        RealTime timeStep;
        lastTime = now;

        // Lock the frame rate
        RealTime wait;
        do {
            now = getTime();
            timeStep = now - lastTime;

            wait = 1 / (maxFrameRate + 0.5) - timeStep;
            if (wait > 0) {
                System::sleep(wait);
            }
        } while (wait > 0);

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
    delete controller;
    delete userInput;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;
    delete scene;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////


void doSimulation(GameTime timeStep) {
    // Simulation
    controller->doSimulation(max(0.1, min(0, timeStep)));
    camera->setCoordinateFrame(controller->getCoordinateFrame());

    scene->simulate(1 / maxFrameRate);
}


void doGraphics() {
    // Stats that will be updated periodically
    static RealTime lastFrameRateUpdate = getTime() - 0.9;
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
                
            camera->setProjectionAndCameraMatrix();

            LightingParameters lighting(gameTime);
            lighting.lightDirection = (lighting.lightDirection + Vector3(0,0,.4)).direction();
            lighting.ambient = lighting.ambient + Color3(.3,.3,.4);

            scene->render(lighting);
      
                renderDevice->push2D();

                font->draw2D(
                    format("%d fps", frameRate),
                    Vector2(10, 10), 28, Color3::WHITE, Color3::BLACK);

                font->draw2D(
                    format("%d tris", numTris),
                    Vector2(10, 72), 20, Color3::WHITE, Color3::BLACK);

                font->draw2D(
                    format("%d ktri/s", triRate),
                    Vector2(10, 100), 20, Color3::WHITE, Color3::BLACK);

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
            default:;
            }
            break;

            // Add other event handlers here
        default:;
        }

        userInput->processEvent(event);
    }

    userInput->endEvents();
}



