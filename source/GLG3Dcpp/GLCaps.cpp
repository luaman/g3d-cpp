/**
  @file GLCaps.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2004-03-28
  @edited  2005-06-01
*/

#include "GLG3D/GLCaps.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/TextureFormat.h"
#include "GLG3D/getOpenGLState.h"
#include <sstream>

#ifdef G3D_WIN32
    #include <winver.h>
#endif

namespace G3D {

bool GLCaps::loadedExtensions = false;

int GLCaps::_numTextureCoords = 0;
int GLCaps::_numTextures = 0;
int GLCaps::_numTextureUnits = 0;

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
std::string GLCaps::_glVersion;
std::string GLCaps::_driverVendor;
std::string GLCaps::_driverVersion;
std::string GLCaps::_glRenderer;


GLCaps::Vendor GLCaps::computeVendor() {
    std::string s = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

    if (s == "ATI Technologies Inc.") {
        return ATI;
    } else if (s == "NVIDIA Corporation") {
        return NVIDIA;
    } else {
        return ARB;
    }
}


#ifdef G3D_WIN32
/**
 Used by the Windows version of getDriverVersion().
 @cite Based on code by Ted Peck tpeck@roundwave.com http://www.codeproject.com/dll/ShowVer.asp
 */
struct VS_VERSIONINFO { 
    WORD                wLength; 
    WORD                wValueLength; 
    WORD                wType; 
    WCHAR               szKey[1]; 
    WORD                Padding1[1]; 
    VS_FIXEDFILEINFO    Value; 
    WORD                Padding2[1]; 
    WORD                Children[1]; 
};
#endif

std::string GLCaps::getDriverVersion() {
    #ifdef G3D_WIN32
    
        std::string driver;

        // Locate the windows\system directory
        {
            char sysDir[1024];
            int sysSize = GetSystemDirectory(sysDir, 1024);
            if (sysSize == 0) {
                return "Unknown (can't find Windows directory)";
            }
            driver = sysDir;
        }

        switch (computeVendor()) {
        case ATI:
            driver = driver + "\\ati2dvag.dll";
            break;

        case NVIDIA:
            driver = driver + "\\nv4_disp.dll";
            break;

        default:
            return "Unknown (Unknown vendor)";

        }

        char* lpdriver = const_cast<char*>(driver.c_str());
        DWORD dummy;

        int size = GetFileVersionInfoSize(lpdriver, &dummy);
        if (size == 0) {
            return "Unknown (Can't find driver)";
        }

        void* buffer = new uint8[size];

        if (GetFileVersionInfo(lpdriver, NULL, size, buffer) == 0) {
            delete[] buffer;
            return "Unknown";
        }

	    // Interpret the VS_VERSIONINFO header pseudo-struct
	    VS_VERSIONINFO* pVS = (VS_VERSIONINFO*)buffer;
        debugAssert(!wcscmp(pVS->szKey, L"VS_VERSION_INFO"));

	    uint8* pVt = (uint8*) &pVS->szKey[wcslen(pVS->szKey) + 1];

        #define roundoffs(a,b,r)	(((uint8*)(b) - (uint8*)(a) + ((r) - 1)) & ~((r) - 1))
        #define roundpos(b, a, r)	(((uint8*)(a)) + roundoffs(a, b, r))

	    VS_FIXEDFILEINFO* pValue = (VS_FIXEDFILEINFO*) roundpos(pVt, pVS, 4);

        #undef roundoffs
        #undef roundpos

        std::string result = "Unknown (No information)";

	    if (pVS->wValueLength) {
	        result = format("%d.%d.%d.%d",
                pValue->dwProductVersionMS >> 16,
                pValue->dwProductVersionMS & 0xFFFF,
	            pValue->dwProductVersionLS >> 16,
                pValue->dwProductVersionLS & 0xFFFF);
        }

        delete[] buffer;

        return result;
    #else
        return "Unknown";
    #endif
}

// We're going to need exactly the same code for each of 
// several extensions.
#define DECLARE_EXT(extname)    bool GLCaps::_supports_##extname = false; 
    DECLARE_EXT(GL_ARB_texture_non_power_of_two);
    DECLARE_EXT(GL_EXT_texture_rectangle);
    DECLARE_EXT(GL_ARB_vertex_program);
    DECLARE_EXT(GL_NV_vertex_program2);
    DECLARE_EXT(GL_ARB_vertex_buffer_object);
    DECLARE_EXT(GL_ARB_fragment_program);
    DECLARE_EXT(GL_ARB_multitexture);
    DECLARE_EXT(GL_EXT_texture_edge_clamp);
    DECLARE_EXT(GL_ARB_texture_border_clamp);
    DECLARE_EXT(GL_EXT_texture_3D);
    DECLARE_EXT(GL_EXT_stencil_wrap);
    DECLARE_EXT(GL_EXT_stencil_two_side);
    DECLARE_EXT(GL_EXT_texture_compression_s3tc);
    DECLARE_EXT(GL_EXT_texture_cube_map);
    DECLARE_EXT(GL_ARB_shadow);
    DECLARE_EXT(GL_ARB_shader_objects);
    DECLARE_EXT(GL_ARB_shading_language_100);
    DECLARE_EXT(GL_ARB_fragment_shader);
    DECLARE_EXT(GL_ARB_vertex_shader);
    DECLARE_EXT(GL_EXT_framebuffer_object);
#undef DECLARE_EXT


void GLCaps::loadExtensions(Log* debugLog) {

    if (loadedExtensions) {
        return;
    }

    loadedExtensions = true;

	_driverVendor   = (char*)glGetString(GL_VENDOR);
	_glRenderer     = (char*)glGetString(GL_RENDERER);
	_glVersion      = (char*)glGetString(GL_VERSION);
	_driverVersion  = getDriverVersion();


    #define LOAD_EXTENSION(name) \
        *((void**)&name) = glGetProcAddress(#name);
/*
       if (debugLog) {debugLog->print("Loading " #name " extension");} \
        *((void**)&name) = glGetProcAddress(#name); \
       if (debugLog) {debugLog->printf("(0x%x)\n", name);}
*/

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

    LOAD_EXTENSION(glBlendEquationEXT);

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
    LOAD_EXTENSION(glGetAttribLocationARB);
    LOAD_EXTENSION(glGetObjectParameterivARB);
    LOAD_EXTENSION(glGetActiveUniformARB);

    LOAD_EXTENSION(glGenFragmentShadersATI);
    LOAD_EXTENSION(glDeleteFragmentShaderATI);
    LOAD_EXTENSION(glBindFragmentShaderATI);
    LOAD_EXTENSION(glBeginFragmentShaderATI);
    LOAD_EXTENSION(glEndFragmentShaderATI);
    LOAD_EXTENSION(glPassTexCoordATI);
    LOAD_EXTENSION(glSampleMapATI);
    LOAD_EXTENSION(glSetFragmentShaderConstantATI);
    LOAD_EXTENSION(glColorFragmentOp1ATI);
    LOAD_EXTENSION(glColorFragmentOp2ATI);
    LOAD_EXTENSION(glColorFragmentOp3ATI);
    LOAD_EXTENSION(glAlphaFragmentOp1ATI);
    LOAD_EXTENSION(glAlphaFragmentOp2ATI);
    LOAD_EXTENSION(glAlphaFragmentOp3ATI);

    LOAD_EXTENSION(glDrawBuffersARB);
    
    LOAD_EXTENSION(glTexImage3DEXT);

    #ifdef G3D_WIN32
        
        LOAD_EXTENSION(wglEnableGenlockI3D);
        LOAD_EXTENSION(wglCreatePbufferEXT);
        LOAD_EXTENSION(wglDestroyPbufferEXT);
        
    #elif defined(G3D_LINUX)

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

    // Frame buffer object
    LOAD_EXTENSION(glIsRenderBufferEXT);
    LOAD_EXTENSION(glBindRenderbufferEXT);
    LOAD_EXTENSION(glDeleteRenderbuffersEXT);
    LOAD_EXTENSION(glGenRenderbuffersEXT);
    LOAD_EXTENSION(glRenderbufferStorageEXT);
    LOAD_EXTENSION(glGetRenderbufferParameterivEXT);
    LOAD_EXTENSION(glIsFramebufferEXT);
    LOAD_EXTENSION(glBindFramebufferEXT);
    LOAD_EXTENSION(glDeleteFramebuffersEXT);
    LOAD_EXTENSION(glGenFramebuffersEXT);
    LOAD_EXTENSION(glCheckFramebufferStatusEXT);
    LOAD_EXTENSION(glFramebufferTexture1DEXT);
    LOAD_EXTENSION(glFramebufferTexture2DEXT);
    LOAD_EXTENSION(glFramebufferTexture3DEXT);
    LOAD_EXTENSION(glFramebufferRenderbufferEXT);
    LOAD_EXTENSION(glGetFramebufferAttachmentParameterivEXT);
    LOAD_EXTENSION(glGenerateMipmapEXT);

    #undef LOAD_EXTENSION

    std::istringstream extensions;
	std::string extStringCopy = (char*)glGetString(GL_EXTENSIONS);
    extensions.str(extStringCopy.c_str());
    {
        // Parse the extensions into the supported set
        std::string s;
        while (extensions >> s) {
	        extensionSet.insert(s);
        }

        // We're going to need exactly the same code for each of 
        // several extensions.
        #define DECLARE_EXT(extname) _supports_##extname = supports(#extname)
            DECLARE_EXT(GL_ARB_texture_non_power_of_two);
            DECLARE_EXT(GL_EXT_texture_rectangle);
            DECLARE_EXT(GL_ARB_vertex_program);
            DECLARE_EXT(GL_NV_vertex_program2);
            DECLARE_EXT(GL_ARB_vertex_buffer_object);
            DECLARE_EXT(GL_EXT_texture_edge_clamp);
            DECLARE_EXT(GL_ARB_texture_border_clamp);
            DECLARE_EXT(GL_EXT_texture_3D);
            DECLARE_EXT(GL_ARB_fragment_program);
            DECLARE_EXT(GL_ARB_multitexture);
            DECLARE_EXT(GL_EXT_stencil_wrap);
            DECLARE_EXT(GL_EXT_stencil_two_side);
            DECLARE_EXT(GL_EXT_texture_compression_s3tc);
            DECLARE_EXT(GL_EXT_texture_cube_map);
            DECLARE_EXT(GL_ARB_shadow);
            DECLARE_EXT(GL_ARB_shader_objects);
            DECLARE_EXT(GL_ARB_shading_language_100);
            DECLARE_EXT(GL_ARB_fragment_shader);
            DECLARE_EXT(GL_ARB_vertex_shader);
            DECLARE_EXT(GL_EXT_framebuffer_object);
        #undef DECLARE_EXT

        // Verify that multitexture loaded correctly
        if (supports_GL_ARB_multitexture() &&
            ((glActiveTextureARB == NULL) ||
            (glMultiTexCoord4fvARB == NULL))) {
            _supports_GL_ARB_multitexture = false;
            #ifdef G3D_WIN32
                *((void**)&glActiveTextureARB) = (void*)glIgnore;
            #endif
        }

        _supports_GL_EXT_texture_rectangle = 
            _supports_GL_EXT_texture_rectangle ||
            supports("GL_NV_texture_rectangle");


        // GL_ARB_texture_cube_map doesn't work on Radeon Mobility
        // GL Renderer:    MOBILITY RADEON 9000 DDR x86/SSE2
        // GL Version:     1.3.4204 WinXP Release
        // Driver version: 6.14.10.6430

		// GL Vendor:      ATI Technologies Inc.
		// GL Renderer:    MOBILITY RADEON 7500 DDR x86/SSE2
		// GL Version:     1.3.3842 WinXP Release
		// Driver version: 6.14.10.6371

		if (beginsWith(_glRenderer, "MOBILITY RADEON") &&
			beginsWith(std::string(_driverVersion), "6.14.10.6")) {
            Log::common()->printf("WARNING: This ATI Radeon Mobility card has a known bug with cube maps.\n"
                "   Put cube map texture coordinates in the normals and use ARB_NORMAL_MAP to work around.\n\n");
        }
    }

    // Don't use more texture units than allowed at compile time.
    if (GLCaps::supports_GL_ARB_multitexture()) {
        _numTextureUnits = iMin(G3D_MAX_TEXTURE_UNITS, 
                                glGetInteger(GL_MAX_TEXTURE_UNITS_ARB));
    } else {
        _numTextureUnits = 1;
    }

    // NVIDIA cards with GL_NV_fragment_program have different 
    // numbers of texture coords, units, and textures
    if (GLCaps::supports("GL_NV_fragment_program")) {
        glGetIntegerv(GL_MAX_TEXTURE_COORDS_NV, &_numTextureCoords);
        _numTextureCoords = iClamp(_numTextureCoords,
                                   _numTextureUnits,
                                   G3D_MAX_TEXTURE_UNITS);

        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_NV, &_numTextures);
        _numTextures = iClamp(_numTextures,
                              _numTextureUnits, 
                              G3D_MAX_TEXTURE_UNITS);
    } else {
        _numTextureCoords = _numTextureUnits;
        _numTextures      = _numTextureUnits;
    }

    if (! GLCaps::supports_GL_ARB_multitexture()) {
        // No multitexture
        if (debugLog) {
            debugLog->println("No GL_ARB_multitexture support: "
                              "forcing number of texture units "
                              "to no more than 1");
        }
        _numTextureCoords = iMax(1, _numTextureCoords);
        _numTextures      = iMax(1, _numTextures);
        _numTextureUnits  = iMax(1, _numTextureUnits);
    }
    debugAssertGLOk();

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


const std::string& GLCaps::glVersion() {
	loadExtensions();
	return _glVersion;
}


const std::string& GLCaps::driverVersion() {
	loadExtensions();
	return _driverVersion;
}


const std::string& GLCaps::vendor() {
	loadExtensions();
	return _driverVendor;
}


const std::string& GLCaps::renderer() {
	loadExtensions();
	return _glRenderer;
}


}
