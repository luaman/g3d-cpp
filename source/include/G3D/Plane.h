/**
 @file Plane.h

 Plane class

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2001-06-02
 @edited  2003-02-06
*/

#ifndef G3D_PLANE_H
#define G3D_PLANE_H

#include "G3D/Vector3.h"
#include "G3D/CoordinateFrame.h"

namespace G3D {

/**
 An infinite 2D plane in 3D space.
 */
class Plane {

    /** normal.Dot(x,y,z) = distance */
    Vector3						normal;
    Real						distance;

public:

    Plane() : normal(Vector3::UNIT_Y), distance(0) {
    }

    /**
     Constructs a plane from three points.
     */
    Plane(
        const Vector3&      point0,
        const Vector3&      point1,
        const Vector3&      point2);

    Plane(
        const Vector3&      _normal,
        const Vector3&      point);

	Plane(class BinaryInput& b);
	void serialize(class BinaryOutput& b) const;
	void deserialize(class BinaryInput& b);

    virtual ~Plane() {}

    /**
     Returns true if point is on the side the normal points to or 
     is in the plane.
     */
    inline bool halfSpaceContains(const Vector3& point) const {
        return normal.dot(point) >= distance;
    }

    /**
     Returns true if the point is nearly in the plane.
     */
    inline bool fuzzyContains(const Vector3 &point) const {
        return fuzzyEq(point.dot(normal), distance);
    }

    inline Vector3 getNormal() const {
        return normal;
    }

    /**
     Inverts the facing direction of the plane so the new normal
     is the inverse of the old normal.
     */
    void flip();

    /**
      Returns the equation in the form:

      <CODE>normal.Dot(Vector3(<I>x</I>, <I>y</I>, <I>z</I>)) + d = 0</CODE>
     */
    void getEquation(Vector3 &normal, Real& d) const;

    /**
      ax + by + cz + d = 0
     */
    void getEquation(Real& a, Real& b, Real& c, Real& d) const;

    std::string toString() const;
};

} // namespace

#endif
