/**
  @file PixelProgram.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-13
  @edited  2003-04-13
*/

#include "GLG3D/PixelProgram.h"


namespace G3D {


PixelProgram::PixelProgram(const std::string& name, const std::string& filename) : 
    GPUProgram(name, filename, GL_FRAGMENT_PROGRAM_ARB) {
}


PixelProgram* PixelProgram::fromFile(const std::string& name, const std::string& _filename) {
    PixelProgram* v = new PixelProgram(name, _filename);
    v->reload(std::string(""));
    return v;
}


PixelProgram* PixelProgram::fromCode(const std::string& name, const std::string& code) {
    PixelProgram* v = new PixelProgram(name, std::string(""));
    v->reload(code);
    return v;
}

}

