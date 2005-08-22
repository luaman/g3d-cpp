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
        static const Color4 staticColor(0.5, 0.5, 0.5, 0.5);
        static const Color4 dynamicColor(1, 1, 0, 0.5);

        physics.g3dGeometry->render(rd, cframe(), physics.canMove ? dynamicColor : staticColor);

        if (physics.velocity.squaredLength() > 0.001) {
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
            Plane plane = physics.g3dGeometry->asPlane()->geometry;
            // Take the plane to world space
            plane = cframe().toWorldSpace(plane);

            plane.getEquation(a, b, c, d);
            physics.odeGeometry = dCreatePlane(space, a, b, c, -d);
            physics.canMove = false;
        }
        break;

    case Shape::BOX:
        if (physics.canMove) {
            // Create the body
            physics.body = dBodyCreate(world);
            dBodySetPosition(physics.body, frame.translation.x, frame.translation.y, frame.translation.z);

            // Attach a moment of inertia to the body
            const AABox& box = physics.g3dGeometry->asBox()->geometry;
            dMassSetBox(&physics.odeMass, 1, box.extent().x, box.extent().y, box.extent().z);
            dMassAdjust(&physics.odeMass, physics.mass);
            dBodySetMass(physics.body, &physics.odeMass);

            // Attach geometry to the body
            physics.odeGeometry = dCreateBox(space, box.extent().x, box.extent().y, box.extent().z);
            dGeomSetBody(physics.odeGeometry, physics.body);
        } else {
            const AABox& box = physics.g3dGeometry->asBox()->geometry;
            physics.odeGeometry = dCreateBox(space, box.extent().x, box.extent().y, box.extent().z);
            Matrix3 g3dR = cframe().translation;
            dMatrix3 odeR;
         
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    odeR[i * 4 + j] = g3dR[i][j];
                }
                odeR[i * 4 + 3] = 0.0;
            }
            
            Vector3 c = frame.translation + box.center();

            dGeomSetPosition(physics.odeGeometry, c.x, c.y, c.z);
            dGeomSetRotation(physics.odeGeometry, odeR);
        }
        break;

    case Shape::SPHERE:
        if (physics.canMove) {
            // Create the body
            physics.body = dBodyCreate(world);
            dBodySetPosition(physics.body, frame.translation.x, frame.translation.y, frame.translation.z);

            // Attach a moment of inertia to the body
            const Sphere& sphere = physics.g3dGeometry->asSphere()->geometry;
            debugAssert(sphere.center == Vector3::zero());
            dMassSetSphere(&physics.odeMass, 1, sphereradius);
            dMassAdjust(&physics.odeMass, physics.mass);
            dBodySetMass(physics.body, &physics.odeMass);

            // Attach geometry to the body
            physics.odeGeometry = dCreateSphere(space, sphere.radius);
            dGeomSetBody(physics.odeGeometry, physics.body);
        }
        break;

    case Shape::CYLINDER:
        break;

    default:
        debugAssertM(false, "unimplemented");

    }

    if (physics.canMove) {
        // Insert into space
//        dSpaceAdd(space, physics.odeGeometry);
    }
    
}

//////////////////////////////////////////

Entity::Physics::Physics() : g3dGeometry(NULL), odeGeometry(0), mass(1), velocity(Vector3::zero()), canMove(true) {}

Entity::Physics::~Physics() {
    dGeomDestroy(odeGeometry);
    delete g3dGeometry;
}

/*
void Entity::Physics::setFrame(const CoordinateFrame& c) {
    dMatrix3 R;
    dRFromAxisAndAngle (R,0,1,0,-0.15);
    dGeomSetPosition(ground_box,2,0,-0.34);
    dGeomSetRotation(ground_box,R);
}
*/

void Entity::Physics::getFrame(CoordinateFrame& c) {

    // dReal may be either single or double
    const dReal* t = dBodyGetPosition(body);
    const dReal* r = dBodyGetRotation(body);    

    for (int i = 0; i < 3; ++i) {
        c.translation[i] = t[i];
        for (int j = 0; j < 3; ++j) {
            c.rotation[i][j] = r[i * 4 + j];
        }
    }
}

void Entity::Physics::updateVelocity() {

    // dReal may be either single or double
    const dReal* t = dBodyGetLinearVel(body);
    const dReal* r = dBodyGetAngularVel(body);    

    for (int i = 0; i < 3; ++i) {
        velocity[i] = t[i];
        angularVelocity[i] = r[i];
    }
}
