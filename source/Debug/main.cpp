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

#if G3D_VER < 60700
    #error Requires G3D 6.07
#endif


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*          app;

    Demo(App* app);

    virtual ~Demo() {}

    virtual void init();

    virtual void doLogic();

	virtual void doNetwork();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();

    virtual void cleanup();

};




class App : public GApp {
protected:
    void main();
public:
    SkyRef              sky;

    Demo*               applet;

    App(const GAppSettings& settings);

    ~App();
};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}

char tmpStr[2000000];

void Demo::init()  {
/*
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));

    GImage maskInput(app->dataDir + "/smallfont.bmp");

    // write font mask
    TextOutput maskOutput(app->dataDir + "/font_mask.cpp");

    maskOutput.printf("unsigned char font_mask[256][2000] = {\n");

    for (int i = 0; i < 256; ++i) {
        Vector2 letterBounds = app->debugFont->get2DStringBounds(G3D::format("%c", (char)i), 12);

        maskOutput.printf("{");

        maskOutput.printf("%d,%d,", (int)letterBounds.x, (int)letterBounds.y);

        int startX = (i % 16) * app->debugFont->texelSize().x;
        int startY = (i / 16) * app->debugFont->texelSize().y;

        for (int j = 0; j < (int)app->debugFont->texelSize().y; ++j) {
            for (int k = 0; k < (int)app->debugFont->texelSize().x; ++k) {
                int pixelX = startX + k;
                int pixelY = startY + j;
                maskOutput.printf("%d,", maskInput.pixel3(pixelX, pixelY).r);
            }
        }
        maskOutput.printf("},\n");
    }

    maskOutput.printf("};\n");
    maskOutput.commit();
*/
}


void Demo::cleanup() {
    // Called when Demo::run() exits
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

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (app->sky.notNull()) {
        app->sky->render(app->renderDevice, lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);

        Draw::capsule(Capsule(Vector3::Vector3(-2,1,1), Vector3(3,4,0), 2), app->renderDevice);

    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(app->renderDevice, lighting);
    }
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    // Load objects here
    sky = Sky::create(NULL, dataDir + "sky/");
    
    applet->run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}


int main(int argc, char** argv) {

    GFont::convertRAWINItoPWF("data/smallfont", "data/font/smallfont.fnt"); 

    GAppSettings settings;
    settings.useNetwork = false;
    settings.window.fullScreen = false;
    settings.window.framed = true;
    settings.window.width = 800;
    settings.window.height = 600;
    settings.debugFontName = "console-small.fnt";
    App(settings).run();
    return 0;
}
