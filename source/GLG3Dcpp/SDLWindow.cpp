/**
  @file SDLWindow.cpp

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2004-02-10
  @edited  2004-02-11
*/

#include "GLG3D/SDLWindow.h"
#include "GLG3D/glcalls.h"

namespace G3D {

SDLWindow::SDLWindow(const GWindowSettings& settings) {

	if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0 ) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		debugPrintf("Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
 
	// Request various OpenGL parameters
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,      settings.depthBits);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,    1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,    settings.stencilBits);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,        settings.rgbBits);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,      settings.rgbBits);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,       settings.rgbBits);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,      settings.alphaBits);
    SDL_GL_SetAttribute(SDL_GL_STEREO,          settings.stereo);

    #ifdef SDL_1_26
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
    _settings.rgbBits     = iMin(iMin(redBits, greenBits), blueBits);
    _settings.alphaBits   = alphaBits;
    _settings.stencilBits = stencilBits;
    _settings.depthBits   = depthBits;

    SDL_version ver;
    SDL_VERSION(&ver);
    _version = format("%d,%0d.%0d", ver.major, ver.minor, ver.patch);

    SDL_EnableUNICODE(1);
    setCaption("G3D");
    _win32HDC();
}


void SDLWindow::_win32HDC() {
#ifdef G3D_WIN32
    Log* debugLog = Log::common();

    SDL_SysWMinfo info;

    if (debugLog) {
        debugLog->println("Getting HDC... ");
    }
    SDL_VERSION(&info.version);

    int result = SDL_GetWMInfo(&info);

    if (result != 1) {
        if (debugLog) {
            debugLog->println(SDL_GetError());
        }
        debugAssertM(false, SDL_GetError());
    }

    hdc = GetDC(info.window);

    if (hdc == 0) {
        if (debugLog) {
            debugLog->println("hdc == 0");
        }
        debugAssert(hdc != 0);
    }

    if (debugLog) {debugLog->println("HDC acquired.");}
#endif
}


SDLWindow::~SDLWindow() {
    SDL_Quit();
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


void setGammaRamp(const Array<uint16>& gammaRamp) {
    alwaysAssertM(gammaRamp.size() >= 256, "Gamma ramp must have at least 256 entries");

    Log* debugLog = Log::common();

    uint16* ptr = const_cast<uint16*>(gammaRamp.getCArray());
    bool success = (SDL_SetGammaRamp(ptr, ptr, ptr) != -1);

    if (! success) {
        if (debugLog) {debugLog->println("Error setting brightness!");}

        #ifdef WIN32
            debugAssertM(false, "Failed to set brightness");
        #else
            if (debugLog) {debugLog->println(SDL_GetError());}
            debugAssertM(false, SDL_GetError());
        #endif
    }
}


void SDLWindow::setCaption(const std::string& caption) {
    _caption = caption;
	SDL_WM_SetCaption(_caption.c_str(), NULL);
}


#ifdef G3D_WIN32
HDC SDLWindow::getHDC() const {
    return hdc;
}
#endif


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

} // namespace
