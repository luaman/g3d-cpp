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
  @edited  2003-12-18
 */ 

#include <G3DAll.h>
#include "IFSModel.h"
#include "IFSModelBuilder.h"

/** Defined in IFSModelBuilder.cpp. Collapse radius. */
extern double close;
std::string             DATA_DIR        = "data/";

Log*                    debugLog        = NULL;
RenderDevice*           renderDevice    = NULL;
CFontRef                font            = NULL;
UserInput*              userInput       = NULL;
GCamera                 camera;
ManualCameraController* controller      = NULL;
bool                    endProgram      = false;

XIFSModel*              model           = NULL;

bool                    pauseBetweenModels = true;

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
    DATA_DIR = demoFindData();

    // Initialize
    debugLog     = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(RenderDeviceSettings(), debugLog);

    font         = GFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    controller   = new ManualCameraController(renderDevice, userInput);

    controller->setMoveRate(1);

    controller->setPosition(Vector3(1.5, 1.5, 1.5));
    controller->lookAt(Vector3(0,0,0));

    renderDevice->resetState();
    renderDevice->setColorClearValue(Color3(.5, .7, .8));

    camera.setNearPlaneZ(-.05);
    RealTime now = System::getTick() - 0.001, lastTime;

//    std::string in("d:/libraries/g3d-6_00/data/ifs/elephant.ifs");
    std::string in("c:/tmp/models/elephant.sm");

//    std::string in("C:/Documents and Settings/morgan/Desktop/cars/dmc/delorean.3ds");

    //std::string outDir("d:/libraries/g3d-6_00/data/ifs/");
    std::string outDir("d:/graphics3d/book/data/ifs/");

    Array<std::string> filename;
    getFiles(in, filename, true);

    camera.setCoordinateFrame(controller->getCoordinateFrame());
    controller->setActive(true);

    debugAssertM(filename.size() > 0, "No files found");

    //renderDevice->setCullFace(RenderDevice::CULL_NONE);

    for (int i = 0; i < filename.size(); ++i) {
        std::string base = getFilename(filename[i]);
        
        /*
        // Code to prevent recreation of models
        if (fileExists(outDir + base + ".ifs")) {
            // Skip this model
            continue;
        }
        */

        model = new XIFSModel(filename[i]);
        model->name = "Saddle";

        if (! pauseBetweenModels) {
            model->save(outDir + base + ".ifs");
        }
        model->save(outDir + "low-poly-saddle.ifs");

        // Main loop (display 3D object)
        do {
            lastTime = now;
            now = System::getTick();
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
    camera.setCoordinateFrame(controller->getCoordinateFrame());
}


void doGraphics() {
    renderDevice->beginFrame();
        renderDevice->clear(true, true, true);
        renderDevice->pushState();
                
            renderDevice->setProjectionAndCameraMatrix(camera);

            if (model != NULL) {
                model->render();
                renderDevice->push2D();
                    double y = 10;
                    font->draw2D(model->name, Vector2(10, y), 20, Color3::WHITE, Color3::BLACK); y += 30;
                    font->draw2D(format("%d verts", model->numVertices()), Vector2(15, y), 15, Color3::YELLOW, Color3::BLACK); y += 20;
                    font->draw2D(format("%d faces", model->numFaces()), Vector2(15, y), 15, Color3::WHITE, Color3::BLACK); y += 20;
                    if (model->numBrokenEdges() > 0) {
                        font->draw2D(format("%d broken edges", model->numBrokenEdges()), Vector2(15, y), 15, Color3::RED, Color3::BLACK); y += 20;
                    }

                    y = renderDevice->getHeight();
                    font->draw2D(format("Vertices within radius %g collapsed", close), Vector2(10, y - 15), 10, Color3::BLACK);
                renderDevice->pop2D();
            }
            
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

