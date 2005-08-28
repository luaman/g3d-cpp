#include "Shape.h"

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

