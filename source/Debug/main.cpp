#include "../include/G3DAll.h"
#include <string>

class Entity {
protected:

    Vector3                     basePos;
    //CoordinateFrame             cframe;
    PhysicsFrame                pframe;
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
    //cframe.translation = basePos - 
    //    Vector3::UNIT_Y * getPosedModel()->objectSpaceBoundingBox().getCorner(0).y;
    pframe.translation = basePos -
           Vector3::UNIT_Y * getPosedModel()->objectSpaceBoundingBox().getCorner(0).y;
    // Face the viewer
    //cframe.rotation = Matrix3::fromAxisAngle(Vector3::UNIT_Y, G3D_PI);
    Quat q1 = Quat::fromAxisAngleRotation(Vector3::UNIT_Y, G3D_PI / 2);
    Quat q2 = Quat::fromAxisAngleRotation(Vector3::UNIT_X, toRadians(45));
    //pframe.rotation = Quat::fromAxisAngleRotation(Vector3::UNIT_Y, G3D_PI);

    static double t = 0;
    t += dt * .1;
    if (t > 1) {
        t = 0;
    }

    pframe.rotation = q1.slerp(q2, t);

    //CoordinateFrame cframe = pframe.toCoordinateFrame();
}


void Entity::render(RenderDevice* renderDevice) {
    PosedModelRef pm = getPosedModel();
    pm->render(renderDevice);
//    pm->renderNormals(renderDevice);
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
        GMaterial mat;
        mat.color = Color3::RED;
        mat.specularCoefficient = 1.0;
        return ifs->pose(pframe.toCoordinateFrame(), mat);
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
        return md2->pose(pframe.toCoordinateFrame(), pose);
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

class App : public GApp {
public:

    App(const GAppSettings& settings) : GApp(settings) {
    }

    void main();
};


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

    App*                app;

public:

    SkyRef              sky;
    Array<Entity*>      entityArray;

    Demo(App* _app) : GApplet(_app), app(_app) {
        app->renderDevice->setSpecularCoefficient(0);
    }
    
    virtual void init();
    virtual void doLogic();
    virtual void doSimulation(SimTime dt);
    virtual void doGraphics();
    virtual void cleanup();
};


void Demo::init()  {
    sky = Sky::create(app->renderDevice, app->dataDir + "sky/");

    app->debugCamera.setPosition(Vector3(0, 0.5, 2));
    app->debugCamera.lookAt(Vector3(0, 0.5, 0));

    IFSModelRef cube   = IFSModel::create("../../../data/ifs/cow.ifs");

    entityArray.append(new IFSEntity(cube, Vector3(0, 0, 0), Color3::BLUE));
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

    if (app->userInput->keyPressed('k')) {
        Vector2 m;
        uint8 i;
        app->renderDevice->window()->getRelativeMouseState(m, i);
        app->renderDevice->window()->setRelativeMousePosition(m);
        debugPrintf("%s\n", m.toString().c_str());
    }

    debugAssert(! app->userInput->keyPressed(' '));

    //if (app->userInput->keyPressed(' ')) {
    //    _asm { int 3 };
    //}

	app->debugPrintf("(%g, %g)", app->userInput->getMouseX(), app->userInput->getMouseY());
}


void Demo::doGraphics() {
    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));

    app->renderDevice->clear(sky == NULL, true, true);
     
    // Render the scene to the full-screen
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    app->renderDevice->clear(sky == NULL, true, true);
    renderScene(lighting);


    // The lens flare shouldn't be reflected, so it is only rendered
    // for the final composite image
    if (sky != NULL) {
        sky->renderLensFlare(lighting);
    }

/*    app->debugFont->draw3D("Test",
        CoordinateFrame(Matrix3::fromAxisAngle(Vector3::UNIT_X, toRadians(0)),
        Vector3(0, 0, 1)), .1, Color3::BLACK, Color4::CLEAR, GFont::XALIGN_CENTER,
        GFont::YALIGN_CENTER);*/

    app->debugFont->draw3D("Test", CoordinateFrame(), .1, Color3::BLACK, Color4::CLEAR, GFont::XALIGN_CENTER,
        GFont::YALIGN_CENTER);

    Draw::axes(app->renderDevice);
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

}


void Demo::cleanup() {
    entityArray.deleteAll();
}


void App::main() {
    Demo applet(this);
    applet.run();
}


int main(int argc, char** argv) {

    GAppSettings settings;

	settings.window.resizable = true;
    settings.window.width = 640;
    settings.window.height = 480;

    App app(settings);

    app.setDebugMode(true);
    app.run();

    return 0;
}



