/**
  @file IFSModel.cpp
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite Original IFS code by Nate Robbins

  @created 2003-11-12
  @edited  2003-11-12
 */ 


#include "GLG3D/RenderDevice.h"
#include "GLG3D/IFSModel.h"

namespace G3D {

IFSModel::IFSModel() {
}


IFSModel::~IFSModel() {
}


void IFSModel::getGeometry(MeshAlg::Geometry& geometry) const {
    geometry = this->geometry;
}


void IFSModel::render(RenderDevice* renderDevice, bool perVertexNormals) {
    if (perVertexNormals) {
        renderDevice->pushState();
            renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
            renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
                for (int i = 0; i < indexArray.size(); ++i) {
                    const int v = indexArray[i];
                    
                    const Vector3& P = geometry.vertexArray[v];  
                    const Vector3& N = geometry.normalArray[v];

                    renderDevice->setTexCoord(0, P * 0.25 + P.direction() * .3);
                    renderDevice->setNormal(N);
                    renderDevice->sendVertex(P);
                }
            renderDevice->endPrimitive();
        renderDevice->popState();
    } else {
        renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
            for (int f = 0; f < faceArray.size(); ++f) {
                renderDevice->setNormal(faceNormalArray[f]);
                for (int j = 0; j < 3; ++j) {
                    
                    const Vector3& P = geometry.vertexArray[faceArray[f].vertexIndex[j]];  
                    renderDevice->setTexCoord(0, P * 0.25 + P.direction() * .3);
                    renderDevice->sendVertex(P);
                }
            }
        renderDevice->endPrimitive();
    }
}


void IFSModel::reset() {
    geometry.clear();
    indexArray.clear();
    faceArray.clear();
    adjacentFaceArray.clear();
    edgeArray.clear();
}


void IFSModel::load(const std::string& filename) {
    reset();

    this->filename = filename;
    BinaryInput bi(filename, G3D_LITTLE_ENDIAN);

    if (bi.getLength() == 0) {
        throw std::string("Failed to open " + filename);
    }

    std::string header = bi.readString32();
    if (header != "IFS") {
       throw std::string("File is not an IFS file");
    }

    if (bi.readFloat32() != 1.0f) {
       throw std::string("Bad IFS version, expecting 1.0");
    }

    _name = bi.readString32();

    while (bi.hasMore()) {
        std::string str = bi.readString32();

        if (str == "VERTICES") {
            debugAssertM(geometry.vertexArray.size() == 0, "Multiple vertex fields!");
            uint32 num = bi.readUInt32();

            if ((num <= 0) || (num > 1000000)) {
                throw std::string("Bad number of vertices");
            }

            geometry.vertexArray.resize(num);


            for (int i = 0; i < num; ++i) {
                geometry.vertexArray[i] = bi.readVector3() * 2;
            }

        } else if (str == "TRIANGLES") {
            debugAssertM(faceArray.size() == 0, "Multiple triangle fields!");
            uint32 num = bi.readUInt32();

            if ((num <= 0) || (num > 1000000)) {
                throw std::string("Bad number of triangles");
            }

            indexArray.resize(num * 3);
            for (int i = 0; i < indexArray.size(); ++i) {
                indexArray[i] = bi.readUInt32();
            }
        }
    }


    debugAssert(geometry.vertexArray.size() > 0);
    debugAssert(indexArray.size() > 0);

    MeshAlg::computeAdjacency(geometry.vertexArray, indexArray, faceArray, edgeArray, adjacentFaceArray);
    MeshAlg::computeNormals(geometry.vertexArray, faceArray, adjacentFaceArray, geometry.normalArray, faceNormalArray);

    MeshAlg::computeBounds(geometry.vertexArray, _boundingBox, _boundingSphere);

    _numBrokenEdges = MeshAlg::countBrokenEdges(edgeArray);
}


size_t IFSModel::mainMemorySize() const {

    size_t frameSize   = sizeof(MeshAlg::Geometry)  + (sizeof(Vector3) + sizeof(Vector3)) * geometry.vertexArray.size();
    size_t indexSize   = indexArray.size() * sizeof(int);
    size_t faceSize    = faceArray.size() * sizeof(MeshAlg::Face);
    size_t valentSize  = adjacentFaceArray.size() * sizeof(Array<int>);
    for (int i = 0; i < adjacentFaceArray.size(); ++i) {
        valentSize += adjacentFaceArray[i].size() * sizeof(int);
    }

    size_t edgeSize    = edgeArray.size() * sizeof(MeshAlg::Edge);

    return sizeof(IFSModel) + frameSize + indexSize + faceSize + valentSize + edgeSize;
}

}

