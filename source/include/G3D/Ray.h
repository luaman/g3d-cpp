/**
 @file Ray.h
 
 Ray class
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2002-07-12
 @edited  2004-03-17
 */

#ifndef G3D_RAY_H
#define G3D_RAY_H

#include "G3D/Vector3.h"

namespace G3D {

/**
 A 3D Ray.
 */
class Ray {
private:
    Ray(const Vector3& origin, const Vector3& direction) {
        this->origin    = origin;
        this->direction = direction;
    }

public:
    Vector3         origin;

    /**
     Not unit length
     */
    Vector3         direction;

    Ray() : origin(Vector3::ZERO3), direction(Vector3::ZERO3) {}

	Ray(class BinaryInput& b);
	void serialize(class BinaryOutput& b) const;
	void deserialize(class BinaryInput& b);

    virtual ~Ray() {}

    /**
     Creates a Ray from a origin and a (nonzero) direction.
     */
    static Ray fromOriginAndDirection(const Vector3& point, const Vector3& direction) {
        return Ray(point, direction);
    }

    Ray unit() const {
        return Ray(origin, direction.unit());
    }

    /**
     Returns the closest point on the Ray to point.
     */
    Vector3 closestPoint(const Vector3& point) const {
        double t = direction.dot(point - this->origin);
        if (t < 0) {
            return this->origin;
        } else {
            return this->origin + direction * t;
        }
    }

    /**
     Returns the closest distance between point and the Ray
     */
    double distance(const Vector3& point) const {
        return (closestPoint(point) - this->origin).length();
    }

    /**
     Returns the point where the Ray and plane intersect.  If there
     is no intersection, returns a point at infinity.
     */
    Vector3 intersection(const class Plane& plane) const;

    /**
     Returns the distance until intersection with the (solid) sphere.
     Will be 0 if inside the sphere, inf if there is no intersection.

     The ray direction is <B>not</B> normalized.  If the ray direction
     has unit length, the distance from the origin to intersection
     is equal to the time.  If the direction does not have unit length,
     the distance = time * direction.length().

     See also G3D::CollisionDetection.
     */
    double intersectionTime(const class Sphere& sphere) const;

    double intersectionTime(const class Plane& plane) const;

    double intersectionTime(const class Box& box) const;

    double intersectionTime(const class AABox& box) const;

    /* One-sided triangle 
       @cite http://www.acm.org/jgt/papers/MollerTrumbore97/
       http://www.graphics.cornell.edu/pubs/1997/MT97.html
       */
    double intersectionTime(const class Triangle& triangle) const;

    /**
     Ray-triangle intersection
     */
    double intersectionTime(
        const Vector3& v0,
        const Vector3& v1,
        const Vector3& v2) const;

    /** Refracts about the normal
        using G3D::Vector3::refractionDirection
        and bumps the ray slightly from the newOrigin. */
    Ray refract(
        const Vector3&  newOrigin,
        const Vector3&  normal,
        double          iInside,
        double          iOutside) const;

    /** Reflects about the normal
        using G3D::Vector3::reflectionDirection
        and bumps the ray slightly from
        the newOrigin. */
    Ray reflect(
        const Vector3&  newOrigin,
        const Vector3&  normal) const;
};


inline double Ray::intersectionTime(
    const Vector3& vert0,
    const Vector3& vert1,
    const Vector3& vert2) const {

    // Barycenteric coords
    double u, v;
    #define EPSILON 0.000001
    #define CROSS(dest,v1,v2) \
              dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
              dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
              dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

    #define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

    #define SUB(dest,v1,v2) \
              dest[0]=v1[0]-v2[0]; \
              dest[1]=v1[1]-v2[1]; \
              dest[2]=v1[2]-v2[2]; 

    double edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
    
    // find vectors for two edges sharing vert0
    SUB(edge1, vert1, vert0);
    SUB(edge2, vert2, vert0);
    
    // begin calculating determinant - also used to calculate U parameter
    CROSS(pvec, direction, edge2);
    
    // if determinant is near zero, ray lies in plane of triangle
    const double det = DOT(edge1, pvec);
    
    if (det < EPSILON) {
        return inf;
    }
    
    // calculate distance from vert0 to ray origin
    SUB(tvec, origin, vert0);
    
    // calculate U parameter and test bounds
    u = DOT(tvec, pvec);
    if ((u < 0.0) || (u > det)) {
        // Hit the plane outside the triangle
        return inf;
    }
    
    // prepare to test V parameter
    CROSS(qvec, tvec, edge1);
    
    // calculate V parameter and test bounds
    v = DOT(direction, qvec);
    if ((v < 0.0) || (u + v > det)) {
        // Hit the plane outside the triangle
        return inf;
    }
    
    // calculate t, scale parameters, ray intersects triangle
    // If we want u,v, we can compute this
    // double t = DOT(edge2, qvec);
    //const double inv_det = 1.0 / det;
    //t *= inv_det;
    //u *= inv_det;
    //v *= inv_det;
    // return t;

    // Case where we don't need correct (u, v):

    const double t = DOT(edge2, qvec);
    
    if (t >= 0) {
        // Note that det must be positive
        return t / det;
    } else {
        // We had to travel backwards in time to intersect
        return inf;
    }

    #undef EPSILON
    #undef CROSS
    #undef DOT
    #undef SUB
}

}// namespace

#endif
