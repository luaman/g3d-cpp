/**
 @file Shader.h
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2004-04-25
 @edited  2004-04-30
 */

#ifndef G3D_SHADER_H
#define G3D_SHADER_H

#include "graphics3D.h"
#include "glheaders.h"
#include "GLG3D/Texture.h"

namespace G3D {

typedef ReferenceCountedPointer<class VertexAndPixelShader>  VertexAndPixelShaderRef;
typedef ReferenceCountedPointer<class ObjectShader> ObjectShaderRef;

#ifdef _DEBUG
    #define DEBUG_SHADER true
#else
    #define DEBUG_SHADER false
#endif

/**
  An ObjectShader is run once per primitive group.
  A primitive group is defined by either the pair of calls 
  RenderDevice::beginPrimitive...RenderDevice::endPrimitive
  or the single call RenderDevice::sendIndices.

  To create an ObjectShader, subclass it and 
  override the ObjectShader::run method to set up the
  RenderDevice state as you wish.  Commonly, the ObjectShader
  is used to compute certain world-space constants (e.g. lighting),
  set the VertexAndPixelShader and 
  
  <B>Do not</B> make a public constructor for your subclass.
  Instead, write static "create" method that calls a private constructor
  and then returns an ObjectShaderRef (or your own subclass).

  Example:
  <PRE>
	typedef ReferenceCountedPointer<class BumpShader> BumpShaderRef;

	class BumpShader : ObjectShader {
	private:
		VertexAndPixelShaderRef			vps;
		VertexAndPixelShader::ArgList	args;
		Vector3							lightVector;

		BumpShader() {
			vps = VertexAndPixelShader::fromFiles("bump_vertex.glsl", "bump_pixel.glsl");
		}

	public:

		void setBumpMap(TextureRef b) {
			args.set("bumpMap", b);
		}

	    void setLight(const Vector3& L) {
			lightVector = L;
		}

		static BumpShaderRef create() {
			return new BumpShader();
		}

	    void run(RenderDevice* rd) {
			args.set("osLightDirection", 
				rd->objectToWorldMatrix().vectorToObjectSpace(lightVector));

			rd->setVertexAndPixelShader(vps, args);
		}
	};

  </PRE>


 */
class ObjectShader : public ReferenceCountedObject {
private:
    std::string     _messages;

protected:

	inline ObjectShader() {}

public:

    bool ok() const {
        return true;
    }

	/**
	 Invoked by RenderDevice immediately before a primitive group.
	 Use this to set state on the render device.  Do not call
	 pushState from inside this method.
	 */
	virtual void run(class RenderDevice* renderDevice) = NULL;

    const std::string& messages() const {
        return _messages;
    }
};


/**
  A compatible vertex and pixel shader.

  Only newer graphics cards with recent drivers (e.g. GeForceFX cards with driver version 57 or greater)
  support this API.  Use the VertexAndPixelShader::fullySupported method to determine at run-time
  if your graphics card is compatible.

  For purposes of shading, a "pixel" is technically a "fragment" in OpenGL terminology.

  Pixel and vertex shaders are loaded as text strings written in 
  <A HREF="http://www.3dlabs.com/support/developer/ogl2/downloads/ShaderSpecV1.051.pdf">GLSL</A>, the high-level
  OpenGL shading language.  Object shaders are written in C++ by subclassing ObjectShader.

  Typically, the object shader sets up constants like the object-space position
  of the light source and the object-to-world matrix.  The vertex shader transforms
  input vertices to homogeneous clip space and computes values that are interpolated
  across the surface of a triangle (e.g. reflection vector).  The pixel shader
  computes the final color of a pixel (it does not perform alpha-blending, however).

  Multiple VertexAndPixelShaders may share object, vertex, and pixel shaders.

  @cite http://oss.sgi.com/projects/ogl-sample/registry/ARB/shader_objects.txt
  @cite http://oss.sgi.com/projects/ogl-sample/registry/ARB/vertex_shader.txt

  <B>BETA API</B>
  This API is subject to change.
 */
class VertexAndPixelShader : public ReferenceCountedObject {
public:

    class UniformDeclaration {
    public:
        /** Name of the variable.  May include [] and . (e.g.
            "foo[1].normal")*/
        std::string         name;

        /** OpenGL type of the variable (e.g. GL_INT) */
        GLenum              type;

        /** Unknown... appears to always be 1 */
        int                 size;

        /**
         Index of the texture unit in which this value
         is stored.  -1 for uniforms that are not textures. */  
        int                 textureUnit;
    };

protected:

	class GPUShader {
	protected:

		/** argument for output on subclasses */
		static std::string          ignore;

		std::string                 _name;
		std::string                 _code;
		bool                        fromFile;

		GLhandleARB                 _glShaderObject;

		bool                        _ok;
		std::string                 _messages;

		/** Returns true on success.  Called from init. */
		void compile();

		/** Initialize a shader object and returns object.  
			Called from subclass create methods. */
		static GPUShader*           init(GPUShader* shader, bool debug);

		/** Set to true when name and code both == "" */
		bool						_fixedFunction;

		GLenum						_glShaderType;

		std::string					_shaderType;

	public:
		void init(
			const std::string& name,
			const std::string& code,
			bool			   fromFile,
			bool			   debug,
			GLenum		       glType,
			const std::string& type);

		/** Deletes the underlying glShaderObject.  Between GL's reference
			counting and G3D's reference counting, an underlying object
			can never be deleted while in use. */
		~GPUShader();

		/** Shader type, e.g. GL_VERTEX_SHADER_ARB */
		inline GLenum glShaderType() const {
			return _glShaderType;
		}

		inline const std::string& shaderType() const {
			return _shaderType;
		}

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

		inline bool fixedFunction() const {
			return _fixedFunction;
		}
	};


    static std::string      ignore;

    GPUShader				vertexShader;
    GPUShader				pixelShader;

    GLhandleARB             _glProgramObject;

    bool                    _ok;
    std::string             _messages;


    /** Converts from int and bool types to float types (e.g. GL_INT_VEC2_ARB -> GL_FLOAT_VEC2_ARB).
        Other types are left unmodified.*/
    static GLenum canonicalType(GLenum e);

    /** Computes the uniformArray from the current
        program object.  Called from the constructor */
    void computeUniformArray();

    Array<UniformDeclaration>   uniformArray;

    /** Returns true for types that are textures (e.g. GL_TEXTURE_2D) */
    static bool isSamplerType(GLenum e);

	VertexAndPixelShader(
		const std::string& vsCode,
		const std::string& vsFilename,
		bool vsFromFile,
		const std::string& psCode,
		const std::string& psFilename,
		bool psFromFile);

public:

    /** Thrown by validateArgList */
    class ArgumentError {
    public:
        std::string             message;

        ArgumentError(const std::string& m) : message(m) {}
    };

	/**
	 To use the fixed function pipeline for part of the
	 shader, pass an empty string.
	 */
	static VertexAndPixelShaderRef fromStrings(
		const std::string& vertexShader,
		const std::string& pixelShader);

	/**
	 To use the fixed function pipeline for part of the
	 shader, pass an empty string.
	 */
	static VertexAndPixelShaderRef fromFiles(
		const std::string& vertexShader,
		const std::string& pixelShader);

    /**
     Bindings of values to uniform variables for a VertexAndPixelShader.
     Be aware that 
     the uniform namespace is global across the pixel and vertex shader.

     GL_BOOL_ARB and GL_INT_ARB-based values are coerced to floats
     automatically by the arg list.
     */
    class ArgList {
    private:
        friend class VertexAndPixelShader;

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

    ~VertexAndPixelShader();

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

    /** The underlying OpenGL object for the vertex/pixel shader pair.

        To bind a shader with RenderDevice, call renderDevice->setShader(s);
        To bind a shader <B>without</B> RenderDevice, call
        glUseProgramObjectARB(s->glProgramObject());

    */
    GLhandleARB glProgramObject() const {
        return _glProgramObject;
    }

    int numArgs() const {
        return uniformArray.size();
    }

    /** Checks the actual values of uniform variables against those 
        expected by the program.
        If one of the arguments does not match, an ArgumentError
        exception is thrown.
    
        Called by 
    */
    void validateArgList(const ArgList& args) const;

    /**
       Makes renderDevice calls to bind this argument list.
       Calls validateArgList.
     */
    void bindArgList(class RenderDevice* rd, const ArgList& args) const;

    /** Returns information about one of the arguments expected
        by this VertexAndPixelShader.  There are VertexAndPixelShader::numArgs()
        total.*/
    const UniformDeclaration& arg(int i) const {
        return uniformArray[i];
    }
};

}

#endif
