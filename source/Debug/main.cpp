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

#if G3D_VER < 60800
    #error Requires G3D 6.08
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

    TextureRef texture[2];

    Demo(App* app);

    virtual ~Demo() {}

    virtual void onInit();

    virtual void onLogic();

	virtual void onNetwork();

    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt);

    virtual void onGraphics(RenderDevice* rd);

    virtual void onUserInput(UserInput* ui);

    virtual void onCleanup();

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

void Demo::onInit()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));
    GApplet::init();

    texture[0] = NULL;//Texture::fromFile("0.jpg");
//    GImage im0("0.jpg");

//    GImage im(512,512,3);
//    GImage im("1.bmp");
//    im.save("1.bmp");
    
    // Fixes
//    memcpy(im.byte() + 512*3 * 25, im0.byte(), 512*3 * 24);

    // Still broken
//    memset(im.byte() + 512 * 3 * 50, 0, 512*3 * 462);

    // Still broken
  /*  

    for (int i = 0; i < 512 * 512 * 3; ++i) {
        uint8& c = *(im.byte() + i);
        c = 0;
    }
    

    // Still broken!
    for (int i = 0; i < 512 * 512 * 3; i += 3) {
        uint8& c = *(im.byte() + i);
        c = 0xFF;
    }
    */

//    memcpy(b, im.byte(), 512*512*3);

    // X = 3 fails, X = 4 succeeds
    static const int X = 3;
    uint8* b = new uint8[4*4*X];


    memset(b, 0, 4*4*X);
    for (int i = 0; i < 4 * 4 * X; i += X) {
        b[i] = 0xFF;
    }

    // Format bug only occurs when using bilinear interpolation
    texture[1] = Texture::fromMemory("Red", b, (X == 3) ? TextureFormat::RGB8 : TextureFormat::RGBA8, 4, 4);
    //, TextureFormat::AUTO, Texture::TILE, Texture::NO_INTERPOLATION, Texture::DIM_2D);
    delete[] b;
    
    b = new uint8[4*4*3];
    glBindTexture(GL_TEXTURE_2D, texture[1]->getOpenGLID());
    glGetTexImage(GL_TEXTURE_2D,
			      0,
			      GL_RGB,
			      GL_UNSIGNED_BYTE,
			      b);

    Log::common()->printf("%d %d %d", b[0], b[1], b[2]);
/*    
    alwaysAssertM(b[0] == 0xFF, "Read back value with Red != 0xFF");
    alwaysAssertM(b[1] == 0x00, "Read back value with Green != 0x00");
    alwaysAssertM(b[2] == 0x00, "Read back value with Blue != 0x00");
  */  
    delete[] b;

   // alwaysAssertM(! GLCaps::hasBug_redBlueMipmapSwap(), "Red and blue are swapped.");


    setDesiredFrameRate(90);
}


void Demo::onCleanup() {
    // Called when Demo::run() exits
}


void Demo::onLogic() {
    // Add non-simulation game logic and AI code here
}


void Demo::onNetwork() {
	// Poll net messages here
}


void Demo::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
	// Add physical simulation here.  You can make your time advancement
    // based on any of the three arguments.
}


void Demo::onUserInput(UserInput* ui) {
    if (ui->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

    if (ui->keyPressed(SDLK_F10)) {
        endApplet = true;
    }

	// Add other key handling here
}


void Demo::onGraphics(RenderDevice* rd) {

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    
    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (app->sky.notNull()) {
        app->sky->render(app->renderDevice, lighting);
    }

    app->renderDevice->push2D();
        Rect2D rect = Rect2D::xywh(0, 100, 100, 100);
        app->renderDevice->setTexture(0, texture[0]);
        Draw::rect2D(rect, rd);

        rect = rect + Vector2(200, 0);
        app->renderDevice->setTexture(0, texture[1]);
        Draw::rect2D(rect, rd);

    app->renderDevice->pop2D();
    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

//		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);

/*        
        for (int i = 0; i < 100; ++i) {
            rd->pushState();
                rd->setColor(Color3::white());
            rd->popState();
        }
  */      

    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(app->renderDevice, lighting);
    }
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    // Load objects here
    sky = NULL;//Sky::create(NULL, dataDir + "sky/");
    
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
    settings.useNetwork = false;
    App(settings).run();
    return 0;
}
