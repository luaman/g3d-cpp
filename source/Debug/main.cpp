
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
#include "../contrib/Win32Window/Win32Window.h"
#include "../contrib/Win32Window/Win32Window.cpp"

#if G3D_VER < 60400
    #error Requires G3D 6.04
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
	
	TextureRef					brush;

    VertexAndPixelShaderRef shader;

    Demo(App* app);    

	void drawNailboard();

    virtual void init();

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}


void Demo::init()  {
   
	// Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));

    debugAssert(VertexAndPixelShader::fullySupported());

	brush = Texture::fromFile("D:/games/hardwarecontours/code/distrib/brush/toonfur.tga",
		TextureFormat::AUTO, Texture::CLAMP, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D_RECT);


    std::string ps =
        "uniform sampler2DRect T;  "
        "void main (void) {           "
        "   gl_FragColor    = texture2DRect(T, gl_TexCoord[0].st)); "
        "}";

    shader = VertexAndPixelShader::fromStrings("", ps);
}



void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }
}


void Demo::drawNailboard() {

	app->renderDevice->pushState();
		
		app->renderDevice->setObjectToWorldMatrix(CoordinateFrame());
		app->renderDevice->disableLighting();

//		app->renderDevice->setTexture(0, brush);

		VertexAndPixelShader::ArgList args;
		args.set("T", brush);

		app->renderDevice->setVertexAndPixelShader(shader, args);

		app->renderDevice->beginPrimitive(RenderDevice::QUADS);
			app->renderDevice->setTexCoord(0, Vector2(0, 0));
			app->renderDevice->sendVertex(Vector3(-1, 1, 0));

			app->renderDevice->setTexCoord(0, Vector2(0, 256));
			app->renderDevice->sendVertex(Vector3(-1, -1, 0));

			app->renderDevice->setTexCoord(0, Vector2(256, 256));
			app->renderDevice->sendVertex(Vector3( 1, -1, 0));

			app->renderDevice->setTexCoord(0, Vector2(256, 0));
			app->renderDevice->sendVertex(Vector3( 1, 1, 0));
		app->renderDevice->endPrimitive();
	app->renderDevice->pop2D();
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

		drawNailboard();

    app->renderDevice->disableLighting();

    if (! app->sky.isNull()) {
        app->sky->renderLensFlare(lighting);
    }
	
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    // Load objects here
    sky = Sky::create(renderDevice, dataDir + "sky/");
    
    Demo(this).run();
}


App::App(const GAppSettings& settings) : GApp(settings, new Win32Window(settings.window)) {
}


int main(int argc, char** argv) {
    GAppSettings settings;
    App(settings).run();
    return 0;
}
