#include "ArticulatedModel.h"

class PosedArticulatedModel : public PosedModel {
private:
    friend class ArticulatedModel;
    friend class ArticulatedModel::Part;

    ArticulatedModelRef     model;

    int                     partIndex;

    int                     listIndex;

    /** Object to world space */
    CoordinateFrame         cframe;

    std::string             _name;

    bool                    useMaterial;

    /** 
      If NULL, use whatever OpenGL state is currently enabled.

    */
    SuperShader::LightingEnvironmentRef lightingEnvironment;

    /** Called from render to draw geometry after the material properties are set.*/
    void renderGeometry(RenderDevice* rd) const;

public:

    virtual std::string name() const;

    virtual bool hasTransparency() const;

    virtual void getCoordinateFrame(CoordinateFrame& c) const;

    virtual const MeshAlg::Geometry& objectSpaceGeometry() const;

    virtual const Array<Vector3>& objectSpaceFaceNormals(bool normalize = true) const;

    virtual const Array<MeshAlg::Face>& faces() const;

    virtual const Array<MeshAlg::Edge>& edges() const;

    virtual const Array<MeshAlg::Vertex>& vertices() const;

    virtual const Array<Vector2>& texCoords() const;

    virtual bool hasTexCoords() const;

    virtual const Array<MeshAlg::Face>& weldedFaces() const;

    virtual const Array<MeshAlg::Edge>& weldedEdges() const;

    virtual const Array<MeshAlg::Vertex>& weldedVertices() const;

    virtual const Array<int>& triangleIndices() const;

    virtual void getObjectSpaceBoundingSphere(Sphere&) const;

    virtual void getObjectSpaceBoundingBox(Box&) const;

    virtual void render(class RenderDevice* renderDevice) const;

    virtual int numBoundaryEdges() const;

    virtual int numWeldedBoundaryEdges() const;
};


void ArticulatedModel::pose(
    Array<PosedModelRef>&       posedArray, 
    const CoordinateFrame&      cframe, 
    const Pose&                 posex,
    SuperShader::LightingEnvironmentRef lightingEnvironment) {

    for (int p = 0; p < partArray.size(); ++p) {
        const Part& part = partArray[p];
        part.pose(this, p, posedArray, cframe, posex, lightingEnvironment);
    }
}


void ArticulatedModel::Part::pose(
    ArticulatedModelRef         model,
    int                         partIndex,
    Array<PosedModelRef>&       posedArray,
    const CoordinateFrame&      parent, 
    const Pose&                 posex,
    SuperShader::LightingEnvironmentRef lightingEnvironment) const {

    CoordinateFrame frame;

    if (posex.cframe.containsKey(name)) {
        frame = parent * keyframe * posex.cframe[name] * keyframe.inverse();
    } else {
        frame = parent;
    }

    if (hasGeometry()) {

        for (int t = 0; t < triListArray.size(); ++t) {
            PosedArticulatedModel* posed = new PosedArticulatedModel();

            posed->cframe = frame;
            posed->_name = model->name + ": " + name;
            posed->partIndex = partIndex;
            posed->listIndex = t;
            posed->model = model;
            posed->useMaterial = posex.useMaterial;
            posed->lightingEnvironment = lightingEnvironment;

            posedArray.append(posed);
        }
    }

    // TODO: sub-parts
}


void PosedArticulatedModel::render(
    RenderDevice*           rd) const {

    rd->setObjectToWorldMatrix(cframe);

    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];

    rd->pushState();

    if (useMaterial) {
        bool makeTransparentPass = ! triList.material.transmit.isBlack();

        if (makeTransparentPass) {
            if (triList.cullFace == RenderDevice::CULL_NONE) {
                // Two sided, transparent.  We need four passes: T back, D back, T front, D front

                rd->enableTwoSidedLighting();
                rd->setCullFace(RenderDevice::CULL_FRONT);
                rd->disableDepthWrite();
                for (int i = 0; i < 2; ++i) { 
                    // dst1 * transmission
                    rd->pushState();
                        rd->setBlendFunc(RenderDevice::BLEND_ZERO, RenderDevice::BLEND_SRC_COLOR);
                        rd->disableLighting();
                        rd->setColor(triList.material.transmit.constant);
                        rd->setTexture(0, triList.material.transmit.map);
                        renderGeometry(rd);
                    rd->popState();

                    //   SUM OVER LIGHTS {
                    //     light * (diffuse * NdotL +
                    //              specular * NdotH^specularExponent)}
                    rd->pushState();
                        rd->disableDepthWrite();
                        rd->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);
                        rd->setColor(triList.material.diffuse.constant);
                        rd->setTexture(0, triList.material.diffuse.map);
                        rd->setSpecularCoefficient(triList.material.specular.constant.average());
                        rd->setShininess(triList.material.specularExponent.constant.average());
                        renderGeometry(rd);
                    rd->popState();
                    rd->setCullFace(RenderDevice::CULL_BACK);
                } // for i
            } else {
            	rd->setCullFace(triList.cullFace);
                // Transparent, one sided.  We need two passes: T, D
                // dst1 * transmission
                rd->pushState();
                    rd->setBlendFunc(RenderDevice::BLEND_ZERO, RenderDevice::BLEND_SRC_COLOR);
                    rd->disableLighting();
                    rd->setColor(triList.material.transmit.constant);
                    rd->setTexture(0, triList.material.transmit.map);
                    renderGeometry(rd);
                rd->popState();

                //   SUM OVER LIGHTS {
                //     light * (diffuse * NdotL +
                //              specular * NdotH^specularExponent)}
                rd->pushState();
                    rd->disableDepthWrite();
                    rd->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);
                    rd->setColor(triList.material.diffuse.constant);
                    rd->setTexture(0, triList.material.diffuse.map);
                    rd->setSpecularCoefficient(triList.material.specular.constant.average());
                    rd->setShininess(triList.material.specularExponent.constant.average());
                    renderGeometry(rd);
                rd->popState();
            }
        } else {
            // No transparent pass.
            rd->setColor(triList.material.diffuse.constant);
            rd->setTexture(0, triList.material.diffuse.map);
            rd->setSpecularCoefficient(triList.material.specular.constant.average());
            rd->setShininess(triList.material.specularExponent.constant.average());
        	rd->setCullFace(triList.cullFace);
            renderGeometry(rd);
        }
    } else {
        // No material; just draw the surface
    	rd->setCullFace(triList.cullFace);
        renderGeometry(rd);
    }

    rd->popState();
}


void PosedArticulatedModel::renderGeometry(
    RenderDevice*           rd) const {

    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];

    if (rd->renderMode() == RenderDevice::RENDER_SOLID) {

        rd->beginIndexedPrimitives();
            rd->setVertexArray(part.vertexVAR);
            rd->setNormalArray(part.normalVAR);
            if (part.texCoordArray.size() > 0) {
                rd->setTexCoordArray(0, part.texCoord0VAR);
            }
            rd->sendIndices(RenderDevice::TRIANGLES, triList.indexArray);
        rd->endIndexedPrimitives();

    } else {

        // Radeon mobility cards crash rendering wireframe in wireframe mode.
        // switch to begin/end
        rd->beginPrimitive(RenderDevice::TRIANGLES);
        for (int i = 0; i < triList.indexArray.size(); ++i) {
            int v = triList.indexArray[i];
            if (part.texCoordArray.size() > 0) {
                rd->setTexCoord(0, part.texCoordArray[v]);
            }
            rd->setNormal(part.geometry.normalArray[v]);
            rd->sendVertex(part.geometry.vertexArray[v]);
        }
        rd->endPrimitive();
    }
}


std::string PosedArticulatedModel::name() const {
    return _name;
}


bool PosedArticulatedModel::hasTransparency() const {
    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];
    return !(useMaterial && triList.material.transmit.isBlack());
}


void PosedArticulatedModel::getCoordinateFrame(CoordinateFrame& c) const {
    c = cframe;
}


const MeshAlg::Geometry& PosedArticulatedModel::objectSpaceGeometry() const {
    const ArticulatedModel::Part& part = model->partArray[partIndex];
    return part.geometry;
}


const Array<Vector3>& PosedArticulatedModel::objectSpaceFaceNormals(bool normalize) const {
    static Array<Vector3> n;
    debugAssert(false);
    return n;
    // TODO
}


const Array<MeshAlg::Face>& PosedArticulatedModel::faces() const {
    static Array<MeshAlg::Face> f;
    debugAssert(false);
    return f;
    // TODO
}


const Array<MeshAlg::Edge>& PosedArticulatedModel::edges() const {
    static Array<MeshAlg::Edge> f;
    debugAssert(false);
    return f;
    // TODO
}


const Array<MeshAlg::Vertex>& PosedArticulatedModel::vertices() const {
    static Array<MeshAlg::Vertex> f;
    debugAssert(false);
    return f;
    // TODO
}

const Array<Vector2>& PosedArticulatedModel::texCoords() const {
    const ArticulatedModel::Part& part = model->partArray[partIndex];
    return part.texCoordArray;
}


bool PosedArticulatedModel::hasTexCoords() const {
    const ArticulatedModel::Part& part = model->partArray[partIndex];
    return part.texCoordArray.size() > 0;
}


const Array<MeshAlg::Face>& PosedArticulatedModel::weldedFaces() const {
    static Array<MeshAlg::Face> f;
    debugAssert(false);
    return f;
    // TODO
}


const Array<MeshAlg::Edge>& PosedArticulatedModel::weldedEdges() const {
    static Array<MeshAlg::Edge> e;
    debugAssert(false);
    return e;
    // TODO
}


const Array<MeshAlg::Vertex>& PosedArticulatedModel::weldedVertices() const {
    static Array<MeshAlg::Vertex> v;
    return v;
    debugAssert(false);
    // TODO
}


const Array<int>& PosedArticulatedModel::triangleIndices() const {
    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];
    return triList.indexArray;
}


void PosedArticulatedModel::getObjectSpaceBoundingSphere(Sphere& s) const {
    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];
    s = triList.sphereBounds;
}


void PosedArticulatedModel::getObjectSpaceBoundingBox(Box& b) const {
    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];
    b = triList.boxBounds;
}


int PosedArticulatedModel::numBoundaryEdges() const {
    // TODO
    debugAssert(false);
    return 0;
}


int PosedArticulatedModel::numWeldedBoundaryEdges() const {
    // TODO
    return 0;
}
