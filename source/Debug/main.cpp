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

#if G3D_VER < 60900
    #error Requires G3D 6.09
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

    float               angle;
    GameTime            gameTime;

    IFSModelRef         ifsModel;
    MD2ModelRef         md2Model;

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

    gameTime += sdt;

}


void Demo::onUserInput(UserInput* ui) {
    if (ui->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

    if (ui->keyPressed(SDLK_EQUALS)) {
        angle += 5;
    }
    if (ui->keyPressed(SDLK_MINUS)) {
        angle -= 5;
    }
	// Add other key handling here

}


void Demo::onGraphics(RenderDevice* rd) {

	app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

	// Can now render from the texture
    
    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1f, .5f, 1));
    app->renderDevice->clear();

    /*
    app->renderDevice->pushState();
        rd->enableColorWrite();
        Draw::axes(rd);
    app->renderDevice->popState();
    */
    rd->push2D();
        GImage im(1024, 768, 3);
        GImage::makeCheckerboard(im);        
        TextureRef t = Texture::fromGImage("Checker", im, TextureFormat::AUTO, Texture::DIM_2D_NPOT, Texture::Parameters::video());
        rd->setTexture(0, t);
        Draw::rect2D(t->rect2DBounds(), rd);
    rd->pop2D();
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);
    debugShowRenderingStats = false;

    // Load objects here
//    sky = Sky::create(NULL, dataDir + "sky/");
    
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
    settings.window.resizable = true;
    App(settings).run();
    return 0;
}
