/**

  @file demos/main.cpp


  This is a sample main.cpp to get you started with G3D.  It is
  designed to make writing an application easy.  You are not
  restricted to using this infrastructure-- choose the level of
  support that is best for your project (see the G3D Map in the
  documentation).

  @author Morgan McGuire, matrix@graphics3d.com
 */


#include <G3DAll.h>


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





