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

    Entity() {}

public:

    ArticulatedModelRef         model;

    ArticulatedModel::Pose      pose;

    /** Root frame */
    CoordinateFrame             cframe;

    static EntityRef create(
        ArticulatedModelRef model = NULL,
        const CoordinateFrame& c = CoordinateFrame()) {

        Entity* e = new Entity();

        e->model = model;
        e->cframe = c;
        return e;
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

    Array<EntityRef>            entityArray;
    TextureRef                  texture;

    App(const GAppSettings& settings);
};

/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
private:

    void generateShadowMap(const GLight& light, const Array<PosedModelRef>& shadowCaster);

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
            shadowCaster[s]->renderNonShadowed(app->renderDevice, NULL);
        }
   
    app->renderDevice->popState();
    debugAssert(shadowMap.notNull());
    shadowMap->copyFromScreen(rect);
}


void Demo::doGraphics() {

    // Pose all
    Array<PosedModelRef> posedModels;
    for (int e = 0; e < app->entityArray.size(); ++e) {
        static RealTime t0 = System::time();
        RealTime t = (System::time() - t0) * 10;
        app->entityArray[e]->pose.cframe.set("m_rotor", 
            CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitY(), t),
                            Vector3::zero()));
        app->entityArray[e]->pose.cframe.set("t_rotor",
            CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitX(), t*2),
                            Vector3::zero()));

        app->entityArray[e]->model->pose(posedModels, app->entityArray[e]->cframe, app->entityArray[e]->pose);
    }
    Array<PosedModelRef> opaque, transparent;
    PosedModel::sort(posedModels, app->debugCamera.getCoordinateFrame().lookVector(), opaque, transparent);

    if (GLCaps::supports_GL_ARB_shadow() && (app->lighting->shadowedLightArray.size() > 0)) {        
        // Generate shadow map
        generateShadowMap(app->lighting->shadowedLightArray[0], opaque);
    }

    /////////////////////////////////////////////////////////////////////

    if (debugShadows) {
        return;
    }

    if (! GLCaps::supports_GL_ARB_shadow() && (app->lighting->shadowedLightArray.size() > 0)) {
        // We're not going to be able to draw shadows, so move the shadowed lights into
        // the unshadowed category.
        app->lighting->lightArray.append(app->lighting->shadowedLightArray);
        app->lighting->shadowedLightArray.clear();
    }

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
        // Opaque unshadowed
        for (int m = 0; m < opaque.size(); ++m) {
            opaque[m]->renderNonShadowed(app->renderDevice, app->lighting);
        }

        // Opaque shadowed
        if (app->lighting->shadowedLightArray.size() > 0) {
            for (int m = 0; m < opaque.size(); ++m) {
                opaque[m]->renderShadowMappedLightPass(app->renderDevice, app->lighting->shadowedLightArray[0], lightMVP, shadowMap);
            }
        }

        // Transparent + shadowed
        for (int m = 0; m < transparent.size(); ++m) {
            transparent[m]->renderNonShadowed(app->renderDevice, app->lighting);
            if (app->lighting->shadowedLightArray.size() > 0) {
                transparent[m]->renderShadowMappedLightPass(app->renderDevice, app->lighting->shadowedLightArray[0], lightMVP, shadowMap);
            }
        }

    app->renderDevice->popState();


    if (app->sky.notNull()) {
        app->sky->renderLensFlare(lighting);
    }

    app->debugPrintf("%s Profile\n", toString(ArticulatedModel::profile()));
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    const std::string path = "";

    double x = -5;
/*
    {
        ArticulatedModelRef model = ArticulatedModel::fromFile("demo/sphere.ifs", 1);

        SuperShader::Material& material = model->partArray[0].triListArray[0].material;
        model->partArray[0].triListArray[0].twoSided = true;
        material.diffuse = Color3::yellow() * .7;
        material.transmit = Color3(.5,.3,.3);
        material.reflect = Color3::white() * .1;
        material.specular = Color3::white() * .8;
        material.specularExponent = Color3::white() * 40;
        model->updateAll();

        entityArray.append(Entity::create(model, CoordinateFrame(Vector3(x,0,0))));
        x += 2;
    }

    
    if (false) {
        ArticulatedModelRef model = ArticulatedModel::fromFile("d:/users/morgan/projects/3ds/fs/fs.3ds", 1);
        entityArray.append(Entity::create(model, CoordinateFrame(Vector3(x,0,0))));
        x += 2;
    }

    if (false) {
        ArticulatedModelRef model = ArticulatedModel::fromFile("3ds/55-porsche/55porsmx.3ds", .25);
        entityArray.append(Entity::create(model, CoordinateFrame(Vector3(x,0,0))));
        x += 2;
    }

    const Matrix3 rot180 = Matrix3::fromAxisAngle(Vector3::unitY(), toRadians(180));

    if (true) {
        CoordinateFrame xform;

        xform.rotation[0][0] = xform.rotation[1][1] = xform.rotation[2][2] = 0.04;

        xform.rotation = xform.rotation * rot180;
        xform.translation = Vector3(-0.35, -1.45, 2.25);
        ArticulatedModelRef model = ArticulatedModel::fromFile("demo/legocar.3ds", xform);
        entityArray.append(Entity::create(model, CoordinateFrame(rot180, Vector3(x,0,0))));
        x += 2;
    }

    {
        ArticulatedModelRef model = ArticulatedModel::fromFile("demo/jackolantern.ifs", 1);

        SuperShader::Material& material = model->partArray[0].triListArray[0].material;
//        model->partArray[0].triListArray[0].twoSided = true;
        material.diffuse = Color3::fromARGB(0xF28900);
        material.transmit = Color3::black();
        material.reflect = Color3::black();
        material.specular = Color3::white() * .3;
        material.specularExponent = Color3::white() * 60;
        model->updateAll();

        entityArray.append(Entity::create(model, CoordinateFrame(rot180, Vector3(x,0,0))));
        x += 2;
    }

    {
        ArticulatedModelRef model = ArticulatedModel::fromFile("demo/teapot.ifs", 1.5);

        Color3 brass = Color3::fromARGB(0xFFFDDC01);

        SuperShader::Material& material = model->partArray[0].triListArray[0].material;
        material.diffuse = brass * .4;
        material.reflect = brass * .5;
        material.specular = Color3::white() * .8;
        material.specularExponent = Color3::white() * 25;
        model->updateAll();

        CoordinateFrame cframe(Vector3(x,-.25,0));
        cframe.lookAt(Vector3(5,0,5));
        entityArray.append(Entity::create(model, cframe));
        x += 2;
    }

    {
        CoordinateFrame xform;

        xform.rotation[0][0] = xform.rotation[1][1] = xform.rotation[2][2] = 0.004;
        xform.translation.x = 3.5;
        xform.translation.y = 1;
        ArticulatedModelRef model = ArticulatedModel::fromFile("demo/imperial.3ds", xform);

        entityArray.append(Entity::create(model, CoordinateFrame(rot180, Vector3(x - 2, 1, 0))));
    }

    if (false){
        ArticulatedModelRef model = ArticulatedModel::fromFile(path + "ifs/mech-part.ifs", 1);

        SuperShader::Material& material = model->partArray[0].triListArray[0].material;
        material.diffuse = Color3::red();
        material.reflect = Color3::black();
        material.specular = Color3::white();
        material.specularExponent = Color3::white() * 30;
        model->updateAll();

        entityArray.append(Entity::create(model, CoordinateFrame(Vector3(x,3,0))));
//        x += 2;
    }

    if (true) {
        ArticulatedModelRef model = ArticulatedModel::createEmpty();

        model->name = "Ground Plane";
        ArticulatedModel::Part& part = model->partArray.next();
        part.cframe = CoordinateFrame();
        part.name = "root";
    
        const double S = 10.0;
        part.geometry.vertexArray.append(
            Vector3(-S, 0, -S),
            Vector3(-S, 0, S),
            Vector3(S, 0, S),
            Vector3(S, 0, -S));

        part.geometry.normalArray.append(
            Vector3::unitY(),
            Vector3::unitY(),
            Vector3::unitY(),
            Vector3::unitY());

        double texScale = 5;
        part.texCoordArray.append(
            Vector2(0,0) * texScale,
            Vector2(0,1) * texScale,
            Vector2(1,1) * texScale,
            Vector2(1,0) * texScale);

        part.tangentArray.append(
            Vector3::unitX(),
            Vector3::unitX(),
            Vector3::unitX(),
            Vector3::unitX());

        ArticulatedModel::Part::TriList& triList = part.triListArray.next();
        triList.indexArray.clear();
        triList.indexArray.append(0, 1, 2);
        triList.indexArray.append(0, 2, 3);

        triList.twoSided = true;
        triList.material.emit.constant = Color3::black();

        triList.material.specular.constant = Color3::black();

        triList.material.diffuse.constant = Color3::white() * 0.8;
        triList.material.diffuse.map = Texture::fromFile("demo/stone.jpg", TextureFormat::AUTO, Texture::TILE);

        GImage normalBumpMap;
        computeNormalMap(GImage("demo/stone-bump.png"), normalBumpMap, false, true);
        triList.material.normalBumpMap =         
            Texture::fromGImage("Bump Map", normalBumpMap, TextureFormat::AUTO, Texture::TILE);

        triList.material.bumpMapScale = 0.04;

        triList.material.specular.constant = Color3::black();
        triList.material.specularExponent.constant = Color3::white() * 60;
        triList.material.reflect.constant = Color3::black();

        triList.computeBounds(part);

        part.indexArray = triList.indexArray;

        part.computeIndexArray();
        part.updateVAR();
        part.updateShaders();

        entityArray.append(Entity::create(model, CoordinateFrame(Vector3(0,-1,0))));
    }
 */
     if (true) {
        ArticulatedModelRef model = ArticulatedModel::createEmpty();

        model->name = "Stained Glass";
        ArticulatedModel::Part& part = model->partArray.next();
        part.cframe = CoordinateFrame();
        part.name = "root";
    
        const double S = 1.0;
        part.geometry.vertexArray.append(
            Vector3(-S,  S, 0),
            Vector3(-S, -S, 0),
            Vector3( S, -S, 0),
            Vector3( S,  S, 0));

        part.geometry.normalArray.append(
            -Vector3::unitZ(),
            -Vector3::unitZ(),
            -Vector3::unitZ(),
            -Vector3::unitZ());

        double texScale = 1;
        part.texCoordArray.append(
            Vector2(0,0) * texScale,
            Vector2(0,1) * texScale,
            Vector2(1,1) * texScale,
            Vector2(1,0) * texScale);

        part.tangentArray.append(
            Vector3::unitX(),
            Vector3::unitX(),
            Vector3::unitX(),
            Vector3::unitX());

        ArticulatedModel::Part::TriList& triList = part.triListArray.next();
        triList.indexArray.clear();
        triList.indexArray.append(0, 1, 2);
        triList.indexArray.append(0, 2, 3);

        triList.twoSided = true;
        triList.material.emit.constant = Color3::black();

        triList.material.diffuse.constant = Color3::white() * 0.2;

        GImage normalBumpMap;
        computeNormalMap(GImage("demo/stained-glass-bump.png"), normalBumpMap, false, true);
        triList.material.normalBumpMap =         
            Texture::fromGImage("Bump Map", normalBumpMap, TextureFormat::AUTO, Texture::CLAMP);

        triList.material.bumpMapScale = 0.02;

        triList.material.specular.constant = Color3::white() * 0.4;
        triList.material.specular.map = Texture::fromFile("demo/stained-glass-mask.png", TextureFormat::AUTO, Texture::CLAMP);
        triList.material.specularExponent.constant = Color3::white() * 60;

        triList.material.reflect.constant = Color3::white() * 0.2;
        triList.material.reflect.map = Texture::fromFile("demo/stained-glass-mask.png", TextureFormat::AUTO, Texture::CLAMP);

        triList.material.transmit.constant = Color3::white();
        triList.material.transmit.map = Texture::fromFile("demo/stained-glass-transmit.png", TextureFormat::AUTO, Texture::CLAMP);

        triList.computeBounds(part);

        part.indexArray = triList.indexArray;

        part.computeIndexArray();
        part.updateVAR();
        part.updateShaders();

        entityArray.append(Entity::create(model, CoordinateFrame(Vector3(x,0,0))));
        x += 2;
    }

//		"contrib/ArticulatedModel/3ds/f16/f16b.3ds"
//		"contrib/ArticulatedModel/3ds/cube.3ds"
//		"contrib/ArticulatedModel/3ds/jeep/jeep.3ds", 0.1
//		"contrib/ArticulatedModel/3ds/house/house.3ds", 0.01
//		"contrib/ArticulatedModel/3ds/delorean/delorean.3ds", 0.1
//		"contrib/ArticulatedModel/3ds/car35/car35.3ds", 0.1
//		"d:/users/morgan/projects/3ds/fs/fs.3ds"

    lighting = Lighting::create();
    {
        LightingParameters params(G3D::toSeconds(12, 00, 00, PM));
    
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

        lighting->shadowedLightArray.clear();

        lighting->shadowedLightArray.append(params.directionalLight());

        // Decrease the blue since we're adding blue ambient
        lighting->shadowedLightArray.last().color *= Color3(1.2, 1.2, 1);
    }

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
    settings.window.fsaaSamples = 4;
    settings.window.width = 800;
    settings.window.height = 600;
	settings.useNetwork = false;
    App(settings).run();
    return 0;
}
