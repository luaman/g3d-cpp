/**
  @file demos/main.cpp

  This is a prototype main.cpp to use for your programs.  It is a good
  infrastructure for building an interactive demo.
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2003-09-27
 */ 

#include <G3DAll.h>

std::string             DATA_DIR;

Log*                    debugLog		= NULL;
RenderDevice*           renderDevice	= NULL;
CFontRef                font			= NULL;
UserInput*              userInput		= NULL;
Camera*					camera			= NULL;
ManualCameraController* controller      = NULL;
bool                    endProgram		= false;


RealTime getTime() {
    return SDL_GetTicks() / 1000.0;
}

void doSimulation(GameTime timeStep);
void doGraphics();
void doUserInput();


int main(int argc, char** argv) {

    // Initialize
    DATA_DIR     = demoFindData();
    debugLog	 = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(800, 600, debugLog, 1.0, false, 0, true, 8, 0, 24, 0);
    camera 	     = new Camera(renderDevice);

    font         = CFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    controller   = new ManualCameraController(renderDevice, userInput);
    controller->setMoveRate(10);

    controller->setPosition(Vector3(15, 20, 15));
    controller->lookAt(Vector3(-2,3,-5));

    renderDevice->resetState();
	renderDevice->setColorClearValue(Color3(.1, .5, 1));

    controller->setActive(true);

    RealTime now = getTime() - 0.001, lastTime;

    // Main loop
    do {
        lastTime = now;
        now = getTime();
        RealTime timeStep = now - lastTime;

        doUserInput();

        doSimulation(timeStep);

        doGraphics();
   
    } while (! endProgram);


    // Cleanup
    delete controller;
    delete userInput;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////


void doSimulation(GameTime timeStep) {
    // Simulation
    controller->doSimulation(clamp(0.0, timeStep, 0.1));
	camera->setCoordinateFrame(controller->getCoordinateFrame());
}


void doGraphics() {
    renderDevice->beginFrame();
        renderDevice->clear(true, true, true);
        renderDevice->pushState();
			    
		    camera->setProjectionAndCameraMatrix();

            renderDevice->debugDrawAxes(2);

        renderDevice->popState();
	    
    renderDevice->endFrame();
}


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
