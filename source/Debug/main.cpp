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
      Entity(pos, Color3::WHITE, _texture), md2(_md2) {}

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
public:

    TextureRef          tex;
    SkyRef              sky;
    Array<Entity*>      entityArray;

    Demo(GApp* app) : GApplet(app) {
    }
    
    virtual void init();
    virtual void doLogic();
    virtual void doSimulation(SimTime dt);
    virtual void doGraphics();
    virtual void cleanup();
};


void Demo::init()  {
    tex = Texture::fromFile(app->dataDir + "image/lena.tga");
    //sky = Sky::create(app->renderDevice, app->dataDir + "sky/");
    sky = Sky::create(app->renderDevice, "c:/temp/testbox/","testcube_*.jpg",false);

    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));

    IFSModelRef cube   = IFSModel::create(app->dataDir + "ifs/cube.ifs");
    IFSModelRef teapot = IFSModel::create(app->dataDir + "ifs/teapot.ifs");

    MD2ModelRef knight = 
        MD2Model::create(app->dataDir + "quake2/players/pknight/tris.md2");

    TextureRef  knightTexture = 
        Texture::fromFile(app->dataDir + "quake2/players/pknight/knight.pcx", 
                          TextureFormat::AUTO, Texture::CLAMP, Texture::TRILINEAR_MIPMAP,
                          Texture::DIM_2D, 2.0); 

    entityArray.append(new IFSEntity(cube, Vector3(-5, 0, 0), Color3::BLUE));
    entityArray.append(new IFSEntity(teapot, Vector3( 0, 0, 0), Color3::YELLOW));
    entityArray.append(new MD2Entity(knight, Vector3( 5, 0, 0), knightTexture));
    entityArray[1]->selected = true;
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


    app->renderDevice->pushState();
    app->renderDevice->setTexture(0, sky->getEnvironmentMap());
    // Texture coordinates will be generated in object space.
    // Set the texture matrix to transform them into camera space.
    CoordinateFrame cframe;
    app->debugCamera.getCoordinateFrame(cframe);
    // The environment map assumes we are always in the center,
    // so zero the translation.
    cframe.translation = Vector3::ZERO;
    cframe.rotation.setRow(0, -cframe.rotation.getRow(0));

    app->renderDevice->setTextureMatrix(0, cframe);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
        glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_GEN_R);

        Draw::sphere(Sphere(Vector3(0,3,2), 2), app->renderDevice, Color3::WHITE);

    glPopAttrib();
    app->renderDevice->popState();



    Draw::axes(CoordinateFrame(Vector3(0, 7, 0)), app->renderDevice);
    
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

    sky->renderLensFlare(lighting);

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



