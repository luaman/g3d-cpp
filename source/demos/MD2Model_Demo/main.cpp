/**
  @file main.cpp

  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2003-11-07
 */ 

#include <G3DAll.h>

std::string             DATA_DIR        = "data/";

Log*                    debugLog		= NULL;
RenderDevice*           renderDevice	= NULL;
CFontRef                font			= NULL;
UserInput*              userInput		= NULL;
GCamera*				camera			= NULL;
ManualCameraController* controller      = NULL;
bool                    endProgram		= false;
GameTime                gameTime        = 0;
MD2ModelRef             model;
Array<TextureRef>       modelTexture;
MD2ModelRef             weapon;
TextureRef              weaponTexture;
MD2Model::Pose          pose(MD2Model::STAND, 0);

/** Names of all of the models available to load */
Array<std::string>      modelNameArray;

/** Index into modelNameArray*/
int                     currentModel    = 0;

/** Load a model by name */
void load(const std::string& name);
void doSimulation(GameTime timeStep);
void doGraphics();
void doUserInput();


void getModelNameArray();

int main(int argc, char** argv) {
    DATA_DIR = demoFindData();

    // Initialize
    debugLog	 = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(RenderDeviceSettings(), debugLog);
    renderDevice->setCaption("G3D::MD2Model Demo");
    camera 	     = new GCamera();

    font         = GFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    camera->setNearPlaneZ(-.1);
    controller   = new ManualCameraController(renderDevice, userInput);

    controller->setPosition(Vector3(0, 1, -13));
    controller->lookAt(Vector3(0,1.6,-8));

    renderDevice->resetState();
	renderDevice->setColorClearValue(Color3(1, 1, 1));

    getModelNameArray();

    load(modelNameArray[currentModel]);

    RealTime now = System::getTick() - 0.001, lastTime;

    controller->setActive(true);

    doSimulation(0);
    // Main loop
    do {
        lastTime = now;
        now = System::getTick();
        RealTime timeStep = now - lastTime;

        gameTime = System::getTick();

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


void doSimulation(GameTime timeStep) {
    controller->doSimulation(clamp(timeStep, 0.0, 0.1));
    Vector3 v = controller->getPosition();

    // Keep the camera above the ground plane
    if (v.y < .1) {
        controller->setPosition(Vector3(v.x, .1, v.z));
    }

	camera->setCoordinateFrame(controller->getCoordinateFrame());

    if (MD2Model::animationDeath(pose.animation)) {
        if (pose.time > 2) {
            // Bring back to life.
            pose.animation = MD2Model::STAND;
        }

    }

    pose.doSimulation(timeStep,
        userInput->keyDown(SDLK_LCTRL) || userInput->keyDown(SDLK_RCTRL) ||
        userInput->keyDown(SDLK_LSHIFT) || userInput->keyDown(SDLK_RSHIFT),
        userInput->keyDown('r'),
        userInput->keyDown('t'),
        userInput->keyPressed(SDL_LEFT_MOUSE_KEY),
        userInput->keyPressed(' ') || userInput->keyPressed(SDLK_BACKSPACE),
        userInput->keyPressed('1'),
        userInput->keyPressed('2'),
        userInput->keyPressed('3'),
        userInput->keyPressed('4'),
        userInput->keyPressed('5'),
        userInput->keyPressed('6'),
        userInput->keyPressed('7'),
        userInput->keyPressed('8'),
        userInput->keyPressed('9'),
        userInput->keyPressed('0'),
        userInput->keyPressed('-'));
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

            case 'e':
                currentModel = (currentModel + 1) % modelNameArray.size();
                load(modelNameArray[currentModel]);
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


void getModelNameArray() {
    getDirs(DATA_DIR + "quake2/players/*", modelNameArray);

    // Make sure these files all exist
    for (int i = modelNameArray.size() - 1; i >= 0; --i) {
        std::string s = modelNameArray[i];
        if (! fileExists(DATA_DIR + "quake2/players/" + modelNameArray[i] + "/tris.md2")) {
            modelNameArray.fastRemove(i);
        }
    }

    if (modelNameArray.size() == 0) {
        const char* choice[] = {"Ok"};
        prompt("MD2 Demo Error", "No MD2 models found in data/players.  (Download some from polycount.com and expand them into the data directory)", choice, true);
        exit(-1);
    }

    currentModel = 0;
}
