/**
 @file Ray.h
 
 Ray class
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2002-07-12
 @edited  2004-03-21
 */

#ifndef G3D_RAY_H
#define G3D_RAY_H

#include "G3D/Vector3.h"
#include "G3D/Triangle.h"

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

    Ray() : origin(Vector3::zero()), direction(Vector3::zero()) {}

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

    /**
     The three extra arguments are the weights of vertices 0, 1, and 2
     at the intersection point; they are useful for texture mapping
     and interpolated normals.
     */
    double intersectionTime(
        const Vector3& v0, const Vector3& v1, const Vector3& v2,
        const Vector3& edge01, const Vector3& edge02,
        double& w0, double& w1, double& w2) const;

     /**
     Ray-triangle intersection for a 1-sided triangle.  Fastest version.
       @cite http://www.acm.org/jgt/papers/MollerTrumbore97/
       http://www.graphics.cornell.edu/pubs/1997/MT97.html
     */
    inline double Ray::intersectionTime(
        const Vector3& vert0,
        const Vector3& vert1,
        const Vector3& vert2,
        const Vector3& edge01,
        const Vector3& edge02) const;


    inline double Ray::intersectionTime(
        const Vector3& vert0,
        const Vector3& vert1,
        const Vector3& vert2) const {

        return intersectionTime(vert0, vert1, vert2, vert1 - vert0, vert2 - vert0);
    }


    inline double Ray::intersectionTime(
        const Vector3&  vert0,
        const Vector3&  vert1,
        const Vector3&  vert2,
        double&         w0,
        double&         w1,
        double&         w2) const {

        return intersectionTime(vert0, vert1, vert2, vert1 - vert0, vert2 - vert0, w0, w1, w2);
    }

    /* One-sided triangle 
       */
    inline double Ray::intersectionTime(const Triangle& triangle) const {
        return intersectionTime(
            triangle.vertex(0), triangle.vertex(1), triangle.vertex(2),
            triangle.edge01, triangle.edge02);
    }

    inline double Ray::intersectionTime(
        const Triangle& triangle,
        double&         w0,
        double&         w1,
        double&         w2) const {
        return intersectionTime(triangle.vertex(0), triangle.vertex(1), triangle.vertex(2),
            triangle.edge01, triangle.edge02, w0, w1, w2);
    }

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

inline double Ray::intersectionTime(
    const Vector3& vert0,
    const Vector3& vert1,
    const Vector3& vert2,
    const Vector3& edge1,
    const Vector3& edge2) const {

    // Barycenteric coords
    double u, v;

    double tvec[3], pvec[3], qvec[3];
    
    // begin calculating determinant - also used to calculate U parameter
    CROSS(pvec, direction, edge2);
    
    // if determinant is near zero, ray lies in plane of triangle
    const double det = DOT(edge1, pvec);
    
    if (det < EPSILON) {
        return inf();
    }
    
    // calculate distance from vert0 to ray origin
    SUB(tvec, origin, vert0);
    
    // calculate U parameter and test bounds
    u = DOT(tvec, pvec);
    if ((u < 0.0) || (u > det)) {
        // Hit the plane outside the triangle
        return inf();
    }
    
    // prepare to test V parameter
    CROSS(qvec, tvec, edge1);
    
    // calculate V parameter and test bounds
    v = DOT(direction, qvec);
    if ((v < 0.0) || (u + v > det)) {
        // Hit the plane outside the triangle
        return inf();
    }
    

    // Case where we don't need correct (u, v):
    const double t = DOT(edge2, qvec);
    
    if (t >= 0) {
        // Note that det must be positive
        return t / det;
    } else {
        // We had to travel backwards in time to intersect
        return inf();
    }
}


inline double Ray::intersectionTime(
    const Vector3&  vert0,
    const Vector3&  vert1,
    const Vector3&  vert2,
    const Vector3&  edge1,
    const Vector3&  edge2,
    double&         w0,
    double&         w1,
    double&         w2) const {

    // Barycenteric coords
    double u, v;

    double tvec[3], pvec[3], qvec[3];

    // begin calculating determinant - also used to calculate U parameter
    CROSS(pvec, direction, edge2);
    
    // if determinant is near zero, ray lies in plane of triangle
    const double det = DOT(edge1, pvec);
    
    if (det < EPSILON) {
        return inf();
    }
    
    // calculate distance from vert0 to ray origin
    SUB(tvec, origin, vert0);
    
    // calculate U parameter and test bounds
    u = DOT(tvec, pvec);
    if ((u < 0.0) || (u > det)) {
        // Hit the plane outside the triangle
        return inf();
    }
    
    // prepare to test V parameter
    CROSS(qvec, tvec, edge1);
    
    // calculate V parameter and test bounds
    v = DOT(direction, qvec);
    if ((v < 0.0) || (u + v > det)) {
        // Hit the plane outside the triangle
        return inf();
    }
    
    double t = DOT(edge2, qvec);
    
    if (t >= 0) {
        const double inv_det = 1.0 / det;
        t *= inv_det;
        u *= inv_det;
        v *= inv_det;

        w0 = (1.0 - u - v);
        w1 = u;
        w2 = v;

        return t;
    } else {
        // We had to travel backwards in time to intersect
        return inf();
    }
}

#undef EPSILON
#undef CROSS
#undef DOT
#undef SUB

}// namespace

#endif
