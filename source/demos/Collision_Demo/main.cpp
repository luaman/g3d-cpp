/**
 @file Collision_Demo/main.cpp

  To run this program:
    1. Install OpenGL and SDL
    2. Change the DATA_DIR constant (if necessary)
    3. Compile and run
    4. Press ESC to exit

 <P>

 This demo uses the G3D collision detection routines to implement a simple physics
 system.  Shadow maps are rendered using the OpenGL SGIX_SHADOW extension to
 show how to use non-G3D calls with G3D.  A Matrix4 skeleton is provided as well.

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
 @created 2003-02-07
 @edited  2003-02-07
 */

#include <G3DAll.h>
#include "Model.h"
#include "Object.h"
#include "Scene.h"

/**
 The path to the data directory from this program's directory.
 */
const std::string DATA_DIR("d:/libraries/graphics3d-5.00b/data/");

/** The same bit depth is used for the shadow map and the screen */
int depthBits     = 24;

Log*                    debugLog		= NULL;
RenderDevice*           renderDevice	= NULL;
Font*                   font			= NULL;
UserInput*              userInput		= NULL;
VARArea*				varDynamic		= NULL;
VARArea*				varStatic		= NULL;
Camera*					camera			= NULL;
Scene*                  scene           = NULL;
ManualCameraController* controller      = NULL;

/** World time */
GameTime                gameTime        = 0;

bool                    endProgram		= false;


RealTime getTime() {
    return SDL_GetTicks() / 1000.0;
}

void buildScene();
void doSimulation(GameTime timeStep);
void doGraphics();
void doUserInput();


int main(int argc, char** argv) {

    // Initialize
    debugLog	 = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(800, 600, debugLog, 1.0, false,
                       1024 * 1024 * 2, true, 8, 0, depthBits, 0);
    camera 	     = new Camera(renderDevice);

    // Allocate the two VARAreas used in this demo
    varStatic  = renderDevice->createVARArea(1024 * 1024);
    debugAssert(varStatic);
    varDynamic = renderDevice->createVARArea(1024 * 1);
    debugAssert(varDynamic);

    font         = new Font(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    controller   = new ManualCameraController(renderDevice);
    controller->setMoveRate(1);

    controller->setPosition(Vector3(15, 20, 15));
    controller->lookAt(Vector3(-2,3,-5));
//    controller->setPosition(Vector3(-8, 5, 10));
//    controller->lookAt(Vector3(-8,5,-5));

    scene        = new Scene();

    buildScene();

    renderDevice->resetState();
	renderDevice->setColorClearValue(Color3(.1, .5, 1));

	gameTime	 = toSeconds(10, 00, 00, AM);
    
    RealTime now = getTime() - 0.001, lastTime;

    // Main loop
    do {
        lastTime = now;
        now = getTime();
        RealTime timeStep = now - lastTime;

        doUserInput();

        doSimulation(timeStep);

        doGraphics();
   
    } while (! endProgram);

    varStatic->reset();


    debugLog->printf("Static VAR peak size was  %d bytes.\n",
                     varStatic->peakAllocatedSize());
    debugLog->printf("Dynamic VAR peak size was %d bytes.\n", 
                     varDynamic->peakAllocatedSize());

    // Cleanup
    Model::freeModels();
    delete font;
    delete userInput;
    delete controller;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;
    delete scene;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////


void doSimulation(GameTime timeStep) {
    // Simulation
    controller->doSimulation(max(0.1, min(0, timeStep)), *userInput);
	camera->setCoordinateFrame(controller->getCoordinateFrame());

    scene->simulate(timeStep);
}


void doGraphics() {
    // Stats that will be updated periodically
    static RealTime lastFrameRateUpdate = getTime() - 0.9;
    static RealTime lastTime;
    static int frameRate = 0;
    static int numTris = 0;
    static int triRate = 0;
    RealTime now = getTime();

    // Compute the frame rate
    if (lastFrameRateUpdate + 1 < now) {
        frameRate = iRound(renderDevice->getFrameRate());
        numTris = iRound(renderDevice->getTrianglesPerFrame());
        triRate = iRound(renderDevice->getTriangleRate() / 1000);
        lastFrameRateUpdate = now;
    }

    renderDevice->beginFrame();
        renderDevice->pushState();
			    
		    camera->setProjectionAndCameraMatrix(renderDevice->getWidth(), 
                                                 renderDevice->getHeight());

            LightingParameters lighting(gameTime);
            lighting.lightDirection = (lighting.lightDirection + Vector3(0,0,.4)).direction();
            lighting.ambient = lighting.ambient + Color3(.3,.3,.4);

            scene->render(lighting);
      
                renderDevice->push2D();

                if (scene->sim.size() > 0) {
                        font->draw2DString(
                            format("%g", scene->sim[0]->velocity.length()),
                            200, 10, 28, Color3::WHITE, Color3::BLACK);
                }

                font->draw2DString(
                    format("%d fps", frameRate),
                    10, 10, 28, Color3::WHITE, Color3::BLACK);

		        font->draw2DString(
                    format("%d tris", numTris),
                    10, 72, 20, Color3::WHITE, Color3::BLACK);

                font->draw2DString(
                    format("%d ktri/s", triRate),
                    10, 100, 20, Color3::WHITE, Color3::BLACK);

            renderDevice->pop2D();
        renderDevice->popState();
	    
    renderDevice->endFrame();
    varDynamic->reset();
}

/**
  This scene tests whether the system is robust to energy increasing through
  a collision.   The single sphere starts slightly above the surface.  Ideally,
  it should appear to be resting.  It is hard to simulate this scene without
  the sphere bouncing higher and higher through successive iterations.
  */
void insertRestingContactSpheres() { 
    // This sphere must stay at rest
    scene->insertDynamic(new SimSphere(Sphere(Vector3(4, 1.01, 3), 1), Vector3(0, 0, 0), Color3::BLUE));

    // This sphere must become at rest on the surface
    scene->insertDynamic(new SimSphere(Sphere(Vector3(4, .49, 4.5), .5), Vector3(0, 0, 0), Color3::BLUE));

    // This sphere must come to rest (given non-unit restitution)
    scene->insertDynamic(new SimSphere(Sphere(Vector3(5.5, 2, 3), .5), Vector3(0, 0, 0), Color3::BLUE));
}


/**
 Constructs a tray for objects to sit on top of.
 */
void insertTray() {
    // Ground tray
    double wallHeight = 4;
    Color3 trayColor(Color3::GRAY);
    scene->insertStatic(new BoxObject(Box(Vector3(-14, -0.5, -10), Vector3(14, 0, 10)), trayColor));
    scene->insertStatic(new BoxObject(Box(Vector3(-14, 0, -10), Vector3(-13, wallHeight, 10)), trayColor));
    scene->insertStatic(new BoxObject(Box(Vector3(13, 0, -10), Vector3(14, wallHeight, 10)), trayColor));
    scene->insertStatic(new BoxObject(Box(Vector3(-13, 0, -10), Vector3(13, wallHeight, -9)), trayColor));
    scene->insertStatic(new BoxObject(Box(Vector3(-13, 0, 9), Vector3(13, wallHeight, 10)), trayColor));
}


/**
 The sphere beings touching the ground of the tray.  The challenge is to keep the ball on the
 surface of the tray, without using up all of the energy on micro-collisions or getting stuck
 in the ground.
 */
void insertRollingContactSpheres() {
    // Sphere on ground
    scene->insertDynamic(new SimSphere(Sphere(Vector3(-10, .25, -2), .25), Vector3(16, 0, 4), Color3::BLUE));

    // Sphere on ramp
    scene->insertDynamic(new SimSphere(Sphere(Vector3(-2.25, 4.75, 9.4), .25), Vector3(0, 0, 0), Color3::BLUE));
}


void insertSpiralSlide() {
    int i;
    for (i = 0; i < 41; ++i) {
        double angle = PI * i / 10.0;
        double angle2 = PI * (i - 0.6) / 10.0;

        // Outer spiral
        CoordinateFrame c;
        Box b(Vector3(-1, -1, -.1), Vector3(1, 1, .1));
        c.translation = Vector3(cos(angle) * 2.9, i / 3.5 + 1.5, sin(angle) * 2.9);
        c.lookAt(Vector3(cos(angle2) * 1.5, i / 3.5 + 2.2, sin(angle2) * 1.5));
        scene->insertStatic(new BoxObject(c.toWorldSpace(b), (Color3::YELLOW + Color3::WHITE) / 2));

        // Inner inner spiral
        {
            Box b(Vector3(-.3, -.3, -.1), Vector3(.25, .25, .1));
            c.translation = Vector3(cos(angle) * 1.2, i / 3.5 + 1, sin(angle) * 1.2);
            c.lookAt(Vector3(cos(angle2) * 3, i / 3.5 + 2, sin(angle2) * 3));
            scene->insertStatic(new BoxObject(c.toWorldSpace(b), (Color3::YELLOW + Color3::WHITE) / 2));
        }
    }

    scene->insertDynamic(new SimSphere(Sphere(Vector3(1.9, 13, -1), .75), Vector3(-2,-.5,-2), Color3::BLUE));
}


/**
 Two slanted green ramps.
 */
void insertRamps() {
    {
        Box b(Vector3(-1, 0, -5), Vector3(1, .25, 5.5));
        CoordinateFrame c;
        c.lookAt(Vector3(0, 1, 2));
        c.translation = Vector3(-2.5, 2.25, 5.5);
        scene->insertStatic(new BoxObject(c.toWorldSpace(b), (Color3::GREEN + Color3::WHITE) / 2));
    }

    // Corner ramp  
    {
        Box b(Vector3(-1, 0, -5), Vector3(1, .25, 5.5));
        CoordinateFrame c;
        c.lookAt(Vector3(-2, 2, -2));
        c.translation = Vector3(-11.2, 2.85, -7.2);
        scene->insertStatic(new BoxObject(c.toWorldSpace(b), (Color3::GREEN + Color3::WHITE) / 2));
    }     
}

void buildScene() {
    //    scene->insertStatic(new SphereObject(Sphere(Vector3(-8, 1, 4), 1), (Color3::RED + Color3::WHITE) / 2));
    
    scene->insertStatic(new CapsuleObject(Capsule(Vector3(-9, 1, 4), Vector3(-9, 4, 4), 1), (Color3::RED + Color3::WHITE) / 2));

    scene->insertStatic(new BoxObject(Box(Vector3(6, 0, 0), Vector3(7, 1, 8)), (Color3::GREEN + Color3::WHITE) / 2));
    scene->insertStatic(new GeneralObject(Model::getModel("cow.ifs"), CoordinateFrame(Vector3(-7,1.7,4)), Color3::YELLOW));
    scene->insertStatic(new GeneralObject(Model::getModel("trico.ifs"), CoordinateFrame(Vector3(10,1,2)), Color3::ORANGE));
    scene->insertStatic(new GeneralObject(Model::getModel("knot.ifs"), CoordinateFrame(Vector3(7,2.2,-4)), (Color3::BLUE + Color3::WHITE) / 2));

    insertTray();
    insertRamps();
    insertSpiralSlide();
    insertRollingContactSpheres();
    insertRestingContactSpheres();
   
    scene->insertDynamic(new SimSphere(Sphere(Vector3(-4, 3, 4), .25), Vector3(-1, 0, 0), Color3::BLUE));
    
    // Spheres
    
    /*int i;
    for (i = 0; i < 10; ++i) {
        scene->insertDynamic(new SimSphere(Sphere(Vector3(0, 7, 0), .25), Vector3::random() * 14, Color3::BLUE));
    }
    */
    
    /*
    {
        CoordinateFrame c;
        c.lookAt(Vector3(0,-1,0), Vector3::UNIT_X);
        c.translation = Vector3(5.25,4,0);
        scene->insertStatic(new GeneralObject(Model::getModel("triangle.ifs"), c, (Color3::BLUE + Color3::WHITE) / 2));
    }*/
}


//////////////////////////////////////////////////////////////////////////////

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

            }
            break;

            // Add other event handlers here
        }

        userInput->processEvent(event);
    }

    userInput->endEvents();
}



