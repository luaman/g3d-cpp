/**
  @file CollisionDetection.cpp
  
  @maintainer Morgan McGuire, matrix@graphics3d.com
  @cite Bounce direction based on Paul Nettle's ftp://ftp.3dmaileffects.com/pub/FluidStudios/CollisionDetection/Fluid_Studios_Generic_Collision_Detection_for_Games_Using_Ellipsoids.pdf and comments by Max McGuire.  Ray-sphere code by Eric Haines.

  @created 2001-11-24
  @edited  2003-03-18
 */

#include "../include/G3D/CollisionDetection.h"
#include "../include/G3D/debugAssert.h"
#include "../include/G3D/Capsule.h"
#include "../include/G3D/Plane.h"
#include "../include/G3D/Line.h"
#include "../include/G3D/LineSegment.h"
#include "../include/G3D/Sphere.h"
#include "../include/G3D/Box.h"

namespace G3D {

Vector3	CollisionDetection::ignore;


CDTriangle::CDTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) : plane(v0, v1, v2) {
    vertex[0] = v0;
    vertex[1] = v1;
    vertex[2] = v2;

    static int next[] = {1,2,0};

    for (int i = 0; i < 3; ++i) {
        const Vector3 e  = vertex[next[i]] - vertex[i];
        edgeLength[i]    = e.length();
        edgeDirection[i] = e / edgeLength[i];
    }

    primaryAxis = CollisionDetection::normalToPrimaryAxis(plane.getNormal());
}


CollisionDetection::Axis CollisionDetection::normalToPrimaryAxis(
    const Vector3& normal) {
    
    Axis primaryAxis = X_AXIS;

    double nx = abs(normal.x);
    double ny = abs(normal.y);
    double nz = abs(normal.z);

    if (nx > ny) {
        if (nx > nz) {
            primaryAxis = X_AXIS;
        } else {
            primaryAxis = Z_AXIS;
        }
    } else {
        if (ny > nz) {
            primaryAxis = Y_AXIS;
        } else {
            primaryAxis = Z_AXIS;
        }
    }

    return primaryAxis;
}



Real CollisionDetection::collisionTimeForMovingPointFixedPlane(
    const Vector3&  point,
    const Vector3&  velocity,
    const Plane&    plane,
    Vector3&        location,
    Vector3&        outNormal) {

    // Solve for the time at which normal.dot(point + velocity) + d == 0.
    Real d;
    Vector3 normal;
    plane.getEquation(normal, d);
    
    double vdotN = velocity.dot(normal);
    double pdotN = point.dot(normal);

    if (fuzzyEq(pdotN + d, 0)) {
        // The point is *in* the plane.
        location = point;
        outNormal = normal;
        return 0;
    }

    if (vdotN >= 0) {
        // no collision will occur
        location = Vector3::INF3;
        return infReal;
    }

    double t = -(pdotN + d) / vdotN;
    if (t < 0) {
        location = Vector3::INF3;
        return infReal;
    } else {
        location = point + velocity * t;
        outNormal = normal;
        return t;
    }
}


Real CollisionDetection::collisionTimeForMovingPointFixedSphere(
    const Vector3&  point,
    const Vector3&  velocity,
    const Sphere&   sphere,
    Vector3&        location,
    Vector3&        outNormal) {

    double  speed     = velocity.length();
    Vector3 direction = velocity / speed;

    Vector3 L = sphere.center - point;
    double d = L.dot(direction);

    double L2 = L.dot(L);
    double R2 = sphere.radius * sphere.radius;
    double D2 = d * d;

    if ((d < 0) && (L2 > R2)) {
        location = Vector3::INF3;
        return infReal;
    }

    double M2 = L2 - D2;

    if (M2 > R2) {
        location = Vector3::INF3;
        return infReal;
    }

    double q = sqrt(R2 - M2);
    double time;

    if (L2 > R2) {
        time = d - q;
    } else {
        time = d + q;
    }

    time /= speed;

    location  = point + velocity * time;
    outNormal = (location - sphere.center).direction();

    return time;
}


Real CollisionDetection::collisionTimeForMovingSphereFixedSphere(
    const Sphere&   movingSphere,
    const Vector3&  velocity,
    const Sphere&   fixedSphere,
    Vector3&        location,
    Vector3&        outNormal) {

    double time = collisionTimeForMovingPointFixedSphere(movingSphere.center, velocity, Sphere(fixedSphere.center, fixedSphere.radius + movingSphere.radius), location, outNormal);

    if (time < inf) {
        // Location is now the center of the moving sphere at the collision time.
        // Adjust for the size of the moving sphere.  Two spheres always collide
        // along a line between their centers.
        location += movingSphere.radius * (location - fixedSphere.center) / fixedSphere.radius;
    }

    return time;
}


Real CollisionDetection::collisionTimeForMovingPointFixedTriangle(
    const Vector3&			point,
    const Vector3&			velocity,
    const CDTriangle&       triangle,
    Vector3&				outLocation,
    Vector3&                outNormal) {

    Real time = collisionTimeForMovingPointFixedPlane(point, velocity, triangle.plane, outLocation, outNormal);

    if (time == infReal) {
        // No collision with the plane of the triangle.
        return infReal;
    }

    if (isPointInsideTriangle(triangle.vertex[0], triangle.vertex[1], triangle.vertex[2], triangle.plane.getNormal(), outLocation, triangle.primaryAxis)) {
        // Collision occured inside the triangle
        return time;
    } else {
        // Missed the triangle
        outLocation = Vector3::INF3;
        return infReal;
    }

}


Real CollisionDetection::collisionTimeForMovingPointFixedBox(
    const Vector3&          point,
    const Vector3&          velocity,
    const Box&              box,
    Vector3&                location,
    Vector3&                outNormal) {

    Real    bestTime;

    Vector3 normal;
    Vector3 v[4];
    int f = 0;
    box.getFaceCorners(f, v[0], v[1], v[2], v[3]);
    bestTime = collisionTimeForMovingPointFixedRectangle(point, velocity, v[0], v[1], v[2], v[3], location, normal);

    outNormal = normal;

    for (f = 1; f < 6; ++f) {
        Vector3 pos;
        box.getFaceCorners(f, v[0], v[1], v[2], v[3]);
        Real time = collisionTimeForMovingPointFixedRectangle(point, velocity, v[0], v[1], v[2], v[3], pos, outNormal);
        if (time < bestTime) {
            bestTime = time;
            outNormal = normal;
            location = pos;
        }
    }

    return bestTime;
}


Real CollisionDetection::collisionTimeForMovingPointFixedRectangle(
    const Vector3&      point,
    const Vector3&      velocity,
    const Vector3&      v0,
    const Vector3&      v1,
    const Vector3&      v2,
    const Vector3&      v3,
    Vector3&            location,
    Vector3&            outNormal) {

    Plane plane = Plane(v0, v1, v2);

    double time = collisionTimeForMovingPointFixedPlane(point, velocity, plane, location, outNormal);

    if (time == infReal) {
        // No collision is ever going to happen
        return time;
    }

    if (isPointInsideRectangle(v0, v1, v2, v3, plane.getNormal(), location)) {
        // The intersection point is inside the rectangle; that is the location where
        // the point hits the rectangle.
        return time;
    } else {
        return inf;
    }
}

/** Used by findRayCapsuleIntersection.
    @cite From magic software http://www.magic-software.com/Source/Intersection3D/MgcIntr3DLinCap.cpp */
static int findRayCapsuleIntersectionAux(
	const Vector3&		rkOrigin,
	const Vector3&		rkDirection,
    const Capsule&		rkCapsule,
	double   			afT[2]) {

	Vector3 capsuleDirection = rkCapsule.getPoint2() - rkCapsule.getPoint1();

    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kU, kV, kW = capsuleDirection;
    Real fWLength = kW.unitize();
    Vector3::generateOrthonormalBasis(kU, kV, kW);
    Vector3 kD(kU.dot(rkDirection), kV.dot(rkDirection), kW.dot(rkDirection));
    Real fDLength = kD.unitize();

    Real fEpsilon = 1e-6f;

    Real fInvDLength = 1.0f/fDLength;
    Vector3 kDiff = rkOrigin - rkCapsule.getPoint1();
    Vector3 kP(kU.dot(kDiff),kV.dot(kDiff),kW.dot(kDiff));
    Real fRadiusSqr = square(rkCapsule.getRadius());

    Real fInv, fA, fB, fC, fDiscr, fRoot, fT, fTmp;

    // Is the velocity parallel to the capsule direction? (or zero)
    if ((abs(kD.z) >= 1.0f - fEpsilon) || (fDLength < fEpsilon)) {

        Real fAxisDir = rkDirection.dot(capsuleDirection);

        fDiscr = fRadiusSqr - kP.x*kP.x - kP.y*kP.y;
        if ((fAxisDir < 0) && (fDiscr >= 0.0f)) {
            // Velocity anti-parallel to the capsule direction
            fRoot = sqrt(fDiscr);
            afT[0] = (kP.z + fRoot)*fInvDLength;
            afT[1] = -(fWLength - kP.z + fRoot)*fInvDLength;
            return 2;
        } else if ((fAxisDir > 0) && (fDiscr >= 0.0f)) {
            // Velocity parallel to the capsule direction
            fRoot = sqrt(fDiscr);
            afT[0] = -(kP.z + fRoot)*fInvDLength;
            afT[1] = (fWLength - kP.z + fRoot)*fInvDLength;
            return 2;
        } else {
            // sphere heading wrong direction, or no velocity at all
            return 0;
        }   
    }

    // test intersection with infinite cylinder
    fA = kD.x*kD.x + kD.y*kD.y;
    fB = kP.x*kD.x + kP.y*kD.y;
    fC = kP.x*kP.x + kP.y*kP.y - fRadiusSqr;
    fDiscr = fB*fB - fA*fC;
    if (fDiscr < 0.0f) {
        // line does not intersect infinite cylinder
        return 0;
    }

    int iQuantity = 0;

    if (fDiscr > 0.0f) {
        // line intersects infinite cylinder in two places
        fRoot = sqrt(fDiscr);
        fInv = 1.0f/fA;
        fT = (-fB - fRoot)*fInv;
        fTmp = kP.z + fT*kD.z;
        if ((0.0f <= fTmp) && (fTmp <= fWLength)) {
            afT[iQuantity] = fT * fInvDLength;
            iQuantity++;
		}

        fT = (-fB + fRoot)*fInv;
        fTmp = kP.z + fT*kD.z;
        
		if ((0.0f <= fTmp) && (fTmp <= fWLength)) {
            afT[iQuantity++] = fT*fInvDLength;
		}

        if (iQuantity == 2) {
            // line intersects capsule wall in two places
            return 2;
        }
    } else {
        // line is tangent to infinite cylinder
        fT = -fB/fA;
        fTmp = kP.z + fT*kD.z;
        if ((0.0f <= fTmp) && (fTmp <= fWLength)) {
            afT[0] = fT*fInvDLength;
            return 1;
        }
    }

    // test intersection with bottom hemisphere
    // fA = 1
    fB += kP.z*kD.z;
    fC += kP.z*kP.z;
    fDiscr = fB*fB - fC;
    if (fDiscr > 0.0f) {
        fRoot = sqrt(fDiscr);
        fT = -fB - fRoot;
        fTmp = kP.z + fT*kD.z;
        if (fTmp <= 0.0f) {
            afT[iQuantity++] = fT*fInvDLength;
            if (iQuantity == 2) {
                return 2;
			}
        }

        fT = -fB + fRoot;
        fTmp = kP.z + fT*kD.z;
        if (fTmp <= 0.0f) {
            afT[iQuantity++] = fT*fInvDLength;
            if (iQuantity == 2) {
                return 2;
			}
        }
    } else if (fDiscr == 0.0f) {
        fT = -fB;
        fTmp = kP.z + fT*kD.z;
        if (fTmp <= 0.0f) {
            afT[iQuantity++] = fT*fInvDLength;
            if (iQuantity == 2) {
                return 2;
			}
        }
    }

    // test intersection with top hemisphere
    // fA = 1
    fB -= kD.z*fWLength;
    fC += fWLength*(fWLength - 2.0f*kP.z);

    fDiscr = fB*fB - fC;
    if (fDiscr > 0.0f) {
        fRoot = sqrt(fDiscr);
        fT = -fB - fRoot;
        fTmp = kP.z + fT*kD.z;
        if (fTmp >= fWLength) {
            afT[iQuantity++] = fT*fInvDLength;
            if (iQuantity == 2) {
                return 2;
			}
        }

        fT = -fB + fRoot;
        fTmp = kP.z + fT*kD.z;
        if (fTmp >= fWLength) {
            afT[iQuantity++] = fT*fInvDLength;
            if (iQuantity == 2) {
                return 2;
			}
        }
    } else if (fDiscr == 0.0f) {
        fT = -fB;
        fTmp = kP.z + fT*kD.z;
        if (fTmp >= fWLength) {
            afT[iQuantity++] = fT*fInvDLength;
            if (iQuantity == 2) {
                return 2;
			}
        }
    }

    return iQuantity;
}


/** Used by collisionTimeForMovingPointFixedCapsule.
    @cite From magic software http://www.magic-software.com/Source/Intersection3D/MgcIntr3DLinCap.cpp
	
	@param rkRay      The ray
	@param rkCapsule  The capsule
	@param riQuantity The number of intersections found
	@param akPoint    The intersections found
	@return           True if there is at least one intersection
	*/
static bool findRayCapsuleIntersection(
	const Ray&			rkRay,
	const Capsule&		rkCapsule,
	int&				riQuantity,
	Vector3				akPoint[2]) {

    double afT[2];
    riQuantity = findRayCapsuleIntersectionAux(rkRay.origin, rkRay.direction, rkCapsule, afT);

    // Only return intersections that occur in the future
    int iClipQuantity = 0;
	int i;
    for (i = 0; i < riQuantity; i++) {
        if (afT[i] >= 0.0f) {
            akPoint[iClipQuantity] = rkRay.origin + afT[i] * rkRay.direction;
            iClipQuantity++;
        }
    }

    riQuantity = iClipQuantity;
    return (riQuantity > 0);
}

Real CollisionDetection::collisionTimeForMovingPointFixedCapsule(
	const Vector3&		point,
	const Vector3&		velocity,
	const Capsule&		capsule,
	Vector3&		    location,
    Vector3&            outNormal) {

	double timeScale = velocity.length();

    if (timeScale == 0.0) {
        timeScale = 1;
    }

	Vector3 direction = velocity / timeScale;
	int numIntersections;
	Vector3 intersection[2];
	findRayCapsuleIntersection(Ray::fromOriginAndDirection(point, direction), capsule, numIntersections, intersection);

	if (numIntersections == 2) {
		// A collision can only occur if there are two intersections.  If there is one
		// intersection, that one is exiting the capsule.  

		// Find the entering intersection (the first one that occurs).
		double d0 = (intersection[0] - point).squaredLength();
		double d1 = (intersection[1] - point).squaredLength();

        // Compute the surface normal (if we aren't ignoring the result)
        if (&outNormal != &ignore) {
            Vector3 p2 = LineSegment::fromTwoPoints(capsule.getPoint1(), capsule.getPoint2()).closestPoint(point);
            outNormal = (point - p2).direction();
        }

        if (d0 > d1) {
			location = intersection[1];
			return sqrt(d1) / timeScale;
		} else {
			location = intersection[0];
			return sqrt(d0) / timeScale;
		}
	} else {
		// No entering intersection discovered; return no intersection.
		location = Vector3::INF3;
		return inf;
	}
}


Real CollisionDetection::collisionTimeForMovingSphereFixedPlane(
    const Sphere&		sphere,
    const Vector3&		velocity,
    const Plane&		plane,
    Vector3&			location,
    Vector3&            outNormal) {

	if (sphere.radius == 0) {
		// Optimization for zero radius sphere
        return collisionTimeForMovingPointFixedPlane(sphere.center, velocity, plane, location, outNormal);
	}

    // The collision point on the sphere will be the point at
    // center - (radius * normal).  Collisions only occur when
    // the sphere is travelling into the plane.

    Real d;
    plane.getEquation(outNormal, d);
    
    double vdotN = velocity.dot(outNormal);

    if (fuzzyGt(vdotN, 0)) {
        // No collision when the sphere is moving towards a backface.
        location = Vector3::INF3;
        return infReal;
    }

    double cdotN = sphere.center.dot(outNormal);

    // Distance from the center to the plane
    double distance = cdotN + d;

    // Where is the collision on the sphere?
    Vector3 point = sphere.center - (sphere.radius * outNormal);

    if (fuzzyLe(G3D::abs(distance), sphere.radius)) {
        // Already interpenetrating
        location = sphere.center - distance * outNormal;
        return 0;
    } else {
        return collisionTimeForMovingPointFixedPlane(point, velocity, plane, location, outNormal);
    }

}


Real CollisionDetection::collisionTimeForMovingSphereFixedTriangle(
    const class Sphere&		sphere,
    const Vector3&		    velocity,
    const CDTriangle&       triangle,
    Vector3&				outLocation,
    Vector3&                outNormal) {

    Vector3 dummy;

    outNormal = triangle.plane.getNormal();
    double time = collisionTimeForMovingSphereFixedPlane(sphere, velocity, triangle.plane, outLocation, dummy);

    if (time == infReal) {
        // No collision is ever going to happen
        return time;
    }

    if (isPointInsideTriangle(triangle.vertex[0], triangle.vertex[1], triangle.vertex[2], triangle.plane.getNormal(), outLocation, triangle.primaryAxis)) {
        // The intersection point is inside the triangle; that is the location where
        // the sphere hits the triangle.
        return time;
    }

    // Switch over to moving the triangle towards a fixed sphere and see at what time
    // they will hit.

    // Closest point on the triangle to the sphere intersection with the plane.
    Vector3 point = closestPointToTrianglePerimeter(triangle.vertex, triangle.edgeDirection, triangle.edgeLength, outLocation);

    double t = collisionTimeForMovingPointFixedSphere(point, -velocity, sphere, dummy, dummy);

    // The collision occured at the point, if it occured.  The normal was the plane normal,
    // computed above.

    return t;
}


Real CollisionDetection::collisionTimeForMovingSphereFixedRectangle(
    const Sphere&       sphere,
    const Vector3&      velocity,
    const Vector3&      v0,
    const Vector3&      v1,
    const Vector3&      v2,
    const Vector3&      v3,
    Vector3&            location,
    Vector3&            outNormal) {

    Plane plane(v0, v1, v2);

    double time = collisionTimeForMovingSphereFixedPlane(sphere, velocity, plane, location, outNormal);

    if (time == infReal) {
        // No collision is ever going to happen
        return time;
    }

    if (isPointInsideRectangle(v0, v1, v2, v3, plane.getNormal(), location)) {
        // The intersection point is inside the rectangle; that is the location where
        // the sphere hits the rectangle.
        return time;
    }

    // Switch over to moving the rectangle towards a fixed sphere and see at what time
    // they will hit.

    Vector3 point = closestPointToRectanglePerimeter(v0, v1, v2, v3, sphere.center);

    Vector3 dummy;
    double t = collisionTimeForMovingPointFixedSphere(point, -velocity, sphere, location, dummy);

    // Normal is the plane normal, location is the original location of the point.
    location = point;

    return t;
}


Real CollisionDetection::collisionTimeForMovingSphereFixedBox(
    const Sphere&       sphere,
    const Vector3&      velocity,
    const Box&          box,
    Vector3&            location,
    Vector3&            outNormal) {

    Real    bestTime;

    Vector3 v[4];
    int f = 0;
    box.getFaceCorners(f, v[0], v[1], v[2], v[3]);
    bestTime = collisionTimeForMovingSphereFixedRectangle(sphere, velocity, v[0], v[1], v[2], v[3], location, outNormal);

    for (f = 1; f < 6; ++f) {
        Vector3 pos, normal;
        box.getFaceCorners(f, v[0], v[1], v[2], v[3]);
        Real time = collisionTimeForMovingSphereFixedRectangle(sphere, velocity, v[0], v[1], v[2], v[3], pos, normal);
        if (time < bestTime) {
            bestTime  = time;
            location  = pos;
            outNormal = normal;
        }
    }

    return bestTime;
}


Real CollisionDetection::collisionTimeForMovingSphereFixedCapsule(
	const Sphere&		sphere,
	const Vector3&		velocity,
	const Capsule&		capsule,
	Vector3&		    location,
    Vector3&            outNormal) {

	Capsule _capsule(capsule.getPoint1(), capsule.getPoint2(), capsule.getRadius() + sphere.radius);

    Vector3 normal;
	double time = collisionTimeForMovingPointFixedCapsule(sphere.center, velocity, _capsule, location, normal);
    
    if (time < inf) {
        // Location is now the position of the center of the sphere at the time of collision.
        // We have to adjust the collision location for the size of the sphere.
        location -= sphere.radius * normal;
    }

    return time;
}


Vector3 CollisionDetection::bounceDirection(
    const Sphere&   sphere,
    const Vector3&  velocity,
    const Real      collisionTime,
    const Vector3&  collisionLocation,
    const Vector3&  collisionNormal) {

	// Location when the collision occurs
    Vector3 sphereLocation  = sphere.center + velocity * collisionTime;

    Vector3 normal          = (sphereLocation - collisionLocation);
    if (fuzzyEq(normal.squaredLength(), 0)) {
        normal = collisionNormal;
    } else {
        normal.unitize();
    }

    Vector3 direction       = velocity.direction();

    // Reflect direction about the normal
    return direction - 2 * normal * normal.dot(direction);
}


Vector3 CollisionDetection::slideDirection(
    const Sphere&   sphere,
    const Vector3&  velocity,
    const Real      collisionTime,
    const Vector3&  collisionLocation) {

    Vector3 sphereLocation  = sphere.center + velocity * collisionTime;
    Vector3 normal          = (sphereLocation - collisionLocation).direction();
    Vector3 direction       = velocity.direction();

    // subtract off the part in the direction away from the normal.
    return direction - normal * normal.dot(direction);
}


Vector3 CollisionDetection::closestPointOnLineSegment(
    const Vector3& v0,
    const Vector3& v1,
    const Vector3& point) {

    Vector3 edge       = (v1 - v0);
    double  edgeLength = edge.length();

    return closestPointOnLineSegment(v0, v1, edge / edgeLength, edgeLength, point);
}


Vector3 CollisionDetection::closestPointOnLineSegment(
    const Vector3& v0,
    const Vector3& v1,
    const Vector3& edgeDirection,
    const double   edgeLength,
    const Vector3& point) {

    debugAssert((v1 - v0).direction().fuzzyEq(edgeDirection));
    debugAssert(fuzzyEq((v1 - v0).length(), edgeLength));

    Vector3 c = point - v0;
    double t = edgeDirection.dot(c);

    if (t < 0) {
        return v0;
    } else if (t > edgeLength) {
        return v1;
    } else {
        return v0 + edgeDirection * t;
    }
}


Vector3 CollisionDetection::closestPointToTrianglePerimeter(
    const Vector3&			v0, 
    const Vector3&			v1,
    const Vector3&			v2,
    const Vector3&			point) {
    
    Vector3 v[3] = {v0, v1, v2};
    Vector3 edgeDirection[3] = {(v1 - v0), (v2 - v1), (v0 - v2)};
    double edgeLength[3];
    
    for (int i = 0; i < 3; ++i) {
        edgeLength[i] = edgeDirection[i].length();
        edgeDirection[i] /= edgeLength[i];
    }

    return closestPointToTrianglePerimeter(v, edgeDirection, edgeLength, point);
}


Vector3 CollisionDetection::closestPointToTrianglePerimeter(
    const Vector3   v[3],
    const Vector3   edgeDirection[3],
    const double    edgeLength[3],
    const Vector3&  point) {

    // Closest point on each segment
    Vector3 r[3];

    // Distance squared
    double d[3];

    // Index of the next point
    static int next[] = {1, 2, 0};

    for (int i = 0; i < 3; ++i) {
        r[i] = closestPointOnLineSegment(v[i], v[next[i]], edgeDirection[i], edgeLength[i], point);
        d[i] = (r[i] - point).squaredLength();
    }

    if (d[0] < d[1]) {
        if (d[0] < d[2]) {
            return r[0];
        } else {
            return r[2];
        }
    } else {
        if (d[1] < d[2]) {
            return r[1];
        } else {
            return r[2];
        }
    }
}


bool CollisionDetection::isPointInsideTriangle(
    const Vector3&			v0,
    const Vector3&			v1,
    const Vector3&			v2,
    const Vector3&			normal,
    const Vector3&			point,
    CollisionDetection::Axis  primaryAxis) {
    
    if (primaryAxis == DETECT_AXIS) {
        primaryAxis = normalToPrimaryAxis(normal);
    }

    // Check that the point is within the triangle using a Barycentric
    // coordinate test on a two dimensional plane.
    int i, j;

    switch (primaryAxis) {
    case X_AXIS:
        i = Z_AXIS;
        j = Y_AXIS;
        break;

    case Y_AXIS:
        i = Z_AXIS;
        j = X_AXIS;
        break;

    case Z_AXIS:
        i = X_AXIS;
        j = Y_AXIS;
        break;
    }


    // 2D area via crossproduct
    #define AREA2(d, e, f)  (((e)[i] - (d)[i]) * ((f)[j] - (d)[j]) - ((f)[i] - (d)[i]) * ((e)[j] - (d)[j]))

    // Area of the polygon
    Real area = AREA2(v0, v1, v2);

    debugAssert(area != 0);

    Real a = AREA2(point, v1, v2) / area;

    if (a < 0) {
        return false;
    }

    Real b = AREA2(v0,  point, v2) / area;

    if ((b < 0) || ((1 - (a + b)) < 0)) {
        return false;
    }

    #undef AREA2

    return true;
}


bool CollisionDetection::isPointInsideRectangle(
    const Vector3& v0,
    const Vector3& v1,
    const Vector3& v2,
    const Vector3& v3,
    const Vector3& normal,
    const Vector3& point) {

    return isPointInsideTriangle(v0, v1, v2, normal, point) ||
           isPointInsideTriangle(v2, v3, v0, normal, point);  
}


Vector3 CollisionDetection::closestPointToRectanglePerimeter(
    const Vector3& v0,
    const Vector3& v1,
    const Vector3& v2,
    const Vector3& v3,
    const Vector3& point) {

    Vector3 r0 = closestPointOnLineSegment(v0, v1, point);
    Vector3 r1 = closestPointOnLineSegment(v1, v2, point);
    Vector3 r2 = closestPointOnLineSegment(v2, v3, point);
    Vector3 r3 = closestPointOnLineSegment(v3, v0, point);

    double d0 = (r0 - point).squaredLength();
    double d1 = (r1 - point).squaredLength();
    double d2 = (r2 - point).squaredLength();
    double d3 = (r3 - point).squaredLength();

    if (d0 < d1) {
        if (d0 < d2) {
            if (d0 < d3) {
                return r0;
            } else {
                return r3;
            }
        } else {
            if (d2 < d3) {
                return r2;
            } else {
                return r3;
            }
        }
    } else {
        if (d1 < d2) {
            if (d1 < d3) {
                return r1;
            } else {
                return r3;
            }
        } else {
            if (d2 < d3) {
                return r2;
            } else {
                return r3;
            }
        }
    }
}


Vector3 CollisionDetection::closestPointToRectangle(
    const Vector3&      v0,
    const Vector3&      v1,
    const Vector3&      v2,
    const Vector3&      v3,
    const Vector3&      point) {

    Plane plane(v0, v1, v2);

    // Project the point into the plane
    float a, b, c, d;
    plane.getEquation(a, b, c, d);
    
    double distance = a*point.x + b*point.y + c*point.z + d;
    Vector3 planePoint = point - distance * plane.getNormal();

    if (isPointInsideRectangle(v0, v1, v2, v3, plane.getNormal(), planePoint)) {
        return planePoint;
    } else {
        return closestPointToRectanglePerimeter(v0, v1, v2, v3, planePoint);
    }
}


bool CollisionDetection::fixedSolidSphereIntersectsFixedSolidSphere(
    const Sphere&           sphere1,
    const Sphere&           sphere2) {
    
    return (sphere1.center - sphere2.center).squaredLength() < square(sphere1.radius + sphere2.radius);
}


bool CollisionDetection::fixedSolidSphereIntersectsFixedSolidBox(
    const Sphere&           sphere,
    const Box&              box) {

    // If the center of the sphere is within the box, the whole
    // sphere is within the box.
    if (box.contains(sphere.center)) {
        return true;
    }

    double r2 = square(sphere.radius);

    // Find the closest point on the surface of the box to the sphere.  If
    // this point is within the sphere's radius, they intersect.
    int f;
    for (f = 0; f < 6; ++f) {
        Vector3 v0, v1, v2, v3;
        box.getFaceCorners(f, v0, v1, v2, v3);
        if ((closestPointToRectangle(v0, v1, v2, v3, sphere.center) - sphere.center).squaredLength() <= r2) {
            return true;
        }
    }

    return false;
}


bool CollisionDetection::movingSpherePassesThroughFixedBox(
    const Sphere&           sphere,
    const Vector3&          velocity,
    const Box&              box,
    double                  timeLimit) {

    // If they intersect originally, they definitely pass through each other.
    if (fixedSolidSphereIntersectsFixedSolidBox(sphere, box)) {
        return true;
    }

    // See if the sphere hits the box during the time period.
    Vector3 dummy1, dummy2;

    return (collisionTimeForMovingSphereFixedBox(sphere, velocity, box, dummy1, dummy2) < timeLimit);
}


bool CollisionDetection::movingSpherePassesThroughFixedSphere(
    const Sphere&           sphere,
    const Vector3&          velocity,
    const Sphere&           fixedSphere,
    double                  timeLimit) {

    if (fixedSolidSphereIntersectsFixedSolidSphere(sphere, fixedSphere)) {
        return true;
    }

    // Extend the fixed sphere by the radius of the moving sphere
    Sphere bigFixed(fixedSphere.center, fixedSphere.radius + sphere.radius);
    Vector3 dummy1, dummy2;

    // If the sphere collides with the other sphere during the time limit, it passes through
    return (collisionTimeForMovingPointFixedSphere(sphere.center, velocity, bigFixed, dummy1, dummy2) < timeLimit);
}


} // namespace
