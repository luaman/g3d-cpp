/**
  @file CollisionDetection.h
  

  Moving collision detection for simple primitives.

  @author Morgan McGuire, matrix@graphics3d.com
  @cite Spherical collision based on Paul Nettle's
  ftp://ftp.3dmaileffects.com/pub/FluidStudios/CollisionDetection/Fluid_Studios_Generic_Collision_Detection_for_Games_Using_Ellipsoids.pdf
  and comments by Max McGuire.  Ray-sphere intersection by Eric Haines.
  Box-Box intersection written by Kevin Egan.
  Thanks to Max McGuire of Iron Lore for various bug fixes.

  @created 2001-11-19
  @edited  2004-05-01

  Copyright 2000-2004, Morgan McGuire.
  All rights reserved.
 */

#ifndef G3D_COLLISIONDETECTION_H
#define G3D_COLLISIONDETECTION_H

#include "G3D/Vector3.h"
#include "G3D/Plane.h"
#include "G3D/Box.h"
#include "G3D/Triangle.h"
#include "G3D/Array.h"
#include "G3D/Ray.h"
#include "G3D/Line.h"

namespace G3D {


/**
  Collision detection primitives and tools for building
  higher order collision detection schemes.

  These routines provide <I>moving</I> collision detection.
  This allows you to detect collisions that occur during a period
  of time, as opposed to the intersection of two static bodies.
  The routines detect collisions between static primitives and moving
  spheres or points.  Because you can choose your reference frame,
  however, you can assign a velocity to a primitive by subtracting
  that same velocity from the sphere or point.

  The Magic Software Library provides sophisticated static collision
  detection if that is needed.

  Collisions are detected for single-sided objects only.  That is,
  no collision is detected when <I>leaving</I> a primitive or passing
  through a plane or triangle opposite the normal... except for point-sphere.

  For a sphere, the collision location returned is the point in world
  space where the surface of the sphere and the fixed object meet.
  It is <B>not</B> not the position of the center of the sphere at
  the time of the collision.

  The collision normal returned is the surface normal to the fixed
  object at the collision location.
 */
class CollisionDetection {
private:

	/** Provides the default argument for the location parameter */
	static Vector3 ignore;
    static bool    ignoreBool;
    static Array<Vector3> ignoreArray;

    // Static class!
    CollisionDetection() {}
    virtual ~CollisionDetection() {}

public:

    /** converts an index [0, 15] to the corresponding separating axis,
      does not return normalized vector in the edge-edge case
      (indices 6 through 15)
     */
    static Vector3 separatingAxisForSolidBoxSolidBox(
            const int       separatingAxisIndex,
            const Box &     box1,
            const Box &     box2);

    /** tests whether any axes for two boxes are parallel,
      if they are then axis1 and axis2 are set to be the
      parallel axes for box1 and box2 respectively
     */
    static bool parallelAxisForSolidBoxSolidBox(
            const double*   ca,
            const double    epsilon,
            int &           axis1,
            int &           axis2);

    /** returns the projected distance between the two boxes along
      the specified separating axis, negative distances correspond
      to an overlap along that separating axis.
      the distance is not divided by denominator dot(L, L),
      see penetrationDepthForFixedSphereFixedBox() for more details
     */
    static inline double projectedDistanceForSolidBoxSolidBox(
            const int           separatingAxisIndex,
            const Vector3 &     a,
            const Vector3 &     b,
            const Vector3 &     D,
            const double*       c,
            const double*       ca,
            const double*       ad,
            const double*       bd)
    {
        double R0;
        double R1;
        double R;
        switch (separatingAxisIndex) {
        case 0:
            // A0
            R0 = a[0];
            R1 = b[0] * ca[0] + b[1] * ca[1] + b[2] * ca[2];
            R = fabs(ad[0]);
            break;
        case 1:
            // A1
            R0 = a[1];
            R1 = b[0] * ca[3] + b[1] * ca[4] + b[2] * ca[5];
            R = fabs(ad[1]);
            break;
        case 2:
            // A2
            R0 = a[2];
            R1 = b[0] * ca[6] + b[1] * ca[7] + b[2] * ca[8];
            R = fabs(ad[2]);
            break;
        case 3:
            // B0
            R0 = a[0] * ca[0] + a[1] * ca[3] + a[2] * ca[6];
            R1 = b[0];
            R = fabs(bd[0]);
            break;
        case 4:
            // B1
            R0 = a[0] * ca[1] + a[1] * ca[4] + a[2] * ca[7];
            R1 = b[1];
            R = fabs(bd[1]);
            break;
        case 5:
            // B2
            R0 = a[0] * ca[2] + a[1] * ca[5] + a[2] * ca[8];
            R1 = b[2];
            R = fabs(bd[2]);
            break;
        case 6:
            // A0 x B0
            R0 = a[1] * ca[6] + a[2] * ca[3];
            R1 = b[1] * ca[2] + b[2] * ca[1];
            R = fabs(c[3] * ad[2] - c[6] * ad[1]);
            break;
        case 7:
            // A0 x B1
            R0 = a[1] * ca[7] + a[2] * ca[4];
            R1 = b[0] * ca[2] + b[2] * ca[0];
            R = fabs(c[4] * ad[2] - c[7] * ad[1]);
            break;
        case 8:
            // A0 x B2
            R0 = a[1] * ca[8] + a[2] * ca[5];
            R1 = b[0] * ca[1] + b[1] * ca[0];
            R = fabs(c[5] * ad[2] - c[8] * ad[1]);
            break;
        case 9:
            // A1 x B0
            R0 = a[0] * ca[6] + a[2] * ca[0];
            R1 = b[1] * ca[5] + b[2] * ca[4];
            R = fabs(c[6] * ad[0] - c[0] * ad[2]);
            break;
        case 10:
            // A1 x B1
            R0 = a[0] * ca[7] + a[2] * ca[1];
            R1 = b[0] * ca[5] + b[2] * ca[3];
            R = fabs(c[7] * ad[0] - c[1] * ad[2]);
            break;
        case 11:
            // A1 x B2
            R0 = a[0] * ca[8] + a[2] * ca[2];
            R1 = b[0] * ca[4] + b[1] * ca[3];
            R = fabs(c[8] * ad[0] - c[2] * ad[2]);
            break;
        case 12:
            // A2 x B0
            R0 = a[0] * ca[3] + a[1] * ca[0];
            R1 = b[1] * ca[8] + b[2] * ca[7];
            R = fabs(c[0] * ad[1] - c[3] * ad[0]);
            break;
        case 13:
            // A2 x B1
            R0 = a[0] * ca[4] + a[1] * ca[1];
            R1 = b[0] * ca[8] + b[2] * ca[6];
            R = fabs(c[1] * ad[1] - c[4] * ad[0]);
            break;
        case 14:
            // A2 x B2
            R0 = a[0] * ca[5] + a[1] * ca[2];
            R1 = b[0] * ca[7] + b[1] * ca[6];
            R = fabs(c[2] * ad[1] - c[5] * ad[0]);
            break;
        default:
            debugAssertM(false, "fell through switch statement");
        }

        return (R - (R0 + R1));
    }


    /** the following space requirements must be met:
      c[] 9 elements, ca[] 9 elements, ad[] 3 elements, bd[] 3 elements,
     
      adobted from David Eberly's papers, variables used in this function
      correspond to variables used in pages 6 and 7 in the pdf
      http://www.magic-software.com/Intersection.html
      http://www.magic-software.com/Documentation/DynamicCollisionDetection.pdf
     */
    static void fillSolidBoxSolidBoxInfo(
            const Box &     box1,
            const Box &     box2,
            Vector3 &       a,
            Vector3 &       b,
            Vector3 &       D,
            double*         c,
            double*         ca,
            double*         ad,
            double*         bd);

    /** return false - two boxes definitely do not intersect
      return true  - the boxes may intersect, further work must be done
     */
    static bool CollisionDetection::conservativeBoxBoxTest(
            const Vector3 &     a,
            const Vector3 &     b,
            const Vector3 &     D);

    /** adobted from David Eberly's papers, variables used in this function
      correspond to variables used in pages 6 and 7 in the pdf
      http://www.magic-software.com/Intersection.html
      http://www.magic-software.com/Documentation/DynamicCollisionDetection.pdf
     
      to speed up collision detection, if two objects do not intersect
      the lastSeparatingAxis from the previous time step can be passed in,
      and that plane can be checked first.  If the separating axis
      was not saved, or if the two boxes intersected then
      lastSeparatingAxis should equal -1
     */
    static bool CollisionDetection::fixedSolidBoxIntersectsFixedSolidBox(
        const Box&      box1,
        const Box&      box2,
        const int       lastSeparatingAxis = -1);

    /** variables and algorithm based on derivation at the following website:
      http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
     */
    static void closestPointsBetweenLineAndLine(
            const Line &    line1,
            const Line &    line2,
            Vector3 &       closest1,
            Vector3 &       closest2);

    /** adobted from David Eberly's papers, variables used in this function
      correspond to variables used in pages 6 and 7 in the pdf
      http://www.magic-software.com/Intersection.html
      http://www.magic-software.com/Documentation/DynamicCollisionDetection.pdf
     
      to speed up collision detection, if two objects do not intersect
      the lastSeparatingAxis from the previous time step can be passed in,
      and that plane can be checked first.  If the separating axis
      was not saved, or if the two boxes intersected then
      lastSeparatingAxis should equal -1
     
      normal faces away from box1 and into box2
      if there is contact only one contact point is returned
          the minimally violated separating plane is computed
          if the separating axis corresponds to a face
              the contact point is half way between the deepest vertex
              and the face
          if the separating axis corresponds to two edges
              the contact point is the midpoint of the smallest line
              segment between the two edge lines
     */
    static double CollisionDetection::penetrationDepthForFixedBoxFixedBox(
        const Box&          box1,
        const Box&          box2,
        Array<Vector3>&     contactPoints,
        Array<Vector3>&     contactNormals,
        const int           lastSeparatingAxis = -1);


    static double CollisionDetection::penetrationDepthForFixedSphereFixedBox(
        const Box&      box1,
        const Box&      box2,
        Array<Vector3>& contactPoints,
        Array<Vector3>& contactNormals);

    /**
     Returns the penetration depth (negative if there is no penetration)
     of the two spheres and a series of contact points.  The normal
     returned points <B>away</B> from the object A, although it may
     represent a perpendicular to either the faces of object B or object A
     depending on their relative orientations.
     */
    static double penetrationDepthForFixedSphereFixedSphere(
        const class Sphere& sphereA,
        const Sphere&       sphereB,
        Array<Vector3>&     contactPoints,
        Array<Vector3>&     contactNormals = ignoreArray);
    
    /**
     @cite Adapted from Jim Arvo's method in Graphics Gems
     See also http://www.win.tue.nl/~gino/solid/gdc2001depth.pdf
     */
    static double penetrationDepthForFixedSphereFixedBox(
        const Sphere&       sphere,
        const Box&          box,
        Array<Vector3>&     contactPoints,
        Array<Vector3>&     contactNormals = ignoreArray);
                                                                 
    static double penetrationDepthForFixedSphereFixedPlane(
        const Sphere&       sphereA,
        const class Plane&  planeB,
        Array<Vector3>&     contactPoints,
        Array<Vector3>&     contactNormals = ignoreArray);

    static double penetrationDepthForFixedBoxFixedPlane(
        const Box&          box,
        const Plane&        plane,
        Array<Vector3>&     contactPoints,
        Array<Vector3>&     contactNormals = ignoreArray);
    
    /**
     Returns the amount of time until the point intersects the plane 
     (the plane is one sided; the point can only hit the side the 
     normal faces out of).  The return value is inf if no 
     collision will occur, zero if the point is already in the plane.
     
     To perform a two sided collision, call twice, once for each direction
     of the plane normal.

     @param location The location of the collision is point + velocity * time.
      This is returned in location, if a collision occurs.  Otherwise location
      is the infinite vector.
     */
    static double collisionTimeForMovingPointFixedPlane(
        const Vector3&			point,
        const Vector3&			velocity,
        const class Plane&		plane,
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

    /** One-sided triangle */
    static inline double collisionTimeForMovingPointFixedTriangle(
        const Vector3& orig,
        const Vector3& dir,
        const Vector3& vert0,
        const Vector3& vert1,
        const Vector3& vert2) {
        return Ray::fromOriginAndDirection(orig, dir).intersectionTime(vert0, vert1, vert2);
    }

    inline static double collisionTimeForMovingPointFixedTriangle(
        const Vector3& orig,
        const Vector3& dir,
        const Vector3& vert0,
        const Vector3& vert1,
        const Vector3& vert2,
        Vector3&       location) {
        double t = collisionTimeForMovingPointFixedTriangle(orig, dir, vert0, vert1, vert2);
        if (t < inf()) {
            location = orig + dir * t;
        }
        return t;
    }

    inline static double collisionTimeForMovingPointFixedTriangle(
        const Vector3&  orig,
        const Vector3&  dir,
        const Triangle& tri,
        Vector3&        location = ignore,
        Vector3&        normal   = ignore) {
        double t = collisionTimeForMovingPointFixedTriangle(
            orig, dir, tri.vertex(0), tri.vertex(1), tri.vertex(2));
        if ((t < inf()) && (&location != &ignore)) {
            location = orig + dir * t;
            normal   = tri.normal();
        }
        return t;
    }

    inline static double collisionTimeForMovingPointFixedTriangle(
        const Vector3& orig,
        const Vector3& dir,
        const Vector3& vert0,
        const Vector3& vert1,
        const Vector3& vert2,
        Vector3&       location,
        Vector3&       normal) {
        double t = collisionTimeForMovingPointFixedTriangle(orig, dir, vert0, vert1, vert2);
        if (t < inf()) {
            location = orig + dir * t;
            normal   = (vert2 - vert0).cross(vert1 - vert0).direction();
        }
        return t;
    }

    /**
     Unlike other methods, does not support an output normal.
     If the ray origin is inside the box, returns inf() but inside
     is set to true.
     <B>Beta API</B>

     @cite Andrew Woo, from "Graphics Gems", Academic Press, 1990
  	 @cite Optimized code by Pierre Terdiman, 2000 (~20-30% faster on my Celeron 500)
     @cite Epsilon value added by Klaus Hartmann
     @cite http://www.codercorner.com/RayAABB.cpp
     */
    static double collisionTimeForMovingPointFixedAABox(
        const Vector3&			point,
        const Vector3&			velocity,
        const class AABox&      box,
        Vector3&				outLocation,
        bool&                   inside = ignoreBool);

    /** Avoids the sqrt from collisionTimeForMovingPointFixedAABox.
        Returns true if there is a collision, false otherwise.*/
    static bool collisionLocationForMovingPointFixedAABox(
        const Vector3&			point,
        const Vector3&			velocity,
        const class AABox&      box,
        Vector3&				outLocation,
        bool&                   inside = ignoreBool);

    /** When the ray is already inside, detects the exiting intersection */
    static double collisionTimeForMovingPointFixedSphere(
        const Vector3&			point,
        const Vector3&			velocity,
        const class Sphere&		sphere,                                                       
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

    /** If the point is already inside the box, no collision: inf is returned */
    static double collisionTimeForMovingPointFixedBox(
        const Vector3&			point,
        const Vector3&			velocity,
        const class  Box&		box,
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

	/**
	  The 4 vertices are assumed to form a rectangle.
	 */
    static double collisionTimeForMovingPointFixedRectangle(
        const Vector3&			point,
        const Vector3&			velocity,
        const Vector3&			v0,
        const Vector3&			v1,
        const Vector3&			v2,
        const Vector3&			v3,
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

	static double collisionTimeForMovingPointFixedCapsule(
		const Vector3&		    point,
		const Vector3&		    velocity,
		const class Capsule&	capsule,
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

    /////////////////////////

    static double collisionTimeForMovingSphereFixedPlane(
        const class Sphere&		sphere,
        const Vector3&	    	velocity,
        const class Plane&		plane,
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

    static double collisionTimeForMovingSphereFixedTriangle(
        const class Sphere&		sphere,
        const Vector3&		    velocity,
        const Triangle&       triangle,
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

    static double collisionTimeForMovingSphereFixedRectangle(
        const class Sphere&		sphere,
        const Vector3&	    	velocity,
        const Vector3&	    	v0,
        const Vector3&	    	v1,
        const Vector3&	    	v2,
        const Vector3&	    	v3,
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

    static double collisionTimeForMovingSphereFixedBox(
        const class Sphere&		sphere,
        const Vector3&		    velocity,
        const class Box&		box,
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

    /**
      This won't detect a collision if the sphere is already interpenetrating the fixed sphere.
    */
	static double collisionTimeForMovingSphereFixedSphere(
		const class Sphere&		sphere,
		const Vector3&		    velocity,
		const class Sphere&	    fixedSphere,
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

    /**
      This won't detect a collision if the sphere is already interpenetrating the capsule
    */
	static double collisionTimeForMovingSphereFixedCapsule(
		const class Sphere&		sphere,
		const Vector3&		    velocity,
		const class Capsule&	capsule,
        Vector3&				outLocation,
        Vector3&                outNormal = ignore);

    /**
     Returns the direction the sphere should bounce if it collided with
     an object at collisionTime and collisionLocation.
     */
    static Vector3 bounceDirection(
        const class Sphere&		sphere,
        const Vector3&			velocity,
        const float				collisionTime,
        const Vector3&			collisionLocation,
        const Vector3&          collisionNormal);

    /**
     Returns the direction the sphere should slide if it collided with
     an object at collisionTime and collisionLocation and will hug the surface.
     */
    static Vector3 slideDirection(
        const class Sphere&		sphere,
        const Vector3&			velocity,
        const float				collisionTime,
        const Vector3&			collisionLocation);

    /**
     Returns the point on the line segment closest to the input point.
     */
    static Vector3 closestPointOnLineSegment(
        const Vector3&			v0,
        const Vector3&			v1,
        const Vector3&			point);

    /**
     This is the fast version of the function.

     @param v0 The starting vertex of the line segment
     @param v1 The ending vertex of the line segment
     @param edgeLength The length of the segment
     @param edgeDirection The direction of the segment (unit length)
     @param point The point in question.
     */
    static Vector3 closestPointOnLineSegment(
        const Vector3&			v0,
        const Vector3&			v1,
        const Vector3&          edgeDirection,
        double                  edgeLength,
        const Vector3&			point);

    /**
     Returns the point on the perimeter of the triangle closest to the input point and 
     returns it in location.  The return value is the distance to that point.
     */
    static Vector3 closestPointToTrianglePerimeter(
        const Vector3&			v0, 
        const Vector3&			v1,
        const Vector3&			v2,
        const Vector3&			point);

    static Vector3 closestPointToTrianglePerimeter(
        const Vector3           v[3],
        const Vector3           edgeDirection[3],
        const double            edgeLength[3],
        const Vector3&			point);

    /**
     Returns true if a point <B>known to be in the plane of a triangle</B> is inside of its bounds.
     */
    static bool isPointInsideTriangle(
        const Vector3&			v0,
        const Vector3&			v1,
        const Vector3&			v2,
        const Vector3&			normal,
        const Vector3&			point,
        Vector3::Axis  primaryAxis = Vector3::DETECT_AXIS);
    
    /**
     Returns true if any part of the sphere is inside the box
     during the time period (inf means "ever").  Useful for
     performing bounding-box collision detection.
     */
    static bool movingSpherePassesThroughFixedBox(
        const Sphere&           sphere,
        const Vector3&          velocity,
        const Box&              box,
        double                  timeLimit = inf());

    static bool movingSpherePassesThroughFixedSphere(
        const Sphere&           sphere,
        const Vector3&          velocity,
        const Sphere&           fixedSphere,
        double                  timeLimit = inf());

    static bool fixedSolidSphereIntersectsFixedSolidSphere(
        const Sphere&           sphere1,
        const Sphere&           sphere2);

    static bool fixedSolidSphereIntersectsFixedSolidBox(
        const Sphere&           sphere,
        const Box&              box);

    /**
     Returns true if a point in the plane of a triangle is inside of its bounds.
     */
    static bool isPointInsideRectangle(
        const Vector3&			v0,
        const Vector3&			v1,
        const Vector3&			v2,
        const Vector3&			v3,
        const Vector3&			normal,
        const Vector3&			point);

    /**
     Returns the point on the perimeter of the rectangle closest to the input point and 
     returns it in location.  The return value is the distance to that point.
     */
    static Vector3 closestPointToRectanglePerimeter(
        const Vector3&			v0,
        const Vector3&			v1,
        const Vector3&			v2,
        const Vector3&			v3,
        const Vector3&			point);

    static Vector3 closestPointToRectangle(
        const Vector3&			v0,
        const Vector3&			v1,
        const Vector3&			v2,
        const Vector3&			v3,
        const Vector3&			point);
};

} // namespace

#endif
