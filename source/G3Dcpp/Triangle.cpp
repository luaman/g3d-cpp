/**
 @file Triangle.cpp
 
 @maintainer Morgan McGuire, graphics3d.com
 
 @created 2001-04-06
 @edited  2003-04-06

 Copyright 2000-2003, Morgan McGuire.
 All rights reserved.
 */

#include "G3D/Triangle.h"
#include "G3D/Plane.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/debugAssert.h"

namespace G3D {

    
void Triangle::init(const Vector3& v0, const Vector3& v1, const Vector3& v2) {

    _plane = Plane(v0, v1, v2);
    _vertex[0] = v0;
    _vertex[1] = v1;
    _vertex[2] = v2;

    static int next[] = {1,2,0};

    for (int i = 0; i < 3; ++i) {
        const Vector3 e  = _vertex[next[i]] - _vertex[i];
        edgeLength[i]    = e.length();
        edgeDirection[i] = e / edgeLength[i];
    }

    _primaryAxis = _plane.getNormal().primaryAxis();
}


Triangle::Triangle() {
    _vertex[0] = Vector3::ZERO;
    _vertex[1] = Vector3::ZERO;
    _vertex[2] = Vector3::ZERO;
}
    

Triangle::Triangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) {
    _vertex[0] = v0;
    _vertex[1] = v1;
    _vertex[2] = v2;
}

    
Triangle::~Triangle() {
}


Triangle::Triangle(class BinaryInput& b) {
    deserialize(b);
}


void Triangle::serialize(class BinaryOutput& b) {
    _vertex[0].serialize(b);
    _vertex[1].serialize(b);
    _vertex[2].serialize(b);
}


void Triangle::deserialize(class BinaryInput& b) {
    _vertex[0].deserialize(b);
    _vertex[1].deserialize(b);
    _vertex[2].deserialize(b);
}


double Triangle::area() const {
    return edgeDirection[0].cross(edgeDirection[2]).length() * (edgeLength[0] * edgeLength[2]);
}


const Vector3& Triangle::normal() const {
    return _plane.getNormal();
}


const Plane& Triangle::plane() const {
    return _plane;
}


Vector3 Triangle::randomPoint() const {
    // Choose a random point in the parallelogram
    Vector3 U = edgeDirection[0];
    Vector3 V = edgeDirection[2];

    double s = unitRandom();
    double t = unitRandom();

    if (1 - t > s) {
        // Outside the triangle; reflect about the
        // diagonal of the parallelogram
        t = 1 - t;
        s = 1 - s;
    }

    return (U * (s * edgeLength[0])) + (V * (t * -edgeLength[2])) + _vertex[0];
}

}
