/**
 @file SuperShader.cpp

 @author Morgan McGuire, matrix@graphics3d.com
 */

#include "SuperShader.h"

// TODO: remove
TextureRef SuperShader::Material::defaultNormalMap;
// TODO: remove
TextureRef SuperShader::Material::whiteMap;

void SuperShader::Material::configureShaderArgs(
    VertexAndPixelShader::ArgList& args) const {
    
    // TODO: remove
    if (whiteMap.isNull()) {
        GImage im(4,4,3);
        for (int y = 0; y < im.height; ++y) {
            for (int x = 0; x < im.width; ++x) {
                im.pixel3(x, y) = Color3(1, 1, 1);
            }
        }
        whiteMap = Texture::fromGImage("White", im, TextureFormat::RGB8);
    }

    // TODO: remove
    if (defaultNormalMap.isNull()) {
        GImage im(4,4,4);
        for (int y = 0; y < im.height; ++y) {
            for (int x = 0; x < im.width; ++x) {
                im.pixel4(x, y) = Color4(0.5, 0.5, 1.0, 0.0);
            }
        }
        defaultNormalMap = Texture::fromGImage("Normal Map", im, TextureFormat::RGBA8);
    }

    // TODO: don't even set fields that have no corresponding map
    args.set("diffuseMap",              diffuse.map.notNull() ? diffuse.map : whiteMap);
    args.set("diffuseConstant",         diffuse.constant);
    args.set("specularMap",             specular.map.notNull() ? specular.map : whiteMap);
    args.set("specularConstant",        specular.constant);
    args.set("specularExponentMap",     specularExponent.map.notNull() ? specularExponent.map : whiteMap);
    args.set("specularExponentConstant", specularExponent.constant);
    args.set("reflectMap",              reflect.map.notNull() ? reflect.map : whiteMap);
    args.set("reflectConstant",         reflect.constant);
    args.set("emitMap",                 emit.map.notNull() ? emit.map : whiteMap);
    args.set("emitConstant",            emit.constant);
    args.set("normalMap",               normalMap.notNull() ? normalMap : defaultNormalMap);
    args.set("bumpMapScale",            bumpMapScale);
}


ShaderRef SuperShader::getShader(const Material& material) {
 
    ShaderRef s = cache.getSimilar(material);

    if (s.isNull()) {
        std::string path = "";//"../contrib/ArticulatedModel/";

        // TODO: only enable terms needed by this material
        s = Shader::fromFiles(path + "SuperShader.glsl.vrt", "SuperShader.glsl.frg");
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


void SuperShader::setLighting(const LightingRef& _lighting) {
    lighting = _lighting;
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
