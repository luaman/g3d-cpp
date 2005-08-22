#include "App.h"
#include "World.h"

void World::init() {
    renderMode = RENDER_NORMAL;
}


World::~World() {
    cleanup();
}


void World::cleanup() {
    entityArray.clear();
    physics.simArray.clear();

    dJointGroupDestroy(physics.contactGroup);
    dSpaceDestroy(physics.spaceID);
    dWorldDestroy(physics.ID);
}


void World::insert(EntityRef& e) {
    if (e->physics.g3dGeometry != NULL) {
        physics.simArray.append(e);
        e->createODEGeometry(physics.ID, physics.spaceID);
    }

    
    entityArray.append(e);
}


void World::ODENearCallback(void* data, dGeomID o1, dGeomID o2) {
    int i, n;
    
    World* world = (World*)data;
    
    // Return without doing anything if we want to ignore this contact.
    
    const int maxContacts = 10;
    dContact contact[maxContacts];
    n = dCollide (o1, o2, maxContacts, &contact[0].geom, sizeof(dContact));
    if (n > 0) {
        for (i = 0; i < n; ++i) {
            contact[i].surface.mode = dContactSlip1 | dContactSlip2 |
                dContactSoftERP | dContactSoftCFM | dContactApprox1;
            
            contact[i].surface.mu = dInfinity;
            contact[i].surface.slip1 = 0.1;
            contact[i].surface.slip2 = 0.1;
            contact[i].surface.soft_erp = 0.5;
            contact[i].surface.soft_cfm = 0.3;
            
            dJointID c = dJointCreateContact(world->physics.ID, world->physics.contactGroup, &contact[i]);

            dJointAttach(c,
                dGeomGetBody(contact[i].geom.g1),
                dGeomGetBody(contact[i].geom.g2));
        }
    }
}


void World::doSimulation() {

    // Do physics on the models
    dSpaceCollide(physics.spaceID, this, &ODENearCallback);

    // TODO: is 0.05 the timestep?
    dWorldStep(physics.ID, 0.05);

    // Remove all contact joints
    dJointGroupEmpty(physics.contactGroup);

    // Update from simulated data
    CoordinateFrame c;
    for (int s = 0; s < physics.simArray.size(); ++s) {
        EntityRef entity = physics.simArray[s];
        if (entity->physics.canMove) {
            entity->physics.updateVelocity();

            entity->physics.getFrame(c);
            entity->frame = c;
        }
    }
    
}


static bool entitySortBackToFront(const EntityRef& a, const EntityRef& b) {
    return a->sortKey > b->sortKey;
}


static void sortBackToFront(const CoordinateFrame& camera, Array<EntityRef>& array) {
    Vector3 z = camera.getLookVector();

    // Create keys
    for (int e = 0; e < array.size(); ++e) {
        if (array[e]->physics.g3dGeometry->type() == Shape::PLANE) {
            array[e]->sortKey = inf();
            // Draw planes first
        } else {
            array[e]->sortKey = array[e]->frame.translation.dot(z);
        }
    }

    array.sort(entitySortBackToFront);
}


void World::renderPhysicsModels(RenderDevice* rd) {
    sortBackToFront(rd->getCameraToWorldMatrix(), physics.simArray);

    rd->pushState();
        rd->setPolygonOffset(-1);
        for (int s = 0; s < physics.simArray.size(); ++s) {
            physics.simArray[s]->renderPhysicsModel(rd);
        }
    rd->popState();
}


void World::doGraphics(RenderDevice* rd) {
    LightingRef        lighting      = toneMap.prepareLighting(this->lighting);
    LightingParameters skyParameters = toneMap.prepareLightingParameters(this->skyParameters);

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

        switch (renderMode) {
        case RENDER_NORMAL:
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
            break;

        case RENDER_PHYSICS:
            Draw::axes(CoordinateFrame(Vector3(0, 0, 0)), rd);
            renderPhysicsModels(rd);
            break;
        }

    rd->popState();

    toneMap.apply(app->renderDevice);

//    for (int e = 0; e < entityArray.size(); ++e) {
//        entityArray[e]->drawLabels(rd);
//    }

    if (sky.notNull()) {
        sky->renderLensFlare(rd, skyParameters);
    }

    app->debugPrintf("Tone Map %s\n", toneMap.enabled() ? "On" : "Off");
    app->debugPrintf("%s Profile %s\n", toString(ArticulatedModel::profile()),
        #ifdef _DEBUG
                "(DEBUG mode)"
        #else
                ""
        #endif
        );
}


void World::setGravity(const Vector3& g) {
    physics.gravity = g;
    dWorldSetGravity(physics.ID, physics.gravity.x, physics.gravity.y, physics.gravity.z);
}

///////////////////////////////////////////////////////

World::Physics::Physics() {
    ID = dWorldCreate();
    spaceID = dHashSpaceCreate(0);

    contactGroup = dJointGroupCreate(0);

    gravity = Vector3(0, -0.5, 0);
    dWorldSetGravity(ID, gravity.x, gravity.y, gravity.z);
}


