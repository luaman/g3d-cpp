/**
  @file VertexProgram.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-10
  @edited  2003-04-10
*/

#ifndef GLG3D_VERTEXPROGRAM_H
#define GLG3D_VERTEXPROGRAM_H

#include "GLG3D/GPUProgram.h"

namespace G3D {

typedef ReferenceCountedPointer<class VertexProgram> VertexProgramRef;

/**
  Abstraction of OpenGL vertex programs.  This class can be used with raw OpenGL, 
  without RenderDevice or SDL.

  If you use VertexProgramRef instead of VertexProgram*, the texture memory will be
  garbage collected.

  The vertex program must be written in the ARB vertex program assembly language
  specified by:
  http://oss.sgi.com/projects/ogl-sample/registry/ARB/vertex_program.txt

  If an error is encountered inside a shader in debug mode, that error is printed to the
  debug window (under MSVC) and the programmer can fix the error and reload the
  shader without reloading the program.
 */
class VertexProgram : public GPUProgram {
private:

    VertexProgram(const std::string& _name, const std::string& filename);

public:

    static VertexProgram* fromFile(const std::string& name, const std::string& filename);

    static VertexProgram* fromCode(const std::string& name, const std::string& code);

};

}

#endif
