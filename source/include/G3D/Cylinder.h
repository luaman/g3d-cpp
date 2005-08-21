/**
 @file Cylinder.h
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
  
 @created 2003-02-07
 @edited  2003-02-07

 Copyright 2000-2003, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_Cylinder_H
#define G3D_Cylinder_H

#include "G3D/g3dmath.h"
#include "G3D/Vector3.h"
#include "G3D/Line.h"

namespace G3D {

/**
 Right cylinder
 */
class Cylinder {
private:
	Vector3			p1;
	Vector3			p2;

public:

	double			radius;

    /** Uninitialized */
    Cylinder();
    Cylinder(class BinaryInput& b);
	Cylinder(const Vector3& _p1, const Vector3& _p2, double _r);
	void serialize(class BinaryOutput& b) const;
	void deserialize(class BinaryInput& b);
	
	/** The line down the center of the Cylinder */
	Line getAxis() const;

	Vector3 getPoint1() const;

	Vector3 getPoint2() const;

    /**
     Returns true if the point is inside the Cylinder or on its surface.
     */
    bool contains(const Vector3& p) const;

	double getRadius() const;

	double getVolume() const;

	double getSurfaceArea() const;

};

} // namespace

#endif
