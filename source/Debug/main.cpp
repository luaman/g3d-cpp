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

#if G3D_VER < 60500
    #error Requires G3D 6.05
#endif

class Tri {
public:
    Vector3             vertex[3];

    Tri() {}
    
    inline Tri(const Vector3& v0, const Vector3& v1, const Vector3& v2) {
        vertex[0] = v0;
        vertex[1] = v1;
        vertex[2] = v2;
    }

    void serialize(TextOutput& to) const {
        for (int i = 0; i < 3; ++i) {
            vertex[i].serialize(to);
        }
        to.writeNewline();
    }

    void deserialize(TextInput& ti) {
        for (int i = 0; i < 3; ++i) {
            vertex[i].deserialize(ti);
        }
    }

    inline bool operator==(const Tri& B) const {
        for (int i = 0; i < 3; ++i) {
            if (vertex[i] != B.vertex[i]) {
                return false;
            }
        }

        return true;
    }
};


inline void getBounds(const Tri& tri, G3D::AABox& box) {
    Vector3 lo = tri.vertex[0];
    Vector3 hi = lo;

    for (int i = 1; i < 3; ++i) {
        lo = lo.min(tri.vertex[i]);
        hi = hi.max(tri.vertex[i]);
    }
    
    box.set(lo, hi);
}


inline unsigned int hashCode(const Tri& tri) {
    return hashCode(tri.vertex[1]);
}


Array<Tri> triArray;
AABSPTree<Tri> triSet;

double elevationUnder(double x, double z, Vector3& N) {

    double firstDistance = inf();

    double startElevation = 1e6;
    const Ray ray = Ray::fromOriginAndDirection(Vector3(x, startElevation, z), -Vector3::unitY());

    typedef AABSPTree<Tri>::RayIntersectionIterator IT;
    const IT end = triSet.endRayIntersection();

    for (IT tri = triSet.beginRayIntersection(ray);
        tri != end;
        ++tri) {

       Vector3 hitLocation, hitNormal;

       const Vector3& A = tri->vertex[0];
       const Vector3& B = tri->vertex[1];
       const Vector3& C = tri->vertex[2];

       double t = CollisionDetection::collisionTimeForMovingPointFixedTriangle(
            ray.origin, ray.direction,
            A, B, C, 
            hitLocation, hitNormal);

       static const double epsilon = 0.00001;
       if ((t < firstDistance) && 
           (t <= tri.maxDistance + epsilon) &&
           (t >= tri.minDistance - epsilon)) {

           // This is the new best collision time
           firstDistance = t;
           N = hitNormal;

           // Even if we found an object we must keep iterating until
           // we've exhausted all members at this node.
	       tri.markBreakNode();
       }
    }
     
    if (firstDistance == inf()) {
        N = Vector3::unitY();
        return 0;
    }

    double d = startElevation - firstDistance;

    debugAssert(d > -1000);
    debugAssert(d < 1e6);

    return d;
}



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

    Demo(App* app);

    virtual ~Demo() {}

    virtual void init();

    virtual void doLogic();

	virtual void doNetwork();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();

    virtual void cleanup();

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
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));

}


void Demo::cleanup() {
    // Called when Demo::run() exits
}


void Demo::doNetwork() {
	// Poll net messages here
}


void Demo::doSimulation(SimTime dt) {
	// Add physical simulation here
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

	// Add other key handling here
}


void Demo::doGraphics() {

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (app->sky.notNull()) {
        app->sky->render(lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);

        app->renderDevice->setPolygonOffset(1);
        app->renderDevice->setCullFace(RenderDevice::CULL_NONE);
        app->renderDevice->setColor(Color3::white());
        for (int j = 0; j < 2; ++j) {
            app->renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
            app->renderDevice->setNormal(Vector3::unitY());
            for (int i = 0; i < triArray.size(); ++i) {
                for (int v = 0; v < 3; ++v) {
                    app->renderDevice->sendVertex(triArray[i].vertex[v] / 1e3);
                }
            }
            app->renderDevice->endPrimitive();
            app->renderDevice->setColor(Color3::black());
            app->renderDevice->setRenderMode(RenderDevice::RENDER_WIREFRAME);
            app->renderDevice->setPolygonOffset(0);
        }
        app->renderDevice->setRenderMode(RenderDevice::RENDER_SOLID);

        for (int x = 0; x < 20; ++x) {
            for (int z = 0; z < 20; ++z) {
                double xx = x;// / 2.0;
                double zz = z;// / 2.0;
                Vector3 pos(xx, elevationUnder(xx*1e3, zz*1e3, Vector3()) / 1e3, zz);
                Draw::box(
                    AABox(pos - Vector3(.1,.1,.1), pos + Vector3(.1,.1,.1)),
                    app->renderDevice, Color3::red(), Color3::black());
            }
        }

    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(lighting);
    }
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    // Load objects here
    sky = Sky::create(renderDevice, dataDir + "sky/");
    
    applet->run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}




int main(int argc, char** argv) {            

    {
    TextInput ti("c:/tmp/tris.txt");
    while (ti.hasMore()) {
        triArray.next().deserialize(ti);
        triSet.insert(triArray.last());
    }

    }
//    triSet.insert(triArray);
    triSet.balance();


    double y = elevationUnder(9149, 10418, Vector3());


    GAppSettings settings;
    settings.window.width = 800;
    settings.window.height = 700;
    App(settings).run();
    return 0;
}
