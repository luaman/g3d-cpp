/**
  @file UserInput.h
 
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-09-28
  @edited  2004-02-28
 */

#ifndef G3D_USERINPUT_H
#define G3D_USERINPUT_H

#include "G3D/platform.h"

#if defined(G3D_OSX)
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "graphics3D.h"
#include "GLG3D/GWindow.h"

namespace G3D {

/**
 Some special key codes for use with UserInput.
 */
// In an enum so they can be used with switch.
enum CustomKeyCode {

    // The order of the mouse buttons is intentionally chosen to match SDL's button codes
    // and cannot be changed.
    SDL_LEFT_MOUSE_KEY        = 324,
    SDL_MIDDLE_MOUSE_KEY,
    SDL_RIGHT_MOUSE_KEY,
    SDL_MOUSE_WHEEL_UP_KEY,
    SDL_MOUSE_WHEEL_DOWN_KEY,

    SDL_CUSTOM_LAST};


/**
 User input class that consolidates joystick, keyboard, and mouse input.

 Four axes are supported directly: joystick/keyboard x and y and 
 mouse x and y.  Mouse buttons, joystick buttons, and keyboard keys
 can all be used as generic buttons.

 Call beginEvents() immediately before your SDL event handling routine and hand
 events to processEvent() as they become available.  Call endEvents() immediately
 after the loop.

 <PRE>
    ::SDL_Event event;

    userInput->beginEvents();
    while (SDL_PollEvent(&event)) {
        userInput->processEvent(event);

        switch (event.type) {
        case SDL_QUIT:
            exit(0);
            break;
        }
    }
    userInput->endEvents();
   </PRE>

    
    Note that UserInput uses the SDLEvent structures (defined in SDL.h), but not
    the SDL <B>library</B>.  This means you can write your own G3D::GWindow subclass 
    that translates platform events into SDL_Events and not use SDL.dll at all.  
*/
class UserInput {
public:
    typedef uint16 KeyCode;

    enum UIFunction {UP, DOWN, LEFT, RIGHT, NONE};

private:

    GWindow*                _window;

    /**
     keyState[x] is true if key[x] is depressed.
     */
    Array<bool>             keyState;

    /**
      All keys that were just pressed down since the last call to
      poll().
     */
    // Since relatively few keys are pressed every frame, keeping an array of
    // key codes pressed is much more compact than clearing a large array of bools.
    Array<KeyCode>          justPressed;

    /**
     Function of key[x]
     */
    Array<UIFunction>       keyFunction;

    bool                    inEventProcessing;

    /** Called from the constructors */
    void init(GWindow* window, Table<KeyCode, UIFunction>* keyMapping);

    /** Called from setPureDeltaMouse */
    void grabMouse();

    /** Called from setPureDeltaMouse */
    void releaseMouse();

    /** Position of the mouse before setPureDeltaMouse = true.
        This position is returned by mouseXY. */
    Vector2                 guiMouse;

    /** Center of the window.  Recomputed in endEvents. */
    Vector2                 windowCenter;

    /** True if appHasFocus was true on the previous call to endEvents.
        Updated during endEvents. */
    bool                    appHadFocus;

    bool                    _pureDeltaMouse;
    Vector2                 deltaMouse;

	/** Whether each direction key is up or down.*/
	bool                    left;
	bool                    right;
	bool                    up;
	bool                    down;

    uint8                   mouseButtons;

    /**
     Joystick x, y
     */
	double                  jx;
	double                  jy;

    /**
     In pixels
     */
    Vector2                 mouse;

    /**
     Expects SDL_MOUSEBUTTONDOWN, etc. to be translated into key codes.
     */
	void processKey(KeyCode code, int event);

public:
    /**
     Turns a UserInput key code into a human readable string
     describing the key.
     */
    static std::string keyCodeToString(KeyCode i);

    /**
     Inverse of keyCodeToString
     */
    static KeyCode stringToKeyCode(const std::string& s);

	bool                    useJoystick;
    
	/**
     @param keyMapping Mapping of various key codes to UI functions.
     If no mapping is provided, the arrow keys and WASD are mapped
     as the keys controlling getX() and getY().

     Example:
      <PRE>
      Table<int, UIFunction> map;
      map.set(SDLK_RIGHT, UserInput::RIGHT);
      map.set(SDLK_LEFT,  UserInput::LEFT);
      UserInput ui(&map);
      </PRE>

      @deprecated The keyMapping argument is deprecated
	 */
    UserInput(GWindow* window, Table<KeyCode, UIFunction>* keyMapping = NULL);

    /**
     Provided for 6.00 compatibility.  Finds the last RenderDevice created
     and uses the window from it.

	 Do not call until after G3D::RenderDevice::init has been invoked.
     @deprecated
     */
    UserInput(Table<KeyCode, UIFunction>* keyMapping);

    /**
     Provided for 6.00 compatibility.  Finds the last RenderDevice created
     and uses the window from it.

	 Do not call until after G3D::RenderDevice::init has been invoked.
     @deprecated
     */
    UserInput();

    /**
     Return the window used internally by the UserInput
     */
    GWindow* window() const;

    void setKeyMapping(Table<KeyCode, UIFunction>* keyMapping = NULL);

	/**
	 Closes the joystick if necessary.
	 */
	virtual ~UserInput();

	/**
	 Call from inside the event loop for every event inside
	 processEvents() (done for you by App3D.processEvents())
	 */
    void processEvent(const ::SDL_Event& event);

	/**
     Call after your SDL event polling loop.
	 */
	void endEvents();

    /**
     Call before your SDL event polling loop.
     */
	void beginEvents();

    /**
     Sets the mouse position.  That new position will
     be returned by getMouseXY until the next endEvents()
     statement.
     */
    void setMouseXY(double x, double y);

    inline void setMouseXY(const Vector2& v) {
        setMouseXY(v.x, v.y);
    }

	int getNumJoysticks() const;

	/**
	 Returns a number between -1 and 1 indicating the horizontal
	 input from the user.  Keyboard overrides joystick.
     @deprecated
     Use ManualCameraControllerHelper
	 */
	double getX() const;

	/**
	 Returns a number between -1 and 1 indicating the vertical
	 input from the user.  Up is positive, down is negative. 
	 Keyboard overrides joystick.
     @deprecated
     Use ManualCameraControllerHelper
	 */
	double getY() const;

    /**
     @deprecated
     Use ManualCameraControllerHelper
     */
    Vector2 getXY() const {
        return Vector2(getX(), getY());
    }

    inline Vector2 mouseXY() const {
        return guiMouse;
    }

    /** @deprecated */
    inline Vector2 getMouseXY() const {
        return guiMouse;
    }

    /** @deprecated */
	inline double getMouseX() const {
		return guiMouse.x;
	}

    /** @deprecated */
	inline double getMouseY() const {
		return guiMouse.y;
	}

    /**
     Returns true iff the given key is currently held down.
     The SDL key codes are used, plus
        SDL_LEFT_MOUSE_KEY,
        SDL_MIDDLE_MOUSE_KEY,
        SDL_RIGHT_MOUSE_KEY,
        SDL_MOUSE_WHEEL_UP_KEY,
        SDL_MOUSE_WHEEL_DOWN_KEY
     */
    bool keyDown(KeyCode code) const;

    /**
     Returns true if this key went down since the last call to
     poll().
     */
    bool keyPressed(KeyCode code) const;

    /**
     True if any key has been pressed since the last call to poll().
     */
    bool anyKeyPressed() const;

    /** An array of all keys pressed since the last poll() call. */
    void pressedKeys(Array<KeyCode>& code) const;

    /** Returns true when this app is in the "foreground" 
        (shortcut for window()->hasFocus()).
        @deprecated*/
    bool appHasFocus() const;


    Vector2 mouseDXY() const;
    double mouseDX() const;
    double mouseDY() const;

    /**
     When set to true, the mouse cursor is invisible and
     the mouse is restricted to the current window.  Regardless
     of how far the user moves the mouse in one direction,
     continous mouseDXY values are returned.  
     
     The mouseXY values are not particularly useful in this mode.

     This setting is commonly used by first-person games.

     When the app loses focus the mouse is automatically freed
     and recaptured when focus returns.  debugBreak also frees
     the mouse.
     */
    void setPureDeltaMouse(bool m);
    bool pureDeltaMouse() const;
};

}


#endif
