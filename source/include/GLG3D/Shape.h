/**
  @file GLG3D/Shape.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2005-08-10
  @edited  2005-09-25
*/
#ifndef G3D_SHAPE_H
#define G3D_SHAPE_H

#include "../graphics3D.h"

namespace G3D {

typedef ReferenceCountedPointer<class Shape> ShapeRef;

/** 
   Base class for other shapes.  G3D primitives like Box and Cylinder
   have no base class so that their implementations are maximally efficient,
   and have no "render" method because they are lower-level than the
   rendering API.  
   
   The Shape classes provide a parallel set of classes to the G3D primitives
   that support more functionality.

   <b>BETA API</b> This API is subject to change in future releases.
  */
class Shape : public ReferenceCountedObject {
public:

    /** Mesh is reserved */
    enum Type {MESH=1, BOX, CYLINDER, SPHERE, RAY, CAPSULE, PLANE};

    static std::string typeToString(Type t);

    virtual Type type() = 0;
   
    virtual void render(
        class RenderDevice* rd, 
        const CoordinateFrame& cframe, 
        Color4 solidColor = Color4(.5,.5,0,.5), 
        Color4 wireColor = Color3::black()) = 0;

    virtual Box& box() { 
        debugAssertM(false, "Not a box");
        static Box b;
        return b;
    }

    virtual const Box& box() const { 
        debugAssertM(false, "Not a box");
        static Box b;
        return b;
    }

    virtual Ray& ray() {
        debugAssertM(false, "Not a ray");
        static Ray r;
        return r; 
    }
    
    virtual const Ray& ray() const {
        debugAssertM(false, "Not a ray");
        static Ray r;
        return r; 
    }

    virtual Cylinder& cylinder() { 
        debugAssertM(false, "Not a cylinder");
        static Cylinder c;
        return c; 
    }

    virtual const Cylinder& cylinder() const { 
        debugAssertM(false, "Not a cylinder");
        static Cylinder c;
        return c; 
    }

    virtual Sphere& sphere() { 
        debugAssertM(false, "Not a sphere");
        static Sphere s;
        return s; 
    }

    virtual const Sphere& sphere() const { 
        debugAssertM(false, "Not a sphere");
        static Sphere s;
        return s; 
    }

    virtual Capsule& capsule() { 
        debugAssertM(false, "Not a capsule");
        static Capsule c;
        return c; 
    }

    virtual const Capsule& capsule() const { 
        debugAssertM(false, "Not a capsule");
        static Capsule c;
        return c; 
    }

    virtual Plane& plane() { 
        debugAssertM(false, "Not a plane");
        static Plane p;
        return p;
    }

    virtual const Plane& plane() const { 
        debugAssertM(false, "Not a plane");
        static Plane p;
        return p;
    }


    /** Surface area of the outside of this object. */
    virtual float area() const = 0;

    /** Volume of the interior of this object. */
    virtual float volume() const = 0;

    /** A point selected uniformly at random with respect to the
        surface area of this object.  Not available on the Plane or
        Ray, which have infinite extent.  The normal has unit length
        and points out of the surface. */
    virtual void getRandomSurfacePoint(Vector3& P, 
                                       Vector3& N = Vector3::dummy) const = 0;

    /** A point selected uniformly at random with respect to the
        volume of this object.  Not available on objects with infinite
        extent.*/
    virtual Vector3 randomInteriorPoint() const = 0;

    virtual ~Shape() {}
};


class BoxShape : public Shape {
    
    G3D::Box          geometry;

public:

    explicit inline BoxShape(const G3D::Box& b) : geometry(b) {}

    /** Creates a box of the given extents centered at the origin. */
    explicit inline BoxShape(const Vector3& extent) : geometry(-extent/2, extent) {}

    /** Creates a box of the given extents centered at the origin. */
    inline BoxShape(float x, float y, float z) : geometry(Vector3(-x/2, -y/2, -z/2), Vector3(x/2, y/2, z/2)) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return BOX;
    }

    virtual Box& box() { 
        return geometry;
    }

    virtual const Box& box() const { 
        return geometry;
    }

    virtual float area() const {
        return geometry.area();
    }

    virtual float volume() const {
        return geometry.volume();
    }

    virtual void getRandomSurfacePoint(Vector3& P, 
                                       Vector3& N = Vector3::dummy) const {
        geometry.getRandomSurfacePoint(P, N);
    }

    virtual Vector3 randomInteriorPoint() const {
        return geometry.randomInteriorPoint();
    }
};


class RayShape : public Shape {
    
    G3D::Ray          geometry;

public:

    explicit inline RayShape(const G3D::Ray& r) : geometry(r) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return RAY;
    }

    virtual Ray& ray() { 
        return geometry;
    }

    virtual const Ray& ray() const { 
        return geometry;
    }

    virtual float area() const {
        return 0.0f;
    }

    virtual float volume() const {
        return 0.0f;
    }

    virtual void getRandomSurfacePoint(Vector3& P, Vector3& N = Vector3::dummy) const {
        P = Vector3::nan();
        N = Vector3::nan();
    }

    virtual Vector3 randomInteriorPoint() const {
        return Vector3::nan();
    }

};


class CylinderShape : public Shape {

    G3D::Cylinder       geometry;

public:

    explicit inline CylinderShape(const G3D::Cylinder& c) : geometry(c) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return CYLINDER;
    }

    virtual Cylinder& cylinder() { 
        return geometry;
    }

    virtual const Cylinder& cylinder() const {
        return geometry;
    }

    virtual float area() const {
        return geometry.area();
    }

    virtual float volume() const {
        return geometry.volume();
    }

    virtual void getRandomSurfacePoint(Vector3& P, 
                                       Vector3& N = Vector3::dummy) const {
        geometry.getRandomSurfacePoint(P, N);
    }

    virtual Vector3 randomInteriorPoint() const {
        return geometry.randomInteriorPoint();
    }

};


class SphereShape : public Shape {

    G3D::Sphere         geometry;

public:

    explicit inline SphereShape(const G3D::Sphere& s) : geometry(s) {}

    inline SphereShape(const Vector3& center, float radius) : geometry(center, radius) {}

    /** Creates a sphere centered at the origin with the given radius. */
    inline SphereShape(float radius) : geometry(Vector3::zero(), radius) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return SPHERE;
    }

    virtual Sphere& sphere() { 
        return geometry;
    }

    virtual const Sphere& sphere() const { 
        return geometry;
    }

    virtual void getRandomSurfacePoint(Vector3& P, Vector3& N = Vector3::dummy) const {
        P = geometry.randomSurfacePoint();
        N = (P - geometry.center).direction();
    }

    virtual Vector3 randomInteriorPoint() const {
        return geometry.randomInteriorPoint();
    }

    virtual float area() const {
        return geometry.area();
    }

    virtual float volume() const {
        return geometry.volume();
    }

};


class CapsuleShape : public Shape {
public:

    G3D::Capsule         geometry;

    explicit inline CapsuleShape(const G3D::Capsule& c) : geometry(c) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return CAPSULE;
    }

    virtual Capsule& capsule() { 
        return geometry;
    }

    virtual const Capsule& capsule() const { 
        return geometry;
    }

    virtual float area() const {
        return geometry.area();
    }

    virtual float volume() const {
        return geometry.volume();
    }

    virtual void getRandomSurfacePoint(Vector3& P, Vector3& N = Vector3::dummy) const {
        geometry.getRandomSurfacePoint(P, N);
    }

    virtual Vector3 randomInteriorPoint() const {
        return geometry.randomInteriorPoint();
    }

};


class PlaneShape : public Shape {

    G3D::Plane         geometry;

public:

    explicit inline PlaneShape(const G3D::Plane& p) : geometry(p) {}

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return PLANE;
    }

    virtual Plane& plane() { 
        return geometry;
    }

    virtual const Plane& plane() const { 
        return geometry;
    }

    virtual float area() const {
        return inf();
    }

    virtual float volume() const {
        return 0.0;
    }

    virtual void getRandomSurfacePoint(Vector3& P, Vector3& N = Vector3::dummy) const {
        P = Vector3::nan();
        N = geometry.normal();
    }

    virtual Vector3 randomInteriorPoint() const {
        return Vector3::nan();
    }

};

}

#endif
