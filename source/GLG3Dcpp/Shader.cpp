/**
 @file Shader.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2004-04-24
 @edited  2004-04-24
 */

#include "GLG3D/Shader.h"
#include "GLG3D/GLCaps.h"

namespace G3D {

bool ShaderGroup::fullySupported() {
    return
        GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_ARB_fragment_shader() &&
        GLCaps::supports_GL_ARB_vertex_shader();
}

}
