/**
  @file GPUProgram.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-04-13
  @edited  2003-09-25
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

  Subclasses of GPUProgram (VertexProgram and PixelProgram) automatically recognize
  the output of NVIDIA's Cg compiler and will load constants from comments of the
  form:

  <CODE>    # const c[N] = V0 V1 V2 V3</CODE>

  You must track matrices and load variables yourself using the NVIDIA or ARB APIs. 
  For example:

  <PRE>
    glTrackMatrixNV(GL_VERTEX_PROGRAM_NV, 0, GL_MODELVIEW_PROJECTION_NV, GL_IDENTITY_NV);
    glTrackMatrixNV(GL_VERTEX_PROGRAM_NV, 4, GL_MODELVIEW, GL_INVERSE_NV);
    glProgramParameter4fvNV(GL_VERTEX_PROGRAM_NV, 8, Vector4(myVariable,0,0,0));
  </PRE>

 */
class GPUProgram : public ReferenceCountedObject {
private:

    /**
     Manages the constants in a vertex or fragment program produced by Cg, where
     the header has comments of the form:

      # const c[N] = V0 V1 V2 V3

      where N, V0, V1, V2, and V3 are numbers.
     */
    class BindingTable {
    private:
        enum Type {FLOAT4X4, FLOAT4};
        enum Source {VARIABLE, CONSTANT};

        /**
         A constant, variable, or matrix binding for a vertex program.
         */
        class VPBinding {
        public:
            /**
             Variable/constant name
             */
            std::string         name;

            /**
             Which const register receives this value.
             */
            int                 slot;

            Type                type;

            Source              source;

            Vector4             vector;
        };

        Array<VPBinding>    bindingArray;

        static bool symbolMatch(const Token& t, const std::string& s);

        /**
         Returns true if it is able to consume the next symbol, which must match s.
         */
        static bool consumeSymbol(TextInput& ti, const std::string& s);

    public:

        /**
         Initializes from a Cg generated assembly program.
         */
        void parse(const std::string& code);

        /**
         Binds constants using the NVIDIA API.
         */
        void nvBind(GLenum target) const;

        /**
         Binds constants using the ARB API.
         */
        void arbBind(GLenum target) const;

    };

    BindingTable                bindingTable;

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
