/**
  @file gfxmeter/main.cpp


  multitexture
  vs level
  ps level

  cube map
  shadow map
  vertex array

  2-side stencil
  texture compression
  fbo

  cube map bug (radeon 9000)
  slow/shadow map (radeon)

  @author Morgan McGuire, matrix@graphics3d.com
 */

#include "../include/G3DAll.h"

#if G3D_VER < 60700
    #error Requires G3D 6.07
#endif

int CPU_speed_in_MHz();


void shaderVersions(
    std::string& regStr,
    std::string& asmStr,
    std::string& glslStr);


class MD2 {
public:
    CoordinateFrame     cframe;
    MD2ModelRef         model;
    MD2Model::Pose      pose;
    GMaterial           material;

    void load(const std::string& filename) {
        model = MD2Model::create(filename + ".md2");
        material.texture.append(Texture::fromFile(filename + ".pcx", 
            TextureFormat::AUTO, Texture::TILE, Texture::TRILINEAR_MIPMAP, Texture::DIM_2D, 2.0));
    }

    void render(RenderDevice* rd) {
        model->pose(cframe, pose, material)->render(rd);
    }

    void renderShadow(RenderDevice* rd) {
        CoordinateFrame cframe2 = cframe;
        cframe2.rotation.setColumn(1, Vector3::zero());
        cframe2.translation.y -= 1.7;
        rd->setColor(Color3(.9, .9, 1));

        PosedModelRef m = model->pose(cframe2, pose);

        // Intentionally render a lot of shadows to gauge rendering performance
        for (int i = 0; i < 20; ++i) {
            m->render(rd);
        }
    }

    void doSimulation(GameTime dt) {
        pose = MD2Model::Pose(MD2Model::STAND, System::getTick());
        /*
        pose.doSimulation(dt,
            false, false, false, 
            true,  false, false, 
            false, false, false, 
            false, false, false,
            false, false, false,
            false);
            */
    }
};

/**
 Renders the summary page
 */
class Report : public GApplet {
public:

    MD2                 knight;
    MD2                 ogre;

    class App*          app;

    Report(App* app);

    /** Draw some nice graphics */
    void doFunStuff();

    virtual ~Report() {}

    virtual void init();

    virtual void doLogic();

	virtual void doNetwork();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();

    virtual void cleanup();

};



class App : public GApp {
protected:
    void main();
public:

    SkyRef                  sky;

    TextureRef              cardLogo;
    TextureRef              chipLogo;
    std::string             chipSpeed;
    TextureRef              osLogo;

    GFontRef                titleFont;
    GFontRef                reportFont;

    std::string             combineShader;
    std::string             asmShader;
    std::string             glslShader; 

    int                     featureRating;
    float                   performanceRating;
    int                     bugCount;

    Report*                 applet;

    void computeFeatureRating();
    void countBugs();

    App(const GAppSettings& settings);

    ~App();
};


/** Converts a bug count into a quality rating*/
const char* quality(int bugCount) {
    static const char* q[] = {"A+", "A-", "B+", "B-", "C+", "C-", "D+", "D-", "F"};
    return q[iClamp(bugCount, 0, 8)];
}

Report::Report(App* _app) : GApplet(_app), app(_app) {
}


void Report::init()  {
    // Called before Report::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));

    // Init the fun stuff
    knight.load("pknight");
    knight.cframe.translation = Vector3(-5, 0, 0);

    ogre.load("ogro");
    ogre.cframe.translation = Vector3(-1.5, 0, 0);

    knight.cframe.lookAt(ogre.cframe.translation + Vector3(0,0,1));
    ogre.cframe.lookAt(knight.cframe.translation + Vector3(0,0,1));
}


void Report::cleanup() {
    // Called when Report::run() exits
}


void Report::doNetwork() {
	// Poll net messages here
}


void Report::doSimulation(SimTime dt) {
	// Add physical simulation here

    GameTime deltaTime = 0.02;

    knight.doSimulation(deltaTime);
    ogre.doSimulation(deltaTime);
}


void Report::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

}


void Report::doFunStuff() {
    app->renderDevice->pushState();
        GCamera camera;
        camera.setCoordinateFrame(Vector3(0,1,10));
        camera.lookAt(Vector3(0,2.8,0));

        app->renderDevice->setProjectionAndCameraMatrix(camera);


        knight.renderShadow(app->renderDevice);
        ogre.renderShadow(app->renderDevice);

        app->renderDevice->enableLighting();
        app->renderDevice->setLight(0, GLight::directional(Vector3(-1,1,2).direction(), Color3(.8,.8,.7)));
        app->renderDevice->setLight(1, GLight::directional(Vector3(.5,-1,1).direction(), Color3::red() * 0.2));
        app->renderDevice->setAmbientLightColor(Color3(.5,.5,.6));

        knight.render(app->renderDevice);
        ogre.render(app->renderDevice);

    app->renderDevice->popState();
}


static void drawBar(RenderDevice* rd, int value, const Vector2& p) {
    float s = rd->getWidth() * 0.35 / 100.0;
    Draw::rect2D(Rect2D::xywh(p.x, p.y, 100 * s, 20), rd, Color3::white() * 0.9);
    Draw::rect2D(Rect2D::xywh(p.x, p.y, value * s, 20), rd, Color3::yellow());
    Draw::rect2DBorder(Rect2D::xywh(p.x, p.y, 100 * s, 20), rd, Color3::black());
}

void Report::doGraphics() {
    app->renderDevice->setColorClearValue(Color3::white());
    app->renderDevice->clear();

    doFunStuff();

    app->renderDevice->push2D();

        int w = app->renderDevice->getWidth();
        int h = app->renderDevice->getHeight();

        ///////////////////////////////////////
        // Left panel
#       define LABEL(str) p.y += app->titleFont->draw2D(app->renderDevice, str, p - Vector2(w * 0.0075, 0), s * 2, Color3::white() * 0.4).y
#       define PRINT(str) p.y += app->reportFont->draw2D(app->renderDevice, str, p, s, Color3::black()).y

        int x0 = w * 0.015;
        // Cursor position
        Vector2 p(x0, h * 0.02);

        // Font size
        float s = w * 0.013;

        LABEL("Shaders");
        PRINT(std::string("Combiners: ") + app->combineShader);
        PRINT(std::string("Assembly:   ") + app->asmShader);
        PRINT(std::string("GLSL:         ") + app->glslShader);

        p.y += s * 2;
        LABEL("Extensions");
        PRINT(std::string("FSAA:                           ") + ((GLCaps::supports("WGL_ARB_multisample") || GLCaps::supports("GL_ARB_multisample")) ? "Yes" : "No"));
        PRINT(std::string("Two-sided Stencil:        ") + (GLCaps::supports("GL_EXT_stencil_two_side") ? "Yes" : "No"));
        PRINT(std::string("Stencil Wrap:               ") + (GLCaps::supports("GL_EXT_stencil_wrap") ? "Yes" : "No"));
        PRINT(std::string("Texture Compression: ") + (GLCaps::supports("GL_EXT_texture_compression_s3tc") ? "Yes" : "No"));
        PRINT(std::string("Shadow Maps:             ") + (GLCaps::supports("GL_ARB_shadow") ? "Yes" : "No"));
        PRINT(std::string("Frame Buffer Object:   ") + (GLCaps::supports("GL_EXT_framebuffer_object") ? "Yes" : "No"));
        PRINT(std::string("Vertex Arrays:              ") + (GLCaps::supports_GL_ARB_vertex_buffer_object() ? "Yes" : "No"));
        
            
        ///////////////////////////////////////
        // Right Panel
        x0 = w * 0.6;
        // Cursor position
        p = Vector2(x0, h * 0.02);

        // Graphics Card
        LABEL("Graphics Card");
        app->renderDevice->setTexture(0, app->cardLogo);
        Draw::rect2D(Rect2D::xywh(p.x - s * 6, p.y, s * 5, s * 5), app->renderDevice);
        app->renderDevice->setTexture(0, NULL);

        PRINT(GLCaps::vendor().c_str());
        PRINT(GLCaps::renderer().c_str());
        PRINT(format("Driver Version %s", GLCaps::driverVersion().c_str()));

        p.y += s * 2;

        // Operating System
        LABEL("Processor");
        app->renderDevice->setTexture(0, app->chipLogo);
        Draw::rect2D(Rect2D::xywh(p.x - s * 6, p.y, s * 5, s * 5), app->renderDevice);
        app->renderDevice->setTexture(0, NULL);

        PRINT(System::cpuVendor().c_str());
        PRINT(System::cpuArchitecture().c_str());

        Array<std::string> features;
        if (System::has3DNow()) {
            features.append("3DNow");
        }
        if (System::hasMMX()) {
            features.append("MMX");
        }
        if (System::hasSSE()) {
            features.append("SSE");
        }
        if (System::hasSSE2()) {
            features.append("SSE2");
        }
        if (app->chipSpeed != "") {
            PRINT(app->chipSpeed + " " + stringJoin(features, '/'));
        } else {
            PRINT(stringJoin(features, '/'));
        }

        p.y += s * 2;

        // Operating System
        LABEL("OS");
        app->renderDevice->setTexture(0, app->osLogo);
        Draw::rect2D(Rect2D::xywh(p.x - s * 6, p.y, s * 5, s * 5), app->renderDevice);
        app->renderDevice->setTexture(0, NULL);


        if (beginsWith(System::operatingSystem(), "Windows 5.0")) {
            PRINT("Windows 2000");
        } else if (beginsWith(System::operatingSystem(), "Windows 5.1")) {
            PRINT("Windows XP");
        }
        PRINT(System::operatingSystem().c_str());

        p.y += s * 4;

        x0 = w - s * 10;
        app->titleFont->draw2D(app->renderDevice, "Features", p - Vector2(w * 0.0075, 0), s * 2, Color3::white() * 0.4);
        p.y += app->reportFont->draw2D(app->renderDevice, format("f%d", app->featureRating), Vector2(x0, p.y), s*2, Color3::red() * 0.5).y;
        drawBar(app->renderDevice, app->featureRating, p);

        // Designed to put NV40 at 50
        app->performanceRating = app->renderDevice->getFrameRate() / 2.0;

        p.y += s * 4;
        app->titleFont->draw2D(app->renderDevice, "Speed", p - Vector2(w * 0.0075, 0), s * 2, Color3::white() * 0.4);
        p.y += app->reportFont->draw2D(app->renderDevice, format("%5.1f", iRound(app->performanceRating * 10) / 10.0), Vector2(x0 - s*2, p.y), s*2, Color3::red() * 0.5).y;
        drawBar(app->renderDevice, app->performanceRating, p);

        p.y += s * 4;
        app->titleFont->draw2D(app->renderDevice, "Quality", p - Vector2(w * 0.0075, 0), s * 2, Color3::white() * 0.4);
        p.y += app->reportFont->draw2D(app->renderDevice, quality(app->bugCount), Vector2(x0, p.y), s*2, Color3::red() * 0.5).y;
        drawBar(app->renderDevice, iClamp(100 - app->bugCount * 10, 0, 100), p);

#       undef PRINT


        p.y = h - 50;
#define PRINT(str) p.y += app->reportFont->draw2D(app->renderDevice, str, p, 8, Color3::black()).y;

        PRINT("These ratings are based on the performance of G3D apps.");
        PRINT("They may not be representative of overall 3D performance.");
        PRINT("Speed is based on both processor and graphics card. Upgrading");
        PRINT("your graphics driver may improve Quality and Features.");

#       undef LABEL
        

    app->renderDevice->pop2D();
}


void App::main() {
	setDebugMode(false);
	debugController.setActive(false);


    // Choose a card logo
    {
        std::string filename = "";
        if (beginsWith(GLCaps::vendor(), "NVIDIA")) {
            filename = "nvidia.jpg";
        } else if (beginsWith(GLCaps::vendor(), "ATI")) {
            filename = "ati.jpg";
        }

        if (filename != "") {
            cardLogo = Texture::fromFile(filename, TextureFormat::AUTO, Texture::CLAMP);
        }
    }

    // Choose chip logo
    {
        std::string filename = "";
        if (endsWith(toLower(System::cpuVendor()), "intel")) {
            filename = "intel.jpg";
        } else if (endsWith(toLower(System::cpuVendor()), "amd")) {
            filename = "amd.jpg";
        }

        if (filename != "") {
            chipLogo = Texture::fromFile(filename, TextureFormat::AUTO, Texture::CLAMP);
        }

#       ifdef G3D_WIN32
            double speed = CPU_speed_in_MHz() * 1e6;
            if (speed > 1e9) {
                chipSpeed = format("%.1f GHz", speed / 1e9);
            } else if (speed > 10e6) {
                chipSpeed = format("%.1f MHz", speed / 1e6);
            }
            // Probably a bad result if speed is less than 1 MHz
#       endif
    }

    // Choose os logo
    {
        std::string filename = "";
        std::string os = System::operatingSystem ();

        //Windows 5.0 build 2195 Platform 2 Service Pack 4
        if (beginsWith(os, "Windows 5.0")) {
            filename = "win2k.jpg";
        } else if (beginsWith(os, "Windows 5.1")) {
            filename = "winxp.jpg";
        }

        if (filename != "") {
            osLogo = Texture::fromFile(filename, TextureFormat::AUTO, Texture::CLAMP);
        }
    }

    titleFont = GFont::fromFile(NULL, dataDir + "carbon.fnt");
    reportFont = GFont::fromFile(NULL, dataDir + "arial.fnt");

    shaderVersions(combineShader, asmShader, glslShader);
    computeFeatureRating();
    countBugs();

    Log::common()->printf("Shaders:\n");
    Log::common()->printf("   Combiners: %s\n", combineShader.c_str());
    Log::common()->printf("   Assembly:  %s\n", asmShader.c_str());
    Log::common()->printf("   GLSL:      %s\n", glslShader.c_str());
    Log::common()->printf("\n\n");


    // Load objects here
    sky = NULL;//Sky::create(NULL, dataDir + "sky/");

    applet->run();
}


void App::countBugs() {
    bugCount = 0;

    if (GLCaps::hasBug_glMultiTexCoord3fvARB()) {
        // Radeon mobility has a cube map bug where 
        // tex coords are handled incorrectly.
        ++bugCount;
    } else if (beginsWith(GLCaps::renderer(), "RADEON") &&
        GLCaps::supports_GL_ARB_shadow() &&
        GLCaps::supports_GL_ARB_shading_language_100()) {
        // Slow shadow map and GLSL texture binding bugs.
        // TODO: add actual tests
        bugCount += 2;
    }
}


void App::computeFeatureRating() {

    if ((glslShader != "None") &&
        GLCaps::supports("GL_ARB_shadow") &&
        GLCaps::supports("GL_EXT_framebuffer_object") &&
        GLCaps::supports("GL_EXT_stencil_wrap")) {

        // NV47 w/ good drivers
        featureRating = 60;

    } else if ((glslShader != "None") &&
        GLCaps::supports("GL_ARB_shadow") &&
        GLCaps::supports("GL_EXT_stencil_wrap")) {

        // R800 w/ good drivers
        featureRating = 50;

    } else if ((asmShader != "None") &&
        GLCaps::supports("GL_ARB_shadow") &&
        GLCaps::supports("GL_EXT_stencil_wrap")) {

        // NV30
        featureRating = 40;

    } else if ((asmShader != "None") &&
        GLCaps::supports("GL_EXT_stencil_wrap")) {

        featureRating = 35;

    } else if (asmShader != "None") {

        // PS 2.0 card
        featureRating = 30;

    } else if ((combineShader != "None") &&
        GLCaps::supports_GL_ARB_multitexture() &&
        GLCaps::supports_GL_ARB_texture_cube_map()) {

        // PS 1.4 card
        featureRating = 20;

    } else if (
        GLCaps::supports_GL_ARB_multitexture() &&
        GLCaps::supports_GL_ARB_texture_cube_map()) {

        featureRating = 15;

    } else if (GLCaps::supports_GL_ARB_texture_cube_map()) {

        featureRating = 14;

    } else {

        featureRating = 10;
    }
}

App::App(const GAppSettings& settings) : GApp(settings) {
    window()->setCaption("GFX-Meter 0.5");
    applet = new Report(this);
}


App::~App() {
    delete applet;
}


int main(int argc, char** argv) {
    GAppSettings settings;
    
    settings.useNetwork = false;
    settings.window.fsaaSamples = 4;
    settings.dataDir = "./";
    settings.window.defaultIconFilename = "g3d.ico";

    App(settings).run();
    return 0;
}
