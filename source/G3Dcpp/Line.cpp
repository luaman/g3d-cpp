/**
  @file Line.cpp
 
  Line class
 
  @maintainer Morgan McGuire, graphics3d.com
 
  @created 2001-06-02
  @edited  2003-05-24
 */

#include "G3D/Line.h"
#include "G3D/Plane.h"

namespace G3D {

Vector3 Line::intersection(const Plane &plane) const {
    double d;
    Vector3 normal = plane.normal();
    plane.getEquation(normal, d);
    double rate = direction.dot(normal);

    if (rate == 0) {

        return Vector3::INF3;

    } else {
        double t = -(d + point.dot(normal)) / rate;

        return point + direction * t;
    }
}

Line::Line(class BinaryInput& b) {
	deserialize(b);
}


void Line::serialize(class BinaryOutput& b) const {
	point.serialize(b);
	direction.serialize(b);
}


void Line::deserialize(class BinaryInput& b) {
	point.deserialize(b);
	direction.deserialize(b);
}

}

