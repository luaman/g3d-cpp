/**
  @file IFSBuilder/main.cpp

  This is not a demo.  It is a piece of code I wrote to create
  IFS files which may be useful to others trying to use the
  format.

  A utility for converting some common 3D file formats into IFS format.
  This only handles a subset of all possible 3DS, OBJ, and MD2 files-- 
  models in these formats certainly exist that cannot be converted
  by this simple utility.  You can use IFSModelBuilder to create
  IFS files from triangle soups and write them out.  This also doubles
  as a model viewer, but it is not as nice as the IFSDemo one which
  has smoothed surface normals and lighting.

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2003-10-06
 */ 

#include <G3DAll.h>
#include "IFSModel.h"
#include "IFSModelBuilder.h"

std::string             DATA_DIR        = "data/";

Log*                    debugLog        = NULL;
RenderDevice*           renderDevice    = NULL;
CFontRef                font            = NULL;
UserInput*              userInput       = NULL;
Camera*                 camera          = NULL;
ManualCameraController* controller      = NULL;
bool                    endProgram      = false;

IFSModel*               model           = NULL;

bool                    pauseBetweenModels = true;

RealTime getTime() {
    return SDL_GetTicks() / 1000.0;
}

void doSimulation(GameTime timeStep);
void doGraphics();
void doUserInput();

/**
 Returns the base name (between the last slash and the extension).
 */
std::string getFilename(const std::string& filename);
IFSModel* makeDinosaur();


int main(int argc, char** argv) {

    // Search for the data
    for (int count = 0; (count < 3) && (! fileExists(DATA_DIR + "font/dominant.fnt")); ++count) {
        DATA_DIR = std::string("../") + DATA_DIR;
    }

    // Initialize
    debugLog     = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(600, 600, debugLog, 1.0, false, 0, true, 8, 0, 24, 0);
    camera       = new Camera(renderDevice);

    font         = CFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    controller   = new ManualCameraController(renderDevice, userInput);
    controller->setMoveRate(.1);

    controller->setPosition(Vector3(1.5, 1.5, 1.5));
    controller->lookAt(Vector3(0,0,0));

    renderDevice->resetState();
    renderDevice->setColorClearValue(Color3(.5, .7, .8));

    camera->setNearPlaneZ(-.05);
    RealTime now = getTime() - 0.001, lastTime;

//    std::string in("d:/graphics3d/book/data/ifs/bunny.ifs");
//    std::string in("D:/users/morgan/Projects/_Silhouette/models/ROCKHRSE.ifs");
    std::string in("d:/libraries/g3d-6_00/data/ifs/teapot.ifs");
//    std::string in("d:/libraries/g3d-6_00/data/ifs/sphere.ifs");
    std::string outDir("d:/libraries/g3d-6_00/data/ifs/");

    Array<std::string> filename;
    getFiles(in, filename, true);

    camera->setCoordinateFrame(controller->getCoordinateFrame());
    controller->setActive(true);

    for (int i = 0; i < filename.size(); ++i) {
        std::string base = getFilename(filename[i]);
        
        /*
        // Code to prevent recreation of models
        if (fileExists(outDir + base + ".ifs")) {
            // Skip this model
            continue;
        }
        */

        model = new IFSModel(filename[i]);
       // model->name = "Teapot";
        
        if (! pauseBetweenModels) {
            model->save(outDir + base + ".ifs");
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
    delete controller;
    delete userInput;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;
    delete model;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////


void doSimulation(GameTime timeStep) {
    // Simulation
    controller->doSimulation(max(0.1, min(0, timeStep)));
    camera->setCoordinateFrame(controller->getCoordinateFrame());
}


void doGraphics() {
    renderDevice->beginFrame();
        renderDevice->clear(true, true, true);
        renderDevice->pushState();
                
            camera->setProjectionAndCameraMatrix();

            if (model != NULL) {
                model->render();
                renderDevice->push2D();
                    double y = 10;
                    font->draw2D(model->name, 10, y, 20, Color3::WHITE, Color3::BLACK); y += 30;
                    font->draw2D(format("%d verts", model->numVertices()), 15, y, 15, Color3::YELLOW, Color3::BLACK); y += 20;
                    font->draw2D(format("%d faces", model->numFaces()), 15, y, 15, Color3::WHITE, Color3::BLACK); y += 20;
                    if (model->numBrokenEdges() > 0) {
                        font->draw2D(format("%d broken edges", model->numBrokenEdges()), 15, y, 15, Color3::RED, Color3::BLACK); y += 20;
                    }

                    y = renderDevice->getHeight();
                    font->draw2D(format("Vertices within radius %g collapsed", IFSModelBuilder::CLOSE), 10, y - 15, 10, Color3::BLACK);
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

