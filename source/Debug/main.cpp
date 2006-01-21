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

#include "../contrib/AVI/AVI.h"
#include "../contrib/AVI/AVI.cpp"
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
	// Create Texture
	static TextureRef tex = Texture::createEmpty(256, 256, "Rendered Texture", TextureFormat::RGB8, Texture::CLAMP, Texture::NO_INTERPOLATION, Texture::DIM_2D);

	// Create a framebuffer that uses this texture as the color buffer
	static FramebufferRef fb = Framebuffer::create("Offscreen target");
	bool init = false;

	if (! init) {
		fb->set(Framebuffer::COLOR_ATTACHMENT0, tex);
		init = true;
	}

	// Set framebuffer as the render target
	rd->push2D(fb);

		// Draw on the texture
		Draw::rect2D(Rect2D::xywh(0,0,128,256), rd, Color3::white());
		Draw::rect2D(Rect2D::xywh(128,0,128,256), rd, Color3::red());

		// Restore renderdevice state (old frame buffer)
	rd->pop2D();

	app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

	// Remove the texture from the framebuffer
	//	fb->set(Framebuffer::COLOR_ATTACHMENT0, NULL);

	// Can now render from the texture

    
    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1f, .5f, 1));
    app->renderDevice->clear();

    app->renderDevice->push2D();
		rd->setTexture(0, tex);
		Draw::rect2D(Rect2D::xywh(10,10,256,256), rd);
	app->renderDevice->pop2D();
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
/*
    GImage im("D:/games/data/image/testimage.tga");

    {
        AVIWriter avi("c:/tmp/test.avi", im.width, im.height, 100);
        alwaysAssertM(avi.ok(), avi.errorString());
        avi.writeFrame(im);
        avi.commit();
    }

    exit(0);
	*/
    GAppSettings settings;
    settings.useNetwork = false;
    settings.window.resizable = true;
    App(settings).run();
    return 0;
}
