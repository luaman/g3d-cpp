/**
  @file IFSModel.cpp
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite Original IFS code by Nate Robbins

  @created 2003-11-12
  @edited  2004-02-18
 */ 


#include "GLG3D/RenderDevice.h"
#include "GLG3D/IFSModel.h"
#include "GLG3D/VARArea.h"
#include "GLG3D/VAR.h"

namespace G3D {

VARAreaRef IFSModel::varArea = NULL;


IFSModel::IFSModel() {
}


IFSModel::~IFSModel() {
}


void IFSModel::reset() {
    geometry.clear();
    indexArray.clear();
    faceArray.clear();
    vertexArray.clear();
    edgeArray.clear();
}


IFSModelRef IFSModel::create(const std::string& filename, double scale, const CoordinateFrame& cframe) {
    return create(filename, Vector3(scale, scale, scale), cframe);
}


IFSModelRef IFSModel::create(const std::string& filename, const Vector3& scale, const CoordinateFrame& cframe) {
    IFSModel* ret = new IFSModel();
    ret->load(filename, scale, cframe);
    return ret;
}


void IFSModel::load(const std::string& filename, const Vector3& scale, const CoordinateFrame& cframe) {
    reset();

    this->filename = filename;
    load(filename, name, indexArray, geometry.vertexArray);

    debugAssert(geometry.vertexArray.size() > 0);
    debugAssert(indexArray.size() > 0);

    for (int i = 0; i < geometry.vertexArray.size(); ++i) {
        geometry.vertexArray[i] = cframe.pointToWorldSpace(geometry.vertexArray[i] * scale);
    }

    MeshAlg::computeAdjacency(geometry.vertexArray, indexArray, faceArray, edgeArray, vertexArray);
    weldedFaceArray = faceArray;
    weldedEdgeArray = edgeArray;
    weldedVertexArray = vertexArray;
    MeshAlg::weldAdjacency(geometry.vertexArray, faceArray, edgeArray, vertexArray);
    MeshAlg::computeNormals(geometry.vertexArray, faceArray, vertexArray, geometry.normalArray, faceNormalArray);
    MeshAlg::computeBounds(geometry.vertexArray, boundingBox, boundingSphere);

    numBoundaryEdges = MeshAlg::countBoundaryEdges(edgeArray);
    numWeldedBoundaryEdges = MeshAlg::countBoundaryEdges(weldedEdgeArray);
}


size_t IFSModel::mainMemorySize() const {

    size_t frameSize   = sizeof(MeshAlg::Geometry)  + (sizeof(Vector3) + sizeof(Vector3)) * geometry.vertexArray.size();
    size_t indexSize   = indexArray.size() * sizeof(int);
    size_t faceSize    = faceArray.size() * sizeof(MeshAlg::Face);
    size_t valentSize  = vertexArray.size() * sizeof(Array<MeshAlg::Vertex>);
    for (int i = 0; i < vertexArray.size(); ++i) {
        valentSize += vertexArray[i].faceIndex.size() * sizeof(int);
        valentSize += vertexArray[i].edgeIndex.size() * sizeof(int);
    }

    size_t edgeSize    = edgeArray.size() * sizeof(MeshAlg::Edge);

    return sizeof(IFSModel) + frameSize + indexSize + faceSize + valentSize + edgeSize;
}


PosedModelRef IFSModel::pose(const CoordinateFrame& cframe, bool perVertexNormals) {
    return new PosedIFSModel(this, cframe, perVertexNormals, GMaterial(), false);
}

PosedModelRef IFSModel::pose(const CoordinateFrame& cframe, const GMaterial& mat, bool perVertexNormals) {
    return new PosedIFSModel(this, cframe, perVertexNormals, mat, true);
}

//////////////////////////////////////////////////////////////////////////
void IFSModel::save(
    const std::string&          filename,
    const std::string&          name,
    const Array<int>&           index,
    const Array<Vector3>&       vertex) {

    BinaryOutput b(filename, G3D_LITTLE_ENDIAN);

    b.writeString32("IFS");
    b.writeFloat32(1.0);
    b.writeString32(name);

    b.writeString32("VERTICES");

    b.writeUInt32(vertex.size());

    for (int v = 0; v < vertex.size(); ++v) {
        vertex[v].serialize(b);
    }

    b.writeString32("TRIANGLES");

    b.writeUInt32(index.size() / 3);
    for (int i = 0; i < index.size(); ++i) {
        b.writeUInt32(index[i]);
    }
    b.commit();
}


void IFSModel::load(
    const std::string&      filename,
    std::string&            name,
    Array<int>&             index, 
    Array<Vector3>&         vertex) {

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
            debugAssertM(vertex.size() == 0, "Multiple vertex fields!");
            uint32 num = bi.readUInt32();

            if ((num <= 0) || (num > 10000000)) {
                throw std::string("Bad number of vertices");
            }

            vertex.resize(num);

            for (int i = 0; i < (int)num; ++i) {
                vertex[i].deserialize(bi);
            }

        } else if (str == "TRIANGLES") {
            debugAssertM(index.size() == 0, "Multiple triangle fields!");
            uint32 num = bi.readUInt32();

            if ((num <= 0) || (num > 100000000)) {
                throw std::string("Bad number of triangles");
            }

            index.resize(num * 3);
            for (int i = 0; i < index.size(); ++i) {
                index[i] = bi.readUInt32();
            }
        }
    }

}


void GMaterial::configure(class RenderDevice* rd) const {
    rd->setColor(color);
    for (int t = 0; t < texture.size(); ++t) {
        rd->setTexture(t, texture[t]);
    }
    rd->setShininess(shininess);
    rd->setSpecularCoefficient(specularCoefficient);
}

//////////////////////////////////////////////////////////////////////////
IFSModel::PosedIFSModel::PosedIFSModel(
    IFSModelRef                 _model,
    const CoordinateFrame&      _cframe,
    bool                        _pvn,
    const GMaterial&            _mat,
    bool                        _useMat) :
     model(_model), 
     cframe(_cframe),
     perVertexNormals(_pvn),
     useMaterial(_useMat),
     material(_mat) {
}


void IFSModel::PosedIFSModel::render(RenderDevice* renderDevice) const {
    renderDevice->pushState();

        if (useMaterial) {
            material.configure(renderDevice);
        }

        renderDevice->setObjectToWorldMatrix(coordinateFrame());

        const size_t varSize = 1024 * 1024;
        if (IFSModel::varArea.isNull()) {
            // Initialize VAR
            IFSModel::varArea = VARArea::create(varSize);
        }

        if (perVertexNormals) {
            renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
            if (! IFSModel::varArea.isNull() && 
                (varArea->totalSize() <= 
                  sizeof(Vector3) * 2 * model->geometry.vertexArray.size())) {

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

            const Vector3* vertexArray = model->geometry.vertexArray.getCArray();
            const Vector3* faceNormalArray = model->faceNormalArray.getCArray();           
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


const MeshAlg::Geometry& IFSModel::PosedIFSModel::objectSpaceGeometry() const {
    return model->geometry;
}


const Array<int>& IFSModel::PosedIFSModel::triangleIndices() const {
    return model->indexArray;
}


const Array<MeshAlg::Face>& IFSModel::PosedIFSModel::faces() const {
    return model->faceArray;
}


const Array<MeshAlg::Edge>& IFSModel::PosedIFSModel::edges() const {
    return model->edgeArray;
}


const Array<MeshAlg::Vertex>& IFSModel::PosedIFSModel::vertices() const {
    return model->vertexArray;
}


const Array<MeshAlg::Face>& IFSModel::PosedIFSModel::weldedFaces() const {
    return model->weldedFaceArray;
}


const Array<MeshAlg::Edge>& IFSModel::PosedIFSModel::weldedEdges() const {
    return model->weldedEdgeArray;
}


const Array<MeshAlg::Vertex>& IFSModel::PosedIFSModel::weldedVertices() const {
    return model->weldedVertexArray;
}


void IFSModel::PosedIFSModel::getObjectSpaceBoundingSphere(Sphere& s) const {
    s = model->boundingSphere;
}


void IFSModel::PosedIFSModel::getObjectSpaceBoundingBox(Box& b) const {
    b = model->boundingBox;
}


int IFSModel::PosedIFSModel::numBoundaryEdges() const {
    return model->numBoundaryEdges;
}


int IFSModel::PosedIFSModel::numWeldedBoundaryEdges() const {
    return model->numWeldedBoundaryEdges;
}


const Array<Vector3>& IFSModel::PosedIFSModel::objectSpaceFaceNormals(bool normalize) const {
    return model->faceNormalArray;
}

}
