/**
 @file GApp.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-11-03
 @edited  2003-11-03
 */

#include <SDL.h>
#include "GLG3D/GApp.h"
#include "GLG3D/Camera.h"
#include "GLG3D/ManualCameraController.h"
#include "GLG3D/UserInput.h"

namespace G3D {


GApp::GApp(const GAppSettings& settings) {
    debugLog          = NULL;
    renderDevice      = NULL;
    debugFont         = NULL;
    userInput         = NULL;
    debugCamera       = NULL;
    debugController   = NULL;
    endProgram        = false;

    init(settings);
}


void GApp::loadFont(const std::string& fontName) {
    std::string filename = fontName;
    if (! fileExists(filename)) {

        if (fileExists(dataDir + filename)) {
            filename = dataDir + filename;
        } else if (fileExists(dataDir + "font/" + filename)) {
            filename = dataDir + "font/" + filename;
        }
    }

    if (fileExists(filename)) {
        debugFont = CFont::fromFile(renderDevice, filename);
    } else {
        debugFont = NULL;
    }
}


void GApp::init(const GAppSettings& settings) {

    debugAssertM(renderDevice == NULL, "Can't initialize GApp twice");

    dataDir = demoFindData(false);

    debugLog	 = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(settings.window, debugLog);
    debugCamera  = new Camera(renderDevice);

    loadFont(settings.debugFontName);

    userInput            = new UserInput();

    debugController      = new ManualCameraController(renderDevice, userInput);
    debugController->setMoveRate(10);
    debugController->setPosition(Vector3(0, 0, 4));
    debugController->lookAt(Vector3(0, 0, 0));
    debugController->setActive(true);

    autoResize              = true;

    _debugMode              = false;
    debugShowText           = true;
    debugQuitOnEscape       = true;
    debugTabSwitchCamera    = true;
    debugShowRenderingStats = true;
};


bool GApp::debugMode() const {
    return _debugMode;
}


void GApp::setDebugMode(bool b) {
    _debugMode = b;
}


void GApp::debugPrintf(const char* fmt ...) {
    if (debugMode() && debugShowText) {

        va_list argList;
        va_start(argList, fmt);
        std::string s = G3D::vformat(fmt, argList);
        va_end(argList);

        debugText.append(s);
    }    
}


GApp::~GApp() {
    debugFont = NULL;
    delete debugController;
    delete debugCamera;
    delete userInput;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;
    renderDevice = NULL;
    debugLog = NULL;
}

void GApp::renderDebugInfo() {
    if (debugMode() && (debugFont != NULL)) {
        renderDevice->push2D();
            Color3 color = Color3::WHITE;
            double size = 10;

            double x = 5;
            Vector2 pos(x, 5);

            if (debugShowRenderingStats) {
                Color3 statColor = Color3::YELLOW;

                debugFont->draw2D(renderDevice->getCardDescription(), pos, size, color, Color3::BLACK);

                pos.y += size * 1.5;

                std::string s = format("%d fps", iRound(renderDevice->getFrameRate()));
                debugFont->draw2D(s, pos, size, statColor, Color3::BLACK);

                pos.x += size * 8;
                s = format("%3.1gM tris", iRound(renderDevice->getTrianglesPerFrame() / 1e5) * .1);
                debugFont->draw2D(s, pos, size, statColor, Color3::BLACK);

                pos.x += size * 8;
                s = format("%3.1gM tris/s", iRound(renderDevice->getTrianglesPerFrame() / 1e5) * .1);
                debugFont->draw2D(s, pos, size, statColor, Color3::BLACK);

                pos.x = x;
                pos.y += size * 3;
            }

            for (int i = 0; i < debugText.length(); ++i) {
                debugFont->draw2D(debugText[i], pos, size, color, Color3::BLACK);
                pos.y += size * 1.5;
            }


        renderDevice->pop2D();
    }
}

//////////////////////////////////////////////


GApplet::GApplet(GApp* _app) : app(_app) {
    debugAssert(app != NULL);
}


RealTime getTime() {
    return SDL_GetTicks() / 1000.0;
}


void GApplet::run() {

    endApplet = false;

    init();

    RealTime now = getTime() - 0.001, lastTime;

    // Main loop
    do {
        lastTime = now;
        now = getTime();
        RealTime timeStep = now - lastTime;

        doUserInput();
        doNetwork();
        doSimulation(timeStep);
        doLogic();
        app->renderDevice->beginFrame();
            app->renderDevice->pushState();
                doGraphics();
                app->renderDebugInfo();
            app->renderDevice->popState();
        app->renderDevice->endFrame();
        app->debugText.clear();
   
    } while (! app->endProgram && ! endApplet);

    cleanup();
}


void GApplet::doSimulation(RealTime rdt) {
    if (app->debugController->active()) {
        app->debugController->doSimulation(clamp(rdt, 0.0, 0.1));
    	app->debugCamera->setCoordinateFrame(app->debugController->getCoordinateFrame());
    }
}


void GApplet::doUserInput() {

    app->userInput->beginEvents();

    // Event handling
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
	        app->endProgram = true;
	        break;

        case SDL_VIDEORESIZE:
            if (app->autoResize) {
                app->renderDevice->notifyResize(event.resize.w, event.resize.h);
                Rect2D full(0, 0, app->renderDevice->getWidth(), app->renderDevice->getHeight());
                app->renderDevice->setViewport(full);
            }
            break;

	    case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                if (app->debugMode() && app->debugQuitOnEscape) {
                    app->endProgram = true;
                }
                break;

            case SDLK_TAB:
                if (app->debugMode() && app->debugTabSwitchCamera) {
                    app->debugController->setActive(! app->debugController->active());
                }
                break;

            // Add other key handlers here
            default:;
            }
            break;

        // Add other event handlers here

        default:;
        }

        processEvent(event);

        app->userInput->processEvent(event);
    }

    app->userInput->endEvents();
}

}
