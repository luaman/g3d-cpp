/**
 @file GLCaps.h

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2004-03-28
 @edited  2004-03-28

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
 Can be used without G3D::RenderDevice.

 The presence of an arbitrary extension can be determined
 using GLExt::supports(extString).  For convenience, frequently used
 extensions have fast tests named on the extension; e.g.
 GLExt::supports_GL_EXT_texture_rectangle.

 Note that GL_NV_texture_rectangle and GL_EXT_texture_rectangle
 have exactly the same constants, so supports_GL_EXT_texture_rectangle
 returns true if GL_NV_texture_rectangle is supported.
 */
class GLCaps {
private:

    /** True when loadExtensions has already been called */
    static bool     loadedExtensions;
  
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

#undef DECLARE_EXT

    static Set<std::string>         extensionSet;

public:
    /** Loads OpenGL extensions (e.g. glBindBufferARB).
        Call this once at the beginning of the program,
        after a video device is created.  This is called
        for you if you use G3D::RenderDevice.*/
    static void loadExtensions();

    static bool supports(const std::string& extName);
};

} // namespace

#endif
