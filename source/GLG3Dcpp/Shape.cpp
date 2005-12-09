/**
 @file GLG3D/Shape.cpp

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2005-08-30
 @edited  2005-08-30
 */

#include "GLG3D/Shape.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/Draw.h"

namespace G3D {

std::string Shape::typeToString(Type t) {
    switch (t) {
    case MESH:
        return "MESH";
    
    case BOX:
        return "BOX";
    
    case CYLINDER:
        return "CYLINDER";
    
    case SPHERE:
        return "SPHERE";
    
    case RAY:
        return "RAY";

    case CAPSULE:
        return "CAPSULE";

    case PLANE:
        return "PLANE";
    }

    return "";
}


float MeshShape::area() const {
    debugAssert(false); // TODO        
    return 0;
}


float MeshShape::volume() const {
    return 0;
}


void MeshShape::getRandomSurfacePoint(
    Vector3& P,
    Vector3& N) const {
    
    debugAssert(false); // TODO
}


Vector3 MeshShape::randomInteriorPoint() const {
    Vector3 P;
    getRandomSurfacePoint(P);
    return P;
}


void MeshShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();

    rd->pushState();
        rd->setObjectToWorldMatrix(cframe0 * cframe);
        if (solidColor.a < 1.0) {
            rd->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        }
        rd->setColor(solidColor);
        rd->beginPrimitive(RenderDevice::TRIANGLES);
            for (int i = 0; i < _indexArray.size(); i += 3) {
                const Vector3& v0 = _vertexArray[_indexArray[i + 0]];
                const Vector3& v1 = _vertexArray[_indexArray[i + 1]];
                const Vector3& v2 = _vertexArray[_indexArray[i + 2]];
                rd->sendVertex(v0);
                rd->sendVertex(v1);
                rd->sendVertex(v2);
            }
        rd->endPrimitive();

        rd->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        rd->setLineWidth(2);
        rd->setColor(wireColor);
        rd->beginPrimitive(RenderDevice::LINES);
            for (int i = 0; i < _indexArray.size(); i += 3) {
                const Vector3& v0 = _vertexArray[_indexArray[i + 0]];
                const Vector3& v1 = _vertexArray[_indexArray[i + 1]];
                const Vector3& v2 = _vertexArray[_indexArray[i + 2]];
                rd->sendVertex(v0);
                rd->sendVertex(v1);
                rd->sendVertex(v1);
                rd->sendVertex(v2);
                rd->sendVertex(v2);
                rd->sendVertex(v0);
            }
        rd->endPrimitive();
    rd->popState();
}


void BoxShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::box(geometry, rd, solidColor, wireColor);
    rd->setObjectToWorldMatrix(cframe0);
}


void SphereShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::sphere(geometry, rd, solidColor, wireColor);
    rd->setObjectToWorldMatrix(cframe0);
}


void CylinderShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::cylinder(geometry, rd, solidColor, wireColor);
    rd->setObjectToWorldMatrix(cframe0);
}


void CapsuleShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::capsule(geometry, rd, solidColor, wireColor);
    rd->setObjectToWorldMatrix(cframe0);
}


void RayShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::ray(geometry, rd, solidColor);
    rd->setObjectToWorldMatrix(cframe0);
}


void PlaneShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();

    rd->setObjectToWorldMatrix(cframe0 * cframe);

    Draw::plane(geometry, rd, solidColor, wireColor);
    
    rd->setObjectToWorldMatrix(cframe0);
}

}

