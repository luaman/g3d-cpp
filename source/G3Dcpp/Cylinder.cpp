/**
 @file Cylinder.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2003-02-07
 @edited  2005-08-18

 Copyright 2000-2005, Morgan McGuire.
 All rights reserved.
 */

#include "G3D/Cylinder.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/LineSegment.h"

namespace G3D {

Cylinder::Cylinder(class BinaryInput& b) {
	deserialize(b);
}


Cylinder::Cylinder() {
}


Cylinder::Cylinder(const Vector3& _p1, const Vector3& _p2, double _r) 
	: p1(_p1), p2(_p2), radius(_r) {
}


void Cylinder::serialize(class BinaryOutput& b) const {
	p1.serialize(b);
	p2.serialize(b);
	b.writeFloat64(radius);
}


void Cylinder::deserialize(class BinaryInput& b) {
	p1.deserialize(b);
	p2.deserialize(b);
	radius = b.readFloat64();
}


Line Cylinder::getAxis() const {
	return Line::fromTwoPoints(p1, p2);
}


Vector3 Cylinder::getPoint1() const {
	return p1;
}


Vector3 Cylinder::getPoint2() const {
	return p2;
}


double Cylinder::getRadius() const {
	return radius;
}


double Cylinder::getVolume() const {
	return 
		// Cylinder volume
		pow(radius, 2) * (p1 - p2).length();
}


double Cylinder::getSurfaceArea() const {

	return
		// Cylinder area
		2 * G3D_PI * radius * (p1 - p2).length();
}


bool Cylinder::contains(const Vector3& p) const { 
    return LineSegment::fromTwoPoints(p1, p2).distanceSquared(p) <= square(radius);
}


} // namespace
