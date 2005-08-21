#include "App.h"
#include "World.h"
#include "AMUtil.h"


void World::init() {
    sky = Sky::create(NULL, app->dataDir + "sky/");

    lighting = Lighting::create();
    {
        skyParameters = LightingParameters(G3D::toSeconds(10, 00, 00, AM));
    
        skyParameters.skyAmbient = Color3::white();

        if (sky.notNull()) {
            lighting->environmentMap = sky->getEnvironmentMap();
            lighting->environmentMapColor = skyParameters.skyAmbient;
        } else {
            lighting->environmentMapColor = Color3::black();
        }

        lighting->ambientTop = Color3(0.6, 0.6, 1.0) * skyParameters.diffuseAmbient;
        lighting->ambientBottom = Color3::white() * .6 * skyParameters.diffuseAmbient;

        lighting->emissiveScale = skyParameters.emissiveScale;

        lighting->lightArray.clear();

        lighting->shadowedLightArray.clear();

        GLight L = skyParameters.directionalLight();
        // Decrease the blue since we're adding blue ambient
        L.color *= Color3(1.2, 1.2, 1);
        L.position = Vector4(Vector3(0,1,1).direction(), 0);

        lighting->shadowedLightArray.append(L);
    }

    // Ground plane
    entityArray.append(Entity::create(createPlaneModel("grid.png", 20, 1), CoordinateFrame(Vector3(-5, -1, 5))));

    entityArray.append(Entity::create(ASFModel::create("26.asf"), 
        CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitY(), toRadians(180)), Vector3::zero())));

    entityArray.append(Entity::create(createIFSModel("cube.ifs"), CoordinateFrame(Vector3(-3,0,0)))); 
    entityArray.append(Entity::create(createIFSModel("sphere.ifs", Color3::cyan()), CoordinateFrame(Vector3(-6,0,0)))); 

}



void World::render(RenderDevice* rd) {
//    LightingRef        lighting      = toneMap.prepareLighting(lighting);
//    LightingParameters skyParameters = toneMap.prepareLightingParameters(skyParameters);

    // Pose all
    Array<PosedModelRef> posedModels;

    for (int e = 0; e < entityArray.size(); ++e) {
        entityArray[e]->pose(posedModels);
    }
    Array<PosedModelRef> opaque, transparent;
    PosedModel::sort(posedModels, app->debugCamera.getCoordinateFrame().lookVector(), opaque, transparent);

    bool shadows = false;
    if (!shadows && (lighting->shadowedLightArray.size() > 0)) {
        // We're not going to be able to draw shadows, so move the shadowed lights into
        // the unshadowed category.
        lighting->lightArray.append(lighting->shadowedLightArray);
        lighting->shadowedLightArray.clear();
    }

    rd->setProjectionAndCameraMatrix(app->debugCamera);
    rd->setObjectToWorldMatrix(CoordinateFrame());

    app->debugPrintf("%d opaque, %d transparent\n", opaque.size(), transparent.size());

    // Cyan background
    rd->setColorClearValue((lighting->ambientTop + lighting->ambientBottom) / 2);

    rd->clear(sky.notNull(), true, true);
    if (sky.notNull()) {
        sky->render(rd, skyParameters);
    }

    rd->pushState();
//		Draw::axes(CoordinateFrame(Vector3(0, 0, 0)), rd);

        
        //rd->setRenderMode(RenderDevice::RENDER_WIREFRAME);

        // Opaque unshadowed
        for (int m = 0; m < opaque.size(); ++m) {
            opaque[m]->renderNonShadowed(rd, lighting);
        }

        // Opaque shadowed
        if (lighting->shadowedLightArray.size() > 0) {
            for (int m = 0; m < opaque.size(); ++m) {
            //    opaque[m]->renderShadowMappedLightPass(rd, lighting->shadowedLightArray[0], lightMVP, shadowMap);
            }
        }

        // Transparent
        for (int m = 0; m < transparent.size(); ++m) {
            transparent[m]->renderNonShadowed(rd, lighting);
            if (lighting->shadowedLightArray.size() > 0) {
            //    transparent[m]->renderShadowMappedLightPass(rd, lighting->shadowedLightArray[0], lightMVP, shadowMap);
            }
        }

    rd->popState();

    //toneMap.apply(app->renderDevice);

//    for (int e = 0; e < entityArray.size(); ++e) {
//        entityArray[e]->drawLabels(rd);
//    }

    if (sky.notNull()) {
        sky->renderLensFlare(rd, skyParameters);
    }

    //app->debugPrintf("Tone Map %s\n", toneMap.enabled() ? "On" : "Off");
    app->debugPrintf("%s Profile %s\n", toString(ArticulatedModel::profile()),
        #ifdef _DEBUG
                "(DEBUG mode)"
        #else
                ""
        #endif
        );

}