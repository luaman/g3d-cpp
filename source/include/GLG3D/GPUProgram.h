/**
  @file GPUProgram.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-13
  @edited  2003-09-09
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

    /** e.g. GL_VERTEX_PROGRAM_ARB, GL_FRAGMENT_PROGRAM_ARB */
    GLenum                      unit;

    std::string                 name;
    GLuint                      glProgram;

    std::string                 filename;

    /**
     Which extension set to use.
     */
    enum Extension {ARB, NVIDIA} extension;

    // Abstraction over NVIDIA/ARB extensions
    void genPrograms(int num, unsigned int* id) const;
    void bindProgram(int unit, unsigned int glProgram) const;
    void loadProgram(const std::string& code) const;
    void getProgramError(int& pos, const unsigned char*& msg) const;
    void deletePrograms(int num, unsigned int* id) const;
    
protected:

    /**
     Determines which unit and extension API a shader uses from the first line.
     */
    static GLenum getUnitFromCode(const std::string& code, Extension& extension);

    GPUProgram(const std::string& name, const std::string& filename);

public:

    ~GPUProgram();

    /** Reload from supplied code or from the original file that
        was specified (handy when debugging shaders) */
    void reload(const std::string& code = "");

    GLuint getOpenGLID() const;

    /**
     Binds this program.
     */
    void bind();

    /**
     Unbinds and disables.
     */
    void disable();
};

}

#endif
