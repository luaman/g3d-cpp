/**
  @file Mocap/main.cpp

  @author Morgan McGuire, matrix@graphics3d.com
 */

#include <G3DAll.h>
#if G3D_VER < 60700
    #error Requires G3D 6.07
#endif

#include "World.h"
#include "App.h"

#include "../../ArticulatedModel/all.cpp"
#include "AMUtil.h"


#ifdef G3D_WIN32
#   ifdef _NDEBUG
#       pragma comment(lib, "oded.lib")
#   else
#       pragma comment(lib, "ode.lib")
#   endif
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


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(2, 0, -2));
    app->debugCamera.lookAt(Vector3(0, -0.75, 0));
}


void Demo::cleanup() {
    // Called when Demo::run() exits
}


void Demo::doNetwork() {
	// Poll net messages here
}


void Demo::doSimulation(SimTime dt) {
	// Add physical simulation here
    app->world.doSimulation();
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

    if (app->userInput->keyPressed('r')) {
        app->world.renderMode = (World::RenderMode)((app->world.renderMode + 1) % World::RENDER_MAX);
    }
}


void Demo::doGraphics() {

    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);
    app->world.doGraphics(app->renderDevice);

    app->debugPrintf("r  - Cycle rendering modes\n");
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    loadScene();
    
    applet->run();
}


void App::loadScene() {
    world.init();


    world.sky = Sky::create(NULL, app->dataDir + "sky/");

    world.lighting = Lighting::create();
    {
        world.skyParameters = LightingParameters(G3D::toSeconds(10, 00, 00, AM));
    
        world.skyParameters.skyAmbient = Color3::white();

        if (world.sky.notNull()) {
            world.lighting->environmentMap = world.sky->getEnvironmentMap();
            world.lighting->environmentMapColor = world.skyParameters.skyAmbient;
        } else {
            world.lighting->environmentMapColor = Color3::black();
        }

        world.lighting->ambientTop = Color3(0.6, 0.6, 1.0) * world.skyParameters.diffuseAmbient;
        world.lighting->ambientBottom = Color3::white() * .6 * world.skyParameters.diffuseAmbient;

        world.lighting->emissiveScale = world.skyParameters.emissiveScale;

        world.lighting->lightArray.clear();

        world.lighting->shadowedLightArray.clear();

        GLight L = world.skyParameters.directionalLight();
        // Decrease the blue since we're adding blue ambient
        L.color *= Color3(1.2, 1.2, 1);
        L.position = Vector4(Vector3(0,1,1).direction(), 0);

        world.lighting->shadowedLightArray.append(L);
    }

    // Ground plane
    {
        EntityRef e = Entity::create(createPlaneModel("grid.png", 20, 1), CoordinateFrame(Vector3(-5, -1, 5)));
        e->physics.g3dGeometry = new PlaneShape(Plane(Vector3::unitY(), Vector3::zero()));
        world.insert(e);
    }

    if (false) {
        // Character
        world.insert(Entity::create(ASFModel::create("26.asf"), 
            CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitY(), toRadians(180)), Vector3::zero())));
    }

    {
        EntityRef e = Entity::create(createIFSModel("cube.ifs"), CoordinateFrame(Vector3(.7,3,0)));
        float s = 0.5;
        e->physics.g3dGeometry = new BoxShape(AABox(Vector3(-s,-s,-s), Vector3(s,s,s)));
        e->physics.velocity = Vector3::unitX();
        world.insert(e);
    }

    {
        EntityRef e = Entity::create(createIFSModel("cube.ifs", Color3::blue()), CoordinateFrame(Vector3(0,-0.5,0)));
        float s = 0.5;
        e->physics.g3dGeometry = new BoxShape(AABox(Vector3(-s,-s,-s), Vector3(s,s,s)));
        e->physics.canMove = false;
        world.insert(e);
    }

    if (false) {
        EntityRef e = Entity::create(createIFSModel("sphere.ifs", Color3::cyan()), CoordinateFrame(Vector3(-3,0,0)));
        e->physics.g3dGeometry = new SphereShape(Sphere(Vector3::zero(), 1));
        e->physics.velocity = Vector3(1,-2,-1);
        world.insert(e);
    }

}


App::App(const GAppSettings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}

App* app = NULL;

int main(int argc, char** argv) {
    
    GAppSettings settings;
    
    settings.useNetwork = false;
    settings.window.stencilBits = 8;
    settings.window.alphaBits = 0;
    
    app = new App(settings);
    app->run();
    delete app;
    app = NULL;

    return 0;
}
