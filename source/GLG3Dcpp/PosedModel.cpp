/**
  @file PosedModel.cpp
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-11-15
  @edited  2003-12-09
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


void PosedModel::getObjectSpaceFaceNormals(Array<Vector3>& faceNormals, bool normalize) const {
    MeshAlg::Geometry geometry;
    getObjectSpaceGeometry(geometry);

    Array<MeshAlg::Face> faceArray;
    getFaces(faceArray);

    MeshAlg::computeFaceNormals(geometry.vertexArray, faceArray, faceNormals, normalize);
}


void PosedModel::getWorldSpaceFaceNormals(Array<Vector3>& faceNormals, bool normalize) const {
    MeshAlg::Geometry geometry;
    getWorldSpaceGeometry(geometry);

    Array<MeshAlg::Face> faceArray;
    getFaces(faceArray);

    MeshAlg::computeFaceNormals(geometry.vertexArray, faceArray, faceNormals, normalize);
}

////////////////////////////////////////////////////////////////////////////////////////



std::string PosedModelWrapper::name() const {
    return model->name();
}


void PosedModelWrapper::getCoordinateFrame(CoordinateFrame& c) const {
    model->getCoordinateFrame(c);
}


CoordinateFrame PosedModelWrapper::coordinateFrame() const {
    return model->coordinateFrame();
}


void PosedModelWrapper::getObjectSpaceGeometry(MeshAlg::Geometry& geometry) const {
    model->getObjectSpaceGeometry(geometry);
}


void PosedModelWrapper::getWorldSpaceGeometry(MeshAlg::Geometry& geometry) const {
    model->getWorldSpaceGeometry(geometry);
}


void PosedModelWrapper::getFaces(Array<MeshAlg::Face>& faces) const {
    model->getFaces(faces);
}


void PosedModelWrapper::getObjectSpaceFaceNormals(Array<Vector3>& faceNormals, bool normalize) const {
    model->getObjectSpaceFaceNormals(faceNormals, normalize);
}


void PosedModelWrapper::getWorldSpaceFaceNormals(Array<Vector3>& faceNormals, bool normalize) const {
    model->getWorldSpaceFaceNormals(faceNormals, normalize);
}

void PosedModelWrapper::getEdges(Array<MeshAlg::Edge>& edges) const {
    model->getEdges(edges);
}


void PosedModelWrapper::getAdjacentFaces(Array< Array<int> >& adjacentFaces) const {
    model->getAdjacentFaces(adjacentFaces);
}


void PosedModelWrapper::getTriangleIndices(Array<int>& indices) const {
    model->getTriangleIndices(indices);
}


void PosedModelWrapper::getObjectSpaceBoundingSphere(Sphere& s) const {
    model->getObjectSpaceBoundingSphere(s);
}


Sphere PosedModelWrapper::objectSpaceBoundingSphere() const {
    return model->objectSpaceBoundingSphere();
}


void PosedModelWrapper::getWorldSpaceBoundingSphere(Sphere& s) const {
    model->getWorldSpaceBoundingSphere(s);
}


Sphere PosedModelWrapper::worldSpaceBoundingSphere() const {
    return model->worldSpaceBoundingSphere();
}


void PosedModelWrapper::getObjectSpaceBoundingBox(Box& box) const {
    model->getObjectSpaceBoundingBox(box);
}


Box PosedModelWrapper::objectSpaceBoundingBox() const {
    return model->objectSpaceBoundingBox();
}


void PosedModelWrapper::getWorldSpaceBoundingBox(Box& box) const {
    model->getWorldSpaceBoundingBox(box);
}


Box PosedModelWrapper::worldSpaceBoundingBox() const {
    return model->worldSpaceBoundingBox();
}


void PosedModelWrapper::render(class RenderDevice* renderDevice) const { 
    model->render(renderDevice);
}


int PosedModelWrapper::numBrokenEdges() const {
    return model->numBrokenEdges();
}


}

