
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

GApp* app = NULL; 

#include "../contrib/ArticulatedModel/ArticulatedModel.cpp"


class App : public GApp {
protected:
    void main();
public:
    SkyRef              sky;
    ArticulatedModelRef model;
    TextureRef          texture;

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

    Demo(App* app);    

    virtual void init();

    virtual void doSimulation(RealTime dt);

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {

}


void Demo::init()  {
	// Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 0, 10));
    app->debugCamera.lookAt(Vector3(0, 0, 0));
}


void Demo::doSimulation(RealTime dt) {
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
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

    ArticulatedModel::Pose pose;
    static RealTime t0 = System::time();

    RealTime t = (System::time() - t0) * 10;
    pose.cframe.set("m_rotor", 
        CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitY(), t),
                        Vector3::zero()));
    pose.cframe.set("t_rotor",
        CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitX(), t*2),
                        Vector3::zero()));

    if (app->model.notNull()) {
        app->model->render(app->renderDevice, pose);
        app->renderDevice->pushState();
            app->renderDevice->setTexture(0, NULL);
            app->renderDevice->setColor(Color3::black());
            app->renderDevice->setRenderMode(RenderDevice::RENDER_WIREFRAME);
            app->model->render(app->renderDevice, pose);
        app->renderDevice->popState();
    }

    Draw::axes(app->renderDevice);
}



void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    model = ArticulatedModel::fromFile("c:/tmp/3ds/fs.3ds");
//    model = ArticulatedModel::fromFile("c:/tmp/car35/car35.3ds");

    Demo(this).run();
}

App::App(const GAppSettings& settings) : GApp(settings) {
    ::app = this;
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
