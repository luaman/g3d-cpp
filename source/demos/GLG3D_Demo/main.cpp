/**
 @file GLG3D_Demo/main.cpp

 This provides a compact infrastructure for building simple 3D apps like
 research demos and homework assignments.  It demonstrates the use of the
 GL_G3D classes RenderDevice, Log, Sky, UserInput, LightingParameters, and
 ManualCameraController to perform scene rendering and camera manipulation.
 It shows how to load the IFS models and render them with smooth shading.
 Per-primitive and array based rendering are demonstrated, as well as simple
 event handling to switch between them.
 
 <P>
 To run this demo, you must install SDL and copy SDL.DLL to the same directory
 as main.cpp.
 
 <P>
 Requires:
   OpenGL
   <A HREF="http://www.libsdl.org">SDL</A>
   G3D
   GLG3D

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-10-22
 @edited  2003-12-16
 */

#include <G3DAll.h>
  
/**
 Number of planes
 */
const int                   N = 50;

/**
 Change this to change the model being rendered.
 */
const std::string modelName = "p51-mustang.ifs";
//const std::string           modelName = "angel.ifs";

/**
 The path to the data directory from this program's directory.
 */
std::string                 DATA_DIR;

/**
 See also G3D::IFSModel and G3D::MD2Model.  This stripped-down
 demo uses neither, but you can save yourself a lot of coding
 by using the built-in classes (which are optimized for dynamic
 var usage).
 */
class Model {

    VAR             varVertex;
    VAR             varNormal;

    Array<uint32>   index;
    Array<Vector3>  vertex;
    Array<Vector3>  normal;

public:

    Model(const std::string& filename);
    virtual ~Model() {}
    void render(const CoordinateFrame& c, 
                const LightingParameters& lighting) const;
    int numPolys() const {
        return index.size() / 3;
    }
};


Log*                    debugLog    = NULL;
RenderDevice*           renderDevice= NULL;
CFontRef                font        = NULL;
UserInput*              userInput   = NULL;
SkyRef                  sky         = NULL;
VARAreaRef              varStream   = NULL;
VARAreaRef              varStatic   = NULL;
GCamera                 camera;

bool                    endProgram  = false;

enum RenderMethod {TRIANGLES = 0, VARSTREAM, VARSTATIC, NUM_RENDER_METHODS} renderMethod = VARSTATIC;

Model*                  model       = NULL;

void handleEvents();


/////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

    DATA_DIR = demoFindData();

    // Initialize
    debugLog     = new Log();
    
    renderDevice = new RenderDevice();
    RenderDeviceSettings settings;
    settings.rgbBits = 0;
    settings.alphaBits = 0;
    settings.stencilBits = 0;
    settings.depthBits = 16;

    //settings.fullScreen = true;
    renderDevice->init(settings, debugLog);

    // Allocate the two VARAreas used in this demo
    varStatic  = VARArea::create(1024 * 640 * 5, VARArea::WRITE_ONCE);
    varStream = VARArea::create(1024 * 1280 * 5, VARArea::WRITE_EVERY_FRAME);
    debugAssert(varStatic);
    debugAssert(varStream);

    font         = GFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");
    sky          = Sky::create(renderDevice, DATA_DIR + "sky/");
    userInput    = new UserInput();
    model        = new Model(DATA_DIR + "ifs/" + modelName);

    ManualCameraController* controller = new ManualCameraController();
    controller->init(renderDevice, userInput);

    controller->setPosition(Vector3(-25, 2, 0));
    controller->lookAt(Vector3(-20, 2.5, 1));
    controller->setActive(true);
    renderDevice->resetState();
    renderDevice->setColorClearValue(Color3(.1, .5, 1));
    GameTime gameTime   = G3D::toSeconds(8, 00, 00, AM);

    // Main loop
    do {
        // User input
        handleEvents();

		System::sleep(0.02);

        // Simulation
        gameTime = (int)(gameTime + MINUTE * .5) % (int)DAY;
        controller->doSimulation(.05);
        camera.setCoordinateFrame(controller->getCoordinateFrame());

        // Graphics
        renderDevice->beginFrame();
        renderDevice->clear(sky == NULL, true, false);
            renderDevice->pushState();
                
                renderDevice->setProjectionAndCameraMatrix(camera);
                LightingParameters lighting(gameTime, false);

                if (sky) {
                   sky->render(lighting);
                }
                                
                renderDevice->pushState();
                    // Setup lighting
                    renderDevice->setSpecularCoefficient(1);
                    renderDevice->enableLighting();
                    renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
                    renderDevice->setLight(1, GLight::directional(-lighting.lightDirection, Color3::white() * .25));
                    renderDevice->setAmbientLightColor(lighting.ambient);

                    renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

                    static const Color3 color[] = {Color3::black(), Color3::white(), Color3::orange(), Color3::blue()};
                    double  t[2];
                    Vector3 c[2];

                    t[0] = System::getTick();
                    t[1] = t[0] + 0.01;

					// Draw the planes
                    for (int x = 0; x < N; ++x) {

                        for (int i = 0; i < 2; ++i) {
                            double a = x + t[i] + 2; 
                            double a2 = t[i] * (x + 1) * .01 + 1005.1;
                            c[i] = Vector3(cos(a) * (10 + x / 2.0), sin(a2) * 10, sin(a) * 15);
                        }

                        CoordinateFrame cframe(c[0]);

                        double a = t[0] * (x + 1) * .1; 
                        cframe.lookAt(c[1], Vector3(cos(a), 3, sin(a)).direction());


                        renderDevice->setColor(color[x % 4]);
                        model->render(cframe, lighting);
                    }
                renderDevice->popState();


                if (sky) {
                    sky->renderLensFlare(lighting);
                }
                
                renderDevice->push2D();

                    font->draw2D(
                      format("%d fps", iRound(renderDevice->getFrameRate())),
                      Vector2(10, 10), 28, Color3::white(), Color3::black());
  
                    font->draw2D(
                      format("%d tris", iRound(renderDevice->getTrianglesPerFrame())),
                      Vector2(10, 72), 20, Color3::white(), Color3::black());

                    font->draw2D(
                      format("%d ktri/s", 
                      iRound(renderDevice->getTriangleRate() / 1000)),
                      Vector2(10, 100), 20, Color3::white(), Color3::black());

                    font->draw2D(
                      format("%dx%d poly planes", 
                      N, model->numPolys()),
                      Vector2(10, 150), 20, Color3::white(), Color3::black());

                    char* str = NULL;
                    switch (renderMethod) {
                    case TRIANGLES:
                        str = "Using begin/end (SPACE to change)";
                        break;

                    case VARSTREAM:
                        str = "Using streaming vertex array (SPACE to change)";
                        break;
 
                    case VARSTATIC:
                        str = "Using static vertex array (SPACE to change)";
                        break;

                    default:;
                    }
    
                   font->draw2D(str, Vector2(10, renderDevice->getHeight() - 40), 20, Color3::yellow(), Color3::black());

                renderDevice->pop2D();
            renderDevice->popState();
            debugAssertGLOk();    
        renderDevice->endFrame();
        varStream->reset();

    } while (! endProgram);

    varStatic->reset();


    debugLog->printf("Static VAR peak size was  %d bytes.\n",
                     varStatic->peakAllocatedSize());
    debugLog->printf("Streaming VAR peak size was %d bytes.\n", 
                     varStream->peakAllocatedSize());

    // Cleanup
    delete controller;
    delete userInput;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////

void handleEvents() {

    userInput->beginEvents();

    // Event handling
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
        endProgram = true;
        break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                endProgram = true;
                break;

            case SDLK_SPACE:
                renderMethod =
                  (RenderMethod)((renderMethod + 1) % NUM_RENDER_METHODS);
                break;

            // Add other key handlers here
            default:;

            }
            break;

        default:;
            // Add other event handlers here
        }

        userInput->processEvent(event);
    }

    userInput->endEvents();
}


//////////////////////////////////////////////////////////////////////////////


Model::Model(const std::string& filename) {
    // file := 
    //    fileheader +
    //    vertexheader +
    //    vertex* +
    //    triheader +
    //    tri*
    //
    //  fileheader   := (string32) "IFS" + (float32)1.0 + (string32)modelname
    //  vertexheader := (string32) "VERTICES" + (uint32)numVertices
    //  vertex       := (float32)x + (float32)y + (float32)z
    //  triheader    := (string32)"TRIANGLES" + (uint32)numFaces
    //  tri          := (uint32)v0 + (uint32)v1 + (uint32)v2

    debugLog->println(std::string("Loading ") + filename);

    BinaryInput b(filename, G3D_LITTLE_ENDIAN);

    std::string fmt         = b.readString32();
    float       version     = b.readFloat32();
    (void) version;
    std::string name        = b.readString32();

    debugAssert(fmt     == "IFS");
    debugAssert(version == 1.0);

    std::string vertexHeader = b.readString32();

    // Load the vertices
    vertex.resize(b.readUInt32());

    for (int v = 0; v < vertex.size(); ++v) {
        vertex[v] = b.readVector3() * 5;
    }

    // Per-vertex normals
    normal.resize(vertex.size());

    // Load the triangles
    std::string triHeader   = b.readString32();

    int numTris = b.readUInt32();

    for (int t = 0; t < numTris; ++t) {
        int v0 = b.readUInt32();
        int v1 = b.readUInt32();
        int v2 = b.readUInt32();

        // Compute the non-unit face normal
        Vector3 faceNormal = 
          (vertex[v1] - vertex[v0]).cross( 
           (vertex[v2] - vertex[v0]));

        normal[v0] += faceNormal;
        normal[v1] += faceNormal;
        normal[v2] += faceNormal;

        // Record the indices
        index.append(v0, v1, v2);
    }

    // Rescale the normals to unit length
    for (int n = 0; n < normal.size(); ++n) {
        normal[n] = normal[n].direction();
    }

    varVertex = VAR(vertex, varStatic);
    varNormal = VAR(normal, varStatic);
}


void Model::render(const CoordinateFrame& c,
                   const LightingParameters& lighting) const {

    renderDevice->setObjectToWorldMatrix(c);

    // Draw the model
    switch (renderMethod) {
    case TRIANGLES:
        renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
        {
            for (int i = 0; i < index.size(); ++i) {
                renderDevice->setNormal(normal[index[i]]);
                renderDevice->sendVertex(vertex[index[i]]);
            }
        }
        renderDevice->endPrimitive();
        break;


    case VARSTREAM:
        renderDevice->beginIndexedPrimitives();

        // This implementation would work fine for a number of *different*
        // models.  We happen to keep uploading the same model every time
        // because it is a test.

        {
            if (varStream->freeSize() < normal.size() * sizeof(Vector3) * 2) {
                // There isn't enough head room left, so reset.
                varStream->reset();
            }
            VAR n(normal, varStream);
            VAR v(vertex, varStream);

            renderDevice->setNormalArray(n);
            renderDevice->setVertexArray(v);
            renderDevice->sendIndices(RenderDevice::TRIANGLES, index);
        }
        renderDevice->endIndexedPrimitives();
        break;


    case VARSTATIC:
        renderDevice->beginIndexedPrimitives();
        {
            renderDevice->setNormalArray(varNormal);
            renderDevice->setVertexArray(varVertex);
            renderDevice->sendIndices(RenderDevice::TRIANGLES, index);
        }
        renderDevice->endIndexedPrimitives();
        break;

    default:;
    }

}

