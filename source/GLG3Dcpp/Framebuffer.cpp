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


Framebuffer::Framebuffer(
    const std::string&  _name, 
    GLuint              _framebufferID) : 
    m_name(_name),
    framebufferID(_framebufferID),
    m_height(0),
    m_width(0),
    numAttachments(0) {
}


Framebuffer::~Framebuffer () {
    glDeleteFramebuffersEXT(1, &framebufferID);
    framebufferID = 0;
}


FramebufferRef Framebuffer::fromGLFramebuffer(const std::string& _name, GLuint _framebufferID) {
    // TODO: If there are existing attachments, find their size
    return new Framebuffer(_name, _framebufferID);
}


FramebufferRef Framebuffer::create(const std::string& _name) {
    GLuint _framebufferID;
    
    // Generate Framebuffer
    glGenFramebuffersEXT(1, &_framebufferID);
    debugAssertGLOk();

    return new Framebuffer(_name, _framebufferID);
}


void Framebuffer::set(AttachmentPoint ap, const void* n) {
    debugAssert(n == NULL);

    // Get current framebuffer
    GLint origFB = glGetInteger(GL_FRAMEBUFFER_BINDING_EXT);

    // If we aren't already bound, bind us now
    if (origFB != openGLID()) {
        // Bind this framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, openGLID());
        debugAssertGLOk();
    }

    if (attachmentTable.containsKey(ap)) { 
        // Detach
        if (attachmentTable[ap].type == Attachment::TEXTURE) {
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, ap, 0, 0, 0);
        } else {
            glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, ap, GL_RENDERBUFFER_EXT, 0);
        }

        --numAttachments;

    } else {
        // Wipe our record for that slot
        attachmentTable.remove(ap);
    }

    // If we were already bound, don't bother restoring
    if (origFB != openGLID()) {
        // Bind original framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, origFB);
    }
}


void Framebuffer::set(AttachmentPoint ap, const TextureRef& texture) {
    if (texture.isNull()) {
        // We're in the wrong overload
        set(ap, (void*)NULL);
        return;
    }

    // Get current framebuffer
    GLint origFB = glGetInteger(GL_FRAMEBUFFER_BINDING_EXT);

    // If we aren't already bound, bind us now
    if (origFB != openGLID()) {
        // Bind this framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, openGLID());
        debugAssertGLOk();
    }

    // Check for completeness
    if (numAttachments == 0) {
        // This is the first attachment.
        // Set texture height/width
        m_width  = texture->texelWidth();
        m_height = texture->texelHeight();
    } else {
        // Verify same dimensions
        debugAssertM((texture->texelWidth() != width()) || 
                      (texture->texelHeight() != height()), 
           "All attachments bound to a Framebuffer must have identical dimensions!");
    }
    

    // Bind texture to framebuffer
    glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT, 
        ap, 
        texture->getOpenGLTextureTarget(), 
        texture->openGLID(), 0);

    if (!attachmentTable.containsKey(ap)) {
        attachmentTable.set(ap, Attachment(texture));
    }

    // If we were already bound, don't bother restoring
    if (origFB != openGLID()) {
        // Bind original framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, origFB);
    }

    debugAssertGLOk();
}



void Framebuffer::set(                      
    AttachmentPoint ap,
    const RenderbufferRef& renderbuffer) {

    if (renderbuffer.isNull()) {
        // We're in the wrong overload
        set(ap, (void*)NULL);
        return;
    }

    // Get current framebuffer
    GLint origFB = glGetInteger(GL_FRAMEBUFFER_BINDING_EXT);

    // If we aren't already bound, bind us now
    if (origFB != openGLID()) {
        // Bind this framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, openGLID());
        debugAssertGLOk();
    }

    // Check for completeness
    if (numAttachments == 0) {
        // This is the first attachment.
        // Set texture height/width
        m_width  = renderbuffer->width();
        m_height = renderbuffer->height();
    } else {
        // Verify same dimensions
        debugAssertM((renderbuffer->width()  != width()) || 
                     (renderbuffer->height() != height()), 
           "All attachments bound to a Framebuffer must have identical dimensions!");
    }
    
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, ap, GL_RENDERBUFFER_EXT, renderbuffer->openGLID());

    if (!attachmentTable.containsKey(ap)) {
        attachmentTable.set(ap, Attachment(renderbuffer));
    }

    // If we were already bound, don't bother restoring
    if (origFB != openGLID()) {
        // Bind original framebuffer
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, origFB);
    }

    debugAssertGLOk();
}


bool Framebuffer::isComplete(std::string& whyNot) {
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


} // G3D

unsigned int hashCode(const G3D::Framebuffer::AttachmentPoint& a) {
    return (int)a;
}

