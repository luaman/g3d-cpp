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

    TextureRef          base, alpha;

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

    /*
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

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    app->renderDevice->setColorClearValue(Color3::red());

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

    app->renderDevice->disableLighting();

    app->renderDevice->setColor(Color3::white());
    app->renderDevice->setTexture(0, app->im);
    Draw::rect2D(Rect2D::xywh(0, 0, 2, 2), app->renderDevice);



}


void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    // Load objects here
    sky = Sky::create(renderDevice, dataDir + "sky/");

    im = Texture::fromFile("../contrib/ArticulatedModel/demo/stone.jpg", TextureFormat::AUTO, Texture::TILE,
            Texture::TRILINEAR_MIPMAP, Texture::DIM_2D, 1.0, Texture::DEPTH_NORMAL, 8.0);
        
    applet->run();
}


App::App(const GAppSettings& settings) : GApp(settings) {//, new SDLWindow(settings.window)) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}

int main(int argc, char** argv) {
    uint32 x = 12345678;

    long y;
    int z;

    uint8 buf[1024];
    
uint8 out[8];



    if (swapBytes) {
        out[0] = buffer[pos + 7];
        out[1] = buffer[pos + 6];
        out[2] = buffer[pos + 5];
        out[3] = buffer[pos + 4];
        out[4] = buffer[pos + 3];
        out[5] = buffer[pos + 2];
        out[6] = buffer[pos + 1];
        out[7] = buffer[pos + 0];
    } else {
        out[0] = buffer[pos + 0];
        out[1] = buffer[pos + 1];
        out[2] = buffer[pos + 2];
        out[3] = buffer[pos + 3];
        out[4] = buffer[pos + 4];
        out[5] = buffer[pos + 5];
        out[6] = buffer[pos + 6];
        out[7] = buffer[pos + 7];
    }
    
    for (int i=0;i<4;++i){ 
        
    }

    /*
    BinaryOutput b("c:/tmp/bin.dat", G3D_LITTLE_ENDIAN);
    b.writeUInt8(200);
    b.writeInt8(-5);
    b.writeInt8(101);
    b.writeBool8(true);
    b.writeInt32(12345);
    b.writeInt32(-12345);
    b.writeUInt32(50000);
    b.writeUInt64(1234567);
    b.writeInt64(-1234567);
    b.writeFloat32(50000);
    b.writeFloat64(50000);
    b.commit();
    */

    GAppSettings settings;
    settings.window.width = 600;
    settings.window.height = 400;
    settings.window.alphaBits = 8;
    settings.useNetwork = false;
    App(settings).run();
    return 0;
}
