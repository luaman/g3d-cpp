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

#ifndef GLG3D_FRAMEBUFFER_H
#define GLG3D_FRAMEBUFFER_H

#include "GLG3D/Texture.h"
#include "GLG3D/Renderbuffer.h"
#include "G3D/Array.h"
#include "G3D/Table.h"
#include <string.h>

namespace G3D {

typedef ReferenceCountedPointer<class Framebuffer> FramebufferRef;

/**
 Abstraction of OpenGL's Framebuffer Object extension.  This is a fast and 
 efficient way of rendering to textures.  This class can be used with raw OpenGL, 
 without RenderDevice or SDL.

 Basic Framebuffer Theory:
	Every OpenGL program has at least one Framebuffer.  This framebuffer is
 setup by the windowing system and its image format is that specified by the
 OS.  With the Framebuffer Object extension, OpenGL gives the developer
 the ability to create offscreen framebuffers that can be used to render 
 to textures of any specified format.
    The Framebuffer class is used in conjunction with the RenderDevice to
 set a render target.  The RenderDevice method setFramebuffer performs this
 action.  If a NULL argument is passed to setFramebuffer, the render target
 defaults to the window display framebuffer.
    Framebuffer works in conjunction with the push/pop RenderDevice state, but
 in a limited form.   The state will save the current Framebuffer, but will not
 save the state of the Framebuffer itself.  If the attachment points are changed
 in the push/pop block, these will not be restored by a pop.
    The following example shows how to create a texture and bind it to Framebuffer
 for rendering.

 Framebuffer Example:

 <PRE>
 // Create Framebuffer
  FramebuffeRef fb = G3D::Framebuffer::createFramebuffer ("offscreen target");
 
 // Create Texture
  TextureRef tex = G3D::Texture::createEmpty(256, 256, "texture1", 
 											 G3D::TextureFormat::RGBA16, 
 											 G3D::Texture::WrapMode::TILE, 
											 G3D::Texture::InterpolateMode::NO_INTERPOLATION, 
											 G3D::Texture::Dimension::DIM_2D);

  // Bind Texture to Framebuffer
  fb->bindTexture (tex);

  // Set framebuffer as the render target
  app->renderDevice->setFramebuffer(fb);

  if (app->renderDevice->checkFramebufferStatus() != 0) {
 	// Render code here
 	...
  }

  // Set Screen as render target
  app->renderDevice->setFramebuffer(NULL);

  // Now tex can be used as a texture to render to the display
  .... More render code ....
 </PRE>

 In addition to Textures, Renderbuffers may also be bound to the
 Framebuffer.   This is done in a very similar manner to the Texture
 object in the example.  Renderbuffer class contains an example of this
 operation.

 Note:  Not any combination of images may be attached to a Framebuffer.
 OpenGL lays out some restrictions that must be considered:
	1) In order to render to a Framebuffer, there must be at least
	one image (Renderbuffer or Texture) attached to an attachment point.
	2) All images must have the same height and width.
	3) All images attached to a COLOR_ATTACHMENT[n] point must have
	the same internal format (RGBA8, RGBA16...etc)
	4) If RenderDevice->setDrawBuffer is used then the specified 
	attachment point must have a bound image.
	5) The combination of internal formats of attached images does not
	violate some implementation-dependent set of restrictions (ie. Your
	graphics card must completely implement all combinations that you
	plan to use!)


	<B>BETA API</B> -- Subject to change
*/
class Framebuffer : public ReferenceCountedObject {
public:

    /**
	 Specifies the attachment to which a framebuffer-attachable-image
	 (Renderbuffer or a Texture image) will be attached.  These mirror
	 the OpenGL definition as do their values.
	 */
	enum AttachmentPoint {
		COLOR_ATTACHMENT0 = 0x8CE0,
        COLOR_ATTACHMENT1 = 0x8CE1,
        COLOR_ATTACHMENT2 = 0x8CE2,
        COLOR_ATTACHMENT3 = 0x8CE3,
        COLOR_ATTACHMENT4 = 0x8CE4,
        COLOR_ATTACHMENT5 = 0x8CE5,
        COLOR_ATTACHMENT6 = 0x8CE6,
        COLOR_ATTACHMENT7 = 0x8CE7,
        COLOR_ATTACHMENT8 = 0x8CE8,
        COLOR_ATTACHMENT9 = 0x8CE9,
        COLOR_ATTACHMENT10 = 0x8CEA,
        COLOR_ATTACHMENT11 = 0x8CEB,
        COLOR_ATTACHMENT12 = 0x8CEC,
        COLOR_ATTACHMENT13 = 0x8CED,
        COLOR_ATTACHMENT14 = 0x8CEE,
        COLOR_ATTACHMENT15 = 0x8CEF,
        DEPTH_ATTACHMENT = 0x8D00,
		STENCIL_ATTACHMENT = 0x8D20};

private:

    class Attachment {
    public:
        enum Type {TEXTURE, RENDERBUFFER};
        Type type;

        RenderbufferRef             renderbuffer;
        TextureRef                  texture;

        Attachment() {}
        Attachment(const RenderbufferRef& r) : type(RENDERBUFFER), renderbuffer(r) {}
        Attachment(const TextureRef& r) : type(TEXTURE), texture(r) {}
    };

    /**
     Slots are not specified if they correspond to NULL elements.
     */
    Table<AttachmentPoint, Attachment>  attachmentTable;

	/** OpenGL Object ID */
	GLuint							framebufferID;

	/** Framebuffer name */
	std::string                     m_name;

	/** 
	 Not yet implemented yet -- for non-gl error checking to pre-check
	 for Framebuffer completeness.  Width & Height should also be
	 implemented for this check.
	*/
    const class TextureFormat*      format;

    /**
     Framebuffer Height
     */
    GLuint                          m_height;
    GLuint                          m_width;

    /**
     Number of currently bound attachments.  When this hits zero we can
     add attachments with new sizes.
     */
    int                             numAttachments;

	/** Default Constructor. */
	Framebuffer(const std::string& name, GLuint framebufferID);

    static std::string              ignore;

public:


	/** Reclaims OpenGL ID */
	~Framebuffer();

	/**
	 Creates a framebuffer object from an OpenGL context.

	 @param name			Name of framebuffer
	 @param framebufferID	OpenGL id of ramebuffer
	 */
	static FramebufferRef fromGLFramebuffer(const std::string& name, GLuint framebufferID);

	/**
	 Creates a framebuffer object from scratch.

	 @param name			Name of framebuffer
	 */
	static FramebufferRef create(const std::string& name);

    /** Overload used when setting attachment points to NULL */
    void set(AttachmentPoint ap, const void* n);

	/**
	 Set one of the attachment points to reference a texture.  Set to NULL to unset.

	 @param texture		Texture to bind to the framebuffer.
	 @param ap	Attachment point to bind texture to.
	 */
	void set(AttachmentPoint ap, const TextureRef& texture);

	/**
	 Set one of the attachment points to reference a renderbuffer.  Set to NULL to unset.

	 @param renderbuffer	Renderbuffer to bind to the framebuffer
	 @param slot		Attachment point to bind renderbuffer to.
	 */
	void set(AttachmentPoint ap, const RenderbufferRef& renderbuffer);

    /**
     Gets the OpenGL ID of the framebuffer object.
     */
	inline unsigned int openGLID() const {
        return framebufferID;
    }

    /**
     Gets the OpenGL ID of the framebuffer object.
     */
	inline unsigned int width() const {
        return m_width;
    }

    /**
     Gets the OpenGL ID of the framebuffer object.
     */
	inline unsigned int height() const {
        return m_height;
    }

    inline const std::string& name() const {
        return m_name;
    }

    /**
     Checks to see if the framebuffer is complete.  

     NOTE: This function is meant to be called by RenderDevice through currentFramebufferComplete.
     The results are unpredictable if this function is directly called by the user.  

     @param whyNot Defined when the test fails.  Will contain the reason for failure.

     @return true   If complete framebuffer.
     @return false  If incomplete or error.
     */
    bool isComplete(std::string& whyNot = ignore);

}; // class Framebuffer 


} //  G3D

unsigned int hashCode(const G3D::Framebuffer::AttachmentPoint& a);

#endif // GLG3D_FRAMEBUFFER_H

