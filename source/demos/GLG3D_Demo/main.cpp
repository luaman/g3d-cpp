/**
 @file GLG3D_Demo/main.cpp

  To run this program:
    1. Install OpenGL and SDL
    2. Change the DATA_DIR constant (if necessary)
    3. Compile and run
    4. Use the mouse and W,A,S,D keys to move the camera about
    5. Press SPACE to toggle different rendering styles.
    6. Press ESC to exit

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
 @edited  2003-08-13
 */

#include <G3DAll.h>
  
/**
 The path to the data directory from this program's directory.
 */
std::string DATA_DIR("data/");


class Model {

    VAR			varVertex;
    VAR			varNormal;

    Array<uint32>	index;
    Array<Vector3>	vertex;
    Array<Vector3>	normal;

public:

    Model(const std::string& filename);
    virtual ~Model() {}
    void render(const CoordinateFrame& c, 
                const LightingParameters& lighting) const;
};


Log*                    debugLog	= NULL;
RenderDevice*           renderDevice= NULL;
CFontRef                font		= NULL;
UserInput*              userInput	= NULL;
Sky*                    sky		    = NULL;
VARArea*		        varDynamic	= NULL;
VARArea*		        varStatic	= NULL;
Camera*			        camera		= NULL;

bool                    endProgram	= false;

enum RenderMethod {TRIANGLES = 0, VARDYNAMIC, VARSTATIC, NUM_RENDER_METHODS} renderMethod = TRIANGLES;

Model*                  model		= NULL;

void handleEvents();

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

    DATA_DIR = demoFindData();

    // Initialize
    debugLog	 = new Log();
    
	renderDevice = new RenderDevice();
    renderDevice->init(640, 480, debugLog, 1.0, false,
                       1024 * 512, true, 8, 0, 16, 0);
    camera 	 = new Camera(renderDevice);

    // Allocate the two VARAreas used in this demo
    varStatic  = renderDevice->createVARArea(1024 * 64);
    varDynamic = renderDevice->createVARArea(1024 * 128);
    debugAssert(varStatic);
    debugAssert(varDynamic);

    font         = CFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");
    sky		     = new Sky(renderDevice, "Sky", DATA_DIR + "sky/");
    userInput    = new UserInput();
    model        = new Model(DATA_DIR + "ifs/p51-mustang.ifs");

    ManualCameraController controller(renderDevice);

    controller.setPosition(Vector3(0, 10, -25));
    controller.lookAt(Vector3::ZERO);

	renderDevice->resetState();
	renderDevice->setColorClearValue(Color3(.1, .5, 1));

    GameTime gameTime	= G3D::toSeconds(11, 00, 00, AM);

    // Main loop
    do {
        // User input
        handleEvents();

        // Simulation
//        gameTime = (int)(gameTime + MINUTE * 5) % (int)DAY;
        controller.doSimulation(.05, *userInput);
	    camera->setCoordinateFrame(controller.getCoordinateFrame());

        // Graphics
        renderDevice->beginFrame();
	    renderDevice->clear(sky == NULL, true, false);
            renderDevice->pushState();
				
                camera->setProjectionAndCameraMatrix();

                LightingParameters lighting(gameTime);

                if (sky) {
                   sky->render(camera->getCoordinateFrame(), lighting);
                }

                renderDevice->debugDrawAxes(3);
				
                renderDevice->pushState();
                    for (int x = 0; x < 3; ++x) {
                        CoordinateFrame c(Vector3((x - 1) * 6, 0, 0));
                        model->render(c, lighting);
                    }
                renderDevice->popState();

                if (sky) {
                    sky->renderLensFlare(camera->getCoordinateFrame(), lighting);
                }

                renderDevice->push2D();
                    font->draw2DString(
                      format("%d fps", iRound(renderDevice->getFrameRate())),
                      10, 10, 28, Color3::WHITE, Color3::BLACK);
  
                    font->draw2DString(
                      format("%d tris", iRound(renderDevice->getTrianglesPerFrame())),
                      10, 72, 20, Color3::WHITE, Color3::BLACK);

                    font->draw2DString(
                      format("%d ktri/s", 
                      iRound(renderDevice->getTriangleRate() / 1000)),
                      10, 100, 20, Color3::WHITE, Color3::BLACK);

                    char* str = NULL;
	            switch (renderMethod) {
                    case TRIANGLES:
                        str = "Using begin/end (SPACE to change)";
                        break;

                    case VARDYNAMIC:
                        str = "Using dynamic VAR (SPACE to toggle)";
                        break;
 
                    case VARSTATIC:
                        str = "Using static VAR (SPACE to toggle)";
                        break;

                    default:;
                    }
	
                   font->draw2DString(str, 10,
                      renderDevice->getHeight() - 40, 20, Color3::YELLOW, Color3::BLACK);

                renderDevice->pop2D();
            renderDevice->popState();
			
        renderDevice->endFrame();
        varDynamic->reset();

    } while (! endProgram);

    varStatic->reset();


    debugLog->printf("Static VAR peak size was  %d bytes.\n",
                     varStatic->peakAllocatedSize());
    debugLog->printf("Dynamic VAR peak size was %d bytes.\n", 
                     varDynamic->peakAllocatedSize());

    // Cleanup
    delete sky;
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

    renderDevice->pushState();

    // Setup lighting
    float black[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

    renderDevice->configureDirectionalLight
      (0, lighting.lightDirection, lighting.lightColor);

    renderDevice->configureDirectionalLight
      (1, -lighting.lightDirection, Color3::WHITE * .25);

    renderDevice->setAmbientLightLevel(lighting.ambient);
    
    renderDevice->setColor(Color3::WHITE);

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


    case VARDYNAMIC:
        renderDevice->beginIndexedPrimitives();
        {
            VAR n(normal, varDynamic);
            VAR v(vertex, varDynamic);

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

    // Turn off lighting
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHTING);

    renderDevice->popState();
}


