#include "../include/G3DAll.h"
#include <string>


class Entity {
public:
    bool selected;
    
    Entity() : selected(false) {}
    virtual ~Entity() {}

    virtual void render(RenderDevice*) = NULL;
};

class SphereEntity : public Entity {
private:
    Sphere sphere;
    Color4 color;
public:
    SphereEntity(const Vector3& position, double radius, const Color4& _color = Color3::BLUE);
    virtual void render(RenderDevice*);

};

SphereEntity::SphereEntity(const Vector3& position, double radius, const Color4& _color) {
    sphere = Sphere(position,radius);
    color = _color;
}

void SphereEntity::render(RenderDevice* device) {
    Draw::sphere(sphere, device, color, selected ? Color3::BLACK : Color4::CLEAR);
}


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    TextureRef          tex;
    SkyRef              sky;
    Array<Entity*>      entityArray;

    Demo(GApp* app) : GApplet(app) {
        tex = Texture::fromFile(app->dataDir + "image/lena.tga");
        sky = Sky::create(app->renderDevice, app->dataDir + "sky/");
    }
    
    virtual void init();
    virtual void doLogic();
    virtual void doGraphics();
    virtual void cleanup();
};


void Demo::init()  {
    app->debugCamera->setPosition(Vector3(0,4,10));
    app->debugCamera->lookAt(Vector3::ZERO);
    entityArray.append(new SphereEntity(Vector3(0,1,0),1,Color3::WHITE));
    entityArray.append(new SphereEntity(Vector3(-2.5,1,0),1,Color3::GREEN));
    entityArray.append(new SphereEntity(Vector3(2.5,1,0),1,Color3::YELLOW));
    entityArray[1]->selected = true;
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }
}


void Demo::doGraphics() {
    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));

    app->debugPrintf("Mouse X %g Y %g", app->userInput->getMouseX(), app->userInput->getMouseY());
    //app->renderDevice->setViewport(Rect2D(100, 0, 400, 600));
    app->debugCamera->setProjectionAndCameraMatrix();

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));
    app->renderDevice->clear(sky == NULL, true, true);

    sky->render(lighting);

    // Setup lighting
    app->renderDevice->enableLighting();
    glEnable(GL_LIGHT0);

    app->renderDevice->configureDirectionalLight
      (0, lighting.lightDirection, lighting.lightColor);

    app->renderDevice->setAmbientLightColor(lighting.ambient);

    for (int e = 0; e < entityArray.length(); ++e) { 
        entityArray[e]->render(app->renderDevice);
    }

    Draw::axes(CoordinateFrame(Vector3(0,0,0)), app->renderDevice);
     
    
    app->renderDevice->setTexture(0, tex);
    app->renderDevice->setCullFace(RenderDevice::CULL_NONE);
    app->renderDevice->setColor(Color3::WHITE);
    app->renderDevice->beginPrimitive(RenderDevice::QUADS);
        app->renderDevice->setNormal(Vector3::UNIT_Y);
        app->renderDevice->setTexCoord(0, Vector2(2, 0));
        app->renderDevice->sendVertex(Vector3(5, 0, -2.5));

        app->renderDevice->setTexCoord(0, Vector2(0, 0));
        app->renderDevice->sendVertex(Vector3(-5, 0, -2.5));

        app->renderDevice->setTexCoord(0, Vector2(0, 1));
        app->renderDevice->sendVertex(Vector3(-5, 0, 2.5));

        app->renderDevice->setTexCoord(0, Vector2(2, 1));
        app->renderDevice->sendVertex(Vector3(5, 0, 2.5));
    app->renderDevice->endPrimitive();

    glDisable(GL_LIGHT0);
    app->renderDevice->disableLighting();
    
    sky->renderLensFlare(lighting);
}

void Demo::cleanup() {
    entityArray.deleteAll();
}


int main(int argc, char** argv) {

    GAppSettings settings;
    settings.window.fsaaSamples = 1;
    settings.window.resizable = false;
    settings.window.width  = 800;
    settings.window.height = 600;

    GApp app(settings);

    app.setDebugMode(true);
//    app.debugController->setActive(true);

    Demo applet(&app);

    applet.run();

    return 0;
}



