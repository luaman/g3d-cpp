/**
  @file ParallaxBump_Demo/main.cpp

  This demo shows how to use the GApp/GApplet framework
  and how to use pixel and vertex shaders with Cg under 
  G3D.  It is an implementation of parallax
  bump mapping with per-pixel shading and normal maps.

  @author Morgan McGuire, matrix@graphics3d.com
  @created 2004-02-04
  @edited  2004-02-04
 */

#include <G3DAll.h>
#ifdef G3D_WIN32
    #include <direct.h>
    #define _chdir chdir
#endif
#include "Mesh.h"

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

    class App*              app;

    Demo(App* app);

    Mesh                    model;

    TextureRef              texture;

    /**
     Tangent space normal map with the bump map in 
     the alpha channel.
     */
    TextureRef              normalMap;

    PixelProgramRef         parallaxPP;
    VertexProgramRef        parallaxVP;

    virtual void init();

    virtual void doLogic();

	virtual void doNetwork();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();

    virtual void cleanup();

};


/**
 Given a tangent space bump map, computes a new image where the
 RGB channels are a tangent space normal map and the alpha channel
 is the original bump map.  Assumes the input image is tileable.

  @cite ATI demo
 */
void computeNormalMap(const GImage& bump, GImage& normal) {
    const int w = bump.width;
    const int h = bump.height;
    const int stride = bump.channels;

    normal.resize(w, h, 4);

    const uint8* const B = bump.byte();
    Color4uint8* const N = normal.pixel4();

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            // Index into normal map pixel
            int i = x + y * w;

            // Index into bump map *byte*
            int j = stride * i;

            // Copy over the bump value into the alpha channel
            N[i].a = B[j];

            Vector3 delta;

            // Get a value from B (with wrapping lookup) relative to (x, y)
            // and divide by 255
            #define height(DX, DY) ((B[(((DX + x + w) % w) + \
                                        ((DY + y + h) % h) * w) * stride]) / 255.0)

            // Sobel filter to compute the normal.  
            //
            // Y Filter (X filter is the transpose)
            //  [ -1 -2 -1 ]
            //  [  0  0  0 ]
            //  [  1  2  1 ]

            // Write the Y value directly into the x-component so we don't have
            // to explicitly compute a cross product at the end.
            delta.x = height(-1, -1) *  1 + height( 0, -1) *  2 + height( 1, -1) *  1 +
                      height(-1,  1) * -1 + height( 0,  1) * -2 + height( 1,  1) * -1;

            delta.y = height(-1, -1) * -1 + height( 1, -1) * 1 + 
                      height(-1,  0) * -2 + height( 1,  0) * 2 + 
                      height(-1,  1) * -1 + height( 1,  1) * 1;

            delta.z = 1.0;

            #undef height

            // Pack into byte range
            delta = delta.direction() * 127.5 + Vector3(127.5, 127.5, 127.5);
            N[i].r = iClamp(iRound(delta.x), 0, 255);
            N[i].g = iClamp(iRound(delta.y), 0, 255);
            N[i].b = iClamp(iRound(delta.z), 0, 255);
        }
    }
}


Demo::Demo(App* _app) : GApplet(_app), app(_app) {

    if (! fileExists("texture.jpg")) {
        // Go into the right directory
        chdir("ParallaxBump_Demo");
    }

    texture = Texture::fromFile("texture.jpg");

    CImage bump("bump.jpg");
    CImage normal;

    computeNormalMap(bump, normal);

    normalMap = Texture::fromGImage("Normal Map", normal);

    parallaxPP = PixelProgram::fromFile("parallaxPP.pp");
    parallaxVP = VertexProgram::fromFile("parallaxVP.vp");
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 4, 3));
    app->debugCamera.lookAt(Vector3(0, 1, 0));
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

    System::sleep(.005);

    LightingParameters lighting(G3D::toSeconds(2, 00, 00, AM), false);
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (! app->sky.isNull()) {
        app->sky->render(lighting);
    }

    // Create a light 
    Vector4 wsLight(1,2.5,2,1);

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

        CoordinateFrame cframe;
        // Rotate the quad
        cframe.rotation = Matrix3::fromAxisAngle(Vector3::unitY(), System::getTick() * .1);

        app->renderDevice->pushState();
            GPUProgram::ArgList vertexArgs;

            app->renderDevice->setObjectToWorldMatrix(cframe);

            // Take the light to object space
            Vector4 osLight = cframe.toObjectSpace(wsLight);

            // Take the viewer to object space
            Vector3 osEye = cframe.pointToObjectSpace(app->debugCamera.getCoordinateFrame().translation);

            vertexArgs.set("MVP", app->renderDevice->getModelViewProjectionMatrix());
            vertexArgs.set("osLight", osLight);
            vertexArgs.set("osEye", osEye);
            app->renderDevice->setVertexProgram(parallaxVP, vertexArgs);

            GPUProgram::ArgList pixelArgs;
            pixelArgs.set("texture", texture);
            pixelArgs.set("normalMap", normalMap);
            app->renderDevice->setPixelProgram(parallaxPP, pixelArgs);

            model.render(app->renderDevice);
        app->renderDevice->popState();


    app->renderDevice->disableLighting();

    Draw::sphere(Sphere(wsLight.xyz(), .1), app->renderDevice, Color3::white(), Color4::clear());

    if (! app->sky.isNull()) {
        app->sky->renderLensFlare(lighting);
    }

    app->renderDevice->push2D();
        app->debugFont->draw2D("The surface is a single quad textured with parallax bump mapping and per-pixel shading.", Vector2(10, 10), 10, Color3::white(), Color3::black());
        app->debugFont->draw2D("Press TAB to toggle to first person camera controls.", Vector2(10, 30), 10, Color3::white(), Color3::black());
    app->renderDevice->pop2D();
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    // Load objects here
    sky = Sky::create(renderDevice, dataDir + "sky/");
    debugShowRenderingStats = false;

    Demo(this).run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    renderDevice->setCaption("G3D Parallax Bump Mapping Demo");
}


int main(int argc, char** argv) {
    GAppSettings settings;
    settings.window.width = 800;
    settings.window.height = 600;
    App(settings).run();
    return 0;
}
