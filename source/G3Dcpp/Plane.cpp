/**
 @file Plane.cpp
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-02-06
 @edited  2004-07-09
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
    Vector4      point0,
    Vector4      point1,
    Vector4      point2) {

    debugAssertM(
        point0.w != 0 || 
        point1.w != 0 || 
        point2.w != 0,
        "At least one point must be finite.");

    // Rotate the points around so that the finite points come first.

    while ((point0.w == 0) && 
           ((point1.w == 0) || (point2.w != 0))) {
        Vector4 temp = point0;
        point0 = point1;
        point1 = point2;
        point2 = temp;
    }

    Vector3 dir1;
    Vector3 dir2;

    if (point1.w == 0) {
        // 1 finite, 2 infinite points; the plane must contain
        // the direction of the two direcitons
        dir1 = point1.xyz();
        dir2 = point2.xyz();
    } else if (point2.w != 0) {
        // 3 finite points, the plane must contain the directions
        // betwseen the points.
        dir1 = point1.xyz() - point0.xyz();
        dir2 = point2.xyz() - point0.xyz();
    } else {
        // 2 finite, 1 infinite point; the plane must contain
        // the direction between the first two points and the
        // direction of the third point.
        dir1 = point1.xyz() - point0.xyz();
        dir2 = point2.xyz();
    }

    _normal   = dir1.cross(dir2).direction();
    distance = _normal.dot(point0.xyz());
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
    double magnitude = n.magnitude();
    d /= magnitude;
    n /= magnitude;
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
