/**
  @file IFSBuilder/main.cpp

  A utility for converting some common 3D file formats into IFS format.
  This only handles some "nice" cases of 3DS, OBJ, and MD2 files-- 
  models in these formats certainly exist that cannot be converted
  by this simple utility.

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2002-04-03
 */ 

#include <G3DAll.h>
#include "IFSModel.h"
#include "IFSModelBuilder.h"

std::string             DATA_DIR        = "d:/libraries/g3d-5_00/data/";

Log*                    debugLog		= NULL;
RenderDevice*           renderDevice	= NULL;
Font*                   font			= NULL;
UserInput*              userInput		= NULL;
Camera*					camera			= NULL;
ManualCameraController* controller      = NULL;
bool                    endProgram		= false;

IFSModel*               model           = NULL;

RealTime getTime() {
    return SDL_GetTicks() / 1000.0;
}

void doSimulation(GameTime timeStep);
void doGraphics();
void doUserInput();


int main(int argc, char** argv) {

    // Initialize
    debugLog	 = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(800, 600, debugLog, 1.0, false, 0, true, 8, 0, 24, 0);
    camera 	     = new Camera(renderDevice);

    font         = new Font(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    controller   = new ManualCameraController(renderDevice);
    controller->setMoveRate(.1);

    controller->setPosition(Vector3(5, 10, 5));
    controller->lookAt(Vector3(-2,3,-5));

    renderDevice->resetState();
	renderDevice->setColorClearValue(Color3(.1, .5, 1));

    RealTime now = getTime() - 0.001, lastTime;

    std::string in("D:/tmp/obj/");
    std::string out("D:/users/morgan/Projects/Silhouette/models/");

    model = new IFSModel(in + "747.obj");
    model->name = "Blade";
    //model->save(out + "q2mdl-blade.ifs");

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
    delete font;
    delete userInput;
    delete controller;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;
    delete model;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////


void doSimulation(GameTime timeStep) {
    // Simulation
    controller->doSimulation(max(0.1, min(0, timeStep)), *userInput);
	camera->setCoordinateFrame(controller->getCoordinateFrame());
}


void doGraphics() {
    renderDevice->beginFrame();
        renderDevice->clear(true, true, true);
        renderDevice->pushState();
			    
		    camera->setProjectionAndCameraMatrix(renderDevice->getWidth(), 
                                                 renderDevice->getHeight());


            if (model != NULL) {
                model->render();
            }

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

            }
            break;

            // Add other event handlers here
        }

        userInput->processEvent(event);
    }

    userInput->endEvents();
}