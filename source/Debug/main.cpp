
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


class SphereMap : public Shader {
private:

    VertexAndPixelShaderRef     vap;

    SphereMap() {}
public:

    TextureRef                  texture;
    AABox                       bounds;

    static ShaderRef create() {

        SphereMap* shader = new SphereMap();
        std::string vs = 
            "uniform vec3 hi;\n"
            "uniform vec3 lo;\n"
            "void main(void) { \n"
            "    gl_Position = ftransform();\n"
            "    gl_Color = vec4(1,1,1,1);\n"
            "    vec3 v = (gl_Vertex.xyz - lo) / (hi - lo);\n"
            "    const float PI = 3.1415927;\n"
            "    gl_TexCoord[0] = vec4(atan2(v.x - 0.5, v.y - 0.5) / (2*PI) + 0.5, v.z, 0, 1);\n"
             "}\n";

        std::string ps =
            "uniform sampler2D texture;\n"
            "void main(void) { \n"
            "  gl_FragColor = tex2D(texture, gl_TexCoord[0].st);\n"
            "}\n";

        shader->vap = VertexAndPixelShader::fromStrings(vs, ps);

        return shader;
    }


    void beforePrimitive(RenderDevice* rd) {
        rd->pushState();
        VertexAndPixelShader::ArgList args;
        args.set("texture", texture);
        args.set("hi", bounds.high());
        args.set("lo", bounds.low());
        rd->setVertexAndPixelShader(vap, args);
    }


    void afterPrimitive(RenderDevice* rd) {
        rd->popState();
    }

};

typedef ReferenceCountedPointer<SphereMap> SphereMapRef;

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

    SphereMapRef                shader;

    SimpleShaderRef             shader2;

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
    texture = Texture::fromFile("D:/games/data/image/testImage.jpg");
    shader = SphereMap::create();


    // Cylindrical projection about the z-axis
    std::string vs = 
//        "uniform vec3 hi;\n"
//        "uniform vec3 lo;\n"
        "void main(void) { \n"
        "    gl_Position = ftransform();\n"
        "    gl_Color = vec4(1,1,1,1);\n"
        "    // Scale vertex to range [-0.5, 0.5]\n"
//        "    const vec3 H = vec3(0.5,0.5,0.5);\n"
//        "    const vec3 L = vec3(-0.5,-0.5,-0.5);\n"
        "    const vec3 v = (gl_Vertex.xyz - 0.5) / (0.5 - -0.5) - 0.5;\n"
        "    const float PI2 = 2.0 * 3.1415927;\n"
        "    // Compute roll angle on [-PI, PI] and map to [0, 1]\n"
        "    gl_TexCoord[0] = vec4(atan2(v.x, v.y) / PI2, v.z, 0, 1) + vec4(0.5, 0.5, 0.0, 0.0);\n"
         "}\n";


    std::string ps =
        "uniform sampler2D texture;\n"
        "void main(void) { \n"
        "    gl_FragColor = tex2D(texture, gl_TexCoord[0].st);\n"
        "}\n";

    shader2 = SimpleShader::fromStrings(vs, ps);
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


    Sphere sphere(Vector3::zero(), 0.5);

    //shader->texture = texture;
    //sphere.getBounds(shader->bounds);
    //app->renderDevice->setShader(shader);

    AABox bounds;
    sphere.getBounds(bounds);
    shader2->args.set("texture", texture);
//    shader2->args.set("hi", bounds.high());
//    shader2->args.set("lo", bounds.low());
    app->renderDevice->setShader(shader2);

    Draw::sphere(sphere, app->renderDevice, Color3::WHITE, Color4::CLEAR);
/*        Draw::box(AABox(Vector3(-1,-1,-1),Vector3(1,1,1)), app->renderDevice,
            Color3::WHITE, Color4::CLEAR);
            */
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
