
/**
  @file demos/main.cpp

  This is a sample main.cpp to get you started with G3D.  It is
  designed to make writing an application easy.  Although the
  GApp/GApplet infrastructure is helpful for most projects,
  you are not restricted to using it-- choose the level of
  support that is best for your project (see the G3D Map in the
  documentation).

  @author Morgan McGuire, matrix@graphics3d.com
 */

#include <G3DAll.h>

#if G3D_VER < 60500
    #error Requires G3D 6.05
#endif


class App : public GApp {
protected:
    void main();
public:
    SkyRef              sky;

    App(const GAppSettings& settings);
};

/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*					app;

    IFSModelRef                 model;

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {

    model = IFSModel::create("D:/libraries/g3d-6_05-b01/data/ifs/cube.ifs");

}


void Demo::init()  {
	// Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 0, 10));
    app->debugCamera.lookAt(Vector3(0, 0, 0));
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

    if (app->userInput->keyPressed(' ')) {
        CoordinateFrame x = app->debugController.getCoordinateFrame();
        app->debugController.setCoordinateFrame(x);
    }
}


void Demo::doGraphics() {

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);
    app->renderDevice->setObjectToWorldMatrix(CoordinateFrame());

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(true, true, true);
    Draw::axes(app->renderDevice);

//    debugAssertM(false, "Intentional assertion");

}



void App::main() {
	setDebugMode(true);
	debugController.setActive(false);
    Demo(this).run();
}

App::App(const GAppSettings& settings) : GApp(settings, Win32Window::create(settings.window)) {
//App::App(const GAppSettings& settings) : GApp(settings) {    
}


int main(int argc, char** argv) {

    /*

#9 Format Description
nSize:				40
nVersion:			1
dwFlags:			PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER|
iPixelType:			0
cColorBits:			32
cRedBits:			8
cRedShift:			16
cGreenBits:			8
cGreenShift:			8
cBlueBits:			8
cBlueShift:			0
cAlphaBits:			0
cAlphaShift:			0
cAccumBits:			64
cAccumRedBits:			16
cAccumGreenBits:		16
cAccumBlueBits:			16
cAccumAlphaBits:		16
cDepthBits:			24
cStencilBits:			8
cAuxBuffers:			4
iLayerType:			0
bReserved:			0
dwLayerMask:			0
dwDamageMask:			0
    */
    GAppSettings settings;
    settings.window.depthBits = 24;
    settings.window.stencilBits = 8;
    settings.window.alphaBits = 0;
    settings.window.rgbBits = 8;
    settings.window.fsaaSamples = 4;
    App(settings).run();
    return 0;
}
