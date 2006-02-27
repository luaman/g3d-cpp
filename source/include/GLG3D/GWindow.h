/**
  @file Window.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2005-02-10
  @edited  2006-02-15
*/

#ifndef G3D_GWINDOW_H
#define G3D_GWINDOW_H

#include "G3D/platform.h"
#include "G3D/GImage.h"
#include "G3D/Array.h"
#include "GLG3D/GWindowSettings.h"

// For SDL_Event
#if defined(G3D_OSX)
#include <SDL/SDL_events.h>
#elif defined(G3D_WIN32)
#include <SDL_events.h>
#else
#include <SDL.h>
#endif


// Setup the "main" linkage if not using SDL_main
#if (defined(NO_SDL_MAIN) || defined(_CONSOLE)) && defined(main)
    #undef main
#endif

namespace G3D {

/** @deprecated To be replaced in 7.00 with a new implementation. */
typedef SDL_Event GEvent;

class Rect2D;

/**
 Interface to window APIs for window management, event processing,
 and OpenGL context management.  A GWindow may be a user-level window, 
 with a minimize button and frame, or simply a rectangular area within 
 a larger window.  In the latter case, several of the methods (e.g.,
 setCaption) are likely to be ignored by the implementation.  See
 G3D::SDLWindow and the wxWindows, Qt, and Glut implementations in
 the contrib directory.
   
 Many parts of G3D assume that there is only one
 OS-level window, with one OpenGL context.  (Although you <B>can</B> have
 multiple non-GL windows if using a native windowing API and you can even have
 multiple OpenGL contexts if you manage switching between them yourself).

 Subclass this interface to support your preferred window system
 (e.g. ActiveX window, OS/X Carbon, MFC window, glut) or use the 
 generic SDLWindow subclass that supports all platforms.

 All dimensions are of the client area (inside the frame, if the
 window has a frame).

 After instantiation, a GWindow guarantees that the OpenGL context for this
 window is bound.  It may be unbound by later code--use GWindow::makeCurrent
 if you have multiple windows in your application.

 <B>Subclassing</B>

 It is common to accept a GWindowSettings as an argument to the 
 constructor.

 <B>Input</B>
 This class will be extended with UI events and constants 
 in a future revision.  For now it is limited to rendering
 and window management.  Consider writing your own UserInput
 replacement in the mean time.

 <B>Implementations</B>
 The following GWindow subclasses already exist: 
 G3D::SDLWindow, 
 G3D::Win32Window, 
 <A HREF="../contrib/wxGWindow">wxGWindow</A> (wxWidgets),
 <A HREF = "../contrib/GlutWindow">GlutWindow</a>,
 and <A HREF="../contrib/CoreyGWindow/QGWindow.h">QGWindow</a> (Qt).

 One typically chooses among these based on the GUI API used 
 to manage the main window.
 */
class GWindow {
private:

    /** */
    class LoopBody {
    public:
        union{
            void (*func)(void*);
            class GApplet*      applet;
        };

        void*                   arg;        
        
        /** If true, the applet's oneFrame method is invoked in executeLoopBody.
            If false, func is invoked on arg in  executeLoopBody. */
        bool                    isGApplet;

        LoopBody() : func(NULL), arg(NULL), isGApplet(false) {}
        LoopBody(GApplet* a) : applet(a), arg(NULL), isGApplet(true) {}
        LoopBody(void (*f)(void*), void* a) : func(f), arg(a), isGApplet(false) {}
    };

    Array<LoopBody>             loopBodyStack;

protected:

    bool notDone() {
        return loopBodyStack.size() > 0;
    }

    /** Subclasses should call from their idle function. */
    void executeLoopBody();

    /** Subclasses are required to call this
        from the end of their constructor/create function to
        finish initializing OpenGL. 

        @deprecated Call GLCaps::init instead.
    */
    void G3D_DEPRECATED loadExtensions();

public:


    /** Return the <I>actual</I> properties of this window (as opposed to
        the desired settings from which it was initialized) */
    virtual void getSettings(GWindowSettings& settings) const = 0;

    /**
     Measured in pixels, this is the client area of the window.
     Returns the same width as getSettings and dimensions().
     Convenience method for users.
     */
    virtual int width() const = 0;

    virtual int height() const = 0;

    /**
     Shape of the client area of the window in screen coordinates.
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

    /** Description of the windowing API for logging purposes (e.g. "1.2.7" for SDL 1.2.7). */
    virtual std::string getAPIVersion() const = 0;

    /** Description of the windowing API for logging purposes (e.g. "SDL"). */
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
        or the window has no frame.  <I>May also fail if the GWindow implementation's
        underlying GUI library is too limited.</I>
        @param image May have any dimension supported by underlying OS.*/
    virtual void setIcon(const GImage& image) {
        (void)image;
    }

	virtual void setIcon(const std::string& imageFilename) {
		setIcon(GImage(imageFilename));
	}

    /** Swap the OpenGL front and back buffers.  Called by RenderDevice::endFrame. */
    virtual void swapGLBuffers() = 0;

    /** Notifies the window that it has been resized 
        (called by RenderDevice::notifyResize).  Some window systems
        (e.g. SDL) need explicit notification in this form when
        a resize event comes in.
        
        <B>Must not</B> destroy the underlying
        OpenGL context (i.e. textures, vertex buffers, and GPU programs
        must be preserved across calls).
     */
    virtual void notifyResize(int w, int h) = 0;

    virtual void setRelativeMousePosition(double x, double y) = 0;

    /** Relative to the window origin */
    virtual void setRelativeMousePosition(const Vector2& p) = 0;

    /** Checks to see if any events are waiting.  If there is an event,
        returns true and fills out the GEvent structure.  Otherwise
        returns false.  The caller is responsible for invoking GWindow::notifyResize
        with any resize events; the GWindow does not notify itself. */
    virtual bool pollEvent(GEvent& e) {
        (void)e;
        return false;
    }

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
     Joystick axes are normalized to the range [-1, 1] (but might not be calibrated...).
     Joystick axes should be in the starting order: X, Y, Z, Slider1, Slider2, rX, rY, rZ
     */
    virtual void getJoystickState(unsigned int stickNum, Array<float>& axis, Array<bool>& button) = 0;

    /** 
      Capture the keyboard and mouse focus, locking the mouse to the client area of this window.
     */
    virtual void setInputCapture(bool c) = 0;

    /** Returns the current state of input capture */
    virtual bool inputCapture() const = 0;

    virtual void setMouseVisible(bool b) = 0;

    virtual bool mouseVisible() const = 0;

    /** Windows for which this is true require a program
        to hand control of the main loop to GWindow::startMainLoop.
        The program's functionality may then be implemented through
        the "loop body" function.
    
        That is, if requiresMainLoop returns true, you must use
        the following structure:
        <PRE>
           void doEvents() {
              GEvent e;
              while (window->pollEvent(e)) {
                  ... // Event handling
              } 
           }

           void doGraphics() {
              renderDevice->beginFrame();
                  renderDevice->clear(true, true, true);
                  ...  // draw stuff
              renderDevice->endFrame();
           }

           void loopBody(void*) {
              // all per-frame code; event-handling, physics, networking, AI, etc.
              doEvents();
              doLogic();
              doNetwork();
              doAI();
              doGraphics();

              // To end the program, invoke window->popLoopBody
           }

           window->pushLoopBody(callback, NULL);
           window->runMainLoop(); // doesn't return
        </PRE>

        When requiresMainLoop returns false, you may use either the
        above structure or the following one (which puts you in more
        control of when graphics vs. other code is executed):

        <PRE>
            while (true) {
              doEvents();
              doLogic();
              doNetwork();
              doAI();
              doGraphics();
            }       
        </PRE>

        This design is necessary because some underlying Window APIs
        (e.g. ActiveX, GLUT) enforce an event-driven structure.
    */
    virtual bool requiresMainLoop() const {
        return false;
    }

    /** Pushes a function onto the stack of functions called by runMainLoop */
    virtual void pushLoopBody(void (*body)(void*), void* arg) {
        loopBodyStack.push(LoopBody(body, arg));
    }

    /** Invokes GApplet::beginRun after the applet is on the stack. */
    virtual void pushLoopBody(GApplet* applet);

    /** Pops a loop body off the stack.  If the loop body was a GApplet,
        invokes GApplet::endRun on it.*/
    virtual void popLoopBody();

    /**
     Executes an event loop, invoking callback repeatedly.  Put everything
     that you want to execute once per frame into the callback function.
     It is guaranteed safe to call pollEvents and all other GWindow methods
     from the callback function.
     
     The default implementation (for requiresMainLoop() == false GWindows)
     just calls the callback continuously.  Subclasses should use the
     notDone() and executeLoopBody() functions.
     */
    virtual void runMainLoop() {
        
        alwaysAssertM(requiresMainLoop() == false,
            "This GWindow implementation failed to overwrite runMainLoop "
            "even though it requires a main loop.");

        while (notDone()) {
            executeLoopBody();
        }
    }

private:
    /** Tracks the current GWindow.  If back-to-back calls are made to make
        the same GWindow current, they are ignored. */
    static const GWindow* m_current;

    friend class RenderDevice;

    class RenderDevice* m_renderDevice;

protected:

    GWindow() : m_renderDevice(NULL) {}

    /** Override this with the glMakeCurrent call appropriate for your window.*/
    virtual void reallyMakeCurrent() const {
    }
    

public:
    /** Closes the window and frees any resources associated with it.
        When subclassing, put any shutdown code (e.g. SDL_Quit()) in 
        your destructor.  Put initialization code (e.g. SDL_Init()) in
        the constructor. */
    virtual ~GWindow() {
        if (m_current == this) {
            // Once this window is destroyed, there will be no current context.
            m_current = NULL;
        }
    }

    /** 
      If a RenderDevice has been created and initialized for this window, 
      returns that renderDevice.  Otherwise returns NULL.
      */
    inline RenderDevice* renderDevice() const {
        return m_renderDevice;
    }

    inline static const GWindow* current() {
        return m_current;
    }
    /**
      Makes the OpenGL context of this window current.  If you have multiple
      windows, call this before rendering to one of them.
      <b>beta</b>
     */
    inline void makeCurrent() const {
        if (m_current != this) {
            reallyMakeCurrent();
            m_current = this;
        }
    }

};

}

#endif
