#include "../include/G3DAll.h"
#include <string>


class Entity {
public:
    bool selected;
    
    Entity() : selected(false) {}
    virtual ~Entity() {}

    virtual void render(RenderDevice*) = NULL;
    /**
      Returns amount of time to intersection starting on ray.origin and
      travels at ray.velocity.
    */
    virtual RealTime getIntersectionTime(const Ray&) = NULL;
};


////////////////////////////////////////////////////////////////

class IFSEntity : public Entity {
private:
    CoordinateFrame cframe;
    IFSModelRef     ifs;
    Color4          color;

public:
    IFSEntity(IFSModelRef _ifs, const Vector3& pos, const Color4& _color);

    virtual void render(RenderDevice*);
    virtual RealTime getIntersectionTime(const Ray&);

};

IFSEntity::IFSEntity(IFSModelRef _ifs, const Vector3& pos, const Color4& _color) : 
    ifs(_ifs), cframe(pos), color(_color) {
}

void IFSEntity::render(RenderDevice* renderDevice) {
    PosedModelRef pm = ifs->pose(cframe);

    renderDevice->pushState();
        renderDevice->setObjectToWorldMatrix(cframe);

        if (selected) {
            renderDevice->pushState();
                renderDevice->setColor(Color3::BLACK);
                renderDevice->setLineWidth(3);
                renderDevice->setRenderMode(RenderDevice::RENDER_WIREFRAME);
                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
                pm->render(renderDevice);
            renderDevice->popState();
        }

        renderDevice->setColor(color);
        pm->render(renderDevice);
    renderDevice->popState();
}

RealTime IFSEntity::getIntersectionTime(const Ray& ray) {

    Vector3 dummy;
    return CollisionDetection::collisionTimeForMovingPointFixedBox(ray.origin, 
        ray.direction, ifs->pose(cframe)->worldSpaceBoundingBox(), dummy);
}


///////////////////////////////////////////////////////////////
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
    app->debugCamera.setPosition(Vector3(0,4,10));
    app->debugCamera.lookAt(Vector3::ZERO);

    IFSModelRef cube   = IFSModel::create(app->dataDir + "ifs/cube.ifs");
    IFSModelRef teapot = IFSModel::create(app->dataDir + "ifs/teapot.ifs");

    entityArray.append(new IFSEntity(cube, Vector3(-5, 1, 0), Color3::BLUE));
    entityArray.append(new IFSEntity(teapot, Vector3( 0, 1, 0), Color3::YELLOW));
    entityArray.append(new IFSEntity(cube, Vector3( 5, 1, 0), Color3::WHITE));
    entityArray[1]->selected = true;
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

    if (app->userInput->keyPressed(SDL_LEFT_MOUSE_KEY)) {

        Ray ray = app->debugCamera.worldRay(app->userInput->getMouseX(), app->userInput->getMouseY(), app->renderDevice->getViewport());

        // Deselect all
        for (int i = 0; i < entityArray.length(); ++i) {
            entityArray[i]->selected = false;
        }

        // Find the *first* selected one
        int i = -1;
        RealTime t = infReal;
        for (int e = 0; e < entityArray.length(); ++e) {
            Entity* entity = entityArray[e];
            RealTime test = entity->getIntersectionTime(ray);
            if (test < t) {
                i = e;
                t = test;
            }
        }

        if (i >= 0) {
            entityArray[i]->selected = true;
        }
    }
}


void Demo::doGraphics() {
    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));

    app->debugPrintf("Mouse (%g, %g)", app->userInput->getMouseX(), app->userInput->getMouseY());

    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));
    app->renderDevice->clear(sky == NULL, true, true);

    sky->render(lighting);
    
    // Setup lighting
    app->renderDevice->enableLighting();

    app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
 
    app->renderDevice->setAmbientLightColor(lighting.ambient);

    for (int e = 0; e < entityArray.length(); ++e) { 
        entityArray[e]->render(app->renderDevice);
    }

    Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);
    
    app->renderDevice->setTexture(0, tex);
    app->renderDevice->setCullFace(RenderDevice::CULL_NONE);
    app->renderDevice->setColor(Color3::WHITE);
    app->renderDevice->beginPrimitive(RenderDevice::QUADS);
        app->renderDevice->setNormal(Vector3::UNIT_Y);
        app->renderDevice->setTexCoord(0, Vector2(2, 0));
        app->renderDevice->sendVertex(Vector3(7, 0, -3.5));

        app->renderDevice->setTexCoord(0, Vector2(0, 0));
        app->renderDevice->sendVertex(Vector3(-7, 0, -3.5));

        app->renderDevice->setTexCoord(0, Vector2(0, 1));
        app->renderDevice->sendVertex(Vector3(-7, 0, 3.5));

        app->renderDevice->setTexCoord(0, Vector2(2, 1));
        app->renderDevice->sendVertex(Vector3(7, 0, 3.5));
    app->renderDevice->endPrimitive();

    Vector4 p = app->renderDevice->project(Vector3::ZERO);
    app->debugPrintf("%g, %g", p.x, p.y);
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



