
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

    PhysicsFrame                current;

    PhysicsFrame                frame;
    PhysicsFrame                oldDeltaFrame;

    Demo(App* app);    

    virtual void init();

    virtual void doSimulation(RealTime dt);

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {

    model = IFSModel::create("D:/libraries/g3d-6_05-b01/data/ifs/p51-mustang.ifs", 5);

}


void Demo::init()  {
	// Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 0, 10));
    app->debugCamera.lookAt(Vector3(0, 0, 0));
}


void Demo::doSimulation(RealTime dt) {
    RealTime t = System::time();

    // Original frame
    PhysicsFrame estimated(CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitY(), toRadians(-45)), Vector3(.4,0,0)));

    // Where we should have been
    PhysicsFrame correct(CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitZ(), toRadians(0)), Vector3(0,0,0)));

    // Eventual goal (not generally known)
    PhysicsFrame target(CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitZ(), toRadians(90)), Vector3(0,2,0)));

    // Highest velocity at 0.5
    double alpha = (cos(t) + 1.0) / 2.0;
    // Where we *should* be (usually computed by simulation)
    current = correct.lerp(target, alpha);

    // old Delta, "correct - estimated"
    PhysicsFrame D;
    D.translation = correct.translation - estimated.translation;
    D.rotation = correct.rotation * estimated.rotation.inverse();

    // Compose: position = current + delta;
    PhysicsFrame scaledDelta = D.lerp(PhysicsFrame(), alpha);
    frame.translation = current.translation + scaledDelta.translation;
    frame.rotation = current.rotation * scaledDelta.rotation;
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
    app->renderDevice->enableLighting();
    app->renderDevice->setLight(0, lighting.directionalLight());
    app->renderDevice->setLight(1, GLight::directional(-Vector3::unitY(), Color3::brown() * .5, false));
    app->renderDevice->setAmbientLightColor(lighting.ambient);

    Draw::axes(app->renderDevice);

    PosedModelRef posed = model->pose(frame.toCoordinateFrame());
    posed->render(app->renderDevice);

    app->renderDevice->pushState();
        app->renderDevice->setColor(Color4(0,.5, 1,0.5));
        app->renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        posed = model->pose(current.toCoordinateFrame());
        posed->render(app->renderDevice);
    app->renderDevice->popState();

    app->renderDevice->push2D();
        Draw::rect2D(Rect2D::xyxy(10,210,20,220), app->renderDevice);
        Draw::rect2D(Rect2D::xyxy(30,210,40,220), app->renderDevice);
        Draw::rect2D(Rect2D::xyxy(41,210,50,220), app->renderDevice);
        Draw::rect2D(Rect2D::xyxy(50,210,60,220), app->renderDevice);
        Draw::rect2DBorder(Rect2D::xyxy(10,210,20,220), app->renderDevice, Color3::black());
    app->renderDevice->pop2D();

    debugAssert(false);
}



void App::main() {
	setDebugMode(true);
	debugController.setActive(false);
    Demo(this).run();
}

App::App(const GAppSettings& settings) : GApp(settings) {    
}


int main(int argc, char** argv) {
    GAppSettings settings;
    settings.window.depthBits = 24;
    settings.window.stencilBits = 8;
    settings.window.alphaBits = 0;
    settings.window.rgbBits = 8;
    settings.window.fsaaSamples = 0;
    settings.window.width = 400;
    settings.window.height = 400;
    App(settings).run();
    return 0;
}
