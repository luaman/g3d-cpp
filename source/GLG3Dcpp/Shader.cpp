/**
 @file Shader.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2004-04-24
 @edited  2004-04-27
 */

#include "GLG3D/Shader.h"
#include "GLG3D/GLCaps.h"

namespace G3D {

GPUShader::GPUShader(const std::string& name, const std::string& code, bool _fromFile) :
    _name(name), _code(code + "\n"), _ok(true), fromFile(_fromFile) {
}


GPUShader* GPUShader::init(GPUShader* shader, bool debug) {
    if (shader->fromFile) {
        if (fileExists(shader->_name)) {
            shader->_code = readFileAsString(shader->_name);
        } else {
            shader->_ok = false;
            shader->_messages = format("Could not load shader file \"%s\".", 
                shader->_name.c_str());
        }
    }

    if (shader->_ok) {
        shader->compile();
    }

    if (debug) {
        alwaysAssertM(shader->ok(), shader->messages());
    }

    return shader;
}


void GPUShader::compile() {

    GLint compiled = GL_FALSE;
	_glShaderObject     = glCreateShaderObjectARB(glShaderType());

    // Compile the shader
	GLint length = _code.length();
    const GLcharARB* codePtr = static_cast<const GLcharARB*>(_code.c_str());

	glShaderSourceARB(_glShaderObject, 1, &codePtr, &length);
	glCompileShaderARB(_glShaderObject);
	glGetObjectParameterivARB(_glShaderObject, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);

    // Read the result of compilation
	GLint	  maxLength;
	glGetObjectParameterivARB(_glShaderObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
    GLcharARB* pInfoLog = (GLcharARB *)malloc(maxLength * sizeof(GLcharARB));
	glGetInfoLogARB(_glShaderObject, maxLength, &length, pInfoLog);

    // Copy the result to the output string
    _messages = pInfoLog;
	free(pInfoLog);

    _ok = (compiled == GL_TRUE);
}


GPUShader::~GPUShader() {
	glDeleteObjectARB(_glShaderObject);
}

////////////////////////////////////////////////////////////////////////////////////

VertexShaderRef VertexShader::fromFile(const std::string& filename, bool debug) {
    return static_cast<VertexShader*>(GPUShader::init(new VertexShader(filename, "", true), debug));
}


VertexShaderRef VertexShader::fromCode(const std::string& name, const std::string& code, bool debug) {
    return static_cast<VertexShader*>(GPUShader::init(new VertexShader(name, "", false), debug));
}

////////////////////////////////////////////////////////////////////////////////////

PixelShaderRef PixelShader::fromFile(const std::string& filename, bool debug) {
    return static_cast<PixelShader*>(GPUShader::init(new PixelShader(filename, "", true), debug));
}


PixelShaderRef PixelShader::fromCode(const std::string& name, const std::string& code, bool debug) {
    return static_cast<PixelShader*>(GPUShader::init(new PixelShader(name, "", false), debug));
}

////////////////////////////////////////////////////////////////////////////////////

ShaderGroup::ShaderGroup(
    const ObjectShaderRef& os,
    const VertexShaderRef& vs,
    const PixelShaderRef&  ps) : 
        _objectShader(os), 
        _vertexShader(vs), 
        _pixelShader(ps), 
        _ok(true) {

    if (! os.isNull() && ! os->ok()) {
        _ok = false;
        _messages += os->messages() + "\n";
    }
    
    if (! vs.isNull() && ! vs->ok()) {
        _ok = false;
        _messages += vs->messages() + "\n";
    }    

    if (! ps.isNull() && ! ps->ok()) {
        _ok = false;
        _messages += ps->messages() + "\n";
    }    

    if (_ok) {
        // Create GL object
        _glProgramObject = glCreateProgramObjectARB();

        // Attach vertex and pixel shaders
        if (! vertexShader().isNull()) {
            glAttachObjectARB(_glProgramObject, vertexShader()->glShaderObject());
        }

        if (! pixelShader().isNull()) {
            glAttachObjectARB(_glProgramObject, pixelShader()->glShaderObject());
        }

        // Link
        GLint linked = GL_FALSE;
        glLinkProgramARB(_glProgramObject);

        // Read back messages
	    glGetObjectParameterivARB(_glProgramObject, GL_OBJECT_LINK_STATUS_ARB, &linked);
        GLint maxLength = 0, length = 0;
	    glGetObjectParameterivARB(_glProgramObject, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
	    GLcharARB* pInfoLog = (GLcharARB *)malloc(maxLength * sizeof(GLcharARB));
	    glGetInfoLogARB(_glProgramObject, maxLength, &length, pInfoLog);

        _messages += std::string(pInfoLog) + "\n";
	    free(pInfoLog);
        _ok = _ok && (linked == GL_TRUE);
    }

    if (_ok) {
        computeUniformArray();
    }
}


bool ShaderGroup::isSamplerType(GLenum e) {
    return
       (e == GL_TEXTURE_1D) ||
       (e == GL_TEXTURE_2D) ||
       (e == GL_TEXTURE_3D) ||
       (e == GL_TEXTURE_RECTANGLE_EXT) ||
       (e == GL_TEXTURE_CUBE_MAP_ARB);
}


void ShaderGroup::computeUniformArray() {
    uniformArray.clear();

    GLint maxLength;
    GLint uniformCount;

    // Get the number of uniforms, and the length of the longest name.
    glGetObjectParameterivARB(glProgramObject(), GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxLength);
    glGetObjectParameterivARB(glProgramObject(), GL_OBJECT_ACTIVE_UNIFORMS_ARB, &uniformCount);

    uniformArray.resize(uniformCount);

    GLcharARB* name = (GLcharARB *) malloc(maxLength * sizeof(GLcharARB));
    
    int lastSampler = -1;
    // Loop over glGetActiveUniformARB and store the results away.
    for (int i = 0; i < uniformCount; ++i) {

        GLint size;
        GLenum type;

	    glGetActiveUniformARB(glProgramObject(), 
            i, maxLength, NULL, &size, &type, name);

        uniformArray[i].name = name;
        uniformArray[i].size = size;
        uniformArray[i].type = type;

        if (isSamplerType(type)) {
            ++lastSampler;
            uniformArray[i].sampler = lastSampler;
        } else {
            uniformArray[i].sampler = -1;
        }
    }

    free(name);
}


ShaderGroupRef ShaderGroup::create(
    const ObjectShaderRef& os,
    const VertexShaderRef& vs,
    const PixelShaderRef&  ps) {

    return new ShaderGroup(os, vs, ps);
}


ShaderGroup::~ShaderGroup() {
    glDeleteObjectARB(_glProgramObject);
}


bool ShaderGroup::fullySupported() {
    return
        GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_ARB_fragment_shader() &&
        GLCaps::supports_GL_ARB_vertex_shader();
}


GLenum ShaderGroup::canonicalType(GLenum e) {

    switch (e) {
    case GL_INT:
    case GL_BOOL_ARB:
        return GL_FLOAT;

    case GL_INT_VEC2_ARB:
    case GL_BOOL_VEC2_ARB:
        return GL_FLOAT_VEC2_ARB;

    case GL_INT_VEC3_ARB:
    case GL_BOOL_VEC3_ARB:
        return GL_FLOAT_VEC3_ARB;

    case GL_INT_VEC4_ARB:
    case GL_BOOL_VEC4_ARB:
        return GL_FLOAT_VEC4_ARB;
        
    default:
        // Return the input
        return e;    
    }
}

////////////////////////////////////////////////////////////////////////

void ShaderGroup::ArgList::set(const std::string& var, const TextureRef& val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type    = val->getOpenGLTextureTarget();
    arg.texture = val;
    argTable.set(var, arg);

}


void ShaderGroup::ArgList::set(const std::string& var, const CoordinateFrame& val) {
    set(var, Matrix4(val));
}


void ShaderGroup::ArgList::set(const std::string& var, const Matrix4& val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type = GL_FLOAT_MAT4_ARB;
    for (int r = 0; r < 4; ++r) {
        arg.vector[r] = val.getRow(r);
    }

    argTable.set(var, arg);
}


void ShaderGroup::ArgList::set(const std::string& var, const Vector4& val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type = GL_FLOAT_VEC4_ARB;
    arg.vector[0] = val;
    argTable.set(var, arg);
}


void ShaderGroup::ArgList::set(const std::string& var, const Vector3& val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type = GL_FLOAT_VEC3_ARB;
    arg.vector[0] = Vector4(val, 0);
    argTable.set(var, arg);

}


void ShaderGroup::ArgList::set(const std::string& var, const Vector2& val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type = GL_FLOAT_VEC2_ARB;
    arg.vector[0] = Vector4(val, 0, 0);
    argTable.set(var, arg);
}


void ShaderGroup::ArgList::set(const std::string& var, float          val) {
    alwaysAssertM(! argTable.containsKey(var), std::string("Cannot set variable \"") + var + "\" more than once");

    Arg arg;
    arg.type = GL_FLOAT;
    arg.vector[0] = Vector4(val, 0, 0, 0);
    argTable.set(var, arg);
}


void ShaderGroup::ArgList::clear() {
    argTable.clear();
}



}
