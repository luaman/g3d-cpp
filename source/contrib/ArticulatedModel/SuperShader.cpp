/**
 @file SuperShader.cpp

 @author Morgan McGuire, matrix@graphics3d.com
 */

#include "SuperShader.h"

void SuperShader::configureFixedFunction(RenderDevice* rd) {

    // Set lighting
    rd->enableLighting();

    // Ambient
    rd->setAmbientLightColor(lighting->global.ambientTop);
    if (lighting->global.ambientBottom != lighting->global.ambientTop) {
        rd->setLight(0, GLight::directional(-Vector3::unitY(), 
            lighting->global.ambientBottom - lighting->global.ambientTop, false)); 
    }

    for (int L = 0; L < iMin(7, lighting->lightArray.size()); ++L) {
        rd->setLight(L + 1, lighting->lightArray[L]);
    }

    // Set materials
    rd->setTexture(0, material.diffuse.map);
    rd->setColor(material.diffuse.constant);
    rd->setSpecularCoefficient(material.specular.constant.average());
    rd->setShininess(material.specularExponent.constant.average());
    rd->setShadeMode(RenderDevice::SHADE_SMOOTH);
}


void SuperShader::LightingEnvironment::set(GameTime time, SkyRef sky) {
    LightingParameters lighting(time);
    
    if (sky.notNull()) {
        environment.constant = lighting.skyAmbient;
        environment.map = sky->getEnvironmentMap();
    }

    ambientTop = Color3(.9, .9, 1) * lighting.diffuseAmbient;
    ambientBottom = Color3::brown() * lighting.diffuseAmbient;

    lightArray.clear();
    lightArray.append(lighting.directionalLight());
    lightArray.last().color *= Color3(1, 1, .9);
}


void SuperShader::LightingEnvironment::configureShaderArgs(
    VertexAndPixelShader::ArgList& args) const {

    // TODO: special cases where all fields are not used
    args.set("ambientTop", ambientTop);
    args.set("ambientBottom", ambientBottom);
    args.set("environmentConstant", environment.constant);
    args.set("environmentMap", environment.map);
    args.set("lightPos", lightArray[0].position);
    args.set("lightColor", lightArray[0].color);
}


void SuperShader::Material::configureShaderArgs(
    VertexAndPixelShader::ArgList& args) const {
    
    args.set("diffuseMap",              diffuse.map);
    args.set("diffuseConstant",         diffuse.constant);
    args.set("specularMap",             specular.map);
    args.set("specularConstant",        specular.constant);
    args.set("specularExponentMap",     specularExponent.map);
    args.set("specularExponentConstant", specularExponent.constant);
    args.set("reflectMap",              reflect.map);
    args.set("reflectConstant",         reflect.constant);
    args.set("emitMap",                 emit.map);
    args.set("emitConstant",            emit.constant);
    args.set("normalMap",               normalMap);
    args.set("bumpMapScale",            bumpMapScale);
}


ShaderRef SuperShader::getShader(const Material& material) {
 
    ShaderRef s = cache.getSimilar(material);

    if (s.isNull()) {
        std::string path = "";//"../contrib/ArticulatedModel/";
        // TODO: only enable terms needed by this material
        s = Shader::fromFiles(path + "SuperShader.glsl.vert", "SuperShader.glsl.frag");
        cache.add(material, s);
    }

    return s;
}


SuperShaderRef SuperShader::create(const Material& mat) {
    return new SuperShader(mat);
}


SuperShader::SuperShader(const Material& mat) : material(mat) {
    if (profile() != FIXED_FUNCTION) {
        shader = getShader(material);
    }
}


bool SuperShader::ok() const {
    if (profile() == FIXED_FUNCTION) {
        return true;
    } else {
        return shader->ok();
    }
}


void SuperShader::beforePrimitive(RenderDevice* renderDevice) {
    if (profile() == FIXED_FUNCTION) {
        renderDevice->pushState();
        configureFixedFunction(renderDevice);
    } else {
        material.configureShaderArgs(shader->args);
        lighting->configureShaderArgs(shader->args);
        shader->beforePrimitive(renderDevice);
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
    }
}


void SuperShader::afterPrimitive(RenderDevice* renderDevice) {
    if (profile() == FIXED_FUNCTION) {
        renderDevice->popState();
    } else {
        shader->afterPrimitive(renderDevice);
    }
}


const std::string& SuperShader::messages() const {
    static const std::string t = "TODO";
    return t;
}


void SuperShader::setLighting(const LightingEnvironmentRef& lightingEnvironment) {
    lighting = lightingEnvironment;
}

////////////////////////////////////////////////////////////////////////////
SuperShader::Cache SuperShader::cache;

void SuperShader::Cache::add(const Material& mat, ShaderRef shader) {
    materialArray.append(mat);
    shaderArray.append(shader);
}


ShaderRef SuperShader::Cache::getSimilar(const Material& mat) const {
    for (int m = 0; m < materialArray.size(); ++m) {
        if (materialArray[m].similarTo(mat)) {
            return shaderArray[m];
        }
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////

bool SuperShader::Material::similarTo(const Material& other) const {
    return 
        diffuse.similarTo(other.diffuse) &&
        emit.similarTo(other.emit) &&
        specular.similarTo(other.specular) &&
        specularExponent.similarTo(other.specularExponent) &&
        transmit.similarTo(other.transmit) &&
        reflect.similarTo(other.reflect) &&
        (normalMap.isNull() == other.normalMap.isNull());
}
