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

    debugAssert(ShaderGroup::fullySupported());
    debugPrintf(vs->messages().c_str());
    debugAssert(vs->ok());

    
    ShaderGroupRef effect = ShaderGroup::create(NULL, vs, NULL);
    debugPrintf(effect->messages().c_str());
    debugAssert(effect->ok());


    effect->glProgramObject();

    GLint maxLength;
    GLint i, uniformCount;
    GLcharARB **name;
    GLsizei *length;
    GLint *size;
    GLenum *type;

    //
    // Get the number of uniforms, and the length of the longest name.
    //
    glGetObjectParameterivARB(effect->glProgramObject(),
			      GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB,
			      &maxLength);
    glGetObjectParameterivARB(effect->glProgramObject(), GL_OBJECT_ACTIVE_UNIFORMS_ARB,
			      &uniformCount);

    //
    // Allocate arrays to store the answers in. For simplicity, the return
    // from malloc is not checked for NULL.
    //
    size   = (GLint *) malloc(uniformCount * sizeof(GLint));
    type   = (GLenum *) malloc(uniformCount * sizeof(GLenum));
    length = (GLsizei *) malloc(uniformCount * sizeof(GLsizei));
    name   = (GLcharARB **) malloc(uniformCount * sizeof(GLcharARB **));

    //
    // Loop over glGetActiveUniformARB and store the results away.
    //
    for (i = 0; i < uniformCount; i++) {
	    name[i] = (GLcharARB *) malloc(maxLength * sizeof(GLcharARB));
	    glGetActiveUniformARB(effect->glProgramObject(), 
            i, maxLength, &length[i], &size[i], &type[i], name[i]);
        debugPrintf("\"%s\" : %s\n", name[i], GLenumToString(type[i]));
    }

    return 0;
}
