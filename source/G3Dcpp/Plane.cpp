/**
 @file Plane.cpp
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-02-06
 @edited  2003-06-13
 */

#include "G3D/Plane.h"
#include "G3D/BinaryOutput.h"
#include "G3D/BinaryInput.h"
#include "G3D/stringutils.h"

namespace G3D {

Plane::Plane(class BinaryInput& b) {
	deserialize(b);
}


void Plane::serialize(class BinaryOutput& b) const {
	_normal.serialize(b);
	b.writeFloat64(distance);
}


void Plane::deserialize(class BinaryInput& b) {
	_normal.deserialize(b);
	distance = b.readFloat64();
}


Plane::Plane(
    const Vector3&      point0,
    const Vector3&      point1,
    const Vector3&      point2) {

    _normal   = (point1 - point0).cross(point2 - point0).direction();
    distance = _normal.dot(point0);
}


Plane::Plane(
    const Vector3&      __normal,
    const Vector3&      point) {

    _normal   = __normal.direction();
    distance  = _normal.dot(point);
}


Plane Plane::fromEquation(double a, double b, double c, double d) {
    Vector3 n(a, b, c);
    double length = n.length();
    d /= length;
    n /= length;
    return Plane(n, -d);
}


void Plane::flip() {
    _normal   = -_normal;
    distance  = -distance;
}


void Plane::getEquation(Vector3& n, float& d) const {
    double _d;
    getEquation(n, _d);
    d = _d;
}

void Plane::getEquation(Vector3& n, double& d) const {
    n = _normal;
    d = -distance;
}


void Plane::getEquation(float& a, float& b, float& c, float& d) const {
    double _a, _b, _c, _d;
    getEquation(_a, _b, _c, _d);
    a = _a;
    b = _b;
    c = _c;
    d = _d;
}

void Plane::getEquation(double& a, double& b, double& c, double& d) const {
    a = _normal.x;
    b = _normal.y;
    c = _normal.z;
    d = -distance;
}


std::string Plane::toString() const {
    return format("Plane(%g, %g, %g, %g)", _normal.x, _normal.y, _normal.z, distance);
}

}
