/**
 @file Ray.cpp 
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2002-07-12
 @edited  2004-01-12
 */

#include "G3D/Ray.h"
#include "G3D/Plane.h"
#include "G3D/Sphere.h"
#include "G3D/CollisionDetection.h"

namespace G3D {

Ray::Ray(class BinaryInput& b) {
	deserialize(b);
}


void Ray::serialize(class BinaryOutput& b) const {
	origin.serialize(b);
	direction.serialize(b);
}


void Ray::deserialize(class BinaryInput& b) {
	origin.deserialize(b);
	direction.deserialize(b);
}


Vector3 Ray::intersection(const Plane& plane) const {
    float d;
    Vector3 normal = plane.normal();
    plane.getEquation(normal, d);
    double rate = direction.dot(normal);

    if (rate <= 0) {
        return Vector3::INF3;
    } else {
        double t = -(d + origin.dot(normal)) / rate;

        return origin + direction * t;
    }
}


float Ray::intersectionTime(const class Sphere& sphere) const {
    Vector3 dummy;
    return CollisionDetection::collisionTimeForMovingPointFixedSphere(
            origin, direction, sphere, dummy);
}


float Ray::intersectionTime(const class Plane& plane) const {
    Vector3 dummy;
    return CollisionDetection::collisionTimeForMovingPointFixedPlane(
            origin, direction, plane, dummy);
}


float Ray::intersectionTime(const class Box& box) const {
    Vector3 dummy;
    return CollisionDetection::collisionTimeForMovingPointFixedBox(
            origin, direction, box, dummy);
}


float Ray::intersectionTime(
    const Vector3& v0,
    const Vector3& v1,
    const Vector3& v2) const {

    return intersectionTime(Triangle(v0, v1, v2));
}


float Ray::intersectionTime(
    const Triangle& triangle) const {

    Vector3 dummy;
    return CollisionDetection::collisionTimeForMovingPointFixedTriangle(
            origin, direction, triangle, dummy);
}

}
