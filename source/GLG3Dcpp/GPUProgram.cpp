/**
  @file GPUProgram.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-13
  @edited  2003-09-09
*/

#include "GLG3D/GPUProgram.h"
#include "GLG3D/getOpenGLState.h"
#include "G3D/debugAssert.h"

namespace G3D {

GPUProgram::GPUProgram(
    const std::string&  _name,
    const std::string&  _filename) : name(_name), filename(_filename) {
}


GLenum GPUProgram::getUnitFromCode(const std::string& code) {

    if (beginsWith(code, "!!ARBvp1.0")) {
        return GL_VERTEX_PROGRAM_ARB;

    } if (beginsWith(code, "!!ARBfp1.0")) {
        return GL_FRAGMENT_PROGRAM_ARB;

    } if (beginsWith(code, "!!VP2.0")) {
        return GL_VERTEX_PROGRAM_NV;

    } else {
        return GL_NONE;
    }
}


void GPUProgram::genPrograms(int num, unsigned int* id) const {

    switch (extension) {
    case NVIDIA:
        alwaysAssertM(glGenProgramsNV, "Requires an NVIDIA card with the GL_NV_vertex_program extension");
        glGenProgramsNV(1, id);
        break;

    case ARB:
        glGenProgramsARB(1, id);
        break;
    }
}


void GPUProgram::deletePrograms(int num, unsigned int* id) const {

    switch (extension) {
    case NVIDIA:
        glDeleteProgramsNV(1, id);
        break;

    case ARB:
        glDeleteProgramsARB(1, id);
        break;
    }
}


void GPUProgram::bindProgram(int unit, unsigned int glProgram) const {
    switch (extension) {
    case NVIDIA:
        glBindProgramNV(unit, glProgram);
        break;

    case ARB:
        glBindProgramARB(unit, glProgram);
        break;
    }
}


void GPUProgram::loadProgram(const std::string& code) const {
    switch (extension) {
    case NVIDIA:
        glLoadProgramNV(unit, glProgram, code.size(), (const unsigned char*)code.c_str());
        break;

    case ARB:
        glProgramStringARB(unit, GL_PROGRAM_FORMAT_ASCII_ARB, code.size(), code.c_str());
        break;
    }
}


void GPUProgram::getProgramError(int& pos, const unsigned char*& msg) const {

    switch (extension) {
    case NVIDIA:
        pos = glGetInteger(GL_PROGRAM_ERROR_POSITION_NV);
        msg = glGetString(GL_PROGRAM_ERROR_STRING_NV);
        break;

    case ARB:
        pos = glGetInteger(GL_PROGRAM_ERROR_POSITION_ARB);
        msg = glGetString(GL_PROGRAM_ERROR_STRING_ARB);
        break;
    }
}

void GPUProgram::reload(const std::string& _code) {
    std::string code = _code;

    // If a syntax error occurs while loading the shader we want to break.  
    // However, it makes no sense to break in this loading code when the
    // error is really in the shader code.  To hack this under MSVC we print
    // out the error as if it were a MSVC error so double clicking will take
    // us there, then break in this code.  To reload the shader we jump back
    // to the top of the loading routine and try again.
     
    bool reloadFromFile = (code == "");

    bool ignore = false;

LOADSHADER:

    if (reloadFromFile) {
 
        if (fileExists(filename)) {
            code = readFileAsString(filename);
        } else {
            error("Critical Error", 
                std::string("Cannot locate file \"") + filename + "\" to reload it.", true);
            exit(-1);
        }
    }

    unit = getUnitFromCode(code);

    switch (unit) {
    case GL_VERTEX_PROGRAM_NV:
        extension = NVIDIA;
        break;
       
    default:
        extension = ARB;
        break;
    }

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glEnable(unit);

    genPrograms(1, &glProgram);
    bindProgram(unit, glProgram);
    // Clear the error flag.
    glGetError();

    loadProgram(code);

    // Check for load errors
    if ((glGetError() == GL_INVALID_OPERATION) && (! ignore)) {

        int                  pos;
        const unsigned char* msg;
        getProgramError(pos, msg);

        deletePrograms(1, &glProgram);

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
        while ((x < (int)code.size()) && (code[x] != '\n') && (code[x] != '\r')) {
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
            debugPrintf("%s%s(%d) : GPU Program Error : %s%s%s",
                   NEWLINE, fullFilename.c_str(), line, msg, NEWLINE, NEWLINE);
        }
        #endif

        #ifndef _DEBUG
            Log::common()->print("\n******************************\n");
            Log::common()->print(text);
            exit(-1);
        #endif

        const char* choice[] = {"Debug", "Ignore", "Ignore All", "Exit"};

        switch (prompt("Error Loading Program", text.c_str(), choice, 4, true)) {
        case 0:
            // Debug
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
                reloadFromFile = true;
                goto LOADSHADER;
                break;
            }

        case 1:
            // Ignore
            break;

        case 2:
            // Ignore all
            ignore = true;
            break;

        case 3:
            // Exit
            exit(-1);
        }
    }

    glPopAttrib();
}


GPUProgram::~GPUProgram() {
    deletePrograms(1, &glProgram);
    glProgram = 0;
}


GLuint GPUProgram::getOpenGLID() const {
    return glProgram;
}


void GPUProgram::bind() {
    glEnable(unit);
    bindProgram(unit, glProgram);
}


void GPUProgram::disable() {
    glDisable(unit);
}


}

