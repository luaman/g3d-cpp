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

#if G3D_VER < 60700
    #error Requires G3D 6.07
#endif


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

char tmpStr[2000000];

void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 0, 0));

/*    GImage maskInput(app->dataDir + "/smallfont.bmp");

    // write font mask
    TextOutput maskOutput(app->dataDir + "/font_mask.cpp");

    maskOutput.printf("unsigned char font_mask[256][2000] = {\n");

    for (int i = 0; i < 256; ++i) {
        Vector2 letterBounds = app->debugFont->get2DStringBounds(G3D::format("%c", (char)i), 12);

        maskOutput.printf("{");

        maskOutput.printf("%d,%d,", (int)letterBounds.x, (int)letterBounds.y);

        int startX = (i % 16) * app->debugFont->texelSize().x;
        int startY = (i / 16) * app->debugFont->texelSize().y;

        for (int j = 0; j < (int)app->debugFont->texelSize().y; ++j) {
            for (int k = 0; k < (int)app->debugFont->texelSize().x; ++k) {
                int pixelX = startX + k;
                int pixelY = startY + j;
                maskOutput.printf("%d,", maskInput.pixel3(pixelX, pixelY).r);
            }
        }
        maskOutput.printf("},\n");
    }

    maskOutput.printf("};\n");
    maskOutput.commit();
*/
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
        app->sky->render(app->renderDevice, lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);

//        Draw::box(AABox(Vector3(-1,-2,1), Vector3(2,2,3)), app->renderDevice);
        Draw::cylinder(Cylinder(Vector3::Vector3(-2,1,1), Vector3(3,4,0), 2), app->renderDevice);

        Plane p(Vector3::unitY(), Vector3::zero());
        Draw::plane(p, app->renderDevice);

    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(app->renderDevice, lighting);
    }
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    // Load objects here
    sky = Sky::create(NULL, dataDir + "sky/");
    
    applet->run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}


class Welder {
public:
    /** Indices of newVertexArray elements in <B>or near</B> a grid cell. */
    typedef Array<int> List;

    enum {GRID_RES = 32};

    List grid[GRID_RES][GRID_RES][GRID_RES];

    const Array<Vector3>&   oldVertexArray;
    Array<Vector3>&         newVertexArray;
    Array<int>&             toNew;
    Array<int>&             toOld;

    /** Must be less than one grid cell, not checked */
    const double            radius;

    /** (oldVertexArray[i] - offset) * scale is on the range [0, 1] */
    Vector3                 offset;
    Vector3                 scale;

    Welder(    
        const Array<Vector3>& _oldVertexArray,
        Array<Vector3>&       _newVertexArray,
        Array<int>&           _toNew,
        Array<int>&           _toOld,
        double                _radius);
        
    /**
     Computes the grid index from an ordinate.
     */
    void toGridCoords(Vector3 v, int& x, int& y, int& z) const;

    /** Gets the index of a vertex, adding it to 
        newVertexArray if necessary. */
    int getIndex(const Vector3& vertex);
    
    void weld();
};



G3D::uint32 hashCode(const Welder::List* x) {
    return (G3D::uint32)x;
}


Welder::Welder(    
    const Array<Vector3>& _oldVertexArray,
    Array<Vector3>&       _newVertexArray,
    Array<int>&           _toNew,
    Array<int>&           _toOld,
    double                _radius) :
    oldVertexArray(_oldVertexArray),
    newVertexArray(_newVertexArray),
    toNew(_toNew),
    toOld(_toOld),
    radius(_radius) {

    // Compute a scale factor that moves the range
    // of all ordinates to [0, 1]
    Vector3 minBound = Vector3::inf();
    Vector3 maxBound = -minBound;

    for (int i = 0; i < oldVertexArray.size(); ++i) {
        minBound.min(oldVertexArray[i]);
        maxBound.max(oldVertexArray[i]);
    }

    offset = minBound;
    scale  = maxBound - minBound;
    for (int i = 0; i < 3; ++i) {
        // The model might have zero extent along some axis
        if (fuzzyEq(scale[i], 0.0)) {
            scale[i] = 1.0;
        } else {
            scale[i] = 1.0 / scale[i];
        }
    }
}


void Welder::toGridCoords(Vector3 v, int& x, int& y, int& z) const {
    v = (v - offset) * scale;
    x = iClamp(iFloor(v.x * GRID_RES), 0, GRID_RES - 1);
    y = iClamp(iFloor(v.y * GRID_RES), 0, GRID_RES - 1);
    z = iClamp(iFloor(v.z * GRID_RES), 0, GRID_RES - 1);
}


int Welder::getIndex(const Vector3& vertex) {

    int closestIndex = -1;
    double distanceSquared = inf();

    int ix, iy, iz;
    toGridCoords(vertex, ix, iy, iz);

    // Check against all vertices within radius of this grid cube
    const List& list = grid[ix][iy][iz];

    for (int i = 0; i < list.size(); ++i) {
        double d = (newVertexArray[list[i]] - vertex).squaredLength();

        if (d < distanceSquared) {
            distanceSquared = d;
            closestIndex = list[i];
        }
    }

    if (distanceSquared <= radius * radius) {

        return closestIndex;

    } else {

        // This is a new vertex
        int newIndex = newVertexArray.size();
        newVertexArray.append(vertex);

        // Create a new vertex and store its index in the
        // neighboring grid cells (usually, only 1 neighbor)

        Set<List*> neighbors;

        for (int dx = -1; dx <= +1; ++dx) {
            for (int dy = -1; dy <= +1; ++dy) {
                for (int dz = -1; dz <= +1; ++dz) {
                    int ix, iy, iz;
                    toGridCoords(vertex + Vector3(dx, dy, dz) * radius, ix, iy, iz);
                    neighbors.insert(&(grid[ix][iy][iz]));
                }
            }
        }

        Set<List*>::Iterator neighbor(neighbors.begin());
        Set<List*>::Iterator none(neighbors.end());

        while (neighbor != none) {
            (*neighbor)->append(newIndex);
            ++neighbor;
        }

        return newIndex;
    }
}


void Welder::weld() {
    newVertexArray.resize(0);

    // Prime the vertex positions
    for (int i = 0; i < oldVertexArray.size(); ++i) {
        getIndex(oldVertexArray[i]);
    }

    // Now create the official remapping by snapping to 
    // nearby vertices.
    toNew.resize(oldVertexArray.size());
    toOld.resize(newVertexArray.size());

    for (int oi = 0; oi < oldVertexArray.size(); ++oi) {
        toNew[oi] = getIndex(oldVertexArray[oi]);
        toOld[toNew[oi]] = oi;
    }
}



int main(int argc, char** argv) {

//    GFont::convertRAWINItoPWF("data/smallfont", "data/font/smallfont.fnt"); 

    GAppSettings settings;
    settings.useNetwork = false;
    settings.window.fullScreen = false;
    settings.window.framed = true;
    settings.window.width = 800;
    settings.window.height = 600;
    settings.debugFontName = "console-small.fnt";
    App(settings).run();
    return 0;
}
