/**
 @file GApp.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-11-03
 @edited  2005-02-24
 */

#include "G3D/platform.h"

#include "GLG3D/GApp.h"
#include "G3D/GCamera.h"
#include "GLG3D/ManualCameraController.h"
#include "GLG3D/UserInput.h"
#include "GLG3D/GWindow.h"
#include "GLG3D/Shader.h"

namespace G3D {

/** Attempt to write license file */
static void writeLicense() {
    FILE* f = fopen("g3d-license.txt", "wt");
    if (f != NULL) {
        fprintf(f, "%s", license().c_str());
        fclose(f);
    }
}


GApp::GApp(const GAppSettings& settings, GWindow* window) {
    debugLog          = NULL;
    debugFont         = NULL;
    endProgram        = false;
    _debugControllerWasActive = false;

    if (settings.dataDir == "<AUTO>") {
        dataDir = demoFindData(false);
    } else {
        dataDir = settings.dataDir;
    }


    if (settings.writeLicenseFile && ! fileExists("g3d-license.txt")) {
        writeLicense();
    }

    debugLog	 = new Log(settings.logFilename);
    renderDevice = new RenderDevice();
    if (window != NULL) {
        renderDevice->init(window, debugLog);
    } else {
        renderDevice->init(settings.window, debugLog);
    }

    debugAssertGLOk();

    _window = renderDevice->window();

    if (settings.useNetwork) {
        networkDevice = new NetworkDevice();
        networkDevice->init(debugLog);
    } else {
        networkDevice = NULL;
    }

    debugCamera  = GCamera();

    debugAssertGLOk();
    loadFont(settings.debugFontName);
    debugAssertGLOk();

    userInput = new UserInput();


    debugController.init(renderDevice, userInput);
    debugController.setMoveRate(10);
    debugController.setPosition(Vector3(0, 0, 4));
    debugController.lookAt(Vector3(0, 0, 0));
    debugController.setActive(true);

    autoResize                  = true;
    _debugMode                  = false;
    debugShowText               = true;
    debugQuitOnEscape           = true;
    debugTabSwitchCamera        = true;
    debugShowRenderingStats     = true;
    catchCommonExceptions       = true;

    debugAssertGLOk();
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
        debugFont = GFont::fromFile(renderDevice, filename);
    } else {
        debugLog->printf(
            "Warning: G3D::GApp could not load font \"%s\".\n"
            "This may be because the G3D::GAppSettings::dataDir was not\n"
            "properly set in main().\n",
            filename.c_str());

        debugFont = NULL;
    }
}


bool GApp::debugMode() const {
    return _debugMode;
}


void GApp::setDebugMode(bool b) {
    if (! b) {
        _debugControllerWasActive = debugMode();
    } else {
        debugController.setActive(_debugControllerWasActive);
    }
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
    if (networkDevice) {
        networkDevice->cleanup();
        delete networkDevice;
    }

    debugFont = NULL;
    delete userInput;
    userInput = NULL;
    renderDevice->cleanup();
    delete renderDevice;
    renderDevice = NULL;
    delete debugLog;
    debugLog = NULL;
}


void GApp::run() {
    if (catchCommonExceptions) {
        try {
            main();
        } catch (const char* e) {
            alwaysAssertM(false, e);
        } catch (const GImage::Error& e) {
            alwaysAssertM(false, e.reason + "\n" + e.filename);
        } catch (const std::string& s) {
            alwaysAssertM(false, s);
        } catch (const TextInput::WrongTokenType& t) {
            alwaysAssertM(false, t.message);
        } catch (const TextInput::WrongSymbol& t) {
            alwaysAssertM(false, t.message);
        } catch (const VertexAndPixelShader::ArgumentError& e) {
            alwaysAssertM(false, e.message);
        } catch (const LightweightConduit::PacketSizeException& e) {
            alwaysAssertM(false, e.message);
        }
    } else {
        main();
    }
}


void GApp::renderDebugInfo() {
    if (debugMode() && ! debugFont.isNull()) {
        // Capture these values before we render debug output
        int majGL  = renderDevice->debugNumMajorOpenGLStateChanges();
        int majAll = renderDevice->debugNumMajorStateChanges();
        int minGL  = renderDevice->debugNumMinorOpenGLStateChanges();
        int minAll = renderDevice->debugNumMinorStateChanges();

        renderDevice->push2D();
            Color3 color = Color3::white();
            double size = 10;

            double x = 5;
            Vector2 pos(x, 5);

            if (debugShowRenderingStats) {

                Color3 statColor = Color3::yellow();

                debugFont->draw2D(renderDevice->getCardDescription() + "   " + System::version(), 
                    pos, size, color, Color3::black());
                pos.y += size * 1.5;

                std::string s = format("%d fps", iRound(renderDevice->getFrameRate()));
                debugFont->draw2D(s, pos, size, statColor, Color3::black());

                pos.x += size * 8;
                s = format("%3.1gM tris", iRound(renderDevice->getTrianglesPerFrame() / 1e5) * .1);
                debugFont->draw2D(s, pos, size, statColor, Color3::black());

                pos.x += size * 8;
                s = format("%3.1gM tris/s", iRound(renderDevice->getTrianglesPerFrame() / 1e5) * .1);
                debugFont->draw2D(s, pos, size, statColor, Color3::black());

                pos.x += size * 14;
                s = format("GL Calls: %d/%d Maj; %d/%d Min",
                    majGL, majAll, minGL, minAll);
                debugFont->draw2D(s, pos, size, statColor, Color3::black());

                pos.x = x;
                pos.y += size * 3;
            }

            for (int i = 0; i < debugText.length(); ++i) {
                debugFont->draw2D(debugText[i], pos, size, color, Color3::black());
                pos.y += size * 1.5;
            }


        renderDevice->pop2D();
    }
}

//////////////////////////////////////////////


GApplet::GApplet(GApp* _app) : app(_app) {
    debugAssert(app != NULL);
}


void GApplet::beginRun() {
    endApplet = false;

    init();

    // Move the controller to the camera's location
    app->debugController.setCoordinateFrame(app->debugCamera.getCoordinateFrame());

    now = System::getTick() - 0.001;
}


void GApplet::oneFrame() {
    lastTime = now;
    now = System::getTick();
    RealTime timeStep = now - lastTime;

    // User input
    doUserInput();

    // Network
    doNetwork();

    // Simulation
    if (app->debugController.active()) {
        app->debugController.doSimulation(clamp(timeStep, 0.0, 0.1));
    	app->debugCamera.setCoordinateFrame(app->debugController.getCoordinateFrame());
    }
    doSimulation(timeStep);

    // Logic
    doLogic();

    // Graphics
    app->renderDevice->beginFrame();
        app->renderDevice->pushState();
            doGraphics();
        app->renderDevice->popState();
        app->renderDebugInfo();
    app->renderDevice->endFrame();
    app->debugText.clear();

    if ((endApplet || app->endProgram) && app->window()->requiresMainLoop()) {
        app->window()->popLoopBody();
    }
}


void GApplet::endRun() {
    cleanup();

    Log::common()->section("Files Used");
    for (int i = 0; i < _internal::currentFilesUsed.size(); ++i) {
        Log::common()->println(_internal::currentFilesUsed[i]);
    }
    Log::common()->println("");

    if (app->window()->requiresMainLoop() && app->endProgram) {
        exit(0);
    }
}


void GApplet::run() {

    if (app->window()->requiresMainLoop()) {

        app->window()->pushLoopBody(this);

    } else {
        beginRun();

        // Main loop
        do {
            oneFrame();   
        } while (! app->endProgram && ! endApplet);

        endRun();
    }
}


void GApplet::doUserInput() {

    app->userInput->beginEvents();

    // Event handling
    GEvent event;
    while (app->window()->pollEvent(event)) {
        switch(event.type) {
        case SDL_QUIT:
	        app->endProgram = true;
	        break;

        case SDL_VIDEORESIZE:
            if (app->autoResize) {
                app->renderDevice->notifyResize(event.resize.w, event.resize.h);
                Rect2D full = Rect2D::xywh(0, 0, app->renderDevice->getWidth(), app->renderDevice->getHeight());
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
                // Make sure it wasn't ALT-TAB that was pressed !
                if (app->debugMode() && app->debugTabSwitchCamera && 
                    ! (app->userInput->keyDown(SDLK_RALT) || app->userInput->keyDown(SDLK_LALT))) {
                    app->debugController.setActive(! app->debugController.active());
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
