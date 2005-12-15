/**
 @file Win32Window.h
  
 A GWindow that uses the Win32 API.

 @maintainer Corey Taylor 
 @created 	  2004-05-21
 @edited  	  2005-09-21
    
 Copyright 2000-2005, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_WIN32WINDOW_H
#define G3D_WIN32WINDOW_H

#include "G3D/platform.h"

// This file is only used on Windows
#ifdef G3D_WIN32

#include "GLG3D/GWindow.h"
#include <windows.h>
#include <string>

namespace G3D {

// Forward declaration so directinput8.h is included in cpp
namespace _internal {
class _DirectInput;
}
using _internal::_DirectInput;

class Win32Window : public GWindow {
private:
	
    static Array<GWindowSettings> _supportedSettings;

	GWindowSettings		 settings;
	std::string			 _title;
    HDC                  _hDC;
	HGLRC				 _glContext;
	bool				 _mouseVisible;
	bool				 _inputCapture;
    bool                 _windowActive;

    bool                 _receivedCloseEvent;

    /** Mouse Button State Array: false - up, true - down
        [0] - left, [1] - middle, [2] - right */
    bool                 _mouseButtons[3];

    _DirectInput*        _diDevices;

    G3D::Set< int >      _usedIcons;

	/** Coordinates of the client area in screen coordinates */
	int				     clientX;
	int					 clientY;

	/** Only one thread allowed for use with Win32Window::makeCurrent */
	HANDLE				 _thread;

    Array<GEvent>        sizeEventInjects;

    void injectSizeEvent(int width, int height) {
        GEvent e;
        e.type = SDL_VIDEORESIZE;
        e.resize.w = width;
        e.resize.h = height;
        sizeEventInjects.append(e);
    }

    friend static LRESULT WINAPI window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	
    /** Called from all constructors */
	void init(HWND hwnd, bool creatingShareWindow = false);

	static Win32Window*			_shareWindow;	

	/** OpenGL technically does not allow sharing of resources between
	  multiple windows (although this tends to work most of the time
	  in practice), so we create an invisible HDC and context with which
	  to explicitly share all resources. 
	  
	  @param s The settings describing the pixel format of the windows with which
	  resources will be shared.  Sharing may fail if all windows do not have the
	  same format.*/ 
	static void createShareWindow(GWindowSettings s);

    /** Initializes the WGL extensions by creating and then destroying a window.  
        Also registers our window class.  
    
        It is necessary to create a dummy window to avoid a catch-22 in the Win32
        API: fsaa window creation is supported through a WGL extension, but WGL 
        extensions can't be called until after a window has already been created. */
    static void initWGL();
    
	/** Constructs from a new window */
	explicit Win32Window(const GWindowSettings& settings, bool creatingShareWindow = false);

	/** Constructs from an existing window */
	explicit Win32Window(const GWindowSettings& settings, HWND hwnd);

	/** Constructs from an existing window */
	explicit Win32Window(const GWindowSettings& settings, HDC hdc);

    HWND                 window;
	const bool		     createdWindow;

public:

    /** Different subclasses will be returned depending on
        whether DirectInput8 is available. You must delete 
        the window returned when you are done with it. */
    static Win32Window* create(const GWindowSettings& settings=GWindowSettings());

    static Win32Window* create(const GWindowSettings& settings, HWND hwnd);

    /** The HDC should be a private CS_OWNDC device context because it is assumed to
        be perisistant.*/
    static Win32Window* create(const GWindowSettings& settings, HDC hdc);

    /** Finds all of the compatible GWindowSettings supported by hardware.

        The width and height fields always refer to full-screen resolution.

        Only width, height, rgbBits, alphaBits, depthBits, stencilBits, fsaaSamples,
        stereo and refreshRate are valid. */
    static const Array<GWindowSettings>& SupportedWindowSettings();

    /** Finds the closest match to the desired GWindowSettings.

        The width and height fields always refer to full-screen resolution.

        Only width, height, rgbBits, alphaBits, depthBits, stencilBits, fsaaSamples,
        stereo and refreshRate are valid. */
    static bool ClosestSupportedWindowSettings(const GWindowSettings& desired, GWindowSettings& closest);
	
    virtual ~Win32Window();
	
	void close();
	
    inline HWND hwnd() const {
        return window;
    }

	inline HDC hdc() const {
		return _hDC;
	}

	void getSettings(GWindowSettings& settings) const;
	
    virtual int width() const;
	
    virtual int height() const;
	
    virtual Rect2D dimensions() const;
	
    virtual void setDimensions(const Rect2D& dims);
	
    virtual void setPosition(int x, int y) {
        setDimensions( Rect2D::xywh(x, y, settings.width, settings.height) );
	}
	
    virtual bool hasFocus() const;
	
    virtual std::string getAPIVersion() const;
	
    virtual std::string getAPIName() const;
	
    virtual void setGammaRamp(const Array<uint16>& gammaRamp);
    
	virtual void setCaption(const std::string& caption);
	
    virtual int numJoysticks() const;
	
    virtual std::string joystickName(unsigned int sticknum);
	
    virtual std::string caption();
	
    virtual void setIcon(const GImage& image);
	
    virtual void swapGLBuffers();
	
    virtual void notifyResize(int w, int h);
	
    virtual void setRelativeMousePosition(double x, double y);
	
    virtual void setRelativeMousePosition(const Vector2& p);
	
    virtual bool pollEvent(GEvent& e);
	
    virtual void getRelativeMouseState(Vector2& position, uint8& mouseButtons) const;
	
    virtual void getRelativeMouseState(int& x, int& y, uint8& mouseButtons) const;
	
    virtual void getRelativeMouseState(double& x, double& y, uint8& mouseButtons) const;
	
    virtual void getJoystickState(unsigned int stickNum, Array<float>& axis, Array<bool>& button);
	
    virtual void setInputCapture(bool c);
	
    virtual bool inputCapture() const;
	
    virtual void setMouseVisible(bool b);
	
    virtual bool mouseVisible() const;
	
    virtual bool requiresMainLoop() const;

protected:
    virtual void reallyMakeCurrent() const;
};


} // namespace G3D


#endif // G3D_WIN32

#endif // G3D_WIN32WINDOW_H
