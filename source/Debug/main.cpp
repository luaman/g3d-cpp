
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

#include "../contrib/Image/Image.h"

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

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {

}


void Demo::init()  {
	// Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 0, 10));
    app->debugCamera.lookAt(Vector3(0, 0, 0));
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

    if (app->userInput->keyPressed(' ')) {
        CoordinateFrame x = app->debugController.getCoordinateFrame();
        app->debugController.setCoordinateFrame(x);
    }
}


void Demo::doGraphics() {

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(true, true, true);

    debugAssert(glGetInteger(GL_ARRAY_BUFFER_BINDING_ARB) == 0);

    VARAreaRef area = VARArea::create(1024);

    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, area->gl_vertexBufferObject());
        debugAssert(glGetInteger(GL_ARRAY_BUFFER_BINDING_ARB) == area->gl_vertexBufferObject());
    glPopClientAttrib();

    debugAssert(glGetInteger(GL_ARRAY_BUFFER_BINDING_ARB) == 0);

//    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

}



void App::main() {
	setDebugMode(true);
	debugController.setActive(false);
    Demo(this).run();    
}

//App::App(const GAppSettings& settings) : GApp(settings, Win32Window::create(settings.window)) {
App::App(const GAppSettings& settings) : GApp(settings) {    
}


int main(int argc, char** argv) {
    GAppSettings settings;
    App(settings).run();
    return 0;
}

/*
int main(int argc, char** argv) {

    Image<double> im(8, 4);

    for (int y = 0; y < 4; ++y) {
        im(0, y) = 0.0;
        im(1, y) = 1.0;
        im(2, y) = 0.0;
        im(3, y) = 0.0;
        im(4, y) = 0.0;
        im(5, y) = 0.0;
        im(6, y) = 0.0;
        im(7, y) = 0.0;
    }
   
    RenderDevice rd;
    rd.init();

    while (true) {
        rd.beginFrame();
        rd.clear();
        rd.push2D();

        rd.beginPrimitive(RenderDevice::LINES);
        for (int i = 100; i < 600; ++i) {
            double t = i / 100.0;
            double f = im.bicubic(t, 1);

            rd.sendVertex(Vector2(t * 100, 300 - f * 100));
        }
        rd.endPrimitive();
        rd.pop2D();
        rd.endFrame();
    }
    return 0;

}
*/