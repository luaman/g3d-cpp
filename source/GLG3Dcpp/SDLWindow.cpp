/**
  @file SDLWindow.cpp

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2004-02-10
  @edited  2004-02-28
*/

#include "GLG3D/SDLWindow.h"
#include "GLG3D/glcalls.h"

#define SDL_FSAA (SDL_MAJOR_VERSION * 100 + SDL_MINOR_VERSION * 10 + SDL_PATCH_LEVEL > 125)

namespace G3D {

SDLWindow::SDLWindow(const GWindowSettings& settings) {

	if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0 ) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		debugPrintf("Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

    _mouseVisible = true;
    _mouseCapture = false;

	// Request various OpenGL parameters
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,      settings.depthBits);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,    1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,    settings.stencilBits);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,        settings.rgbBits);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,      settings.rgbBits);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,       settings.rgbBits);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,      settings.alphaBits);
    SDL_GL_SetAttribute(SDL_GL_STEREO,          settings.stereo);

    #if SDL_FSAA
        if (settings.fsaaSamples > 1) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, settings.fsaaSamples);
        }
    #endif

	// Create a width x height OpenGL screen 
    int flags = 
        SDL_HWSURFACE |
        SDL_OPENGL |
        (settings.fullScreen ? SDL_FULLSCREEN : 0) |
        (settings.resizable ? SDL_RESIZABLE : 0) |
        (settings.framed ? 0 : SDL_NOFRAME);

	if (SDL_SetVideoMode(settings.width, settings.height, 0, flags) == NULL) {
        debugAssert(false);
        Log::common()->printf("Unable to create OpenGL screen: %s\n", SDL_GetError());
		error("Critical Error", format("Unable to create OpenGL screen: %s\n", SDL_GetError()).c_str(), true);
		SDL_Quit();
		exit(2);
	}

    // See what video mode we really got
    _settings = settings;
    int depthBits, stencilBits, redBits, greenBits, blueBits, alphaBits;
    glGetIntegerv(GL_DEPTH_BITS, &depthBits);
    glGetIntegerv(GL_STENCIL_BITS, &stencilBits);

    glGetIntegerv(GL_RED_BITS,   &redBits);
    glGetIntegerv(GL_GREEN_BITS, &greenBits);
    glGetIntegerv(GL_BLUE_BITS,  &blueBits);
    glGetIntegerv(GL_ALPHA_BITS, &alphaBits);
    int actualFSAABuffers = 0, actualFSAASamples = 0;

    #if SDL_FSAA
        SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &actualFSAABuffers);
        SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &actualFSAASamples);
    #endif
    _settings.rgbBits     = iMin(iMin(redBits, greenBits), blueBits);
    _settings.alphaBits   = alphaBits;
    _settings.stencilBits = stencilBits;
    _settings.depthBits   = depthBits;
    _settings.fsaaSamples = actualFSAASamples;

    SDL_version ver;
    SDL_VERSION(&ver);
    _version = format("%d,%0d.%0d", ver.major, ver.minor, ver.patch);

    SDL_EnableUNICODE(1);
    setCaption("G3D");

	if (numJoysticks() > 0) {
        SDL_JoystickEventState(SDL_ENABLE);
        // Turn on joystick 0
        _joy = SDL_JoystickOpen(0);
        debugAssert(_joy);
	}
}


SDLWindow::~SDLWindow() {
	// Close joystick, if opened
	if (SDL_JoystickOpened(0)) {
  		SDL_JoystickClose(_joy);
		_joy = NULL;
	}

    SDL_Quit();
}


Vector2 SDLWindow::joystickPosition(int stickNum) const {
    if (_joy) {
        return Vector2(SDL_JoystickGetAxis(_joy, 0) / 32768.0,
                       -SDL_JoystickGetAxis(_joy, 1) / 32768.0);
    } else {
        return Vector2(0, 0);
    }
}


::SDL_Joystick* SDLWindow::joystick(int num) const {
    debugAssert(num < 1);
    return _joy;
}


void SDLWindow::getSettings(GWindowSettings& settings) const {
    settings = _settings;
}


int SDLWindow::width() const {
    return _settings.width;
}


int SDLWindow::height() const {
    return _settings.height;
}


Rect2D SDLWindow::dimensions() const {
    return Rect2D::xywh(0, 0, _settings.width, _settings.height);;
}


void SDLWindow::setDimensions(const Rect2D& dims) {
    // Do nothing
}


void SDLWindow::setPosition(int x, int y) const  {
    // DO nothing
}


bool SDLWindow::hasFocus() const {
    uint8 s = SDL_GetAppState();
    
    return ((s & SDL_APPMOUSEFOCUS) != 0) &&
           ((s & SDL_APPINPUTFOCUS) != 0) &&
           ((s & SDL_APPACTIVE) != 0);
}


std::string SDLWindow::getAPIVersion() const {
    return _version;
}


std::string SDLWindow::getAPIName() const {
    return "SDL";
}


void SDLWindow::setGammaRamp(const Array<uint16>& gammaRamp) {
    alwaysAssertM(gammaRamp.size() >= 256, "Gamma ramp must have at least 256 entries");

    Log* debugLog = Log::common();

    uint16* ptr = const_cast<uint16*>(gammaRamp.getCArray());
    #ifdef WIN32
        // On windows, use the more reliable SetDeviceGammaRamp function.
        // It requires separate RGB gamma ramps.
        uint16 wptr[3 * 256];
        for (int i = 0; i < 256; ++i) {
            wptr[i] = wptr[i + 256] = wptr[i + 512] = ptr[i]; 
        }
        BOOL success = SetDeviceGammaRamp(wglGetCurrentDC(), wptr);
    #else
        bool success = (SDL_SetGammaRamp(ptr, ptr, ptr) != -1);
    #endif

    if (! success) {
        if (debugLog) {debugLog->println("Error setting gamma ramp!");}

        #ifdef WIN32
            debugAssertM(false, "Failed to set gamma ramp");
        #else
            if (debugLog) {debugLog->println(SDL_GetError());}
            debugAssertM(false, SDL_GetError());
        #endif
    }
}


int SDLWindow::numJoysticks() const {
	// Check for joysticks
    int j = SDL_NumJoysticks();
    if ((j < 0) || (j > 10)) {
        // If there is no joystick adapter on Win32,
        // SDL returns ridiculous numbers.
        j = 0;
    }

    return j;
}


void SDLWindow::setCaption(const std::string& caption) {
    _caption = caption;
	SDL_WM_SetCaption(_caption.c_str(), NULL);
}


std::string SDLWindow::caption() {
    return _caption;
}


void SDLWindow::setIcon(const GImage& image) {
    alwaysAssertM(image.channels > 2, "Icon image must have at least 3 channels.");

    uint8* mask = NULL;

    if (image.channels == 4) {
        // Has an alpha channel; construct a mask
        int len = iCeil(image.width/8) * image.height;
        mask = new uint8[len];
        // Initialize the mask to transparent
        System::memset(mask, 0, len);

        // Set bits with an alpha value >= 127.
        for (int y = 0; y < image.height; ++y) {
            for (int x = 0; x < image.width; ++x) {
                // Test this pixel
                bool bit = image.pixel4()[y * image.width + x].a >= 127;

                // Set the correct bit
                mask[y * image.width + x / 8] |= (bit << (x % 8));
            }
        }
    }

    uint32 rmask = 0xFF000000;
    uint32 gmask = 0x00FF0000;
    uint32 bmask = 0x0000FF00;
    uint32 amask = 0x000000FF;

    if (image.channels < 4) {
        rmask = rmask >> 8;
        gmask = gmask >> 8;
        bmask = bmask >> 8;
        amask = amask >> 8;
    }

    int pixelByteLen    = image.channels;
    int scanLineByteLen = pixelByteLen * image.width;

    SDL_Surface* surface =
        SDL_CreateRGBSurfaceFrom((void*)image.byte(), image.width, image.height,
        pixelByteLen, scanLineByteLen, 
        rmask, gmask, bmask, amask);

    SDL_WM_SetIcon(surface, mask);

    SDL_FreeSurface(surface);
    delete mask;
}


void SDLWindow::swapGLBuffers() {
    SDL_GL_SwapBuffers();
}


void SDLWindow::notifyResize(int w, int h) {
    debugAssert(w > 0);
    debugAssert(h > 0);
    _settings.width = w;
    _settings.height = h;

	// Mutate the SDL surface (which one is not supposed to do).
	// We can't resize the actual surface or SDL will destroy
	// our GL context, however.
	SDL_Surface* surface = SDL_GetVideoSurface();
	surface->w = w;
	surface->h = h;
	surface->clip_rect.x = 0;
	surface->clip_rect.y = 0;
	surface->clip_rect.w = w;
	surface->clip_rect.h = h;
}


void SDLWindow::setRelativeMousePosition(double x, double y) {
    SDL_WarpMouse(iRound(x), iRound(y));
}


void SDLWindow::setRelativeMousePosition(const Vector2& p) {
    setRelativeMousePosition(p.x, p.y);
}


void SDLWindow::getRelativeMouseState(Vector2& p, uint8& mouseButtons) const {
    int x, y;
    getRelativeMouseState(x, y, mouseButtons);
    p.x = x;
    p.y = y;
}


void SDLWindow::getRelativeMouseState(int& x, int& y, uint8& mouseButtons) const {
    mouseButtons = SDL_GetMouseState(&x, &y);
}


void SDLWindow::getRelativeMouseState(double& x, double& y, uint8& mouseButtons) const {
    int ix, iy;
    getRelativeMouseState(ix, iy, mouseButtons);
    x = ix;
    y = iy;
}


void SDLWindow::setMouseVisible(bool v) {
    if (v) {
        SDL_ShowCursor(SDL_ENABLE);
    } else {
        SDL_ShowCursor(SDL_DISABLE);
    }

    _mouseVisible = v;
}


bool SDLWindow::mouseVisible() const {
    return _mouseVisible;
}


void SDLWindow::setMouseCapture(bool c) {
    _mouseCapture = c;

    if (_mouseCapture) {
        SDL_WM_GrabInput(SDL_GRAB_ON);
    } else {
        SDL_WM_GrabInput(SDL_GRAB_OFF);
    }
}


bool SDLWindow::mouseCapture() const {
    return _mouseCapture;
}


} // namespace
