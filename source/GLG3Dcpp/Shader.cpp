/**
 @file Shader.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2004-04-24
 @edited  2004-07-18
 */

#include "GLG3D/Shader.h"
#include "GLG3D/GLCaps.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/RenderDevice.h"

namespace G3D {

void VertexAndPixelShader::GPUShader::init(
	const std::string&	name,
	const std::string&	code,
	bool				_fromFile,
	bool				debug,	
	GLenum				glType,
	const std::string&	type) {

	_name			= name;
	_shaderType		= type;
	_glShaderType	= glType;
	_code			= code + "\n";
	_ok				= true;
	fromFile		= _fromFile;
	_fixedFunction	= (name == "") && (code == "");

	if (! _fixedFunction) {		
		if (fromFile) {
			if (fileExists(_name)) {
				_code = readFileAsString(_name);
			} else {
				_ok = false;
				_messages = format("Could not load shader file \"%s\".", 
					_name.c_str());
			}
		}

		if (_ok) {
			compile();
		}

		if (debug) {
			alwaysAssertM(ok(), messages());
		}
	}
}


void VertexAndPixelShader::GPUShader::compile() {

    GLint compiled = GL_FALSE;
	_glShaderObject = glCreateShaderObjectARB(glShaderType());

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

	int c = 0;
    // Copy the result to the output string, prepending the filename
	while (pInfoLog[c] != '\0') {
		_messages += _name;
		while (pInfoLog[c] != '\n' && pInfoLog[c] != '\r' && pInfoLog[c] != '\0') {
			_messages += pInfoLog[c];
			++c;
		}

		if (pInfoLog[c] == '\r' && pInfoLog[c + 1] == '\n') {
			// Windows newline
			#ifdef G3D_WIN32
				_messages += "\r\n";
			#else
				_messages += "\n";
			#endif
			c += 2;
		} else if (pInfoLog[c] == '\r' && pInfoLog[c + 1] != '\n') {
			// Dangling \r; treat it as a newline
			_messages += "\r\n";
			++c;
		} else if (pInfoLog[c] == '\n') {
			// Newline
			#ifdef G3D_WIN32
				_messages += "\r\n";
			#else
				_messages += "\n";
			#endif
			++c;
		}
	}
	free(pInfoLog);

    _ok = (compiled == GL_TRUE);
}


VertexAndPixelShader::GPUShader::~GPUShader() {
	if (! _fixedFunction) {
		glDeleteObjectARB(_glShaderObject);
	}
}


////////////////////////////////////////////////////////////////////////////////////

VertexAndPixelShader::VertexAndPixelShader(
	const std::string&  vsCode,
	const std::string&  vsFilename,
	bool                vsFromFile,
	const std::string&  psCode,
	const std::string&  psFilename,
	bool                psFromFile,
    bool                debug) :
        _ok(true) {

	vertexShader.init(vsFilename, vsCode, vsFromFile, debug, GL_VERTEX_SHADER_ARB, "Vertex Shader");
	pixelShader.init(psFilename, psCode, psFromFile, debug, GL_FRAGMENT_SHADER_ARB, "Pixel Shader");
    
    if (! vertexShader.ok()) {
        _ok = false;
        _vertCompileMessages += vertexShader.messages();
        _messages += 
			std::string("Compiling ") + vertexShader.shaderType() + " " + vsFilename + NEWLINE +
			vertexShader.messages() + NEWLINE + NEWLINE;
    }

    if (! pixelShader.ok()) {
        _ok = false;
        _fragCompileMessages += pixelShader.messages();
        _messages += 
			std::string("Compiling ") + pixelShader.shaderType() + " " + psFilename + NEWLINE +
			pixelShader.messages() + NEWLINE + NEWLINE;
    }    

    if (_ok) {
        // Create GL object
        _glProgramObject = glCreateProgramObjectARB();

        // Attach vertex and pixel shaders
        if (! vertexShader.fixedFunction()) {
            glAttachObjectARB(_glProgramObject, vertexShader.glShaderObject());
        }

        if (! pixelShader.fixedFunction()) {
            glAttachObjectARB(_glProgramObject, pixelShader.glShaderObject());
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

        _messages += std::string("Linking\n") + std::string(pInfoLog) + "\n";
        _linkMessages += std::string(pInfoLog);
	    free(pInfoLog);
        _ok = _ok && (linked == GL_TRUE);

        if (debug) {
            alwaysAssertM(_ok, _messages);
        }
    }

    if (_ok) {
        computeUniformArray();
    }
}


bool VertexAndPixelShader::isSamplerType(GLenum e) {
    return
       (e == GL_SAMPLER_1D_ARB) ||
       (e == GL_SAMPLER_2D_ARB) ||
       (e == GL_SAMPLER_2DRECT_ARB) ||
       (e == GL_SAMPLER_3D_ARB) ||
       (e == GL_SAMPLER_CUBE_ARB);
}


void VertexAndPixelShader::computeUniformArray() {
    uniformArray.clear();

    GLint maxLength;
    GLint uniformCount;

    // Get the number of uniforms, and the length of the longest name.
    glGetObjectParameterivARB(glProgramObject(), GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxLength);
    glGetObjectParameterivARB(glProgramObject(), GL_OBJECT_ACTIVE_UNIFORMS_ARB, &uniformCount);

    uniformArray.resize(uniformCount);

    GLcharARB* name = (GLcharARB *) malloc(maxLength * sizeof(GLcharARB));
    
    int lastTextureUnit = -1;
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
            ++lastTextureUnit;
            uniformArray[i].textureUnit = lastTextureUnit;
        } else {
            uniformArray[i].textureUnit = -1;
        }
    }

    free(name);
}


VertexAndPixelShaderRef VertexAndPixelShader::fromStrings(
	const std::string& vs,
    const std::string& ps,
    bool debugErrors) {

    return new VertexAndPixelShader(vs, "", false, ps, "", false, debugErrors);
}


VertexAndPixelShaderRef VertexAndPixelShader::fromFiles(
	const std::string& vsFilename,
    const std::string& psFilename,
    bool debugErrors) {

	std::string vs;
	std::string ps;

	if (vsFilename != "") {
		vs = readFileAsString(vs);
	}

	if (psFilename != "") {
		ps = readFileAsString(ps);
	}

    return new VertexAndPixelShader(vs, vsFilename, true, ps, psFilename, true, debugErrors);
}


VertexAndPixelShader::~VertexAndPixelShader() {
    glDeleteObjectARB(_glProgramObject);
}


bool VertexAndPixelShader::fullySupported() {
    return
        GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_ARB_fragment_shader() &&
        GLCaps::supports_GL_ARB_vertex_shader();
}


GLenum VertexAndPixelShader::canonicalType(GLenum e) {

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

	case GL_SAMPLER_2D_ARB:
		return GL_TEXTURE_2D;

	case GL_SAMPLER_CUBE_ARB:
		return GL_TEXTURE_CUBE_MAP_ARB;
 
	case GL_SAMPLER_2DRECT_ARB:
		return GL_TEXTURE_RECTANGLE_EXT;

    default:
        // Return the input
        return e;    
    }
}


void VertexAndPixelShader::validateArgList(const ArgList& args) const {
    int numVariables = 0;

    // Iterate through formal bindings
    for (int u = 0; u < uniformArray.size(); ++u) {
        const UniformDeclaration& decl = uniformArray[u];
        
        ++numVariables;
        if (! args.argTable.containsKey(decl.name)) {
            throw ArgumentError(
                format("No value provided for VertexAndPixelShader uniform variable %s of type %s.",
                    decl.name.c_str(), GLenumToString(decl.type)));
        }

        const ArgList::Arg& arg = args.argTable[decl.name];

        // check the type
        if (canonicalType(arg.type) != canonicalType(decl.type)) {
            throw ArgumentError(
            format("Variable %s was declared as type %s and the value provided at runtime had type %s.",
                    decl.name.c_str(), GLenumToString(decl.type), GLenumToString(arg.type)));
        }

    }

    if (numVariables < args.argTable.size()) {
        // Some variables were unused.  Figure out which they were.
        Table<std::string, ArgList::Arg>::Iterator arg = args.argTable.begin();
        Table<std::string, ArgList::Arg>::Iterator end = args.argTable.end();

        while (arg != end) {
            // See if this arg was in the formal binding list

            bool foundArgument = false;
            
            for (int u = 0; u < uniformArray.size(); ++u) {
                if (uniformArray[u].name == arg->key) {
                    foundArgument = true;
                    break;
                }
            }

            if (! foundArgument) {
                throw ArgumentError(
                std::string("Extra VertexAndPixelShader uniform variable provided at runtime: ") + arg->key + ".");
            }

            ++arg;
        }
    }

}


void VertexAndPixelShader::bindArgList(RenderDevice* rd, const ArgList& args) const {
    validateArgList(args);

    // Iterate through the formal parameter list
    for (int u = 0; u < uniformArray.size(); ++u) {
        const UniformDeclaration& decl  = uniformArray[u];
        const ArgList::Arg&       value = args.argTable.get(decl.name); 

        // Bind based on the declared type
        switch (canonicalType(decl.type)) {
        case GL_TEXTURE_1D:
            debugAssertM(false, "1D texture binding not implemented");
            break;

        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP_ARB:
        case GL_TEXTURE_RECTANGLE_EXT:
            // Textures are bound as if they were integers.  The
            // value of the texture is the texture unit into which
            // the texture is placed.
            glUniform1iARB(u, decl.textureUnit);
            rd->setTexture(decl.textureUnit, value.texture);
            break;

        case GL_TEXTURE_3D:
            debugAssertM(false, "3D texture binding not implemented");
            break;

        case GL_FLOAT:
            glUniform1fvARB(u, 1, value.vector[0]);
            break;

        case GL_FLOAT_VEC2_ARB:
            glUniform2fvARB(u, 1, value.vector[0]);
            break;

        case GL_FLOAT_VEC3_ARB:
            glUniform3fvARB(u, 1, value.vector[0]);
            break;

        case GL_FLOAT_VEC4_ARB:
            glUniform4fvARB(u, 1, value.vector[0]);
            break;

        case GL_INT:
        case GL_BOOL_ARB:
            // OpenGL allows us to treat bools as ints, but not ints as floats
            glUniform1iARB(u, (int)value.vector[0][0]);
            break;

        case GL_INT_VEC2_ARB:
        case GL_BOOL_VEC2_ARB:
            glUniform2iARB(u, (int)value.vector[0].x, (int)value.vector[0].y);
            break;

        case GL_INT_VEC3_ARB:
        case GL_BOOL_VEC3_ARB:
            glUniform3iARB(u, (int)value.vector[0].x, (int)value.vector[0].y,
                (int)value.vector[0].z);
            break;

        case GL_INT_VEC4_ARB:
        case GL_BOOL_VEC4_ARB:
            glUniform4iARB(u, (int)value.vector[0].x, (int)value.vector[1].y,
                (int)value.vector[2].z, (int)value.vector[3].w);
            break;

        case GL_FLOAT_MAT2_ARB:
            debugAssertM(false, "GL_FLOAT_MAT2_ARB binding not implemented");
            break;

        case GL_FLOAT_MAT3_ARB:
            {
                float m[9];
                for (int i = 0, c = 0; c < 3; ++c) {
                    for (int r = 0; r < 3; ++r, ++i) {
                        m[i] = value.vector[r][c];
                    }
                }
                glUniformMatrix3fvARB(u, 1, GL_FALSE, m);
            }            
            break;

        case GL_FLOAT_MAT4_ARB:
            {
                float m[9];
                for (int i = 0, c = 0; c < 4; ++c) {
                    for (int r = 0; r < 4; ++r, ++i) {
                        m[i] = value.vector[r][c];
                    }
                }
                glUniformMatrix4fvARB(u, 1, GL_FALSE, m);
            }
            break;

        default:
            alwaysAssertM(false, format("Unsupported argument type: %s", GLenumToString(decl.type)));
        }

    }
}


////////////////////////////////////////////////////////////////////////

void VertexAndPixelShader::ArgList::set(const std::string& var, const TextureRef& val) {
    Arg arg;
    arg.type    = val->getOpenGLTextureTarget();
    arg.texture = val;
    argTable.set(var, arg);

}


void VertexAndPixelShader::ArgList::set(const std::string& var, const CoordinateFrame& val) {
    set(var, Matrix4(val));
}


void VertexAndPixelShader::ArgList::set(const std::string& var, const Matrix4& val) {
    Arg arg;
    arg.type = GL_FLOAT_MAT4_ARB;
    for (int r = 0; r < 4; ++r) {
        arg.vector[r] = val.getRow(r);
    }

    argTable.set(var, arg);
}


void VertexAndPixelShader::ArgList::set(const std::string& var, const Vector4& val) {
    Arg arg;
    arg.type = GL_FLOAT_VEC4_ARB;
    arg.vector[0] = val;
    argTable.set(var, arg);
}


void VertexAndPixelShader::ArgList::set(const std::string& var, const Vector3& val) {
    Arg arg;
    arg.type = GL_FLOAT_VEC3_ARB;
    arg.vector[0] = Vector4(val, 0);
    argTable.set(var, arg);

}


void VertexAndPixelShader::ArgList::set(const std::string& var, const Vector2& val) {
    Arg arg;
    arg.type = GL_FLOAT_VEC2_ARB;
    arg.vector[0] = Vector4(val, 0, 0);
    argTable.set(var, arg);
}


void VertexAndPixelShader::ArgList::set(const std::string& var, float          val) {
    Arg arg;
    arg.type = GL_FLOAT;
    arg.vector[0] = Vector4(val, 0, 0, 0);
    argTable.set(var, arg);
}


void VertexAndPixelShader::ArgList::clear() {
    argTable.clear();
}


}
