/**
 @file LineSegment.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-02-08
 @edited  2003-02-15
 */

#include "G3D/LineSegment.h"
#include "G3D/Sphere.h"

namespace G3D {


Vector3 LineSegment::closestPoint(const Vector3& p) const {

    // The vector from the end of the capsule to the point in question.
    Vector3 v(p - point);

    // Projection of v onto the line segment scaled by 
    // the length of direction.
    double t = direction.dot(v);

    // Avoid some square roots.  Derivation:
    //    t/direction.length() <= direction.length()
    //      t <= direction.squaredLength()

    if ((t >= 0) && (t <= direction.squaredLength())) {
    
        // The point falls within the segment.  Normalize direction,
        // divide t by the length of direction.
        return point + direction * t / direction.squaredLength();
    
    } else {

        // The point does not fall within the segment; see which end is closer.

        // Distance from 0, squared
        double d0Squared = v.squaredLength();

        // Distance from 1, squared
        double d1Squared = (v - direction).squaredLength();

        if (d0Squared < d1Squared) {

            // Point 0 is closer
            return point;

        } else {

            // Point 1 is closer
            return point + direction;
        
        }
    }

}


bool LineSegment::intersectsSolidSphere(const class Sphere& s) const {
    return distanceSquared(s.center) <= square(s.radius);
}


LineSegment::LineSegment(class BinaryInput& b) {
	deserialize(b);
}


void LineSegment::serialize(class BinaryOutput& b) const {
	point.serialize(b);
	direction.serialize(b);
}


void LineSegment::deserialize(class BinaryInput& b) {
	point.deserialize(b);
	direction.deserialize(b);
}

}

