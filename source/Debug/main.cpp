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

//#include "../contrib/VideoSerializer/VideoSerializer.h"
#include "../contrib/VideoSerializer/VideoSerializer.cpp"


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

    TextureRef          im;

    App(const GAppSettings& settings);

    ~App();
};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));

/*
    GImage frame0("c:/tmp/brunette-walk-I_P-009.png");
    GImage frame1("c:/tmp/brunette-walk-I_P-010.png");

    debugAssert(frame0.channels == 3);
    debugAssert(frame1.channels == 3);

    VideoSerializer s;
    s.frameFormat = VideoSerializer::FORMAT_R8G8B8;
    s.width = frame0.width;
    s.height = frame0.height;
    s.preferredEncoding = VideoSerializer::ENCODING_SPACE_DIFF1;
    s.previousFrame = frame0.byte();
    s.currentFrame = frame1.byte();
    s.zip = false;

    int n = frame0.width * frame0.height * 3;
    int num0 = 0, num1 = 0, num2 = 0, num3 = 0;

    for (int i = 0; i < n; ++i) {
        switch (iAbs(frame1.byte()[i] - frame0.byte()[i])) {
        case 0:
            ++num0;
            break;
        case 1:
            ++num1;
            break;
        case 2:
            ++num2;
            break;
        case 3:
            ++num3;
            break;
        }
    }

    uint8* x = (uint8*)malloc(1024 * 1024 * 10);
    int compressedSize = 0;
    {
        BinaryOutput b("<memory>", G3D_LITTLE_ENDIAN);
        s.serialize(b);
        compressedSize = b.length();
        b.commit(x);
    }

    int rawSize = frame0.width * frame0.height * 3;

    GImage frame1decompressed(frame1.width, frame1.height, 3);
    s.currentFrame = frame1decompressed.byte();
    {
        BinaryInput b(x, compressedSize, G3D_LITTLE_ENDIAN);
        s.deserialize(b);
    }

    // r:1 ratio
    double ratio = double(rawSize) / double(compressedSize);

    for (int i = 0; i < 8000; ++i) {
        debugAssert(frame1decompressed.byte()[i] == frame1.byte()[i]);
    }

    app->im = Texture::fromFile("c:/tmp/brunette-walk-I_P-009.png");
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

    app->renderDevice->enableAlphaWrite();
    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (app->sky.notNull()) {
        app->sky->render(lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);
       
    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(lighting);
    }

    app->renderDevice->push2D();
//        app->renderDevice->setTexture(0, app->im);
//        Draw::rect2D(Rect2D::xywh(0,0,800,600), app->renderDevice);
    app->renderDevice->pop2D();
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    // Load objects here
    sky = Sky::create(renderDevice, dataDir + "sky/");
    
    applet->run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}


int main(int argc, char** argv) {

    GAppSettings settings;
    settings.window.width = 800;
    settings.window.height = 700;
    settings.window.alphaBits = 8;
    App(settings).run();
    return 0;
}
