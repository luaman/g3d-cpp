/**
  @file GLCaps.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2004-03-28
  @edited  2004-05-02
*/

#include "GLG3D/GLCaps.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/TextureFormat.h"
#include <sstream>

namespace G3D {

bool GLCaps::loadedExtensions = false;

/**
 Dummy function to which unloaded extensions can be set.
 */
static void __stdcall glIgnore(GLenum e) {}

/** Cache of values supplied to supportsTextureFormat.
    Works on pointers since there is no way for users
    to construct their own TextureFormats.
 */
static Table<const TextureFormat*, bool>      _supportedTextureFormat;


Set<std::string> GLCaps::extensionSet;

// We're going to need exactly the same code for each of 
// several extensions.
#define DECLARE_EXT(extname)    bool GLCaps::_supports_##extname = false; 
    DECLARE_EXT(GL_EXT_texture_rectangle);
    DECLARE_EXT(GL_ARB_vertex_program);
    DECLARE_EXT(GL_NV_vertex_program2);
    DECLARE_EXT(GL_ARB_vertex_buffer_object);
    DECLARE_EXT(GL_ARB_fragment_program);
    DECLARE_EXT(GL_ARB_multitexture);
    DECLARE_EXT(GL_EXT_stencil_wrap);
    DECLARE_EXT(GL_ARB_stencil_two_side);
    DECLARE_EXT(GL_EXT_texture_compression_s3tc);
    DECLARE_EXT(GL_EXT_texture_cube_map);
    DECLARE_EXT(GL_ARB_shadow);
    DECLARE_EXT(GL_ARB_shader_objects);
    DECLARE_EXT(GL_ARB_shading_language_100);
    DECLARE_EXT(GL_ARB_fragment_shader);
    DECLARE_EXT(GL_ARB_vertex_shader);
#undef DECLARE_EXT


void GLCaps::loadExtensions() {

    if (loadedExtensions) {
        return;
    }

    loadedExtensions = true;

    Log* debugLog = Log::common();

    #define LOAD_EXTENSION(name) \
       if (debugLog) {debugLog->print("Loading " #name " extension");} \
        *((void**)&name) = glGetProcAddress(#name); \
       if (debugLog) {debugLog->printf("(0x%x)\n", #name);}

    // Don't load the multitexture extensions when they are
    // statically linked
    #ifndef G3D_GL_ARB_multitexture_static
        LOAD_EXTENSION(glMultiTexCoord2fARB);
        LOAD_EXTENSION(glMultiTexCoord1fARB);
        LOAD_EXTENSION(glMultiTexCoord2fvARB);
        LOAD_EXTENSION(glMultiTexCoord3fvARB);
        LOAD_EXTENSION(glMultiTexCoord4fvARB);
        LOAD_EXTENSION(glMultiTexCoord1dARB);
        LOAD_EXTENSION(glMultiTexCoord2dvARB);
        LOAD_EXTENSION(glMultiTexCoord3dvARB);
        LOAD_EXTENSION(glMultiTexCoord4dvARB);
        LOAD_EXTENSION(glActiveTextureARB);
        LOAD_EXTENSION(glClientActiveTextureARB);
    #endif

    #ifdef G3D_WIN32
        LOAD_EXTENSION(wglSwapIntervalEXT);
        LOAD_EXTENSION(wglChoosePixelFormatARB);
        LOAD_EXTENSION(wglAllocateMemoryNV);
        LOAD_EXTENSION(wglFreeMemoryNV);
    #endif
    LOAD_EXTENSION(glVertexArrayRangeNV);
    LOAD_EXTENSION(glCompressedTexImage2DARB);
    LOAD_EXTENSION(glGetCompressedTexImageARB);
    LOAD_EXTENSION(glGenFencesNV);
    LOAD_EXTENSION(glDeleteFencesNV);
    LOAD_EXTENSION(glSetFenceNV);
    LOAD_EXTENSION(glFlushVertexArrayRangeNV);
    LOAD_EXTENSION(glFinishFenceNV);
    LOAD_EXTENSION(glGenProgramsARB);
    LOAD_EXTENSION(glBindProgramARB);
    LOAD_EXTENSION(glDeleteProgramsARB);
    LOAD_EXTENSION(glProgramStringARB);
    LOAD_EXTENSION(glProgramEnvParameter4fARB);
    LOAD_EXTENSION(glProgramLocalParameter4fARB);
    LOAD_EXTENSION(glProgramLocalParameter4fvARB);
    LOAD_EXTENSION(glProgramEnvParameter4dvARB);
    LOAD_EXTENSION(glProgramLocalParameter4dvARB);
    LOAD_EXTENSION(glVertexAttribPointerARB);
    LOAD_EXTENSION(glEnableVertexAttribArrayARB);
    LOAD_EXTENSION(glDisableVertexAttribArrayARB);
    LOAD_EXTENSION(glPointParameterfARB);
    LOAD_EXTENSION(glPointParameterfvARB);
    LOAD_EXTENSION(glMultiDrawArraysEXT);
    LOAD_EXTENSION(glMultiDrawElementsEXT);
    LOAD_EXTENSION(glCombinerParameterfvNV);
    LOAD_EXTENSION(glCombinerParameterfNV);
    LOAD_EXTENSION(glCombinerParameterivNV);
    LOAD_EXTENSION(glCombinerParameteriNV);
    LOAD_EXTENSION(glCombinerInputNV);
    LOAD_EXTENSION(glCombinerOutputNV);
    LOAD_EXTENSION(glFinalCombinerInputNV);
    LOAD_EXTENSION(glGetCombinerInputParameterfvNV);
    LOAD_EXTENSION(glGetCombinerInputParameterivNV);
    LOAD_EXTENSION(glGetCombinerOutputParameterfvNV);
    LOAD_EXTENSION(glGetCombinerOutputParameterivNV);
    LOAD_EXTENSION(glGetFinalCombinerInputParameterfvNV);
    LOAD_EXTENSION(glGetFinalCombinerInputParameterivNV);
    LOAD_EXTENSION(glCombinerStageParameterfvNV);
    LOAD_EXTENSION(glGetCombinerStageParameterfvNV);
    LOAD_EXTENSION(glGenProgramsNV);
    LOAD_EXTENSION(glDeleteProgramsNV);
    LOAD_EXTENSION(glBindProgramNV);
    LOAD_EXTENSION(glLoadProgramNV);
    LOAD_EXTENSION(glTrackMatrixNV);
    LOAD_EXTENSION(glProgramParameter4fvNV);
    LOAD_EXTENSION(glActiveStencilFaceEXT);
    LOAD_EXTENSION(glGetProgramParameterfvNV);
    LOAD_EXTENSION(glGetProgramParameterdvNV);
    LOAD_EXTENSION(glBindBufferARB);
    LOAD_EXTENSION(glDeleteBuffersARB);
    LOAD_EXTENSION(glGenBuffersARB);
    LOAD_EXTENSION(glIsBufferARB);
    LOAD_EXTENSION(glBufferDataARB);
    LOAD_EXTENSION(glBufferSubDataARB);
    LOAD_EXTENSION(glGetBufferSubDataARB);
    LOAD_EXTENSION(glMapBufferARB);
    LOAD_EXTENSION(glUnmapBufferARB);
    LOAD_EXTENSION(glGetBufferParameterivARB);
    LOAD_EXTENSION(glGetBufferPointervARB);
    LOAD_EXTENSION(glDrawRangeElements);

    LOAD_EXTENSION(glDeleteObjectARB);
    LOAD_EXTENSION(glGetHandleARB);
    LOAD_EXTENSION(glDetachObjectARB);
    LOAD_EXTENSION(glCreateShaderObjectARB);
    LOAD_EXTENSION(glShaderSourceARB);
    LOAD_EXTENSION(glCompileShaderARB);
    LOAD_EXTENSION(glCreateProgramObjectARB);
    LOAD_EXTENSION(glAttachObjectARB);
    LOAD_EXTENSION(glLinkProgramARB);
    LOAD_EXTENSION(glUseProgramObjectARB);
    LOAD_EXTENSION(glValidateProgramARB);
    LOAD_EXTENSION(glUniform1fvARB);
    LOAD_EXTENSION(glUniform2fvARB);
    LOAD_EXTENSION(glUniform3fvARB);
    LOAD_EXTENSION(glUniform4fvARB);
    LOAD_EXTENSION(glUniform1iARB);
    LOAD_EXTENSION(glUniform2iARB);
    LOAD_EXTENSION(glUniform3iARB);
    LOAD_EXTENSION(glUniform4iARB);
    LOAD_EXTENSION(glUniformMatrix2fvARB);
    LOAD_EXTENSION(glUniformMatrix3fvARB);
    LOAD_EXTENSION(glUniformMatrix4fvARB);
    LOAD_EXTENSION(glGetInfoLogARB);
    LOAD_EXTENSION(glGetUniformLocationARB);
    LOAD_EXTENSION(glGetObjectParameterivARB);
    LOAD_EXTENSION(glGetActiveUniformARB);

    #ifdef G3D_WIN32
        LOAD_EXTENSION(wglEnableGenlockI3D);
    #elif defined(G3D_OSX) || defined(G3D_LINUX)
        LOAD_EXTENSION(glXJoinSwapGroupNV);
        LOAD_EXTENSION(glXBindSwapBarrierNV);
        LOAD_EXTENSION(glXQuerySwapGroupNV);
        LOAD_EXTENSION(glXQueryMaxSwapGroupsNV);
        LOAD_EXTENSION(glXQueryFrameCountNV);
        LOAD_EXTENSION(glXResetFrameCountNV);

        LOAD_EXTENSION(glXCreateGLXPbufferSGIX);
        LOAD_EXTENSION(glXDestroyGLXPbufferSGIX);
        LOAD_EXTENSION(glXQueryGLXPbufferSGIX);
        LOAD_EXTENSION(glXChooseFBConfigSGIX);
        LOAD_EXTENSION(glXCreateContextWithConfigSGIX);
    #endif

    #undef LOAD_EXTENSION


    std::istringstream extensions;
    extensions.str((char*)glGetString(GL_EXTENSIONS));
    {
        // Parse the extensions into the supported set
        std::string s;
        while (extensions >> s) {
            extensionSet.insert(s);
        }

        // We're going to need exactly the same code for each of 
        // several extensions.
        #define DECLARE_EXT(extname) _supports_##extname = supports(#extname)
            DECLARE_EXT(GL_EXT_texture_rectangle);
            DECLARE_EXT(GL_ARB_vertex_program);
            DECLARE_EXT(GL_NV_vertex_program2);
            DECLARE_EXT(GL_ARB_vertex_buffer_object);
            DECLARE_EXT(GL_ARB_fragment_program);
            DECLARE_EXT(GL_ARB_multitexture);
            DECLARE_EXT(GL_EXT_stencil_wrap);
            DECLARE_EXT(GL_ARB_stencil_two_side);
            DECLARE_EXT(GL_EXT_texture_compression_s3tc);
            DECLARE_EXT(GL_EXT_texture_cube_map);
            DECLARE_EXT(GL_ARB_shadow);
            DECLARE_EXT(GL_ARB_shader_objects);
            DECLARE_EXT(GL_ARB_shading_language_100);
            DECLARE_EXT(GL_ARB_fragment_shader);
            DECLARE_EXT(GL_ARB_vertex_shader);
        #undef DECLARE_EXT

        // Verify that multitexture loaded correctly
        if (supports_GL_ARB_multitexture() &&
            ((glActiveTextureARB == NULL) ||
            (glMultiTexCoord4fvARB == NULL))) {
            _supports_GL_ARB_multitexture = false;
            #ifdef G3D_WIN32
                *((void**)&glActiveTextureARB) = glIgnore;
            #endif
        }

        _supports_GL_EXT_texture_rectangle = 
            _supports_GL_EXT_texture_rectangle ||
            supports("GL_NV_texture_rectangle");
    }
}


bool GLCaps::supports(const std::string& extension) {
    return extensionSet.contains(extension);
}


bool GLCaps::supports(const TextureFormat* fmt) {
    // First, check if we've already tested this format
    if (! _supportedTextureFormat.containsKey(fmt)) {
        uint8 bytes[8 * 8 * 4];

        glPushAttrib(GL_TEXTURE_BIT);

            // See if we can create a texture in this format
            unsigned int id;
            glGenTextures(1, &id);
            glBindTexture(id, GL_TEXTURE_2D);

            // Clear the old error flag
            glGetError();
            // 2D texture, level of detail 0 (normal), internal format, x size from image, y size from image, 
            // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
            glTexImage2D(GL_TEXTURE_2D, 0, fmt->OpenGLFormat, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

            bool success = (glGetError() == GL_NO_ERROR);
            _supportedTextureFormat.set(fmt, success);

            glDeleteTextures(1, &id);
        // Restore old texture state
        glPopAttrib();
    }

    return _supportedTextureFormat[fmt];
}


}
