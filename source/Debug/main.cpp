
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


class SphereMap : public Shader {
private:

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

        shader->_vertexAndPixelShader = VertexAndPixelShader::fromStrings(vs, ps);

        return shader;
    }


    void beforePrimitive(RenderDevice* rd) {
        rd->pushState();
        VertexAndPixelShader::ArgList args;
        args.set("texture", texture);
        args.set("hi", bounds.high());
        args.set("lo", bounds.low());
        rd->setVertexAndPixelShader(_vertexAndPixelShader, args);
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

    ShaderRef                   shader2;

    IFSModelRef                 model;

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
	texture = Texture::fromFile("d:/libraries/g3d-6_04/data/image/testImage.jpg");
    
//    shader = SphereMap::create();

/*
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
*/

    std::string vs = "";
    std::string ps = "void main(void) { gl_FragColor = vec4(1,1,1,1); }\n";
    //shader2 = Shader::fromStrings(vs, ps);

	model = IFSModel::create("d:/games/data/ifs/teapot.ifs");

//        "uniform vec3 hi;\n"
//        "uniform vec3 lo;\n"
   // Initialization
    
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


//	Draw::axes(CoordinateFrame(Vector3(0, 0, 0)), app->renderDevice);

    /*
    app->renderDevice->enableLighting();
    app->renderDevice->setLight(0, GLight::directional(Vector3(0,-1,1), Color3::yellow() * 0.5));
    app->renderDevice->setLight(1, GLight::directional(Vector3(0,1,1), Color3::red() * 0.5));
    app->renderDevice->setLight(2, GLight::directional(Vector3(1,0,1), Color3::blue() * 0.5));
    app->renderDevice->setLight(3, GLight::directional(Vector3(-1,0,1), Color3::green() * 0.5));
    app->renderDevice->setLight(4, GLight::directional(Vector3(0,1,-2), Color3::red() * 0.5));
    CoordinateFrame cframe;
    PosedModelRef posed = model->pose(cframe, false);
    app->renderDevice->setColor(Color3::white());
    app->renderDevice->setPolygonOffset(1);
    posed->render(app->renderDevice);
    app->renderDevice->setLineWidth(0.5);
    app->renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
    app->renderDevice->setPolygonOffset(0);
    app->renderDevice->disableLighting();
    app->renderDevice->setColor(Color3::black());
    app->renderDevice->setRenderMode(RenderDevice::RENDER_WIREFRAME);
    posed->render(app->renderDevice);
    */

    // Rendering loop
    app->renderDevice->pushState();

    app->renderDevice->setShader(shader2);
//    app->renderDevice->setLight(0, GLight::directional(Vector3(1,1,1), Color3::white() - Color3(.2,.2,.3)));
    app->renderDevice->setTexture(0, texture);
    model->pose()->render(app->renderDevice);

    app->renderDevice->popState();


    /*
    // Shader test
    Sphere sphere(Vector3::zero(), 0.5);
    app->renderDevice->setShader(shader2);
    Draw::sphere(sphere, app->renderDevice, Color3::WHITE, Color4::CLEAR);
    app->renderDevice->setShader(NULL);

    app->renderDevice->push2D();
    RenderDevice* renderDevice = app->renderDevice;

    //glPushAttrib(GL_ALL_ATTRIB_BITS);
  //  renderDevice->pushState();
        renderDevice->setTexture(0, texture);
        renderDevice->setAlphaTest(RenderDevice::ALPHA_GEQUAL, 0.05);
        glAlphaFunc(GL_GEQUAL, 0.5);
        glBegin(GL_LINES);
        glVertex2f(0,0);
        glVertex2f(0,10);
        glEnd();
//    renderDevice->popState();
    //glPopAttrib();
    
    app->renderDevice->pop2D();

//    SwapBuffers(
/*
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
    */
/*        Draw::box(AABox(Vector3(-1,-1,-1),Vector3(1,1,1)), app->renderDevice,
            Color3::WHITE, Color4::CLEAR);
            */
}



void App::main() {
	setDebugMode(true);
	debugController.setActive(false);
    Demo(this).run();    
}

App::App(const GAppSettings& settings) : GApp(settings, Win32Window::create(settings.window)) {
//App::App(const GAppSettings& settings) : GApp(settings) {    
}


void setIcon(const GImage& image) {
    alwaysAssertM((image.channels == 3) ||
                  (image.channels == 4), 
                  "Icon image must have at least 3 channels.");

    #ifdef G3D_WIN32
        alwaysAssertM((image.width == 32) && (image.height == 32),
            "Icons must be 32x32 on windows.");
    #endif

    uint8* mask = NULL;

    uint32 amask = 0xFF000000;
    uint32 bmask = 0x00FF0000;
    uint32 gmask = 0x0000FF00;
    uint32 rmask = 0x000000FF;

    if (image.channels == 4) {
        /*
        // Has an alpha channel; construct a mask
        int len = iCeil(image.width / 8) * image.height;
        mask = new uint8[len];
        // Initialize the mask to transparent
        System::memset(mask, 0, len);

        // Set bits with an alpha value >= 127.
        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                // Test this pixel
                bool bit = image.pixel4()[y * image.width + x].a >= 127;

                // Set the correct bit
                mask[y * image.width/8 + x / 8] |= (bit << (x % 8));
            }
        }
        */
    } else if (image.channels == 3) {
        // Take away the 4th channel.
        rmask = (rmask << 8) >> 16;
        gmask = (gmask << 8) >> 16;
        bmask = (bmask << 8) >> 16;
        amask = (amask << 8) >> 16;
    }

    int pixelBitLen     = image.channels * 8;
    int scanLineByteLen = image.channels * image.width;

    SDL_Surface* surface =
        SDL_CreateRGBSurfaceFrom((void*)image.byte(), image.width, image.height,
        pixelBitLen, scanLineByteLen, 
        rmask, gmask, bmask, amask);

    SDL_WM_SetIcon(surface, NULL);

    SDL_FreeSurface(surface);
    delete[] mask;
}

//////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    App(GAppSettings()).run();
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