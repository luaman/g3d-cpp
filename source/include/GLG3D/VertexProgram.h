/**
  @file VertexProgram.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2001-04-10
  @edited  2003-04-10
*/

#ifndef GLG3D_VERTEXPROGRAM_H
#define GLG3D_VERTEXPROGRAM_H

#include "graphics3D.h"
#include "GLG3D/glheaders.h"

namespace G3D {

typedef ReferenceCountedPointer<class VertexProgram> VertexProgramRef;

/**
  Abstraction of OpenGL vertex programs.  This class can be used with raw OpenGL, 
  without RenderDevice or SDL.

  If you use VertexProgramRef instead of VertexProgram*, the texture memory will be
  garbage collected.
 */
class VertexProgram : public ReferenceCountedObject {
private:

    std::string                 name;
    GLuint                      glProgram;

    VertexProgram(const std::string& _name, const std::string& asmCode);

public:

    static VertexProgram* fromFile(const std::string& name, const std::string& filename);

    static VertexProgram* fromCode(const std::string& name, const std::string& code);

    ~VertexProgram();

    GLuint getOpenGLID() const;
};

}

#endif