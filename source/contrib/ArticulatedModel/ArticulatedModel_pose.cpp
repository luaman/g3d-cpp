/**
 @file ArticulatedModel_pose.cpp
 Rendering and pose aspects of ArticulatedModel

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2004-11-20
  @edited  2005-11-25

  Copyright 2004-2005, Morgan McGuire
 */
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

    SuperShaderRef          superShader;

protected:

    /** Called from render to draw geometry after the material properties are set.*/
    void defaultRender(RenderDevice* rd) const;

    /** Renders emission, reflection, and lighting for non-shadowed lights.
        The first term rendered uses the current blending/depth mode
        and subsequent terms use additive blending.  Returns true if  
        anything was rendered, false if nothing was rendered (because 
        all terms were black). 
    
        Called from renderNonShadowedOpaqueTerms.
    */ 
    bool renderFFNonShadowedOpaqueTerms(
        RenderDevice*                   rd,
        const LightingRef&              lighting,
        const ArticulatedModel::Part&   part,
        const ArticulatedModel::Part::TriList& triList,
        const SuperShader::Material&    material) const;

    bool renderPS20NonShadowedOpaqueTerms(
        RenderDevice*                   rd,
        const LightingRef&              lighting,
        const ArticulatedModel::Part&   part,
        const ArticulatedModel::Part::TriList& triList,
        const SuperShader::Material&    material) const;

    /** Switches between rendering paths.  Called from renderNonShadowed.*/
    bool renderNonShadowedOpaqueTerms(
        RenderDevice*                   rd,
        const LightingRef&              lighting,
        const ArticulatedModel::Part&   part,
        const ArticulatedModel::Part::TriList& triList,
        const SuperShader::Material&    material) const;

    void renderFFShadowMappedLightPass(
        RenderDevice*                   rd,
        const GLight&                   light,
        const Matrix4&                  lightMVP, 
        const TextureRef&               shadowMap,
        const ArticulatedModel::Part&   part,
        const ArticulatedModel::Part::TriList& triList,
        const SuperShader::Material&    material) const;

    void renderPS20ShadowMappedLightPass(
        RenderDevice*                   rd,
        const GLight&                   light,
        const Matrix4&                  lightMVP, 
        const TextureRef&               shadowMap,
        const ArticulatedModel::Part&   part,
        const ArticulatedModel::Part::TriList& triList,
        const SuperShader::Material&    material) const;

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

    virtual void render(RenderDevice* renderDevice) const;
    
    virtual void renderNonShadowed(RenderDevice* rd, const LightingRef& lighting) const;
    
    virtual void renderShadowedLightPass(RenderDevice* rd, const GLight& light) const;

    virtual void renderShadowMappedLightPass(RenderDevice* rd, const GLight& light, const Matrix4& lightMVP, const TextureRef& shadowMap) const;

    virtual int numBoundaryEdges() const;

    virtual int numWeldedBoundaryEdges() const;
};


void ArticulatedModel::pose(
    Array<PosedModelRef>&       posedArray, 
    const CoordinateFrame&      cframe, 
    const Pose&                 posex) {

    for (int p = 0; p < partArray.size(); ++p) {
        const Part& part = partArray[p];
        part.pose(this, p, posedArray, cframe, posex);
    }
}


void ArticulatedModel::Part::pose(
    ArticulatedModelRef         model,
    int                         partIndex,
    Array<PosedModelRef>&       posedArray,
    const CoordinateFrame&      parent, 
    const Pose&                 posex) const {

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

            if (ArticulatedModel::profile() != ArticulatedModel::FIXED_FUNCTION) {
                // Construct the shader
                const ArticulatedModel::Part& part = model->partArray[partIndex];
                const ArticulatedModel::Part::TriList& triList = part.triListArray[t];
                const SuperShader::Material& material = triList.material;
                posed->superShader = SuperShader::create(material);
            }

            posedArray.append(posed);
        }
    }

    // TODO: sub-parts
}



void PosedArticulatedModel::render(RenderDevice* renderDevice) const {

    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];
    const SuperShader::Material& material = triList.material;

    renderDevice->pushState();
        renderDevice->setTexture(0, material.diffuse.map);
        renderDevice->setColor(material.diffuse.constant);

        renderDevice->setSpecularCoefficient(material.specular.constant);
        renderDevice->setShininess(material.specularExponent.constant.average());

        defaultRender(renderDevice);
    renderDevice->popState();
}


/** 
 Switches to additive rendering, if not already in that mode.
 */
static void setAdditive(RenderDevice* rd, bool& additive) {
    if (! additive) {
        rd->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);
        rd->setDepthWrite(false);
        additive = true;
    }
}


bool PosedArticulatedModel::renderNonShadowedOpaqueTerms(
    RenderDevice*                   rd,
    const LightingRef&              lighting,
    const ArticulatedModel::Part&   part,
    const ArticulatedModel::Part::TriList& triList,
    const SuperShader::Material&    material) const {

    bool renderedOnce = false;

    rd->pushState();
        switch (ArticulatedModel::profile()) {
        case ArticulatedModel::FIXED_FUNCTION:
            renderedOnce = renderFFNonShadowedOpaqueTerms(rd, lighting, part, triList, material);
            break;

        case ArticulatedModel::PS20:
            renderedOnce = renderPS20NonShadowedOpaqueTerms(rd, lighting, part, triList, material);
            break;

        default:
            debugAssertM(false, "Fell through switch");
        }
    rd->popState();

    return renderedOnce;
}


bool PosedArticulatedModel::renderPS20NonShadowedOpaqueTerms(
    RenderDevice*                   rd,
    const LightingRef&              lighting,
    const ArticulatedModel::Part&   part,
    const ArticulatedModel::Part::TriList& triList,
    const SuperShader::Material&    material) const {

    if (material.emit.isBlack() && 
        material.reflect.isBlack() &&
        material.specular.isBlack() &&
        material.diffuse.isBlack()) {
        // Nothing to draw
        return false;
    }

    superShader->setLighting(lighting);
    rd->setShader(superShader);
    defaultRender(rd);

    return true;
}


bool PosedArticulatedModel::renderFFNonShadowedOpaqueTerms(
    RenderDevice*                   rd,
    const LightingRef&              lighting,
    const ArticulatedModel::Part&   part,
    const ArticulatedModel::Part::TriList& triList,
    const SuperShader::Material&    material) const {

    bool renderedOnce = false;

    // Emissive
    if (! material.emit.isBlack()) {
        rd->setColor(material.emit.constant);
        rd->setTexture(0, material.emit.map);
        defaultRender(rd);
        setAdditive(rd, renderedOnce);
    }
    
    // Add reflective
    if (! material.reflect.isBlack() && 
        GLCaps::supports_GL_EXT_texture_cube_map() &&
        lighting.notNull() &&
        (lighting->environmentMapColor != Color3::black())) {

        // Reflections are specular and not affected by surface texture, only
        // the reflection coefficient
        rd->setColor(material.reflect.constant * lighting->environmentMapColor);
        rd->setTexture(0, material.reflect.map);

        // Configure reflection map
        rd->configureReflectionMap(1, lighting->environmentMap);

        defaultRender(rd);
        setAdditive(rd, renderedOnce);

        // Disable reflection map
        rd->setTexture(1, 0);
    }

    // Add ambient + lights
    rd->enableLighting();
    if (! material.diffuse.isBlack() || ! material.specular.isBlack()) {
        rd->setTexture(0, material.diffuse.map);
        rd->setColor(material.diffuse.constant);

        // Fixed function does not receive specular texture maps, only constants.
        rd->setSpecularCoefficient(material.specular.constant);
        rd->setShininess(material.specularExponent.constant.average());

        // Ambient
        if (lighting.notNull()) {
            rd->setAmbientLightColor(lighting->ambientTop);
            if (lighting->ambientBottom != lighting->ambientTop) {
                rd->setLight(0, GLight::directional(-Vector3::unitY(), 
                    lighting->ambientBottom - lighting->ambientTop, false)); 
            }

            // Lights
            for (int L = 0; L < iMin(8, lighting->lightArray.size()); ++L) {
                rd->setLight(L + 1, lighting->lightArray[L]);
            }
        }

        defaultRender(rd);
        setAdditive(rd, renderedOnce);
    }

    return renderedOnce;
}


void PosedArticulatedModel::renderNonShadowed(
    RenderDevice*                   rd,
    const LightingRef&              lighting) const {

    if (! rd->colorWrite()) {
        // No need for fancy shading
        defaultRender(rd);
        return;
    }

    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];
    const SuperShader::Material& material = triList.material;

    rd->pushState();

    if (! material.transmit.isBlack()) {
        // Transparent
        bool oldDepthWrite = rd->depthWrite();

        // Render backfaces first, and then front faces
        rd->setCullFace(RenderDevice::CULL_FRONT);
        for (int i = 0; i <= 1; ++i) {
            rd->disableLighting();
            rd->enableTwoSidedLighting();

            // Modulate background by transparent color
            rd->setBlendFunc(RenderDevice::BLEND_ZERO, RenderDevice::BLEND_SRC_COLOR);
            rd->setTexture(0, material.transmit.map);
            rd->setColor(material.transmit.constant);
            defaultRender(rd);

            bool alreadyAdditive = false;
            setAdditive(rd, alreadyAdditive);
            renderNonShadowedOpaqueTerms(rd, lighting, part, triList, material);
        
            // restore depth write
            rd->setDepthWrite(oldDepthWrite);
            rd->setCullFace(RenderDevice::CULL_BACK);
        }
    } else {
        // Opaque
        rd->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ZERO);

        bool wroteDepth = renderNonShadowedOpaqueTerms(rd, lighting, part, triList, material);

        if (! wroteDepth) {
            // Draw black
            rd->disableLighting();
            rd->setColor(Color3::black());
            defaultRender(rd);
        }
    }

    rd->popState();
}


void PosedArticulatedModel::renderShadowedLightPass(
    RenderDevice*       rd, 
    const GLight&       light) const {

    // TODO
    debugAssertM(false, "Unimplemented");
}


void PosedArticulatedModel::renderShadowMappedLightPass(
    RenderDevice*       rd, 
    const GLight&       light, 
    const Matrix4&      lightMVP, 
    const TextureRef&   shadowMap) const {

    const ArticulatedModel::Part& part              = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList  = part.triListArray[listIndex];
    const SuperShader::Material& material           = triList.material;

    if (material.diffuse.isBlack() && material.specular.isBlack()) {
        return;
    }

    rd->pushState();
        switch (ArticulatedModel::profile()) {
        case ArticulatedModel::FIXED_FUNCTION:
            renderFFShadowMappedLightPass(rd, light, lightMVP, shadowMap, part, triList, material);
            break;

        case ArticulatedModel::PS20:
            renderPS20ShadowMappedLightPass(rd, light, lightMVP, shadowMap, part, triList, material);
            break;

        default:
            debugAssertM(false, "Fell through switch");
        }
    rd->popState();
}


void PosedArticulatedModel::renderPS20ShadowMappedLightPass(
    RenderDevice*       rd,
    const GLight&       light, 
    const Matrix4&      lightMVP, 
    const TextureRef&   shadowMap,
    const ArticulatedModel::Part& part,
    const ArticulatedModel::Part::TriList& triList,
    const SuperShader::Material& material) const {

    // TODO: implement
    renderFFShadowMappedLightPass(rd, light, lightMVP, shadowMap, part, triList, material);
}


void PosedArticulatedModel::renderFFShadowMappedLightPass(
    RenderDevice*       rd,
    const GLight&       light, 
    const Matrix4&      lightMVP, 
    const TextureRef&   shadowMap,
    const ArticulatedModel::Part& part,
    const ArticulatedModel::Part::TriList& triList,
    const SuperShader::Material& material) const {

    rd->configureShadowMap(1, lightMVP, shadowMap);

    rd->setObjectToWorldMatrix(cframe);

    rd->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);

    rd->setTexture(0, material.diffuse.map);
    rd->setColor(material.diffuse.constant);

    // We disable specular highlights because they will not be modulated
    // by the shadow map.  We then make a separate pass to render specular
    // highlights.
    rd->setSpecularCoefficient(Vector3::zero());

    rd->enableLighting();
    rd->setAmbientLightColor(Color3::black());

    rd->setLight(0, light);

    defaultRender(rd);

    if (! material.specular.isBlack()) {
        // Make a separate pass for specular. 
        static bool separateSpecular = GLCaps::supports("GL_EXT_separate_specular_color");

        if (separateSpecular) {
            // We disable the OpenGL separate
            // specular behavior so that the texture will modulate the specular
            // pass, and then put the specularity coefficient in the texture.
            glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SINGLE_COLOR_EXT);
        }

        rd->setColor(Color3::white());// TODO: when I put the specular coefficient here, it doesn't modulate.  What's wrong?
        rd->setTexture(0, material.specular.map);
        rd->setSpecularCoefficient(material.specular.constant);

        // Turn off the diffuse portion of this light
        GLight light2 = light;
        light2.diffuse = false;
        rd->setLight(0, light2);
        rd->setShininess(material.specularExponent.constant.average());

        defaultRender(rd);

        if (separateSpecular) {
            // Restore normal behavior
            glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, 
                          GL_SEPARATE_SPECULAR_COLOR_EXT);
        }

        // TODO: use this separate specular pass code in all fixed function 
        // cases where there is a specularity map.
    }
}


void PosedArticulatedModel::defaultRender(
    RenderDevice*           rd) const {

    CoordinateFrame o2w = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe);

    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];

    if (rd->renderMode() == RenderDevice::RENDER_SOLID) {
        rd->setShadeMode(RenderDevice::SHADE_SMOOTH);
        rd->beginIndexedPrimitives();
            rd->setVertexArray(part.vertexVAR);
            rd->setNormalArray(part.normalVAR);
            if (part.texCoordArray.size() > 0) {
                rd->setTexCoordArray(0, part.texCoord0VAR);
            }

            // In programmable pipeline mode, load the tangents into tex coord 1
            if ((part.tangentArray.size()) > 0 && 
                (ArticulatedModel::profile() != ArticulatedModel::FIXED_FUNCTION)) {
                rd->setTexCoordArray(1, part.tangentVAR);
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
            if (part.tangentArray.size() > 0) {
                rd->setTexCoord(1, part.tangentArray[v]);
            }
            rd->setNormal(part.geometry.normalArray[v]);
            rd->sendVertex(part.geometry.vertexArray[v]);
        }
        rd->endPrimitive();
    }

    rd->setObjectToWorldMatrix(o2w);
}


std::string PosedArticulatedModel::name() const {
    return _name;
}


bool PosedArticulatedModel::hasTransparency() const {
    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];
    return ! triList.material.transmit.isBlack();
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
