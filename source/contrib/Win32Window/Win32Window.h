/**
 @file Win32Window.h
  
 A GWindow that uses the Win32 API.

 @maintainer Corey Taylor 
 @created 	  2004-05-21
 @edited  	  2004-10-12
    
 Copyright 2000-2002, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_WIN32WINDOW_H
#define G3D_WIN32WINDOW_H

#include <G3DAll.h>

#ifdef G3D_WIN32

#include <windows.h>
#include <string>
#include <time.h>


namespace G3D {

// Forward declaration so friend works
class Win32APIWindow;

class Win32Window : public GWindow {
private:
	
    friend Win32APIWindow;

	GWindowSettings		 settings;
	int                  _width;
    int                  _height;
	std::string			 _title;
    HDC                  _hDC;
	HGLRC				 _glContext;
	bool				 _mouseVisible;
	bool				 _inputCapture;

    /** Mouse Button State Array: false - up, true - down
        [0] - left, [1] - middle, [2] - right */
    bool                 _mouseButtons[3];

    G3D::Queue< GEvent > _keyboardEvents;

    G3D::Set< int >      _usedIcons;

	/** Coordinates of the client area in screen coordinates */
	int				     clientX;
	int					 clientY;

	static LRESULT WINAPI window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

	/** Called from both constructors */
	void init(HWND hwnd);

    /** Initializes the WGL extensions by creating and then destroying a window.  
        Also registers our window class.  
    
        It is necessary to create a dummy window to avoid a catch-22 in the Win32
        API: fsaa window creation is supported through a WGL extension, but WGL 
        extensions can't be called until after a window has already been created. */
    static void initWGL();

    // Special private constuctor for Win32APIWindow
    explicit Win32Window() {}

public:

	/** Constructs from a new window */
	explicit Win32Window(const GWindowSettings& settings);

	/** Constructs from an existing window */
	Win32Window(const GWindowSettings& settings, HWND hwnd);
	
    /** Detects where DirectInput8 is available or not
        and uses Win32 API keyboard input appropriately. */
    static Win32Window* createBestWindow(const GWindowSettings& settings);

	virtual ~Win32Window();
	
	void close();
	
	HWND                 window;
	
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
};


class Win32APIWindow : public Win32Window {

public:

    Win32APIWindow(const GWindowSettings& settings);

    ~Win32APIWindow();

    virtual bool pollEvent(GEvent& e);

    virtual int numJoysticks() const {
        return 0;
    }
	
    virtual std::string joystickName(unsigned int sticknum) {
        return std::string("");
    }

    virtual void getJoystickState(unsigned int stickNum, Array<float>& axis, Array<bool>& button) {
        axis.clear();
        button.clear();
        axis.resize(0);
        button.resize(0);
    }
    
};


} // namespace G3D

using G3D::Win32Window; // temporary backwards compatibility.
using G3D::Win32APIWindow;

#endif // G3D_WIN32

#endif // G3D_WIN32WINDOW_H
