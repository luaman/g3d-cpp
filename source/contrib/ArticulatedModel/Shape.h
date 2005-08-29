#ifndef AX2_SHAPE_H
#define AX2_SHAPE_H

#include <G3DAll.h>

namespace G3D {

/** 
   Base class for other shapes.  G3D primitives like Box and Cylinder
   have no base class so that their implementations are maximally efficient,
   and have no "render" method because they are lower-level than the
   rendering API.  
   
   The Shape classes provide a parallel set of classes to the G3D primitives
   that support more functionality.
  */
class Shape {
public:

    /** Mesh is reserved */
    enum Type {MESH=1, BOX, CYLINDER, SPHERE, RAY, CAPSULE, PLANE};

    static std::string typeToString(Type t);

    virtual Type type() = 0;
   
    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black()) = 0;

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

    virtual ~Shape() {}
};


class BoxShape : public Shape {
    
    G3D::Box          geometry;

public:

    inline BoxShape(const G3D::Box& b) : geometry(b) {}

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
};


class RayShape : public Shape {
    
    G3D::Ray          geometry;

public:

    inline RayShape(const G3D::Ray& r) : geometry(r) {}

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
};


class CylinderShape : public Shape {

    G3D::Cylinder       geometry;

public:

    inline CylinderShape(const G3D::Cylinder& c) : geometry(c) {}

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
};


class SphereShape : public Shape {

    G3D::Sphere         geometry;

public:

    inline SphereShape(const G3D::Sphere& s) : geometry(s) {}

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
};


class CapsuleShape : public Shape {
public:

    G3D::Capsule         geometry;

    inline CapsuleShape(const G3D::Capsule& c) : geometry(c) {}

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
};


class PlaneShape : public Shape {

    G3D::Plane         geometry;

public:

    inline PlaneShape(const G3D::Plane& p) : geometry(p) {}

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
};

}

#endif
