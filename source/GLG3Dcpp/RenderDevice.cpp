/**
 @file RenderDevice.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2001-07-08
 @edited  2003-12-07
 */


#include "G3D/platform.h"

#if defined(G3D_OSX)
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#else
#include <SDL.h>
#include <SDL_syswm.h>
#endif

#include <sstream>
#include "G3D/platform.h"
#include "GLG3D/glcalls.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/Texture.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/VARArea.h"
#include "GLG3D/VAR.h"

#ifdef G3D_WIN32
    #include <winver.h>
#endif

// Code for testing FSAA:
//#define SDL_1_26


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

#ifdef G3D_WIN32
PFNWGLSWAPINTERVALEXTPROC                   wglSwapIntervalEXT 			    = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC              wglChoosePixelFormatARB		    = NULL;
PFNWGLALLOCATEMEMORYNVPROC                  wglAllocateMemoryNV 		    = NULL;
PFNWGLFREEMEMORYNVPROC                      wglFreeMemoryNV 			    = NULL;
#endif

PFNGLVERTEXARRAYRANGENVPROC                 glVertexArrayRangeNV 		    = NULL;
PFNGLFLUSHVERTEXARRAYRANGENVPROC            glFlushVertexArrayRangeNV       = NULL;

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
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC        glProgramLocalParameter4fvARB   = NULL;
PFNGLPROGRAMENVPARAMETER4DVARBPROC          glProgramEnvParameter4dvARB     = NULL;
PFNGLPROGRAMLOCALPARAMETER4DVARBPROC        glProgramLocalParameter4dvARB   = NULL;

PFNGLGENPROGRAMSNVPROC                      glGenProgramsNV                 = NULL;
PFNGLDELETEPROGRAMSNVPROC                   glDeleteProgramsNV              = NULL;
PFNGLBINDPROGRAMNVPROC                      glBindProgramNV                 = NULL;
PFNGLLOADPROGRAMNVPROC                      glLoadProgramNV                 = NULL;
PFNGLTRACKMATRIXNVPROC                      glTrackMatrixNV                 = NULL;
PFNGLPROGRAMPARAMETER4FVNVPROC              glProgramParameter4fvNV         = NULL;
PFNGLGETPROGRAMPARAMETERFVNVPROC            glGetProgramParameterfvNV       = NULL;
PFNGLGETPROGRAMPARAMETERDVNVPROC            glGetProgramParameterdvNV       = NULL;

PFNGLVERTEXATTRIBPOINTERARBPROC             glVertexAttribPointerARB        = NULL;
PFNGLENABLEVERTEXATTRIBARRAYARBPROC         glEnableVertexAttribArrayARB    = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYARBPROC        glDisableVertexAttribArrayARB   = NULL;

PFNGLPOINTPARAMETERFARBPROC                 glPointParameterfARB            = NULL;
PFNGLPOINTPARAMETERFVARBPROC                glPointParameterfvARB           = NULL;

PFNGLMULTIDRAWARRAYSEXTPROC                 glMultiDrawArraysEXT            = NULL;
PFNGLMULTIDRAWELEMENTSEXTPROC               glMultiDrawElementsEXT          = NULL;

PFNGLCOMBINERPARAMETERFVNVPROC              glCombinerParameterfvNV         = NULL;
PFNGLCOMBINERPARAMETERFNVPROC               glCombinerParameterfNV          = NULL;
PFNGLCOMBINERPARAMETERIVNVPROC              glCombinerParameterivNV         = NULL;
PFNGLCOMBINERPARAMETERINVPROC               glCombinerParameteriNV          = NULL;
PFNGLCOMBINERINPUTNVPROC                    glCombinerInputNV               = NULL;
PFNGLCOMBINEROUTPUTNVPROC                   glCombinerOutputNV              = NULL;
PFNGLFINALCOMBINERINPUTNVPROC               glFinalCombinerInputNV          = NULL;
PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC      glGetCombinerInputParameterfvNV        = NULL;
PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC      glGetCombinerInputParameterivNV        = NULL;
PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC     glGetCombinerOutputParameterfvNV       = NULL;
PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC     glGetCombinerOutputParameterivNV       = NULL;
PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV   = NULL;
PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV   = NULL;
PFNGLCOMBINERSTAGEPARAMETERFVNVPROC         glCombinerStageParameterfvNV           = NULL;
PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC      glGetCombinerStageParameterfvNV        = NULL;

PFNGLACTIVESTENCILFACEEXTPROC               glActiveStencilFaceEXT          = NULL;

PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;
PFNGLISBUFFERARBPROC glIsBufferARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;
PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB = NULL;
PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB = NULL;
PFNGLMAPBUFFERARBPROC glMapBufferARB = NULL;
PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = NULL;
PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB = NULL;
PFNGLDRAWRANGEELEMENTSPROC glDrawRangeElements = NULL;


namespace G3D {


static void _glViewport(double a, double b, double c, double d) {
    glViewport(iRound(a), iRound(b), iRound(a + c) - iRound(a), iRound(b + d) - iRound(b));
}

/**
 Dummy function to which unloaded extensions can be set.
 */
static void __stdcall glIgnore(GLenum e) {}


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

void RenderDevice::computeVendor() {
    std::string s = reinterpret_cast<const char*>(glGetString(GL_VENDOR));

    if (s == "ATI Technologies Inc.") {
        vendor = ATI;
    } else if (s == "NVIDIA Corporation") {
        vendor = NVIDIA;
    } else {
        vendor = ARB;
    }
}


std::string RenderDevice::getCardDescription() const {
    return cardDescription;
}

/**
 Returns the version string for the video driver.

 @cite Based in part on code by Ted Peck tpeck@roundwave.com http://www.codeproject.com/dll/ShowVer.asp
 */
std::string RenderDevice::getDriverVersion() {
    #ifdef G3D_WIN32
    
        std::string driver;

        // Locate the windows\system directory
        {
            char sysDir[1024];
            int sysSize = GetSystemDirectory(sysDir, 1024);
            if (sysSize == 0) {
                return "Unknown (can't find Windows directory)";
            }
            driver = sysDir;
        }

        switch (vendor) {
        case ATI:
            driver = driver + "\\ati2dvag.dll";
            break;

        case NVIDIA:
            driver = driver + "\\nv4_disp.dll";
            break;

        default:
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
    _numTextures = 0;
    _numTextureCoords = 0;
    emwaFrameRate = 0;
    lastTime = getTime();

	if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0 ) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
		debugPrintf("Unable to initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

    for (int i = 0; i < MAX_TEXTURE_UNITS; ++i) {
        currentlyBoundTexture[i] = 0;
    }
}


void RenderDevice::setVARAreaMilestone() {
    MilestoneRef milestone = createMilestone("VAR Milestone");
    setMilestone(milestone);

    // Overwrite any preexisting milestone
    currentVARArea->milestone = milestone;
}


RenderDevice::~RenderDevice() {
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

    // Older machines can't handle multitexture, 
    // so give it a version that will be safe for
    // single texture GL
    if (glActiveTextureARB == NULL) {
        #ifdef G3D_WIN32
            *((void**)&glActiveTextureARB) = glIgnore;
        #else
            glActiveTextureARB = (void(*)(unsigned int))glIgnore;
        #endif
    }

    LOAD_EXTENSION(glClientActiveTextureARB);
    #ifdef G3D_WIN32
        LOAD_EXTENSION(wglSwapIntervalEXT);
        LOAD_EXTENSION(wglChoosePixelFormatARB);
        LOAD_EXTENSION(wglAllocateMemoryNV);
        LOAD_EXTENSION(wglFreeMemoryNV);
    #endif
    LOAD_EXTENSION(glVertexArrayRangeNV);
    LOAD_EXTENSION(glCompressedTexImage2DARB);
    LOAD_EXTENSION(glGetCompressedTexImageARB);
    LOAD_EXTENSION(glGenFencesNV);
    LOAD_EXTENSION(glDeleteFencesNV);
    LOAD_EXTENSION(glSetFenceNV);
    LOAD_EXTENSION(glFlushVertexArrayRangeNV);
    LOAD_EXTENSION(glFinishFenceNV);
    LOAD_EXTENSION(glGenProgramsARB);
    LOAD_EXTENSION(glBindProgramARB);
    LOAD_EXTENSION(glDeleteProgramsARB);
    LOAD_EXTENSION(glProgramStringARB);
    LOAD_EXTENSION(glProgramEnvParameter4fARB);
    LOAD_EXTENSION(glProgramLocalParameter4fARB);
    LOAD_EXTENSION(glProgramLocalParameter4fvARB);
    LOAD_EXTENSION(glProgramEnvParameter4dvARB);
    LOAD_EXTENSION(glProgramLocalParameter4dvARB);
    LOAD_EXTENSION(glVertexAttribPointerARB);
    LOAD_EXTENSION(glEnableVertexAttribArrayARB);
    LOAD_EXTENSION(glDisableVertexAttribArrayARB);
    LOAD_EXTENSION(glPointParameterfARB);
    LOAD_EXTENSION(glPointParameterfvARB);
    LOAD_EXTENSION(glMultiDrawArraysEXT);
    LOAD_EXTENSION(glMultiDrawElementsEXT);
    LOAD_EXTENSION(glCombinerParameterfvNV);
    LOAD_EXTENSION(glCombinerParameterfNV);
    LOAD_EXTENSION(glCombinerParameterivNV);
    LOAD_EXTENSION(glCombinerParameteriNV);
    LOAD_EXTENSION(glCombinerInputNV);
    LOAD_EXTENSION(glCombinerOutputNV);
    LOAD_EXTENSION(glFinalCombinerInputNV);
    LOAD_EXTENSION(glGetCombinerInputParameterfvNV);
    LOAD_EXTENSION(glGetCombinerInputParameterivNV);
    LOAD_EXTENSION(glGetCombinerOutputParameterfvNV);
    LOAD_EXTENSION(glGetCombinerOutputParameterivNV);
    LOAD_EXTENSION(glGetFinalCombinerInputParameterfvNV);
    LOAD_EXTENSION(glGetFinalCombinerInputParameterivNV);
    LOAD_EXTENSION(glCombinerStageParameterfvNV);
    LOAD_EXTENSION(glGetCombinerStageParameterfvNV);
    LOAD_EXTENSION(glGenProgramsNV);
    LOAD_EXTENSION(glDeleteProgramsNV);
    LOAD_EXTENSION(glBindProgramNV);
    LOAD_EXTENSION(glLoadProgramNV);
    LOAD_EXTENSION(glTrackMatrixNV);
    LOAD_EXTENSION(glProgramParameter4fvNV);
    LOAD_EXTENSION(glActiveStencilFaceEXT);
    LOAD_EXTENSION(glGetProgramParameterfvNV);
    LOAD_EXTENSION(glGetProgramParameterdvNV);
    LOAD_EXTENSION(glBindBufferARB);
    LOAD_EXTENSION(glDeleteBuffersARB);
    LOAD_EXTENSION(glGenBuffersARB);
    LOAD_EXTENSION(glIsBufferARB);
    LOAD_EXTENSION(glBufferDataARB);
    LOAD_EXTENSION(glBufferSubDataARB);
    LOAD_EXTENSION(glGetBufferSubDataARB);
    LOAD_EXTENSION(glMapBufferARB);
    LOAD_EXTENSION(glUnmapBufferARB);
    LOAD_EXTENSION(glGetBufferParameterivARB);
    LOAD_EXTENSION(glGetBufferPointervARB);
    LOAD_EXTENSION(glDrawRangeElements);

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
    const RenderDeviceSettings& _settings,
    Log*                        log) {

    settings = _settings;

    debugAssert(! initialized());

    std::istringstream extensions;

    debugLog = log;

    beginEndFrame = 0;
    if (debugLog) {debugLog->section("Initialization");}

    debugAssert((settings.lightSaturation >= 0.5) && (settings.lightSaturation <= 2.0));

    // Under Windows, reset the last error so that our debug box
    // gives the correct results
    #if _WIN32
        SetLastError(0);
    #endif

	const int minimumDepthBits    = iMin(16, settings.depthBits);
	const int desiredDepthBits    = settings.depthBits;

	const int minimumStencilBits  = settings.stencilBits;
	const int desiredStencilBits  = settings.stencilBits;

    const int desiredTextureUnits = 8;

    if (debugLog) {debugLog->println("Setting video mode");}

    setVideoMode();

    if (!strcmp((char*)glGetString(GL_RENDERER), "GDI Generic") && debugLog) {
        debugLog->printf("\n*********************************************************\n");
        debugLog->printf("* WARNING: This computer does not have correctly        *\n");
        debugLog->printf("*          installed graphics drivers and is using      *\n");
        debugLog->printf("*          the default Microsoft OpenGL implementation. *\n");
        debugLog->printf("*          Most graphics capabilities are disabled.  To *\n");
        debugLog->printf("*          correct this problem, download and install   *\n");
        debugLog->printf("*          the latest drivers for the graphics card.    *\n");
        debugLog->printf("*********************************************************\n\n");
    }

    setCaption("Graphics3D");

	glViewport(0, 0, getWidth(), getHeight());

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
    computeVendor();

    extensions.str((char*)glGetString(GL_EXTENSIONS));
    {
        // Parse the extensions into the supported set
        std::string s;
        while (extensions >> s) {
            extensionSet.insert(s);
        }

        stencilWrapSupported        = supportsOpenGLExtension("EXT_stencil_wrap");
        textureRectangleSupported   = supportsOpenGLExtension("GL_NV_texture_rectangle");
        _supportsVertexProgram      = supportsOpenGLExtension("GL_ARB_vertex_program");
        _supportsNVVertexProgram2   = supportsOpenGLExtension("GL_NV_vertex_program2");
        _supportsFragmentProgram    = supportsOpenGLExtension("GL_ARB_fragment_program");
        _supportsVertexBufferObject = supportsOpenGLExtension("GL_ARB_vertex_buffer_object");
    }

    std::string ver = getDriverVersion();
    if (debugLog) {
        debugLog->printf("Operating System: %s\n", System::operatingSystem().c_str());
        debugLog->printf("Processor Architecture: %s\n\n", System::cpuArchitecture().c_str());

        debugLog->printf(
            "GL Vendor:      %s\n",
            glGetString(GL_VENDOR));

        debugLog->printf(
            "GL Renderer:    %s\n",
            glGetString(GL_RENDERER));

        debugLog->printf(
            "GL Version:     %s\n",
            glGetString(GL_VERSION));

        debugLog->printf(
            "Driver version: %s\n\n",
            ver.c_str());

        debugLog->printf(
            "GL extensions: \"%s\"\n\n",
            extensions.str().c_str());
    }
 

    cardDescription = format("%s %s", glGetString(GL_RENDERER), ver.c_str());

    // Don't use more texture units than allowed at compile time.
    _numTextureUnits = iMin(MAX_TEXTURE_UNITS, _numTextureUnits);

    // NVIDIA cards have different numbers of texture coords, units, and textures
    if (vendor == NVIDIA) {
        glGetIntegerv(GL_MAX_TEXTURE_COORDS_NV, &_numTextureCoords);
        _numTextureCoords = iMin(MAX_TEXTURE_UNITS, iMax(_numTextureUnits, _numTextureCoords));

        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_NV, &_numTextures);
        _numTextures = iMin(MAX_TEXTURE_UNITS, iMax(_numTextureUnits, _numTextures));
    } else {
        _numTextureCoords = _numTextureUnits;
        _numTextures = _numTextureUnits;
    }

    if (debugLog) {
    debugLog->section("Video Status");

    int actualFSAABuffers = 0, actualFSAASamples = 17;
#ifdef SDL_1_26
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &actualFSAABuffers);
    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &actualFSAASamples);
#endif

    // This call is here to make GCC realize that isOk is used.
    (void)isOk(false);

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
             "TextureUnits              %4d    %3d:%3d   %s\n"
             "FSAA                      %2d:%2d    %2d:%2d    %s\n"

             "Width             %8d pixels           %s\n"
             "Height            %8d pixels           %s\n"
             "Mode                 %10s             %s\n\n"

             "* JOYSTICK\n"
             "Number                              %4d    %s\n\n",
                         

             minimumDepthBits, desiredDepthBits, depthBits, isOk(depthOk),
             minimumStencilBits, desiredStencilBits, stencilBits, isOk(stencilOk),

             alphaBits, "ok",
             redBits, "ok", 
             greenBits, "ok", 
             blueBits, "ok", 

             desiredTextureUnits, _numTextureUnits, _numTextureCoords, isOk(_numTextureUnits >= desiredTextureUnits),
             1, settings.fsaaSamples, actualFSAABuffers, actualFSAASamples, isOk(settings.fsaaSamples == actualFSAASamples),

             settings.width, "ok",
             settings.height, "ok",
             (settings.fullScreen ? "Fullscreen" : "Windowed"), "ok",             
             SDL_NumJoysticks(), "ok"
             );
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
                          65535 + 0.5));

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


void RenderDevice::notifyResize(int w, int h) {
    debugAssert(w > 0);
    debugAssert(h > 0);
    settings.width = w;
    settings.height = h;
}


void RenderDevice::setVideoMode() {

    debugAssertM(stateStack.size() == 0, "Cannot call setVideoMode between pushState and popState");
    debugAssertM(beginEndFrame == 0, "Cannot call setVideoMode between beginFrame and endFrame");

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
        if (debugLog) {debugLog->printf("Unable to create OpenGL screen: %s\n", SDL_GetError());}
		error("Critical Error", format("Unable to create OpenGL screen: %s\n", SDL_GetError()).c_str(), true);
		SDL_Quit();
		exit(2);
	}

    static bool extensionsInitialized = false;
    if (! extensionsInitialized) {
        // First time through, initialize extensions
        initGLExtensions();
        extensionsInitialized = true;
    }

    // Reset all state

    // Set the refresh rate
    #ifdef G3D_WIN32
        if (wglSwapIntervalEXT != NULL) {
            if (debugLog) {
                if (settings.asychronous) {
                    debugLog->printf("wglSwapIntervalEXT(0);\n");
                } else {
                    debugLog->printf("wglSwapIntervalEXT(1);\n");
                }
            }
            wglSwapIntervalEXT(settings.asychronous ? 0 : 1);
        }
    #endif

    SDL_EnableUNICODE(1);

	glClearDepth(1.0);

    {
        if (debugLog) debugLog->printf("Setting brightness to %g\n", settings.lightSaturation);
        // Adjust the gamma so that changing the 
        // light intensities won't affect the actual screen
        // brightness.  This method due to John Carmack.
        lightSaturation = settings.lightSaturation;
        brightScale = 1.0 / lightSaturation;
        setGamma(lightSaturation, 1.0);
        if (debugLog) debugLog->println("Brightness set.");
    }

    // Enable proper specular lighting
    if (debugLog) debugLog->println("Enabling separate specular lighting.\n");
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, GL_SEPARATE_SPECULAR_COLOR_EXT);
    debugAssertGLOk();

    // Make sure we use good interpolation
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    // glHint(GL_GENERATE_MIPMAP_HINT_EXT, GL_NICEST);
    glEnable(GL_MULTISAMPLE_ARB);
    if (vendor == NVIDIA) {
        glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    }
    glEnable(GL_NORMALIZE);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    debugAssertGLOk();

    if (debugLog) debugLog->println("Setting initial rendering state.\n");
    glDisable(GL_LIGHT0);
    debugAssertGLOk();
    {
        // WARNING: this must be kept in sync with the 
        // RenderState constructor
        state = RenderState(getWidth(), getHeight());

        _glViewport(state.viewport.x0(), state.viewport.y0(), state.viewport.width(), state.viewport.height());
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

        for (int i = 0; i < MAX_LIGHTS; ++i) {
            setLight(i, NULL, true);
        }
        glColor4d(1, 1, 1, 1);
        glNormal3d(0, 0, 0);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        setShininess(state.shininess);
        setSpecularCoefficient(state.specular);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glShadeModel(GL_FLAT);

        glClearStencil(0);
        glClearDepth(1);
        glClearColor(0,0,0,1);
        glMatrixMode(GL_PROJECTION);
        glLoadMatrix(state.projectionMatrix);
        glMatrixMode(GL_MODELVIEW);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glDepthRange(0, 1);

        // Set up the texture units.
        for (int t = _numTextureCoords - 1; t >= 0; --t) {
            double d[] = {0,0,0,1};
            glMultiTexCoord4dvARB(GL_TEXTURE0_ARB + t, d);
        }

        glActiveTextureARB(GL_TEXTURE0_ARB);
    }
    debugAssertGLOk();

    if (debugLog) debugLog->printf("Done setting initial state.\n");
}


void RenderDevice::setCaption(const std::string& caption) {
	// Set the title bar
	SDL_WM_SetCaption(caption.c_str(), NULL);
}



int RenderDevice::getWidth() const {
    return settings.width;
}


int RenderDevice::getHeight() const {
    return settings.height;
}


//////////////////////////////////////////////////////////////////////////////////////////


Vector4 RenderDevice::project(const Vector3& v) const {
    return project(Vector4(v, 1));
}


Vector4 RenderDevice::project(const Vector4& v) const {
    return glToScreen(v);
}


void RenderDevice::cleanup() {
    debugAssert(initialized());

    if (debugLog) {debugLog->println("Restoring gamma.");}
    setGamma(1, 1);

    if (debugLog) {debugLog->println("Shutting down SDL.");}
    SDL_Quit();
}


void RenderDevice::push2D() {
    push2D(getViewport());
}


void RenderDevice::push2D(const Rect2D& viewport) {
    pushState();
    resetState();
    setDepthTest(DEPTH_ALWAYS_PASS);
    disableLighting();
    setCullFace(CULL_NONE);
    disableDepthWrite();
    setViewport(viewport);
    setProjectionMatrix(Matrix4::orthogonalProjection(viewport.x0(), viewport.x0() + viewport.width() - 1, viewport.y0() + viewport.height() - 1, viewport.y0(), -1, 1));
}


void RenderDevice::pop2D() {
    popState();
}

////////////////////////////////////////////////////////////////////////////////////////////////
RenderDevice::RenderState::RenderState(int width, int height) {

    // WARNING: this must be kept in sync with the initialization code
    // in init();
    viewport                    = Rect2D::xywh(0, 0, width, height);

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

    renderMode                  = RenderDevice::RENDER_SOLID;

    shininess                   = 15;
    specular                    = 0.8;

    ambient                     = Color4(0.25, 0.25, 0.25, 1.0);

    lighting                    = false;
    color                       = Color4(1,1,1,1);
    normal                      = Vector3(0,0,0);

    // Note: texture units and lights initialize themselves

    objectToWorldMatrix         = CoordinateFrame();
    cameraToWorldMatrix         = CoordinateFrame();

    stencilClear                = 0;
    depthClear                  = 1;
    colorClear                  = Color4(0,0,0,1);

    shadeMode                   = SHADE_FLAT;

    vertexProgram               = NULL;
    pixelProgram                = NULL;

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        lightEnabled[i] = false;
    }

    // Set projection matrix
    double aspect;
    aspect = (double)viewport.width() / viewport.height();

    projectionMatrix = Matrix4::perspectiveProjection(-aspect, aspect, -1, 1, 0.1, 100.0);

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
    glPushAttrib(GL_TEXTURE_BIT | GL_FOG_BIT);
}


void RenderDevice::resetState() {
    setState(RenderState(getWidth(), getHeight()));
}


void RenderDevice::popState() {
    debugAssert(! inPrimitive);
    glPopAttrib();
    debugAssertM(stateStack.size() > 0, "More calls to RenderDevice::pushState() than RenderDevice::popState().");
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

    setStencilConstant(newState.stencilReference);
    setStencilTest(newState.stencilTest);
    
    setAlphaTest(newState.alphaTest, newState.alphaReference);

    setBlendFunc(newState.srcBlendFunc, newState.dstBlendFunc);

    setRenderMode(newState.renderMode);
    setPolygonOffset(newState.polygonOffset);
    setLineWidth(newState.lineWidth);
    setPointSize(newState.pointSize);

    setSpecularCoefficient(newState.specular);
    setShininess(newState.shininess);

    if (newState.lighting) {
        enableLighting();
    } else {
        disableLighting();
    }


    for (int i = 0; i < MAX_LIGHTS; ++i) {
    
        if (newState.lightEnabled[i]) {
            setLight(i, newState.light[i]);
        } else {
            setLight(i, NULL);
        }
    }

    setStencilOp(newState.stencilFail, newState.stencilZFail, newState.stencilZPass);

    setColor(newState.color);
    setNormal(newState.normal);

    for (int u = iMax(_numTextures, _numTextureCoords) - 1; u >= 0; --u) {
        if (memcmp(&(newState.textureUnit[u]), 
                   &(state.textureUnit[u]), 
                   sizeof(RenderState::TextureUnit))) {

            if (u < (int)numTextures()) {
                setTexture(u, newState.textureUnit[u].texture);

                if (u < (int)numTextureUnits()) {
                    setTextureCombineMode(u, newState.textureUnit[u].combineMode);
                    setTextureMatrix(u, newState.textureUnit[u].textureMatrix);
                }
            }
            setTexCoord(u, newState.textureUnit[u].texCoord);
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

    setProjectionMatrix(newState.projectionMatrix);

    setCullFace(newState.cullFace);

    setDepthRange(newState.lowDepthRange, newState.highDepthRange);
}


void RenderDevice::setSpecularCoefficient(double s) {
    state.specular = s;

    float spec[4];
    spec[0] = spec[1] = spec[2] = s;
    spec[3] = 1.0f;

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
}


void RenderDevice::setShininess(double s) {
    state.shininess = s;
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, s);
}


void RenderDevice::setRenderMode(RenderMode m) {
    if (state.renderMode != m) {
        state.renderMode = m;
        switch (m) {
        case RENDER_SOLID:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;

        case RENDER_WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;

        case RENDER_POINTS:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            break;
        }
    }
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


uint RenderDevice::numTextures() const {
    return _numTextures;
}


uint RenderDevice::numTextureCoords() const {
    return _numTextureCoords;
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
        dt = 0.0001;
    }

    {
        // high frame rate: A (interpolation parameter) is small
        // low frame rate: A is big
        const double A = clamp(dt, .07, 1);
    
        emwaFrameRate     = lerp(emwaFrameRate, 1 / dt, A);
        emwaTriangleRate  = lerp(emwaTriangleRate, triangleCount / dt, A);
        emwaTriangleCount = lerp(emwaTriangleCount, triangleCount, A);
    }

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
    if (state.viewport != v) {
        // Flip to OpenGL y-axis
        _glViewport(v.x0(), getHeight() - v.y1(), v.width(), v.height());
        state.viewport = v;
    }
}


void RenderDevice::setProjectionAndCameraMatrix(const GCamera& camera) {
    double pixelAspect = state.viewport.width() / state.viewport.height();

    double y = -camera.getNearPlaneZ() * tan(camera.getFieldOfView() / 2.0);
    double x = y * pixelAspect;

    double r, l, t, b, n, f;
    n = -camera.getNearPlaneZ();
    f = -camera.getFarPlaneZ();
    r = x;
    l = -x;
    t = y;
    b = -y;

    setProjectionMatrix(Matrix4::perspectiveProjection(l, r, b, t, n, f));
	setCameraToWorldMatrix(camera.getCoordinateFrame());
}


Rect2D RenderDevice::getViewport() const {
    return state.viewport;
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


static void _setStencilTest(RenderDevice::StencilTest test, int reference) {
    switch (test) {
    case RenderDevice::STENCIL_ALWAYS_PASS:
        glStencilFunc(GL_ALWAYS, reference, 0xFFFFFF);
        break;

    case RenderDevice::STENCIL_LESS:
        glStencilFunc(GL_LESS, reference, 0xFFFFFF);
        break;

    case RenderDevice::STENCIL_LEQUAL:
        glStencilFunc(GL_LEQUAL, reference, 0xFFFFFF);
        break;

    case RenderDevice::STENCIL_GREATER:
        glStencilFunc(GL_GREATER, reference, 0xFFFFFF);
        break;

    case RenderDevice::STENCIL_GEQUAL:
        glStencilFunc(GL_GEQUAL, reference, 0xFFFFFF);
        break;

    case RenderDevice::STENCIL_EQUAL:
        glStencilFunc(GL_EQUAL, reference, 0xFFFFFF);
        break;

    case RenderDevice::STENCIL_NOTEQUAL:
        glStencilFunc(GL_NOTEQUAL, reference, 0xFFFFFF);
        break;

    case RenderDevice::STENCIL_NEVER_PASS:
        glStencilFunc(GL_NEVER, reference, 0xFFFFFF);
        break;

    default:
        debugAssertM(false, "Fell through switch");
    }
}


void RenderDevice::setStencilConstant(int reference) {
    debugAssert(! inPrimitive);
    if (state.stencilReference != reference) {
        state.stencilReference = reference;
        _setStencilTest(state.stencilTest, reference);
    }
}


void RenderDevice::setStencilTest(StencilTest test) {
    debugAssert(! inPrimitive);

    if (state.stencilTest != test) {
        glEnable(GL_STENCIL_TEST);

        if (test == STENCIL_ALWAYS_PASS) {

            // Can't actually disable if the stencil op is using the test as well
            if ((state.stencilFail == STENCIL_KEEP) &&
                (state.stencilZFail == STENCIL_KEEP) &&
                (state.stencilZPass == STENCIL_KEEP)) {
                glDisable(GL_STENCIL_TEST);
            }


        } else {

            _setStencilTest(test, state.stencilReference);
        }

        state.stencilTest = test;

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

    case RenderDevice::STENCIL_ZERO:
        return GL_ZERO;

    case RenderDevice::STENCIL_REPLACE:
        return GL_REPLACE;

    case RenderDevice::STENCIL_INVERT:
        return GL_INVERT;

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
        if (state.vertexProgram != (VertexProgramRef)NULL) {
            state.vertexProgram->disable();
        }

        if (vp != (VertexProgramRef)NULL) {
            debugAssert(supportsVertexProgram());
            vp->bind();
        }

        state.vertexProgram = vp;
    }
}


void RenderDevice::setVertexProgram(
    const VertexProgramRef& vp,
    const GPUProgram::ArgList& args) {

    setVertexProgram(vp);
    
    vp->setArgs(this, args);
}


void RenderDevice::setPixelProgram(const PixelProgramRef& pp) {
    if (pp != state.pixelProgram) {
        if (state.pixelProgram != (PixelProgramRef)NULL) {
            state.pixelProgram->disable();
        }
        if (pp != (PixelProgramRef)NULL) {
            debugAssert(supportsPixelProgram());
            pp->bind();
        }
        state.pixelProgram = pp;
    }
}


void RenderDevice::setPixelProgram(
    const PixelProgramRef& pp,
    const GPUProgram::ArgList& args) {

    setPixelProgram(pp);
    
    pp->setArgs(this, args);
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

            debugAssertM(glGetInteger(GL_STENCIL_BITS) > 0,
                "Allocate stencil bits from RenderDevice::init before using the stencil buffer.");

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
        float c[] =
            {color.r / lightSaturation,
             color.g / lightSaturation,
             color.b / lightSaturation,
             1.0f};
    
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, c);
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

    // Re-load lights since the camera matrix changed.
    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (state.lightEnabled[i]) {
    
            setLight(i, &state.light[i], true);
        }
    }
}


CoordinateFrame RenderDevice::getCameraToWorldMatrix() const {
    return state.cameraToWorldMatrix;
}


Matrix4 RenderDevice::getProjectionMatrix() const {
    return state.projectionMatrix;
}


CoordinateFrame RenderDevice::getModelViewMatrix() const {
    return getCameraToWorldMatrix().inverse() * getObjectToWorldMatrix();
}


Matrix4 RenderDevice::getModelViewProjectionMatrix() const {
    return getProjectionMatrix() * getModelViewMatrix();
}


void RenderDevice::setProjectionMatrix(const Matrix4& P) {
    if (state.projectionMatrix != P) {
        state.projectionMatrix = P;
        glMatrixMode(GL_PROJECTION);
        glLoadMatrix(P);
        glMatrixMode(GL_MODELVIEW);
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


Matrix4 RenderDevice::getTextureMatrix(uint unit) {
    debugAssertM(unit < _numTextureUnits,
        format("Attempted to access texture unit %d on a device with %d units.",
        unit, _numTextureUnits));

    const double* M = state.textureUnit[unit].textureMatrix;

    return Matrix4(M[0], M[4], M[8],  M[12],
                   M[1], M[5], M[9],  M[13],
                   M[2], M[6], M[10], M[14],
                   M[3], M[7], M[11], M[15]);
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
    debugAssertM(unit < _numTextureCoords,
        format("Attempted to access texture coordinate %d on a device with %d coordinates.",
        unit, _numTextureCoords));

    state.textureUnit[unit].texCoord = texCoord;
    glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
}


void RenderDevice::setTexCoord(uint unit, const Vector3& texCoord) {
    debugAssertM(unit < _numTextureCoords,
        format("Attempted to access texture coordinate %d on a device with %d coordinates.",
        unit, _numTextureCoords));
    state.textureUnit[unit].texCoord = Vector4(texCoord, 1);
    glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
}


void RenderDevice::setTexCoord(uint unit, const Vector3int16& texCoord) {
    debugAssertM(unit < _numTextureCoords,
        format("Attempted to access texture coordinate %d on a device with %d coordinates.",
        unit, _numTextureCoords));
    state.textureUnit[unit].texCoord = Vector4(texCoord.x, texCoord.y, texCoord.z, 1);
    glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
}


void RenderDevice::setTexCoord(uint unit, const Vector2& texCoord) {
    debugAssertM(unit < _numTextureCoords,
        format("Attempted to access texture coordinate %d on a device with %d coordinates.",
        unit, _numTextureCoords));
    state.textureUnit[unit].texCoord = Vector4(texCoord.x, texCoord.y, 0, 1);
    glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
}


void RenderDevice::setTexCoord(uint unit, const Vector2int16& texCoord) {
    debugAssertM(unit < _numTextureCoords,
        format("Attempted to access texture coordinate %d on a device with %d coordinates.",
        unit, _numTextureCoords));
    state.textureUnit[unit].texCoord = Vector4(texCoord.x, texCoord.y, 0, 1);
    glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
}


void RenderDevice::setTexCoord(uint unit, double texCoord) {
    debugAssertM(unit < _numTextureCoords,
        format("Attempted to access texture coordinate %d on a device with %d coordinates.",
        unit, _numTextureCoords));
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

    // NVIDIA cards have more textures than texture units.
    // "fixedFunction" textures have an associated unit 
    // and must be glEnabled.  Programmable units *cannot*
    // be enabled.
    bool fixedFunction = ((int)unit < _numTextureUnits);

    debugAssertM(! inPrimitive, 
                 "Can't change textures while rendering a primitive.");

    debugAssertM(unit < _numTextures,
        format("Attempted to access texture %d"
               " on a device with %d textures.",
               unit, _numTextureUnits));

    TextureRef oldTexture = state.textureUnit[unit].texture;

    if (oldTexture == texture) {
        return;
    }

    state.textureUnit[unit].texture = texture;

    glActiveTextureARB(GL_TEXTURE0_ARB + unit);

    // Turn off whatever was on previously if this is a fixed function unit
    if (fixedFunction) {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_3D);
        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
        glDisable(GL_TEXTURE_1D);
        
        if (supportsTextureRectangle()) {
            glDisable(GL_TEXTURE_RECTANGLE_NV);
        }
    }

    if ((Texture*)texture != NULL) {
        GLint id = texture->getOpenGLID();
        GLint u = texture->getOpenGLTextureTarget();

        if ((GLint)currentlyBoundTexture[unit] != id) {
            glBindTexture(u, id);
            currentlyBoundTexture[unit] = id;
        }

        if (fixedFunction) {
            glEnable(u);
        }
    } else {
        // Disabled texture unit
        currentlyBoundTexture[unit] = 0;
    }

    // Force a reload of the texture matrix if invertY != old invertY.
    // This will take care of flipping the texture when necessary.
    if (((Texture*)oldTexture == NULL) ||
        ((Texture*)texture == NULL) ||
        (oldTexture->invertY != texture->invertY)) {

        if (fixedFunction) {
            // We can only set the matrix for some units
            forceSetTextureMatrix(unit, state.textureUnit[unit].textureMatrix);
        }
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


void RenderDevice::screenshotPic(GImage& dest) const {
    // TODO: adjust for gamma

    // Read back the front buffer
    glReadBuffer(GL_FRONT);
    
    dest.resize(getWidth(), getHeight(), 3);
    glReadPixels(0, 0, getWidth(), getHeight(), GL_RGB,
            GL_UNSIGNED_BYTE, dest.byte());

    // Flip right side up
    flipRGBVertical(dest.byte(), dest.byte(), getWidth(), getHeight());

    // Restore the read buffer to the back
    glReadBuffer(GL_BACK);

    int i;
    double s = getLightSaturation();

    if (s != 1.0) {
        // Adjust the coloring for gamma correction
        // Lookup table for mapping v -> v * lightSaturation;
        uint8 L[255];
        uint8 *data = dest.byte();
        
        for (i = 255; i >= 0; --i) {
            L[i] = iMin(255, iRound((double)i * s));
        }

        for (i = dest.width * dest.height * 3 - 1; i >= 0; --i) {
            data[i] = L[data[i]];
        }
    }
}



std::string RenderDevice::screenshot(const std::string& filepath) const {
    GImage screen;
    int i;

    screenshotPic(screen);

    // Save the file
    std::string basename = currentDateString();
    std::string filename;
    i = 0;
    do {
        filename = filepath + basename + "_" + format("%03d", i) + ".jpg";
        ++i;
    } while (fileExists(filename));   

    screen.save(filename);

    return filename;
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
            const double angle = a * G3D_PI * 2.0 / sides;
            const Vector3 offset(cos(angle) * x + sin(angle) * y);
            device->sendVertex(v0 + offset);
            device->sendVertex(v1 + offset);
        }
    device->endPrimitive();

    if (caps) {
        device->beginPrimitive(RenderDevice::TRIANGLE_FAN);
            for (a = 0; a <= sides; ++a) {
                const double angle = a * G3D_PI * 2.0 / sides;
                const Vector3 offset(cos(angle) * x + sin(angle) * y);
                device->sendVertex(v0 + offset);
            }
        device->endPrimitive();

        device->beginPrimitive(RenderDevice::TRIANGLE_FAN);
            for (a = 0; a <= sides; ++a) {
                const double angle = a * G3D_PI * 2.0 / sides;
                const Vector3 offset(cos(-angle) * x + sin(-angle) * y);
                device->sendVertex(v1 + offset);
            }
        device->endPrimitive();
    }
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

	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT); 

	inIndexedPrimitive = true;
}


void RenderDevice::endIndexedPrimitives() {
	debugAssert(! inPrimitive);
	debugAssert(inIndexedPrimitive);

	glPopClientAttrib();
	inIndexedPrimitive = false;
    currentVARArea = NULL;
}


void RenderDevice::setVARAreaFromVAR(const class VAR& v) {
	debugAssert(inIndexedPrimitive);
	debugAssert(! inPrimitive);
    alwaysAssertM(currentVARArea.isNull() || (v.area == currentVARArea), 
        "All vertex arrays used within a single begin/endIndexedPrimitive"
        " block must share the same VARArea.");


    if (v.area != currentVARArea) {
        currentVARArea = const_cast<VAR&>(v).area;

        if (VARArea::mode == VARArea::VBO_MEMORY) {
            // Bind the buffer (for MAIN_MEMORY, we need do nothing)
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, currentVARArea->glbuffer);
        }
    }

}


void RenderDevice::setVertexArray(const class VAR& v) {
    setVARAreaFromVAR(v);
	v.vertexPointer();
}


void RenderDevice::setVertexAttribArray(unsigned int attribNum, const class VAR& v, bool normalize) {
    setVARAreaFromVAR(v);
	v.vertexAttribPointer(attribNum, normalize);
}


void RenderDevice::setNormalArray(const class VAR& v) {
    setVARAreaFromVAR(v);
	v.normalPointer();
}


void RenderDevice::setColorArray(const class VAR& v) {
    setVARAreaFromVAR(v);
	v.colorPointer();
}


void RenderDevice::setTexCoordArray(unsigned int unit, const class VAR& v) {
    setVARAreaFromVAR(v);
	v.texCoordPointer(unit);
}


MilestoneRef RenderDevice::createMilestone(const std::string& name) {
    return new Milestone(name);
}


void RenderDevice::setMilestone(const MilestoneRef& m) {
    m->set();
}


void RenderDevice::waitForMilestone(const MilestoneRef& m) {
    m->wait();
}


void RenderDevice::setLight(int i, const GLight& light) {
    
    setLight(i, &light, false);
}


void RenderDevice::setLight(int i, void* x) {
    debugAssert(x == NULL);
    setLight(i, NULL, false);
}


void RenderDevice::setLight(int i, const GLight* _light, bool force) {
    
    debugAssert(i >= 0);
    debugAssert(i < MAX_LIGHTS);
    int gi = GL_LIGHT0 + i;

    const GLight& light = *_light;


    if (_light == NULL) {

        if (state.lightEnabled[i] || force) {
            state.lightEnabled[i] = false;
            glDisable(gi);
        }

    
    } else {

        for (int j = 0; j < 3; ++j) {
            debugAssert(light.attenuation[j] >= 0);
        }

    
        if (! state.lightEnabled[i] || force) {
            glEnable(gi);
            state.lightEnabled[i] = true;
        }

    
        if ((state.light[i] != light) || force) {
            state.light[i] = light;

            Color4 zero(0, 0, 0, 1);
            Color4 brightness(light.color / lightSaturation, 1);

            int mm = glGetInteger(GL_MATRIX_MODE);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
                glLoadIdentity();
                glLoadMatrix(state.cameraToWorldMatrix.inverse());
                glLightfv(gi, GL_POSITION,              light.position);
                glLightfv(gi, GL_SPOT_DIRECTION,        light.spotDirection);
                glLightf (gi, GL_SPOT_CUTOFF,           light.spotCutoff);
                glLightfv(gi, GL_AMBIENT,               zero);
                glLightfv(gi, GL_DIFFUSE,               brightness);
                glLightfv(gi, GL_SPECULAR,              brightness);
                glLightf (gi, GL_CONSTANT_ATTENUATION,  light.attenuation[0]);
                glLightf (gi, GL_LINEAR_ATTENUATION,    light.attenuation[1]);
                glLightf (gi, GL_QUADRATIC_ATTENUATION, light.attenuation[2]);
            glPopMatrix();
            glMatrixMode(mm);
        }    
    }
    
}


void RenderDevice::configureShadowMap(
    uint                unit,
    const Matrix4&      lightMVP,
    const TextureRef&   shadowMap) {

    debugAssertM(shadowMap->getFormat()->OpenGLBaseFormat == GL_DEPTH_COMPONENT,
        "Can only configure shadow maps from depth textures");

    debugAssertM(supportsOpenGLExtension("GL_ARB_shadow"),
        "The device does not support shadow maps");
    
	// Set up tex coord generation - all 4 coordinates required
    setTexture(unit, shadowMap);
    
    glActiveTextureARB(GL_TEXTURE0_ARB + unit);

    static const Matrix4 bias(
        0.5f, 0.0f, 0.0f, 0.5f,
        0.0f, 0.5f, 0.0f, 0.5f,
        0.0f, 0.0f, 0.5f, 0.5f - .000001,
        0.0f, 0.0f, 0.0f, 1.0f);
    
    Matrix4 textureMatrix = glGetMatrix(GL_TEXTURE_MATRIX);

	Matrix4 textureProjectionMatrix2D =
        textureMatrix * bias * lightMVP;

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, textureProjectionMatrix2D[0]);
	glEnable(GL_TEXTURE_GEN_S);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, textureProjectionMatrix2D[1]);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, textureProjectionMatrix2D[2]);
	glEnable(GL_TEXTURE_GEN_R);
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, textureProjectionMatrix2D[3]);
	glEnable(GL_TEXTURE_GEN_Q);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
}


void RenderDevice::configureReflectionMap(
    uint                textureUnit,
    TextureRef          reflectionTexture) {

    debugAssert(reflectionTexture->getDimension() == Texture::DIM_CUBE_MAP);

    // Texture coordinates will be generated in object space.
    // Set the texture matrix to transform them into camera space.
    CoordinateFrame cframe = getCameraToWorldMatrix();

    // The environment map assumes we are always in the center,
    // so zero the translation.
    cframe.translation = Vector3::ZERO;

    // The environment map is in world space.  The reflection vector
    // will automatically be multiplied by the object->camera space 
    // transformation by hardware (just like any other vector) so we 
    // take it back from camera space to world space for the correct
    // vector.
    setTexture(textureUnit, reflectionTexture);

    setTextureMatrix(textureUnit, cframe);

    glActiveTextureARB(GL_TEXTURE0_ARB + textureUnit);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
}



void RenderDevice::internalSendIndices(
    RenderDevice::Primitive primitive,
    size_t                  indexSize, 
    int                     numIndices, 
    const void*             index) const {

	GLenum i, p;

	switch (indexSize) {
	case sizeof(uint32):
		i = GL_UNSIGNED_INT;
		break;

	case sizeof(uint16):
		i = GL_UNSIGNED_SHORT;
		break;

	case sizeof(uint8):
		i = GL_UNSIGNED_BYTE;
		break;

	default:
		debugAssertM(false, "Indices must be either 8, 16, or 32-bytes each.");
        i = 0;
	}


	switch (primitive) {
    case LINES:
		p = GL_LINES;
        break;

    case LINE_STRIP:
		p = GL_LINE_STRIP;
        break;

    case TRIANGLES:
		p = GL_TRIANGLES;
        break;

    case TRIANGLE_STRIP:
		p = GL_TRIANGLE_STRIP;
		break;

    case TRIANGLE_FAN:
		p = GL_TRIANGLE_FAN;
        break;

    case QUADS:
		p = GL_QUADS;
        break;

    case QUAD_STRIP:
		p = GL_QUAD_STRIP;
        break;

    case POINTS:
        p = GL_POINTS;
        break;
    default:
        debugAssertM(false, "Fell through switch");
        p = 0;
    }

	glDrawElements(p, numIndices, i, index);
}


} // namespace
