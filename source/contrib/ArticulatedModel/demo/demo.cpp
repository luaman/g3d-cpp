/**
  @file ArticulatedModel/demo.cpp


  // Known driver bugs: 
  //  Radeon Mobility 9000.  Cube maps render incorrectly
  //                         Vertex arrays don't work with wireframe mode


  @author Morgan McGuire, matrix@graphics3d.com
 */

#include <G3DAll.h>

#if G3D_VER < 60500
    #error Requires G3D 6.05
#endif

#include "App.h"



// When not in the same directory, you can #include these files 
// from the G3D distribution as:
// #include <../contrib/ArticulatedModel/ArticulatedModel.h>
#include "../ToneMap.h"
#include "../SuperShader.h"
#include "../ArticulatedModel.h"


GApp* app = NULL; 

/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
private:

    void generateShadowMap(const GLight& light, const Array<PosedModelRef>& shadowCaster);

    ToneMap                     toneMap;

    TextureRef                  shadowMap;
    Matrix4                     lightMVP;

public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*					app;

    Demo(App* app);    

    virtual void init();

    virtual void doSimulation(RealTime dt);

    virtual void doLogic();

    virtual void doGraphics();

};


/**
 Width and height of shadow map.
 */
const int shadowMapSize = 512;

Demo::Demo(App* _app) : GApplet(_app), app(_app) {

    if (GLCaps::supports_GL_ARB_shadow()) {        
        shadowMap = Texture::createEmpty(shadowMapSize, shadowMapSize, "Shadow map", TextureFormat::depth(),
            Texture::CLAMP, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D, Texture::DEPTH_LEQUAL);
            
    }

}


void Demo::init()  {
	// Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 0, 10));
    app->debugCamera.lookAt(Vector3(0, 0, 0));
}


void Demo::doSimulation(RealTime dt) {
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

    if (app->userInput->keyPressed(' ')) {
        toneMap.setEnabled(! toneMap.enabled());
    }
}

bool debugShadows = false;

void Demo::generateShadowMap(const GLight& light, const Array<PosedModelRef>& shadowCaster) {
    debugAssert(GLCaps::supports_GL_ARB_shadow()); 

    app->renderDevice->pushState();

        const double lightProjX = 12, lightProjY = 12, lightProjNear = 1, lightProjFar = 40;

        // Construct a projection and view matrix for the camera so we can 
        // render the scene from the light's point of view
        Matrix4 lightProjectionMatrix(Matrix4::orthogonalProjection(-lightProjX, lightProjX, -lightProjY, lightProjY, lightProjNear, lightProjFar));

        CoordinateFrame lightCFrame;
        lightCFrame.translation = light.position.xyz() * 20;

        // The light will never be along the z-axis
        lightCFrame.lookAt(Vector3::zero(), Vector3::unitZ());

        debugAssert(shadowMapSize < app->renderDevice->getHeight());
        debugAssert(shadowMapSize < app->renderDevice->getWidth());

        app->renderDevice->setColorClearValue(Color3::white());
        app->renderDevice->clear(debugShadows, true, false);

        Rect2D rect = Rect2D::xywh(0, 0, shadowMapSize, shadowMapSize);

        app->renderDevice->enableDepthWrite();
        app->renderDevice->setViewport(rect);

	    // Draw from the light's point of view
        app->renderDevice->setCameraToWorldMatrix(lightCFrame);
        app->renderDevice->setProjectionMatrix(lightProjectionMatrix);

        // Flip the Y-axis to account for the upside down Y-axis on read back textures
        lightMVP = lightProjectionMatrix * lightCFrame.inverse();

        if (! debugShadows) {
            app->renderDevice->disableColorWrite();
        }

        // Avoid acne
        app->renderDevice->setPolygonOffset(2);

        for (int s = 0; s < shadowCaster.size(); ++s) {
            shadowCaster[s]->render(app->renderDevice);
        }
   
    app->renderDevice->popState();
    debugAssert(shadowMap.notNull());
    shadowMap->copyFromScreen(rect);
}



void Demo::doGraphics() {

    LightingRef        lighting      = toneMap.prepareLighting(app->lighting);
    LightingParameters skyParameters = toneMap.prepareLightingParameters(app->skyParameters);

    // Pose all
    Array<PosedModelRef> posedModels;

    for (int e = 0; e < app->entityArray.size(); ++e) {
        static RealTime t0 = System::time();
        RealTime t = (System::time() - t0) * 10;
        app->entityArray[e]->pose.cframe.set("Top", 
            CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitY(), t),
                            Vector3::zero()));

        app->entityArray[e]->pose.cframe.set("Clouds", 
            CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitY(), t/1000),
                            Vector3::zero()));

        app->entityArray[e]->model->pose(posedModels, app->entityArray[e]->cframe, app->entityArray[e]->pose);
    }
    Array<PosedModelRef> opaque, transparent;
    PosedModel::sort(posedModels, app->debugCamera.getCoordinateFrame().lookVector(), opaque, transparent);

    if (GLCaps::supports_GL_ARB_shadow() && (lighting->shadowedLightArray.size() > 0)) {     
        // Generate shadow map
        generateShadowMap(lighting->shadowedLightArray[0], opaque);
    }

    /////////////////////////////////////////////////////////////////////

    if (debugShadows) {
        return;
    }

    if (! GLCaps::supports_GL_ARB_shadow() && (lighting->shadowedLightArray.size() > 0)) {
        // We're not going to be able to draw shadows, so move the shadowed lights into
        // the unshadowed category.
        lighting->lightArray.append(lighting->shadowedLightArray);
        lighting->shadowedLightArray.clear();
    }

    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);
    app->renderDevice->setObjectToWorldMatrix(CoordinateFrame());


    app->debugPrintf("%d opaque, %d transparent\n", opaque.size(), transparent.size());

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.notNull(), true, true);
    if (app->sky.notNull()) {
        app->sky->render(skyParameters);
    }

    app->renderDevice->pushState();
        // Opaque unshadowed
        for (int m = 0; m < opaque.size(); ++m) {
            opaque[m]->renderNonShadowed(app->renderDevice, lighting);
        }

        // Opaque shadowed
        if (lighting->shadowedLightArray.size() > 0) {
            for (int m = 0; m < opaque.size(); ++m) {
                opaque[m]->renderShadowMappedLightPass(app->renderDevice, lighting->shadowedLightArray[0], lightMVP, shadowMap);
            }
        }

        // Transparent + shadowed
        for (int m = 0; m < transparent.size(); ++m) {
            transparent[m]->renderNonShadowed(app->renderDevice, lighting);
            if (lighting->shadowedLightArray.size() > 0) {
                transparent[m]->renderShadowMappedLightPass(app->renderDevice, lighting->shadowedLightArray[0], lightMVP, shadowMap);
            }
        }

    app->renderDevice->popState();

    toneMap.apply(app->renderDevice);

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(skyParameters);
    }

    app->debugPrintf("State Changes for OpenGL/RenderDevice: Major: %d/%d  Minor: %d/%d\n\n", 
        (int)app->renderDevice->debugNumMajorOpenGLStateChanges(),
        (int)app->renderDevice->debugNumMajorStateChanges(),
        (int)app->renderDevice->debugNumMinorOpenGLStateChanges(),
        (int)app->renderDevice->debugNumMinorStateChanges());

    app->debugPrintf("Tone Map %s\n", toneMap.enabled() ? "On" : "Off");
    app->debugPrintf("%s Profile %s\n", toString(ArticulatedModel::profile()),
        #ifdef _DEBUG
                "(DEBUG mode)"
        #else
                ""
        #endif
        );
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    loadScene();

    Demo(this).run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    ::app = this;
    debugShowRenderingStats = true;
    sky = Sky::create(renderDevice, dataDir + "sky/");//"D:/games/data/sky/", "majestic/majestic512_*.jpg");
}


int main(int argc, char** argv) {
    GAppSettings settings;
    settings.window.depthBits = 24;
    settings.window.stencilBits = 8;
    settings.window.alphaBits = 0;
    settings.window.rgbBits = 8;
    settings.window.fsaaSamples = 1;
    settings.window.width = 800;
    settings.window.height = 600;
	settings.useNetwork = false;
    App(settings).run();
    return 0;
}
