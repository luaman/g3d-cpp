
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

    TextureRef                  texture;
    VertexAndPixelShaderRef     shader;

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {

    texture = Texture::fromFile("D:/games/data/image/testImage.jpg");

    std::string vs = 
        "void main(void) { \n"
        "    gl_Position = ftransform();\n"
        "    gl_Color = vec4(1,1,1,1);\n"
        "    vec3 lo = vec3(-1,-1,-1);\n"
        "    vec3 hi = vec3(1,1,1);\n"
        "    vec3 v = (gl_Vertex.xyz - lo) / (hi - lo);\n"
        "    const float PI = 3.1415927;\n"
        "    gl_TexCoord[0] = vec4(atan2(v.x - 0.5, v.y - 0.5) / (2*PI) + 0.5, v.z, 0, 1);\n"
         "}\n";

    std::string ps =
        "uniform sampler2D texture;\n"
        "uniform float xxy;\n"
        "void main(void) { \n"
        "  gl_FragColor = tex2D(texture, gl_TexCoord[0].st);\n"
        "}\n";

    shader = VertexAndPixelShader::fromStrings(vs, ps);
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
}


void Demo::doGraphics() {
    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(true, true, true);


    Vector3 control[5] = 
    {Vector3(0,0,0), 
     Vector3(0,1,0),
     Vector3(1,1,0),
     Vector3(1,2,0),
     Vector3(4,0,0)};

	Draw::axes(CoordinateFrame(Vector3(0, 0, 0)), app->renderDevice);


    app->renderDevice->pushState();
        VertexAndPixelShader::ArgList args;
        args.set("texture", texture);
        args.set("xxy", 3);
        app->renderDevice->setVertexAndPixelShader(shader, args);
//        app->renderDevice->setTexture(0, texture);


        Draw::sphere(Sphere(Vector3::zero(), 1), app->renderDevice,
            Color3::WHITE, Color4::CLEAR);
/*        Draw::box(AABox(Vector3(-1,-1,-1),Vector3(1,1,1)), app->renderDevice,
            Color3::WHITE, Color4::CLEAR);
            */
    app->renderDevice->popState();
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
