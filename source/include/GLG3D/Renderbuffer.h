/**
  @file Renderbuffer.h

  @maintainer Daniel Hilferty, djhilferty@users.sourceforge.net

  @created 2006-01-06
  @edited  2006-01-11
*/

#ifndef GLG3D_RENDERBUFFER_H
#define GLG3D_RENDERBUFFER_H

#include <string>
#include "G3D/ReferenceCount.h"
#include "G3D/Vector2.h"
#include "G3D/Rect2D.h"
#include "GLG3D/glheaders.h"

namespace G3D {


typedef ReferenceCountedPointer<class Renderbuffer> RenderbufferRef;

/**
 Abstraction of the OpenGL renderbuffer object.  A Framebuffer has many
 buffers inside it; typically one for colors, one for stencil, one for depth.
 Those are Renderbuffers.

 A Renderbuffer object is essentially an image that cannot be used as a
 texture.  It may be rendered to, but not used for rendering.  A perfect 
 example of a pure Renderbuffer would be a Stencil buffer.  All other 
 Renderbuffers could just as easily be implemented as textures.

 Example:

	<PRE>
		// Create Renderbuffer
		Renderbuffer rb = G3D::Renderbuffer::createEmpty("Depth buffer", DEPTH16, 256, 256);

		// Create Framebuffer
		FramebuffeRef fb = G3D::Framebuffer::createFramebuffer("offscreen target");

		// Bind renderbuffer
		fb->bindRenderbuffer(rb, G3D::FRAMEBUFFER::Attachment::DEPTH_COMPONENT);
		
		// Bind Framebuffer
		app->renderDevice->setFramebuffer(fb);
		
		// Check status and render away!
		if (app->renderDevice->checkFramebufferStatus() != 0) {
			// Render code here
			....
		}

		// Depth buffer can now be used at will for shadow maps and such
        TODO: show me how
	</PRE>

	<B>BETA API</B> -- Subject to change

  @cite http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
*/
class Renderbuffer : public ReferenceCountedObject {
private:

	/** Renderbuffer name */
    std::string						m_name;

	/** OpenGL Object ID */
	GLuint							m_imageID;

	/** Texel format */
    const class TextureFormat*      m_format;

	/** Buffer width */
    int								m_width;

	/** Buffer height */
    int								m_height;

	/** Constructor */
	Renderbuffer (	
		const std::string&			_name,
		const GLuint				_renderbufferID,
		const G3D::TextureFormat*	_format, 
		const int					_width, 
		const int					_height);

public:

	/** Destroys the underlying OpenGL id */
	~Renderbuffer();

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
		const std::string&			name, 
		const GLuint				imageID,
		const G3D::TextureFormat*   format);

	/**
	 Creates an empty renderbuffer object.

	 @param _name	Name of renderbuffer
	 @param _format	Texel format
	 @param _width  Width
	 @param _height Height
	 @param _depth  Image depth
	 */
    static RenderbufferRef createEmpty(
		const std::string&			name, 
		const class TextureFormat*  format, 
		const int					width, 
		const int					height);

	/**
	 Get OpenGL renderbuffer ID, useful for accessing
     functionality not directly exposed by G3D.
	*/
    inline unsigned int openGLID() const {
        return m_imageID;
	}

    inline const TextureFormat* format() const {
        return m_format;
    }

    inline const std::string& name() const {
        return m_name;
    }

	inline unsigned int width() const {
        return m_width;
    }

	inline unsigned int height() const {
        return m_height;
    }
   
    inline Vector2 vector2Bounds() const {
        return Vector2(m_width, m_height);
    }

    inline Rect2D rect2DBounds() const {
        return Rect2D::xywh(0, 0, m_width, m_height);
    }

}; // class Renderbuffer
} // G3D

#endif // GLG3D_RENDERBUFFER_H 
