/**
  @file IFSModel.cpp
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite Original IFS code by Nate Robbins

  @created 2003-11-12
  @edited  2003-11-19
 */ 


#include "GLG3D/RenderDevice.h"
#include "GLG3D/IFSModel.h"
#include "GLG3D/VARArea.h"
#include "GLG3D/VAR.h"

namespace G3D {

VARArea* IFSModel::varArea = NULL;


IFSModel::IFSModel() {
}


IFSModel::~IFSModel() {
}


void IFSModel::reset() {
    geometry.clear();
    indexArray.clear();
    faceArray.clear();
    adjacentFaceArray.clear();
    edgeArray.clear();
}


IFSModelRef IFSModel::create(const std::string& filename, double scale) {
    IFSModel* ret = new IFSModel();
    ret->load(filename, scale);
    return ret;
}


void IFSModel::load(const std::string& filename, double scale) {
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

    name = bi.readString32();

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
                geometry.vertexArray[i] = bi.readVector3() * scale;
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

    MeshAlg::computeBounds(geometry.vertexArray, boundingBox, boundingSphere);

    numBrokenEdges = MeshAlg::countBrokenEdges(edgeArray);
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


PosedModelRef IFSModel::pose(const CoordinateFrame& cframe, bool perVertexNormals) {
    return new PosedIFSModel(this, cframe, perVertexNormals);
}

//////////////////////////////////////////////////////////////////////////
IFSModel::PosedIFSModel::PosedIFSModel(
    IFSModelRef                 _model,
    const CoordinateFrame&      _cframe,
    bool                        _pvn) :
     model(_model), 
     cframe(_cframe),
     perVertexNormals(_pvn) {
}


void IFSModel::PosedIFSModel::render(RenderDevice* renderDevice) const {
    renderDevice->pushState();
        renderDevice->setObjectToWorldMatrix(coordinateFrame());

        const size_t varSize = 1024 * 1024;
        if ((IFSModel::varArea == NULL) && (renderDevice->freeVARSize() <= varSize)) {
            // Initialize VAR
            IFSModel::varArea = renderDevice->createVARArea(varSize);
        }

        if (perVertexNormals) {
            renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
            if (IFSModel::varArea && (varArea->totalSize() <= sizeof(Vector3) * 2 * model->geometry.vertexArray.size())) {

                // Can use VAR

                varArea->reset();

                VAR vertex(model->geometry.vertexArray, IFSModel::varArea);
                VAR normal(model->geometry.normalArray, IFSModel::varArea);

                renderDevice->beginIndexedPrimitives();
                    renderDevice->setNormalArray(normal);
                    renderDevice->setVertexArray(vertex);
                    renderDevice->sendIndices(RenderDevice::TRIANGLES, model->indexArray);
                renderDevice->endIndexedPrimitives();

            } else {

                // No VAR
                const int* indexArray = model->indexArray.getCArray();
                const Vector3* vertexArray = model->geometry.vertexArray.getCArray();
                const Vector3* normalArray = model->geometry.normalArray.getCArray();
                int n = model->indexArray.size();

                renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
                    for (int i = 0; i < n; ++i) {
                        const int v = indexArray[i];            
                        renderDevice->setNormal(normalArray[v]);
                        renderDevice->sendVertex(vertexArray[v]);
                    }
                renderDevice->endPrimitive();
            }

        } else {

            // Face Normals (slow)

            const int* indexArray = model->indexArray.getCArray();
            const Vector3* vertexArray = model->geometry.vertexArray.getCArray();
            const Vector3* faceNormalArray = model->faceNormalArray.getCArray();
            const Vector3* normalArray = model->geometry.normalArray.getCArray();
            const MeshAlg::Face* faceArray = model->faceArray.getCArray();
            int n = model->faceArray.size();

            renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
                for (int f = 0; f < n; ++f) {
                    renderDevice->setNormal(faceNormalArray[f]);
                    for (int j = 0; j < 3; ++j) {                    
                        renderDevice->sendVertex(vertexArray[faceArray[f].vertexIndex[j]]);
                    }
                }
            renderDevice->endPrimitive();
        }

    renderDevice->popState();
}


std::string IFSModel::PosedIFSModel::name() const {
    return model->name;
}


void IFSModel::PosedIFSModel::getCoordinateFrame(CoordinateFrame& c) const {
    c = cframe;
}


void IFSModel::PosedIFSModel::getObjectSpaceGeometry(MeshAlg::Geometry& geometry) const {
    geometry = model->geometry;
}


void IFSModel::PosedIFSModel::getWorldSpaceGeometry(MeshAlg::Geometry& geometry) const {
    CoordinateFrame C;
    getCoordinateFrame(C);

    C.pointToWorldSpace(model->geometry.vertexArray, geometry.vertexArray);
    C.normalToWorldSpace(model->geometry.normalArray, geometry.normalArray);
}


void IFSModel::PosedIFSModel::getTriangleIndices(Array<int>& index) const {
    index = model->indexArray;
}


void IFSModel::PosedIFSModel::getFaces(Array<MeshAlg::Face>& faces) const {
    faces = model->faceArray;
}


void IFSModel::PosedIFSModel::getEdges(Array<MeshAlg::Edge>& edges) const {
    edges = model->edgeArray;
}


void IFSModel::PosedIFSModel::getAdjacentFaces(Array< Array<int> >& adjacentFaces) const {
    adjacentFaces = model->adjacentFaceArray;
}


void IFSModel::PosedIFSModel::getObjectSpaceBoundingSphere(Sphere& s) const {
    s = model->boundingSphere;
}


void IFSModel::PosedIFSModel::getObjectSpaceBoundingBox(Box& b) const {
    b = model->boundingBox;
}


int IFSModel::PosedIFSModel::numBrokenEdges() const {
    return model->numBrokenEdges;
}

}
