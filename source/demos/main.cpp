/**
  @file demos/main.cpp

  This is a sample main.cpp to get you started with G3D.  It is
  designed to make writing an application easy.  You are not
  restricted to using this infrastructure-- choose the level of
  support that is best for your project:

  Application level:
     G3D::GApp, G3D::GApplet

  Scene level:
     G3D::MD2Model, G3D::IFSModel, G3D::PosedModel, G3D::ManualCameraController
  
  Rendering and UI Abstraction:
     G3D::RenderDevice, G3D::CFont, G3D::Sky, G3D::UserInput

  OpenGL Abstraction (use your own OpenGL calls and your own event model):
     G3D::Texture, G3D::gl*

  Math: (use OpenGL/DirectX/other rendering and your own event model) 
     G3D::Vector3, G3D::Array

  @author Morgan McGuire, matrix@graphics3d.com
 */

// TODO: move me to platform.h

// On MSVC, we need to link against the multithreaded DLL version of
// the C++ runtime because that is what SDL and ZLIB are compiled
// against.  This is not the default for MSVC, so we set the following
// defines to force correct linking.  
//
// For documentation on compiler options, see:
//  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccore/html/_core_.2f.md.2c_2f.ml.2c_2f.mt.2c_2f.ld.asp
//  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccore98/HTML/_core_Compiler_Reference.asp
//

#ifndef _DLL
	#define _DLL
#endif

#ifndef _MT
	#define _MT
#endif

#ifdef _STATIC_CPPLIB
	#undef _STATIC_CPPLIB
#endif

#ifdef _DEBUG
    #pragma comment(linker, "/NODEFAULTLIB:LIBCD.LIB")
    #pragma comment(linker, "/DEFAULTLIB:MSVCRTD.LIB")
#else
    #pragma comment(linker, "/NODEFAULTLIB:LIBC.LIB")
    #pragma comment(linker, "/DEFAULTLIB:MSVCRT.LIB")
#endif

#include "../include/G3DAll.h"


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    SkyRef              sky;

    Demo(GApp* app);    
    virtual void init();
    virtual void doLogic();
	virtual void doNetwork();
    virtual void doSimulation(SimTime dt);
    virtual void doGraphics();
    virtual void cleanup();
};


Demo::Demo(GApp* app) : GApplet(app) {
	// Load objects hrere
    sky = Sky::create(app->renderDevice, app->dataDir + "sky/");
}


void Demo::init()  {
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));

	// Create scene here (called every time Demo::run() is invoked)
}


void Demo::cleanup() {
	// Destroy scene here (called every time Demo::run() is invoked)
}


void Demo::doNetwork() {
	// Poll net messages here
}

void Demo::doSimulation(SimTime dt) {
	// Add physical simulation here
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

	// Add other key handling here
}


void Demo::doGraphics() {
    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));
    app->renderDevice->clear(sky == NULL, true, true);

    sky->render(lighting);
    
    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);
    
    app->renderDevice->disableLighting();

    sky->renderLensFlare(lighting);

}


int main(int argc, char** argv) {

    GAppSettings settings;

    GApp app(settings);

    app.setDebugMode(true);
    app.debugController.setActive(true);

    Demo applet(&app);

    applet.run();

    return 0;
}


