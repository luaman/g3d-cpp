/**
  @file Window.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2005-02-10
  @edited  2004-03-04
*/

#ifndef G3D_GWINDOW_H
#define G3D_GWINDOW_H

#include "graphics3D.h"
#include "GLG3D/GWindowSettings.h"

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

 It is common to accept a GWindowSettings as an argument to the 
 constructor.

 <B>Input</B>
 This class will be extended with UI events and constants 
 in a future revision.  For now it is limited to rendering
 and window management.  Consider writing your own UserInput
 replacement in the mean time.

 <B>Beta API!  This interface is experimental and is subject to change.</B>

  I added a tentative G3D::Window interface in order to solicit comments.

Previously, RenderDevice and UserInput used SDL to abstract the underlying 
operating system's windowing and event APIs.  The major benefit of this 
architecture is that G3D does not contain the large amount of ugly and 
platform-specific code needed to map the platform APIs to a common API.  
Through getHDC and exposure of the underlying SDL functions, developers 
are still able to access platform specific features.  

The drawback to the previous architecture is that it prevents G3D users 
from initializing the window using a platform API.  An important case to 
consider are ActiveX controls/COM objects, where the HDC already exists 
before OpenGL is initialized.  Another is antialiasing, which is poorly 
supported by SDL on Windows and can only be enabled during window creation.  
A third case is negotiation with the platform for a compromise framebuffer
 when the ideal bit-depth or other features are not available. 

Although I intend to later extend it into event handling, the new Window
API only addresses platform features needed for RenderDevice.  G3D will 
ship with platform-independent SDLWindow (which RenderDevice will use by 
default).  A new init method for RenderDevice (and a new constructor for 
GApp) will accept a Window* and route all platform calls through that. 
Users will then be able to implement other subclasses, like HDCWindow, 
without having to rebuild the library or edit the internals of RenderDevice.  
This proposal is backwards compatible to the 6.00 API since everything will
default to SDLWindow.  
 */
class GWindow {
public:
    /** Closes the window and frees any resources associated with it.
        When subclassing, put any shutdown code (e.g. SDL_Quit()) in 
        your destructor.  Put initialization code (e.g. SDL_Init()) in
        the constructor. */
    virtual ~GWindow() {}

    /** Return the <I>actual</I> properties of this window (as opposed to
        the desired settings from which it was initialized) */
    virtual void getSettings(GWindowSettings& settings) const = 0;

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
     The value returned by getSettings will not change immediately-- 
     it waits for a notifyResize call.
     */
    virtual void setDimensions(const Rect2D& dims) = 0;

    /**
     Fails silently if unable to change the position.
     */
    virtual void setPosition(int x, int y) = 0;

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

    /** Returns 0 if there are no joysticks available */
    virtual int numJoysticks() const = 0;

    /** The name by which the OS refers to this joystick (e.g. "Gravis Gamepad") */
    virtual std::string joystickName(unsigned int sticknum) = 0;

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

    virtual void setRelativeMousePosition(double x, double y) = 0;

    /** Relative to the window origin */
    virtual void setRelativeMousePosition(const Vector2& p) = 0;


    /** Returns the current mouse position and the state of the mouse buttons.
        It is essential to sample both simultaneously so that the mouse has
        not moved from the location where a click occurred.

        @param position In pixels, with y=up.  Relative to the
        upper left corner of the window.

        @param mouseButtons If button <I>i</I> is currently pressed then bit <I>i</I> is set.
    */
    virtual void getRelativeMouseState(Vector2& position, uint8& mouseButtons) const = 0;
    virtual void getRelativeMouseState(int& x, int& y, uint8& mouseButtons) const = 0;
    virtual void getRelativeMouseState(double& x, double& y, uint8& mouseButtons) const = 0;

    /**
     Returns the state of the given joystick.  Not all sticks support all buttons and
     axes and frequently the state of one stick overlaps the state of another.
     An asserition fails if stickNum > numJoysticks().
     Joystick axes are normalized to the range [-1, 1] (but might not be calibrated...)
     */
    virtual void getJoystickState(unsigned int stickNum, Array<float>& axis, Array<bool>& button) = 0;

    /** 
      Capture the keyboard and mouse focus, locking the mouse to this window.
     */
    virtual void setInputCapture(bool c) = 0;

    /** Returns the current state of input capture */
    virtual bool inputCapture() const = 0;

    virtual void setMouseVisible(bool b) = 0;

    virtual bool mouseVisible() const = 0;
};

} // namespace

#endif
