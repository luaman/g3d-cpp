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

#include "../contrib/Matrix/Matrix.h"
#include "../contrib/Matrix/Matrix.cpp"

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

    GameTime            gameTime;

    G3D::Array<G3D::Vector3> vertex; 
    G3D::Array<G3D::Vector3> normal; 
    G3D::Array<G3D::Vector2> tex; 
    G3D::Array<int> index; 

    G3D::Array<G3D::Vector3> binormal; 
    G3D::Array<G3D::Vector3> tangent; 

    RealTime            computeTime;

    TextureRef texture[2];

    VARAreaRef varArea;
    VAR vertexArray;
    VAR normalArray;

    bool modKeys[6];

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
    memset(modKeys, 0, sizeof(modKeys));
}

void Demo::onInit()  {

    Stopwatch watch;
    double min, max;
    double dmin, dmax;
    int imin, imax;

    int numLoops = 1000000;

    watch.tick();
    for (int i = 0; i < numLoops; ++i) {
        dmin = std::min(102.3423 * i, 3000.3 * i);
    }
    watch.tock();
    min = watch.elapsedTime();
    Log::common()->printf("std::min -- %f seconds.\n", (double)min, dmin);
    
    watch.tick();
    for (int i = 0; i < numLoops; ++i) {
        dmin = G3D::min(102.3423 * i, 3000.3 * i);
    }
    watch.tock();
    min = watch.elapsedTime();
    Log::common()->printf("G3D::min -- %f seconds.\n", (double)min, dmin);

    watch.tick();
    for (int i = 0; i < numLoops; ++i) {
        dmax = std::max(102.3423 * i, 3000.3 * i);
    }
    watch.tock();
    max = watch.elapsedTime();
    Log::common()->printf("std::max -- %f seconds.\n", (double)max, dmax);
    
    watch.tick();
    for (int i = 0; i < numLoops; ++i) {
        dmax = G3D::max(102.3423 * i, 3000.3 * i);
    }
    watch.tock();
    max = watch.elapsedTime();
    Log::common()->printf("G3D::max -- %f seconds.\n", (double)max, dmax);

    watch.tick();
    for (int i = 0; i < numLoops; ++i) {
        imin = std::min(102 * i, 3000 * i);
    }
    watch.tock();
    min = watch.elapsedTime();
    Log::common()->printf("std::min -- %f seconds.\n", (double)min, imin);
    
    watch.tick();
    for (int i = 0; i < numLoops; ++i) {
        imin = G3D::iMin(102 * i, 3000 * i);
    }
    watch.tock();
    min = watch.elapsedTime();
    Log::common()->printf("G3D::iMin -- %f seconds.\n", (double)min, imin);

    watch.tick();
    for (int i = 0; i < numLoops; ++i) {
        imax = std::max(102 * i, 3000 * i);
    }
    watch.tock();
    max = watch.elapsedTime();
    Log::common()->printf("std::max -- %f seconds.\n", (double)max, imax);
    
    watch.tick();
    for (int i = 0; i < numLoops; ++i) {
        imax = G3D::iMax(102 * i, 3000 * i);
    }
    watch.tock();
    max = watch.elapsedTime();
    Log::common()->printf("G3D::iMax -- %f seconds.\n", (double)max, imax);

    gameTime = G3D::toSeconds(11, 00, 00, AM);

    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 0, 0));
    GApplet::init();

    texture[0] = NULL;

    //GImage src("rggb.bmp");
    //GImage dst(src.width, src.height, 3);

    //src.convertToL8();
    //Stopwatch timer;

    //timer.tick();
    //GImage::BAYER_R8G8_G8R8_to_R8G8B8_MHC(src.width, src.height, src.byte(), dst.byte());
    //timer.tock();

    //computeTime = timer.elapsedTime();

    //texture[1] = Texture::fromGImage("bayer", dst, TextureFormat::RGB8, Texture::DIM_2D_NPOT, Texture::Parameters::video());

#if 0
    //Texture::fromFile("0.jpg");
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
    texture[1] = Texture::fromMemory("Red", b, (X == 3) ? TextureFormat::RGB8 : TextureFormat::RGBA8, 4, 4, TextureFormat::AUTO, Texture::TILE);//Texture::Parameters::defaults());
    //, TextureFormat::AUTO, Texture::TILE, Texture::NO_INTERPOLATION, Texture::DIM_2D);
    delete[] b;
    
    b = new uint8[4*4*3];
    glBindTexture(GL_TEXTURE_2D, texture[1]->getOpenGLID());
    glGetTexImage(GL_TEXTURE_2D,
			      0,
			      GL_RGB,
			      GL_UNSIGNED_BYTE,
			      b);

    Log::common()->printf("%d %d %d\n", b[0], b[1], b[2]);
/*    
    alwaysAssertM(b[0] == 0xFF, "Read back value with Red != 0xFF");
    alwaysAssertM(b[1] == 0x00, "Read back value with Green != 0x00");
    alwaysAssertM(b[2] == 0x00, "Read back value with Blue != 0x00");
  */  
    delete[] b;

   // alwaysAssertM(! GLCaps::hasBug_redBlueMipmapSwap(), "Red and blue are swapped.");
#endif

    setDesiredFrameRate(90);
     
    double s = 2; 
    vertex.append(G3D::Vector3(-s, 0, -s)); 
    vertex.append(G3D::Vector3( s, 0, -s)); 
    vertex.append(G3D::Vector3( s, 0, s)); 
    vertex.append(G3D::Vector3(-s, 0, s)); 
     
    normal.append(G3D::Vector3::UNIT_Y); 
    normal.append(G3D::Vector3::UNIT_Y); 
    normal.append(G3D::Vector3::UNIT_Y); 
    normal.append(G3D::Vector3::UNIT_Y); 
     
    tex.append(G3D::Vector2(0, 1)); 
    tex.append(G3D::Vector2(1, 1)); 
    tex.append(G3D::Vector2(1, 0)); 
    tex.append(G3D::Vector2(0, 0)); 
     
    index.append(0, 1, 2); 
    index.append(0, 2, 3); 
     
    G3D::Array<G3D::MeshAlg::Face> face; 
    G3D::MeshAlg::computeAdjacency(vertex, index, face, G3D::Array<G3D::MeshAlg::Edge>(), G3D::Array<G3D::MeshAlg::Vertex>()); 
    G3D::MeshAlg::computeTangentSpaceBasis(vertex, tex, normal, face, tangent, binormal); 

    varArea         = VARArea::create(1024 * 1024, VARArea::WRITE_ONCE);
    vertexArray     = VAR(vertex,   varArea);
    normalArray     = VAR(normal,   varArea);

    //Plane pl(Vector3(0, 1, 0), Vector3(20, 20, 20));
    //float distance = pl.distance(Vector3(0, 21, 0));
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

    //app->debugController.setCoordinateFrame(app->debugController.getCoordinateFrame());
}


void Demo::onUserInput(UserInput* ui) {
    if (ui->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

    if (ui->keyDown(SDLK_F10)) {
        endApplet = true;
    }

    if (ui->keyPressed(SDLK_p)) {
        GImage img;
        app->renderDevice->screenshotPic(img);
        img.save(app->dataDir + "pic.png");
    }

	// Add other key handling here

    memset(modKeys, 0, sizeof(modKeys));

    if (ui->keyDown(SDLK_LALT)) {
        modKeys[0] = true;
    }
    if (ui->keyDown(SDLK_RALT)) {
        modKeys[1] = true;
    }
    if (ui->keyDown(SDLK_LCTRL)) {
        modKeys[2] = true;
    }
    if (ui->keyDown(SDLK_RCTRL)) {
        modKeys[3] = true;
    }
    if (ui->keyDown(SDLK_LSHIFT)) {
        modKeys[4] = true;
    }
    if (ui->keyDown(SDLK_RSHIFT)) {
        modKeys[5] = true;
    }
}


void Demo::onGraphics(RenderDevice* rd) {

    app->renderDevice->clear(app->sky.isNull(), true, true);

    LightingParameters lighting(gameTime);
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);
    
    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    if (app->sky.notNull()) {
        app->sky->render(app->renderDevice, lighting);
    }

    //app->renderDevice->push2D();

    //    app->debugPrintf("Time: %fs", computeTime);

    //    app->renderDevice->setCameraToWorldMatrix(CoordinateFrame());

    //    Rect2D rect = texture[1]->rect2DBounds();
    //    app->renderDevice->setTexture(0, texture[1]);
    //    Draw::rect2D(rect/2, rd);

    //app->renderDevice->pop2D();

    // Setup lighting
    app->renderDevice->enableLighting();

    app->renderDevice->setAmbientLightColor(lighting.ambient);

	Draw::axes(app->renderDevice);

	rd->beginIndexedPrimitives();
        // The mapping from properties to texture coordinates is implicit in the bump shader
		rd->setVertexArray(vertexArray);
		rd->setNormalArray(normalArray);
		rd->sendIndices(RenderDevice::TRIANGLES, index);
	rd->endIndexedPrimitives();

    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(app->renderDevice, lighting);
    }

    app->renderDevice->push2D();
        app->debugFont->draw2D(modKeys[0] ? "DOWN": "UP", Vector2(0, 300));
        app->debugFont->draw2D(modKeys[1] ? "DOWN": "UP", Vector2(0, 315));
        app->debugFont->draw2D(modKeys[2] ? "DOWN": "UP", Vector2(0, 330));
        app->debugFont->draw2D(modKeys[3] ? "DOWN": "UP", Vector2(0, 345));
        app->debugFont->draw2D(modKeys[4] ? "DOWN": "UP", Vector2(0, 360));
        app->debugFont->draw2D(modKeys[5] ? "DOWN": "UP", Vector2(0, 375));
    app->renderDevice->pop2D();

    //Draw::fullScreenImage(GImage(app->dataDir + "image/checkerboard.jpg"), app->renderDevice);
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    // Load objects here
    sky = Sky::create(NULL, dataDir + "sky/");
    
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
