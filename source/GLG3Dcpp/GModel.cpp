/**
 @file GModel.h

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2003-11-12
 @edited  2003-11-12
 */

#include "GLG3D/GModel.h"

namespace G3D {

bool GModel::initialized() const {
    return _initialized;
}


std::string GModel::name() const {
    return _name;
}


void GModel::setName(const std::string& n) {
    _name = n;
}


const Array<MeshAlg::Face>&  GModel::faces() const {
    return faceArray;
}


const Array<MeshAlg::Edge>&  GModel::geometricEdges() const {
    return edgeArray;
}


const Array< Array<int> >&   GModel::adjacentFaces() const {
    return adjacentFaceArray;
}


const Sphere& GModel::boundingSphere() const {
    if (! _initialized) {
        static Sphere s(Vector3::ZERO, 0);
        return s;
    }
    return _boundingSphere;
}


const Box& GModel::boundingBox() const {
    if (! _initialized) {
        static Box b(Vector3::ZERO, Vector3::ZERO);
        return b;
    }
    return _boundingBox;
}


int GModel::numBrokenEdges() const {
    return _numBrokenEdges;
}

}
