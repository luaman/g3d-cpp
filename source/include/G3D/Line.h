/**
 @file Line.h
 
 Line class
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2001-06-02
 @edited  2003-02-06
 */

#ifndef G3D_LINE_H
#define G3D_LINE_H

#include "G3D/Vector3.h"

namespace G3D {

class Plane;

/**
 An infinite 3D line.
 */
class Line {

private:

    Vector3 point;
    Vector3 direction;

    Line(const Vector3 &point, const Vector3 &direction) {
        this->point = point;
        this->direction = direction.direction();
    }

public:

	Line(class BinaryInput& b);

	void serialize(class BinaryOutput& b) const;

	void deserialize(class BinaryInput& b);

    virtual ~Line() {}

    /**
     * Constructs a line from two (not equal) points.
     */
    static Line fromTwoPoints(const Vector3 &point1, const Vector3 &point2) {
        return Line(point1, point2 - point1);
    }

    /**
     * Creates a line from a point and a (nonzero) direction.
     */
    static Line fromPointAndDirection(const Vector3 &point, const Vector3 &direction) {
        return Line(point, direction);
    }

    /**
     * Returns the closest point on the line to point.
     */
    Vector3 closestPoint(const Vector3 &point) const {
        double t = direction.dot(point - this->point);
        return this->point + direction * t;
    }

    /**
     * Returns the distance between point and the line
     */
    double distance(const Vector3 &point) const {
        return (closestPoint(point) - point).length();
    }

    /**
     Returns the point where the line and plane intersect.  If there
     is no intersection, returns a point at infinity.
     */
    Vector3 intersection(const Plane &plane) const;
};

};// namespace


#endif
