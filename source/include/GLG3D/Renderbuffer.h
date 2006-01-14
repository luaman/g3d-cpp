/**
  @file Renderbuffer.h

  @maintainer Daniel Hilferty, djhilferty@users.sourceforge.net

  @created 2006-01-06
  @edited  2006-01-11
*/

#ifndef GLG3D_RENDERBUFFER_H
#define GLG3D_RENDERBUFFER_H

#include "GLG3D/glheaders.h"
#include "GLG3D/TextureFormat.h"
#include "GLG3D/Texture.h"

namespace G3D {


typedef ReferenceCountedPointer<class Renderbuffer> RenderbufferRef;

/**
 Abstraction of the OpenGL renderbuffer object.  

 A Renderbuffer object is essentially an image that cannot be used as a
 texture.  It may be rendered to, but not used for rendering.  A perfect 
 example of a pure Renderbuffer would be a Stencil buffer.  All other 
 Renderbuffers could just as easily be implemented as textures.

 Example:

	<PRE>
		// Create Renderbuffer
		Renderbuffer rb = G3D::Renderbuffer::createEmpty ("Depth buffer", DEPTH16, 256, 256);

		// Create Framebuffer
		FramebuffeRef fb = G3D::Framebuffer::createFramebuffer ("offscreen target");

		// Bind renderbuffer
		fb->bindRenderbuffer (rb, G3D::FRAMEBUFFER::Attachment::DEPTH_COMPONENT);
		
		// Bind Framebuffer
		app->renderDevice->setFramebuffer(fb);
		
		// Check status and render away!
		if (app->renderDevice->checkFramebufferStatus() != 0) {
			// Render code here
			....
		}

		// Depth buffer can now be used at will for shadow maps and such
	</PRE>

	<B>BETA API</B> -- Subject to change
*/
class Renderbuffer : public ReferenceCountedObject {
private:

	/** Renderbuffer name */
    std::string						name;

	/** OpenGL Object ID */
	GLuint							imageID;

	/** Texel format */
    const class TextureFormat*      format;

	/** Buffer width */
    int								width;

	/** Buffer height */
    int								height;

	/** Constructor */
	Renderbuffer (	
		const std::string&			_name,
		const GLuint				_renderbufferID,
		const G3D::TextureFormat*	_format, 
		const int					_width, 
		const int					_height);

public:

	/** Destructor */
	~Renderbuffer () {
		glDeleteRenderbuffersEXT(1, &imageID);
	}

	/**
	 Create a renderbuffer object from a previously initialized OpenGL
	 renderbuffer context.

	 @param _name			Name of renderbuffer
	 @param _renderbufferID OpenGL context of prior Renderbuffer
	 @param _format			Texel format
	 @param _width			Width
	 @param _height			Height
	 @param _depth			Image depth
	 */
	static RenderbufferRef fromGLRenderbuffer(
		const std::string&			_name, 
		const GLuint				_imageID,
		const G3D::TextureFormat	*_format);

	/**
	 Creates an empty renderbuffer object.

	 @param _name	Name of renderbuffer
	 @param _format	Texel format
	 @param _width  Width
	 @param _height Height
	 @param _depth  Image depth
	 */
    static RenderbufferRef createEmpty(
		const std::string&			_name, 
		const G3D::TextureFormat	*_format, 
		const int					_width, 
		const int					_height);

	/**
	 Get OpenGL constant
	*/
    inline unsigned int getOpenGLID() const {
        return imageID;
	}

    /**
     Gets the OpenGL ID of the framebuffer object.
     */
	inline unsigned int Width() const {
        return width;
    }

    /**
     Gets the OpenGL ID of the framebuffer object.
     */
	inline unsigned int Height() const {
        return height;
    }

}; // End Clas Declaration Renderbuffer
}

#endif // GLG3D_RENDERBUFFER_H 