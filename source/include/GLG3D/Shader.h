/**
 @file Shader.h
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2004-04-25
 @edited  2004-04-27
 */

#ifndef G3D_SHADER_H
#define G3D_SHADER_H

#include "graphics3D.h"
#include "glheaders.h"
#include "GLG3D/Texture.h"

namespace G3D {

typedef ReferenceCountedPointer<class ShaderGroup>  ShaderGroupRef;
typedef ReferenceCountedPointer<class GPUShader>    GPUShaderRef;
typedef ReferenceCountedPointer<class ObjectShader> ObjectShaderRef;
typedef ReferenceCountedPointer<class VertexShader> VertexShaderRef;
typedef ReferenceCountedPointer<class PixelShader>  PixelShaderRef;


class ObjectShader : public ReferenceCountedObject {
private:
    std::string     _messages;
public:

    bool ok() const {
        return true;
    }

    const std::string& messages() const {
        return _messages;
    }
};

class GPUShader : public ReferenceCountedObject {
protected:

    /** argument for output on subclasses */
    static std::string          ignore;

    std::string                 _name;
    std::string                 _code;
    bool                        fromFile;

    GLhandleARB                 _glShaderObject;

    GPUShader(const std::string& name, const std::string& code, bool fromFile);

    bool                        _ok;
    std::string                 _messages;

    /** Returns true on success.  Called from init. */
    void compile();

    /** Initialize a shader object and returns object.  
        Called from subclass create methods. */
    static GPUShader*           init(GPUShader* shader);

public:
    /** Deletes the underlying glShaderObject.  Between GL's reference
        counting and G3D's reference counting, an underlying object
        can never be deleted while in use. */
    ~GPUShader();

    /** Shader type, e.g. GL_VERTEX_SHADER_ARB */
    virtual GLenum glShaderType() const = 0;

    /** Why compilation failed, or any compiler warnings if it succeeded.*/
    inline const std::string& messages() const {
        return _messages;
    }

    /** Returns true if compilation and loading succeeded.  If they failed,
        check the message string.*/
    inline bool ok() const {
        return _ok;
    }

    /** Returns the underlying OpenGL shader object for this shader */
    inline GLhandleARB glShaderObject() const {
        return _glShaderObject;
    }
};


class VertexShader : public GPUShader {
private:

    VertexShader(const std::string& name, const std::string& code, bool fromFile) :
       GPUShader(name, code, fromFile) {}

public:
    virtual GLenum glShaderType() const {
        return GL_VERTEX_SHADER_ARB;
    }

    /** In the event of a fatal error, the returned VertexShader will not be GPUShader::ok(). */
    static VertexShaderRef fromFile(const std::string& filename);

    /** @param name For debugging purposes.  Typically, the name of the file from which
             the shader was loaded. */
    static VertexShaderRef fromCode(const std::string& name, const std::string& code);
};


class PixelShader : public GPUShader {
private:

    PixelShader(const std::string& name, const std::string& code, bool fromFile) :
       GPUShader(name, code, fromFile) {}

public:
    virtual GLenum glShaderType() const {
        return GL_FRAGMENT_SHADER_ARB;
    }

    /** In the event of a fatal error, the returned PixelShader will not be GPUShader::ok(). */
    static PixelShaderRef fromFile(const std::string& filename);

    /** @param name For debugging purposes.  Typically, the name of the file from which
             the shader was loaded. */
    static PixelShaderRef fromCode(const std::string& name, const std::string& code);
};


/**
  A set of compatible vertex, pixel, and object shaders; the analog of a DirectX "effect pass".

  Only newer graphics cards with recent drivers (e.g. GeForceFX cards with driver version 57 or greater)
  support this API.  Use the ShaderGroup::fullySupported method to determine at run-time
  if your graphics card is compatible.

  A ShaderGroup contains three shaders:
  <OL>
     <LI>Object shader: executes once per primitive group
     <LI>Vertex shader: executes once per vertex
     <LI>Pixel shader: executes once per pixel 
  </OL>

  For purposes of shading, a "pixel" is technically a "fragment" in OpenGL terminology.
  A primitive group is defined by either the pair of calls RenderDevice::beginPrimitive...RenderDevice::endPrimitive
  or the single call RenderDevice::sendIndices.

  Pixel and vertex shaders are loaded as text strings written in 
  <A HREF="http://www.3dlabs.com/support/developer/ogl2/downloads/ShaderSpecV1.051.pdf">GLSL</A>, the high-level
  OpenGL shading language.  Object shaders are written in C++ by subclassing ObjectShader.

  Typically, the object shader sets up constants like the object-space position
  of the light source and the object-to-world matrix.  The vertex shader transforms
  input vertices to homogeneous clip space and computes values that are interpolated
  across the surface of a triangle (e.g. reflection vector).  The pixel shader
  computes the final color of a pixel (it does not perform alpha-blending, however).

  Multiple ShaderGroups may share object, vertex, and pixel shaders.

  @cite http://oss.sgi.com/projects/ogl-sample/registry/ARB/shader_objects.txt
  @cite http://oss.sgi.com/projects/ogl-sample/registry/ARB/vertex_shader.txt
 */
class ShaderGroup : public ReferenceCountedObject {
protected:

    static std::string      ignore;

    ObjectShaderRef         _objectShader;
    VertexShaderRef         _vertexShader;
    PixelShaderRef          _pixelShader;

    GLhandleARB             _glProgramObject;

    bool                    _ok;
    std::string             _messages;

    ShaderGroup(
        const ObjectShaderRef& os,
        const VertexShaderRef& vs,
        const PixelShaderRef&  ps);

public:

    /**
     Argument list for a ShaderGroup.
     */
    class ArgList {
    private:
        friend class ShaderGroup;

        class Arg {
        public:

            /** Row-major */ 
            Vector4                    vector[4];

			TextureRef				   texture;

            GLenum                     type;
        };

        Table<std::string, Arg>        argTable;

    public:

		void set(const std::string& var, const TextureRef& val);
        void set(const std::string& var, const CoordinateFrame& val);
        void set(const std::string& var, const Matrix4& val);
        void set(const std::string& var, const Vector4& val);
        void set(const std::string& var, const Vector3& val);
        void set(const std::string& var, const Vector2& val);
        void set(const std::string& var, float          val);
        void clear();

    };


    ~ShaderGroup();

    /**
     Passing NULL for any parameter selects the default shader for that stage.
     The default object shader does nothing.  The default vertex shader is the
     OpenGL fixed-function vertex transform.  The default pixel shader is the
     OpenGL fixed-function fragment pipeline.

     The individual shaders are analogous to the object files produced by 
     a compiler.  Creating a ShaderGroup "links" them together.  This linking
     step often produces output from the linker.  It may fail due to an error, or
     succeed but produce warnings.  Both kinds of output are stored in
     messages() (i.e. it contains the value returned by glGetInfoLogARB).  
     
     If an unrecoverable error occurs, ok() is false.
     */
    static ShaderGroupRef create(
        const ObjectShaderRef& os,
        const VertexShaderRef& vs,
        const PixelShaderRef&  ps);

    /**
     Returns GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_ARB_fragment_shader() &&
        GLCaps::supports_GL_ARB_vertex_shader()
    */
    static bool fullySupported();

    inline bool ok() const {
        return _ok;
    }

    inline const std::string& messages() const {
        return _messages;
    }

    /** The underlying OpenGL object for the vertex/pixel shader pair */
    GLhandleARB glProgramObject() const {
        return _glProgramObject;
    }

    inline ObjectShaderRef objectShader() const {
        return _objectShader;
    }

    inline VertexShaderRef vertexShader() const {
        return _vertexShader;
    }

    inline PixelShaderRef pixelShader() const {
        return _pixelShader;
    }
};

}

#endif
