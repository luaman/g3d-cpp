/**

  @author Corey Taylor, corey@acz.org

  @created 2004-02-25

  */

#ifndef __WXGWINDOW__H__
#define __WXGWINDOW__H__

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <wx/joystick.h>

#include <G3DAll.h>

#include "main.h"

DECLARE_APP(AlterApp);

class wxGWindow: public GWindow
{
public:
	wxGWindow(GWindowSettings &settings);
	virtual ~wxGWindow();
	virtual void getSettings(GWindowSettings &settings) const; 
	virtual int width() const; 
	virtual int height() const; 
	virtual Rect2D dimensions() const; 
	virtual void setDimensions(const Rect2D &dims); 
	virtual void setPosition(int x, int y) const; 
	virtual bool hasFocus() const; 
	virtual std::string getAPIVersion() const; 
	virtual std::string getAPIName() const; 
	virtual void setGammaRamp(const Array< uint16 > &gammaRamp); 
	virtual void setCaption(const std::string &caption); 
	virtual int numJoysticks() const; 
	virtual Vector2 joystickPosition(int stickNum) const; 
	virtual std::string caption(); 
	virtual void setIcon(const GImage &image); 
	virtual void swapGLBuffers(); 
	virtual void notifyResize(int w, int h); 
	virtual void setRelativeMousePosition(double x, double y); 
	virtual void setRelativeMousePosition(const Vector2 &p); 
	virtual void getRelativeMouseState(Vector2 &position, uint8 &mouseButtons) const; 
	virtual void getRelativeMouseState(int &x, int &y, uint8 &mouseButtons) const; 
	virtual void getRelativeMouseState(double &x, double &y, uint8 &mouseButtons) const; 
	virtual void setMouseCapture(bool c); 
	virtual bool mouseCapture() const; 
	virtual void setMouseVisible(bool b); 
	virtual bool mouseVisible() const; 	

private:
	class wxGWnd_int: public wxGLCanvas{
	public:
		wxGWnd_int(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxPanelNameStr,  int* attribList = 0, const wxPalette& palette = wxNullPalette)
			:wxGLCanvas(parent, id, pos, size, style, name, attribList, palette)
			,leftMouseDown(false)
			,middleMouseDown(false)
			,rightMouseDown(false){ }
		~wxGWnd_int(){ }
		void onLeftDown(wxMouseEvent& event){ leftMouseDown = true;}
		void onLeftUp(wxMouseEvent& event){ leftMouseDown = false;}
		void onMiddleDown(wxMouseEvent& event){ middleMouseDown = true;}
		void onMiddleUp(wxMouseEvent& event){ middleMouseDown = false;}
		void onRightDown(wxMouseEvent& event){ rightMouseDown = true;}
		void onRightUp(wxMouseEvent& event){ rightMouseDown = false;}
		void onPaint(wxIdleEvent& event){ 
//			wxPaintDC pdc(this);
//			event.RequestMore(true);
			wxGetApp().Render();
			Sleep(10);
		}
		void onMouseEvent(wxMouseEvent& event){ wxGetApp().OnMouseEvent(event); }
		void onEraseBackground(wxEraseEvent& event){}
		bool leftMouseDown;
		bool middleMouseDown;
		bool rightMouseDown;
//	private:
		DECLARE_EVENT_TABLE()
	};

	wxGWnd_int* window;

	wxJoystick* joystick1;
	wxJoystick* joystick2;
	int joystickCount;

	GWindowSettings settings;
};


#endif