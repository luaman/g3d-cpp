/**
  @file Window.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2005-02-10
  @edited  2004-02-10
*/

#ifndef G3D_WINDOW_H
#define G3D_WINDOW_H

#include "graphics3d.h"
#include "GLG3D/WindowSettings.h"

namespace G3D {

/**
 Interface to Window APIs for window management, event processing,
 and OpenGL context management.  G3D assumes there is only one
 OS-level window, with one OpenGL context.  (Although you <B>can</B> have
 multiple non-GL windows if using a native windowing API and you can even have
 multiple OpenGL contexts if you manage switching between them yourself).

 Subclass this interface to support your preferred window system
 (e.g. ActiveX window, OS/X Carbon, MFC window, glut) or use the 
 generic SDLWindow subclass that supports all platforms.

 <B>Subclassing</B>

 It is common to accept a WindowSettings as an argument to the 
 constructor.

 <B>Input</B>
 This class will be extended with UI events and constants 
 in a future revision.  For now it is limited to rendering
 and window management.  Consider writing your own UserInput
 replacement in the mean time.

 <B>Beta API!  This interface is experimental and is subject to change.</B>
 */
class Window {
public:
    /** Closes the window and frees any resources associated with it.
        When subclassing, put any shutdown code (e.g. SDL_Quit()) in 
        your destructor.  Put initialization code (e.g. SDL_Init()) in
        the constructor. */
    virtual ~Window() {}

    /** Return the <I>actual</I> properties of this window (as opposed to
        the desired settings from which it was initialized) */
    virtual void getSettings(WindowSettings& settings) const = 0;

    /**
     Measured in pixels.
     Returns the same width as getSettings and dimensions().
     Convenience method for users.
     */
    virtual int width() const = 0;

    virtual int height() const = 0;

    /**
     Shape of the window.
     */
    virtual Rect2D dimensions() const = 0;

    /**
     Fails silently if unable to change the dimensions.
     */
    virtual void setDimensions(const Rect2D& dims) = 0;

    /**
     Fails silently if unable to change the position.
     */
    virtual void setPosition(int x, int y) const = 0;

    /** Returns true if this window currently has [keyboard] focus
        (i.e. is in the foreground, not minimized, recieves keystrokes.) */
    virtual bool hasFocus() const = 0;

    /** Description of the windowing API for logging purposes. */
    virtual std::string getAPIVersion() const = 0;

    /** Description of the windowing API for logging purposes. */
    virtual std::string getAPIName() const = 0;

    /** gammaRamp.length() = 256 */
    virtual void setGammaRamp(const Array<uint16>& gammaRamp) = 0;

    virtual void setCaption(const std::string& caption) = 0;
    
    #ifdef G3D_WIN32
        /** On Windows, returns the HDC.  Not present on other platforms. */
        //  This is part of the base class API because different
        //  Window subclasses provide access to the HDC on Win32 and
        //  the consumer of a Window should not need to know which 
        //  subclass.  We could provide a separate Win32Window interface 
        //  for those subclasses but were we to have separate "mix-ins"
        //  for each special property it would become unweildy given
        //  C++'s inheritance system.
        virtual HDC getHDC() const = 0;
    #endif

    /** The window title */
    virtual std::string caption() = 0;

    /** Set the icon (if supported).  Fails silently if not supported
        or the window has no frame.
        @param image May have any dimensions. */
    virtual void setIcon(const GImage& image) = 0;

    /** Swap the OpenGL front and back buffers.  Called by RenderDevice::endFrame. */
    virtual void swapGLBuffers() = 0;

    /** Notifies the window that it has been resized 
        (called by RenderDevice::notifyResize).  Some window systems
        (e.g. SDL) need explicit notification in this form when
        a resize event comes in.
        
        <B>Must not</B> destroy the underlying
        OpenGL context (i.e. textures, vertex buffers, and GPU programs
        must be preserved across calls).*/
    virtual void notifyResize(int w, int h) = 0;
};

} // namespace

#endif
