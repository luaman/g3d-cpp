/**
 @file RenderDevice.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2001-07-08
 @edited  2003-06-13
 */


#include <SDL.h>
#include <SDL_syswm.h>
#include <sstream>
#include "G3D/platform.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/Texture.h"
#include "GLG3D/getOpenGLState.h"
#ifdef G3D_WIN32
    #include <winver.h>
#endif

PFNGLMULTITEXCOORD1FARBPROC                 glMultiTexCoord1fARB		    = NULL;
PFNGLMULTITEXCOORD1DARBPROC                 glMultiTexCoord1dARB		    = NULL;

PFNGLMULTITEXCOORD2FARBPROC                 glMultiTexCoord2fARB		    = NULL;
PFNGLMULTITEXCOORD2FVARBPROC                glMultiTexCoord2fvARB		    = NULL;
PFNGLMULTITEXCOORD2DVARBPROC                glMultiTexCoord2dvARB		    = NULL;

PFNGLMULTITEXCOORD3FVARBPROC                glMultiTexCoord3fvARB		    = NULL;
PFNGLMULTITEXCOORD3DVARBPROC                glMultiTexCoord3dvARB		    = NULL;

PFNGLMULTITEXCOORD4FVARBPROC                glMultiTexCoord4fvARB		    = NULL;
PFNGLMULTITEXCOORD4DVARBPROC                glMultiTexCoord4dvARB		    = NULL;

PFNGLACTIVETEXTUREARBPROC                   glActiveTextureARB 			    = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC             glClientActiveTextureARB	    = NULL;

PFNWGLSWAPINTERVALEXTPROC                   wglSwapIntervalEXT 			    = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC              wglChoosePixelFormatARB		    = NULL;
PFNWGLALLOCATEMEMORYNVPROC                  wglAllocateMemoryNV 		    = NULL;
PFNWGLFREEMEMORYNVPROC                      wglFreeMemoryNV 			    = NULL;

PFNGLVERTEXARRAYRANGENVPROC                 glVertexArrayRangeNV 		    = NULL;

PFNGLCOMPRESSEDTEXIMAGE2DARBPROC            glCompressedTexImage2DARB 	    = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEARBPROC           glGetCompressedTexImageARB 	    = NULL;

PFNGLGENFENCESNVPROC				        glGenFencesNV				    = NULL;
PFNGLDELETEFENCESNVPROC				        glDeleteFencesNV			    = NULL;
PFNGLSETFENCENVPROC					        glSetFenceNV				    = NULL;
PFNGLFINISHFENCENVPROC	  			        glFinishFenceNV				    = NULL;

PFNGLGENPROGRAMSARBPROC                     glGenProgramsARB                = NULL;
PFNGLBINDPROGRAMARBPROC                     glBindProgramARB                = NULL;
PFNGLDELETEPROGRAMSARBPROC                  glDeleteProgramsARB             = NULL;
PFNGLPROGRAMSTRINGARBPROC                   glProgramStringARB              = NULL;
PFNGLPROGRAMENVPARAMETER4FARBPROC           glProgramEnvParameter4fARB      = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC         glProgramLocalParameter4fARB    = NULL;
PFNGLPROGRAMENVPARAMETER4DVARBPROC          glProgramEnvParameter4dvARB     = NULL;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC        glProgramLocalParameter4dvARB   = NULL;
PFNGLISPROGRAMARBPROC                       glIsProgramARB                  = NULL;

PFNGLVERTEXATTRIBPOINTERARBPROC             glVertexAttribPointerARB        = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC         glEnableVertexAttribArrayARB    = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC        glDisableVertexAttribArrayARB   = NULL;

PFNGLPOINTPARAMETERFARBPROC                 glPointParameterfARB            = NULL;
PFNGLPOINTPARAMETERFVARBPROC                glPointParameterfvARB           = NULL;

PFNGLMULTIDRAWARRAYSEXTPROC                 glMultiDrawArraysEXT            = NULL;
PFNGLMULTIDRAWELEMENTSEXTPROC               glMultiDrawElementsEXT          = NULL;

namespace G3D {

static void frustum(
    GLdouble left,    
    GLdouble right,
    GLdouble bottom,  
    GLdouble top,
    GLdouble nearval, 
    GLdouble farval) {

   debugAssert(right == -left);
   debugAssert(top == -bottom);

   double x, y, a, b, c, d;
   double m[16];

   x = (2.0*nearval) / (right-left);
   y = (2.0*nearval) / (top-bottom);
   a = (right+left) / (right-left);
   b = (top+bottom) / (top-bottom);

   if ((float)farval >= (float)inf) {
       // Infinite view frustum
       c = -1.0;
       d = -2.0 * nearval;
   } else {
       c = -(farval+nearval) / (farval-nearval);
       d = -(2.0*farval*nearval) / (farval-nearval);
   }

   debugAssert(a == 0);
   debugAssert(b == 0);

#define M(row,col)  m[col*4+row]
   M(0,0) = x;    M(0,1) = 0.0;  M(0,2) = a;      M(0,3) = 0.0;
   M(1,0) = 0.0;  M(1,1) = y;    M(1,2) = b;      M(1,3) = 0.0;
   M(2,0) = 0.0;  M(2,1) = 0.0;  M(2,2) = c;      M(2,3) = d;
   M(3,0) = 0.0;  M(3,1) = 0.0;  M(3,2) = -1.0;   M(3,3) = 0.0;
#undef M

   glLoadMatrixd(m);
}


static double getTime() {
    return SDL_GetTicks() / 1000.0;
}


#ifdef G3D_WIN32
/**
 Used by the Windows version of getDriverVersion().
 @cite Based on code by Ted Peck tpeck@roundwave.com http://www.codeproject.com/dll/ShowVer.asp
 */
struct VS_VERSIONINFO { 
    WORD                wLength; 
    WORD                wValueLength; 
    WORD                wType; 
    WCHAR               szKey[1]; 
    WORD                Padding1[1]; 
    VS_FIXEDFILEINFO    Value; 
    WORD                Padding2[1]; 
    WORD                Children[1]; 
};
#endif

/**
 Returns the version string for the video driver.

 @cite Based in part on code by Ted Peck tpeck@roundwave.com http://www.codeproject.com/dll/ShowVer.asp
 */
static std::string getDriverVersion() {
    #ifdef G3D_WIN32
    
        std::string driver;

        std::string vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

        // Locate the windows\system directory
        {
            char sysDir[1024];
            int sysSize = GetSystemDirectory(sysDir, 1024);
            if (sysSize == 0) {
                return "Unknown (can't find Windows directory)";
            }
            driver = sysDir;
        }

        if (vendor == "ATI Technologies Inc.") {

            driver = driver + "\\ati2dvag.dll";

        } else if (vendor =="NVIDIA Corporation") {

            driver = driver + "\\nv4_disp.dll";

        } else {

            return "Unknown (Unknown vendor)";

        }

        char* lpdriver = const_cast<char*>(driver.c_str());
        DWORD dummy;

        int size = GetFileVersionInfoSize(lpdriver, &dummy);
        if (size == 0) {
            return "Unknown (Can't find driver)";
        }

        void* buffer = new uint8[size];

        if (GetFileVersionInfo(lpdriver, NULL, size, buffer) == 0) {
            delete buffer;
            return "Unknown";
        }

	    // Interpret the VS_VERSIONINFO header pseudo-struct
	    VS_VERSIONINFO* pVS = (VS_VERSIONINFO*)buffer;
        debugAssert(!wcscmp(pVS->szKey, L"VS_VERSION_INFO"));

	    uint8* pVt = (uint8*) &pVS->szKey[wcslen(pVS->szKey) + 1];

        #define roundoffs(a,b,r)	(((uint8*)(b) - (uint8*)(a) + ((r) - 1)) & ~((r) - 1))
        #define roundpos(b, a, r)	(((uint8*)(a)) + roundoffs(a, b, r))

	    VS_FIXEDFILEINFO* pValue = (VS_FIXEDFILEINFO*) roundpos(pVt, pVS, 4);

        #undef roundoffs
        #undef roundpos

        std::string result = "Unknown (No information)";

	    if (pVS->wValueLength) {
	        result = format("%d.%d.%d.%d",
                pValue->dwProductVersionMS >> 16,
                pValue->dwProductVersionMS & 0xFFFF,
	            pValue->dwProductVersionLS >> 16,
                pValue->dwProductVersionLS & 0xFFFF);
        }

        delete buffer;

        return result;
    #else
        return "Unknown";
    #endif
}


RenderDevice::RenderDevice() {

    _initialized = false;
    inPrimitive = false;
    _numTextureUnits = 0;
    emwaFrameRate = 0;
    lastTime = getTime();


	if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0 ) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		debugPrintf("Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
}

void RenderDevice::initGLExtensions() {

    #define LOAD_EXTENSION(name) \
       if (debugLog) {debugLog->print("Loading " #name " extension");} \
        *((void**)&name) = glGetProcAddress(#name); \
       if (debugLog) {debugLog->printf("(0x%x)\n", #name);}


    LOAD_EXTENSION(glMultiTexCoord2fARB);
    LOAD_EXTENSION(glMultiTexCoord1fARB);
    LOAD_EXTENSION(glMultiTexCoord2fvARB);
    LOAD_EXTENSION(glMultiTexCoord3fvARB);
    LOAD_EXTENSION(glMultiTexCoord4fvARB);
    LOAD_EXTENSION(glMultiTexCoord1dARB);
    LOAD_EXTENSION(glMultiTexCoord2dvARB);
    LOAD_EXTENSION(glMultiTexCoord3dvARB);
    LOAD_EXTENSION(glMultiTexCoord4dvARB);
    LOAD_EXTENSION(glActiveTextureARB);
    LOAD_EXTENSION(glClientActiveTextureARB);
    LOAD_EXTENSION(wglSwapIntervalEXT);
    LOAD_EXTENSION(wglChoosePixelFormatARB);
    LOAD_EXTENSION(wglAllocateMemoryNV);
    LOAD_EXTENSION(wglFreeMemoryNV);
    LOAD_EXTENSION(glVertexArrayRangeNV);
    LOAD_EXTENSION(glCompressedTexImage2DARB);
    LOAD_EXTENSION(glGetCompressedTexImageARB);
    LOAD_EXTENSION(glGenFencesNV);
    LOAD_EXTENSION(glDeleteFencesNV);
    LOAD_EXTENSION(glSetFenceNV);
    LOAD_EXTENSION(glFinishFenceNV);
    LOAD_EXTENSION(glGenProgramsARB);
    LOAD_EXTENSION(glBindProgramARB);
    LOAD_EXTENSION(glDeleteProgramsARB);
    LOAD_EXTENSION(glProgramStringARB);
    LOAD_EXTENSION(glProgramEnvParameter4fARB);
    LOAD_EXTENSION(glProgramLocalParameter4fARB);
    LOAD_EXTENSION(glProgramEnvParameter4dvARB);
    LOAD_EXTENSION(glProgramLocalParameter4dvARB);
    LOAD_EXTENSION(glVertexAttribPointerARB);
    LOAD_EXTENSION(glEnableVertexAttribArrayARB);
    LOAD_EXTENSION(glDisableVertexAttribArrayARB);
    LOAD_EXTENSION(glPointParameterfARB);
    LOAD_EXTENSION(glPointParameterfvARB);
    LOAD_EXTENSION(glMultiDrawArraysEXT);
    LOAD_EXTENSION(glMultiDrawElementsEXT);

    #undef LOAD_EXTENSION
}


/**
 Used by RenderDevice::init.
 */
static const char* isOk(bool x) {
    return x ? "ok" : "UNSUPPORTED";
}

/**
 Used by RenderDevice::init.
 */
static const char* isOk(void* x) {
    // GCC incorrectly claims this function is not called.
    return isOk(x != NULL);
}


bool RenderDevice::supportsOpenGLExtension(
    const std::string& extension) const {

    return extensionSet.contains(extension);
}


bool RenderDevice::init(
    int                 width, 
    int                 height,
    Log*                log,
    double              lightSaturation,
    bool                fullscreen,
    size_t              varVideoMemory,
    bool                asyncVideoRefresh,
    int                 dcolorBits,
    int                 dalphaBits,
    int                 ddepthBits,
    int                 dstencilBits) {

    debugAssert(! initialized());

    std::istringstream extensions;

    debugLog = log;

    beginEndFrame = 0;
    if (debugLog) {debugLog->section("Initialization");}

    debugAssert((lightSaturation >= 0.5) && (lightSaturation <= 2.0));

    // Under Windows, reset the last error so that our debug box
    // gives the correct results
    #if _WIN32
        SetLastError(0);
    #endif

	const int minimumDepthBits    = iMin(16, ddepthBits);
	const int desiredDepthBits    = ddepthBits;

	const int minimumStencilBits  = dstencilBits;
	const int desiredStencilBits  = dstencilBits;

    const int desiredTextureUnits = 4;

    SDL_ShowCursor(SDL_DISABLE);

    if (debugLog) {debugLog->println("Setting video mode");}
	setVideoMode(width, height, minimumDepthBits, desiredDepthBits, 
		minimumStencilBits, desiredStencilBits, dcolorBits,dalphaBits, fullscreen);

    // Get the number of texture units
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &_numTextureUnits);

    int depthBits, stencilBits, redBits, greenBits, blueBits, alphaBits;
    glGetIntegerv(GL_DEPTH_BITS, &depthBits);
    glGetIntegerv(GL_STENCIL_BITS, &stencilBits);

    glGetIntegerv(GL_RED_BITS,   &redBits);
    glGetIntegerv(GL_GREEN_BITS, &greenBits);
    glGetIntegerv(GL_BLUE_BITS,  &blueBits);
    glGetIntegerv(GL_ALPHA_BITS, &alphaBits);

    bool depthOk   = depthBits >= minimumDepthBits;
    bool stencilOk = stencilBits >= minimumStencilBits;

    extensions.str((char*)glGetString(GL_EXTENSIONS));
    {
        // Parse the extensions into the supported set
        std::string s;
        while(extensions >> s)
        {
            extensionSet.insert(s);
        }

        stencilWrapSupported        = supportsOpenGLExtension("EXT_stencil_wrap");
        textureRectangleSupported   = supportsOpenGLExtension("GL_NV_texture_rectangle");
        _supportsVertexProgram      = supportsOpenGLExtension("GL_ARB_vertex_program");
        _supportsFragmentProgram    = supportsOpenGLExtension("GL_ARB_fragment_program");
    }


	varSystem = new VARSystem(this, varVideoMemory, debugLog);

    if (debugLog) {
        debugLog->printf("Operating System: %s\n", System::operatingSystem().c_str());
        debugLog->printf("Processor Architecture: %s\n\n", System::cpuArchitecture().c_str());
        std::string s = getDriverVersion();

        debugLog->printf(
            "GL Vendor:     %s\n"
            "GL Renderer:   %s\n"
            "GL Version:    %s\n",
            "Driver version: %s\n\n",
            "GL extensions: \"%s\"\n",
            glGetString(GL_VENDOR),
            glGetString(GL_RENDERER),
            glGetString(GL_VERSION),
            s.c_str(),
            extensions.str().c_str());
    }
 
    // Don't use more texture units than allowed at compile time.
    int rawTextureUnits = _numTextureUnits;
    _numTextureUnits = iMin(MAX_TEXTURE_UNITS, _numTextureUnits);
    
    if (debugLog) {

    debugLog->printf(
             "Capability    Minimum   Desired   Received  Ok?\n"
             "-------------------------------------------------\n"
             "* RENDER DEVICE \n"
             "Depth       %4d bits %4d bits %4d bits   %s\n"
             "Stencil     %4d bits %4d bits %4d bits   %s\n"
             "Alpha                           %4d bits   %s\n"
             "Red                             %4d bits   %s\n"
             "Green                           %4d bits   %s\n"
             "Blue                            %4d bits   %s\n"
             "TextureUnits              %4d    %3d/%3d   %s\n"

             "Width             %8d pixels           %s\n"
             "Height            %8d pixels           %s\n"
             "Mode                 %10s             %s\n\n"

             "* GL EXTENSIONS\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n"
             "%31s             %s\n\n"

             "* JOYSTICK\n"
             "Number                              %4d    %s\n\n",
                         

             minimumDepthBits, desiredDepthBits, depthBits, isOk(depthOk),
             minimumStencilBits, desiredStencilBits, stencilBits, isOk(stencilOk),

             alphaBits, "ok",
             redBits, "ok", 
             greenBits, "ok", 
             blueBits, "ok", 

             desiredTextureUnits, _numTextureUnits, rawTextureUnits, isOk(_numTextureUnits >= desiredTextureUnits),

             width, "ok",
             height, "ok",
             (fullscreen ? "Fullscreen" : "Windowed"), "ok",
             
             "glMultiTexCoord2fvARB", isOk(glMultiTexCoord2fvARB),
             "glMultiTexCoord2fARB", isOk(glMultiTexCoord2fARB),
             "glMultiTexCoord4dvARB", isOk(glMultiTexCoord4dvARB),
             "glActiveTextureARB", isOk(glActiveTextureARB),
			 "glClientActiveTextureARB", isOk(glClientActiveTextureARB),
             "wglSwapIntervalEXT", isOk(wglSwapIntervalEXT),
             "wglAllocateMemoryNV", isOk(wglAllocateMemoryNV),
             "wglFreeMemoryNV", isOk(wglFreeMemoryNV),
             "glVertexArrayRangeNV", isOk(glVertexArrayRangeNV),
             "glCompressedTexImage2DARB", isOk(glCompressedTexImage2DARB),
             "glGetCompressedTexImageARBOk", isOk(glGetCompressedTexImageARB),
             "GL_ARB_texture_compression", isOk(supportsOpenGLExtension("GL_ARB_texture_compression")),
             "GL_EXT_texture_compression_s3tc", isOk(supportsOpenGLExtension("GL_EXT_texture_compression_s3tc")),
			 "glGenFencesNV", isOk(glGenFencesNV),
 			 "glDeleteFencesNV", isOk(glDeleteFencesNV),
			 "glSetFenceNV", isOk(glSetFenceNV),
			 "glFinishFenceNV", isOk(glFinishFenceNV),
             "glGenProgramsARB", isOk(glGenProgramsARB),
             "glBindProgramARB", isOk(glBindProgramARB),
             "glDeleteProgramsARB", isOk(glDeleteProgramsARB),
             "glProgramStringARB", isOk(glProgramStringARB),
             "glProgramEnvParameter4fARB", isOk(glProgramEnvParameter4fARB),
             "glProgramLocalParameter4fARB", isOk(glProgramLocalParameter4fARB),
             "glIsProgramARB", isOk(glIsProgramARB),
             "glPointParameterfARB", isOk(glPointParameterfARB),
             "glPointParameterfvARB", isOk(glPointParameterfvARB),
             "glMultiDrawArraysEXT", isOk(glMultiDrawArraysEXT),
             "glMultiDrawElementsEXT", isOk(glMultiDrawElementsEXT),

             SDL_NumJoysticks(), "ok"
             );
    }


    // Set the refresh rate
    if (wglSwapIntervalEXT != NULL) {
        if (debugLog) {
            if (asyncVideoRefresh) {
                debugLog->printf("wglSwapIntervalEXT(0);\n");
            } else {
                debugLog->printf("wglSwapIntervalEXT(1);\n");
            }
        }
        wglSwapIntervalEXT(asyncVideoRefresh ? 0 : 1);
    }

    SDL_EnableUNICODE(1);

    // Cyan background
	glClearColor(0.0f, 0.8f, 1.0f, 0.0f);

	glClearDepth(1.0);

    {
        if (debugLog) debugLog->printf("Setting brightness to %g\n", lightSaturation);
        // Adjust the gamma so that changing the 
        // light intensities won't affect the actual screen
        // brightness.  This method due to John Carmack.
        this->lightSaturation = lightSaturation;
        brightScale = 1.0 / lightSaturation;
        setGamma(lightSaturation, 1.0);
        if (debugLog) debugLog->println("Brightness set.");
    }

    // Enable proper specular lighting
    if (debugLog) debugLog->println("Enabling separate specular lighting.\n");
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SEPARATE_SPECULAR_COLOR_EXT);

    // Make sure we use good interpolation
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    //glHint(GL_GENERATE_MIPMAP_HINT_EXT, GL_NICEST);

    if (debugLog) debugLog->println("Setting initial rendering state.\n");
    glDisable(GL_LIGHT0);
    {
        // WARNING: this must be kept in sync with the 
        // RenderState constructor
        state = RenderState(screenWidth, screenHeight);

        glViewport(state.viewport.x, state.viewport.y, state.viewport.width, state.viewport.height);
        glDepthMask(GL_TRUE);
        glColorMask(1,1,1,0);
        glStencilMask(0x00);

        glDepthFunc(GL_LESS);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
        glDisable(GL_ALPHA_TEST);

        glDisable(GL_BLEND);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glLineWidth(1);
        glPointSize(1);

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, state.ambient);

        glDisable(GL_LIGHTING);
        glColor4d(1,1,1,1);
        glNormal3d(0,0,0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glShadeModel(GL_FLAT);

        glClearStencil(0);
        glClearDepth(1);
        glClearColor(0,0,0,1);
        glMatrixMode(GL_PROJECTION);
        frustum(
            state.projectionMatrixParams[0],
            state.projectionMatrixParams[1],
            state.projectionMatrixParams[2], 
            state.projectionMatrixParams[3],
            state.projectionMatrixParams[4],
            state.projectionMatrixParams[5]);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glDepthRange(0, 1);

        // Set up the texture units.
        for (int t = _numTextureUnits - 1; t >= 0; --t) {
            double d[] = {0,0,0,1};
            glMultiTexCoord4dvARB(GL_TEXTURE0_ARB + t, d);
        }

        glActiveTextureARB(GL_TEXTURE0_ARB);

    }

	inPrimitive        = false;
	inIndexedPrimitive = false;

    if (debugLog) debugLog->println("Done initializing RenderDevice.\n");

    _initialized = true;
    return true;
}


bool RenderDevice::initialized() const {
    return _initialized;
}


#ifdef _WIN32

extern HWND SDL_Window;

HDC RenderDevice::getWindowHDC() const {
    // Get Windows HDC
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

    HDC hdc = GetDC(info.window);

    if (hdc == 0) {
        if (debugLog) {
            debugLog->println("hdc == 0");
        }
        debugAssert(hdc != 0);
    }

    if (debugLog) {debugLog->println("HDC acquired.");}
    return hdc;
}

#endif

void RenderDevice::setGamma(
    double              brightness,
    double              gamma) {
    
    uint16 gammaRamp[256];
    uint16 rgbGammaRamp[256 * 3];

    for (int i = 0; i < 256; ++i) {
        gammaRamp[i] =
            (uint16)min(65535, 
                      max(0, 
                          pow((brightness * (i + 1)) / 256.0, gamma) * 
                          65535 + Real(0.5)));

        rgbGammaRamp[i] = gammaRamp[i];
        rgbGammaRamp[i + 256] = gammaRamp[i];
        rgbGammaRamp[i + 512] = gammaRamp[i];
	}


    #ifdef WIN32
        BOOL success = SetDeviceGammaRamp(getWindowHDC(), rgbGammaRamp);
    #else
        bool success = (SDL_SetGammaRamp(gammaRamp, gammaRamp, gammaRamp) != -1);
    #endif
    
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


void RenderDevice::setVideoMode(
    int width, 
    int height, 
	int minimumDepthBits, 
    int desiredDepthBits, 
	int minimumStencilBits, 
    int desiredStencilBits,
    int colorBits,
    int alphaBits,
    bool fullscreen) {

	// Request various OpenGL parameters
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, desiredDepthBits);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, GL_TRUE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, desiredStencilBits);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, colorBits);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, colorBits);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, colorBits);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, alphaBits);

	// Create a width x height OpenGL screen 
    int flags =  SDL_HWSURFACE | SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0);
	if (SDL_SetVideoMode(width, height, 0, flags) == NULL) {
        debugAssert(false);
        if (debugLog) {debugLog->printf("Unable to create OpenGL screen: %s\n", SDL_GetError());}
		error("Critical Error", format("Unable to create OpenGL screen: %s\n", SDL_GetError()).c_str(), true);
		SDL_Quit();
		exit(2);
	}

    initGLExtensions();

	screenWidth = width;
	screenHeight = height;

    setCaption("Graphics3D");

	glViewport(0, 0, screenWidth, screenHeight);
}


void RenderDevice::setCaption(const std::string& caption) {
	// Set the title bar
	SDL_WM_SetCaption(caption.c_str(), NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////


void RenderDevice::configureDirectionalLight(
    int                 lightNum,
    const Vector3&      toLightVector,
    const Color3&       color) {

    GLfloat position[] =
        {toLightVector.x,
         toLightVector.y,
         toLightVector.z,
         0.0f};

    GLfloat brightness[] =
        {color.r / lightSaturation,
         color.g / lightSaturation,
         color.b / lightSaturation,
         1.0f};

    lightNum += GL_LIGHT0;

    static GLfloat zero[] = {0.0f, 0.0f, 0.0f, 0.0f};

    int mm = glGetInteger(GL_MATRIX_MODE);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glLoadMatrix(state.cameraToWorldMatrix.inverse());
    glLightfv(lightNum, GL_POSITION,              position);
    glLightfv(lightNum, GL_AMBIENT,               zero);
    glLightfv(lightNum, GL_DIFFUSE,               brightness);
    glLightfv(lightNum, GL_SPECULAR,              brightness);
    glLightf (lightNum, GL_CONSTANT_ATTENUATION,  1);
    glLightf (lightNum, GL_LINEAR_ATTENUATION,    0);
    glLightf (lightNum, GL_QUADRATIC_ATTENUATION, 0);
    glPopMatrix();
    glMatrixMode(mm);
}


void RenderDevice::configurePointLight(
    int                 lightNum,
    const Vector3&      position,
    const Color3&       color,
    double              constantAttenuationCoef,
    double              linearAttenuationCoef,
    double              quadraticAttenuationCoef) {

    GLfloat pos[] = {position.x, position.y, position.z, 1.0f};

    GLfloat brightness[] =
        {color.r / lightSaturation,
         color.g / lightSaturation,
         color.b / lightSaturation,
         1.0f};

    static GLfloat zero[] = {0.0f, 0.0f, 0.0f, 0.0f};

    glLightfv(lightNum, GL_POSITION,              pos);
    glLightf (lightNum, GL_CONSTANT_ATTENUATION,  constantAttenuationCoef);
    glLightf (lightNum, GL_LINEAR_ATTENUATION,    linearAttenuationCoef);
    glLightf (lightNum, GL_QUADRATIC_ATTENUATION, quadraticAttenuationCoef);
    glLightfv(lightNum, GL_AMBIENT,               zero);
    glLightfv(lightNum, GL_DIFFUSE,               brightness);
    glLightfv(lightNum, GL_SPECULAR,              brightness);
}


Vector3 RenderDevice::project(const Vector3& v) const {
    return project(Vector4(v, 1));
}


Vector3 RenderDevice::project(const Vector4& v) const {
    // Get the matrices
    double modelView[16];
    double projection[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    // Compose the matrices into a net row-major transformation
    Vector4 transformation[4];
    int r;
    for (r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            transformation[r][c] = 0;
            for (int i = 0; i < 4; ++i) {
                // OpenGL matrices are column major
                transformation[r][c] += projection[r + i * 4] * modelView[i + c * 4];
            }
        }
    }

    // Transform the vertex
    Vector4 result;
    for (r = 0; r < 4; ++r) {
        result[r] = transformation[r].dot(v);
    }

    // Homogeneous division
    const double rhw = 1 / result.w;
    return Vector3((result.x * rhw + 1) * getWidth() / 2,
                   (1 - result.y * rhw) * getHeight() / 2,
                   rhw);
}


void RenderDevice::setAmbientLightLevel(
    const Color3&       color) {

    float c[] =
        {color.r / lightSaturation,
         color.g / lightSaturation,
         color.b / lightSaturation,
         1.0f};

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, c);
}


void RenderDevice::cleanup() {
    debugAssert(initialized());
	delete varSystem;
	varSystem = NULL;

    if (debugLog) {debugLog->println("Restoring gamma.");}
    setGamma(1, 1);

    if (debugLog) {debugLog->println("Shutting down SDL.");}
    SDL_Quit();
}


void RenderDevice::push2D() {
    push2D(Rect2D(0, 0, getWidth(), getHeight()));
}


void RenderDevice::push2D(const Rect2D& viewport) {
    pushState();
    resetState();
    setDepthTest(DEPTH_ALWAYS_PASS);
    disableLighting();
    setCullFace(CULL_NONE);
    disableDepthWrite();
    setProjectionMatrix2D(viewport.x, viewport.width - 1, viewport.height - 1, viewport.y, -1, 1); 
}


void RenderDevice::pop2D() {
    popState();
}

////////////////////////////////////////////////////////////////////////////////////////////////
RenderDevice::RenderState::RenderState(int width, int height) {

    // WARNING: this must be kept in sync with the initialization code
    // in init();
    viewport                    = Rect2D(0, 0, width, height);

    depthWrite                  = true;
    colorWrite                  = true;
    alphaWrite                  = false;

    depthTest                   = DEPTH_LESS;
    stencilTest                 = STENCIL_ALWAYS_PASS;
    stencilReference            = 0;
    alphaTest                   = ALPHA_ALWAYS_PASS;
    alphaReference              = 0.0;

    srcBlendFunc                = BLEND_ONE;
    dstBlendFunc                = BLEND_ZERO;

    stencilFail                 = STENCIL_KEEP;
    stencilZFail                = STENCIL_KEEP;
    stencilZPass                = STENCIL_KEEP;

    polygonOffset               = 0;
    lineWidth                   = 1;
    pointSize                   = 1;

    ambient                     = Color4(0.25, 0.25, 0.25, 1.0);

    lighting                    = false;
    color                       = Color4(1,1,1,1);
    normal                      = Vector3(0,0,0);

    // Note: texture units initialize themselves

    objectToWorldMatrix         = CoordinateFrame();
    cameraToWorldMatrix         = CoordinateFrame();

    stencilClear                = 0;
    depthClear                  = 1;
    colorClear                  = Color4(0,0,0,1);

    shadeMode                   = SHADE_FLAT;

    vertexProgram               = NULL;
    pixelProgram                = NULL;

    // Set projection matrix
    double aspect;
    aspect = viewport.width / viewport.height;

    proj3D                      = true;
    projectionMatrixParams[0]   = -aspect;
    projectionMatrixParams[1]   = aspect;
    projectionMatrixParams[2]   = -1.0;
    projectionMatrixParams[3]   = 1.0;
    projectionMatrixParams[4]   = 0.1;
    projectionMatrixParams[5]   = 100.0;

    cullFace                    = CULL_BACK;

    lowDepthRange               = 0;
    highDepthRange              = 1;
}


RenderDevice::RenderState::TextureUnit::TextureUnit() : texture(NULL) {
    texCoord        = Vector4(0,0,0,1);
    combineMode     = TEX_MODULATE;

    // Identity texture matrix
    memset(textureMatrix, 0, sizeof(double) * 16);
    for (int i = 0; i < 4; ++i) {
        textureMatrix[i + i * 4] = 1.0;
    }
}


void RenderDevice::pushState() {

    debugAssert(! inPrimitive);

    stateStack.push(state);
}

void RenderDevice::resetState() {
    setState(RenderState(screenWidth, screenHeight));
}


void RenderDevice::popState() {
    debugAssert(! inPrimitive);
    setState(stateStack.pop());
}


void RenderDevice::setState(
    const RenderState&          newState) {

    // The state change checks inside the individual
    // methods will (for the most part) minimize
    // the state changes so we can set all of the
    // new state explicitly.

    if (! memcmp(&newState, &state, sizeof(RenderState))) {
        // The new state is identical to the old state.
        return;
    }

    setViewport(newState.viewport);
    
    if (newState.depthWrite) {
        enableDepthWrite();
    } else {
        disableDepthWrite();
    }

    if (newState.colorWrite) {
        enableColorWrite();
    } else {
        disableColorWrite();
    }

    if (newState.alphaWrite) {
        enableAlphaWrite();
    } else {
        disableAlphaWrite();
    }

    setShadeMode(newState.shadeMode);
    setDepthTest(newState.depthTest);

    setStencilTest(newState.stencilTest, newState.stencilReference);
    setAlphaTest(newState.alphaTest, newState.alphaReference);

    setBlendFunc(newState.srcBlendFunc, newState.dstBlendFunc);

    setPolygonOffset(newState.polygonOffset);
    setLineWidth(newState.lineWidth);
    setPointSize(newState.pointSize);

    if (newState.lighting) {
        enableLighting();
    } else {
        disableLighting();
    }

    setStencilOp(newState.stencilFail, newState.stencilZFail, newState.stencilZPass);

    setColor(newState.color);
    setNormal(newState.normal);

    for (int u = _numTextureUnits - 1; u >= 0; --u) {
        if (memcmp(&(newState.textureUnit[u]), &(state.textureUnit[u]), sizeof(RenderState::TextureUnit))) {
            setTexture(u, newState.textureUnit[u].texture);
            setTexCoord(u, newState.textureUnit[u].texCoord);
            setTextureCombineMode(u, newState.textureUnit[u].combineMode);
            setTextureMatrix(u, newState.textureUnit[u].textureMatrix);
        }
    }

    if (memcmp(&newState.cameraToWorldMatrix, &state.cameraToWorldMatrix, sizeof(CoordinateFrame)) ||
        memcmp(&newState.objectToWorldMatrix, &state.objectToWorldMatrix, sizeof(CoordinateFrame))) {
        setObjectToWorldMatrix(newState.objectToWorldMatrix);
        setCameraToWorldMatrix(newState.cameraToWorldMatrix);
    }

    if (supportsVertexProgram()) {
        setVertexProgram(newState.vertexProgram);
    }

    if (supportsPixelProgram()) {
        setPixelProgram(newState.pixelProgram);
    }

    setStencilClearValue(newState.stencilClear);
    setDepthClearValue(newState.depthClear);
    setColorClearValue(newState.colorClear);

    if (newState.proj3D) {
        setProjectionMatrix3D(newState.projectionMatrixParams[0], newState.projectionMatrixParams[1], newState.projectionMatrixParams[2], newState.projectionMatrixParams[3], newState.projectionMatrixParams[4], newState.projectionMatrixParams[5]);
    } else {
        setProjectionMatrix2D(newState.projectionMatrixParams[0], newState.projectionMatrixParams[1], newState.projectionMatrixParams[2], newState.projectionMatrixParams[3], newState.projectionMatrixParams[4], newState.projectionMatrixParams[5]);
    }

    setCullFace(newState.cullFace);

    setDepthRange(newState.lowDepthRange, newState.highDepthRange);
}


void RenderDevice::setShadeMode(ShadeMode s) {
    if (s != state.shadeMode) {
        state.shadeMode = s;
        if (s == SHADE_FLAT) {
            glShadeModel(GL_FLAT);
        } else {
            glShadeModel(GL_SMOOTH);
        }
    }
}


void RenderDevice::setDepthRange(
    double              low,
    double              high) {

    if ((state.lowDepthRange != low) ||
        (state.highDepthRange != high)) {
        glDepthRange(low, high);
        state.lowDepthRange = low;
        state.highDepthRange = high;
    }
}


void RenderDevice::setCullFace(CullFace f) {
    if (f != state.cullFace) {
        switch (f) {
        case CULL_FRONT:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            break;

        case CULL_BACK:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;

        case CULL_NONE:
            glDisable(GL_CULL_FACE);
            break;

        default:
            debugAssertM(false, "Fell through switch");
        }

        state.cullFace = f;
    }
}

void RenderDevice::clear(bool clearColor, bool clearDepth, bool clearStencil) {
    debugAssert(! inPrimitive);

    GLint mask = 0;

    pushState();
    if (clearColor) {
        mask |= GL_COLOR_BUFFER_BIT;
        enableColorWrite();
    }

    if (clearDepth) {
        mask |= GL_DEPTH_BUFFER_BIT;
        enableDepthWrite();
    }

    int oldMask;
    glGetIntegerv(GL_STENCIL_WRITEMASK, &oldMask);

    if (clearStencil) {
        mask |= GL_STENCIL_BUFFER_BIT;
        glStencilMask(~0);
    }

    glClear(mask);
    glStencilMask(oldMask);
    popState();
}


uint RenderDevice::numTextureUnits() const {
    return _numTextureUnits;
}

void RenderDevice::beginFrame() {
    ++beginEndFrame;
    triangleCount = 0;
    debugAssertM(beginEndFrame == 1, "Mismatched calls to beginFrame/endFrame");
}


void RenderDevice::endFrame() {
    --beginEndFrame;
    debugAssertM(beginEndFrame == 0, "Mismatched calls to beginFrame/endFrame");
    SDL_GL_SwapBuffers();

    double now = getTime();
    double dt = now - lastTime;
    if (dt == 0) {
        dt = 0.001;
    }
    emwaFrameRate     = (emwaFrameRate * 0.9) + (0.1 / dt);
    emwaTriangleRate  = (emwaTriangleRate * 0.9) + (0.1 * triangleCount / dt);
    emwaTriangleCount = (emwaTriangleCount * 0.9) + (0.1 * triangleCount);

    if ((emwaFrameRate == inf) || (isNaN(emwaFrameRate))) {
        emwaFrameRate = 1000000;
    } else if (emwaFrameRate < 0) {
        emwaFrameRate = 0;
    }

    if ((emwaTriangleRate == inf) || (isNaN(emwaTriangleRate))) {
        emwaTriangleRate = 1e20;
    } else if (emwaTriangleRate < 0) {
        emwaTriangleRate = 0;
    }

    if ((emwaTriangleCount == inf) || (emwaTriangleCount == nan)) {
        emwaTriangleRate = 1e20;
    } else if (emwaTriangleCount < 0) {
        emwaTriangleCount = 0;
    }

    lastTime = now;
}


void RenderDevice::setStencilClearValue(int s) {
    debugAssert(! inPrimitive);
    if (state.stencilClear != s) {
        glClearStencil(s);
        state.stencilClear = s;
    }
}


void RenderDevice::setDepthClearValue(double d) {
    debugAssert(! inPrimitive);
    if (state.depthClear != d) {
        glClearDepth(d);
        state.depthClear = d;
    }
}


void RenderDevice::setColorClearValue(const Color4& c) {
    debugAssert(! inPrimitive);
    if (state.colorClear != c) {
        glClearColor(c.r, c.g, c.b, c.a);
        state.colorClear = c;
    }
}


void RenderDevice::setViewport(const Rect2D& v) {
    if ((state.viewport.x != v.x) ||
        (state.viewport.y != v.y) ||
        (state.viewport.width != v.width) ||
        (state.viewport.height != v.height)) {

        glViewport(v.x, v.y, v.width, v.height);
        state.viewport = v;
    }
}

void RenderDevice::setDepthTest(DepthTest test) {
    debugAssert(! inPrimitive);

    if (state.depthTest != test) {
        if (test == DEPTH_ALWAYS_PASS) {
            glDisable(GL_DEPTH_TEST);
        } else {
            glEnable(GL_DEPTH_TEST);
            switch (test) {
            case DEPTH_LESS:
                glDepthFunc(GL_LESS);
                break;

            case DEPTH_LEQUAL:
                glDepthFunc(GL_LEQUAL);
                break;

            case DEPTH_GREATER:
                glDepthFunc(GL_GREATER);
                break;

            case DEPTH_GEQUAL:
                glDepthFunc(GL_GEQUAL);
                break;

            case DEPTH_EQUAL:
                glDepthFunc(GL_EQUAL);
                break;

            case DEPTH_NOTEQUAL:
                glDepthFunc(GL_NOTEQUAL);
                break;

            case DEPTH_NEVER_PASS:
                glDepthFunc(GL_NEVER);
                break;

            default:
                debugAssertM(false, "Fell through switch");
            }
        }

        state.depthTest = test;

    }
}


void RenderDevice::setStencilTest(StencilTest test, int reference) {
    debugAssert(! inPrimitive);

    if ((state.stencilTest != test) || (state.stencilReference != reference)) {
        if (test == STENCIL_ALWAYS_PASS) {

            // Can't actually disable if the stencil op is using the test as well
            if ((state.stencilFail == STENCIL_KEEP) &&
                (state.stencilZFail == STENCIL_KEEP) &&
                (state.stencilZPass == STENCIL_KEEP)) {
                glDisable(GL_STENCIL_TEST);
            }

        } else {

            glEnable(GL_STENCIL_TEST);
            switch (test) {
            case STENCIL_LESS:
                glStencilFunc(GL_LESS, reference, 0xFFFFFF);
                break;

            case STENCIL_LEQUAL:
                glStencilFunc(GL_LEQUAL, reference, 0xFFFFFF);
                break;

            case STENCIL_GREATER:
                glStencilFunc(GL_GREATER, reference, 0xFFFFFF);
                break;

            case STENCIL_GEQUAL:
                glStencilFunc(GL_GEQUAL, reference, 0xFFFFFF);
                break;

            case STENCIL_EQUAL:
                glStencilFunc(GL_EQUAL, reference, 0xFFFFFF);
                break;

            case STENCIL_NOTEQUAL:
                glStencilFunc(GL_NOTEQUAL, reference, 0xFFFFFF);
                break;

            case STENCIL_NEVER_PASS:
                glStencilFunc(GL_NEVER, reference, 0xFFFFFF);
                break;

            default:
                debugAssertM(false, "Fell through switch");
            }
        }

        state.stencilTest = test;
        state.stencilReference = reference;

    }
}


void RenderDevice::setAlphaTest(AlphaTest test, double reference) {
    debugAssert(! inPrimitive);

    if ((state.alphaTest != test) || (state.alphaReference != reference)) {
        if (test == ALPHA_ALWAYS_PASS) {
            
            glDisable(GL_ALPHA_TEST);

        } else {

            glEnable(GL_ALPHA_TEST);
            switch (test) {
            case ALPHA_LESS:
                glAlphaFunc(GL_LESS, reference);
                break;

            case ALPHA_LEQUAL:
                glAlphaFunc(GL_LEQUAL, reference);
                break;

            case ALPHA_GREATER:
                glAlphaFunc(GL_GREATER, reference);
                break;

            case ALPHA_GEQUAL:
                glAlphaFunc(GL_GEQUAL, reference);
                break;

            case ALPHA_EQUAL:
                glAlphaFunc(GL_EQUAL, reference);
                break;

            case ALPHA_NOTEQUAL:
                glAlphaFunc(GL_NOTEQUAL, reference);
                break;

            case ALPHA_NEVER_PASS:
                glAlphaFunc(GL_NEVER, reference);
                break;

            default:
                debugAssertM(false, "Fell through switch");
            }
        }

        state.alphaTest = test;
        state.alphaReference = reference;
    }
}


void RenderDevice::enableAlphaWrite() {
    debugAssert(! inPrimitive);
    if (! state.alphaWrite) {
        GLint c = state.colorWrite ? GL_TRUE : GL_FALSE;
        glColorMask(c, c, c, GL_TRUE);
        state.alphaWrite = true;
    }
}


void RenderDevice::disableAlphaWrite() {
    debugAssert(! inPrimitive);
    if (state.alphaWrite) {
        GLint c = state.colorWrite ? GL_TRUE : GL_FALSE;
        glColorMask(c, c, c, GL_FALSE);
        state.alphaWrite = false;
    }
}

void RenderDevice::enableColorWrite() {
    debugAssert(! inPrimitive);
    if (! state.colorWrite) {
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, state.alphaWrite ? GL_TRUE : GL_FALSE);
        state.colorWrite = true;
    }
}


void RenderDevice::disableColorWrite() {
    debugAssert(! inPrimitive);

    if (state.colorWrite) {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, state.alphaWrite ? GL_TRUE : GL_FALSE);
        state.colorWrite = false;
    }
}


void RenderDevice::enableDepthWrite() {
    debugAssert(! inPrimitive);
    if (! state.depthWrite) {
        glDepthMask(GL_TRUE);
        state.depthWrite = true;
    }
}


void RenderDevice::disableDepthWrite() {
    debugAssert(! inPrimitive);
    if (state.depthWrite) {
        glDepthMask(GL_FALSE);
        state.depthWrite = false;
    }
}


GLint RenderDevice::toGLStencilOp(RenderDevice::StencilOp op) const {
    switch (op) {
    case RenderDevice::STENCIL_KEEP:
        return GL_KEEP;
       
    case RenderDevice::STENCIL_INCR_WRAP:
        if (stencilWrapSupported) {
            return GL_INCR_WRAP_EXT;
        }
        // Intentionally fall through

    case RenderDevice::STENCIL_INCR:
        return GL_INCR;

    case RenderDevice::STENCIL_DECR_WRAP:
        if (stencilWrapSupported) {
            return GL_DECR_WRAP_EXT;
        }
        // Intentionally fall through

    case RenderDevice::STENCIL_DECR:
        return GL_DECR;

    default:
        debugAssertM(false, "Fell through switch");
        return GL_KEEP;
    }
}


void RenderDevice::setVertexProgram(const VertexProgramRef& vp) {
    if (vp != state.vertexProgram) {
        state.vertexProgram = vp;
        if (vp == (VertexProgramRef)NULL) {
            glDisable(GL_VERTEX_PROGRAM_ARB);
        } else  {
            debugAssert(supportsVertexProgram());
            glEnable(GL_VERTEX_PROGRAM_ARB);
            glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vp->getOpenGLID());
        }
    }
}


void RenderDevice::setPixelProgram(const PixelProgramRef& pp) {
    if (pp != state.pixelProgram) {
        state.pixelProgram = pp;
        if (pp == (PixelProgramRef)NULL) {
            glDisable(GL_FRAGMENT_PROGRAM_ARB);
        } else  {
            debugAssert(supportsPixelProgram());
            glEnable(GL_FRAGMENT_PROGRAM_ARB);
            glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, pp->getOpenGLID());
        }
    }
}


void RenderDevice::setStencilOp(
    StencilOp           fail,
    StencilOp           zfail,
    StencilOp           zpass) {
    debugAssert(! inPrimitive);

    if ((fail  != state.stencilFail) ||
        (zfail != state.stencilZFail) ||
        (zpass != state.stencilZPass)) {

        glStencilOp(toGLStencilOp(fail), toGLStencilOp(zfail), toGLStencilOp(zpass));

        // Need to manage the mask as well
        if ((fail == STENCIL_KEEP) && (zpass == STENCIL_KEEP) && (zfail == STENCIL_KEEP)) {
            // Turn off writing.  May need to turn off the stencil test.
            glStencilMask(0);

            if (state.stencilTest == STENCIL_ALWAYS_PASS) {
                // Test doesn't need to be on
                glDisable(GL_STENCIL_TEST);
            }

        } else {
            // Turn on writing.  We also need to turn on the
            // stencil test in this case.
            glStencilMask(0xFFFFFFFF);

            if (state.stencilTest == STENCIL_ALWAYS_PASS) {
                // Test is not already on
                glEnable(GL_STENCIL_TEST);
                glStencilFunc(GL_ALWAYS, state.stencilReference, 0xFFFFFF);
            }
        }

        state.stencilFail = fail;
        state.stencilZFail = zfail;
        state.stencilZPass = zpass;
    }
}


static GLint toGLBlendFunc(RenderDevice::BlendFunc b) {
    switch (b) {
    case RenderDevice::BLEND_SRC_ALPHA:
        return GL_SRC_ALPHA;
        
    case RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA:
        return GL_ONE_MINUS_SRC_ALPHA;

    case RenderDevice::BLEND_ONE:
        return GL_ONE;
        
    case RenderDevice::BLEND_ZERO:
        return GL_ZERO;

    case RenderDevice::BLEND_SRC_COLOR:
        return GL_SRC_COLOR;

    case RenderDevice::BLEND_DST_COLOR:
        return GL_DST_COLOR;

    case RenderDevice::BLEND_ONE_MINUS_SRC_COLOR:
        return GL_ONE_MINUS_SRC_COLOR;

    default:
        debugAssertM(false, "Fell through switch");
        return GL_ZERO;
    }
}


void RenderDevice::setBlendFunc(
    BlendFunc src,
    BlendFunc dst) {
    debugAssert(! inPrimitive);

    if ((state.dstBlendFunc != dst) ||
        (state.srcBlendFunc != src)) {
        if ((dst == BLEND_ZERO) && (src == BLEND_ONE)) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
            glBlendFunc(toGLBlendFunc(src), toGLBlendFunc(dst));
        }
        state.dstBlendFunc = dst;
        state.srcBlendFunc = src;
    }
}


void RenderDevice::setLineWidth(
    double               width) {
    debugAssert(! inPrimitive);
    if (state.lineWidth != width) {
        glLineWidth(width);
        state.lineWidth = width;
    }
}


void RenderDevice::setPointSize(
    double               width) {
    debugAssert(! inPrimitive);
    if (state.pointSize != width) {
        glPointSize(width);
        state.pointSize = width;
    }
}


void RenderDevice::setAmbientLightColor(
    const Color4&        color) {
    debugAssert(! inPrimitive);

    if (color != state.ambient) {
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color);
        state.ambient = color;
    }
}


void RenderDevice::setAmbientLightColor(
    const Color3&        color) {
    setAmbientLightColor(Color4(color, 1.0));
}

void RenderDevice::enableLighting() {
    debugAssert(! inPrimitive);
    if (! state.lighting) {
        glEnable(GL_LIGHTING);
        state.lighting = true;
    }
}


void RenderDevice::disableLighting() {
    debugAssert(! inPrimitive);
    if (state.lighting) {
        glDisable(GL_LIGHTING);
        state.lighting = false;
    }
}


void RenderDevice::setObjectToWorldMatrix(
    const CoordinateFrame& cFrame) {
    
    debugAssert(! inPrimitive);

    state.objectToWorldMatrix = cFrame;
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrix(state.cameraToWorldMatrix.inverse() * state.objectToWorldMatrix);
}


CoordinateFrame RenderDevice::getObjectToWorldMatrix() const {
    return state.objectToWorldMatrix;
}


void RenderDevice::setCameraToWorldMatrix(
    const CoordinateFrame& cFrame) {

    debugAssert(! inPrimitive);
    
    state.cameraToWorldMatrix = cFrame;
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrix(state.cameraToWorldMatrix.inverse() * state.objectToWorldMatrix);
}


CoordinateFrame RenderDevice::getCameraToWorldMatrix() const {
    return state.cameraToWorldMatrix;
}


void RenderDevice::setProjectionMatrix3D(
    double l,
    double r,
    double b,
    double t,
    double n,
    double f) {
    debugAssert(! inPrimitive);

    double params[] = {l,r,b,t,n,f};

    if ((! state.proj3D) ||
        (memcmp(state.projectionMatrixParams, params, sizeof(double) * 6))) {

        glMatrixMode(GL_PROJECTION);
        frustum(l,r,b,t,n,f);
        memcpy(state.projectionMatrixParams, params, sizeof(double) * 6);
        state.proj3D = true;

    }
}



static void ortho(
    GLdouble            left,
    GLdouble            right,
    GLdouble            bottom,
    GLdouble            top,
    GLdouble            nearval,
    GLdouble            farval) {

   // Adapted from Mesa
   double x, y, z;
   double tx,  ty, tz;
   double m[16];

   x = 2.0 / (right-left);
   y = 2.0 / (top-bottom);
   z = -2.0 / (farval-nearval);
   tx = -(right+left) / (right-left);
   ty = -(top+bottom) / (top-bottom);
   tz = -(farval+nearval) / (farval-nearval);

#define M(row,col)  m[col*4+row]
   M(0,0) = x;    M(0,1) = 0.0;  M(0,2) = 0.0;  M(0,3) = tx;
   M(1,0) = 0.0;  M(1,1) = y;    M(1,2) = 0.0;  M(1,3) = ty;
   M(2,0) = 0.0;  M(2,1) = 0.0;  M(2,2) = z;    M(2,3) = tz;
   M(3,0) = 0.0;  M(3,1) = 0.0;  M(3,2) = 0.0;  M(3,3) = 1.0;
#undef M

   glLoadMatrixd(m);
}

void RenderDevice::setProjectionMatrix2D(
    double l,
    double r,
    double b,
    double t,
    double n,
    double f) {

    debugAssert(! inPrimitive);
    
    double params[] = {l,r,b,t,n,f};

    if ((state.proj3D) ||
        (memcmp(state.projectionMatrixParams, params, sizeof(double) * 6))) {

        glMatrixMode(GL_PROJECTION);
        ortho(l,r,b,t,n,f);

        memcpy(state.projectionMatrixParams, params, sizeof(double) * 6);
        state.proj3D = false;

    }
}


void RenderDevice::forceSetTextureMatrix(int unit, const double* m) {
    memcpy(state.textureUnit[unit].textureMatrix, m, sizeof(double)*16);
    glActiveTextureARB(GL_TEXTURE0_ARB + unit);

    // Transpose the texture matrix
    double tt[16];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            tt[i + j * 4] = m[j + i * 4];
        }
    }
    glMatrixMode(GL_TEXTURE);
    glLoadMatrixd(tt);

    TextureRef texture = state.textureUnit[unit].texture;

    // invert y
    if ((texture != NULL) && texture->invertY) {

        double ymax = 1.0;
    
        if (texture->getDimension() == Texture::DIM_2D_RECT) {
            ymax = texture->getTexelHeight();
        }

        double m[16] = 
        { 1,  0,  0,  0,
          0, -1,  0,  0,
          0,  0,  1,  0,
          0,  ymax,  0,  1};

        glMultMatrixd(m);
    }

}


void RenderDevice::setTextureMatrix(
    uint                 unit,
    const double*        m) {

    debugAssert(! inPrimitive);
    debugAssertM(unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));

    if (memcmp(m, state.textureUnit[unit].textureMatrix, sizeof(double)*16)) {
        forceSetTextureMatrix(unit, m);
    }

}


void RenderDevice::setTextureMatrix(
    uint                    unit,
    const CoordinateFrame&  c) {

    double m[16] = 
    {c.rotation[0][0], c.rotation[0][1], c.rotation[0][2], c.translation.x,
     c.rotation[1][0], c.rotation[1][1], c.rotation[1][2], c.translation.y,
     c.rotation[2][0], c.rotation[2][1], c.rotation[2][2], c.translation.z,
                    0,                0,                0,               1};

    setTextureMatrix(unit, m);
}


void RenderDevice::setTextureCombineMode(
    uint                    unit,
    const CombineMode       mode) {

    debugAssertM(unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));

    if ((state.textureUnit[unit].combineMode != mode)) {

        state.textureUnit[unit].combineMode = mode;

        glActiveTextureARB(GL_TEXTURE0_ARB + unit);

        switch (mode) {
        case TEX_REPLACE:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            break;

        case TEX_MODULATE:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            break;

        case TEX_INTERPOLATE:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
            break;

        case TEX_ADD:
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
            break;

        default:
            debugAssertM(false, "Unrecognized texture combine mode");
        }
    }
}


void RenderDevice::resetTextureUnit(
    uint                    unit) {
    debugAssertM(unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));

    RenderState newState(state);
    state.textureUnit[unit] = RenderState::TextureUnit();
    setState(newState);
}


void RenderDevice::setPolygonOffset(
    double              offset) {
    debugAssert(! inPrimitive);

    if (state.polygonOffset != offset) {
        if (offset != 0) {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glEnable(GL_POLYGON_OFFSET_POINT);
            glPolygonOffset(offset, 0);
        } else {
            glDisable(GL_POLYGON_OFFSET_POINT);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glDisable(GL_POLYGON_OFFSET_LINE);
        }
        state.polygonOffset = offset;
    }
}


void RenderDevice::setColor(const Color4& color) {
    state.color = color;
    glColor4fv(state.color);
}


void RenderDevice::setColor(const Color3& color) {
    state.color = Color4(color, 1);
    glColor3fv(state.color);
}


void RenderDevice::setNormal(const Vector3& normal) {
    state.normal = normal;
    glNormal3fv(normal);
}


void RenderDevice::setTexCoord(uint unit, const Vector4& texCoord) {
    debugAssertM(unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));

    state.textureUnit[unit].texCoord = texCoord;
    glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
}


void RenderDevice::setTexCoord(uint unit, const Vector3& texCoord) {
    debugAssertM(unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));
    state.textureUnit[unit].texCoord = Vector4(texCoord, 1);
    glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
}


void RenderDevice::setTexCoord(uint unit, const Vector2& texCoord) {
    debugAssertM(unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));
    state.textureUnit[unit].texCoord = Vector4(texCoord.x, texCoord.y, 0, 1);
    glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
}


void RenderDevice::setTexCoord(uint unit, double texCoord) {
    debugAssertM(unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));
    state.textureUnit[unit].texCoord = Vector4(texCoord, 0, 0, 1);
    glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
}


void RenderDevice::sendVertex(const Vector2& vertex) {
    debugAssertM(inPrimitive, "Can only be called inside beginPrimitive()...endPrimitive()");
    glVertex2fv(vertex);
    ++currentPrimitiveVertexCount;
}


void RenderDevice::sendVertex(const Vector3& vertex) {
    debugAssertM(inPrimitive, "Can only be called inside beginPrimitive()...endPrimitive()");
    glVertex3fv(vertex);
    ++currentPrimitiveVertexCount;
}


void RenderDevice::sendVertex(const Vector4& vertex) {
    debugAssertM(inPrimitive, "Can only be called inside beginPrimitive()...endPrimitive()");
    glVertex4fv(vertex);
    ++currentPrimitiveVertexCount;
}


void RenderDevice::beginPrimitive(Primitive p) {
    debugAssertM(! inPrimitive, "Already inside a primitive");
    
    inPrimitive = true;
    currentPrimitiveVertexCount = 0;
    currentPrimitive = p;

    switch (p) {
    case LINES:
        glBegin(GL_LINES);
        break;

    case LINE_STRIP:
        glBegin(GL_LINE_STRIP);
        break;

    case TRIANGLES:
        glBegin(GL_TRIANGLES);
        break;

    case TRIANGLE_STRIP:
        glBegin(GL_TRIANGLE_STRIP);
        break;

    case TRIANGLE_FAN:
        glBegin(GL_TRIANGLE_FAN);
        break;

    case QUADS:
        glBegin(GL_QUADS);
        break;

    case QUAD_STRIP:
        glBegin(GL_QUAD_STRIP);
        break;

    case POINTS:
        glBegin(GL_POINTS);
        break;

    default:
        debugAssertM(false, "Fell through switch");
    }
}


void RenderDevice::endPrimitive() {
    debugAssertM(inPrimitive, "Call to endPrimitive() without matching beginPrimitive()");

	countPrimitive(currentPrimitive, currentPrimitiveVertexCount);

    glEnd();
    inPrimitive = false;
}


void RenderDevice::countPrimitive(RenderDevice::Primitive primitive, int numVertices) {
	switch (primitive) {
    case LINES:
        triangleCount += numVertices;
        break;

    case LINE_STRIP:
        triangleCount += (numVertices - 1) * 2;
        break;

    case TRIANGLES:
        triangleCount += numVertices / 3;
        break;

    case TRIANGLE_STRIP:
    case TRIANGLE_FAN:
        triangleCount += numVertices - 2;
        break;

    case QUADS:
        triangleCount += numVertices / 4;
        break;

    case QUAD_STRIP:
        triangleCount += (numVertices - 2);
        break;

    case POINTS:
        triangleCount += (numVertices * 2);
        break;
    }
}


void RenderDevice::setTexture(
    uint                unit,
    TextureRef          texture) {

    debugAssertM(! inPrimitive, 
                 "Can't change textures while rendering a primitive.");

    debugAssertM(unit < _numTextureUnits,
        format("Attempted to access texture unit %d"
               " on a device with %d units.",
               unit, _numTextureUnits));

    TextureRef oldTexture = state.textureUnit[unit].texture;

    if (oldTexture == texture) {
        return;
    }

    state.textureUnit[unit].texture = texture;

    // Turn off whatever was on previously
    glActiveTextureARB(GL_TEXTURE0_ARB + unit);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
    if (supportsTextureRectangle()) {
        glDisable(GL_TEXTURE_RECTANGLE_NV);
    }

    if ((Texture*)texture != NULL) {
        GLint u = texture->getOpenGLTextureTarget();
        glEnable(u);
        glBindTexture(u, texture->getOpenGLID());
    }

    // Force a reload of the texture matrix if invertY != old invertY
    if (((Texture*)oldTexture == NULL) ||
        ((Texture*)texture == NULL) ||
        (oldTexture->invertY != texture->invertY)) {
        forceSetTextureMatrix(unit, state.textureUnit[unit].textureMatrix);
    }
}


double RenderDevice::getDepthBufferValue(
    int                     x,
    int                     y) const {

    GLfloat depth;

    glReadPixels(x,
		         getHeight() - y,
                 1, 1,
                 GL_DEPTH_COMPONENT,
		         GL_FLOAT,
		         &depth);

    return depth;
}


/**
 Returns the current date as a string in the form YYYY-MM-DD
 */
static std::string currentDateString() {
    time_t t1;
    time(&t1);
    tm* t = localtime(&t1);
    return format("%d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday); 
}


void RenderDevice::screenshot(const std::string& filepath) const {

    // Read back the front buffer
    glReadBuffer(GL_FRONT);
    
    CImage im = CImage(screenWidth, screenHeight);
    glReadPixels(0, 0, screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, im.byte());

    // Flip right side up
    flipRGBVertical(im.byte(), im.byte(), screenWidth, screenHeight);

    // Restore the read buffer to the back
    glReadBuffer(GL_BACK);

    int i;
    double s = getLightSaturation();

    if (s != 1.0) {
        // Adjust the coloring for gamma correction
        // Lookup table for mapping v -> v * lightSaturation;
        uint8 L[255];
        uint8 *data = im.byte();
        
        for (i = 255; i >= 0; --i) {
            L[i] = iMin(255, iRound((double)i * s));
        }

        for (i = im.width * im.height * 3 - 1; i >= 0; --i) {
            data[i] = L[data[i]];
        }
    }

    // Save the file
    std::string basename = currentDateString();
    std::string filename;
    i = 0;
    do {
        filename = filepath + basename + "_" + format("%03d", i) + ".jpg";
        ++i;
    } while (fileExists(filename));   

    im.save(filename);
}


/**
 Draws a capped cylinder between v0 and v1
 */
static void drawCylinder(RenderDevice* device, const Vector3& v0, const Vector3& v1, double radius, int sides = 6, bool caps = true) {

    int a;
    sides = iMax(3, iMin(sides, 20));

    Vector3 z = v0 - v1;

    // Generate two axes perpendicular to v0 - v1
    Vector3 x;
    Vector3 y;

    const Vector3 vector[3] = {Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z};
    for (a = 0; a < 3; ++a) {
        x = z.cross(vector[a]);
        if ((x.dot(x) > 0) && (z.dot(x) == 0)) {
            break;
        }
    }

    y = z.cross(x);

    x = x.direction() * radius;
    y = y.direction() * radius;

    device->beginPrimitive(RenderDevice::QUAD_STRIP);
        for (a = 0; a <= sides; ++a) {
            const double angle = a * PI * 2.0 / sides;
            const Vector3 offset(cos(angle) * x + sin(angle) * y);
            device->sendVertex(v0 + offset);
            device->sendVertex(v1 + offset);
        }
    device->endPrimitive();

    if (caps) {
        device->beginPrimitive(RenderDevice::TRIANGLE_FAN);
            for (a = 0; a <= sides; ++a) {
                const double angle = a * PI * 2.0 / sides;
                const Vector3 offset(cos(angle) * x + sin(angle) * y);
                device->sendVertex(v0 + offset);
            }
        device->endPrimitive();

        device->beginPrimitive(RenderDevice::TRIANGLE_FAN);
            for (a = 0; a <= sides; ++a) {
                const double angle = a * PI * 2.0 / sides;
                const Vector3 offset(cos(-angle) * x + sin(-angle) * y);
                device->sendVertex(v1 + offset);
            }
        device->endPrimitive();
    }
}


void RenderDevice::debugDrawSphere(const Sphere& sphere, const Color3& color, double edgeScale) {
    pushState();

        setDepthTest(RenderDevice::DEPTH_LEQUAL);
        setCullFace(RenderDevice::CULL_BACK);

        // Cylinder radius
        const double r = edgeScale * 0.010;

        drawWireSphereSection(sphere, r, color, true, true);
        drawFaceSphereSection(sphere, color, true, true);

    popState();
}


void RenderDevice::debugDrawCapsule(const Capsule& capsule, const Color3& color, double edgeScale) {
    pushState();

        setDepthTest(RenderDevice::DEPTH_LEQUAL);
        setCullFace(RenderDevice::CULL_BACK);

        // Cylinder radius
        const double cylRadius = edgeScale * 0.010;

        CoordinateFrame cframe(capsule.getPoint1());

        Vector3 Y = (capsule.getPoint2() - capsule.getPoint1()).direction();
        Vector3 X = (abs(Y.dot(Vector3::UNIT_X)) > 0.9) ? Vector3::UNIT_Y : Vector3::UNIT_X;
        Vector3 Z = X.cross(Y).direction();
        X = Y.cross(Z);        
        cframe.rotation.setColumn(0, X);
        cframe.rotation.setColumn(1, Y);
        cframe.rotation.setColumn(2, Z);

        setObjectToWorldMatrix(getObjectToWorldMatrix() * cframe);
  
        double radius = capsule.getRadius();
        double height = (capsule.getPoint2() - capsule.getPoint1()).length();

        Sphere sphere1(Vector3::ZERO, radius);
        Sphere sphere2(Vector3(0, height, 0), radius);

        drawWireSphereSection(sphere1, cylRadius, color, false, true);
        drawWireSphereSection(sphere2, cylRadius, color, true, false);

        // Line around center
        setColor(color);
        Vector3 center(0, height / 2, 0);
        double pitch = 0;
        int y;
        for (y = 0; y < 26; ++y) {
            const double yaw0 = y * PI / 13;
            const double yaw1 = (y + 1) * PI / 13;

            Vector3 v0 = Vector3(cos(yaw0), sin(pitch), sin(yaw0)) * radius + center;
            Vector3 v1 = Vector3(cos(yaw1), sin(pitch), sin(yaw1)) * radius + center;

            drawCylinder(this, v0, v1, cylRadius, 6, false);
        }

        Vector3 top(0,height,0);
        // Edge lines
        for (y = 0; y < 8; ++y) {
            const double yaw = y * PI / 4;
            const Vector3 x(cos(yaw) * radius, 0, sin(yaw) * radius);
        
            drawCylinder(this, x, x + top, cylRadius, 6, false);        
        }

        drawFaceSphereSection(sphere1, color, false, true);
        drawFaceSphereSection(sphere2, color, true, false);

        // Cylinder faces
        setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        setColor(Color4(color, .12));
        setCullFace(RenderDevice::CULL_FRONT);
        disableDepthWrite();

        for (int i = 0; i < 2; ++i) {
            beginPrimitive(RenderDevice::QUAD_STRIP);
                for (y = 0; y <= 26; ++y) {
                    const double yaw0 = y * PI / 13;
                    Vector3 v0 = Vector3(cos(yaw0), sin(pitch), sin(yaw0)) * radius;

                    sendVertex(v0);
                    sendVertex(v0 + top);
                }
            endPrimitive();

            setCullFace(RenderDevice::CULL_BACK);
       }

    popState();
}


void RenderDevice::drawWireSphereSection(const Sphere& sphere, double cylRadius, const Color3& color, bool top, bool bottom) {
    
    int sections = 26;
    int start = top ? 0 : (sections / 2);
    int stop = bottom ? sections : (sections / 2);

    pushState();
        setColor(color);

	    int y;
	    int p;

        double radius = sphere.radius;
        const Vector3& center = sphere.center;

        // Wire frame
        for (y = 0; y < 8; ++y) {
            const double yaw = y * PI / 4;
            const Vector3 x(cos(yaw) * radius, 0, sin(yaw) * radius);
            //const Vector3 z(-sin(yaw) * radius, 0, cos(yaw) * radius);

            for (p = start; p < stop; ++p) {
                const double pitch0 = p * PI / (sections * 0.5);
                const double pitch1 = (p + 1) * PI / (sections * 0.5);

                Vector3 v0 = cos(pitch0) * x + Vector3::UNIT_Y * radius * sin(pitch0) + center;
                Vector3 v1 = cos(pitch1) * x + Vector3::UNIT_Y * radius * sin(pitch1) + center;
                drawCylinder(this, v0, v1, cylRadius, 6, false);
            }
        }

        int a = bottom ? -1 : 0;
        int b = top ? 1 : 0; 
        for (p = a; p <= b; ++p) {
            const double pitch = p * PI / 6;
            for (y = 0; y < 26; ++y) {
                const double yaw0 = y * PI / 13;
                const double yaw1 = (y + 1) * PI / 13;

                Vector3 v0 = Vector3(cos(yaw0) * cos(pitch), sin(pitch), sin(yaw0) * cos(pitch)) * radius + center;
                Vector3 v1 = Vector3(cos(yaw1) * cos(pitch), sin(pitch), sin(yaw1) * cos(pitch)) * radius + center;

                drawCylinder(this, v0, v1, cylRadius, 6, false);
            }
        }
    popState();
}


void RenderDevice::drawFaceSphereSection(const Sphere& sphere, const Color3& color, bool top, bool bottom) {
    int sections = 26;
    int start = top ? 0 : (sections / 2);
    int stop = bottom ? sections : (sections / 2);

    pushState();
        // Faces
        setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        setColor(Color4(color, .12));
        setCullFace(RenderDevice::CULL_FRONT);
        disableDepthWrite();

        int i;
        int p;
        int y;

        for (i = 0; i < 2; ++i) {
            for (p = start; p < stop; ++p) {
                const double pitch0 = p * PI / (double)sections;
                const double pitch1 = (p + 1) * PI / (double)sections;

                beginPrimitive(RenderDevice::QUAD_STRIP);
                for (y = 0; y <= 26; ++y) {
                    const double yaw = -y * PI / 13;
                    sendVertex(Vector3(cos(yaw) * sin(pitch0), cos(pitch0), sin(yaw) * sin(pitch0)) * sphere.radius + sphere.center);
                    sendVertex(Vector3(cos(yaw) * sin(pitch1), cos(pitch1), sin(yaw) * sin(pitch1)) * sphere.radius + sphere.center);
                }
                endPrimitive();
            }

            setCullFace(RenderDevice::CULL_BACK);
       }
    popState();       
}


void RenderDevice::debugDrawBox(const Box& box, const Color3& color, double edgeScale) {
    pushState();

        setDepthTest(RenderDevice::DEPTH_LEQUAL);
        setCullFace(RenderDevice::CULL_BACK);

	int i;

        // Cylinder radius
        const double r = edgeScale * 0.010;
        setColor(color);

        // Wire frame
        for (i = 0; i < 8; i += 4) {
            drawCylinder(this, box.getCorner(0 + i), box.getCorner(1 + i), r);
            drawCylinder(this, box.getCorner(1 + i), box.getCorner(2 + i), r);
            drawCylinder(this, box.getCorner(2 + i), box.getCorner(3 + i), r);
            drawCylinder(this, box.getCorner(3 + i), box.getCorner(0 + i), r);
        }

        for (i = 0; i < 4; ++i) {
            drawCylinder(this, box.getCorner(i), box.getCorner(4 + i), r);
        }

        // Faces
        disableDepthWrite();
        setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        setColor(Color4(color, .12));

        setCullFace(RenderDevice::CULL_FRONT);
        for (int j = 0; j < 2; ++j) {
            beginPrimitive(RenderDevice::QUADS);
            for (i = 0; i < 6; ++i) {
                Vector3 v0, v1, v2, v3;
                box.getFaceCorners(i, v0, v1, v2, v3);
                sendVertex(v0);
                sendVertex(v1);
                sendVertex(v2);
                sendVertex(v3);
            }
            endPrimitive();
            setCullFace(RenderDevice::CULL_BACK);
        }

    popState();
}


void RenderDevice::debugDrawRay(const Ray& ray, const Color3& color, double scale) {

    // Arrow tip
    const double len = ray.direction.length();

    // Arrowhead radius
    const double r = scale * 0.12;

    // Shaft radius
    const double s = r * .25;

    // Height of arrow head
    const double hh = r * 3;

    // Bottom of head
    const double h = len - hh;

    // Magnitude of the normal vectors, used to make them unit length
    const double k = 1 / sqrt(square(r/hh) + 1);

    pushState();
        CoordinateFrame c1 = state.objectToWorldMatrix;
        CoordinateFrame c2;
        c2.lookAt(ray.direction);
        c2.translation = ray.origin;

        setObjectToWorldMatrix(c2 * c1);
    
        double a;
        // Arrow head
        setColor(color);
        beginPrimitive(RenderDevice::TRIANGLES);
            for (a = 0; a <= 12; ++a) {
                double angle0 = a * PI / 6;
                double angle1 = (a + 1) * PI / 6;


                setNormal( Vector3(cos((angle0 + angle1) * 0.5), -sin((angle0 + angle1) * 0.5), -r/hh) * k);
                sendVertex(-Vector3::UNIT_Z * len);

                setNormal( Vector3(cos(angle0),     -sin(angle0),     -r/hh) * k);
                sendVertex(Vector3(cos(angle0) * r, -sin(angle0) * r, -h));

                setNormal( Vector3(cos(angle1),     -sin(angle1),     -r/hh) * k);
                sendVertex(Vector3(cos(angle1) * r, -sin(angle1) * r, -h));
            }
        endPrimitive();


        // Back of arrow head
        beginPrimitive(RenderDevice::TRIANGLE_FAN);
            setNormal(Vector3(0, 1, 0));
            for (a = 0; a <= 12; ++a) {
                double angle = a * PI / 6;
                sendVertex(Vector3(cos(-angle) * r, -sin(-angle) * r, -h));
            }
        endPrimitive();

        // Arrow shaft
        beginPrimitive(RenderDevice::QUAD_STRIP);
            for (a = 0; a <= 10; ++a) {
                double angle = a * PI / 5;
                setNormal( Vector3(cos(angle),     -sin(angle),      0));
                sendVertex(Vector3(cos(angle) * s, -sin(angle) * s, -h));
                sendVertex(Vector3(cos(angle) * s, -sin(angle) * s,  0));
            }
        endPrimitive();

        // Back of arrow shaft
        beginPrimitive(RenderDevice::TRIANGLE_FAN);
            setNormal(Vector3(0, 1, 0));
            for (a = 0; a <= 10; ++a) {
                double angle = a * PI / 5;
                sendVertex(Vector3(cos(-angle) * s, -sin(-angle) * s, 0));
            }
        endPrimitive();

    popState();
}


void RenderDevice::getProjectionMatrixParams(
    double& l,
    double& r,
    double& b,
    double& t,
    double& n,
    double& f,
    bool&   is3D) {

    l = state.projectionMatrixParams[0];
    r = state.projectionMatrixParams[1];
    b = state.projectionMatrixParams[2];
    t = state.projectionMatrixParams[3];
    n = state.projectionMatrixParams[4];
    f = state.projectionMatrixParams[5];
    is3D = state.proj3D;
}

void RenderDevice::debugDrawAxes(double scale) {
    // Angle index
    int a;

    // Arrowhead radius
    const double r = scale * 0.060;

    // Shaft radius
    const double s = scale * 0.015;

    // How far along the axis the arrow head starts
    const double h = scale * 0.800;

    // Text label scale
    const double xx = scale * .03;
    const double yy = xx * 1.4;


    const Color3 color[3] = {Color3::RED, Color3::GREEN, Color3::BLUE};
    const CoordinateFrame c2w = getCameraToWorldMatrix();
    const CoordinateFrame o2w = getObjectToWorldMatrix();

    pushState();

    setLineWidth(max(0, min(4, 10 * scale / (o2w.translation - c2w.translation).length())));

    setCullFace(RenderDevice::CULL_BACK);
    setDepthTest(RenderDevice::DEPTH_LEQUAL);

    Matrix3 permute(Matrix3::IDENTITY);

    for (int axis = 0; axis < 3; ++axis) {

        setObjectToWorldMatrix(o2w);
        setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ZERO);
        setColor(color[axis]);
    
        // Arrow head
        beginPrimitive(RenderDevice::TRIANGLE_FAN);
            sendVertex(permute * Vector3::UNIT_X * scale);
            for (a = 0; a <= 12; ++a) {
                double angle = a * PI / 6;
                sendVertex(permute * Vector3(h, cos(angle) * r, sin(angle) * r));
            }
        endPrimitive();

        // Back of arrow head
        beginPrimitive(RenderDevice::TRIANGLE_FAN);
            for (a = 0; a <= 12; ++a) {
                double angle = a * PI / 6;
                sendVertex(permute * Vector3(h, cos(-angle) * r, sin(-angle) * r));
            }
        endPrimitive();

        // Arrow shaft
        beginPrimitive(RenderDevice::QUAD_STRIP);
            for (a = 0; a <= 10; ++a) {
                double angle = a * PI / 5;
                sendVertex(permute * Vector3(h, cos(angle) * s, sin(angle) * s));
                sendVertex(permute * Vector3(0, cos(angle) * s, sin(angle) * s));
            }
        endPrimitive();

        // Back of arrow shaft
        beginPrimitive(RenderDevice::TRIANGLE_FAN);
            for (a = 0; a <= 10; ++a) {
                double angle = a * PI / 5;
                sendVertex(permute * Vector3(0, cos(-angle) * s, sin(-angle) * s));
            }
        endPrimitive();

        // Label
        setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        // Look at the camera
        CoordinateFrame c;
        c.translation = o2w.pointToWorldSpace(permute * Vector3::UNIT_X * scale * 1.1);
        c.lookAt(c2w.translation, c2w.rotation.getColumn(1));

        setObjectToWorldMatrix(c);

        switch (axis) {
        case 0: // x
            beginPrimitive(RenderDevice::LINES);
                sendVertex(Vector3(-xx,  yy, 0));
                sendVertex(Vector3( xx, -yy, 0));
                sendVertex(Vector3( xx,  yy, 0));
                sendVertex(Vector3(-xx, -yy, 0));
            endPrimitive();
            break;

        case 1: // y
            beginPrimitive(RenderDevice::LINES);
                sendVertex(Vector3(-xx,  yy, 0));
                sendVertex(Vector3(  0,  0,  0));
                sendVertex(Vector3(  0,  0,  0));
                sendVertex(Vector3(  0, -yy, 0));
                sendVertex(Vector3( xx,  yy, 0));
                sendVertex(Vector3(  0,  0,  0));
            endPrimitive();
            break;

        case 2: // z
            beginPrimitive(RenderDevice::LINE_STRIP);
                sendVertex(Vector3( xx,  yy, 0));
                sendVertex(Vector3(-xx,  yy, 0));
                sendVertex(Vector3( xx, -yy, 0));
                sendVertex(Vector3(-xx, -yy, 0));
            endPrimitive();
            break;
        }

	int i;
        // Cyclic shift the permutation
        Vector3 r[3];
        for (i = 0; i < 3; ++i) {
            r[i] = (*(Vector3*)permute[i]);
        }
        for (i = 0; i < 3; ++i) {
            permute.setRow(i, r[(i + 2) % 3]);
        }

    }

    popState();
}


double RenderDevice::getFrameRate() const {
    return emwaFrameRate;
}


double RenderDevice::getTriangleRate() const {
    return emwaTriangleRate;
}


double RenderDevice::getTrianglesPerFrame() const {
    return emwaTriangleCount;
}


void RenderDevice::beginIndexedPrimitives() {
	debugAssert(! inPrimitive);
	debugAssert(! inIndexedPrimitive);

	varSystem->beginIndexedPrimitives();

	inIndexedPrimitive = true;
}


void RenderDevice::endIndexedPrimitives() {
	debugAssert(! inPrimitive);
	debugAssert(inIndexedPrimitive);

	varSystem->endIndexedPrimitives();
	inIndexedPrimitive = false;
}


void RenderDevice::setVertexArray(const class VAR& v) {
	debugAssert(inIndexedPrimitive);
	debugAssert(! inPrimitive);

	varSystem->setVertexArray(v);
}


void RenderDevice::setVertexAttribArray(unsigned int attribNum, const class VAR& v, bool normalize) {
	debugAssert(inIndexedPrimitive);
	debugAssert(! inPrimitive);

	varSystem->setVertexAttribArray(attribNum, v, normalize);
}


void RenderDevice::setNormalArray(const class VAR& v) {
	debugAssert(inIndexedPrimitive);
	debugAssert(! inPrimitive);

	varSystem->setNormalArray(v);
}


void RenderDevice::setColorArray(const class VAR& v) {
	debugAssert(inIndexedPrimitive);
	debugAssert(! inPrimitive);

	varSystem->setColorArray(v);
}


void RenderDevice::setTexCoordArray(unsigned int unit, const class VAR& v) {
	debugAssert(inIndexedPrimitive);
	debugAssert(! inPrimitive);

	varSystem->setTexCoordArray(unit, v);
}



VARArea* RenderDevice::createVARArea(size_t areaSize) {
	debugAssert(varSystem);
	return varSystem->createArea(areaSize);
}

} // namespace
