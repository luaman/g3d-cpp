/**
 @file Win32Window.h
  
 A GWindow that uses the Win32 API.

 @maintainer Morgan McGuire, matrix@graphics3d.com 
 @created 	  2004-05-21
 @edited  	  2004-05-21
    
 Copyright 2000-2002, Morgan McGuire.
 All rights reserved.
*/

#ifndef WINDOW_H
#define WINDOW_H

#include <G3DAll.h>
#include <windows.h>
#include <string>
#include <time.h>

const UINT BLIT_BUFFER = 0xC001;

class Win32Window : public GWindow {
private:
	
	GWindowSettings		 settings;
	int                  _width;
    int                  _height;
	std::string			 _title;
    HDC                  _hDC;
	HGLRC				 _glContext;
	bool				 _mouseVisible;
	bool				 _inputCapture;

	static LRESULT WINAPI window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

public:
	
	explicit Win32Window(const GWindowSettings& settings);
	
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
	
    virtual void setDimensions(const Rect2D& dims) {
		// TODO
	}
	
    virtual void setPosition(int x, int y) {
		// TODO
	}
	
    virtual bool hasFocus() const {
		// TODO
		return true;
	}
	
    virtual std::string getAPIVersion() const;
	
    virtual std::string getAPIName() const;
	
    virtual void setGammaRamp(const Array<uint16>& gammaRamp);
    
	virtual void setCaption(const std::string& caption);
	
    virtual int numJoysticks() const {
		// TODO
		return 0;
	}
	
    virtual std::string joystickName(unsigned int sticknum) {
		// TODO
		return "";
	}
	
    virtual std::string caption();
	
    virtual void setIcon(const GImage& image) {
		// TODO
	}
	
    virtual void swapGLBuffers();
	
    virtual void notifyResize(int w, int h);
	
    virtual void setRelativeMousePosition(double x, double y);
	
    virtual void setRelativeMousePosition(const Vector2& p);
	
    virtual bool pollEvent(GEvent& e);
	
    virtual void getRelativeMouseState(Vector2& position, uint8& mouseButtons) const;
	
    virtual void getRelativeMouseState(int& x, int& y, uint8& mouseButtons) const;
	
    virtual void getRelativeMouseState(double& x, double& y, uint8& mouseButtons) const;
	
    virtual void getJoystickState(unsigned int stickNum, Array<float>& axis, Array<bool>& button) {
		// TODO
	}
	
    virtual void setInputCapture(bool c);
	
    virtual bool inputCapture() const;
	
    virtual void setMouseVisible(bool b);
	
    virtual bool mouseVisible() const;
	
    virtual bool requiresMainLoop() const;
};

#endif