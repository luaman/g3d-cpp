/**
  @file Win32Window.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com 
  @cite       Special thanks to Max McGuire, mmcguire@ironlore.com
  @created 	  2004-05-21
  @edited  	  2004-05-21
    
  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.
*/

#include "Win32Window.h"
#include <time.h>
#include <crtdbg.h>

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
	_hDC = NULL;
	_mouseVisible = true;
	_inputCapture = false;

	settings = s;
    
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
    
    // Add the non-client area
    int total_width  = settings.width  + GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
    int total_height = settings.height + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);
    
    window = CreateWindow("window", 
        name.c_str(),
        WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        (GetSystemMetrics(SM_CXSCREEN) - total_width) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - total_height) / 2,
        total_width,
        total_height,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    
    alwaysAssertM(window != NULL, "");
            
    SetWindowLong(window, GWL_USERDATA, (LONG)this);
    ShowWindow(window, SW_SHOW);

	////////////////////////////////////////////////////////////////////////
    // Change the desktop resolution if we are running in fullscreen mode

    if (settings.fullScreen) {
        if (!ChangeResolution(settings.width, settings.height, settings.rgbBits * 3, settings.refreshRate)) {
			alwaysAssertM(false, "Failed to change resolution");
        }
    }
 
    // Setup the pixel format properties for the output device
    _hDC = GetDC(window);

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

    int pixelFormat = ChoosePixelFormat(_hDC, &pixelFormatDesc);

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
	// TODO
	return Rect2D::xywh(0, 0, width(), height());
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
}


Win32Window::~Win32Window() {
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


bool Win32Window::pollEvent(GEvent& e) {
	MSG message;

	bool done = false;

	while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.hwnd == window) {
			switch (message.message) {
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
			}
		}
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
    // On windows, use the more reliable SetDeviceGammaRamp function.
    // It requires separate RGB gamma ramps.
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