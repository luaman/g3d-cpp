/**
  @file VertexProgram.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2001-04-10
  @edited  2003-04-10
*/

#include "GLG3D/VertexProgram.h"

namespace G3D {


VertexProgram::VertexProgram(const std::string& _name, const std::string& asmCode) : name(_name) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_VERTEX_PROGRAM_ARB);
    glGenProgramsARB(1, &glProgram);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, glProgram);
    glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, asmCode.size(), asmCode.c_str());
    glPopAttrib();
}


VertexProgram* VertexProgram::fromFile(const std::string& name, const std::string& filename) {
    return new VertexProgram(name, readFileAsString(filename));
}


VertexProgram* VertexProgram::fromCode(const std::string& name, const std::string& code) {
    return new VertexProgram(name, code);
}


VertexProgram::~VertexProgram() {
    glDeleteProgramsARB(1, &glProgram);
    glProgram = 0;
}


GLuint VertexProgram::getOpenGLID() const {
    return glProgram;
}

}

