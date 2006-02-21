#include "../include/G3DAll.h"

void testCollisionDetection() {
    printf("CollisionDetection ");


    {
        Sphere s(Vector3(0,1,0), 1);
        Plane  p = Plane::fromEquation(0,1,0,0);
        Array<Vector3> contact;
        Array<Vector3> outNormal;

        float depth = CollisionDetection::penetrationDepthForFixedSphereFixedPlane(
            s, p, contact, outNormal);

        debugAssert(outNormal[0] == p.normal());
        debugAssert(depth == 0);

        s = Sphere(Vector3(0,2,0), 1);

        depth = CollisionDetection::penetrationDepthForFixedSphereFixedPlane(
            s, p, contact, outNormal);

        debugAssert(depth < 0);
    }

    {
        Sphere s(Vector3(0,1,0), 1);
        Sphere r(Vector3(0,-.5,0), 1);
        Array<Vector3> contact;
        Array<Vector3> outNormal;

        float depth = CollisionDetection::penetrationDepthForFixedSphereFixedSphere(
            s, r, contact, outNormal);
        (void)depth;

        debugAssert(outNormal[0] == Vector3(0,-1,0));
        debugAssert(depth == .5);
    }

    {
        Sphere s(Vector3(5, 0, 0), 1);
        Sphere f(Vector3(4.5, 0, 0), 1);
        debugAssert(CollisionDetection::fixedSolidSphereIntersectsFixedSolidSphere(s, f));
    }

    {
        Sphere s(Vector3(5, 0, 0), 1);
        Sphere f(Vector3(1, 0, 0), 2);
        debugAssert(! CollisionDetection::fixedSolidSphereIntersectsFixedSolidSphere(s, f));
    }

    {
        Sphere s(Vector3(5, 0, 0), 1);
        Box    f(Vector3(-1, -1.5f, -2), Vector3(4.5f, 2, 8));
        debugAssert(CollisionDetection::fixedSolidSphereIntersectsFixedSolidBox(s, f));
    }

    {
        Sphere s(Vector3(0, 0, 0), .1f);
        Box    f(Vector3(-1, -1.5f, -2), Vector3(4.5f, 2, 8));
        debugAssert(CollisionDetection::fixedSolidSphereIntersectsFixedSolidBox(s, f));
    }

    {
        Sphere s(Vector3(6, 0, 0), 1);
        Box    f(Vector3(-1, -1.5f, -2), Vector3(4.5f, 2, 8));
        debugAssert(! CollisionDetection::fixedSolidSphereIntersectsFixedSolidBox(s, f));
    }

    {
        
        Vector3 v0(0, 0, 0);
        Vector3 v1(0, 0, -1);
        Vector3 v2(-1, 0, 0);
        Ray ray = Ray::fromOriginAndDirection(Vector3(-.15f, 1,-.15f), Vector3(0, -1, 0));
        Vector3 location, normal;
        double t = ray.intersectionTime(v0, v1, v2);
        debugAssert(t == 1.0);

        ray.origin.y = -ray.origin.y;
        t = ray.intersectionTime(v0, v1, v2);
        debugAssert(t == inf());

        // One-sided test
        ray.direction.y = -ray.direction.y;
        ray.origin.y = -ray.origin.y;
        t = ray.intersectionTime(v0, v1, v2);
        debugAssert(t == inf());

        // Time scale
        ray = Ray::fromOriginAndDirection(Vector3(-.15f, 1,-.15f), Vector3(0, -2, 0));
        t = ray.intersectionTime(v0, v1, v2);

        debugAssert(t == 0.5);
    }

    printf("passed\n");
}
