/**
 @file Collision_Demo/main.cpp

 <P>

 This demo uses the G3D collision detection routines to implement a simple physics
 system.  
 <P>

 This is not the greatest collision simulation ever written-- see OPCODE for
 something much more sophisticated.  G3D's collision detection routines are
 intended for bounding volume tests and ray casting, not high-performance
 collision detection.
  
 <P>
 Requires:
   OpenGL
   <A HREF="http://www.libsdl.org">SDL</A>
   G3D
   GLG3D

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2003-02-07
 @edited  2003-12-07
 */
#include <G3DAll.h>
#include "Model.h"
#include "Object.h"
#include "Scene.h"

/**
 The path to the data directory from this program's directory.
 */
std::string DATA_DIR("data/");

Log*                    debugLog        = NULL;
RenderDevice*           renderDevice    = NULL;
CFontRef                font            = NULL;
UserInput*              userInput       = NULL;
VARAreaRef              varStatic       = NULL;
GCamera*                camera          = NULL;
Scene*                  scene           = NULL;
ManualCameraController* controller      = NULL;

double                  maxFrameRate    = 30.5;

/** World time */
GameTime                gameTime        = 0;

bool                    endProgram      = false;


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
    renderDevice->init(settings, debugLog);
    camera       = new GCamera();

    // Verify we have the shadow extension
    if (! renderDevice->supportsOpenGLExtension("GL_ARB_shadow")) {
        error("Critical Error", "This demo requires a graphics card with the ARB_shadow extension.", true);
        exit(-1);
    }

    // Allocate the two VARAreas used in this demo
    varStatic  = VARArea::create(1024 * 1024);
    debugAssert(varStatic);

    font         = GFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");

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
    
    RealTime now = System::getTick() - 0.001;

    // Main loop
    RealTime lastTime;
    do {
        RealTime timeStep;
        lastTime = now;

        // Lock the frame rate
        RealTime wait;
        do {
            now = System::getTick();
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
    static RealTime lastFrameRateUpdate = System::getTick() - 0.9;
    static int frameRate = 0;
    static int numTris = 0;
    static int triRate = 0;
    RealTime now = System::getTick();

    // Compute the frame rate
    if (lastFrameRateUpdate + 1 < now) {
        frameRate = iRound(renderDevice->getFrameRate());
        numTris = iRound(renderDevice->getTrianglesPerFrame());
        triRate = iRound(renderDevice->getTriangleRate() / 1000);
        lastFrameRateUpdate = now;
    }

    renderDevice->beginFrame();
        renderDevice->pushState();
                
            renderDevice->setProjectionAndCameraMatrix(*camera);

            LightingParameters lighting(gameTime);
            lighting.lightDirection = (lighting.lightDirection + Vector3(0,0,.4)).direction();
            lighting.ambient = lighting.ambient + Color3(.3,.3,.4);

            debugAssertGLOk();
            scene->render(lighting);
      
            renderDevice->push2D();

                font->draw2D(
                    format("%d fps", frameRate),
                    Vector2(10, 10), 28, Color3::white(), Color3::black());

                font->draw2D(
                    format("%d tris", numTris),
                    Vector2(10, 72), 20, Color3::white(), Color3::black());

                font->draw2D(
                    format("%d ktri/s", triRate),
                    Vector2(10, 100), 20, Color3::white(), Color3::black());

            renderDevice->pop2D();
        renderDevice->popState();
        
    renderDevice->endFrame();
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



