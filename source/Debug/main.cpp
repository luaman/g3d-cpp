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

#if G3D_VER < 70000
    #error Requires G3D 7.00
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

    ThirdPersonManipulatorRef manipulator;

    float               angle;
    GameTime            gameTime;

    IFSModelRef         ifsModel;
    MD2ModelRef         md2Model;

    Array<PosedModelRef>    models;

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

    App(const GApp::Settings& settings);

    ~App();
};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
    manipulator = new ThirdPersonManipulator();
    addModule(manipulator);
}

void Demo::onInit()  {
    ifsModel = IFSModel::create("c:/projects/cpp/source/data/ifs/p51-mustang.ifs");

    CoordinateFrame obj = CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitY(), toRadians(30)), Vector3(2, 0, 0));
    
    app->debugCamera.setPosition(Vector3(5,5,5));
    app->debugCamera.lookAt(Vector3::zero());


    // World control
//    manipulator->setFrame(obj);
//    manipulator->setControlFrame(CoordinateFrame());

//    models.append(IFSModel::create("D:/games/data/ifs/square.ifs")->pose());
//    models.append(IFSModel::create("D:/games/cpp/source/data/ifs/p51-mustang.ifs")->pose(Vector3(0,2,0)));

    // Local control
    manipulator->setFrame(obj);
    manipulator->setControlFrame(obj);
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

    GApplet::onUserInput(ui);

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

    if (ui->keyDown(SDLK_LALT)) {
        app->debugPrintf("ALT PRESSED");
    }
    
    if (ui->keyDown(SDLK_LCTRL)) {
        app->debugPrintf("CTRL PRESSED");
    }

    if (ui->keyDown(SDLK_F10)) {
        app->debugPrintf("F10 PRESSED");
    }

    if (ui->keyDown(SDLK_F11)) {
        app->debugPrintf("F11 PRESSED");
    }
}


void Demo::onGraphics(RenderDevice* rd) {

    debugAssert(GLCaps::supports_two_sided_stencil());

	app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

	// Can now render from the texture
    
    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1f, .5f, 1));
 
    app->renderDevice->clear();
    if (app->sky.notNull()) {
        app->sky->render(rd, LightingParameters(G3D::toSeconds(10,00,00, AM)));
    }

    rd->pushState();
    rd->enableLighting();
    //Configure ambient light
    rd->setAmbientLightColor(Color3::white() * .25);

    for (int m = 0; m < models.size(); ++m) {
        models[m]->render(rd);
    }

    GLight light = GLight::directional(Vector3(0,1,0), Color3::white() * 0.5);
    rd->disableDepthWrite();    
    beginMarkShadows(rd);
        for (int m = 0; m < models.size(); ++m) {
            markShadows(rd, models[m], light.position);
        }
    endMarkShadows(rd);

    rd->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);
    rd->setStencilTest(RenderDevice::STENCIL_EQUAL);
    rd->setStencilConstant(0);
    rd->setAmbientLightColor(Color3::black());
    rd->setDepthTest(RenderDevice::DEPTH_LEQUAL);

    rd->setLight(0, light);
    for (int m = 0; m < models.size(); ++m) {
        models[m]->render(rd);
    }

    rd->popState();

    GApplet::onGraphics(rd);
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);
    debugShowRenderingStats = true;

    debugController.setMouseMode(FirstPersonManipulator::MOUSE_DIRECT_RIGHT_BUTTON);

    // Load objects here
    sky = Sky::fromFile(NULL, dataDir + "sky/");
    
    applet->run();

}


App::App(const GApp::Settings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}


int main(int argc, char** argv) {
	GApp::Settings settings;
    settings.useNetwork = false;
    settings.window.resizable = true;
    App(settings).run();
    return 0;
}
