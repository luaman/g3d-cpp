/**
 @file Shader.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2004-04-24
 @edited  2004-04-24
 */

#include "GLG3D/Shader.h"
#include "GLG3D/GLCaps.h"

namespace G3D {

GPUShader::GPUShader(const std::string& name, const std::string& code, bool _fromFile) :
    _name(name), _code(code + "\n"), _ok(true), fromFile(_fromFile) {
}


GPUShader* GPUShader::init(GPUShader* shader) {
    if (shader->fromFile) {
        if (fileExists(shader->_name)) {
            shader->_code = readFileAsString(shader->_name);
        } else {
            shader->_ok = false;
            shader->_message = format("Could not load shader file \"%s\".", 
                shader->_name.c_str());
        }
    }

    if (shader->_ok) {
        shader->compile();
    }

    return shader;
}


void GPUShader::compile() {

    GLint compiled = GL_FALSE;
	glShaderObject     = glCreateShaderObjectARB(glShaderType());

    // Compile the shader
	GLint length = _code.length();
    const GLcharARB* codePtr = static_cast<const GLcharARB*>(_code.c_str());

	glShaderSourceARB(glShaderObject, 1, &codePtr, &length);
	glCompileShaderARB(glShaderObject);
	glGetObjectParameterivARB(glShaderObject, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

    // Read the result of compilation
	GLint	  maxLength;
	glGetObjectParameterivARB(glShaderObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
    GLcharARB* pInfoLog = (GLcharARB *)malloc(maxLength * sizeof(GLcharARB));
	glGetInfoLogARB(glShaderObject, maxLength, &length, pInfoLog);

    // Copy the result to the output string
    _message = pInfoLog;
	free(pInfoLog);

    _ok = (compiled == GL_TRUE);
}


GPUShader::~GPUShader() {
	glDeleteObjectARB(glShaderObject);
}

////////////////////////////////////////////////////////////////////////////////////

VertexShaderRef VertexShader::fromFile(const std::string& filename) {
    return static_cast<VertexShader*>(GPUShader::init(new VertexShader(filename, "", true)));
}


VertexShaderRef VertexShader::fromCode(const std::string& name, const std::string& code) {
    return static_cast<VertexShader*>(GPUShader::init(new VertexShader(name, "", false)));
}

////////////////////////////////////////////////////////////////////////////////////

PixelShaderRef PixelShader::fromFile(const std::string& filename) {
    return static_cast<PixelShader*>(GPUShader::init(new PixelShader(filename, "", true)));
}


PixelShaderRef PixelShader::fromCode(const std::string& name, const std::string& code) {
    return static_cast<PixelShader*>(GPUShader::init(new PixelShader(name, "", false)));
}


////////////////////////////////////////////////////////////////////////////////////

bool ShaderGroup::fullySupported() {
    return
        GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_ARB_fragment_shader() &&
        GLCaps::supports_GL_ARB_vertex_shader();
}



}
