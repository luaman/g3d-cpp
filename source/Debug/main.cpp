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

		Draw::axes(CoordinateFrame(Vector3(0, 0, 0)), app->renderDevice);

//        Draw::box(AABox(Vector3(-1,-2,1), Vector3(2,2,3)), app->renderDevice);
        Draw::capsule(Capsule(Vector3::Vector3(0,0,0), Vector3(1,0,0), .5), app->renderDevice);

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
    sky = NULL;//Sky::create(NULL, dataDir + "sky/");
    
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
    Array<Vector3>          oldNormalArray;
    Array< Array<Vector2> > oldTexCoordArray;

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
        
    int                     mostCollisions;

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
    radius(_radius), mostCollisions(0) {

    // Compute a scale factor that moves the range
    // of all ordinates to [0, 1]
    Vector3 minBound = Vector3::inf();
    Vector3 maxBound = -minBound;

    for (int i = 0; i < oldVertexArray.size(); ++i) {
        minBound = minBound.min(oldVertexArray[i]);
        maxBound = maxBound.max(oldVertexArray[i]);
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

    mostCollisions = iMax(mostCollisions, list.length());

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
/*
    Array<Vector3> oldArray, newArray;
    Array<int> toNew, toOld;
    double weldRadius = 0.001;

    oldArray.resize(100);
    for (int i = 0; i < oldArray.size(); ++i) {
        oldArray[i] = Vector3::random() * 10;
    }

    Welder welder(oldArray, newArray, toNew, toOld, weldRadius);
    welder.weld();
*/
    

//    GFont::convertRAWINItoPWF("data/smallfont", "data/font/smallfont.fnt"); 

    GAppSettings settings;
    settings.useNetwork = false;
    settings.window.fullScreen = true;
    settings.window.framed = false;
    settings.window.width = 800;
    settings.window.height = 600;
    settings.window.fsaaSamples = 4;

    App(settings).run();
    return 0;
}

#if 0
    
/**
 Indexed surface mesh of triangles or quads, stored in a format 
 optimized for rendering tasks.  To render efficiently, back with
 a series of G3D::VAR arrays.  Those arrays are not built into
 this class because GMesh is independent of the rendering API.

 See also G3D::MeshAlg.
 */    
class GMesh {
public:

    enum PrimitiveType {
        TRIANGLES       = MeshAlg::TRIANGLES,	
        TRIANGLE_STRIP  = MeshAlg::TRIANGLE_STRIP,
        TRIANGLE_FAN    = MeshAlg::TRIANGLE_FAN,
        QUADS           = MeshAlg::QUADS, 	
        QUAD_STRIP      = MeshAlg::QUAD_STRIP
    };

    class Primitive {
    public:
        PrimitiveType                   type;
        Array<int32>                    indexArray;
    };

    /** Topology remains the same even as the geometry changes. */ 
    class Topology {
    public:

        /** All edges.  Some will be boundary edges */
        Array<MeshAlg::Edge>            edgeArray;
        Array<MeshAlg::Face>            faceArray;
        Array<MeshAlg::Vertex>          vertexArray;

        /** Indices into edgeArray of edges with exactly one adjacent face. */
        Array<int>                      boundaryEdges;

        /** Can be used to efficiently render un-textured, un-shaded geometry,
            e.g. for shadow map creation.  Indices are into the Geometry.vertexArray.*/
        Primitive                       primitive;
    };
    
    /** Parallel arrays describing per-vertex properties.  faceNormalArray
        has the same length as topology.faceArray.*/
    class Geometry {
    public:
        Array<Vector3>                  vertexArray;
        Array<Vector3>                  normalArray;
        Array<Vector3>                  tangentArray;
        Array< Array<Vector2> >         texCoordArray;

        Array<Vector3>                  faceNormalArray;
    };

    /**
     Conservative bounds on the Geometry.
     */
    class Bounds {
    public:
        Sphere                          sphere;
        AABox                           box;
        Capsule                         capsule;
        Cylinder                        cylinder;
    };

    Geometry                            geometry;

    /**
      Topology for the fully-welded mesh.  Vertices are <B>not</B>
      duplicated for shading or texturing purposes.
    */
    Topology                            topology;

    /** Index array used for rendering.  Contains duplicated vertices in order
        to have correct*/
    Primitive                           renderPrimitive;

    GMesh();

    virtual ~GMesh();

    /** Resizes isBackface to the length of topology.faceArray.  Determines for
        each face in faceArray whether it is a backface with respect to the observer,
        which is at infinity when <CODE>observer.w == 0.0</CODE>.
     */
    virtual void findBackfaces(const Vector4& observer, Array<bool>& isBackface);

    /** Re-sets to an empty mesh. */
    virtual void clear();

    /**
     Replace this GMesh with new values computed from the input data.
     Co-located vertices will be merged if they have the same texture
     coordinates and if their face's normals are suitably close.

     @param smoothAngle     Angles smaller than this receive separate normals.
     @param collapseRadius  Vertices closer than this are considered colocated.
     */
    virtual void buildMesh(
        const Array<Vector3>&           vertexArray,
        const Array< Array<Vector2> >&  texCoordArray,
        const Array<int32>&             indexArray,
        PrimitiveType                   primitiveType,
        float                           smoothAngle = toRadians(100),
        float                           collapseRadius = 0.001);

    /** Recomputes Bounds from the data in geometry. */
    virtual void updateBounds();

    /** Approximate size this data structure occupies on the CPU. */
    virtual size_t sizeInMemory() const;

    /** Number of triangles in the mesh. */
    int triangleCount() const;
};
#endif
