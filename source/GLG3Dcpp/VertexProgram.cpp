/**
  @file VertexProgram.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-10
  @edited  2003-04-13
*/

#include "GLG3D/VertexProgram.h"

namespace G3D {

VertexProgram::VertexProgram(const std::string& name, const std::string& filename) : 
    GPUProgram(name, filename, GL_VERTEX_PROGRAM_ARB) {
}

VertexProgram* VertexProgram::fromFile(const std::string& name, const std::string& _filename) {
    VertexProgram* v = new VertexProgram(name, _filename);
    v->reload(std::string(""));
    return v;
}


VertexProgram* VertexProgram::fromCode(const std::string& name, const std::string& code) {
    VertexProgram* v = new VertexProgram(name, std::string(""));
    v->reload(code);
    return v;
}

}

