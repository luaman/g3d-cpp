/**
  @file UserInput.cpp
 
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-09-29
  @edited  2004-01-01
 */

#include "GLG3D/UserInput.h"

namespace G3D {

bool UserInput::appHasFocus() const {
    uint8 s = SDL_GetAppState();
    
    return ((s & SDL_APPMOUSEFOCUS) != 0) &&
           ((s & SDL_APPINPUTFOCUS) != 0) &&
           ((s & SDL_APPACTIVE) != 0);
}


UserInput::UserInput(
    Table<KeyCode, UIFunction>* keyMapping) {

    keyState.resize(SDL_CUSTOM_LAST);
    keyFunction.resize(keyState.size());

	up = left = down = right = false;
	jx = jy = 0.0;

    inEventProcessing = false;

    bool tempMapping = (keyMapping == NULL);

    if (tempMapping) {
        keyMapping = new Table<KeyCode, UIFunction>();
        keyMapping->set(SDLK_RIGHT, RIGHT);
        keyMapping->set(SDLK_LEFT, LEFT);
        keyMapping->set(SDLK_UP, UP);
        keyMapping->set(SDLK_DOWN, DOWN);
        keyMapping->set(SDLK_d, RIGHT);
        keyMapping->set(SDLK_a, LEFT);
        keyMapping->set(SDLK_w, UP);
        keyMapping->set(SDLK_s, DOWN);
    }

    setKeyMapping(keyMapping);

    if (tempMapping) {
        delete keyMapping;
        keyMapping = NULL;
    }

	// Check for joysticks
    int j = SDL_NumJoysticks();
    if ((j < 0) || (j > 10)) {
        // If there is no joystick adapter on Win32,
        // SDL returns ridiculous numbers.
        j = 0;
    }

	useJoystick = (j > 0);

	if (useJoystick) {
        SDL_JoystickEventState(SDL_ENABLE);
        // Turn on joystick 0
        joy = SDL_JoystickOpen(0);
        debugAssert(joy);
	}
}


void UserInput::setKeyMapping(
    Table<KeyCode, UIFunction>* keyMapping) {

    for (int i = keyState.size() - 1; i >= 0; --i) {
        keyState[i]    = false;
        if (keyMapping->containsKey(i)) {
            keyFunction[i] = keyMapping->get(i);
        } else {
            keyFunction[i] = NONE;
        }
    }
}


UserInput::~UserInput() {
	// Close joystick, if opened
	if (SDL_JoystickOpened(0)) {
  		SDL_JoystickClose(joy);
		joy = NULL;
	}
}


void UserInput::processEvent(const SDL_Event& event) {
    
    debugAssert(inEventProcessing);
    // Translate everything into a key code then call processKey

	switch(event.type) {
	case SDL_KEYUP:
		processKey(event.key.keysym.sym, SDL_KEYUP);
		break;

	case SDL_KEYDOWN:
		processKey(event.key.keysym.sym, SDL_KEYDOWN);
        break;

    case SDL_MOUSEBUTTONDOWN:
        processKey(SDL_LEFT_MOUSE_KEY + (event.button.button - 1), SDL_KEYDOWN);
        break;
        
    case SDL_MOUSEBUTTONUP:
        processKey(SDL_LEFT_MOUSE_KEY + (event.button.button - 1), SDL_KEYUP);
        break;
    }
}


void UserInput::beginEvents() {
    debugAssert(! inEventProcessing);
    inEventProcessing = true;
    justPressed.resize(0, DONT_SHRINK_UNDERLYING_ARRAY);
}


void UserInput::endEvents() {
    debugAssert(inEventProcessing);

    inEventProcessing = false;
    if (useJoystick) {
        jx =  SDL_JoystickGetAxis(joy, 0) / 32768.0;
        jy = -SDL_JoystickGetAxis(joy, 1) / 32768.0;
    }

    mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
}


double UserInput::getX() const {

	if (left && !right) {
		return -1.0;
	} else if (right && !left) {
		return 1.0;
	}

	if (useJoystick && (fabs(jx) > 0.1)) {
		return jx;
	}

	return 0.0;
}


double UserInput::getY() const {
	if (down && !up) {
		return -1.0;
	} else if (up && !down) {
		return 1.0;
	}

	if (useJoystick && (fabs(jy) > 0.1)) {
		return jy;
	}

	return 0.0;
}


void UserInput::processKey(KeyCode code, int event) {
	bool state = (event == SDL_KEYDOWN);

    if (code < keyFunction.size()) {
        switch (keyFunction[code]) {
        case RIGHT:
            right = state;
            break;

        case LEFT:
            left = state;
            break;

        case UP:
            up = state;
            break;

        case DOWN:
            down = state;
            break;

        case NONE:
            break;
	    }

        keyState[code] = state;

        if (state) {
            justPressed.append(code);
        }
    }
}


void UserInput::setMouseXY(double x, double y) {
    mouseX = iRound(x);
    mouseY = iRound(y);
    SDL_WarpMouse(mouseX, mouseY);
}


int UserInput::getNumJoysticks() const {
	return SDL_NumJoysticks();
}


bool UserInput::keyDown(KeyCode code) const {
    if (code > SDL_CUSTOM_LAST) {
        return false;
    } else {
        return keyState[code];
    }
}


bool UserInput::keyPressed(KeyCode code) const {
    for (int i = justPressed.size() - 1; i >= 0; --i) {
        if (code == justPressed[i]) {
            return true;
        }
    }

    return false;
}


void UserInput::pressedKeys(Array<KeyCode>& code) const {
    code.resize(justPressed.size());
    memcpy(code.getCArray(), justPressed.getCArray(), sizeof(UserInput::KeyCode) * justPressed.size());
}


bool UserInput::anyKeyPressed() const {
    return (justPressed.size() > 0);
}


UserInput::KeyCode UserInput::stringToKeyCode(const std::string& _s) {
    std::string s = trimWhitespace(toLower(_s));    

    for (int i = 0; i < SDL_CUSTOM_LAST; ++i) {
        std::string t = keyCodeToString(i);
        if ((t.size() == s.size()) &&
            (toLower(t) == s)) {
            return i;
        }
    }

    return 0;
}


std::string UserInput::keyCodeToString(KeyCode i) {
    if (i == SDL_LEFT_MOUSE_KEY) {
        return "L Mouse";
    } else if (i == SDL_MIDDLE_MOUSE_KEY) {
        return "Mid Mouse";
    } else if (i == SDL_RIGHT_MOUSE_KEY) {
        return "R Mouse";
    } else if (i == SDL_MOUSE_WHEEL_UP_KEY) {
        return "MWheel Up";
    } else if (i == SDL_MOUSE_WHEEL_DOWN_KEY) {
        return "MWheel Dn";
    }

    switch (i) {
    case SDLK_BACKSPACE:
        return "Bksp";
        
    case SDLK_TAB:
        return "Tab";

    case SDLK_CLEAR:
        return "Clear";
        
    case SDLK_RETURN:
        return "Enter";

    case SDLK_PAUSE:
        return "Pause";

    case SDLK_ESCAPE:
        return "Esc";

    case SDLK_SPACE:
        return "Spc";

    case SDLK_EXCLAIM:
    case SDLK_QUOTEDBL:
    case SDLK_HASH:
    case SDLK_DOLLAR:
    case SDLK_AMPERSAND:
    case SDLK_QUOTE:
    case SDLK_LEFTPAREN:
    case SDLK_RIGHTPAREN:
    case SDLK_ASTERISK:
    case SDLK_PLUS:
    case SDLK_COMMA:
    case SDLK_MINUS:
    case SDLK_PERIOD:
    case SDLK_SLASH:
    case SDLK_0:
    case SDLK_1:
    case SDLK_2:
    case SDLK_3:
    case SDLK_4:
    case SDLK_5:
    case SDLK_6:
    case SDLK_7:
    case SDLK_8:
    case SDLK_9:
    case SDLK_COLON:
    case SDLK_SEMICOLON:
    case SDLK_LESS:
    case SDLK_EQUALS:
    case SDLK_GREATER:
    case SDLK_QUESTION:
    case SDLK_AT:
    case SDLK_LEFTBRACKET:
    case SDLK_BACKSLASH:
    case SDLK_RIGHTBRACKET:
    case SDLK_CARET:
    case SDLK_UNDERSCORE:
    case SDLK_BACKQUOTE:
    case SDLK_a:
    case SDLK_b:
    case SDLK_c:
    case SDLK_d:
    case SDLK_e:
    case SDLK_f:
    case SDLK_g:
    case SDLK_h:
    case SDLK_i:
    case SDLK_j:
    case SDLK_k:
    case SDLK_l:
    case SDLK_m:
    case SDLK_n:
    case SDLK_o:
    case SDLK_p:
    case SDLK_q:
    case SDLK_r:
    case SDLK_s:
    case SDLK_t:
    case SDLK_u:
    case SDLK_v:
    case SDLK_w:
    case SDLK_x:
    case SDLK_y:
    case SDLK_z:
        return std::string("") + (char)toupper(i);

    case SDLK_DELETE:
        return "Del";

    case SDLK_KP0:
    case SDLK_KP1:
    case SDLK_KP2:
    case SDLK_KP3:
    case SDLK_KP4:
    case SDLK_KP5:
    case SDLK_KP6:
    case SDLK_KP7:
    case SDLK_KP8:
    case SDLK_KP9:
        return std::string("Keypad ") + (char)('0' + (i - SDLK_KP0)); 

    case SDLK_KP_PERIOD:
        return "Keypad .";

    case SDLK_KP_DIVIDE:
        return "Keypad \\";

    case SDLK_KP_MULTIPLY:
        return "Keypad *";

    case SDLK_KP_MINUS:
        return "Keypad -";

    case SDLK_KP_PLUS:
        return "Keypad +";

    case SDLK_KP_ENTER:
        return "Keypad Enter";

    case SDLK_KP_EQUALS:
        return "Keypad =";

    case SDLK_UP:
        return "Up";

    case SDLK_DOWN:
        return "Down";

    case SDLK_RIGHT:
        return "Right";

    case SDLK_LEFT:
        return "Left";

    case SDLK_INSERT:
        return "Ins";

    case SDLK_HOME:
        return "Home";

    case SDLK_END:
        return "End";

    case SDLK_PAGEUP:
        return "Pg Up";

    case SDLK_PAGEDOWN:
        return "Pg Dn";

    case SDLK_F1:
    case SDLK_F2:
    case SDLK_F3:
    case SDLK_F4:
    case SDLK_F5:
    case SDLK_F6:
    case SDLK_F7:
    case SDLK_F8:
    case SDLK_F9:
    case SDLK_F10:
    case SDLK_F11:
    case SDLK_F12:
    case SDLK_F13:
    case SDLK_F14:
    case SDLK_F15:
        return std::string("F") + (char)('1' + (i - SDLK_F1));

    case SDLK_NUMLOCK:
        return "Num Lock";

    case SDLK_CAPSLOCK:
        return "Caps Lock";

    case SDLK_SCROLLOCK:
        return "Scroll Lock";

    case SDLK_RSHIFT:
        return "R Shft";
        
    case SDLK_LSHIFT:
        return "L Shft";

    case SDLK_RCTRL:
        return "R Ctrl";

    case SDLK_LCTRL:
        return "L Ctrl";

    case SDLK_RALT:
        return "R Alt";

    case SDLK_LALT:
        return "L Alt";

    case SDLK_RMETA:
        return "R Meta";

    case SDLK_LMETA:
        return "L Meta";

    case SDLK_LSUPER:
        return "L Win";

    case SDLK_RSUPER:
        return "R Win";

    case SDLK_MODE:
        return "Alt Gr";

    case SDLK_HELP:
        return "Help";

    case SDLK_PRINT:
        return "Print";

    case SDLK_SYSREQ:
        return "Sys Req";

    case SDLK_BREAK:
        return "Break";
    }

    return "";
}

}
