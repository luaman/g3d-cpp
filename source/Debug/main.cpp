/**
  @file demos/main.cpp

  This is a sample main.cpp to get you started with G3D.  It is
  designed to make writing an application easy.  Although the
  GApp/GApplet infrastructure is helpful for most projects,
  you are not restricted to using it-- choose the level of
  support that is best for your project (see the G3D Map in the
  documentation).

  @author Morgan McGuire, matrix@graphics3d.com
 */

#include <G3DAll.h>

#if G3D_VER != 60300
    #error Requires G3D 6.03
#endif


RenderDevice* rd;

int main(int argc, char** argv) {

    rd = new RenderDevice();
    rd->init(GWindowSettings());

    VertexShaderRef vs = VertexShader::fromFile("C:/tmp/nvcode/MEDIA/programs/glsl_simple_lighting/vertex_lighting.glsl");
    PixelShaderRef ps = NULL;//PixelShader::fromFile("C:/tmp/nvcode/MEDIA/programs/glsl_bump_mapping/bump_mapping_fragment.glsl");

    debugAssert(ShaderGroup::fullySupported());
    debugPrintf(vs->messages().c_str());
    debugAssert(vs->ok());
    
    if (! ps.isNull()) {
        debugAssert(ShaderGroup::fullySupported());
        debugPrintf(ps->messages().c_str());
        debugAssert(ps->ok());
    }
    
    ShaderGroupRef effect = ShaderGroup::create(NULL, vs, ps);
    debugPrintf(effect->messages().c_str());
    debugAssert(effect->ok());

    // Print the argument names
    for (int a = 0; a < effect->numArgs(); ++a) {
        debugPrintf("%s : %s\n", 
            effect->arg(a).name.c_str(), 
            GLenumToString(effect->arg(a).type));
    }

    ShaderGroup::ArgList args;
    args.set("lightVec", Vector3(1,1,1).direction());

    try {
        effect->bindArgList(rd, args);
    } catch (const ShaderGroup::ArgumentError& e) {
        alwaysAssertM(false, e.message);
    }

    rd->setShader(effect);

    return 0;
}
