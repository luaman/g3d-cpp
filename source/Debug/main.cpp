#include "../include/G3DAll.h"
#include <string>


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    TextureRef          tex;
    SkyRef              sky;

    Demo(GApp* app) : GApplet(app) {
        tex = Texture::fromFile(app->dataDir + "image/lena.tga");
        sky = Sky::create(app->renderDevice, app->dataDir + "sky/");
    }

    virtual void doLogic();
    virtual void doGraphics();
};


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }
}


void Demo::doGraphics() {
    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));

    app->renderDevice->setViewport(Rect2D(100, 0, 400, 600));
    app->debugCamera->setProjectionAndCameraMatrix();

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));
    app->renderDevice->clear(true, true, true);

    sky->render(lighting);

    // Setup lighting
    app->renderDevice->enableLighting();
    glEnable(GL_LIGHT0);

    app->renderDevice->configureDirectionalLight
      (0, lighting.lightDirection, lighting.lightColor);

    app->renderDevice->setAmbientLightColor(lighting.ambient);

    Draw::axes(CoordinateFrame(Vector3(0,0,0)), app->renderDevice);
        
    glDisable(GL_LIGHT0);
    app->renderDevice->disableLighting();
    
    app->renderDevice->setTexture(0, tex);
    app->renderDevice->setCullFace(RenderDevice::CULL_NONE);
    app->renderDevice->setColor(Color3::WHITE);
    app->renderDevice->beginPrimitive(RenderDevice::QUADS);
        app->renderDevice->setTexCoord(0, Vector2(0, 0));
        app->renderDevice->sendVertex(Vector3(0, 1, 0));

        app->renderDevice->setTexCoord(0, Vector2(0, 1));
        app->renderDevice->sendVertex(Vector3(0, 0, 0));

        app->renderDevice->setTexCoord(0, Vector2(1, 1));
        app->renderDevice->sendVertex(Vector3(1, 0, 0));

        app->renderDevice->setTexCoord(0, Vector2(1, 0));
        app->renderDevice->sendVertex(Vector3(1, 1, 0));
    app->renderDevice->endPrimitive();

    app->debugPrintf("Use app->debugPrintf to print text");
    app->debugPrintf("to this overlay window.");

    app->debugPrintf("%g s", System::getTick());

}


int main(int argc, char** argv) {

    GAppSettings settings;
    settings.window.fsaaSamples = 1;
    settings.window.resizable = false;
    settings.window.width  = 800;
    settings.window.height = 600;

    GApp app(settings);

    char* choice[] = {"Ok"}; 
    prompt("Test", "Test", (const char**)choice, 1);

    app.setDebugMode(true);
    app.debugController->setActive(true);

    Demo applet(&app);

    applet.run();

    return 0;
}



