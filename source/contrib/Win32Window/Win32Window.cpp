/**
  @file Win32Window.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com 
  @cite       Special thanks to Max McGuire of Ironlore
  @created 	  2004-05-21
  @edited  	  2004-10-07
    
  Copyright 2000-2004, Morgan McGuire.
  All rights reserved.
*/

#include "Win32Window.h"
#ifndef G3D_WIN32
    #error This is a Win32-only file
#endif
#include <time.h>
#include <crtdbg.h>

#define WIN32WINDOW_INCLUDE_DI8
#include "Win32Window_di8.cpp"
#undef WIN32WINDOW_INCLUDE_DI8

using namespace Win32Window_DI8;

// Handle these interfaces manually instead of using ATL
Win32Window_DI8::IDirectInput8A*                di8Interface = NULL;

// It is pointless to try and keep these in the class
// when everything that initializes them is global
typedef struct {
    Win32Window_DI8::IDirectInputDevice8A*      device;
    std::string                                 name;
    bool                                        valid;
    unsigned int                                numAxes;
    unsigned int                                numButtons;
} di8InterfaceNamePair;

Array< di8InterfaceNamePair >                   joysticks;
int                                             joystickCount;


#define WGL_SAMPLE_BUFFERS_ARB	0x2041
#define WGL_SAMPLES_ARB		    0x2042

static PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;


// Handle the DirectInput8 joystick enumeration
BOOL CALLBACK EnumDirectInput8Joysticks(Win32Window_DI8::LPCDIDEVICEINSTANCEA lpddi, LPVOID pvRef) {
    
    joysticks.resize(joysticks.size() + 1, DONT_SHRINK_UNDERLYING_ARRAY);
    joysticks[joystickCount].name = lpddi->tszInstanceName;

    if (di8Interface->CreateDevice(lpddi->guidInstance, &joysticks[joystickCount].device, NULL) == S_OK) {
        Win32Window_DI8::IDirectInputDevice8A* device = joysticks[joystickCount].device;

        joysticks[joystickCount].valid = true;
        
        // Setup device and retreive axis/button count
        device->SetCooperativeLevel((HWND)pvRef, (Win32Window_DI8::DISCL_FOREGROUND | Win32Window_DI8::DISCL_NONEXCLUSIVE));

        DIDEVCAPS joystickCaps;
        joystickCaps.dwSize = sizeof(DIDEVCAPS);
        
        device->GetCapabilities(&joystickCaps);
        joysticks[joystickCount].numAxes = joystickCaps.dwAxes;
        joysticks[joystickCount].numButtons = joystickCaps.dwButtons;

        device->SetDataFormat(&Win32Window_DI8::G3DJOYDF);
    } else {
        joysticks[joystickCount].device = NULL;
    }

    joystickCount++;
    return true;
}

/** Changes the screen resolution */
static bool ChangeResolution(int width, int height, int bpp, int refreshRate) {

	if (refreshRate == 0) {
		refreshRate = 85;
	}

    DEVMODE deviceMode;

    ZeroMemory(&deviceMode, sizeof(DEVMODE));

    deviceMode.dmSize       = sizeof(DEVMODE);
	deviceMode.dmPelsWidth  = width;
	deviceMode.dmPelsHeight = height;
	deviceMode.dmBitsPerPel = bpp;
	deviceMode.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
    deviceMode.dmDisplayFrequency = refreshRate;

    LONG result = ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);

    if (result != DISP_CHANGE_SUCCESSFUL) {
        // If it didn't work, try just changing the resolution and not the
        // refresh rate.
        deviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        result = ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
    }

    return result == DISP_CHANGE_SUCCESSFUL;
}


Win32Window::Win32Window(const GWindowSettings& s) {
    initWGL();

	_hDC = NULL;
	_mouseVisible = true;
	_inputCapture = false;

	settings = s;
    
	std::string name = "G3D";
    
    // Add the non-client area
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = settings.width;
	rect.bottom = settings.height;

	DWORD style = 0;
	
	if (s.framed) {

        // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/WinUI/WindowsUserInterface/Windowing/Windows/WindowReference/WindowStyles.asp
		style |= WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

		if (s.resizable) {
			style |= WS_SIZEBOX;
		}

        if (s.visible) {
            style |= WS_VISIBLE;
        }
	}

	AdjustWindowRect(&rect, style, false);

	int total_width  = rect.right - rect.left;
	int total_height = rect.bottom - rect.top;
    
    HWND window = CreateWindow("window", 
        name.c_str(),
        style,
        (GetSystemMetrics(SM_CXSCREEN) - total_width) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - total_height) / 2,
        total_width,
        total_height,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    alwaysAssertM(window != NULL, "");

    if (s.visible) {
        ShowWindow(window, SW_SHOW);
    } 
            
    SetWindowLong(window, GWL_USERDATA, (LONG)this);

    if (settings.fullScreen) {
	    // Change the desktop resolution if we are running in fullscreen mode
        if (!ChangeResolution(settings.width, settings.height, settings.rgbBits * 3, settings.refreshRate)) {
			alwaysAssertM(false, "Failed to change resolution");
        }
    }
	init(window);

    // Detect DirectInput8 only and create the joystick interfaces
    HMODULE di8Module = ::LoadLibrary("dinput8.dll");
    if (di8Module == NULL) {
        return;
    }

    // DI8 function pointers
    HRESULT (WINAPI* DirectInput8Create_G3D)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN);
    DirectInput8Create_G3D = (HRESULT (WINAPI*)(HINSTANCE, DWORD, REFIID, LPVOID *, LPUNKNOWN))::GetProcAddress(di8Module, "DirectInput8Create");
    if (DirectInput8Create_G3D == NULL) {
        ::FreeLibrary(di8Module);
        return;
    }

    if (DirectInput8Create_G3D( ::GetModuleHandle(NULL), DIRECTINPUT_VERSION, Win32Window_DI8::IID_IDirectInput8A, reinterpret_cast< void** >(&di8Interface), NULL) != S_OK) {
        ::FreeLibrary(di8Module);
        return;
    }
    
    di8Interface->EnumDevices(Win32Window_DI8::DI8DEVCLASS_GAMECTRL, EnumDirectInput8Joysticks, window, Win32Window_DI8::DIEDFL_ATTACHEDONLY);

    ::FreeLibrary(di8Module);
}


Win32Window::Win32Window(const GWindowSettings& s, HWND hwnd) {
    initWGL();

	settings = s;
	init(hwnd);
}


void Win32Window::init(HWND hwnd) {

	window = hwnd;
 
    // Setup the pixel format properties for the output device
    _hDC = GetDC(window);

    int pixelFormat = 0;

    PIXELFORMATDESCRIPTOR pixelFormatDesc;
    ZeroMemory(&pixelFormatDesc, sizeof(PIXELFORMATDESCRIPTOR));

    pixelFormatDesc.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
    pixelFormatDesc.nVersion     = 1; 
    pixelFormatDesc.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixelFormatDesc.iPixelType   = PFD_TYPE_RGBA; 
    pixelFormatDesc.cColorBits   = settings.rgbBits * 3;
    pixelFormatDesc.cDepthBits   = settings.depthBits;
    pixelFormatDesc.cStencilBits = settings.stencilBits;
    pixelFormatDesc.iLayerType   = PFD_MAIN_PLANE; 
    pixelFormatDesc.cRedBits     = settings.rgbBits;
    pixelFormatDesc.cGreenBits   = settings.rgbBits;
    pixelFormatDesc.cBlueBits    = settings.rgbBits;
    pixelFormatDesc.cAlphaBits   = settings.alphaBits;

    // Get the initial pixel format.  We'll override this below in a moment.
    pixelFormat = ChoosePixelFormat(_hDC, &pixelFormatDesc);

    if (wglChoosePixelFormatARB != NULL) {
        // Use wglChoosePixelFormatARB to override the pixel format choice for antialiasing.
        // Based on http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=46
        // and http://oss.sgi.com/projects/ogl-sample/registry/ARB/wgl_pixel_format.txt

        Array<int> iAttributes;

        iAttributes.append(WGL_DRAW_TO_WINDOW_ARB, GL_TRUE);
		iAttributes.append(WGL_SUPPORT_OPENGL_ARB, GL_TRUE);
		iAttributes.append(WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB);
        iAttributes.append(WGL_COLOR_BITS_ARB,     settings.rgbBits * 3);
        iAttributes.append(WGL_RED_BITS_ARB,       settings.rgbBits);
        iAttributes.append(WGL_GREEN_BITS_ARB,     settings.rgbBits);
        iAttributes.append(WGL_BLUE_BITS_ARB,      settings.rgbBits);
        iAttributes.append(WGL_ALPHA_BITS_ARB,     settings.alphaBits);
        iAttributes.append(WGL_DEPTH_BITS_ARB,     settings.depthBits);
        iAttributes.append(WGL_STENCIL_BITS_ARB,   settings.stencilBits);
        iAttributes.append(WGL_DOUBLE_BUFFER_ARB,  GL_TRUE);
        iAttributes.append(WGL_SAMPLE_BUFFERS_ARB, settings.fsaaSamples > 1);
		iAttributes.append(WGL_SAMPLES_ARB,        settings.fsaaSamples);
        iAttributes.append(WGL_STEREO_ARB,         settings.stereo);
        iAttributes.append(0, 0); // end sentinel
        
        uint32 numFormats;

        int valid = wglChoosePixelFormatARB(
            _hDC,
            iAttributes.getCArray(), 
            NULL,
            1,
            &pixelFormat,
            &numFormats);

        if ((numFormats < 1) || ! valid) {
            // No valid format
            pixelFormat = 0;
        }
    }

	alwaysAssertM(pixelFormat != 0, "[0] Unsupported video mode");

    if (SetPixelFormat(_hDC, pixelFormat, &pixelFormatDesc) == FALSE) {
		alwaysAssertM(false, "[1] Unsupported video mode");
    }

    // Create the OpenGL context
    _glContext = wglCreateContext(_hDC);

	alwaysAssertM(_glContext != NULL, "Failed to create OpenGL context.");

    if (!wglMakeCurrent(_hDC, _glContext)) {
        alwaysAssertM(false, "Failed to make OpenGL context current.");
    }
}


int Win32Window::width() const {
	return settings.width;
}


int Win32Window::height() const {
	return settings.height;
}


Rect2D Win32Window::dimensions() const {
	return Rect2D::xywh(clientX, clientY, width(), height());
}


std::string Win32Window::getAPIVersion() const {
	return "";
}


std::string Win32Window::getAPIName() const {
	return "Win32";
}


bool Win32Window::requiresMainLoop() const {
    return false;
}


void Win32Window::swapGLBuffers() {
	SwapBuffers(hdc());
}


void Win32Window::close() {
    PostMessage(window, WM_CLOSE, 0, 0);
    /*
    TODO: do we need this?
    wglDeleteContext(_glContext);					
	_glContext = 0;	
	ReleaseDC(window, _hDC);	
	window = 0;				
	DestroyWindow(window);			
	window = 0;
*/
}


Win32Window::~Win32Window() {
    close();

    //Release any joystick interfaces
    for(int i = 0; i < joysticks.length(); ++i) {
        if (joysticks[i].valid) {
            joysticks[i].device->Release();
            joysticks[i].device = NULL;
            joysticks[i].valid = false;
        }
    }
    joysticks.clear();
    joysticks.resize(0, true);
}


void Win32Window::getSettings(GWindowSettings& s) const {
	s = settings;
}


void Win32Window::setCaption(const std::string& caption) {
	if (_title != caption) {
		_title = caption;
		SetWindowText(window, _title.c_str());
	}
}


std::string Win32Window::caption() {
	return _title;
}


static void makeKeyEvent(int wparam, int lparam, GEvent& e) {
	char c = wparam;
	e.key.keysym.unicode = c;

    if ((c >= 'A') && (c <= 'Z')) {
        // Make key codes lower case canonically
        e.key.keysym.sym = (SDLKey)(c - 'A' + 'a');
    } else {
        e.key.keysym.sym = (SDLKey)c;
    }
    e.key.keysym.scancode = (lparam >> 16) & 0x07;

    e.key.keysym.mod = KMOD_NONE;

	// Bit 24 is 1 if this is the right hand version of ALT or CTRL					
}


/** 
 Configures a mouse up/down event
 */
static void mouseButton(bool down, int keyEvent, DWORD flags, GEvent& e) {
	if (down) {
		e.key.type  = SDL_KEYDOWN;
		e.key.state = SDL_PRESSED;
	} else {
		e.key.type  = SDL_KEYUP;
		e.key.state = SDL_RELEASED;
	}

    e.key.keysym.unicode = ' ';
	e.key.keysym.sym = (SDLKey)keyEvent;


    e.key.keysym.scancode = 0;
    // TODO: fwKeys = wParam;        // key flags 
    e.key.keysym.mod = KMOD_NONE;
}


bool Win32Window::pollEvent(GEvent& e) {
	MSG message;

	bool done = false;

	while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.hwnd == window) {
			switch (message.message) {
            case WM_CLOSE:
            case WM_QUIT:
                e.type = SDL_QUIT;

                break;

			case WM_KEYDOWN:
				e.key.type = SDL_KEYDOWN;
				e.key.state = SDL_PRESSED;

				if ((message.lParam & 0x0f) == 1) {
					// This is not an autorepeat message
					makeKeyEvent(message.wParam, message.lParam, e);
					return true;
				}
				break;

			case WM_KEYUP:
				e.key.type = SDL_KEYUP;
				e.key.state = SDL_RELEASED;

				if ((message.lParam & 0x0f) == 1) {
					// This is not an autorepeat message
					makeKeyEvent(message.wParam, message.lParam, e);
					return true;
				}
				break;

			case WM_ACTIVATE:
                // TODO
                /*
                fActive = LOWORD(wParam);           // activation flag 
                fMinimized = (BOOL) HIWORD(wParam); // minimized flag 
                hwndPrevious = (HWND) lParam;       // window handle 
                */
                break;

            case WM_LBUTTONDOWN:
				mouseButton(true, SDL_LEFT_MOUSE_KEY, message.wParam, e); 
				return true;

            case WM_MBUTTONDOWN:
				mouseButton(true, SDL_MIDDLE_MOUSE_KEY, message.wParam, e); 
				return true;

            case WM_RBUTTONDOWN:
				mouseButton(true, SDL_RIGHT_MOUSE_KEY, message.wParam, e); 
				return true;

            case WM_LBUTTONUP:
				mouseButton(false, SDL_LEFT_MOUSE_KEY, message.wParam, e); 
				return true;

            case WM_MBUTTONUP:
				mouseButton(false, SDL_MIDDLE_MOUSE_KEY, message.wParam, e); 
				return true;

            case WM_RBUTTONUP:
				mouseButton(false, SDL_RIGHT_MOUSE_KEY, message.wParam, e); 
				return true;
			} // switch
		} // if
    } // while

	RECT rect;
	GetWindowRect(window, &rect);
	settings.x = rect.left;
	settings.y = rect.top;

	GetClientRect(window, &rect);
	settings.width = rect.right - rect.left;
	settings.height = rect.bottom - rect.top;

	clientX = settings.x;
	clientY = settings.y;

	if (settings.framed) {
		// Add the border offset
		clientX	+= GetSystemMetrics(settings.resizable ? SM_CXSIZEFRAME : SM_CXFIXEDFRAME);
		clientY += GetSystemMetrics(settings.resizable ? SM_CYSIZEFRAME : SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION);
	}

    return false;
}


void Win32Window::setMouseVisible(bool b) {
	if (_mouseVisible == b) {
		return;
	}

    if (b) {
		while (ShowCursor(true) < 0);
	} else {
		while (ShowCursor(false) >= 0); 
    }

    _mouseVisible = b;
}


bool Win32Window::mouseVisible() const {
	return _mouseVisible;
}


void Win32Window::notifyResize(int w, int h) {
	settings.width = w;
	settings.height = h;
}


bool Win32Window::inputCapture() const {
	return _inputCapture;
}


void Win32Window::setGammaRamp(const Array<uint16>& gammaRamp) {
    alwaysAssertM(gammaRamp.size() >= 256, "Gamma ramp must have at least 256 entries");

    Log* debugLog = Log::common();

    uint16* ptr = const_cast<uint16*>(gammaRamp.getCArray());
    uint16 wptr[3 * 256];
    for (int i = 0; i < 256; ++i) {
        wptr[i] = wptr[i + 256] = wptr[i + 512] = ptr[i]; 
    }
    BOOL success = SetDeviceGammaRamp(hdc(), wptr);

    if (! success) {
        if (debugLog) {debugLog->println("Error setting gamma ramp!");}

        debugAssertM(false, "Failed to set gamma ramp");
    }
}


LRESULT WINAPI Win32Window::window_proc(
    HWND                window,
    UINT                message,
    WPARAM              wparam,
    LPARAM              lparam) {
    
    Win32Window* this_window = (Win32Window*)GetWindowLong(window, GWL_USERDATA);
    
    switch (message) {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
        
    case WM_DESTROY:
        this_window->window = NULL;
        break;
    }
    
    return DefWindowProc(window, message, wparam, lparam);
}


void Win32Window::setRelativeMousePosition(double x, double y) {
	SetCursorPos(iRound(x + clientX), iRound(y + clientY));
}


void Win32Window::setRelativeMousePosition(const Vector2& p) {
    setRelativeMousePosition(p.x, p.y);
}


void Win32Window::getRelativeMouseState(Vector2& p, uint8& mouseButtons) const {
    int x, y;
    getRelativeMouseState(x, y, mouseButtons);
    p.x = x;
    p.y = y;
}


void Win32Window::getRelativeMouseState(int& x, int& y, uint8& mouseButtons) const {
	POINT point;
	GetCursorPos(&point);
	x = point.x - clientX;
	y = point.y - clientY;
	// TODO: buttons
}


void Win32Window::getRelativeMouseState(double& x, double& y, uint8& mouseButtons) const {
    int ix, iy;
    getRelativeMouseState(ix, iy, mouseButtons);
    x = ix;
    y = iy;
}


std::string Win32Window::joystickName(unsigned int sticknum)
{
    if (joysticks.length() > sticknum) {
        return joysticks[sticknum].name;
    } else {
        debugAssert( joysticks.length() <= sticknum );
    }
    return std::string("No Device.");
}


void Win32Window::getJoystickState(unsigned int stickNum, Array<float>& axis, Array<bool>& button) {
    if (joysticks.length() > stickNum) {
        
        G3DJOYDATA joystickState;
        memset(&joystickState, 0, sizeof(joystickState));
        
        joysticks[stickNum].device->Acquire();
        joysticks[stickNum].device->Poll();

        joysticks[stickNum].device->GetDeviceState(sizeof(joystickState), &joystickState);

        button.resize(joysticks[stickNum].numButtons, false);
        for (int b = 0; (b < joysticks[stickNum].numButtons) && (b < 32); ++b) {
            button[b] = (joystickState.rgbButtons[b] & 128) ? true : false;
        }

        axis.resize(joysticks[stickNum].numAxes, false);
        for (int a = 0; a < (joysticks[stickNum].numAxes) && (a < 8); ++a) {
            axis[a] = (float)( ( (float)((LONG*)&joystickState)[a] - 32768) / 32768);
        }
    } else {
        debugAssert( joysticks.length() <= stickNum );
    }
}


void Win32Window::setInputCapture(bool c) {
	if (c != _inputCapture) {
		_inputCapture = c;

		if (_inputCapture) {
			RECT rect = {settings.x, settings.y, settings.width, settings.height};
			ClipCursor(&rect);
		} else {
			ClipCursor(NULL);
		}
	}
}


void Win32Window::initWGL() {
    std::string name = "G3D";
    WNDCLASS window_class;
    
    window_class.style         = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc   = window_proc;
    window_class.cbClsExtra    = 0; 
    window_class.cbWndExtra    = 0;
    window_class.hInstance     = GetModuleHandle(NULL);
    window_class.hIcon         = LoadIcon(NULL, IDI_APPLICATION); 
    window_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    window_class.lpszMenuName  = name.c_str();
    window_class.lpszClassName = "window"; 
    
    int ret = RegisterClass(&window_class);
	alwaysAssertM(ret, "Registration Failed");

    // Create some dummy pixel format.
	PIXELFORMATDESCRIPTOR pfd =	
    {
		sizeof (PIXELFORMATDESCRIPTOR),									// Size Of This Pixel Format Descriptor
		1,																// Version Number
		PFD_DRAW_TO_WINDOW |											// Format Must Support Window
		PFD_SUPPORT_OPENGL |											// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,												// Must Support Double Buffering
		PFD_TYPE_RGBA,													// Request An RGBA Format
		24,		                        								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,												// Color Bits Ignored
		1,																// Alpha Buffer
		0,																// Shift Bit Ignored
		0,																// No Accumulation Buffer
		0, 0, 0, 0,														// Accumulation Bits Ignored
		16,																// 16Bit Z-Buffer (Depth Buffer)  
		0,																// No Stencil Buffer
		0,																// No Auxiliary Buffer
		PFD_MAIN_PLANE,													// Main Drawing Layer
		0,																// Reserved
		0, 0, 0															// Layer Masks Ignored
	};

    HWND hWnd = CreateWindow("window", "", 0, 0, 0, 100, 100, NULL, NULL, GetModuleHandle(NULL), NULL);
    debugAssert(hWnd);

    HDC  hDC  = GetDC(hWnd);
    debugAssert(hDC);
    
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    debugAssert(pixelFormat);

    if (SetPixelFormat(hDC, pixelFormat, &pfd) == FALSE) {
        debugAssertM(false, "Failed to set pixel format");
	}

	HGLRC hRC = wglCreateContext(hDC);
    debugAssert(hRC);

	if (wglMakeCurrent(hDC, hRC) == FALSE)	{
        debugAssertM(false, "Failed to set context");
	}

    // We've now brought OpenGL online.  Grab the pointers we need and 
    // destroy everything.

    wglChoosePixelFormatARB =
        (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

    // Now destroy everything
    wglDeleteContext(hRC);					
    hRC = 0;	
	ReleaseDC(hWnd, hDC);	
	hWnd = 0;				
	DestroyWindow(hWnd);			
	hWnd = 0;
}
