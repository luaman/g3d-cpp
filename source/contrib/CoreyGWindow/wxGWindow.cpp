/**

  @author Corey Taylor, corey@acz.org
  @created 2004-02-25
  
  */
#include "wxGWindow.h"


BEGIN_EVENT_TABLE(wxGWindow::wxGWnd_int, wxWindow)
//	EVT_LEFT_DOWN(wxGWindow::wxGWnd_int::onLeftDown)
//	EVT_LEFT_UP(wxGWindow::wxGWnd_int::onLeftUp)
//	EVT_MIDDLE_DOWN(wxGWindow::wxGWnd_int::onMiddleDown)
//	EVT_MIDDLE_UP(wxGWindow::wxGWnd_int::onMiddleUp)
//	EVT_RIGHT_DOWN(wxGWindow::wxGWnd_int::onRightDown)
//	EVT_RIGHT_UP(wxGWindow::wxGWnd_int::onRightUp)
	EVT_IDLE(wxGWindow::wxGWnd_int::onPaint)
//	EVT_ERASE_BACKGROUND(wxGWindow::wxGWnd_int::onEraseBackground)
//	EVT_MOUSE_EVENTS(wxGWindow::wxGWnd_int::onMouseEvent)
END_EVENT_TABLE()


wxGWindow::wxGWindow(GWindowSettings &settings)
{
    wxFrame *tmp = new wxFrame(NULL, -1, _T("Minimal wxWindows App"),
                                 wxPoint(50, 50), wxSize(850, 650));
	tmp->Show(true);

	this->settings = settings;
	long style = wxDEFAULT_FRAME_STYLE;//wxDEFAULT_FRAME_STYLE^wxSYSTEM_MENU;
//	if( !settings.resizable ){
//		style ^= ~wxRESIZE_BORDER;
//		settings.resizable = false;
//	}

	joystick1 = new wxJoystick;
	joystick2 = new wxJoystick(wxJOYSTICK2);
	joystickCount = joystick1->GetNumberJoysticks();

	int attribs[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_LEVEL, 0
		, settings.stereo?WX_GL_STEREO:0
		, WX_GL_DEPTH_SIZE, settings.depthBits
		, WX_GL_STENCIL_SIZE, settings.stencilBits
		, WX_GL_MIN_ALPHA, settings.alphaBits
		, WX_GL_MIN_RED, settings.rgbBits
		, WX_GL_MIN_GREEN, settings.rgbBits
		, WX_GL_MIN_BLUE, settings.rgbBits};

	window = new wxGWnd_int(tmp, -1, wxPoint(0,0), wxSize(settings.width, settings.height)
		, style
		, wxString("wxGWindow Frame")
		, attribs
		, wxNullPalette);
	window->Show(true);
	window->SetCurrent();
	window->SwapBuffers();
	window->CaptureMouse();
}

wxGWindow::~wxGWindow()
{
	delete window;
}

void wxGWindow::getSettings(GWindowSettings &settings) const
{
	settings = this->settings;
} 

int wxGWindow::width() const
{
	return settings.width;
}

int wxGWindow::height() const
{
	return settings.height;
}

Rect2D wxGWindow::dimensions() const
{
	const wxRect dim = window->GetRect();
	return Rect2D::xywh(dim.x, dim.y, dim.width, dim.height);
} 

void wxGWindow::setDimensions(const Rect2D &dims)
{
	wxRect dim;
	dim.x = dims.x0();
	dim.y = dims.y0();
	dim.width = dims.width();
	dim.height = dims.height();
	window->SetSize(dim);
}

void wxGWindow::setPosition(int x, int y) const
{
	debugAssert( x < 0 );
	debugAssert( y < 0 );
	window->Move(x, y);
}

bool wxGWindow::hasFocus() const
{
	//keyboard focus
	if( window != NULL )
		return (wxWindow::FindFocus() == window);
	else
		return false;
} 

std::string wxGWindow::getAPIVersion() const
{
	return std::string("empty");
}

std::string wxGWindow::getAPIName() const
{
	return std::string("wxGWindow");
}

void wxGWindow::setGammaRamp(const Array< uint16 > &gammaRamp)
{
}

void wxGWindow::setCaption(const std::string &caption)
{
	window->SetTitle(wxString(caption.c_str()));
}

int wxGWindow::numJoysticks() const
{
	return joystickCount;
}

Vector2 wxGWindow::joystickPosition(int stickNum) const
{
	debugAssert( stickNum < 1 );
	
	Vector2 joyPosition;
	if( (stickNum <= ((joystickCount > 2)?2:joystickCount)) && (stickNum > 0) ){
		wxPoint pos;
		switch(stickNum){
		case 1:
			pos = joystick1->GetPosition();
			joyPosition.x = pos.x;
			joyPosition.y = pos.y;
			break;
		case 2:
			pos = joystick1->GetPosition();
			joyPosition.x = pos.x;
			joyPosition.y = pos.y;
			break;
		}
	}

	return joyPosition;
}

std::string wxGWindow::caption()
{
	//Force conversion
	return std::string((const char*)window->GetTitle());
}

void wxGWindow::setIcon(const GImage &image)
{
//	window->SetIcon(wxIcon(image.byte(), image.width, image.height, image.channels*8));
}

void wxGWindow::swapGLBuffers()
{
	window->SwapBuffers();
}

void wxGWindow::notifyResize(int w, int h)
{
	debugAssert( w < 0 );
	debugAssert( h < 0 );
	settings.width = w;
	settings.height = h;
}

void wxGWindow::setRelativeMousePosition(double x, double y)
{
	window->WarpPointer(x, y);
}

void wxGWindow::setRelativeMousePosition(const Vector2 &p)
{
	window->WarpPointer(p.x, p.y);
}

void wxGWindow::getRelativeMouseState(Vector2 &position, uint8 &mouseButtons) const
{
	int ix, iy;
	getRelativeMouseState(ix, iy, mouseButtons);
	position.x = ix;
	position.y = iy;
}

void wxGWindow::getRelativeMouseState(int &x, int &y, uint8 &mouseButtons) const
{
	wxPoint pos = wxGetMousePosition();
	mouseButtons = (((window->rightMouseDown?1:0)<<2) | ((window->middleMouseDown?1:0)<<1) | (window->leftMouseDown?1:0))&0x7;
}

void wxGWindow::getRelativeMouseState(double &x, double &y, uint8 &mouseButtons) const
{
	int ix, iy;
	getRelativeMouseState(ix, iy, mouseButtons);
	x = (double)ix;
	y = (double)iy;
}

void wxGWindow::setMouseCapture(bool c)
{
//	if( c )
//		window->CaptureMouse();
//	else
//		window->ReleaseMouse();
}

bool wxGWindow::mouseCapture() const
{
	return window->HasCapture();
}

void wxGWindow::setMouseVisible(bool b)
{
} 

bool wxGWindow::mouseVisible() const
{
	return true;
}
