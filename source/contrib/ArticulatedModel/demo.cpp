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

#include "SuperShader.h"
#include "ArticulatedModel.h"

typedef ReferenceCountedPointer<class Entity> EntityRef;
class Entity : public ReferenceCountedObject {
private:

    Entity();

public:

    ArticulatedModelRef         model;

    ArticulatedModel::Pose      pose;

    /** Root frame */
    CoordinateFrame             cframe;

    static EntityRef create() {
        return new Entity();
    }
};

GApp* app = NULL; 

/*
// When not in the same directory, you can #include these files 
// from the G3D distribution as:
#include <../contrib/ArticulatedModel/ArticulatedModel.cpp>
*/

class App : public GApp {
protected:
    void main();
public:
    SkyRef                      sky;

    LightingRef                 lighting;

    Array<ArticulatedModelRef>  modelArray;
    Array<EntityRef>            entityArray;
    TextureRef                  texture;

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

    virtual void doSimulation(RealTime dt);

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


void Demo::doSimulation(RealTime dt) {
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }
}


void Demo::doGraphics() {

    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);
    app->renderDevice->setObjectToWorldMatrix(CoordinateFrame());

    LightingParameters lighting(G3D::toSeconds(2, 00, 00, PM));

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.notNull(), true, true);
    if (app->sky.notNull()) {
        app->sky->render(lighting);
    }

    app->renderDevice->pushState();
        ArticulatedModel::Pose pose;
        static RealTime t0 = System::time();

        RealTime t = (System::time() - t0) * 10;
        pose.cframe.set("m_rotor", 
            CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitY(), t),
                            Vector3::zero()));
        pose.cframe.set("t_rotor",
            CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitX(), t*2),
                            Vector3::zero()));

        Array<PosedModelRef> posedModels;
        for (int m = 0; m < app->modelArray.size(); ++m) {
            app->modelArray[m]->pose(posedModels, CoordinateFrame(Vector3(2*m,0,0)), pose);
        }

        Draw::axes(app->renderDevice);

        Array<PosedModelRef> opaque, transparent;
        PosedModel::sort(posedModels, app->debugCamera.getCoordinateFrame().lookVector(), opaque, transparent);

        for (int m = 0; m < opaque.size(); ++m) {
            opaque[m]->renderNonShadowed(app->renderDevice, app->lighting);
        }
        for (int m = 0; m < transparent.size(); ++m) {
            transparent[m]->renderNonShadowed(app->renderDevice, app->lighting);
        }
    app->renderDevice->popState();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(lighting);
    }

}


void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    {
        ArticulatedModelRef model = ArticulatedModel::fromFile("d:/games/data/ifs/sphere.ifs", 1);

        SuperShader::Material& material = model->partArray[0].triListArray[0].material;
        model->partArray[0].triListArray[0].cullFace = RenderDevice::CULL_NONE;
        material.diffuse = Color3::yellow() * .7;
        material.transmit = Color3(.5,.3,.3);
        material.reflect = Color3::white() * .1;
        material.specular = Color3::white() * .8;
        material.specularExponent = Color3::white() * 40;
        model->updateAll();

        modelArray.append(model);
    }
    
    /*
    {
        ArticulatedModelRef model = ArticulatedModel::fromFile(
		"d:/games/cpp/source/contrib/ArticulatedModel/3ds/fs/fs.3ds"
            );
        modelArray.append(model);
    }
    */

    {
        ArticulatedModelRef model = ArticulatedModel::fromFile("d:/games/data/ifs/venus-torso.ifs", 1.5);

        SuperShader::Material& material = model->partArray[0].triListArray[0].material;
        model->partArray[0].triListArray[0].cullFace = RenderDevice::CULL_NONE;
        material.diffuse = Color3(.9, .9, .8);
        material.transmit = Color3::black();
        material.reflect = Color3::white() * .04;
        material.specular = Color3::white() * .3;
        material.specularExponent = Color3::white() * 5;
        model->updateAll();

        modelArray.append(model);
    }
    {
        ArticulatedModelRef model = ArticulatedModel::fromFile("d:/games/data/ifs/jackolantern.ifs", 1);

        SuperShader::Material& material = model->partArray[0].triListArray[0].material;
        model->partArray[0].triListArray[0].cullFace = RenderDevice::CULL_NONE;
        material.diffuse = Color3::fromARGB(0xF28900);
        material.transmit = Color3::black();
        material.reflect = Color3::black();
        material.specular = Color3::white() * .3;
        material.specularExponent = Color3::white() * 60;
        model->updateAll();

        modelArray.append(model);
    }

    {
        ArticulatedModelRef model = ArticulatedModel::fromFile("d:/games/data/ifs/trumpet.ifs", 1);

        Color3 brass = Color3::fromARGB(0xFFFDDC01);

        SuperShader::Material& material = model->partArray[0].triListArray[0].material;
        model->partArray[0].triListArray[0].cullFace = RenderDevice::CULL_BACK;
        material.diffuse = brass * .4;
        material.reflect = brass * .5;
        material.specular = Color3::white() * .8;
        material.specularExponent = Color3::white() * 25;
        model->updateAll();

        modelArray.append(model);
    }


    {
        ArticulatedModelRef model = ArticulatedModel::fromFile("d:/games/data/ifs/mech-part.ifs", 1);

        SuperShader::Material& material = model->partArray[0].triListArray[0].material;
        model->partArray[0].triListArray[0].cullFace = RenderDevice::CULL_BACK;
        material.diffuse = Color3::red();
        material.reflect = Color3::black();
        material.specular = Color3::white();
        material.specularExponent = Color3::white() * 30;
        model->updateAll();

        modelArray.append(model);
    }
    {
        ArticulatedModelRef model = ArticulatedModel::fromFile("d:/games/data/ifs/sphere.ifs", 1);

        SuperShader::Material& material = model->partArray[0].triListArray[0].material;
        model->partArray[0].triListArray[0].cullFace = RenderDevice::CULL_BACK;
        material.diffuse = Color3::white() * .8;
        material.specular = Color3::white() * .3;
        material.specularExponent = Color3::white() * 40;
        model->updateAll();

        modelArray.append(model);
    }

//		"C:/morgan/cpp/source/contrib/ArticulatedModel/3ds/f16/f16b.3ds"
//		"C:/morgan/cpp/source/contrib/ArticulatedModel/3ds/cube.3ds"
//		"C:/morgan/cpp/source/contrib/ArticulatedModel/3ds/jeep/jeep.3ds", 0.1
//		"C:/morgan/cpp/source/contrib/ArticulatedModel/3ds/house/house.3ds", 0.01
//		"C:/morgan/cpp/source/contrib/ArticulatedModel/3ds/delorean/delorean.3ds", 0.1
//		"C:/morgan/cpp/source/contrib/ArticulatedModel/3ds/car35/car35.3ds", 0.1
//		"C:/morgan/cpp/source/contrib/ArticulatedModel/3ds/fs/fs.3ds"


    lighting = Lighting::create();
    {
        LightingParameters params(G3D::toSeconds(2, 00, 00, PM));
    
        if (sky.notNull()) {
            //lighting->environmentMap.constant = lighting.skyAmbient;
            lighting->environmentMap = sky->getEnvironmentMap();
            lighting->environmentMapColor = params.skyAmbient;
        } else {
            lighting->environmentMapColor = Color3::black();
        }

        lighting->ambientTop = Color3(.7, .7, 1) * params.diffuseAmbient;
        lighting->ambientBottom = Color3::brown() * params.diffuseAmbient;

        lighting->lightArray.clear();
        lighting->lightArray.append(params.directionalLight());

        // Decrease the blue since we're adding blue ambient
        lighting->lightArray.last().color *= Color3(1.2, 1.2, 1);
    }

    Demo(this).run();
}

App::App(const GAppSettings& settings) : GApp(settings) {
    ::app = this;
    sky = Sky::create(renderDevice, "D:/games/data/sky/", "majestic/majestic512_*.jpg");
}


int main(int argc, char** argv) {
    GAppSettings settings;
    settings.window.depthBits = 24;
    settings.window.stencilBits = 8;
    settings.window.alphaBits = 0;
    settings.window.rgbBits = 8;
    settings.window.fsaaSamples = 4;
    settings.window.width = 600;
    settings.window.height = 600;
	settings.useNetwork = false;
    App(settings).run();
    return 0;
}
