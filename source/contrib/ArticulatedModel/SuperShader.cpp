#include "SuperShader.h"

void SuperShader::configureFixedFunction(
    RenderDevice* rd,
    LightingEnvironmentRef env) {

    rd->enableLighting();

    // Ambient
    rd->setAmbientLightColor(env->ambientTop);
    if (env->ambientBottom != env->ambientTop) {
        rd->setLight(0, GLight::directional(-Vector3::unitY(), env->ambientBottom - env->ambientTop, false)); 
    }

    for (int L = 0; L < iMin(7, env->lightArray.size()); ++L) {
        rd->setLight(L + 1, env->lightArray[L]);
    }
}


SuperShader::GraphicsCapabilities SuperShader::graphicsCapabilities() {
    if (GLCaps::supports_GL_ARB_shader_objects()) {
        return PS20;
    } else {
        return FIXED_FUNCTION;
    }
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



ShaderRef SuperShader::createShader(const Material& material) {
    // TODO: only enable terms needed by this material
    // TODO: cache shaders

    std::string path = "../contrib/ArticulatedModel/";
    return Shader::fromFiles(path + "SuperShader.glsl.vert", "SuperShader.glsl.frag");
}

