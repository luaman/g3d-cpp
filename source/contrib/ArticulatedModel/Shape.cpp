#include "Shape.h"

namespace G3D {

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


void RayShape::render(RenderDevice* rd, const CoordinateFrame& cframe, Color4 solidColor, Color4 wireColor) {
    CoordinateFrame cframe0 = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe0 * cframe);
    Draw::ray(geometry, rd, solidColor);
    rd->setObjectToWorldMatrix(cframe0);
}

}

