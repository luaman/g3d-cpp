/**
  @file PosedModel.cpp
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-11-15
  @edited  2003-11-15
 */ 

#include "GLG3D/PosedModel.h"

namespace G3D {

void PosedModel::getWorldSpaceGeometry(MeshAlg::Geometry& geometry) const {
    CoordinateFrame c;
    getCoordinateFrame(c);
    getObjectSpaceGeometry(geometry);
    c.pointToWorldSpace(geometry.vertexArray, geometry.vertexArray);
    c.normalToWorldSpace(geometry.normalArray, geometry.normalArray);
}


CoordinateFrame PosedModel::coordinateFrame() const {
    CoordinateFrame c;
    getCoordinateFrame(c);
    return c;
}


Sphere PosedModel::objectSpaceBoundingSphere() const {
    Sphere s;
    getObjectSpaceBoundingSphere(s);
    return s;
}


void PosedModel::getWorldSpaceBoundingSphere(Sphere& s) const {
    CoordinateFrame C;
    getCoordinateFrame(C);
    getObjectSpaceBoundingSphere(s);
    s = C.toWorldSpace(s);
}


Sphere PosedModel::worldSpaceBoundingSphere() const {
    Sphere s;
    getWorldSpaceBoundingSphere(s);
    return s;
}


Box PosedModel::objectSpaceBoundingBox() const {
    Box b;
    getObjectSpaceBoundingBox(b);
    return b;
}


void PosedModel::getWorldSpaceBoundingBox(Box& box) const {
    CoordinateFrame C;
    getCoordinateFrame(C);
    getObjectSpaceBoundingBox(box);
    box = C.toWorldSpace(box);
}


Box PosedModel::worldSpaceBoundingBox() const {
    Box b;
    getWorldSpaceBoundingBox(b);
    return b;
}

}
