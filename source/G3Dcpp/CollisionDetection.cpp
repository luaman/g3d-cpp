/**
  @file CollisionDetection.cpp
  
  @maintainer Morgan McGuire, matrix@graphics3d.com
  @cite Bounce direction based on Paul Nettle's ftp://ftp.3dmaileffects.com/pub/FluidStudios/CollisionDetection/Fluid_Studios_Generic_Collision_Detection_for_Games_Using_Ellipsoids.pdf and comments by Max McGuire.  Ray-sphere code by Eric Haines.

  @created 2001-11-24
  @edited  2004-03-19
 */

#include "G3D/CollisionDetection.h"
#include "G3D/debugAssert.h"
#include "G3D/Capsule.h"
#include "G3D/Plane.h"
#include "G3D/Line.h"
#include "G3D/LineSegment.h"
#include "G3D/Sphere.h"
#include "G3D/Box.h"
#include "G3D/Triangle.h"
#include "G3D/Vector3.h"
#include "G3D/AABox.h"

namespace G3D {

bool CollisionDetection::ignoreBool;
Vector3	CollisionDetection::ignore;
Array<Vector3> CollisionDetection::ignoreArray;

double CollisionDetection::penetrationDepthForFixedSphereFixedBox(
    const Sphere&   sphere,
    const Box&      box,
    Array<Vector3>& contactPoints,
    Array<Vector3>& contactNormals) {

    contactPoints.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);
    contactNormals.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);

    // In its local coordinate frame, the box measures
    // 2 * halfExtent[a] along dimesion a.
    Vector3 halfExtent(box.extent(0), box.extent(1), box.extent(2));
    halfExtent *= 0.5;

    CoordinateFrame boxFrame;
    box.getLocalFrame(boxFrame);

    // Transform the sphere to the box's coordinate frame.
    Vector3 center = boxFrame.pointToObjectSpace(sphere.center);

    // Find the square of the distance
    // from the sphere to the box
    float d = 0;

    int    minAxis = -1;
    float  minDist = inf;

    // Distance along each axis from the closest side of the box
    // to the sphere center.  Negative values are *inside* the box.
    Vector3 distOutsideBox;

    // Divide space up into the 27 regions corresponding
    // to {+|-|0}X, {+|-|0}Y, {+|-|0}Z and classify the
    // sphere center into one of them.
    Vector3 centerRegion;

    // In the edge collision case, the edge is between vertices
    // (constant + variable) and (constant - variable).
    Vector3 constant, variable;

    int numNonZero = 0;

    // Iterate over axes
    for (int a = 0; a < 3; ++a) { 
        // For each (box side), see which direction the sphere
        // is outside the box (positive or negative).  Add the
        // square of that distance to the total distance from 
        // the box.

        double distanceFromLow  = -halfExtent[a] - center[a];
        double distanceFromHigh = center[a] - halfExtent[a];

        if (abs(distanceFromLow) < abs(distanceFromHigh)) {
            distOutsideBox[a] = distanceFromLow;
        } else {
            distOutsideBox[a] = distanceFromHigh;
        }

        if (distanceFromLow < 0.0) {
            if (distanceFromHigh < 0.0) {
                // Inside the box
                centerRegion[a] = 0.0;
                variable[a]     = 1.0;
            } else {
                // Off the high side
                centerRegion[a] = 1.0;
                constant[a]     = halfExtent[a];
                ++numNonZero;
            }
        } else if (distanceFromHigh < 0.0) {
            // Off the low side
            centerRegion[a] = -1.0;
            constant[a]     = -halfExtent[a];
            ++numNonZero;
        } else {
            debugAssertM(false, 
                "distanceFromLow and distanceFromHigh cannot both be positive");
        }
    }

    // Squared distance between the outside of the box and the
    // sphere center.
    double d2 = Vector3::ZERO.max(distOutsideBox).squaredLength();

    if (d2 > square(sphere.radius)) {
        // There is no penetration because the distance is greater
        // than the radius of the sphere.  This is the common case
        // and we quickly exit.
        return -1;
    }

    // We know there is some penetration but need to classify it.
    //
    // Examine the region that contains the center of the sphere. If
    // there is exactly one non-zero axis, the collision is with a 
    // plane.  If there are exactly two non-zero axes, the collision
    // is with an edge.  If all three axes are non-zero, the collision is
    // with a vertex.  If there are no non-zero axes, the center is inside
    // the box.

    double depth = -1;
    switch (numNonZero) {
    case 3: // Vertex collision
        // The collision point is the vertex at constant, the normal
        // is the vector from there to the sphere center.
        contactNormals.append(boxFrame.normalToWorldSpace(constant - center));
        contactPoints.append(boxFrame.pointToWorldSpace(constant));
        depth = sphere.radius - sqrt(d2);
        break;

    case 2: // Edge collision
        {
            // TODO: unwrapping the edge constructor and closest point
            // code will probably make it faster.

            // Determine the edge
            Line line = Line::fromPointAndDirection(constant, variable);

            // Penetration depth:
            depth = sphere.radius - sqrt(d2);

            // The contact point is the closes point to the sphere on the line 
            Vector3 X = line.closestPoint(center);
            contactNormals.append(boxFrame.normalToWorldSpace(X - center).direction());
            contactPoints.append(boxFrame.pointToWorldSpace(X));
        }
        break;

    case 1: // Plane collision
        {
            // The plane normal is the centerRegion vector,
            // so the sphere normal is the negative.  Take
            // it to world space from box-space.

            // Center region doesn't need to be normalized because
            // it is known to contain only one non-zero value
            // and that value is +/- 1.
            Vector3 N = boxFrame.normalToWorldSpace(-centerRegion);
            contactNormals.append(N);

            // Penetration depth:
            depth = sphere.radius - sqrt(d2);

            // Compute the contact point from the penetration depth
            contactPoints.append(sphere.center + N * (sphere.radius - depth));
        }
        break;

    case 0: // Volume collision

        // The sphere center is inside the box.  This is an easy case
        // to handle.  Note that all axes of distOutsideBox must
        // be negative.  
    
        // Arbitratily choose the sphere center as a contact point
        contactPoints.append(sphere.center);

        // Find the least-negative penetration axis.
        //
        // We could have computed this during the loop over the axes,
        // but since volume collisions are rare (they only occur with
        // large time steps), this case will seldom be executed and
        // should not be optimized at the expense of the others.
        if (distOutsideBox.x > distOutsideBox.y) {
            if (distOutsideBox.x > distOutsideBox.z) {
                // Smallest penetration on x-axis
                // Chose normal based on which side we're closest to.
                // Keep in mind that this is a normal to the sphere,
                // so it is the inverse of the box normal.
                if (center.x > 0) {
                    contactNormals.append(boxFrame.normalToWorldSpace(-Vector3::UNIT_X));
                } else {
                    contactNormals.append(boxFrame.normalToWorldSpace(Vector3::UNIT_X));
                }
                depth = -distOutsideBox.x;
            } else {
                // Smallest penetration on z-axis
                goto ZAXIS;
            }
        } else if (distOutsideBox.y > distOutsideBox.z) {
            // Smallest penetration on y-axis
            // Chose normal based on which side we're closest to.
            // Keep in mind that this is a normal to the sphere,
            // so it is the inverse of the box normal.
            if (center.y > 0) {
                contactNormals.append(boxFrame.normalToWorldSpace(-Vector3::UNIT_Y));
            } else {
                contactNormals.append(boxFrame.normalToWorldSpace(Vector3::UNIT_Y));
            }
            depth = -distOutsideBox.y;
        } else {
            // Smallest on z-axis
ZAXIS:
            // Chose normal based on which side we're closest to.
            // Keep in mind that this is a normal to the sphere,
            // so it is the inverse of the box normal.
            if (center.z > 0) {
                contactNormals.append(boxFrame.normalToWorldSpace(-Vector3::UNIT_Z));
            } else {
                contactNormals.append(boxFrame.normalToWorldSpace(Vector3::UNIT_Z));
            }
            depth = -distOutsideBox.z;
        }
        break;

    default:
        debugAssertM(false, "Fell through switch");
        break;
    }

    return depth;
}


double CollisionDetection::penetrationDepthForFixedSphereFixedSphere(
    const Sphere&           sphereA,
    const Sphere&           sphereB,
    Array<Vector3>&         contactPoints,
    Array<Vector3>&         contactNormals) {

    Vector3 axis = sphereB.center - sphereA.center;
    double radius = sphereA.radius + sphereB.radius;
    double len = axis.length();
    axis /= len;
    double depth = -(len - radius);

    contactPoints.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);
    contactNormals.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);

    if (depth >= 0) {
        contactPoints.append(sphereA.center + axis * (sphereA.radius - depth / 2));
        contactNormals.append(axis);
    }

    return depth;
}


double CollisionDetection::penetrationDepthForFixedSphereFixedPlane(
    const Sphere&           sphereA,
    const Plane&            planeB,
    Array<Vector3>&         contactPoints,
    Array<Vector3>&         contactNormals) {

    Vector3 N;
    double d;

    planeB.getEquation(N, d);
    
    double depth = -(sphereA.center.dot(N) + d - sphereA.radius);

    contactPoints.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);
    contactNormals.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);

    if (depth >= 0) {
        contactPoints.append(N * (-depth - d) + sphereA.center);
        contactNormals.append(-N);
    }

    return depth;
}


double CollisionDetection::penetrationDepthForFixedBoxFixedPlane(
    const Box&          box,
    const Plane&        plane,
    Array<Vector3>&     contactPoints,
    Array<Vector3>&     contactNormals) {

    Vector3 N;
    double d;
    
    plane.getEquation(N, d);

    contactPoints.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);
    contactNormals.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);

    double lowest = inf;
    for (int i = 0; i < 8; ++i) {
        const Vector3 vertex = box.corner(i);
        
        double x = vertex.dot(N) + d;
        
        if (x <= 0) {
            // All vertices below the plane should be contact points.
            contactPoints.append(vertex);
            contactNormals.append(-N);
        }

        lowest = min(lowest, x);
    }

    // Depth should be a positive number
    return -lowest;
}


double CollisionDetection::collisionTimeForMovingPointFixedPlane(
    const Vector3&  point,
    const Vector3&  velocity,
    const Plane&    plane,
    Vector3&        location,
    Vector3&        outNormal) {

    // Solve for the time at which normal.dot(point + velocity) + d == 0.
    double d;
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
        return inf;
    }

    double t = -(pdotN + d) / vdotN;
    if (t < 0) {
        location = Vector3::INF3;
        return inf;
    } else {
        location = point + velocity * t;
        outNormal = normal;
        return t;
    }
}


double CollisionDetection::collisionTimeForMovingPointFixedSphere(
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
        return inf;
    }

    double M2 = L2 - D2;

    if (M2 > R2) {
        location = Vector3::INF3;
        return inf;
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


double CollisionDetection::collisionTimeForMovingSphereFixedSphere(
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
        location += (location - fixedSphere.center) * movingSphere.radius / fixedSphere.radius;
    }

    return time;
}


/*
double CollisionDetection::collisionTimeForMovingPointFixedTriangle(
    const Vector3&			point,
    const Vector3&			velocity,
    const Triangle&       triangle,
    Vector3&				outLocation,
    Vector3&                outNormal) {

    double time = collisionTimeForMovingPointFixedPlane(point, velocity, triangle.plane(), outLocation, outNormal);

    if (time == inf) {
        // No collision with the plane of the triangle.
        return inf;
    }

    if (isPointInsideTriangle(triangle.vertex(0), triangle.vertex(1), triangle.vertex(2), triangle.normal(), outLocation, triangle.primaryAxis())) {
        // Collision occured inside the triangle
        return time;
    } else {
        // Missed the triangle
        outLocation = Vector3::INF3;
        return inf;
    }
}*/

/*
double CollisionDetection::collisionTimeForMovingPointFixedTriangle(
    const Vector3& orig,
    const Vector3& dir,
    const Vector3& vert0,
    const Vector3& vert1,
    const Vector3& vert2) {

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
    CROSS(pvec, dir, edge2);
    
    // if determinant is near zero, ray lies in plane of triangle
    const double det = DOT(edge1, pvec);
    
    if (det < EPSILON) {
        return inf;
    }
    
    // calculate distance from vert0 to ray origin
    SUB(tvec, orig, vert0);
    
    // calculate U parameter and test bounds
    u = DOT(tvec, pvec);
    if ((u < 0.0) || (u > det)) {
        // Hit the plane outside the triangle
        return inf;
    }
    
    // prepare to test V parameter
    CROSS(qvec, tvec, edge1);
    
    // calculate V parameter and test bounds
    v = DOT(dir, qvec);
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
*/

double CollisionDetection::collisionTimeForMovingPointFixedBox(
    const Vector3&          point,
    const Vector3&          velocity,
    const Box&              box,
    Vector3&                location,
    Vector3&                outNormal) {

    double    bestTime;

    Vector3 normal;
    Vector3 v[4];
    int f = 0;
    box.getFaceCorners(f, v[0], v[1], v[2], v[3]);
    bestTime = collisionTimeForMovingPointFixedRectangle(point, velocity, v[0], v[1], v[2], v[3], location, normal);

    outNormal = normal;

    for (f = 1; f < 6; ++f) {
        Vector3 pos;
        box.getFaceCorners(f, v[0], v[1], v[2], v[3]);
        double time = collisionTimeForMovingPointFixedRectangle(point, velocity, v[0], v[1], v[2], v[3], pos, outNormal);
        if (time < bestTime) {
            bestTime = time;
            outNormal = normal;
            location = pos;
        }
    }

    return bestTime;
}



double CollisionDetection::collisionTimeForMovingPointFixedAABox(
    const Vector3&          origin,
    const Vector3&          dir,
    const AABox&            box,
    Vector3&                location,
    bool&                   Inside) {

    if (collisionLocationForMovingPointFixedAABox(origin, dir, box, location, Inside)) {
        return (location - origin).length();
    } else {
        return inf;
    }
}


bool CollisionDetection::collisionLocationForMovingPointFixedAABox(
    const Vector3&          origin,
    const Vector3&          dir,
    const AABox&            box,
    Vector3&                location,
    bool&                   Inside) {

    // Integer representation of a floating-point value.
    #define IR(x)	((uint32&)x)

    Inside = true;
	const Vector3& MinB = box.low();
	const Vector3& MaxB = box.high();
	Vector3 MaxT(-1.0f, -1.0f, -1.0f);

	// Find candidate planes.
    for (int i = 0; i < 3; ++i) {
		if (origin[i] < MinB[i]) {
			location[i]	= MinB[i];
			Inside      = false;

			// Calculate T distances to candidate planes
            if (IR(dir[i])) {
                MaxT[i] = (MinB[i] - origin[i]) / dir[i];
            }
        } else if (origin[i] > MaxB[i]) {
			location[i]	= MaxB[i];
			Inside	    = false;

			// Calculate T distances to candidate planes
            if (IR(dir[i])) {
                MaxT[i] = (MaxB[i] - origin[i]) / dir[i];
            }
		}
	}

	if (Inside) {
    	// Ray origin inside bounding box
		return false;
	}

	// Get largest of the maxT's for final choice of intersection
	int WhichPlane = 0;
    if (MaxT[1] > MaxT[WhichPlane])	{
        WhichPlane = 1;
    }

    if (MaxT[2] > MaxT[WhichPlane])	{
        WhichPlane = 2;
    }

	// Check final candidate actually inside box
    if (IR(MaxT[WhichPlane]) & 0x80000000) {
        // Miss the box
        return false;
    }

	for (int i = 0; i < 3; ++i) {
        if (i != WhichPlane) {
			location[i] = origin[i] + MaxT[WhichPlane] * dir[i];
            if ((location[i] < MinB[i]) ||
                (location[i] > MaxB[i])) {
                // On this plane we're outside the box extents, so
                // we miss the box
                return false;
            }
		}
	}

	return true;

    #undef IR
}



double CollisionDetection::collisionTimeForMovingPointFixedRectangle(
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

    if (time == inf) {
        // No collision is ever going to happen
        return time;
    }

    if (isPointInsideRectangle(v0, v1, v2, v3, plane.normal(), location)) {
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
    float fWLength = kW.unitize();
    Vector3::generateOrthonormalBasis(kU, kV, kW);
    Vector3 kD(kU.dot(rkDirection), kV.dot(rkDirection), kW.dot(rkDirection));
    float fDLength = kD.unitize();

    float fEpsilon = 1e-6f;

    float fInvDLength = 1.0f/fDLength;
    Vector3 kDiff = rkOrigin - rkCapsule.getPoint1();
    Vector3 kP(kU.dot(kDiff),kV.dot(kDiff),kW.dot(kDiff));
    float fRadiusSqr = square(rkCapsule.getRadius());

    float fInv, fA, fB, fC, fDiscr, fRoot, fT, fTmp;

    // Is the velocity parallel to the capsule direction? (or zero)
    if ((abs(kD.z) >= 1.0f - fEpsilon) || (fDLength < fEpsilon)) {

        float fAxisDir = rkDirection.dot(capsuleDirection);

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

double CollisionDetection::collisionTimeForMovingPointFixedCapsule(
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


double CollisionDetection::collisionTimeForMovingSphereFixedPlane(
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

    double d;
    plane.getEquation(outNormal, d);
    
    double vdotN = velocity.dot(outNormal);

    if (fuzzyGt(vdotN, 0)) {
        // No collision when the sphere is moving towards a backface.
        location = Vector3::INF3;
        return inf;
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


double CollisionDetection::collisionTimeForMovingSphereFixedTriangle(
    const class Sphere&		sphere,
    const Vector3&		    velocity,
    const Triangle&       triangle,
    Vector3&				outLocation,
    Vector3&                outNormal) {

    Vector3 dummy;

    outNormal = triangle.normal();
    double time = collisionTimeForMovingSphereFixedPlane(sphere, velocity, triangle.plane(), outLocation, dummy);

    if (time == inf) {
        // No collision is ever going to happen
        return time;
    }

    if (isPointInsideTriangle(triangle.vertex(0), triangle.vertex(1), triangle.vertex(2), triangle.normal(), outLocation, triangle.primaryAxis())) {
        // The intersection point is inside the triangle; that is the location where
        // the sphere hits the triangle.
        return time;
    }

    // Switch over to moving the triangle towards a fixed sphere and see at what time
    // they will hit.

    // Closest point on the triangle to the sphere intersection with the plane.
    Vector3 point = closestPointToTrianglePerimeter(triangle._vertex, triangle.edgeDirection, triangle.edgeLength, outLocation);

    double t = collisionTimeForMovingPointFixedSphere(point, -velocity, sphere, dummy, dummy);

    // The collision occured at the point, if it occured.  The normal was the plane normal,
    // computed above.

    return t;
}


double CollisionDetection::collisionTimeForMovingSphereFixedRectangle(
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

    if (time == inf) {
        // No collision is ever going to happen
        return time;
    }

    if (isPointInsideRectangle(v0, v1, v2, v3, plane.normal(), location)) {
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


double CollisionDetection::collisionTimeForMovingSphereFixedBox(
    const Sphere&       sphere,
    const Vector3&      velocity,
    const Box&          box,
    Vector3&            location,
    Vector3&            outNormal) {

    // TODO: test if sphere is already inside box

    double    bestTime;

    Vector3 v[4];
    int f = 0;
    box.getFaceCorners(f, v[0], v[1], v[2], v[3]);
    bestTime = collisionTimeForMovingSphereFixedRectangle(sphere, velocity, v[0], v[1], v[2], v[3], location, outNormal);

    for (f = 1; f < 6; ++f) {
        Vector3 pos, normal;
        box.getFaceCorners(f, v[0], v[1], v[2], v[3]);
        float time = collisionTimeForMovingSphereFixedRectangle(sphere, velocity, v[0], v[1], v[2], v[3], pos, normal);
        if (time < bestTime) {
            bestTime  = time;
            location  = pos;
            outNormal = normal;
        }
    }

    return bestTime;
}


double CollisionDetection::collisionTimeForMovingSphereFixedCapsule(
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
    const float     collisionTime,
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
    return direction - 2.0 * normal * normal.dot(direction);
}


Vector3 CollisionDetection::slideDirection(
    const Sphere&   sphere,
    const Vector3&  velocity,
    const float      collisionTime,
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
    Vector3::Axis           primaryAxis) {
    
    if (primaryAxis == Vector3::DETECT_AXIS) {
        primaryAxis = normal.primaryAxis();
    }

    // Check that the point is within the triangle using a Barycentric
    // coordinate test on a two dimensional plane.
    int i, j;

    switch (primaryAxis) {
    case Vector3::X_AXIS:
        i = Vector3::Z_AXIS;
        j = Vector3::Y_AXIS;
        break;

    case Vector3::Y_AXIS:
        i = Vector3::Z_AXIS;
        j = Vector3::X_AXIS;
        break;

    case Vector3::Z_AXIS:
        i = Vector3::X_AXIS;
        j = Vector3::Y_AXIS;
        break;

    default:
        // This case is here to supress a warning on Linux
        i = j = 0;
        debugAssertM(false, "Should not get here.");
        break;
    }


    // 2D area via crossproduct
    #define AREA2(d, e, f)  (((e)[i] - (d)[i]) * ((f)[j] - (d)[j]) - ((f)[i] - (d)[i]) * ((e)[j] - (d)[j]))

    // Area of the polygon
    double  area = AREA2(v0, v1, v2);
    if (area == 0) {
        return (v0 == point);
    }

    debugAssert(area != 0);

    double a = AREA2(point, v1, v2) / area;

    if (a < 0) {
        return false;
    }

    double b = AREA2(v0,  point, v2) / area;

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
    double a, b, c, d;
    plane.getEquation(a, b, c, d);
    
    double distance = a*point.x + b*point.y + c*point.z + d;
    Vector3 planePoint = point - distance * plane.normal();

    if (isPointInsideRectangle(v0, v1, v2, v3, plane.normal(), planePoint)) {
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
