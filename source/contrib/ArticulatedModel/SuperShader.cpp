/**
 @file SuperShader.cpp

 @author Morgan McGuire, matrix@graphics3d.com
 */

#include "SuperShader.h"

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
    shader = getShader(material);
}


bool SuperShader::ok() const {
    return shader->ok();
}


void SuperShader::beforePrimitive(RenderDevice* renderDevice) {
    material.configureShaderArgs(shader->args);

    // TODO
    //lighting->configureShaderArgs(shader->args);
    shader->beforePrimitive(renderDevice);
    renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
}


void SuperShader::afterPrimitive(RenderDevice* renderDevice) {
    shader->afterPrimitive(renderDevice);
}

const std::string& SuperShader::messages() const {
    static const std::string t = "TODO";
    return t;
}

/*
void SuperShader::setLighting(const LightingEnvironmentRef& lightingEnvironment) {
    lighting = lightingEnvironment;
}
*/

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
