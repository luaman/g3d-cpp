/**
 @file Shader.h
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2004-04-25
 @edited  2004-04-25
 */

#ifndef G3D_SHADER_H
#define G3D_SHADER_H

#include "graphics3D.h"

namespace G3D {

typedef ReferenceCountedPointer<class ShaderGroup>  ShaderGroupRef;
typedef ReferenceCountedPointer<class ObjectShader> ObjectShaderRef;
typedef ReferenceCountedPointer<class VertexShader> VertexShaderRef;
typedef ReferenceCountedPointer<class PixelShader>  PixelShaderRef;


class ObjectShader : public ReferenceCountedObject {
public:

};

class GPUShader : public ReferenceCountedObject {
public:

};

class VertexShader : public GPUShader {
public:
    static VertexShaderRef fromFile(const std::string& filename);
    static VertexShaderRef fromCode(const std::string& name, const std::string& code);
};


class PixelShader : public GPUShader {
public:
    static PixelShaderRef fromFile(const std::string& filename);
    static PixelShaderRef fromCode(const std::string& name, const std::string& code);
};



/**
  A set of compatible vertex, pixel, and object shaders; the analog of a DirectX "effect".

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
 */
class ShaderGroup : public ReferenceCountedObject {
public:

    /**
     Passing NULL for any parameter selects the default shader for that stage.
     The default object shader does nothing.  The default vertex shader is the
     OpenGL fixed-function vertex transform.  The default pixel shader is the
     OpenGL fixed-function fragment pipeline.

     The individual shaders are analogous to the object files produced by 
     a compiler.  Creating a ShaderGroup "links" them together.  This linking
     step often produces output from the linker.  It may fail due to an error, or
     succeed but produce warnings.  Both kinds of output are returned in 
     the output string (i.e. it contains the value returned by glGetInfoLogARB).  
     
     If an unrecoverable error occurs, NULL is returned. Use the 
     ReferenceCountedPoitner::isNull method to detect this.
     */
    static ShaderGroupRef create(
        const ObjectShaderRef& os,
        const VertexShaderRef& vs,
        const PixelShaderRef&  ps,
        std::string&           output = std::string(""));

    /**
     Returns GLCaps::supports_GL_ARB_shader_objects() && 
        GLCaps::supports_GL_ARB_shading_language_100() &&
        GLCaps::supports_GL_ARB_fragment_shader() &&
        GLCaps::supports_GL_ARB_vertex_shader()
    */
    static bool fullySupported();
};

}

#endif
