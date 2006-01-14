/**
@file Renderbuffer.cpp

@maintainer Daniel Hilferty, djhilferty@users.sourceforge.net

@cite http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt

@created 2006-01-07
@edited  2006-01-11
*/

#include "GLG3D/Renderbuffer.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/TextureFormat.h"

namespace G3D {


/////////////////////////////////////////////////////////////////////////////

Renderbuffer::Renderbuffer (	
    const std::string&			_name, 
    const GLuint				_imageID,
    const G3D::TextureFormat*	_format, 
    const int					_width, 
    const int					_height) : 
    m_name(_name),
    m_imageID(_imageID),
    m_format(_format),
    m_width(_width),
    m_height(_height) {

    // Intentionally empty
}


Renderbuffer::~Renderbuffer () {
    glDeleteRenderbuffersEXT(1, &m_imageID);
}


RenderbufferRef Renderbuffer::fromGLRenderbuffer(
    const std::string&			_name, 
    const GLuint				_imageID,
    const G3D::TextureFormat	*_format) {

    GLint w, h;

    // Extract the width and height
    glGetRenderbufferParameterivEXT (GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &w);
    glGetRenderbufferParameterivEXT (GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &h);
    debugAssertGLOk();

    // Create new renderbuffer
    return new Renderbuffer(_name, _imageID, _format, w, h);
}


RenderbufferRef Renderbuffer::createEmpty(
    const std::string&			_name, 
    const G3D::TextureFormat*   _format, 
    const int					_width, 
    const int					_height) {
    // New Renderbuffer ID
    GLuint _imageID;

    // Save old renderbuffer state
    GLint origBuffer;
    glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &origBuffer);

    // Generate buffer
    glGenRenderbuffersEXT (1, &_imageID);
    debugAssertGLOk();

    // Bind the buffer
    glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, _imageID);
    debugAssertGLOk();

    // Allocate storage for it
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, _format->OpenGLFormat, _width, _height);

    // Check for successful generation (ie, no INVALID_OPERATION)
    debugAssertGLOk();

    // Restore renderbuffer state
    glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, origBuffer);
    debugAssertGLOk();

    // Create new renderbuffer
    return new Renderbuffer(_name, _imageID, _format, _width, _height);
}

} // End Namespace G3D