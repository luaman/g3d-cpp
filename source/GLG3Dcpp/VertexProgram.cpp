/**
  @file VertexProgram.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2001-04-10
  @edited  2003-04-10
*/

#include "GLG3D/VertexProgram.h"
#include "GLG3D/getOpenGLState.h"
#include "G3D/debugAssert.h"

namespace G3D {


VertexProgram::VertexProgram(const std::string& _name, const std::string& _filename, const std::string& _code) : name(_name), filename(_filename) {
    std::string code = _code;

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(GL_VERTEX_PROGRAM_ARB);
    glGenProgramsARB(1, &glProgram);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, glProgram);

    // If a syntax error occurs while loading the shader we want to break.  
    // However, it makes no sense to break in this loading code when the
    // error is really in the shader code.  To hack this under MSVC we print
    // out the error as if it were a MSVC error so double clicking will take
    // us there, then break in this code.  To reload the shader we jump back
    // to the top of the loading routine and try again.
     
    bool reload = false;

LOADSHADER:

    if (reload) {
 
        if (fileExists(filename)) {
            code = readFileAsString(filename);
        } else {
            error("Critical Error", 
                std::string("Cannot locate file \"") + filename + "\" to reload it.", true);
            exit(-1);
        }
    }

    // Clear the error flag.
    glGetError();
    glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, code.size(), code.c_str());

    // Check for load errors
    if (glGetError() == GL_INVALID_OPERATION) {
        int                  pos = glGetInteger(GL_PROGRAM_ERROR_POSITION_ARB);
        const unsigned char* msg = glGetString(GL_PROGRAM_ERROR_STRING_ARB);

        int line = 1;
        int col  = 1;

        // Find the line and column position.
        int x = 0;
        for (x = 0, col = 1; x < pos; ++x, ++col) {
            if (code[x] == '\n') {
                ++line;
                col = 1;
            }
        }

        if (col > 1) {
            --col;
        }

        // Count forward to the end of the line
        int endCol = col;
        while ((x < code.size()) && (code[x] != '\n') && (code[x] != '\r')) {
            ++x;
            ++endCol;
        }

        // Extract the line
        std::string codeLine = code.substr(pos - col + 1, endCol - col);

        // Show the line
        std::string text = format("%s (%d:%d) : %s%s%s", filename.c_str(), line, col, msg, NEWLINE, NEWLINE);
        text += codeLine + NEWLINE;
        for (int i = 0; i < col - 1; ++i) {
            text += " ";
        }

        text += "^";

        #ifdef _WIN32
        {
            // Print the error message in MSVC format
            std::string fullFilename = resolveFilename(filename);
            debugPrintf("%s%s(%d) : Vertex Program Error : %s%s%s",
                   NEWLINE, fullFilename.c_str(), line, msg, NEWLINE, NEWLINE);
        }
        #endif

        const char* choice[] = {"Debug", "Ignore", "Ignore All", "Exit"};

        switch (prompt("Error Loading Program", text.c_str(), choice, 4, true)) {
        case 0:
            {
                ////////////////////////////////////////////////////////////////////////////
                //                                                                        //
                //                              PUSH  F4                                  //
                //                                                                        // 
                //   If your program breaks on this line in debug mode under Windows,     //
                //   go to the MSVC Debug window and click on the error message (or       //
                //   just press F4 be taken to the error line in your shader.             //
                //                                                                        //
                //   When you change it and press continue, G3D will try to reload your   //
                //   shader (if it came from a file).                                     //
                //                                                                        //
                ////////////////////////////////////////////////////////////////////////////
                debugBreak();
                reload = true;
                goto LOADSHADER;
                break;
            }
        case 1:
        case 2:
        case 3:
            exit(-1);
        }
    }

    glPopAttrib();
}


VertexProgram* VertexProgram::fromFile(const std::string& name, const std::string& _filename) {
    return new VertexProgram(name, _filename, readFileAsString(_filename));
}


VertexProgram* VertexProgram::fromCode(const std::string& name, const std::string& code) {
    return new VertexProgram(name, name, code);
}


VertexProgram::~VertexProgram() {
    glDeleteProgramsARB(1, &glProgram);
    glProgram = 0;
}


GLuint VertexProgram::getOpenGLID() const {
    return glProgram;
}

}

