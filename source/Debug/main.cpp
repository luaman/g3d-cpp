
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
#include "../contrib/Win32Window/Win32Window.h"
#include "../contrib/Win32Window/Win32Window.cpp"

#if G3D_VER < 60400
    #error Requires G3D 6.04
#endif

class App : public GApp {
protected:
    void main();
public:
    SkyRef              sky;

    App(const GAppSettings& settings);
};


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*					app;
	
    Array<Vector3>              array;
    AABSPTree<Vector3>          tree;

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
    const int NUM_POINTS = 100000;
    
    for (int i = 0; i < NUM_POINTS; ++i) {
        Vector3 pt = Vector3(random(-10, 10), random(-10, 10), random(-10, 10));
        array.append(pt);
        tree.insert(pt);
    }

    
    Vector3 pt(1,1,1);
    array.append(pt);
    tree.insert(pt);

    tree.balance();
}


void Demo::init()  {
   
	// Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 20, 0));
    app->debugCamera.lookAt(Vector3(0, 0, 0), -Vector3::UNIT_Z);
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
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(true, true, true);

	
	Draw::axes(CoordinateFrame(Vector3(0, 0, 0)), app->renderDevice);

    // Draw all points
    app->renderDevice->setPointSize(1);
    app->renderDevice->setColor(Color3::WHITE);
    app->renderDevice->beginPrimitive(RenderDevice::POINTS);
        for (int i = 0; i < array.size(); ++i) {
            app->renderDevice->sendVertex(array[i]);
        }
    app->renderDevice->endPrimitive();
    

    Array<Vector3> point;
//    tree.getIntersectingMembers(AABox(Vector3(0,-20,0), Vector3(5,20,5)), point);

    Array<Plane> plane;
    plane.append(Plane(Vector3(-1, 0, 0), Vector3(5, 0, 0)));
    plane.append(Plane(Vector3(1, 0, 0), Vector3(0, 0, 0)));
    plane.append(Plane(Vector3(0, 0, -1), Vector3(0, 0, 5)));
    plane.append(Plane(Vector3(0, 0, 1), Vector3(0, 0, 0)));
    tree.getIntersectingMembers(plane, point);

    // Draw points inside planes
    app->renderDevice->setPointSize(5);
    app->renderDevice->setColor(Color3::BLACK);
    app->renderDevice->beginPrimitive(RenderDevice::POINTS);
        for (int i = 0; i < point.size(); ++i) {
            app->renderDevice->sendVertex(point[i]);
        }
    app->renderDevice->endPrimitive();

}


void App::main() {
	setDebugMode(true);
	debugController.setActive(false);
    
    Demo(this).run();
}


App::App(const GAppSettings& settings) : GApp(settings, new Win32Window(settings.window)) {
}


int main(int argc, char** argv) {
    GAppSettings settings;
    App(settings).run();
    return 0;
}
