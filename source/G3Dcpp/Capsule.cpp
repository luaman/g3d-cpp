/**
 @file Capsule.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2003-02-07
 @edited  2003-03-18

 Copyright 2000-2003, Morgan McGuire.
 All rights reserved.
 */

#include "G3D/Capsule.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/LineSegment.h"

namespace G3D {

Capsule::Capsule(class BinaryInput& b) {
	deserialize(b);
}


Capsule::Capsule() {
}


Capsule::Capsule(const Vector3& _p1, const Vector3& _p2, double _r) 
	: p1(_p1), p2(_p2), radius(_r) {
}


void Capsule::serialize(class BinaryOutput& b) const {
	p1.serialize(b);
	p2.serialize(b);
	b.writeFloat64(radius);
}


void Capsule::deserialize(class BinaryInput& b) {
	p1.deserialize(b);
	p2.deserialize(b);
	radius = b.readFloat64();
}


Line Capsule::getAxis() const {
	return Line::fromTwoPoints(p1, p2);
}


Vector3 Capsule::getPoint1() const {
	return p1;
}


Vector3 Capsule::getPoint2() const {
	return p2;
}


double Capsule::getRadius() const {
	return radius;
}


double Capsule::getVolume() const {
	return 
		// Sphere volume
		pow(radius, 3) * PI * 4 / 3 +

		// Cylinder volume
		pow(radius, 2) * (p1 - p2).length();
}


double Capsule::getSurfaceArea() const {

	return
		// Sphere area
		pow(radius, 2) * 4 * PI +

		// Cylinder area
		2 * PI * radius * (p1 - p2).length();
}


bool Capsule::contains(const Vector3& p) const { 
    return LineSegment::fromTwoPoints(p1, p2).distanceSquared(p) <= square(radius);
}


} // namespace
