/**
  @file IFSBuilder/main.cpp

  This is not a demo.  It is a piece of code I wrote to create
  IFS files which may be useful to others trying to use the
  format.

  A utility for converting some common 3D file formats into IFS format.
  This only handles some "nice" cases of 3DS, OBJ, and MD2 files-- 
  models in these formats certainly exist that cannot be converted
  by this simple utility.

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2002-04-04
 */ 

#include <G3DAll.h>
#include "IFSModel.h"
#include "IFSModelBuilder.h"

std::string             DATA_DIR        = "data/";

Log*                    debugLog		= NULL;
RenderDevice*           renderDevice	= NULL;
Font*                   font			= NULL;
UserInput*              userInput		= NULL;
Camera*					camera			= NULL;
ManualCameraController* controller      = NULL;
bool                    endProgram		= false;

IFSModel*               model           = NULL;

bool                    pauseBetweenModels = false;

RealTime getTime() {
    return SDL_GetTicks() / 1000.0;
}

void doSimulation(GameTime timeStep);
void doGraphics();
void doUserInput();

/** Expands a file spec's wildcards and returns a list of
    fully qualified filenames matching it (on Windows).  The result
    array is not cleared before files are added.
    
    On Linux, this just returns the str name in result. */
void expandWildcard(const std::string& str, Array<std::string>& result);
/**
 Returns the base name (between the last slash and the extension).
 */
std::string getFilename(const std::string& filename);



int main(int argc, char** argv) {

    // Search for the data
    for (int count = 0; (count < 3) && (! fileExists(DATA_DIR + "font/dominant.fnt")); ++count) {
        DATA_DIR = std::string("../") + DATA_DIR;
    }

    // Initialize
    debugLog	 = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(400, 400, debugLog, 1.0, false, 0, true, 8, 0, 24, 0);
    camera 	     = new Camera(renderDevice);

    font         = new Font(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    controller   = new ManualCameraController(renderDevice);
    controller->setMoveRate(.1);

    controller->setPosition(Vector3(2, 2, 2));
    controller->lookAt(Vector3(0,0,0));

    renderDevice->resetState();
	renderDevice->setColorClearValue(Color3(.1, .5, 1));

    camera->setNearPlaneZ(-.05);
    RealTime now = getTime() - 0.001, lastTime;

    std::string in("c:/inputDir/");
    std::string out("c:/outputDir/");

    Array<std::string> filename;

    expandWildcard(in + "*", filename);

    camera->setCoordinateFrame(controller->getCoordinateFrame());

    for (int i = 0; i < filename.size(); ++i) {
        std::string base = getFilename(filename[i]);
        
        if (fileExists(out + base + ".ifs")) {
            // Skip this model
            continue;
        }

        model = new IFSModel(filename[i]);
        model->name = base;
        
        if (! pauseBetweenModels) {
            model->save(out + base + ".ifs");
        }
        
        // Main loop (display 3D object)
        do {
            lastTime = now;
            now = getTime();
            RealTime timeStep = now - lastTime;

            if (pauseBetweenModels) {
                doUserInput();
                doSimulation(timeStep);
            }

            doGraphics();
   
        } while (! endProgram && pauseBetweenModels);
        
    }


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
                renderDevice->push2D();
                font->draw2DString(model->name, 10, 10, 20, Color3::WHITE, Color3::BLACK);
                renderDevice->pop2D();
            }

            //renderDevice->debugDrawAxes(2);

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

