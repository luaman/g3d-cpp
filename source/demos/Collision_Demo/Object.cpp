/**
 @file Collision_Demo/Object.h

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2003-02-07
 @edited  2003-03-18
 */

#include "Object.h"
#include "Model.h"

extern RenderDevice* renderDevice;

void glSpecular() {
    // Material properties
    static const float white[] = {1,1,1,1};
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 16.0f);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}


void glDiffuse() {
    // Material properties
    static const float black[] = {0,0,0,1};
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}


Object::Object(const Color3& _color) : color(_color) {
}


Object::~Object() {
}


////////////////////////////////////////////////////////////////////////////


GeneralObject::GeneralObject(
    class Model*            _model,
    const CoordinateFrame&  _cframe,
    const Color3&           _color) : Object(_color), model(_model), cframe(_cframe) {
}


void GeneralObject::render() const {
    renderDevice->setColor(color);
    renderDevice->setObjectToWorldMatrix(cframe);
    glSpecular();
    model->render();
}


GameTime GeneralObject::timeUntilCollisionWithMovingSphere(
    const Sphere&       sphere,
    const Vector3&      velocity,
    GameTime            timeLimit,
    Vector3&            outLocation,
    Vector3&            outNormal) const {

    // Transform the sphere to object space
    Sphere  s(cframe.toObjectSpace(sphere));
    Vector3 v(cframe.vectorToObjectSpace(velocity));

    GameTime t = model->timeUntilCollisionWithMovingSphere(s, v, timeLimit, outLocation, outNormal);

    if (t < inf) {
        outLocation = cframe.pointToWorldSpace(outLocation);

        // There is no scaling in the coordinate frame, so we can
        // transform the normal using the vector routine.
        outNormal   = cframe.vectorToWorldSpace(outNormal);
    }

    return t;
}


////////////////////////////////////////////////////////////////////////////


SphereObject::SphereObject(const Sphere& s, const Color3& _color) :
     Object(_color), sphere(s) {
}


GameTime SphereObject::timeUntilCollisionWithMovingSphere(
    const Sphere&       movingSphere,
    const Vector3&      velocity,
    GameTime            timeLimit,
    Vector3&            outLocation,
    Vector3&            outNormal) const {

    GameTime t = CollisionDetection::collisionTimeForMovingSphereFixedSphere(movingSphere, velocity, sphere, outLocation, outNormal);

    if (t > timeLimit) {
        return inf;
    } else {
        return t;
    }
}


void SphereObject::render() const {
    renderDevice->pushState();
    renderDevice->setColor(color);

    renderDevice->setObjectToWorldMatrix(CoordinateFrame(sphere.center));

    glSpecular();
    glutSolidSphere(sphere.radius, (int)(12 * sphere.radius) + 6, (int)(10 * sphere.radius) + 5);  

    renderDevice->popState();
}


////////////////////////////////////////////////////////////////////////////


CapsuleObject::CapsuleObject(const Capsule& c, const Color3& _color) :
     Object(_color), capsule(c) {
}


GameTime CapsuleObject::timeUntilCollisionWithMovingSphere(
    const Sphere&       movingSphere,
    const Vector3&      velocity,
    GameTime            timeLimit,
    Vector3&            outLocation,
    Vector3&            outNormal) const {

    GameTime t = CollisionDetection::collisionTimeForMovingSphereFixedCapsule(movingSphere, velocity, capsule, outLocation, outNormal);

    if (t > timeLimit) {
        return inf;
    } else {
        return t;
    }
}


static void glutSolidCylinder(float r,float h,int n,int m) {
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -h/2);
    GLUquadricObj* qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluCylinder(qobj, r, r, h, n, m);
    gluDeleteQuadric(qobj);  
    glPopMatrix();
}


void CapsuleObject::render() const {
    renderDevice->pushState();
    renderDevice->setColor(color);

    glSpecular();

    double radius = capsule.getRadius();
    renderDevice->setObjectToWorldMatrix(CoordinateFrame(capsule.getPoint1()));
    glutSolidSphere(radius, (int)(10 * radius) + 7, (int)(8 * radius) + 7);
    
    renderDevice->setObjectToWorldMatrix(CoordinateFrame(capsule.getPoint2()));
    glutSolidSphere(radius, (int)(10 * radius) + 7, (int)(8 * radius) + 7);  

    Vector3 axis = capsule.getPoint2() - capsule.getPoint1();
    CoordinateFrame cframe((capsule.getPoint1() + capsule.getPoint2()) / 2);
  
    if (abs(axis.direction().dot(Vector3::UNIT_Y)) > 0.8) {
        cframe.lookAt(capsule.getPoint2(), Vector3::UNIT_X);
    } else {
        cframe.lookAt(capsule.getPoint2());
    }
    renderDevice->setObjectToWorldMatrix(cframe);
    glutSolidCylinder(radius, axis.length(), (int)(12 * radius) + 5, 2);  

    renderDevice->popState();
}

////////////////////////////////////////////////////////////////////////////


BoxObject::BoxObject(const Box& b, const Color3& _color) : Object(_color), box(b) {
}


GameTime BoxObject::timeUntilCollisionWithMovingSphere(
    const Sphere&       sphere,
    const Vector3&      velocity,
    GameTime            timeLimit,
    Vector3&            outLocation,
    Vector3&            outNormal) const {

    GameTime t = CollisionDetection::collisionTimeForMovingSphereFixedBox(sphere, velocity, box, outLocation, outNormal);

    if (fuzzyLe(t, timeLimit)) {
        return t;
    } else {
        return inf;
    }
}


void BoxObject::render() const {

    // Draw the six faces
    Vector3 v0, v1, v2, v3;

    glDiffuse();
    renderDevice->setColor(color);
    renderDevice->setObjectToWorldMatrix(CoordinateFrame());
    renderDevice->beginPrimitive(RenderDevice::QUADS);
    for (int f = 0; f < 6; ++f) {
        box.getFaceCorners(f, v0, v1, v2, v3);
        renderDevice->setNormal((v1 - v0).cross(v3 - v0).direction());
        renderDevice->sendVertex(v0);
        renderDevice->sendVertex(v1);
        renderDevice->sendVertex(v2);
        renderDevice->sendVertex(v3);
    }
    renderDevice->endPrimitive();
}


////////////////////////////////////////////////////////////////////////////


SimSphere::SimSphere(const Sphere& s, const Vector3& _velocity, const Color3& _color) :
    SphereObject(s, _color), velocity(_velocity) {
}


void SimSphere::render() const {

    SphereObject::render();

    // Render the velocity vector (for debugging purposes)
    // renderDevice->setColor(Color3::RED);
    // renderDevice->debugDrawVector(velocity / 2, sphere.center, 1);
}
