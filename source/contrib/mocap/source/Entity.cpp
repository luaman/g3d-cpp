#include "Entity.h"
#include "ASFModel.h"
#include "odeHelper.h"

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

        if (physics.linearVelocity.squaredLength() > 0.001) {
            Draw::ray(Ray::fromOriginAndDirection(frame.translation, physics.linearVelocity), rd, Color3::red());
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
            Plane plane = physics.g3dGeometry->plane();
            // Take the plane to world space
            plane = cframe().toWorldSpace(plane);

            plane.getEquation(a, b, c, d);
            physics.odeGeometry = dCreatePlane(space, a, b, c, -d);
            physics.canMove = false;
        }
        break;

    case Shape::BOX:
        {
            const AABox& box = physics.g3dGeometry->box();
            physics.odeGeometry = dCreateBox(space, box.extent().x, box.extent().y, box.extent().z);
            debugAssert(box.center().fuzzyEq(Vector3::zero()));

            if (physics.canMove) {
                // Create the body
                physics.body = dBodyCreate(world);
                dBodySetPositionAndRotation(physics.body, cframe());
                dBodySetLinearVel(physics.body, physics.linearVelocity);
                dBodySetAngularVel(physics.body, physics.angularVelocity);

                // Attach a moment of inertia to the body
                dMassSetBox(&physics.odeMass, 1, box.extent().x, box.extent().y, box.extent().z);
                dMassAdjust(&physics.odeMass, physics.mass);
                dBodySetMass(physics.body, &physics.odeMass);

                // Attach geometry to the body
                dGeomSetBody(physics.odeGeometry, physics.body);
            } else {
                dGeomSetPositionAndRotation(physics.odeGeometry, cframe());
            }
        }
        break;

    case Shape::SPHERE:
        {
            // Attach a moment of inertia to the body
            const Sphere& sphere = physics.g3dGeometry->sphere();
            debugAssert(sphere.center == Vector3::zero());

            physics.odeGeometry = dCreateSphere(space, sphere.radius);

            if (physics.canMove) {
                // Create the body
                physics.body = dBodyCreate(world);
                dBodySetPositionAndRotation(physics.body, cframe());
                dBodySetLinearVel(physics.body, physics.linearVelocity);
                dBodySetAngularVel(physics.body, physics.angularVelocity);

                dMassSetSphere(&physics.odeMass, 1, sphere.radius);
                dMassAdjust(&physics.odeMass, physics.mass);
                dBodySetMass(physics.body, &physics.odeMass);

                // Attach geometry to the body
                dGeomSetBody(physics.odeGeometry, physics.body);
            } else {
                dGeomSetPositionAndRotation(physics.odeGeometry, cframe());
            }
        }
        break;

    case Shape::CAPSULE:
        {
            const Capsule& capsule = physics.g3dGeometry->capsule();
            debugAssert(capsule.getPoint1().x == 0);
            debugAssert(capsule.getPoint1().z == 0);
            debugAssert(capsule.getPoint2().x == 0);
            debugAssert(capsule.getPoint2().z == 0);
            debugAssert(fuzzyEq(capsule.getPoint1().y, -capsule.getPoint2().y));

            float r = capsule.getRadius();
            float h = abs(capsule.getPoint2().y - capsule.getPoint1().y);

            dGeomID geom = dCreateCCylinder(ODE_NO_SPACE, r, h);

            // Standard ODE -> G3D coordinate system switch
            static CoordinateFrame orient(Matrix3::fromAxisAngle(Vector3::unitX(), toRadians(90)), Vector3::zero());

            // TODO: we could allow arbitrary transformation here to match any capsule
            // Orient to match the G3D coordinate system
            dGeomSetPositionAndRotation(geom, orient);

            // We always create a transform geom to wrap the underlying
            // geom.  This allows arbitrary orientation relative to body 
            // space.
            physics.odeGeometry = dCreateGeomTransform(space);
            dGeomTransformSetInfo(physics.odeGeometry, 1);
            dGeomTransformSetGeom(physics.odeGeometry, geom);


            if (physics.canMove) {
                // Create the body
                physics.body = dBodyCreate(world);
                dBodySetPositionAndRotation(physics.body, cframe());
                dBodySetLinearVel(physics.body, physics.linearVelocity);
                dBodySetAngularVel(physics.body, physics.angularVelocity);

                // Attach a moment of inertia to the body
                dMassSetCylinder(&physics.odeMass, 1, 0, r, h);

                dMassAdjust(&physics.odeMass, physics.mass);
                dBodySetMass(physics.body, &physics.odeMass);

                // Attach geometry to the body
                dGeomSetBody(physics.odeGeometry, physics.body);
            } else {
                dGeomSetPositionAndRotation(physics.odeGeometry, cframe());
            }
        }
        break;

    case Shape::CYLINDER:
        {
            alwaysAssertM(false, "Cylinders not supported in this release.  Use Capsule instead")
#if 0
            const Cylinder& cylinder = physics.g3dGeometry->cylinder();
            debugAssert(cylinder.getPoint1().x == 0);
            debugAssert(cylinder.getPoint1().z == 0);
            debugAssert(cylinder.getPoint2().x == 0);
            debugAssert(cylinder.getPoint2().z == 0);
            debugAssert(fuzzyEq(cylinder.getPoint1().y, -cylinder.getPoint2().y));

            float r = cylinder.getRadius();
            float h = abs(cylinder.getPoint2().y - cylinder.getPoint1().y);

            physics.odeGeometry = dCreateCylinder(space, r, h);

            if (physics.canMove) {
                // Create the body
                physics.body = dBodyCreate(world);
                dBodySetPositionAndRotation(physics.body, cframe());
                dBodySetLinearVel(physics.body, physics.linearVelocity);
                dBodySetAngularVel(physics.body, physics.angularVelocity);

                // Attach a moment of inertia to the body
                dMassSetCylinder(&physics.odeMass, 1, 0, r, h);

                dMassAdjust(&physics.odeMass, physics.mass);
                dBodySetMass(physics.body, &physics.odeMass);

                // Attach geometry to the body
                dGeomSetBody(physics.odeGeometry, physics.body);
            } else {
                dGeomSetPositionAndRotation(physics.odeGeometry, cframe());
            }
#endif
        }
        break;

    default:
        debugAssertM(false, "This shape is not supported by Entity.");

    }

    if (physics.canMove) {
        // Insert into space
//        dSpaceAdd(space, physics.odeGeometry);
    }
    
}

//////////////////////////////////////////

Entity::Physics::Physics() : 
    g3dGeometry(NULL), 
    odeGeometry(0), 
    mass(1), 
    linearVelocity(Vector3::zero()), 
    angularVelocity(Vector3::zero()), 
    canMove(true) {}


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
    dBodyGetPositionAndRotation(body, c);
}

void Entity::Physics::updateVelocity() {
    dBodyGetLinearAndAngularVel(body, linearVelocity, angularVelocity);
}
