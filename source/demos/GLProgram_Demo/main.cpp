/**
  @file GLProgram_Demo/main.cpp

  This demonstrates how to use the programmable graphics
  pipeline (aka "Vertex shaders and pixel shaders") under
  G3D/OpenGL.

  The OpenGL ARB Vertex Program specification is located at
  http://oss.sgi.com/projects/ogl-sample/registry/ARB/vertex_program.txt

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-10
  @edited  2003-12-07
 */ 

#include <G3DAll.h>
#ifdef _MSC_VER
  #include <direct.h>
#endif

class Model {

    VAR         varVertex;
    VAR         varNormal;

    Array<uint32>   index;
    Array<Vector3>  vertex;
    Array<Vector3>  normal;

public:

    Model(const std::string& filename);
    virtual ~Model() {}
    void render(const CoordinateFrame& c, 
                const LightingParameters& lighting) const;
};


std::string             DATA_DIR;

Log*                    debugLog        = NULL;
RenderDevice*           renderDevice    = NULL;
CFontRef                font            = NULL;
UserInput*              userInput       = NULL;
GCamera                 camera;
ManualCameraController* controller      = NULL;
bool                    endProgram      = false;
Model*                  model           = NULL;
VertexProgramRef        distort         = NULL;


void doSimulation(GameTime timeStep);
void doGraphics();
void doUserInput();


int main(int argc, char** argv) {
    DATA_DIR = demoFindData();
 
    // Initialize
    debugLog     = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(RenderDeviceSettings(), debugLog);

    if (!renderDevice->supportsVertexProgram()) {
        error ("Critical Error", "This demo requires a graphics card and driver with OpenGL Vertex programs.", true);
        exit(-1);
    }

    font         = GFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    model        = new Model(DATA_DIR + "ifs/cow.ifs");

    controller   = new ManualCameraController(renderDevice, userInput);
    controller->setMoveRate(1);

    controller->setPosition(Vector3(2, .2, -2));
    controller->lookAt(Vector3(-2,0,2));
    controller->setActive(true);

    renderDevice->resetState();
    renderDevice->setColorClearValue(Color3(.1, .5, 1));
    renderDevice->setCaption("G3D Vertex Program Demo");

    RealTime now = System::getTick() - 0.001, lastTime;

    {
        std::string p = "GLProgram_Demo/";
        if (! fileExists(p + "twist.vp")) {
            p = "../" + p;
        }
        distort = VertexProgram::fromFile(p + "twist.vp");
    }

    // Main loop
    do {
        lastTime = now;
        now = System::getTick();
        RealTime timeStep = now - lastTime;

        double angle = cos(now) / 2;
        glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, cos(angle), 1, sin(angle), 1);
        doUserInput();

        doSimulation(timeStep);

        doGraphics();
   
    } while (! endProgram);

    // Cleanup
    delete controller;
    delete userInput;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////


void doSimulation(GameTime timeStep) {
    // Simulation
    controller->doSimulation(max(0.1, min(0, timeStep)));
    camera.setCoordinateFrame(controller->getCoordinateFrame());
}


void doGraphics() {
    renderDevice->setAmbientLightColor(Color3::white() * .5);
    renderDevice->beginFrame();
        renderDevice->clear(true, true, true);
        renderDevice->pushState();
                
            renderDevice->setProjectionAndCameraMatrix(camera);

            renderDevice->pushState();
                renderDevice->setVertexProgram(distort);

                // Set depth buffer
                renderDevice->disableColorWrite();
                model->render(CoordinateFrame(), LightingParameters(G3D::toSeconds(11,00,00,AM)));
                renderDevice->enableColorWrite();
            
                // Draw translucent
                renderDevice->setDepthTest(RenderDevice::DEPTH_ALWAYS_PASS);
                renderDevice->disableDepthWrite();
                renderDevice->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);
                model->render(CoordinateFrame(), LightingParameters(G3D::toSeconds(11,00,00,AM)));
 
                // Wireframe
                renderDevice->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ZERO);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                renderDevice->setPolygonOffset(-.25);
                renderDevice->setColor(Color3::black());
                renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
                model->render(CoordinateFrame(), LightingParameters(G3D::toSeconds(11,00,00,AM)));
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            renderDevice->popState();
            
            Draw::axes(renderDevice);

        renderDevice->popState();
        
    renderDevice->endFrame();
}


void doUserInput() {

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

            // Add other key handlers here
            default:;
            }
            break;

            // Add other event handlers here
        default:;
        }

        userInput->processEvent(event);
    }

    userInput->endEvents();
}



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
    (void)version;
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
}


void Model::render(const CoordinateFrame& c,
                   const LightingParameters& lighting) const {


    renderDevice->setObjectToWorldMatrix(c);

    renderDevice->pushState();

    // Setup lighting
    renderDevice->enableLighting();
    renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
    renderDevice->setLight(1, GLight::directional(-lighting.lightDirection, Color3::white() * .25));

    renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
    renderDevice->setAmbientLightColor(lighting.ambient);
    renderDevice->setColor(Color3::white());

    // Draw the model

    renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
    {
        for (int i = 0; i < index.size(); ++i) {
            renderDevice->setNormal(normal[index[i]]);
            renderDevice->sendVertex(vertex[index[i]]);
        }
    }
    renderDevice->endPrimitive();

    renderDevice->popState();
}
