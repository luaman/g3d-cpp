#include "SuperShader.h"

void SuperShader::configureFixedFunction(RenderDevice* rd) {

    // Set lighting
    rd->enableLighting();

    // Ambient
    rd->setAmbientLightColor(lighting->ambientTop);
    if (lighting->ambientBottom != lighting->ambientTop) {
        rd->setLight(0, GLight::directional(-Vector3::unitY(), 
            lighting->ambientBottom - lighting->ambientTop, false)); 
    }

    for (int L = 0; L < iMin(7, lighting->lightArray.size()); ++L) {
        rd->setLight(L + 1, lighting->lightArray[L]);
    }

    // Set materials
    rd->setTexture(0, material.diffuse.map);
    rd->setColor(material.diffuse.constant);
    rd->setSpecularCoefficient(material.specular.constant.average());
    rd->setShininess(material.specularExponent.constant.average());
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
    // TODO: only enable terms needed by this material
    // TODO: cache shaders

    std::string path = "";//"../contrib/ArticulatedModel/";
    return Shader::fromFiles(path + "SuperShader.glsl.vert", "SuperShader.glsl.frag");
}


SuperShaderRef SuperShader::create(const Material& mat) {
    return new SuperShader(mat);
}


SuperShader::SuperShader(const Material& mat) : material(mat) {
    shader = getShader(material);
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
