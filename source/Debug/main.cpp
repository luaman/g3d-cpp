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

#if G3D_VER < 60300
    #error Requires G3D 6.03
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

    class App*          app;

	ShaderGroupRef		effect;

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

	virtual void doNetwork();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();

    virtual void cleanup();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));


    VertexShaderRef vs = VertexShader::fromFile("C:/tmp/nvcode/MEDIA/programs/glsl_simple_lighting/vertex_lighting.glsl");
    PixelShaderRef ps = NULL;//PixelShader::fromFile("C:/tmp/nvcode/MEDIA/programs/glsl_bump_mapping/bump_mapping_fragment.glsl");

    debugAssert(ShaderGroup::fullySupported());
    debugPrintf(vs->messages().c_str());
    debugAssert(vs->ok());
    
    if (! ps.isNull()) {
        debugAssert(ShaderGroup::fullySupported());
        debugPrintf(ps->messages().c_str());
        debugAssert(ps->ok());
    }
    
    effect = ShaderGroup::create(NULL, vs, ps);
    debugPrintf(effect->messages().c_str());
    debugAssert(effect->ok());

    // Print the argument names
    for (int a = 0; a < effect->numArgs(); ++a) {
        debugPrintf("%s : %s\n", 
            effect->arg(a).name.c_str(), 
            GLenumToString(effect->arg(a).type));
    }
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
    if (! app->sky.isNull()) {
        app->sky->render(lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);

		app->renderDevice->pushState();
			ShaderGroup::ArgList args;
			args.set("lightVec", Vector3(0,1,0));

			try {
				app->renderDevice->setShader(effect, args);
			} catch (const ShaderGroup::ArgumentError& e) {
				alwaysAssertM(false, e.message);
			}

			Draw::sphere(Sphere(Vector3::zero(), 1), app->renderDevice, Color3::white(), Color4::CLEAR);
		app->renderDevice->popState();

    app->renderDevice->disableLighting();

    if (! app->sky.isNull()) {
        app->sky->renderLensFlare(lighting);
    }
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    // Load objects here
    sky = Sky::create(renderDevice, dataDir + "sky/");
    
    Demo(this).run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
}


int main(int argc, char** argv) {
    GAppSettings settings;
    App(settings).run();
    return 0;
}
