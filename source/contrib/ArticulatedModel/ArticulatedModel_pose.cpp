/**
 @file ArticulatedModel_pose.cpp
 Rendering and pose aspects of ArticulatedModel

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2004-11-20
  @edited  2005-11-25

  Copyright 2004-2005, Morgan McGuire
 */
//todo:hack reflections for cards w/o cube maps
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

protected:

    /** Called from render to draw geometry after the material properties are set.*/
    void sendGeometry(RenderDevice* rd) const;

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

    bool renderPS14NonShadowedOpaqueTerms(
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


/** PS14 often needs a dummy texture map in order to enable a combiner */
static TextureRef whiteMap() {
    static TextureRef map;

    if (map.isNull()) {
        GImage im(4,4,3);
        for (int y = 0; y < im.height; ++y) {
            for (int x = 0; x < im.width; ++x) {
                im.pixel3(x, y) = Color3(1, 1, 1);
            }
        }
        map = Texture::fromGImage("White", im, TextureFormat::RGB8);
    }
    return map;
}

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
        frame = parent * cframe * posex.cframe[name];
    } else {
        frame = parent * cframe;
    }

    if (hasGeometry()) {

        for (int t = 0; t < triListArray.size(); ++t) {
            PosedArticulatedModel* posed = new PosedArticulatedModel();

            posed->cframe = frame;
            posed->_name = model->name + ": " + name;
            posed->partIndex = partIndex;
            posed->listIndex = t;
            posed->model = model;

            posedArray.append(posed);
        }
    }

    // TODO: recursively render sub-parts an pass along our frame.
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


        if (triList.twoSided) {
            renderDevice->pushState();
            renderDevice->enableTwoSidedLighting();
            renderDevice->setCullFace(RenderDevice::CULL_NONE);
        }

        sendGeometry(renderDevice);

        if (triList.twoSided) {
            renderDevice->popState();
        }

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

        case ArticulatedModel::PS14:
            renderedOnce = renderPS14NonShadowedOpaqueTerms(rd, lighting, part, triList, material);
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

    SuperShader::configureShader(lighting, material, triList.nonShadowedShader->args);
    rd->setShader(triList.nonShadowedShader);
    sendGeometry(rd);

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
        sendGeometry(rd);
        setAdditive(rd, renderedOnce);
    }
    
    // Add reflective
    if (! material.reflect.isBlack() && 
        lighting.notNull() &&
        (lighting->environmentMapColor != Color3::black())) {

        rd->pushState();

            // Reflections are specular and not affected by surface texture, only
            // the reflection coefficient
            rd->setColor(material.reflect.constant * lighting->environmentMapColor);
            rd->setTexture(0, material.reflect.map);

            // Configure reflection map
            if (lighting->environmentMap.isNull()) {
                rd->setTexture(1, NULL);
            } else if (GLCaps::supports_GL_ARB_texture_cube_map() &&
                (lighting->environmentMap->getDimension() == Texture::DIM_CUBE_MAP)) {
                rd->configureReflectionMap(1, lighting->environmentMap);
            } else {
                // Use the top texture as a sphere map
                glActiveTextureARB(GL_TEXTURE0_ARB + 1);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);

                rd->setTexture(1, lighting->environmentMap);
            }

            sendGeometry(rd);
            setAdditive(rd, renderedOnce);

            // Disable reflection map
            rd->setTexture(1, NULL);
        rd->popState();
    }

    bool ps14 = ArticulatedModel::profile() == ArticulatedModel::PS14;

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

        if (renderedOnce) {
            // Make sure we add this pass to the previous terms
            rd->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);
        }

        sendGeometry(rd);
        setAdditive(rd, renderedOnce);
    }

    return renderedOnce;
}


bool PosedArticulatedModel::renderPS14NonShadowedOpaqueTerms(
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
        sendGeometry(rd);
        setAdditive(rd, renderedOnce);
    }
    

    // Full combiner setup (in practice, we only use combiners that are
    // needed):
    //
    // Unit 0:
    // Mode = modulate
    // arg0 = primary
    // arg1 = texture (diffuse)
    //
    // Unit 1:
    // Mode = modulate
    // arg0 = constant (envmap constant * envmap color)
    // arg1 = texture (envmap)
    //
    // Unit 2:
    // Mode = modulate
    // arg0 = previous
    // arg1 = texture (reflectmap)
    //
    // Unit 3:
    // Mode = add
    // arg0 = previous
    // arg1 = texture0


    bool hasDiffuse = ! material.diffuse.isBlack();
    bool hasReflection = ! material.reflect.isBlack() && 
            lighting.notNull() &&
            (lighting->environmentMapColor != Color3::black());

    // Add reflective and diffuse

    rd->pushState();
    // We're going to use combiners, which G3D does not preserve
    glPushAttrib(GL_TEXTURE_BIT);

        GLint nextUnit = 0;
        GLint diffuseUnit = GL_PRIMARY_COLOR_ARB;

        if (hasDiffuse) {

            // Add ambient + lights
            rd->enableLighting();
            if (! material.diffuse.isBlack() || ! material.specular.isBlack()) {
                rd->setTexture(nextUnit, material.diffuse.map);
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
            }

            if (material.diffuse.map.notNull()) {
                glActiveTextureARB(GL_TEXTURE0_ARB + nextUnit);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
                glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_MODULATE);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_PRIMARY_COLOR_ARB);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);
                diffuseUnit = GL_TEXTURE0_ARB + nextUnit;
                ++nextUnit;
            }
        }

        if (hasReflection) {

            // First configure the reflection map.  There must be one or we wouldn't
            // have taken this branch.

            if (GLCaps::supports_GL_ARB_texture_cube_map() &&
                (lighting->environmentMap->getDimension() == Texture::DIM_CUBE_MAP)) {
                rd->configureReflectionMap(nextUnit, lighting->environmentMap);
            } else {
                // Use the top texture as a sphere map
                glActiveTextureARB(GL_TEXTURE0_ARB + nextUnit);
                glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
                glEnable(GL_TEXTURE_GEN_S);
                glEnable(GL_TEXTURE_GEN_T);

                rd->setTexture(nextUnit, lighting->environmentMap);
            }
            debugAssertGLOk();

            glActiveTextureARB(GL_TEXTURE0_ARB + nextUnit);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_MODULATE);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_CONSTANT_ARB);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, Color4(material.reflect.constant * lighting->environmentMapColor, 1));
            debugAssertGLOk();

            ++nextUnit;

            rd->setTexture(nextUnit, material.reflect.map);
            if (material.reflect.map.notNull()) {
                // If there is a reflection map for the surface, modulate
                // the reflected color by it.
                glActiveTextureARB(GL_TEXTURE0_ARB + nextUnit);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
                glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_MODULATE);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_PREVIOUS_ARB);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);
                ++nextUnit;
                debugAssertGLOk();
            }

            if (hasDiffuse) {
                // Need a dummy texture
                rd->setTexture(nextUnit, whiteMap());

                // Add diffuse to the previous (reflective) unit
                glActiveTextureARB(GL_TEXTURE0_ARB + nextUnit);
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
                glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_ADD);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_PREVIOUS_ARB);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
                glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   diffuseUnit);
                glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);
                debugAssertGLOk();
                ++nextUnit;
            }
        }


        sendGeometry(rd);
        setAdditive(rd, renderedOnce);

    glPopAttrib();
    rd->popState();


    return renderedOnce;
}


void PosedArticulatedModel::renderNonShadowed(
    RenderDevice*                   rd,
    const LightingRef&              lighting) const {

    if (! rd->colorWrite()) {
        // No need for fancy shading
        render(rd);
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
        int passes = triList.twoSided ? 2 : 1;

        if (triList.twoSided) {
            // We're going to render the front and back faces separately.
            rd->setCullFace(RenderDevice::CULL_FRONT);
            rd->enableTwoSidedLighting();
        }

        for (int i = 0; i < passes; ++i) {
            rd->disableLighting();

            // Modulate background by transparent color
            rd->setBlendFunc(RenderDevice::BLEND_ZERO, RenderDevice::BLEND_SRC_COLOR);
            rd->setTexture(0, material.transmit.map);
            rd->setColor(material.transmit.constant);
            sendGeometry(rd);

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

        bool ps20 = ArticulatedModel::profile() == ArticulatedModel::PS20;

        if (triList.twoSided) {
            if (! ps20) {
                rd->enableTwoSidedLighting();
                rd->setCullFace(RenderDevice::CULL_NONE);
            } else {
                // Even if back face culling is reversed, for two-sided objects 
                // we always draw the front.
                rd->setCullFace(RenderDevice::CULL_BACK);
            }
        }

        bool wroteDepth = renderNonShadowedOpaqueTerms(rd, lighting, part, triList, material);

        if (triList.twoSided && ps20) {
            // gl_FrontFacing doesn't work on most cards, so we have to draw two-sided objects twice
            rd->setCullFace(RenderDevice::CULL_FRONT);
            triList.nonShadowedShader->args.set("backside", -1.0f);
            renderNonShadowedOpaqueTerms(rd, lighting, part, triList, material);
            triList.nonShadowedShader->args.set("backside", 1.0f);
        }
        

        if (! wroteDepth) {
            // We failed to write to the depth buffer, so
            // do so now.
            rd->disableLighting();
            rd->setColor(Color3::black());
            sendGeometry(rd);
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
        case ArticulatedModel::PS14:
            if (triList.twoSided) {
                rd->enableTwoSidedLighting();
                rd->setCullFace(RenderDevice::CULL_NONE);
            }
            renderFFShadowMappedLightPass(rd, light, lightMVP, shadowMap, part, triList, material);
            break;

        case ArticulatedModel::PS20:
            if (triList.twoSided) {
                // Even if back face culling is reversed, for two-sided objects 
                // we always draw the front.
                rd->setCullFace(RenderDevice::CULL_BACK);
            }

            renderPS20ShadowMappedLightPass(rd, light, lightMVP, shadowMap, part, triList, material);

            if (triList.twoSided) {
                // gl_FrontFacing doesn't work on most cards, so we have to draw two-sided objects twice
                rd->setCullFace(RenderDevice::CULL_FRONT);
                triList.shadowMappedShader->args.set("backside", -1.0f);
                renderPS20ShadowMappedLightPass(rd, light, lightMVP, shadowMap, part, triList, material);
                triList.shadowMappedShader->args.set("backside", 1.0f);
            }
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

    if (material.specular.isBlack() &&
        material.diffuse.isBlack()) {
        // Nothing to draw
        return;
    }

    rd->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);
    SuperShader::configureShadowShader(light, lightMVP, shadowMap, material, triList.shadowMappedShader->args);
    rd->setShader(triList.shadowMappedShader);
    sendGeometry(rd);
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

    sendGeometry(rd);

    if (! material.specular.isBlack()) {
        // Make a separate pass for specular. 
        static bool separateSpecular = GLCaps::supports("GL_EXT_separate_specular_color");

        if (separateSpecular) {
            // We disable the OpenGL separate
            // specular behavior so that the texture will modulate the specular
            // pass, and then put the specularity coefficient in the texture.
            glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SINGLE_COLOR_EXT);
        }

        rd->setColor(Color3::white()); // TODO: when I put the specular coefficient here, it doesn't modulate.  What's wrong?
        rd->setTexture(0, material.specular.map);
        rd->setSpecularCoefficient(material.specular.constant);

        // Turn off the diffuse portion of this light
        GLight light2 = light;
        light2.diffuse = false;
        rd->setLight(0, light2);
        rd->setShininess(material.specularExponent.constant.average());

        sendGeometry(rd);

        if (separateSpecular) {
            // Restore normal behavior
            glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, 
                          GL_SEPARATE_SPECULAR_COLOR_EXT);
        }

        // TODO: use this separate specular pass code in all fixed function 
        // cases where there is a specularity map.
    }
}


void PosedArticulatedModel::sendGeometry(
    RenderDevice*           rd) const {

    const ArticulatedModel::Part& part = model->partArray[partIndex];
    const ArticulatedModel::Part::TriList& triList = part.triListArray[listIndex];

    CoordinateFrame o2w = rd->getObjectToWorldMatrix();
    rd->setObjectToWorldMatrix(cframe);

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
                (ArticulatedModel::profile() == ArticulatedModel::PS20)) {
                rd->setTexCoordArray(1, part.tangentVAR);
            }

            rd->sendIndices(RenderDevice::TRIANGLES, triList.indexArray);
        rd->endIndexedPrimitives();

    } else {

        // Radeon mobility cards crash rendering VAR in wireframe mode.
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
