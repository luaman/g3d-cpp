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
        renderDevice->setTexture(0, texture);
        renderDevice->setColor(color);
        
        MeshAlg::Geometry geometry;
        const Array<int>& index = pm->triangleIndices();

        pm->getWorldSpaceGeometry(geometry);

        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        
        // Using beginIndexedPrimitives
        VARAreaRef area = VARArea::create(geometry.vertexArray.size() * sizeof(Vector3) * 2, VARArea::WRITE_EVERY_FRAME);
        VAR vertex(geometry.vertexArray, area);
        VAR normal(geometry.normalArray, area);
        renderDevice->beginIndexedPrimitives();
            debugAssertGLOk();
            renderDevice->setVertexArray(vertex);
            debugAssertGLOk();
            renderDevice->setNormalArray(normal);
            debugAssertGLOk();
            renderDevice->sendIndices(RenderDevice::TRIANGLES, index);
        renderDevice->endIndexedPrimitives();
       
        
        /*
        // GL_ARB_vertex_buffer_object extension
        glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

        GLuint vbo;
        glGenBuffersARB(1, &vbo);

        glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);

        // Allocate memory for both the vertex and normal pointers
        glBufferDataARB(GL_ARRAY_BUFFER_ARB, 
            geometry.vertexArray.size() * sizeof(Vector3) * 2,
            NULL,
            GL_STREAM_DRAW_ARB);

        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
            0,
            geometry.vertexArray.size() * sizeof(Vector3),
            geometry.vertexArray.getCArray());

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        debugAssertGLOk();
        
        void* N = (void*)(geometry.vertexArray.size() * sizeof(Vector3));
        glBufferSubDataARB(GL_ARRAY_BUFFER_ARB,
            (GLintptrARB)N,
            geometry.normalArray.size() * sizeof(Vector3),
            geometry.normalArray.getCArray());

        debugAssertGLOk();
        glEnableClientState(GL_NORMAL_ARRAY);
        debugAssertGLOk();
        glNormalPointer(GL_FLOAT, 0, N);
       
        debugAssertGLOk();

        glDrawRangeElements(GL_TRIANGLES, 
            0, geometry.vertexArray.size(),
            index.size(), GL_UNSIGNED_INT, 
            index.getCArray());

        glDeleteBuffersARB(1, &vbo);

        glPopClientAttrib();
        */



        /*
        // Using beginPrimitive
        renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
        for (int i = 0; i < index.size(); ++i) {
            renderDevice->setNormal(geometry.normalArray[index[i]]);
            renderDevice->sendVertex(geometry.vertexArray[index[i]]);
        }
        renderDevice->endPrimitive();
        */


        //pm->render(renderDevice);
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

class App : public GApp {
public:
    App(const GAppSettings& settings) : GApp(settings) {}
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
    sky = Sky::create(app->renderDevice, "d:/graphics3d/book/data/sky/","majestic512_*.jpg",false);

    app->debugCamera.setPosition(Vector3(0, 0.5, 2));
    app->debugCamera.lookAt(Vector3(0, 0.5, 0));

    IFSModelRef cube   = IFSModel::create(app->dataDir + "ifs/knot.ifs");

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

    app->renderDevice->push2D();
        app->debugFont->draw2D("1st draw, default colors", Vector2(100, 100), 30);
        app->debugFont->draw2D("2nd draw, default colors", Vector2(100, 140), 30);
    app->renderDevice->pop2D();
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

    App app(settings);

    app.setDebugMode(true);
    app.debugController.setActive(true);
    app.run();

    return 0;
}



