/**
  @file SDL_GL_Demo/initGL.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-01-01
  @edited  2002-07-02
 */

#include "glheaders.h"
#include "SDL_syswm.h"

PFNGLMULTITEXCOORD2FVARBPROC        glMultiTexCoord2fvARB = NULL;
PFNGLMULTITEXCOORD2FARBPROC         glMultiTexCoord2fARB = NULL;
PFNGLACTIVETEXTUREARBPROC           glActiveTextureARB = NULL;
PFNWGLGLSWAPINTERVALEXTPROC         wglSwapIntervalEXT = NULL;

void initGL() {
    // Under Windows, reset the last error so that our debug box
    // gives the correct results
    #if _WIN32
        SetLastError(0);
    #endif

	const int minimumDepthBits = 16;
	const int desiredDepthBits = 24;

	const int minimumStencilBits = 8;
	const int desiredStencilBits = 8;

    const int width = 600;
    const int height = 600;

    const bool fullscreen = false;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0) {
        std::string msg = format("Unable to initialize SDL: %s\n", SDL_GetError());

        error("Critical Error", msg, true);
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, desiredDepthBits);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, GL_TRUE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, desiredStencilBits);

	// Create a width x height OpenGL screen 
    int flags =  SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0);
	if (SDL_SetVideoMode(width, height, 0, flags) == NULL) {
		SDL_Quit();
        error("Critical Error", "Can't create a window.", true);
		exit(2);
	}

	// Set the title bar
	SDL_WM_SetCaption("OpenGL Window", NULL);

	glViewport(0, 0, width, height);

    // Load the OpenGL Extensions we want
    LOAD_GL_EXTENSION(glMultiTexCoord2fvARB);
    LOAD_GL_EXTENSION(glMultiTexCoord2fARB);
    LOAD_GL_EXTENSION(glActiveTextureARB);
    LOAD_GL_EXTENSION(wglSwapIntervalEXT);
}

#ifdef WIN32
extern HWND SDL_Window;

HDC getWindowHDC() {
    // Get Windows HDC
    SDL_SysWMinfo info;

    SDL_VERSION(&info.version);

    int result = SDL_GetWMInfo(&info);

    if (result != 1) {
        debugAssertM(false, SDL_GetError());
    }

    HDC hdc = GetDC(info.window);

    if (hdc == 0) {
        debugAssert(hdc != 0);
    }

    return hdc;
}
#endif