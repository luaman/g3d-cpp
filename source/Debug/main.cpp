/**
  @file demos/main.cpp

  This is a sample main.cpp to get you started with G3D.  It is
  designed to make writing an application easy.  Although the
  GApp/GApplet infrastructure is helpful for most projects,
  you are not restricted to using it-- choose the level of
  support that is best for your project (see the G3D Map in the
  documentation).

  @author Morgan McGuire, matrix@graphics3d.com
 */

#include <G3DAll.h>


#if G3D_VER != 60300
    #error Requires G3D 6.03
#endif

#ifndef GLUTWINDOW_H
#define GLUTWINDOW_H
#define GLUT_API_VERSION 4
#include <GL/glut.h>


/**
 GWindow that uses the glut API http://www.opengl.org/resources/libraries/glut/spec3/spec3.html.

  Not supported by GlutWindow:
  <UL>
   <LI> Joysticks
   <LI> Gamma ramp
   <LI> Window Icons
   <LI> Input capture
  </UL>

 Additionally, Glut does not support all keystrokes (e.g. the key-down event for a modifier
 cannot be detected, scan-codes are not available) so GlutWindow fails to report some keyboard
 events.

 @cite Glut by Mark Kilgard, ported to Windows by Nate Robbins
 */
// The public API of this implementation allows only one window because it does
// not provide a mechanism for switching active windows/contexts.  However,
// internally it has been coded so that this will be a simple change.
class GlutWindow : public GWindow {
private:

    /** Has glutInit been called? (only allowed once per program)*/
    static bool             glutInitialized;

    /** Underlying glut window handle */
    int                     glutWindowHandle;

    bool                    _mouseVisible;

    std::string             _windowTitle;

    Rect2D                  _dimensions;

    GWindowSettings         settings;

    Queue<GEvent>           eventQueue;

    Vector2                 mouse;

    uint8                   mouseButtons;

    /** Where to send GLUT events */
    static GlutWindow*      currentGlutWindow;

    /** Adds an event to the queue-- is synchronized! */
    void postEvent(GEvent& evt);

    // Glut callbacks:
    static void g_reshape(int width, int height);
    static void g_keyboard(unsigned char c, int x, int y);
    static void g_keyboardspecial(int key, int x, int y);
    static void g_keyboardup(unsigned char c, int x, int y);
    static void g_keyboardspecialup(int key, int x, int y);
    static void g_mousemotion(int x, int y);
    static void g_mousebtn(int b, int s, int x, int y);
    static void g_idle();

public:

    GlutWindow(const GWindowSettings& s);
    virtual ~GlutWindow();
    virtual void getSettings(GWindowSettings& settings) const;
    virtual int width() const;
    virtual int height() const;
    virtual Rect2D dimensions() const;
    virtual void setDimensions(const Rect2D& dims);
    virtual void setPosition(int x, int y);
    virtual bool hasFocus() const;
    virtual std::string getAPIVersion() const;
    virtual std::string getAPIName() const;
    virtual void setGammaRamp(const Array<uint16>& gammaRamp);
    virtual void setCaption(const std::string& caption);
    virtual int numJoysticks() const;
    virtual std::string joystickName(unsigned int sticknum);
    virtual std::string caption();
    virtual void setIcon(const GImage& image);
    virtual void swapGLBuffers();
    virtual void notifyResize(int w, int h);
    virtual bool pollEvent(GEvent& e);

    virtual bool requiresMainLoop() const {
        return true;
    }

    virtual void runMainLoop(void (*callback)(void));
    virtual void setRelativeMousePosition(double x, double y);
    virtual void setRelativeMousePosition(const Vector2& p);
    virtual void getRelativeMouseState(Vector2& position, uint8& mouseButtons) const;
    virtual void getRelativeMouseState(int& x, int& y, uint8& mouseButtons) const;
    virtual void getRelativeMouseState(double& x, double& y, uint8& mouseButtons) const;
    virtual void getJoystickState(unsigned int stickNum, Array<float>& axis, Array<bool>& button);
    virtual void setInputCapture(bool c);
    virtual bool inputCapture() const;
    virtual void setMouseVisible(bool b);
    virtual bool mouseVisible() const;
};

GlutWindow* GlutWindow::currentGlutWindow = NULL;
bool GlutWindow::glutInitialized = false;

void GlutWindow::postEvent(GEvent& evt) {
    eventQueue.pushBack(evt);
}

void GlutWindow::g_idle() {
    // Request another redisplay
    glutPostRedisplay();
}


void GlutWindow::g_reshape(int width, int height) {
    GEvent e;
    e.resize.type = SDL_VIDEORESIZE;
    e.resize.w = width;
    e.resize.h = height;

    currentGlutWindow->postEvent(e);
}


static void glutToSDLKeyEvent(GEvent& e, unsigned char c) {
    e.key.keysym.unicode = c;

    if ((c >= 'A') && (c <= 'Z')) {
        // Make key codes lower case canonically
        e.key.keysym.sym = (SDLKey)(c - 'A' + 'a');
    } else {
        e.key.keysym.sym = (SDLKey)c;
    }
    e.key.keysym.scancode = 0;

    e.key.keysym.mod = KMOD_NONE;
    int mods = glutGetModifiers();

    if (mods & GLUT_ACTIVE_SHIFT) {
        e.key.keysym.mod = (SDLMod)(e.key.keysym.mod | KMOD_LSHIFT);
    }

    if (mods & GLUT_ACTIVE_CTRL) {
        e.key.keysym.mod = (SDLMod)(e.key.keysym.mod | KMOD_LCTRL);
    }

    if (mods & GLUT_ACTIVE_ALT) {
        e.key.keysym.mod = (SDLMod)(e.key.keysym.mod | KMOD_LALT);
    }

}

void GlutWindow::g_keyboard(unsigned char c, int x, int y) {

    GEvent e;
    e.key.type = SDL_KEYDOWN;
    e.key.state = SDL_PRESSED;

    glutToSDLKeyEvent(e, c);

    currentGlutWindow->postEvent(e);
}


void GlutWindow::g_keyboardspecial(int key, int x, int y) {
    // TODO
}


void GlutWindow::g_keyboardup(unsigned char c, int x, int y) {
    GEvent e;
    e.key.type = SDL_KEYUP;
    e.key.state = SDL_RELEASED;

    glutToSDLKeyEvent(e, c);

    currentGlutWindow->postEvent(e);
}


void GlutWindow::g_keyboardspecialup(int key, int x, int y) {
    // TODO
}


void GlutWindow::g_mousemotion(int x, int y) {
    GEvent e;
    e.motion.type = SDL_MOUSEMOTION;
    e.motion.state = currentGlutWindow->mouseButtons;
    e.motion.xrel = x - currentGlutWindow->mouse.x;
    e.motion.yrel = y - currentGlutWindow->mouse.y;
    e.motion.x = x;
    e.motion.y = y;

    currentGlutWindow->mouse.x = x;
    currentGlutWindow->mouse.y = y;

    currentGlutWindow->postEvent(e);
}


void GlutWindow::g_mousebtn(int b, int s, int x, int y) {
    currentGlutWindow->mouse.x = x;
    currentGlutWindow->mouse.y = y;

    // Turn corresponding mouseButtons bit on/off and
    // make a GEvent
    GEvent e;

    switch (b) {
    case GLUT_LEFT_BUTTON:
        e.button.button = SDL_BUTTON_LEFT;
        if (s == GLUT_UP) {
            currentGlutWindow->mouseButtons &= 0xFE;
        } else {
            currentGlutWindow->mouseButtons |= 0x01;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        // Button index 1
        e.button.button = SDL_BUTTON_RIGHT;
        if (s == GLUT_UP) {
            currentGlutWindow->mouseButtons &= 0xFD;
        } else {
            currentGlutWindow->mouseButtons |= 0x02;
        }
        break;

    case GLUT_MIDDLE_BUTTON:
        // Button index 2
        e.button.button = SDL_BUTTON_MIDDLE;
        if (s == GLUT_UP) {
            currentGlutWindow->mouseButtons &= 0xFB;
        } else {
            currentGlutWindow->mouseButtons |= 0x04;
        }
        break;
    }

    if (s == GLUT_UP) {
        e.button.type = SDL_MOUSEBUTTONUP;
        e.button.state = SDL_RELEASED;
    } else {
        e.button.type = SDL_MOUSEBUTTONDOWN;
        e.button.state = SDL_PRESSED;
    }

    e.button.x = x;
    e.button.y = y;

    currentGlutWindow->postEvent(e);
}


/** Called from GlutWindow::GlutWindow */
static int computeGlutFlags(const GWindowSettings& settings) {
    int flags = 
        GLUT_RGB |
        GLUT_DOUBLE;
    
    if (settings.stereo) {
        flags |= GLUT_STEREO;
    }

    if (settings.alphaBits > 0) {
        flags |= GLUT_ALPHA;
    }

    if (settings.fsaaSamples > 0) {
        flags |= GLUT_MULTISAMPLE;
    }

    if (settings.stencilBits > 0) {
        flags |= GLUT_STENCIL;
    }

    if (settings.depthBits > 0) {
        flags |= GLUT_DEPTH;
    }

    return flags;
}


static void glutInitDummy() {
    int argc = 1;
    char** argv = (char**)malloc(sizeof(char*));
    argv[0] = "";
    char** argv2 = argv;
    glutInit(&argc, argv2);
    free(argv);
}


GlutWindow::GlutWindow(const GWindowSettings& s) {
    _mouseVisible = true;
    _windowTitle = "G3D";
    settings = s;

    currentGlutWindow = this;

    // Set window size and position
    glutInitWindowSize(settings.width, settings.height);
    glutInitWindowPosition(settings.x, settings.y);   
    glutInitDisplayMode(computeGlutFlags(settings));

    if (! glutInitialized) {
        glutInitDummy();
        glutInitialized = true;
    }

    glutWindowHandle = glutCreateWindow("G3D");

    if (settings.center) {
        glutReshapeWindow(
            (glutGet(GLUT_SCREEN_WIDTH) - settings.width) / 2,
            (glutGet(GLUT_SCREEN_HEIGHT) - settings.height) / 2);            
    }

    if (settings.fullScreen) {
        glutFullScreen();
    }

    // See what settings we actually got
    settings.alphaBits = glutGet(GLUT_WINDOW_ALPHA_SIZE);
    settings.depthBits = glutGet(GLUT_WINDOW_DEPTH_SIZE);

    // No gamma ramp on Glut windows
    settings.lightSaturation = 1.0;
    settings.stencilBits = glutGet(GLUT_WINDOW_STENCIL_SIZE);
    settings.fsaaSamples = glutGet(GLUT_WINDOW_NUM_SAMPLES);

    settings.stereo = (glutGet(GLUT_WINDOW_STEREO) != 0);

    mouseButtons = 0;

    glutReshapeFunc(g_reshape);
    glutKeyboardFunc(g_keyboard);
    glutKeyboardUpFunc(g_keyboardup);
    glutSpecialFunc(g_keyboardspecial);
    glutSpecialUpFunc(g_keyboardspecialup);
    glutMotionFunc(g_mousemotion);
    glutPassiveMotionFunc(g_mousemotion);
    glutMouseFunc(g_mousebtn);
    glutIdleFunc(g_idle);

    // Make sure SDL is initialized
    if (SDL_WasInit(SDL_INIT_EVERYTHING) == 0) {
        // Initialize SDL
        SDL_Init(0);
    }
}


void GlutWindow::runMainLoop(void (*callback)(void)) {
    debugAssert(callback != NULL);
    glutDisplayFunc(callback);
    glutMainLoop();
}


GlutWindow::~GlutWindow() {
    currentGlutWindow = NULL;
    glutDestroyWindow(glutWindowHandle);
}


bool GlutWindow::pollEvent(GEvent& e) {
    if (eventQueue.size() > 0) {
        e = eventQueue.popFront();
        return true;
    } else {
        return false;
    }
}


void GlutWindow::getSettings(GWindowSettings& _settings) const {
    GlutWindow* w = const_cast<GlutWindow*>(this);
    
    w->settings.x = glutGet(GLUT_WINDOW_X);
    w->settings.y = glutGet(GLUT_WINDOW_Y);
    w->settings.width = glutGet(GLUT_WINDOW_WIDTH);
    w->settings.height = glutGet(GLUT_WINDOW_HEIGHT);
    _settings = settings;
}


int GlutWindow::width() const {
    const_cast<GlutWindow*>(this)->settings.width = glutGet(GLUT_WINDOW_WIDTH);
    return settings.width;
}


int GlutWindow::height() const {
    const_cast<GlutWindow*>(this)->settings.height = glutGet(GLUT_WINDOW_HEIGHT);
    return settings.height;
}


Rect2D GlutWindow::dimensions() const {
    GlutWindow* w = const_cast<GlutWindow*>(this);

    w->settings.x = glutGet(GLUT_WINDOW_X);
    w->settings.y = glutGet(GLUT_WINDOW_Y);
    w->settings.width = glutGet(GLUT_WINDOW_WIDTH);
    w->settings.height = glutGet(GLUT_WINDOW_HEIGHT);

    return Rect2D::xywh(settings.x, settings.y, settings.width, settings.height);
}


void GlutWindow::setDimensions(const Rect2D& dims) {
    setPosition(dims.x0(), dims.x1());
    glutReshapeWindow(dims.width(), dims.height());
}


void GlutWindow::setPosition(int x, int y) {
    glutPositionWindow(x, y);
}


bool GlutWindow::hasFocus() const {
    return true;
}


std::string GlutWindow::getAPIVersion() const {
    return format("%d", GLUT_API_VERSION);
}


std::string GlutWindow::getAPIName() const {
    return "glut";
}


void GlutWindow::setGammaRamp(const Array<uint16>& gammaRamp) {
    // Not supported
}


void GlutWindow::setCaption(const std::string& caption) {
    if (caption != _windowTitle) {
        _windowTitle = caption;
        glutSetWindowTitle(caption.c_str());
        glutSetIconTitle(caption.c_str());
    }
}


int GlutWindow::numJoysticks() const {
    return 0;
}


std::string GlutWindow::joystickName(unsigned int sticknum) {
    return "";
}


std::string GlutWindow::caption() {
    return _windowTitle;
}


void GlutWindow::setIcon(const GImage& image) {}


void GlutWindow::swapGLBuffers() {
    glutSwapBuffers();
}


void GlutWindow::notifyResize(int w, int h) {
    settings.width = w;
    settings.height = h;
}


void GlutWindow::setRelativeMousePosition(double x, double y) {
    glutWarpPointer(x, y);
}


void GlutWindow::setRelativeMousePosition(const Vector2& p) {
    setRelativeMousePosition(p.x, p.y);
}


void GlutWindow::getRelativeMouseState(Vector2& position, uint8& b) const {
    position = mouse;
    b = mouseButtons;
}


void GlutWindow::getRelativeMouseState(int& x, int& y, uint8& b) const {
    x = mouse.x;
    y = mouse.y;
    b = mouseButtons;
}


void GlutWindow::getRelativeMouseState(double& x, double& y, uint8& b) const {
    x = mouse.x;
    y = mouse.y;
    b = mouseButtons;
}

void GlutWindow::getJoystickState(unsigned int stickNum, Array<float>& axis, Array<bool>& button) {}


void GlutWindow::setInputCapture(bool c) {
}


bool GlutWindow::inputCapture() const {
    return false;
}


void GlutWindow::setMouseVisible(bool b) {
    if (b != _mouseVisible) {
        _mouseVisible = b;
        if (_mouseVisible) {
            glutSetCursor(GLUT_CURSOR_INHERIT );
        } else {
            glutSetCursor(GLUT_CURSOR_NONE);
        }
    }
}


bool GlutWindow::mouseVisible() const {
    return _mouseVisible;
}

#endif

class App : public GApp {
protected:
    void main();
public:
    SkyRef              sky;

    App(const GAppSettings& settings);
};


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*          app;

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

	virtual void doNetwork();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();

    virtual void cleanup();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));
}


void Demo::cleanup() {
    // Called when Demo::run() exits
}


void Demo::doNetwork() {
	// Poll net messages here
}


void Demo::doSimulation(SimTime dt) {
	// Add physical simulation here
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

	// Add other key handling here
}


void Demo::doGraphics() {

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (! app->sky.isNull()) {
        app->sky->render(lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);

    app->renderDevice->disableLighting();

    if (! app->sky.isNull()) {
        app->sky->renderLensFlare(lighting);
    }
}


Demo* demo;
App* app;

void callback() {
    demo->oneFrame();
}

void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    // Load objects here
    sky = Sky::create(renderDevice, dataDir + "sky/");

    // TODO: remove
    demo = new Demo(this);    
    demo->beginRun();
    window()->runMainLoop(callback);
    demo->endRun();
    
    //Demo(this).run();
}


App::App(const GAppSettings& settings) : GApp(settings, new GlutWindow(settings.window)) {
//App::App(const GAppSettings& settings) : GApp(settings) {
}


int main(int argc, char** argv) {
    GAppSettings settings;

 //   settings.window.fullScreen = true;
    App(settings).run();

    return 0;
}
