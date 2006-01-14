/**
@file Renderbuffer.cpp

@maintainer Daniel Hilferty, djhilferty@users.sourceforge.net

Notes:
<UL>
<LI>http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
</UL>

@created 2006-01-07
@edited  2006-01-11
*/

#include "GLG3D/Renderbuffer.h"
#include "GLG3D/glcalls.h"

namespace G3D {


/////////////////////////////////////////////////////////////////////////////

Renderbuffer::Renderbuffer (	
    const std::string&			_name, 
    const GLuint				_imageID,
    const G3D::TextureFormat*	_format, 
    const int					_width, 
    const int					_height) : 
    name(name),
    imageID(_imageID),
    format(_format),
    width(_width),
    height(_height) {}


RenderbufferRef Renderbuffer::fromGLRenderbuffer(
    const std::string&			_name, 
    const GLuint				_imageID,
    const G3D::TextureFormat	*_format) {
    GLint _width, _height;

    glGetRenderbufferParameterivEXT (GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &_width);
    debugAssertGLOk();

    glGetRenderbufferParameterivEXT (GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &_height);
    debugAssertGLOk();

    // Create new renderbuffer
    return new Renderbuffer (_name, _imageID, _format,
        _width, _height);
}


RenderbufferRef Renderbuffer::createEmpty(
    const std::string&			_name, 
    const G3D::TextureFormat	*_format, 
    const int					_width, 
    const int					_height) {
    // New Renderbuffer ID
    GLuint _imageID;

    // Renderbuffer Push
    GLint origBuffer;
    glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &origBuffer);

    // Generate buffer
    glGenRenderbuffersEXT (1, &_imageID);
    debugAssertGLOk();

    // Bind the buffer
    glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, _imageID);
    debugAssertGLOk();

    // Allocate storage for it
    glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT, 
                                _format->OpenGLFormat, 
                                _width, _height);

    // Check for successful generation (ie, no INVALID_OPERATION)
    debugAssertGLOk();

    // Renderbuffer Pop
    glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, origBuffer);
    debugAssertGLOk();

    // Create new renderbuffer
    return new Renderbuffer (_name, _imageID, _format,
                                _width, _height);
}

} // End Namespace G3D