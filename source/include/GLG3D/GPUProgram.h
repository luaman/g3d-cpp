/**
  @file GPUProgram.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-13
  @edited  2003-04-13
*/

#ifndef GLG3D_GPUPROGRAM_H
#define GLG3D_GPUPROGRAM_H

#include "graphics3D.h"
#include "GLG3D/glheaders.h"

namespace G3D {

typedef ReferenceCountedPointer<class GPUProgram> GPUProgramRef;

/**
  Base class for VertexProgram and PixelProgram

  If you use GPUProgramRef instead of GPUProgram*, the texture memory will be
  garbage collected.

  If an error is encountered inside a shader in debug mode, that error is printed to the
  debug window (under MSVC) and the programmer can fix the error and reload the
  shader without reloading the program.
 */
class GPUProgram : public ReferenceCountedObject {
private:

    /** GL_VERTEX_PROGRAM_ARB or GL_FRAGMENT_PROGRAM_ARB */
    GLenum                      unit;

    std::string                 name;
    GLuint                      glProgram;

    std::string                 filename;

protected:

    GPUProgram(const std::string& name, const std::string& filename, GLenum unit);

public:

    ~GPUProgram();

    /** Reload from supplied code or from the original file that
        was specified (handy when debugging shaders) */
    void reload(const std::string& code = "");

    GLuint getOpenGLID() const;
};

}

#endif
