#include "../include/G3DAll.h"
#include <string>


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    TextureRef          tex;

    Demo(GApp* app) : GApplet(app) {
        tex = Texture::fromFile(app->dataDir + "image/lena.tga");
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

    // Cyan background
	glClearColor(0.1f, 0.5f, 1.0f, 0.0f);

    app->renderDevice->clear(true, true, true);

    app->debugCamera->setProjectionAndCameraMatrix();

    app->debugPrintf("Use app->debugPrintf to print text");
    app->debugPrintf("to this overlay window.");

    // Setup lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    app->renderDevice->configureDirectionalLight
      (0, lighting.lightDirection, lighting.lightColor);

    app->renderDevice->setAmbientLightLevel(lighting.ambient);

    Draw::axes(CoordinateFrame(Vector3(0,0,0)), app->renderDevice);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    
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
}


int main(int argc, char** argv) {

    GAppSettings settings;
    settings.window.fsaaSamples = 4;
    settings.window.resizable = true;
    settings.window.width  = 640;
    settings.window.height = 480;
 
    GApp app(settings);

    app.setDebugMode(true);
    app.debugController->setActive(true);

    Demo applet(&app);

    applet.run();

    return 0;
}



