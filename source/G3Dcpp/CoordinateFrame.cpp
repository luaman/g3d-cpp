/**
 @file CoordinateFrame.cpp

 Coordinate frame class

 Morgan McGuire and Laura Wollstadt, graphics3d.com

 @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com

 @created 2001-06-02
 @edited  2003-02-06
*/

#include "G3D/CoordinateFrame.h"
#include "G3D/Quat.h"

namespace G3D {

const Real CoordinateFrame::zLookDirection = -1;


Box CoordinateFrame::toWorldSpace(const Box &b) const {
    Box out;

    for (int i = 0; i < 8; i++) {
        out.corner[i] = rotation * b.corner[i] + translation;
    }

    return out;
}


Box CoordinateFrame::toObjectSpace(const Box &b) const {
    return inverse().toWorldSpace(b);
}


CoordinateFrame::CoordinateFrame(class BinaryInput& b) : rotation(Matrix3::ZERO) {
    deserialize(b);
}


void CoordinateFrame::deserialize(class BinaryInput& b) {
    rotation.deserialize(b);
    translation.deserialize(b);
}


void CoordinateFrame::serialize(class BinaryOutput& b) const {
    rotation.serialize(b);
    translation.serialize(b);
}


Sphere CoordinateFrame::toWorldSpace(const Sphere &b) const {
    return Sphere(pointToWorldSpace(b.center), b.radius);
}


Sphere CoordinateFrame::toObjectSpace(const Sphere &b) const {
    return Sphere(pointToObjectSpace(b.center), b.radius);
}


Ray CoordinateFrame::toWorldSpace(const Ray& r) const {
    return Ray::fromOriginAndDirection(pointToWorldSpace(r.origin), vectorToWorldSpace(r.direction));
}


Ray CoordinateFrame::toObjectSpace(const Ray& r) const {
    return Ray::fromOriginAndDirection(pointToObjectSpace(r.origin), vectorToObjectSpace(r.direction));
}


void CoordinateFrame::lookAt(const Vector3 &target) {
    lookAt(target, Vector3::UNIT_Y);
}


void CoordinateFrame::lookAt(
    const Vector3&  target,
    Vector3         up) {

    up = up.direction();

    Vector3 look = target - translation;
    look.unitize();

    Vector3 z = -look;
    Vector3 x = -z.cross(up);
    x.unitize();

    Vector3 y = z.cross(x);

    rotation.setColumn(0, x);
    rotation.setColumn(1, y);
    rotation.setColumn(2, z);
}


CoordinateFrame CoordinateFrame::lerp(
    double                  alpha,
    const CoordinateFrame&  other) const {

    Quat q1 = Quat(this->rotation);
    Quat q2 = Quat(other.rotation);

    return CoordinateFrame(
        q1.lerp(alpha, q2).toRotationMatrix(),
        this->translation * (1 - alpha) + other.translation * alpha);
} 


}; // namespace
