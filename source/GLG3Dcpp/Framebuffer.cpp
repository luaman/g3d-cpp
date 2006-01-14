/**
@file Framebuffer.cpp

@maintainer Daniel Hilferty, djhilferty@users.sourceforge.net

Notes:
<UL>
<LI>http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
</UL>

@created 2006-01-07
@edited  2006-01-11
*/

#include "GLG3D/Framebuffer.h"
#include "GLG3D/glcalls.h"

namespace G3D {


/////////////////////////////////////////////////////////////////////////////

int attachmentToIndex(G3D::Framebuffer::Attachment attachment) {
    return attachment - G3D::Framebuffer::COLOR_ATTACHMENT0;
}


Framebuffer::Framebuffer (const std::string &_name, 
                          GLuint _framebufferID) : 
                          name(_name),
                          framebufferID(_framebufferID) {
    height = 0;
    width = 0;
    numAttachments = 0;

    // Create array (16 color + Stencil & Depth)
    rbAttachments.resize(18);
    texAttachments.resize(16);

    // Initialize Arrays
    for (int i = 0; i < 16; i++) {
        rbAttachments[i] = NULL;
        texAttachments[i] = NULL;
    }

    rbAttachments[16] = NULL;
    rbAttachments[17] = NULL;
}


Framebuffer::~Framebuffer () {
    glDeleteFramebuffersEXT(1, &framebufferID);
}


FramebufferRef Framebuffer::fromGLFramebuffer(const std::string &_name, GLuint _framebufferID) {
    return new Framebuffer (_name, _framebufferID);
}


FramebufferRef Framebuffer::createFramebuffer(const std::string &_name) {
    GLuint _framebufferID;
    
    // Generate Framebuffer
    glGenFramebuffersEXT(1, &_framebufferID);
    debugAssertGLOk();

    return new Framebuffer (_name, _framebufferID);
}


void Framebuffer::bindTexture (const TextureRef& texture, const G3D::Framebuffer::Attachment attachment) {
    GLint origFB;

    // Check for valid attachment point
    debugAssertM((attachment < Attachment::DEPTH_ATTACHMENT), "Invalid texture attachment point!");

    // Check for completeness
    if (this->numAttachments > 0) {
        // Same dimensions
        debugAssertM ((texture->texelWidth() != this->Width()) || (texture->texelHeight() != this->Height()), 
                      "All Images bound to a Framebuffer must have identical dimensions!");
    }
    
    // Set texture height/width
    width = texture->texelWidth();
    height = texture->texelHeight();

    // Get current framebuffer
    glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &origFB);

    // Only do this if the FB isn't already bound.
    if (origFB != this->getOpenGLID()) {
        // Bind this framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->getOpenGLID());
        debugAssertGLOk();
    }

    // Bind texture to framebuffer
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment, 
        texture->getOpenGLTextureTarget(), texture->getOpenGLID(), 0);

    // Modify array accordingly
    if ((texAttachments[attachmentToIndex(attachment)] == NULL) || 
        (rbAttachments[attachmentToIndex(attachment)] == NULL))
        numAttachments++;
    texAttachments[attachmentToIndex(attachment)] = texture;

    // Only do this if the FB isn't already bound.
    if (origFB !=this->getOpenGLID()) {
        // Bind original framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, origFB);
        debugAssertGLOk();
    }
}


/*
Unbinds a texture from an attachment point by attempting to bind the GL reserved
zero to that attachment.
*/
void Framebuffer::unbindTexture (const G3D::Framebuffer::Attachment attachment) {
    GLint origFB;

    // Get current framebuffer
    glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &origFB);

    // Only do this if the FB isn't already bound.
    if (origFB != this->getOpenGLID()) {
        // Bind this framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->getOpenGLID());
        debugAssertGLOk();
    }

    // Bind texture to framebuffer
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment, 0, 0, 0);
    debugAssertGLOk();

    // Modify array accordingly
    if ((texAttachments[attachmentToIndex(attachment)] != NULL) || 
        (rbAttachments[attachmentToIndex(attachment)] != NULL))
        numAttachments--;

    texAttachments[attachment - Attachment::COLOR_ATTACHMENT0] = NULL;
    rbAttachments[attachment - Attachment::COLOR_ATTACHMENT0] = NULL;

    // Only do this if the FB isn't already bound.
    if (origFB != this->getOpenGLID()) {
        // Bind original framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, origFB);
        debugAssertGLOk();
    }
}


void Framebuffer::bindRenderbuffer (
        const RenderbufferRef& renderbuffer, 
        const G3D::Framebuffer::Attachment attachment) {
    GLint origFB;

    // Check for completeness
    if (this->numAttachments > 0) {
        // Same dimensions
        debugAssertM ((renderbuffer->Width() != this->Width()) || 
                      (renderbuffer->Height() != this->Height()), 
                      "All Images bound to a Framebuffer must have identical dimensions!");
    }
    
    // Set texture height/width
    width = renderbuffer->Width();
    height = renderbuffer->Height();

    // Get current framebuffer
    glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &origFB);

    // Bind this framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->getOpenGLID());
    debugAssertGLOk();

    // Modify array accordingly
    if ((texAttachments[attachment - Attachment::COLOR_ATTACHMENT0] == NULL) || 
        (rbAttachments[attachment - Attachment::COLOR_ATTACHMENT0] == NULL))
        numAttachments++;
    rbAttachments[attachmentToIndex(attachment)] = renderbuffer;


    // Bind texture to framebuffer
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, attachment, 
        GL_RENDERBUFFER_EXT, renderbuffer->getOpenGLID());
    debugAssertGLOk();

    // Bind original framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, origFB);									
    debugAssertGLOk();
}


void Framebuffer::unbindRenderbuffer (const G3D::Framebuffer::Attachment attachment) {
    GLint origFB;

    // Get current framebuffer
    glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &origFB);

    // Bind this framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->getOpenGLID());
    debugAssertGLOk();

    // Bind texture to framebuffer
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, attachment, 
        GL_RENDERBUFFER_EXT, 0);
    debugAssertGLOk();

    // Modify array accordingly
    if ((texAttachments[attachmentToIndex(attachment)] != NULL) || 
        (rbAttachments[attachmentToIndex(attachment)] != NULL))
        numAttachments--;

    texAttachments[attachmentToIndex(attachment)] = NULL;
    rbAttachments[attachmentToIndex(attachment)] = NULL;


    // Bind original framebuffer
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, origFB);									
    debugAssertGLOk();
}


bool Framebuffer::isComplete(std::string& whyNot = std::string("")) {
    GLenum status;
    status = (GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE_EXT:
            return true;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENTS_EXT:
            whyNot = "Framebuffer Incomplete: Incomplete Attachment.";
            return false;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            whyNot = "Unsupported framebuffer format.";
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            whyNot = "Framebuffer Incomplete: Missing attachment.";
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            whyNot = "Framebuffer Incomplete: Attached images must have same dimensions.";
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            whyNot = "Framebuffer Incomplete: Attached images must have same format.";
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            whyNot = "Framebuffer Incomplete: Missing draw buffer.";
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            whyNot = "Framebuffer Incomplete: Missing read buffer.";
            return false;
        default:
            whyNot = "Framebuffer Incomplete: Unknown error.";
    }
    return false;    
}


} // End G3D Namespace
