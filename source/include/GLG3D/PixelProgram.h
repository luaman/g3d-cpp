/**
  @file PixelProgram.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-13
  @edited  2003-04-13
*/

#ifndef GLG3D_PIXELPROGRAM_H
#define GLG3D_PIXELPROGRAM_H

#include "GLG3D/GPUProgram.h"

namespace G3D {

typedef ReferenceCountedPointer<class PixelProgram> PixelProgramRef;

/**
  Abstraction of OpenGL pixel programs.  This class can be used with raw OpenGL, 
  without RenderDevice or SDL.

  If you use PixelProgramRef instead of PixelProgram*, the texture memory will be
  garbage collected.

  The vertex program must be written in the ARB pixel program assembly language
  specified by:
  http://oss.sgi.com/projects/ogl-sample/registry/ARB/fragment_program.txt

  If an error is encountered inside a shader in debug mode, that error is printed to the
  debug window (under MSVC) and the programmer can fix the error and reload the
  shader without reloading the program.
 */
class PixelProgram : public GPUProgram {
private:

    PixelProgram(const std::string& _name, const std::string& filename);

public:

    static PixelProgram* fromFile(const std::string& name, const std::string& filename);

    static PixelProgram* fromCode(const std::string& name, const std::string& code);
};

}

#endif