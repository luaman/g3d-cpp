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

    IFSModelRef model;

    ShaderRef   lambertian;  

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

  
    model = IFSModel::create(app->dataDir + "ifs/teapot.ifs");

//    lambertian = Shader::fromStrings(STR(

//     uniform vec3 k_A;

//     void main(void) {
//        gl_Position = ftransform();
//        gl_FrontColor.rgb = max(dot(gl_Normal, g3d_ObjectLight0.xyz), 0.0) * gl_LightSource[0].diffuse + k_A;
//     }), "");

    AABSPTree<Sphere> boxTree;

    boxTree.insert(Sphere(Vector3(0,0,0), 20));
    boxTree.insert(Sphere(Vector3(10,10,10), 50));

    for (AABSPTree<Sphere>::BoxIntersectionIterator it = boxTree.beginBoxIntersection(AABox(Vector3(0,0,0), Vector3(10,10,10)));
        it != boxTree.endBoxIntersection();
        ++it) {
        
        Log::common()->println("Intersection");
    }

    Array<Sphere> members;
    boxTree.getIntersectingMembers(AABox(Vector3(0,0,0), Vector3(10,10,10)), members);
    for(Array<Sphere>::Iterator it = members.begin(); it != members.end(); ++it) {

        Log::common()->println("Intersection 2");
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
    
    if (app->sky.notNull()) {
        app->sky->render(lighting);
    }

    app->renderDevice->setLight(0, GLight::directional(
            Vector3(1,0,0), Color3::white()));
    
    // Setup lighting
    app->renderDevice->enableLighting();
        app->renderDevice->setAmbientLightColor(Color3::black());
        app->renderDevice->setSpecularCoefficient(0);
       
  
    // Rendering loop
    app->renderDevice->setLight(0, GLight::directional(Vector3(1,1,1), Color3::white() - Color3(.2,.2,.3)));

//    app->renderDevice->setShader(lambertian);
//    lambertian->args.set("k_A", Color3(.2,.2,.3));
    model->pose()->render(app->renderDevice);
//        Draw::box(AABox(Vector3(-2,-2,-2), Vector3(2,2,2)), app->renderDevice, Color3::white() * 0.5, Color4::clear());

    app->renderDevice->disableLighting();

    /*
    if (app->sky.notNull()) {
        app->sky->renderLensFlare(lighting);
    }
    */

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

    Array<int> array(1);

    array.resize(1);

    for (int i = 0; i < 200; ++i) {
        array.resize(i);
    }

    GAppSettings settings;
    settings.window.width = 800;
    settings.window.height = 700;
    settings.window.alphaBits = 8;
    App(settings).run();
    return 0;
}
