/**
 @file Plane.cpp
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-02-06
 @edited  2003-02-15
 */

#include "../include/G3D/Plane.h"
#include "../include/G3D/BinaryOutput.h"
#include "../include/G3D/BinaryInput.h"
#include "../include/G3D/stringutils.h"

namespace G3D {

Plane::Plane(class BinaryInput& b) {
	deserialize(b);
}


void Plane::serialize(class BinaryOutput& b) const {
	normal.serialize(b);
	b.writeFloat64(distance);
}


void Plane::deserialize(class BinaryInput& b) {
	normal.deserialize(b);
	distance = b.readFloat64();
}


Plane::Plane(
    const Vector3&      point0,
    const Vector3&      point1,
    const Vector3&      point2) {

    normal   = (point1 - point0).cross(point2 - point0).direction();
    distance = normal.dot(point0);
}


Plane::Plane(
    const Vector3&      _normal,
    const Vector3&      point) {

    normal   = _normal.direction();
    distance = normal.dot(point);
}


void Plane::flip() {
    normal   = -normal;
    distance = -distance;
}


void Plane::getEquation(Vector3 &normal, Real& d) const {
    normal = this->normal;
    d = -this->distance;
}


void Plane::getEquation(Real& a, Real& b, Real& c, Real& d) const {
    a = normal.x;
    b = normal.y;
    c = normal.z;
    d = -distance;
}


std::string Plane::toString() const {
    return format("Plane(%g, %g, %g, %g)", normal.x, normal.y, normal.z, distance);
}

}
