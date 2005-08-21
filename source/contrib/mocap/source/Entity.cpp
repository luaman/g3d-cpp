#include "Entity.h"
#include "ASFModel.h"

Entity::Entity() {}


Entity::~Entity() {
    delete modelData;
}


void Entity::pose(Array<PosedModelRef>& posedModels) {
    modelData->pose(posedModels, cframe());
}


void Entity::drawLabels(RenderDevice* rd) {
    modelData->drawLabels(rd, CoordinateFrame(cframe().rotation, cframe().translation + Vector3(0,0,-.25)));
}


void Entity::renderPhysicsModel(RenderDevice* rd) {
    if (physics.g3dGeometry != NULL) {
        physics.g3dGeometry->render(rd, cframe());

        if (physics.velocity.squaredLength() > 0.1) {
            Draw::ray(Ray::fromOriginAndDirection(frame.translation, physics.velocity), rd, Color3::red());
        }
    }
}


void Entity::createODEGeometry(dWorldID world, dSpaceID space) {
    switch (physics.g3dGeometry->type()) {
    case Shape::PLANE:
        {
            // Plane won't have real physics attached to it, so we just need
            // to make it part of the space.
            float a,b,c,d;
            physics.g3dGeometry->asPlane()->geometry.getEquation(a, b, c, d);
            physics.odeGeometry = dCreatePlane(space, a, b, c, d);
        }
        break;

    case Shape::BOX:
        {
            const AABox& box = physics.g3dGeometry->asBox()->geometry;
            physics.body = dBodyCreate(world);
            dBodySetPosition(physics.body, frame.translation.x, frame.translation.x, frame.translation.y);

            dMassSetBox(&physics.odeMass, 1, box.extent().x, box.extent().y, box.extent().z);
            dMassAdjust(&physics.odeMass, physics.mass);
            dBodySetMass(physics.body, &physics.odeMass);
            physics.odeGeometry = dCreateBox(0, box.extent().x, box.extent().y, box.extent().z);
            dGeomSetBody(physics.odeGeometry, physics.body);
        }
        break;

    default:
        debugAssertM(false, "unimplemented");

    }
}

//////////////////////////////////////////

Entity::Physics::Physics() : g3dGeometry(NULL), odeGeometry(0), mass(1), velocity(Vector3::zero()) {}

Entity::Physics::~Physics() {
    delete g3dGeometry;
}


