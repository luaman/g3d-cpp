#include "../include/G3DAll.h"
#include <string>

class Entity {
protected:

    Vector3                     basePos;
    CoordinateFrame             cframe;
    Color4                      color;
    TextureRef                  texture;

public:

    bool selected;
    
    Entity(
        const Vector3&          pos,
        const Color4&           _color,
        const TextureRef        _texture);

    virtual ~Entity() {}

    virtual void doSimulation(SimTime dt);

    virtual PosedModelRef getPosedModel() const = 0;

    void render(RenderDevice*);

    /**
      Returns amount of time to intersection starting on ray.origin and
      travels at ray.velocity.
     */
    RealTime getIntersectionTime(const Ray&);
};


Entity::Entity(
    const Vector3&         pos,
    const Color4&          _color,
    const TextureRef       _texture) : 
    color(_color),
    texture(_texture),
    basePos(pos),
    selected(false) {}


void Entity::doSimulation(SimTime dt) {
    // Put the base on the ground
    cframe.translation = basePos - 
        Vector3::UNIT_Y * getPosedModel()->objectSpaceBoundingBox().getCorner(0).y;

    // Face the viewer
    cframe.rotation.fromAxisAngle(Vector3::UNIT_Y, G3D_PI);
}


void Entity::render(RenderDevice* renderDevice) {
    PosedModelRef pm = getPosedModel();

    renderDevice->pushState();
        if (selected) {
            renderDevice->pushState();
                renderDevice->setColor(Color3::BLACK);
                renderDevice->setLineWidth(3);
                renderDevice->setRenderMode(RenderDevice::RENDER_WIREFRAME);
                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
                pm->render(renderDevice);
            renderDevice->popState();
        }

        // Draw bounding box:
        // Draw::box(pm->worldSpaceBoundingBox(), renderDevice);

        renderDevice->setTexture(0, texture);
        renderDevice->setColor(color);
        pm->render(renderDevice);
    renderDevice->popState();
}


RealTime Entity::getIntersectionTime(const Ray& ray) {
    PosedModelRef pm = getPosedModel();
    Vector3 dummy;
    return CollisionDetection::collisionTimeForMovingPointFixedBox(ray.origin, 
        ray.direction, pm->worldSpaceBoundingBox(), dummy);
}

////////////////////////////////////////////////////////////////

class IFSEntity : public Entity {
private:

    IFSModelRef     ifs;

public:

    IFSEntity(IFSModelRef _ifs, const Vector3& pos, const Color4& _color): 
      Entity(pos, _color, NULL), ifs(_ifs) {}

    virtual PosedModelRef getPosedModel() const {
        return ifs->pose(cframe);
    }
};

////////////////////////////////////////////////////////////////

class MD2Entity : public Entity {
private:

    MD2ModelRef     md2;
    MD2Model::Pose  pose;

public:

    MD2Entity(MD2ModelRef _md2, const Vector3& pos, const TextureRef _texture): 
      Entity(pos, Color3::WHITE, _texture), md2(_md2) {
        pose.time = random(0, 20);
    }

    virtual PosedModelRef getPosedModel() const {
        return md2->pose(cframe, pose);
    }

    virtual void doSimulation(SimTime dt) {
        // Jump/wave periodically
        bool jump = random(0, 10.0 / dt) <= 0.5;
        bool wave = random(0, 10.0 / dt) <= 0.5;

        Entity::doSimulation(dt);

        pose.doSimulation(dt, false, false, false, false, jump, 
            false, false, false, wave, false, false, false, false, 
            false, false, false);
    }
};

///////////////////////////////////////////////////////////////

/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
private:

    /** Renders the scene, using the currently set camera and viewport. 
        Called first to render the environment map, then to render
        the visible frame. */
    void renderScene(const LightingParameters& lighting);

public:

    TextureRef          tex;
    SkyRef              sky;
    Array<Entity*>      entityArray;
    TextureRef          reflectionMap;

    Demo(GApp* app) : GApplet(app) {
        app->renderDevice->setSpecularCoefficient(0);
    }
    
    virtual void init();
    virtual void doLogic();
    virtual void doSimulation(SimTime dt);
    virtual void doGraphics();
    virtual void cleanup();
};


IFSModelRef teapot;
void Demo::init()  {
    tex = Texture::fromFile(app->dataDir + "image/lena.tga");
    //sky = Sky::create(app->renderDevice, app->dataDir + "sky/");
//    sky = Sky::create(app->renderDevice, "d:/graphics3d/book/data/sky/","testcube_*.jpg",false);
    sky = Sky::create(app->renderDevice, "d:/graphics3d/book/data/sky/","majestic512_*.jpg",false);
//    sky = NULL;

    reflectionMap = Texture::createEmpty(128, 128, "Reflection Map", TextureFormat::RGB8,
        Texture::CLAMP, Texture::TRILINEAR_MIPMAP, Texture::DIM_CUBE_MAP);

    debugAssert(reflectionMap->getTexelHeight() <= app->renderDevice->getHeight());

    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));

    IFSModelRef cube   = IFSModel::create(app->dataDir + "ifs/cube.ifs");
    teapot = IFSModel::create(app->dataDir + "ifs/teapot.ifs");

    MD2ModelRef knight = 
        MD2Model::create(app->dataDir + "quake2/players/pknight/tris.md2");

    TextureRef  knightTexture = 
        Texture::fromFile(app->dataDir + "quake2/players/pknight/knight.pcx", 
                          TextureFormat::AUTO, Texture::CLAMP, Texture::TRILINEAR_MIPMAP,
                          Texture::DIM_2D, 2.0); 

//    entityArray.append(new IFSEntity(cube, Vector3(-5, 0, 0), Color3::BLUE));
//    entityArray.append(new IFSEntity(teapot, Vector3( 0, 0, 0), Color3::WHITE));
    entityArray.append(new MD2Entity(knight, Vector3( 5, 0, 0), knightTexture));
    entityArray.append(new MD2Entity(knight, Vector3( 0, 0, 0), knightTexture));
    entityArray.append(new MD2Entity(knight, Vector3( -5, 0, 0), knightTexture));
}


void Demo::doSimulation(SimTime dt) {
    for (int e = 0; e < entityArray.length(); ++e) { 
        entityArray[e]->doSimulation(dt);
    }
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

    // Generate dynamic environment map
    Rect2D rect   = Rect2D::xywh(0, 0, reflectionMap->getTexelWidth(), reflectionMap->getTexelHeight());

    GCamera camera;
    camera.setFieldOfView(toRadians(90));
    camera.setNearPlaneZ(-.01);
    camera.setFarPlaneZ(-inf);
    CoordinateFrame cframe(Vector3(2, 4, 0));

    app->renderDevice->clear(sky == NULL, true, true);
for (int i = 0; i < 4; ++i) {

    for (int f = 0; f < 6; ++f) {
        app->renderDevice->pushState();
            app->renderDevice->setViewport(rect);
            Texture::CubeFace face = (Texture::CubeFace)f;
        
            Texture::getCameraRotation(face, cframe.rotation);
            camera.setCoordinateFrame(cframe);

            app->renderDevice->setProjectionAndCameraMatrix(camera);
            renderScene(lighting);
        app->renderDevice->popState();
        reflectionMap->copyFromScreen(rect, face);

        // Shift over and use a different part of the screen
        // so we don't have to repeatedly clear the screen.
        rect = rect + Vector2(rect.width(), 0);
        if (rect.x1() > app->renderDevice->getWidth()) {
            // Go to next row
            rect = rect + Vector2(-rect.x0(), rect.height());

            if (rect.y1() > app->renderDevice->getHeight()) {
                // Move back to the beginning and clear the screen.
                rect = rect - Vector2(rect.x0(), rect.y0());
                app->renderDevice->clear(sky == NULL, true, true);
            }
        }
    }
}
    // Render the scene to the full-screen
    app->debugPrintf("Mouse (%g, %g)", app->userInput->getMouseX(), app->userInput->getMouseY());
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    app->renderDevice->clear(sky == NULL, true, true);
    renderScene(lighting);

    
    app->renderDevice->pushState();
        app->renderDevice->enableLighting();

        app->renderDevice->setColor(Color3::WHITE);
        app->renderDevice->configureReflectionMap(0, reflectionMap);
        CoordinateFrame boxframe(Vector3(2, 4, 0));
        boxframe.rotation.fromAxisAngle(Vector3::UNIT_Y, toRadians(90));

        app->renderDevice->setObjectToWorldMatrix(boxframe);

        // Draw::box(Box(Vector3(-1,-1,-1), Vector3(1,1,1)), app->renderDevice, Color3::WHITE, Color4::CLEAR);
        Draw::sphere(Sphere(Vector3(0, 0, 0), 1.3), app->renderDevice, Color3::WHITE, Color4::CLEAR);

     //   teapot->pose(CoordinateFrame(Vector3(0, 4, 0)))->render(app->renderDevice);
    app->renderDevice->popState();
    

    // The lens flare shouldn't be reflected, so it is only rendered
    // for the final composite image
    if (sky != NULL) {
        sky->renderLensFlare(lighting);
    }
}


void Demo::renderScene(const LightingParameters& lighting) {

    if (sky != NULL) {
        sky->render(lighting);
    }
    
    // Setup lighting
    app->renderDevice->enableLighting();

    app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
 
    app->renderDevice->setAmbientLightColor(lighting.ambient);

    for (int e = 0; e < entityArray.length(); ++e) { 
        entityArray[e]->render(app->renderDevice);
    }

    //Draw::axes(CoordinateFrame(Vector3(0, 7, 0)), app->renderDevice);
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

    app->renderDevice->disableLighting();
}


void Demo::cleanup() {
    entityArray.deleteAll();
}


int main(int argc, char** argv) {

    GAppSettings settings;

    GApp app(settings);

    app.setDebugMode(true);
    app.debugController.setActive(true);

    Demo applet(&app);

    applet.run();

    return 0;
}



