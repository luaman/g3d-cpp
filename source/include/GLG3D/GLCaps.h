/**
 @file GLCaps.h

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2004-03-28
 @edited  2004-05-07

 Copyright 2004, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_GLCAPS_H
#define G3D_GLCAPS_H

#include "graphics3D.h"
#include "G3D/platform.h"
#include "GLG3D/glheaders.h"

namespace G3D {

/**
 Low-level wrapper for OpenGL extension management.
 Can be used without G3D::RenderDevice to load and
 manage extensions.

 OpenGL has a base API and an extension API.  All OpenGL drivers
 must support the base API.  The latest features may not 
 be supported by some drivers, so they are in the extension API
 and are dynamically loaded at runtime using GLCaps::loadExtensions.  
 Before using a specific extension you must test for its presence
 using the GLCaps::supports method.
 
 For convenience, frequently used extensions have fast tests, e.g.,
 GLExt::supports_GL_EXT_texture_rectangle.

 Note that GL_NV_texture_rectangle and GL_EXT_texture_rectangle
 have exactly the same constants, so supports_GL_EXT_texture_rectangle
 returns true if GL_NV_texture_rectangle is supported.

 GLCaps assumes all OpenGL contexts have the same capabilities.
 */
class GLCaps {
private:
	enum Vendor {ATI, NVIDIA, ARB};

    /** True when loadExtensions has already been called */
    static bool     loadedExtensions;

	/** Set by loadExtensions */
	static std::string _glVersion;
	static std::string _driverVendor;
	static std::string _driverVersion;
	static std::string _glRenderer;

	static Vendor computeVendor();

	/**
	 Returns the version string for the video driver.

	 @cite Based in part on code by Ted Peck tpeck@roundwave.com http://www.codeproject.com/dll/ShowVer.asp
	 */
	static std::string getDriverVersion();
  
 // We're going to need exactly the same code for each of 
 // several extensions, so we abstract the boilerplate into
 // a macro that declares a private variable and public accessor.
#define DECLARE_EXT(extname)                    \
private:                                        \
    static bool     _supports_##extname;        \
public:                                         \
    static bool inline supports_##extname() {   \
        return _supports_##extname;             \
    }                                           \
private:

    // New extensions must be added in 3 places: 1. here.
    // 2. at the top of GLCaps.cpp.  3. underneath the LOAD_EXTENSION code.
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

    static Set<std::string>         extensionSet;

public:
    /** Loads OpenGL extensions (e.g. glBindBufferARB).
        Call this once at the beginning of the program,
        after a video device is created.  This is called
        for you if you use G3D::RenderDevice.*/
    static void loadExtensions();

    static bool supports(const std::string& extName);

    /** Returns true if the given texture format is supported on this device.*/
    static bool supports(const class TextureFormat* fmt);

	static const std::string& glVersion();

	static const std::string& driverVersion();

	static const std::string& vendor();

	static const std::string& renderer();};

} // namespace

#endif
