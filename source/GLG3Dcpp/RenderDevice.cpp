/**
 @file RenderDevice.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2001-07-08
 @edited  2004-07-15
 */


#include "G3D/platform.h"

#include "GLG3D/glcalls.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/Texture.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/VARArea.h"
#include "GLG3D/VAR.h"
#include "GLG3D/SDLWindow.h"
#include "GLG3D/GLCaps.h"

namespace G3D {

RenderDevice* RenderDevice::lastRenderDeviceCreated = NULL;

static void _glViewport(double a, double b, double c, double d) {
    glViewport(iRound(a), iRound(b), 
	       iRound(a + c) - iRound(a), iRound(b + d) - iRound(b));
}


static GLenum primitiveToGLenum(RenderDevice::Primitive primitive) {
	switch (primitive) {
    case RenderDevice::LINES:
		return GL_LINES;

    case RenderDevice::LINE_STRIP:
		return GL_LINE_STRIP;

    case RenderDevice::TRIANGLES:
		return GL_TRIANGLES;

    case RenderDevice::TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;

    case RenderDevice::TRIANGLE_FAN:
		return GL_TRIANGLE_FAN;

    case RenderDevice::QUADS:
		return GL_QUADS;

    case RenderDevice::QUAD_STRIP:
		return GL_QUAD_STRIP;

    case RenderDevice::POINTS:
        return GL_POINTS;

    default:
        debugAssertM(false, "Fell through switch");
        return 0;
    }
}


std::string RenderDevice::getCardDescription() const {
    return cardDescription;
}


RenderDevice::RenderDevice() : _window(NULL), deleteWindow(false) {

    _initialized = false;
    inPrimitive = false;
    _numTextureUnits = 0;
    _numTextures = 0;
    _numTextureCoords = 0;
    emwaFrameRate = 0;
    lastTime = System::getTick();

    for (int i = 0; i < GLCaps::G3D_MAX_TEXTURE_UNITS; ++i) {
        currentlyBoundTexture[i] = 0;
    }

    lastRenderDeviceCreated = this;
}


void RenderDevice::setVARAreaMilestone() {
    MilestoneRef milestone = createMilestone("VAR Milestone");
    setMilestone(milestone);

    // Overwrite any preexisting milestone
    currentVARArea->milestone = milestone;
}


RenderDevice::~RenderDevice() {
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

    return GLCaps::supports(extension);
}


bool RenderDevice::init(
    const GWindowSettings&      _settings,
    Log*                        log) {

    deleteWindow = true;
    return init(new SDLWindow(_settings), log);
}


GWindow* RenderDevice::window() const {
    return _window;
}


bool RenderDevice::init(GWindow* window, Log* log) {
    _window = window;

    GWindowSettings settings;
    window->getSettings(settings);
    
    // Load the OpenGL extensions if they have not already been loaded.
    GLCaps::loadExtensions();
    debugAssertGLOk();

    debugAssert(! initialized());

    debugLog = log;

    beginEndFrame = 0;
    if (debugLog) {debugLog->section("Initialization");}

    debugAssert((settings.lightSaturation >= 0.5) && 
                (settings.lightSaturation <= 2.0));

    // Under Windows, reset the last error so that our debug box
    // gives the correct results
    #ifdef G3D_WIN32
        SetLastError(0);
    #endif

	const int minimumDepthBits    = iMin(16, settings.depthBits);
	const int desiredDepthBits    = settings.depthBits;

	const int minimumStencilBits  = settings.stencilBits;
	const int desiredStencilBits  = settings.stencilBits;

    const int desiredTextureUnits = 8;

    // Don't use more texture units than allowed at compile time.
    if (GLCaps::supports_GL_ARB_multitexture()) {
        _numTextureUnits = iMin(GLCaps::G3D_MAX_TEXTURE_UNITS, 
                                glGetInteger(GL_MAX_TEXTURE_UNITS_ARB));
    } else {
        _numTextureUnits = 1;
    }

    // NVIDIA cards with GL_NV_fragment_program have different 
    // numbers of texture coords, units, and textures
    if (GLCaps::supports("GL_NV_fragment_program")) {
        glGetIntegerv(GL_MAX_TEXTURE_COORDS_NV, &_numTextureCoords);
        _numTextureCoords = iClamp(_numTextureCoords,
                                   _numTextureUnits,
                                   GLCaps::G3D_MAX_TEXTURE_UNITS);

        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_NV, &_numTextures);
        _numTextures = iClamp(_numTextures,
                              _numTextureUnits, 
                              GLCaps::G3D_MAX_TEXTURE_UNITS);
    } else {
        _numTextureCoords = _numTextureUnits;
        _numTextures      = _numTextureUnits;
    }

    if (! GLCaps::supports_GL_ARB_multitexture()) {
        // No multitexture
        if (debugLog) {
            debugLog->println("No GL_ARB_multitexture support: "
                              "forcing number of texture units "
                              "to no more than 1");
        }
        _numTextureCoords = iMax(1, _numTextureCoords);
        _numTextures      = iMax(1, _numTextures);
        _numTextureUnits  = iMax(1, _numTextureUnits);
    }
    debugAssertGLOk();

    if (debugLog) {
		int t = glGetInteger(GL_MAX_TEXTURE_UNITS_ARB);

		if (GLCaps::supports_GL_ARB_fragment_program()) {
			t = glGetInteger(GL_MAX_TEXTURE_IMAGE_UNITS_ARB);
		}

        debugLog->printf("numTextureCoords                      = %d\n"
                         "numTextures                           = %d\n"
                         "numTextureUnits                       = %d\n"
                         "glGet(GL_MAX_TEXTURE_UNITS_ARB)       = %d\n"
                         "glGet(GL_MAX_TEXTURE_IMAGE_UNITS_ARB) = %d\n",
                         _numTextureCoords, _numTextures, _numTextureUnits,
                         glGetInteger(GL_MAX_TEXTURE_UNITS_ARB),
						 t);
    }

    if (debugLog) {debugLog->println("Setting video mode");}

    setVideoMode();

    if (!strcmp((char*)glGetString(GL_RENDERER), "GDI Generic") && debugLog) {
        debugLog->printf(
         "\n*********************************************************\n"
           "* WARNING: This computer does not have correctly        *\n"
           "*          installed graphics drivers and is using      *\n"
           "*          the default Microsoft OpenGL implementation. *\n"
           "*          Most graphics capabilities are disabled.  To *\n"
           "*          correct this problem, download and install   *\n"
           "*          the latest drivers for the graphics card.    *\n"
           "*********************************************************\n\n");
    }

    setCaption("Graphics3D");

	glViewport(0, 0, getWidth(), getHeight());

    int depthBits, stencilBits, redBits, greenBits, blueBits, alphaBits;
    depthBits       = glGetInteger(GL_DEPTH_BITS);
    stencilBits     = glGetInteger(GL_STENCIL_BITS);
    redBits         = glGetInteger(GL_RED_BITS);
    greenBits       = glGetInteger(GL_GREEN_BITS);
    blueBits        = glGetInteger(GL_BLUE_BITS);
    alphaBits       = glGetInteger(GL_ALPHA_BITS);
    debugAssertGLOk();

    bool depthOk   = depthBits >= minimumDepthBits;
    bool stencilOk = stencilBits >= minimumStencilBits;

    if (debugLog) {
        debugLog->printf("Operating System: %s\n",
                         System::operatingSystem().c_str());

        debugLog->printf("Processor Architecture: %s\n\n", 
                         System::cpuArchitecture().c_str());

        debugLog->printf("GL Vendor:      %s\n", GLCaps::vendor().c_str());

        debugLog->printf("GL Renderer:    %s\n", GLCaps::renderer().c_str());

        debugLog->printf("GL Version:     %s\n", GLCaps::glVersion().c_str());

        debugLog->printf("Driver version: %s\n\n", GLCaps::driverVersion().c_str());

        debugLog->printf(
            "GL extensions: \"%s\"\n\n",
            glGetString(GL_EXTENSIONS));
    }
 

    cardDescription = GLCaps::renderer() + " " + GLCaps::driverVersion();

    if (debugLog) {
    debugLog->section("Video Status");

    GWindowSettings actualSettings;
    window->getSettings(actualSettings);

    // This call is here to make GCC realize that isOk is used.
    (void)isOk(false);
    (void)isOk((void*)NULL);

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
             "FSAA                      %2d    %2d    %s\n"

             "Width             %8d pixels           %s\n"
             "Height            %8d pixels           %s\n"
             "Mode                 %10s             %s\n\n",

             minimumDepthBits, desiredDepthBits, depthBits, isOk(depthOk),
             minimumStencilBits, desiredStencilBits, stencilBits, isOk(stencilOk),

             alphaBits, "ok",
             redBits, "ok", 
             greenBits, "ok", 
             blueBits, "ok", 

             settings.fsaaSamples, actualSettings.fsaaSamples,
             isOk(settings.fsaaSamples == actualSettings.fsaaSamples),

             settings.width, "ok",
             settings.height, "ok",
             (settings.fullScreen ? "Fullscreen" : "Windowed"), "ok"
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


#ifdef G3D_WIN32

HDC RenderDevice::getWindowHDC() const {
    return wglGetCurrentDC();
}

#endif

void RenderDevice::setGamma(
    double              brightness,
    double              gamma) {
    
    Array<uint16> gammaRamp(256);

    for (int i = 0; i < 256; ++i) {
        gammaRamp[i] =
            (uint16)min(65535, 
                      max(0, 
                      pow((brightness * (i + 1)) / 256.0, gamma) * 
                          65535 + 0.5));
	}
    
    _window->setGammaRamp(gammaRamp);
}


void RenderDevice::notifyResize(int w, int h) {
    _window->notifyResize(w, h);
}


void RenderDevice::setVideoMode() {

    debugAssertM(stateStack.size() == 0, 
                 "Cannot call setVideoMode between pushState and popState");
    debugAssertM(beginEndFrame == 0, 
                 "Cannot call setVideoMode between beginFrame and endFrame");

    // Reset all state

    GWindowSettings settings;
    _window->getSettings(settings);

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

	glClearDepth(1.0);

    {
        if (debugLog) {
            debugLog->printf("Setting brightness to %g\n", 
                                        settings.lightSaturation);
        }
        // Adjust the gamma so that changing the 
        // light intensities won't affect the actual screen
        // brightness.  This method due to John Carmack.
        lightSaturation = settings.lightSaturation;
        brightScale = 1.0 / lightSaturation;
        setGamma(lightSaturation, 1.0);
        if (debugLog) debugLog->println("Brightness set.");
    }

    // Enable proper specular lighting
    if (GLCaps::supports("GL_EXT_separate_specular_color")) {
        if (debugLog) {
            debugLog->println("Enabling separate specular lighting.\n");
        }
        glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL_EXT, 
                      GL_SEPARATE_SPECULAR_COLOR_EXT);
        debugAssertGLOk();
    } else if (debugLog) {
        debugLog->println("Cannot enable separate specular lighting, extension not supported.\n");
    }


    // Make sure we use good interpolation
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    // glHint(GL_GENERATE_MIPMAP_HINT_EXT, GL_NICEST);
    if (GLCaps::supports("GL_ARB_multisample")) {
        glEnable(GL_MULTISAMPLE_ARB);
    }

    debugAssertGLOk();
    if (GLCaps::supports("GL_NV_multisample_filter_hint")) {
        glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    }
    glEnable(GL_NORMALIZE);

    debugAssertGLOk();
    if (GLCaps::supports_GL_ARB_stencil_two_side()) {
        glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
    }

    debugAssertGLOk();
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    debugAssertGLOk();
    glEnable(GL_COLOR_MATERIAL);
    debugAssertGLOk();

    // Compute specular term correctly
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
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

        if (GLCaps::supports_GL_ARB_stencil_two_side()) {
            glActiveStencilFaceEXT(GL_BACK);
        }
        for (int i = 0; i < 2; ++i) {
            glStencilMask(~0);
            glDisable(GL_STENCIL_TEST);
            glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
            glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
            glDisable(GL_ALPHA_TEST);
            if (GLCaps::supports_GL_ARB_stencil_two_side()) {
                glActiveStencilFaceEXT(GL_FRONT);
            }
        }

        glDepthFunc(GL_LEQUAL);
        glEnable(GL_DEPTH_TEST);

        glDisable(GL_BLEND);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glLineWidth(1);
        glPointSize(1);

        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, state.ambient);

        glDisable(GL_LIGHTING);

        glDrawBuffer(GL_BACK);

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
        if (glMultiTexCoord4fvARB != NULL) {
            for (int t = _numTextureCoords - 1; t >= 0; --t) {
                float f[] = {0,0,0,1};
                glMultiTexCoord4fvARB(GL_TEXTURE0_ARB + t, f);
            }
        } else if (_numTextureCoords > 0) {
            glTexCoord(Vector4(0,0,0,1));
        }

        if (glActiveTextureARB != NULL) {
            glActiveTextureARB(GL_TEXTURE0_ARB);
        }
    }
    debugAssertGLOk();

    if (debugLog) debugLog->printf("Done setting initial state.\n");
}


void RenderDevice::setCaption(const std::string& caption) {
    _window->setCaption(caption);
}


int RenderDevice::getWidth() const {
    return _window->width();
}


int RenderDevice::getHeight() const {
    return _window->height();
}


int RenderDevice::width() const {
    return _window->width();
}


int RenderDevice::height() const {
    return _window->height();
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

    if (debugLog) {debugLog->println("Shutting down RenderDevice.");}

    if (deleteWindow) {
        delete _window;
    }
}


void RenderDevice::push2D() {
    push2D(getViewport());
}


void RenderDevice::push2D(const Rect2D& viewport) {
    pushState();
    setDepthTest(DEPTH_ALWAYS_PASS);
    disableLighting();
    setCullFace(CULL_NONE);
    disableDepthWrite();
    setViewport(viewport);
	setObjectToWorldMatrix(CoordinateFrame());
	setCameraToWorldMatrix(CoordinateFrame());
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

    depthTest                   = DEPTH_LEQUAL;
    stencilTest                 = STENCIL_ALWAYS_PASS;
    stencilReference            = 0;
    alphaTest                   = ALPHA_ALWAYS_PASS;
    alphaReference              = 0.0;

    srcBlendFunc                = BLEND_ONE;
    dstBlendFunc                = BLEND_ZERO;
    blendEq                     = BLENDEQ_ADD;

    drawBuffer                  = BUFFER_BACK;

    frontStencilFail            = STENCIL_KEEP;
    frontStencilZFail           = STENCIL_KEEP;
    frontStencilZPass           = STENCIL_KEEP;
    backStencilFail             = STENCIL_KEEP;
    backStencilZFail            = STENCIL_KEEP;
    backStencilZPass            = STENCIL_KEEP;

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

    vertexAndPixelShader        = NULL;
	objectShader				= NULL;
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

    setDrawBuffer(newState.drawBuffer);

    setShadeMode(newState.shadeMode);
    setDepthTest(newState.depthTest);

    setStencilConstant(newState.stencilReference);
    setStencilTest(newState.stencilTest);
    
    setAlphaTest(newState.alphaTest, newState.alphaReference);

    setBlendFunc(newState.srcBlendFunc, newState.dstBlendFunc, newState.blendEq);

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

    setAmbientLightColor(newState.ambient);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
    
        if (newState.lightEnabled[i]) {
            setLight(i, newState.light[i]);
        } else {
            setLight(i, NULL);
        }
    }

    setStencilOp(
        newState.frontStencilFail, newState.frontStencilZFail, newState.frontStencilZPass,
        newState.backStencilFail, newState.backStencilZFail, newState.backStencilZPass);

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

    setVertexAndPixelShader(newState.vertexAndPixelShader);
	setObjectShader(newState.objectShader);

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


void RenderDevice::runObjectShader() {
	if (! state.objectShader.isNull()) {
		state.objectShader->run(this);
	}
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

	if (m == RENDER_CURRENT) {
		return;
	}

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


void RenderDevice::setDrawBuffer(Buffer b) {
    if (b != state.drawBuffer) {
        state.drawBuffer = b;
        switch (b) {
        case BUFFER_FRONT:
            glDrawBuffer(GL_FRONT);
            break;

        case BUFFER_FRONT_LEFT:
            glDrawBuffer(GL_FRONT_LEFT);
            break;

        case BUFFER_FRONT_RIGHT:
            glDrawBuffer(GL_FRONT_RIGHT);
            break;

        case BUFFER_BACK:
            glDrawBuffer(GL_BACK);
            break;

        case BUFFER_BACK_LEFT:
            glDrawBuffer(GL_BACK_LEFT);
            break;

        case BUFFER_BACK_RIGHT:
            glDrawBuffer(GL_BACK_RIGHT);
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
    debugAssertM(stateStack.size() == 0, "Missing RenderDevice::popState or RenderDevice::pop2D.");

    _window->swapGLBuffers();

    double now = System::getTick();
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

    if ((emwaFrameRate == inf()) || (isNaN(emwaFrameRate))) {
        emwaFrameRate = 1000000;
    } else if (emwaFrameRate < 0) {
        emwaFrameRate = 0;
    }

    if ((emwaTriangleRate == inf()) || isNaN(emwaTriangleRate)) {
        emwaTriangleRate = 1e20;
    } else if (emwaTriangleRate < 0) {
        emwaTriangleRate = 0;
    }

    if ((emwaTriangleCount == inf()) || isNaN(emwaTriangleCount)) {
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

	if (test == DEPTH_CURRENT) {
		return;
	}

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

	if (test == RenderDevice::STENCIL_CURRENT) {
		return;
	}

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

        if (GLCaps::supports_GL_ARB_stencil_two_side()) {
            glActiveStencilFaceEXT(GL_BACK);
        }

        _setStencilTest(state.stencilTest, reference);

        if (GLCaps::supports_GL_ARB_stencil_two_side()) {
            glActiveStencilFaceEXT(GL_FRONT);
            _setStencilTest(state.stencilTest, reference);
        }
    }
}


void RenderDevice::setStencilTest(StencilTest test) {

	if (test == STENCIL_CURRENT) {
		return;
	}

    debugAssert(! inPrimitive);

    if (state.stencilTest != test) {
        glEnable(GL_STENCIL_TEST);

        if (test == STENCIL_ALWAYS_PASS) {

            // Can't actually disable if the stencil op is using the test as well
            if ((state.frontStencilFail   == STENCIL_KEEP) &&
                (state.frontStencilZFail  == STENCIL_KEEP) &&
                (state.frontStencilZPass  == STENCIL_KEEP) &&
                (! GLCaps::supports_GL_ARB_stencil_two_side() ||
                 ((state.backStencilFail  == STENCIL_KEEP) &&
                  (state.backStencilZFail == STENCIL_KEEP) &&
                  (state.backStencilZPass == STENCIL_KEEP)))) {
                glDisable(GL_STENCIL_TEST);
            }


        } else {

            if (GLCaps::supports_GL_ARB_stencil_two_side()) {
                glActiveStencilFaceEXT(GL_BACK);
            }

            _setStencilTest(test, state.stencilReference);

            if (GLCaps::supports_GL_ARB_stencil_two_side()) {
                glActiveStencilFaceEXT(GL_FRONT);
                _setStencilTest(test, state.stencilReference);
            }
        }

        state.stencilTest = test;

    }
}


void RenderDevice::setAlphaTest(AlphaTest test, double reference) {
    debugAssert(! inPrimitive);


	if (test == ALPHA_CURRENT) {
		return;
	}

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
        if (GLCaps::supports_GL_EXT_stencil_wrap()) {
            return GL_INCR_WRAP_EXT;
        }
        // Intentionally fall through

    case RenderDevice::STENCIL_INCR:
        return GL_INCR;


    case RenderDevice::STENCIL_DECR_WRAP:
        if (GLCaps::supports_GL_EXT_stencil_wrap()) {
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


void RenderDevice::setObjectShader(const ObjectShaderRef& s) {
	if (s != state.objectShader) {
		state.objectShader = s;
	}
}


void RenderDevice::setVertexAndPixelShader(const VertexAndPixelShaderRef& s) {
    if (s != state.vertexAndPixelShader) {

        state.vertexAndPixelShader = s;
        if (s.isNull()) {
            // Disables the programmable pipeline
            glUseProgramObjectARB(0);
        } else {
            alwaysAssertM(s->ok(), s->messages());
            glUseProgramObjectARB(s->glProgramObject());
        }

    }
}


void RenderDevice::setVertexAndPixelShader(const VertexAndPixelShaderRef& s,
                             const VertexAndPixelShader::ArgList& args) {
    setVertexAndPixelShader(s);

    if (! s.isNull()) {
        s->bindArgList(this, args);
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
    StencilOp                       frontStencilFail,
    StencilOp                       frontZFail,
    StencilOp                       frontZPass,
    StencilOp                       backStencilFail,
    StencilOp                       backZFail,
    StencilOp                       backZPass) {

	if (frontStencilFail == STENCIL_CURRENT) {
		frontStencilFail = state.frontStencilFail;
	}
	
	if (frontZFail == STENCIL_CURRENT) {
		frontZFail = state.frontStencilZFail;
	}
	
	if (frontZPass == STENCIL_CURRENT) {
		frontZPass = state.frontStencilZPass;
	}

	if (backStencilFail == STENCIL_CURRENT) {
		backStencilFail = state.backStencilFail;
	}
	
	if (backZFail == STENCIL_CURRENT) {
		backZFail = state.backStencilZFail;
	}
	
	if (backZPass == STENCIL_CURRENT) {
		backZPass = state.backStencilZPass;
	}
    
	if ((frontStencilFail  != state.frontStencilFail) ||
        (frontZFail        != state.frontStencilZFail) ||
        (frontZPass        != state.frontStencilZPass) || 
        (GLCaps::supports_GL_ARB_stencil_two_side() && 
        ((backStencilFail  != state.backStencilFail) ||
         (backZFail        != state.backStencilZFail) ||
         (backZPass        != state.backStencilZPass)))) { 

        if (GLCaps::supports_GL_ARB_stencil_two_side()) {
            glActiveStencilFaceEXT(GL_FRONT);
        }

        // Set front face operation
        glStencilOp(
            toGLStencilOp(frontStencilFail),
            toGLStencilOp(frontZFail),
            toGLStencilOp(frontZPass));

        // Set back face operation
        if (GLCaps::supports_GL_ARB_stencil_two_side()) {
            glActiveStencilFaceEXT(GL_BACK);

            glStencilOp(
                toGLStencilOp(backStencilFail),
                toGLStencilOp(backZFail),
                toGLStencilOp(backZPass));
            
            glActiveStencilFaceEXT(GL_FRONT);
        }


        // Need to manage the mask as well

        if ((frontStencilFail  == STENCIL_KEEP) &&
            (frontZPass        == STENCIL_KEEP) && 
            (frontZFail        == STENCIL_KEEP) &&
            (! GLCaps::supports_GL_ARB_stencil_two_side() ||
            ((backStencilFail  == STENCIL_KEEP) &&
             (backZPass        == STENCIL_KEEP) &&
             (backZFail        == STENCIL_KEEP)))) {

            // Turn off writing.  May need to turn off the stencil test.
            if (state.stencilTest == STENCIL_ALWAYS_PASS) {
                // Test doesn't need to be on
                glDisable(GL_STENCIL_TEST);
            }

        } else {

            debugAssertM(glGetInteger(GL_STENCIL_BITS) > 0,
                "Allocate stencil bits from RenderDevice::init before using the stencil buffer.");

            // Turn on writing.  We also need to turn on the
            // stencil test in this case.

            if (state.stencilTest == STENCIL_ALWAYS_PASS) {
                // Test is not already on
                glEnable(GL_STENCIL_TEST);

                if (GLCaps::supports_GL_ARB_stencil_two_side()) {
                    glActiveStencilFaceEXT(GL_BACK);
                }

                glStencilFunc(GL_ALWAYS, state.stencilReference, 0xFFFFFF);

                if (GLCaps::supports_GL_ARB_stencil_two_side()) {
                    glActiveStencilFaceEXT(GL_FRONT);
                    glStencilFunc(GL_ALWAYS, state.stencilReference, 0xFFFFFF);
                }
            }
        }

        state.frontStencilFail  = frontStencilFail;
        state.frontStencilZFail = frontZFail;
        state.frontStencilZPass = frontZPass;
        state.backStencilFail   = backStencilFail;
        state.backStencilZFail  = backZFail;
        state.backStencilZPass  = backZPass;
    }
}


void RenderDevice::setStencilOp(
    StencilOp           fail,
    StencilOp           zfail,
    StencilOp           zpass) {
    debugAssert(! inPrimitive);

    setStencilOp(fail, zfail, zpass, fail, zfail, zpass);
}


static GLenum toGLBlendEq(RenderDevice::BlendEq e) {
    switch (e) {
    case RenderDevice::BLENDEQ_MIN:
        debugAssert(GLCaps::supports("GL_EXT_blend_minmax"));
        return GL_MIN;

    case RenderDevice::BLENDEQ_MAX:
        debugAssert(GLCaps::supports("GL_EXT_blend_minmax"));
        return GL_MAX;

    case RenderDevice::BLENDEQ_ADD:
        return GL_FUNC_ADD;

    case RenderDevice::BLENDEQ_SUBTRACT:
        debugAssert(GLCaps::supports("GL_EXT_blend_subtract"));
        return GL_FUNC_SUBTRACT;

    case RenderDevice::BLENDEQ_REVERSE_SUBTRACT:
        debugAssert(GLCaps::supports("GL_EXT_blend_subtract"));
        return GL_FUNC_REVERSE_SUBTRACT;

    default:
        debugAssertM(false, "Fell through switch");
        return GL_ZERO;
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
    BlendFunc dst,
    BlendEq   eq) {
    debugAssert(! inPrimitive);

	if (src == BLEND_CURRENT) {
		src = state.srcBlendFunc;
	}

	if (dst == BLEND_CURRENT) {
		dst = state.dstBlendFunc;
	}

    if (eq == BLENDEQ_CURRENT) {
        eq = state.blendEq;
    }

    if ((state.dstBlendFunc != dst) ||
        (state.srcBlendFunc != src) ||
        (state.blendEq != eq)) {

        if ((dst == BLEND_ZERO) && (src == BLEND_ONE) && ((eq == BLENDEQ_ADD) || (eq == BLENDEQ_SUBTRACT))) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
            glBlendFunc(toGLBlendFunc(src), toGLBlendFunc(dst));

            debugAssert(eq == BLENDEQ_ADD ||
                GLCaps::supports("GL_EXT_blend_minmax") ||
                GLCaps::supports("GL_EXT_blend_subtract"));

            if (glBlendEquationEXT != 0) {
                glBlendEquationEXT(toGLBlendEq(eq));
            }
        }
        state.dstBlendFunc = dst;
        state.srcBlendFunc = src;
        state.blendEq = eq;
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
    debugAssertM((int)unit < _numTextureUnits,
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
	const Matrix4&	     m) {

	double d[16];
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			d[r * 4 + c] = m[r][c];
		}
	}

	setTextureMatrix(unit, d);
}


void RenderDevice::setTextureMatrix(
    uint                 unit,
    const double*        m) {

    debugAssert(! inPrimitive);
    debugAssertM((int)unit < _numTextureUnits,
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

	if (mode == TEX_CURRENT) {
		return;
	}

    debugAssertM((int)unit < _numTextureUnits,
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
    debugAssertM((int)unit < _numTextureUnits,
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
    debugAssertM((int)unit < _numTextureCoords,
        format("Attempted to access texture coordinate %d on a device with %d coordinates.",
        unit, _numTextureCoords));

    state.textureUnit[unit].texCoord = texCoord;
    if (GLCaps::supports_GL_ARB_multitexture()) {
        glMultiTexCoord(GL_TEXTURE0_ARB + unit, texCoord);
    } else {
        debugAssertM(unit == 0, "This machine has only one texture unit");
        glTexCoord(texCoord);
    }
}


void RenderDevice::setTexCoord(uint unit, const Vector3& texCoord) {
    setTexCoord(unit, Vector4(texCoord, 1));
}


void RenderDevice::setTexCoord(uint unit, const Vector3int16& texCoord) {
    setTexCoord(unit, Vector4(texCoord.x, texCoord.y, texCoord.z, 1));
}


void RenderDevice::setTexCoord(uint unit, const Vector2& texCoord) {
    setTexCoord(unit, Vector4(texCoord, 0, 1));
}


void RenderDevice::setTexCoord(uint unit, const Vector2int16& texCoord) {
    setTexCoord(unit, Vector4(texCoord.x, texCoord.y, 0, 1));
}


void RenderDevice::setTexCoord(uint unit, double texCoord) {
    setTexCoord(unit, Vector4(texCoord, 0, 0, 1));
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

	runObjectShader();
    
    inPrimitive = true;
    currentPrimitiveVertexCount = 0;
    currentPrimitive = p;

    glBegin(primitiveToGLenum(p));
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

    debugAssertM((int)unit < _numTextures,
        format("Attempted to access texture %d"
               " on a device with %d textures.",
               unit, _numTextures));

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
            glDisable(GL_TEXTURE_RECTANGLE_EXT);
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
		         (getHeight() - 1) - y,
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


void RenderDevice::screenshotPic(GImage& dest, bool useBackBuffer) const {
    if (useBackBuffer) {
        glReadBuffer(GL_BACK);
    } else {
        glReadBuffer(GL_FRONT);
    }
    
    if ((dest.channels != 3) ||
        (dest.width != getWidth()) ||
        (dest.height != getHeight())) {
        // Only resize if the current size is not correct
        dest.resize(getWidth(), getHeight(), 3);
    }
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


bool RenderDevice::supportsTextureFormat(const TextureFormat* fmt) {
    debugAssertM(! inPrimitive, 
        "Cannot call supportsTextureFormat between beginPrimitive and endPrimitive.");
    return GLCaps::supports(fmt);
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

    const Vector3 vector[3] = {Vector3::unitX(), Vector3::unitY(), Vector3::unitZ()};
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
                if (light.specular) {
                    glLightfv(gi, GL_SPECULAR,              brightness);
                } else {
                    glLightfv(gi, GL_SPECULAR,              zero);
                }
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

    debugAssertM(GLCaps::supports("GL_ARB_shadow"),
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
    cframe.translation = Vector3::zero();

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


void RenderDevice::sendSequentialIndices(RenderDevice::Primitive primitive, int numVertices) {

	runObjectShader();

    glDrawArrays(primitiveToGLenum(primitive), 0, numVertices);
    // Mark all active arrays as busy.
    setVARAreaMilestone();

	countPrimitive(primitive, numVertices);
}


void RenderDevice::internalSendIndices(
    RenderDevice::Primitive primitive,
    size_t                  indexSize, 
    int                     numIndices, 
    const void*             index) {

	runObjectShader();

	GLenum i;

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

    GLenum p = primitiveToGLenum(primitive);

	glDrawElements(p, numIndices, i, index);
}


bool RenderDevice::supportsTwoSidedStencil() const {
    return GLCaps::supports_GL_ARB_stencil_two_side();
}


bool RenderDevice::supportsTextureRectangle() const {
    return GLCaps::supports_GL_EXT_texture_rectangle();
}


bool RenderDevice::supportsVertexProgram() const {
    return GLCaps::supports_GL_ARB_vertex_program();
}


bool RenderDevice::supportsVertexProgramNV2() const {
    return GLCaps::supports_GL_NV_vertex_program2();
}


bool RenderDevice::supportsPixelProgram() const {
    return GLCaps::supports_GL_ARB_fragment_program();
}


bool RenderDevice::supportsVertexBufferObject() const { 
    return GLCaps::supports_GL_ARB_vertex_buffer_object();
}

} // namespace
