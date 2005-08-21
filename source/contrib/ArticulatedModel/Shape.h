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

    virtual Type type() = 0;
   
    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black()) = 0;

    virtual class BoxShape* asBox() { 
        debugAssertM(false, "Not a box");
        return NULL; 
    }

    virtual class RayShape* asRay() { 
        debugAssertM(false, "Not a ray");
        return NULL; 
    }
    
    virtual class CylinderShape* asCylinder() { 
        debugAssertM(false, "Not a cylinder");
        return NULL; 
    }

    virtual class SphereShape* asSphere() { 
        debugAssertM(false, "Not a sphere");
        return NULL; 
    }

    virtual class CapsuleShape* asCapsule() { 
        debugAssertM(false, "Not a capsule");
        return NULL; 
    }

    virtual class PlaneShape* asPlane() { 
        debugAssertM(false, "Not a plane");
        return NULL; 
    }

    virtual ~Shape() {}
};


class BoxShape : public Shape {
public:
    
    G3D::AABox          geometry;

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return BOX;
    }

    virtual class BoxShape* asBox() { 
        return this;
    }
};


class RayShape : public Shape {
public:
    
    G3D::Ray          geometry;

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return RAY;
    }

    virtual class RayShape* asRay() { 
        return this;
    }
};


class CylinderShape : public Shape {
public:

    G3D::Cylinder       geometry;

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return CYLINDER;
    }

    virtual class CylinderShape* asCylinder() { 
        return this;
    }
};


class SphereShape : public Shape {
public:

    G3D::Sphere         geometry;

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return SPHERE;
    }

    virtual class SphereShape* asSphere() { 
        return this;
    }
};


class CapsuleShape : public Shape {
public:

    G3D::Capsule         geometry;

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return CAPSULE;
    }

    virtual class CapsuleShape* asCapsule() { 
        return this;
    }
};


class PlaneShape : public Shape {
public:

    G3D::Plane         geometry;

    virtual void render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor = Color4(.5,.5,0,.5), Color4 wireColor = Color3::black());

    virtual Type type() {
        return PLANE;
    }

    virtual class PlaneShape* asPlane() { 
        return this;
    }
};

}

#endif
