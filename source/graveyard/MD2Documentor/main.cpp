/**
  @file main.cpp

  This is not a demo; it is the tool the G3D maintainers use
  to produce the MD2 screenshot for documentation.  It is not
  officially supported.

  @maintainer Kevin Egan, ktegan@cs.rpi.edu

  @created 2002-08-21
  @edited  2003-09-05
 */ 

#include <G3DAll.h>

/**
 Where the input is.
 */
std::string             DATA_DIR        = "data/";

/**
 Where the output goes.
 */
std::string             OUTPUT_DIR      = "c:/tmp/md2shot/";

Log*                    debugLog		= NULL;
RenderDevice*           renderDevice	= NULL;
CFontRef                font			= NULL;
UserInput*              userInput		= NULL;
GCamera*					camera			= NULL;
ManualCameraController* controller      = NULL;
bool                    endProgram		= false;
GameTime                gameTime        = 0;
MD2Model                model;
Array<TextureRef>       modelTexture;
MD2Model                weapon;
TextureRef              weaponTexture;
MD2Model::Pose          pose(MD2Model::STAND, 0);
bool                    singleScreen    = false;
bool                    batchScreen     = true;
int                     batchWidth      = 700;
int                     batchHeight     = 700;
GImage*                 batchImage      = NULL;
int                     screenWidth     = 800;
int                     screenHeight    = 800;


/** Names of all of the models available to load */
Array<std::string>      modelNameArray;

/** Index into modelNameArray*/
int                     currentModel    = 0;

/** Load a model by name */
void load(const std::string& name);
void doSimulation(GameTime timeStep);
void doGraphics();
void doScreenshot();
void doUserInput();
void setCameraStandard();
void writeBatchImage();
void pasteToBatchImage();

RealTime getTime() {
    return SDL_GetTicks() / 1000.0;
}

int main(int argc, char** argv) {
    DATA_DIR = demoFindData();

    // Initialize
    debugLog	 = new Log();
    renderDevice = new RenderDevice();

    
    // we want to fit 5x3 characters on a 700x700 image
    if (batchScreen) {
        screenWidth = batchWidth / 5;
        screenHeight = batchHeight / 3;
        batchWidth = screenWidth * 5;
        batchHeight = screenHeight * 3;
        batchImage = new GImage(batchWidth, batchHeight, 3);

        // clear to white, which is the background color of the screenshots
        memset(batchImage->byte(), 255, batchWidth * batchHeight * 3);
    }

    renderDevice->init(screenWidth, screenHeight, debugLog, 1.0, false,
        2*1024*1024, true, 8, 0, 24, 0);
    renderDevice->setCaption("G3D::MD2Model Demo");
    camera 	     = new GCamera(renderDevice);

    font         = GFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    controller   = new ManualCameraController(renderDevice);
    setCameraStandard();

    renderDevice->resetState();
	renderDevice->setColorClearValue(Color3(1, 1, 1));
    
    getDirs(DATA_DIR + "quake2/players/*", modelNameArray);

    // filter out directories that don't have valid files in them,
    // like the CVS directory
    for (int i = 0; i < modelNameArray.size(); ) {
        Array<std::string> md2Files;
        getFiles(DATA_DIR + "quake2/players/" +
            modelNameArray[i] + "/tris.md2", md2Files);
        if (md2Files.size() == 0) {
            modelNameArray.remove(i);
        } else {
            i++;
        }
    }

    if (modelNameArray.size() == 0) {
        const char* choice[] = {"Ok"};
        prompt("MD2 Demo Error", "No MD2 models found in data/players.  (Download some"
            "from polycount.com and expand them into the data directory)", choice, true);
        exit(-1);
    }

    currentModel = 0;

    load(modelNameArray[currentModel]);

    RealTime now = getTime() - 0.001, lastTime;

    doSimulation(0);
    // Main loop
    do {
        lastTime = now;
        now = getTime();
        RealTime timeStep = now - lastTime;

        gameTime = getTime();

        doUserInput();

        doSimulation(timeStep);

        doGraphics();
   
        doScreenshot();
    } while (! endProgram);


    // Cleanup
    delete userInput;
    delete controller;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;

    return 0;
}


void setCameraStandard()
{
    camera->setNearPlaneZ(-.1);
    if (batchScreen) {
        controller->setPosition(Vector3(0, 2, -15));
        controller->lookAt(Vector3(0,1.6,-8));
    } else {
        controller->setPosition(Vector3(0, 1, -13));
        controller->lookAt(Vector3(0,1.6,-8));
    }
	camera->setCoordinateFrame(controller->getCoordinateFrame());
}

void writeBatchImage()
{
    std::string subdir = OUTPUT_DIR;
    batchImage->save(subdir + "collage" +
                    format("%03d", currentModel / 15) + ".jpg");

    // clear to white, which is the background color of the screenshots
    memset(batchImage->byte(), 255, batchWidth * batchHeight * 3);
}

void pasteToBatchImage()
{
    int collageNum = currentModel % 15;
    GImage curScreen;

    int offsetX = (collageNum % 5) * screenWidth;
    int offsetY = (collageNum / 5) * screenHeight;

    renderDevice->screenshotPic(curScreen);
    bool ret = GImage::pasteSubImage(*batchImage, curScreen,
        offsetX, offsetY, 0, 0, screenWidth, screenHeight);
	debugAssert(ret);

}


void doScreenshot() {
    if (singleScreen || batchScreen) {
        std::string subdir = OUTPUT_DIR;
        
        createDirectory(subdir);

        if (singleScreen) {
            renderDevice->screenshot(subdir + modelNameArray[currentModel]);
            singleScreen = false;
        } else {
            // paste current screenshot to one panel of batch image
            pasteToBatchImage();
            if (currentModel % 15 == 14) {
                // batch image is full, write it out
                writeBatchImage();
            }
            if (currentModel == modelNameArray.size() - 1) {
                // wrapped around to beginning, write out the final
                // batch image and exit
                writeBatchImage();
                exit(0);
            }
            currentModel = (currentModel + 1) % modelNameArray.size();
            load(modelNameArray[currentModel]);
        }
    }
}


void doSimulation(GameTime timeStep) {

    if (singleScreen || batchScreen) {
        return;
    }

    controller->doSimulation(clamp(timeStep, 0.0, 0.1), *userInput);
    Vector3 v = controller->getPosition();

    // Keep the camera above the ground plane
    if (v.y < .1) {
        controller->setPosition(Vector3(v.x, .1, v.z));
    }

	camera->setCoordinateFrame(controller->getCoordinateFrame());

    pose.time += timeStep;

    if (model.animationDeath(pose.animation)) {
        if (pose.time > 2) {
            // Bring back to life.
            pose.animation = MD2Model::STAND;
        }

    } else {

        pose = model.choosePose(pose,
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

            case 'z':
                singleScreen = true;
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
