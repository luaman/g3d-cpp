/**
 @file GApp.h
 
 @maintainer Morgan McGuire, morgan@graphics3d.com

 @created 2003-11-03
 @edited  2003-11-03
 */

#ifndef G3D_GAPP_H
#define G3D_GAPP_H

#include "GLG3D/RenderDevice.h"
#include "GLG3D/CFont.h"

namespace G3D {

class UserInput;
class Camera;
class ManualCameraController;

class GAppSettings {
public:
    RenderDeviceSettings    window;

    /**
     Can be relative to the G3D data direction (e.g. "font/dominant.fnt")
     or relative to the current directory.
     */
    std::string             debugFontName;

    GAppSettings() {
        debugFontName = "console-small.fnt";
    }
};

/**
  The GApp classes are provisional: the interface is likely to change (or they might
  even go away) in the future.  You don't <B>have</B> to use these-- it is fine
  to instantiate RenderDevice and the other classes yourself.

  GApp/GApplet structure:

  You have one GApp subclass that handles any state shared between all pieces of your
  program.
  
  You create one or more GApplet subclasses and instantiate them at the beginning
  of the program (typically, they will be stored in your GApp subclass).  Each corresponds
  to a different major UI state.  For example, the main menu of a game is one GApplet,
  and the actual game screen is another.  If you have a big in-game menu that is modal,
  that can be another GApplet.

  You write code that calls GApplet::run on the current GApplet.  That applet
  releases control by setting GApplet::endApplet = true.  Your master loop
  (typically implemented inside the GApp subclass) then chooses the next GApplet
  to run and invokes run() on it.  If anything sets GApp::endProgram to true,
  the entire program should quit.
 */
class GApp {
private:
    bool                    _debugMode;

    /** Called from init */
    void loadFont(const std::string& fontName);

protected:
    /**
     Called from the constructor
     */
    virtual void init(const GAppSettings& settings);


public:

    std::string             dataDir;
    Log*                    debugLog;
    RenderDevice*           renderDevice;

    /**
     NULL if not loaded
     */
    CFontRef                debugFont;
    UserInput*              userInput;
    bool                    endProgram;

    /**
     A default camera that is driven by the debugController.
     */
    Camera*					debugCamera;

    /**
     When in debugMode
     */
    ManualCameraController* debugController;

    /**
     Strings that have been printed with debugPrint.
     */
    Array<std::string>      debugText;

    /** Returns the state of debugMode.
        All debugX options are only in effect
       when debugMode is also true.  Default is false*/
    bool debugMode() const;

    /**
      Changes the state of debugMode. 
      You must <B>separately</B> activate the debugController 
      if you want events to go to it.
     */
    virtual void setDebugMode(bool b);

    /**
     When true and debugMode is true, debugPrintf prints to the screen.
     (default is true)
     */
    bool                    debugShowText;

    /**
     When true and debugMode is true, an SDL_ESCAPE keydown event
     quits the program.
     (default is true)
     */
    bool                    debugQuitOnEscape;

    /**
     When true and debugMode is true, SDL_TAB keydown deactivates
     the camera and restores the mouse cursor.
     (default is true)
     */
    bool                    debugTabSwitchCamera;

    /**
     When debugMode is true and debugShowRenderingStats is true,
     renderDebugInfo prints the frame rate and other data to the screen.
     */
    bool                    debugShowRenderingStats;

    /**
     When true and the window is resizable, automatically
     responds to SDL_RESIZE events by notifying the
     RenderDevice that the window has been resized and
     resetting the viewport to full screen.
     (default is true)
     */
    bool                    autoResize;

    /**
     If app->debugShowText is true, prints to an on-screen buffer that
     is cleared every frame.
     */
    virtual void debugPrintf(const char* fmt ...);

    /**
     Called from GApplet::run immediately after doGraphics to render
     the debugging text.  Does nothing if debugMode is false.  It
     is not usually necessary to override this method.
     */
    virtual void renderDebugInfo();

    GApp(const GAppSettings& settings = GAppSettings());

    virtual ~GApp();

};


class GApplet {
public:

    GApp*               app;

    /**
     Set to false to break out of the run() loop.
     */
    bool                endApplet;

    /** @param _app This is usually your own subclass of GApp.*/
    GApplet(GApp* _app); 

    /**
     Override this with your simulation code.
     Called from GApp::run.
     
     The default implementation simulated the debugCamera and
     should be called from the subclass's method.

     @param rdt Elapsed real-world time since the last call to doSimulation.
     */
    virtual void doSimulation(RealTime rdt);

    /**
     Override and implement.  If you want the debugCamera to 
     work correctly, you need to explicitly set the projection
     matrix from it.  RenderDevice::beginFrame and endFrame are
     called for you.
     
       For example:
        <PRE>
        void Demo::doGraphics() {

            LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));

            // Cyan background
	        glClearColor(0.1f, 0.5f, 1.0f, 0.0f);

            renderDevice->clear(true, true, true);

            debugCamera->setProjectionAndCameraMatrix();

            // Setup lighting
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT0);
            renderDevice->configureDirectionalLight
              (0, lighting.lightDirection, lighting.lightColor);
            renderDevice->setAmbientLightLevel(lighting.ambient);

            Draw::axes(CoordinateFrame(Vector3(0,0,0)), renderDevice);

            glDisable(GL_LIGHTING);
            glDisable(GL_LIGHT0);
	            
        }
        </PRE>     
     */
    virtual void doGraphics() = 0;

    /**
     Updates the userInput.  Called from run.
     Rarely needs to be overriden by a subclass.

     Instead, override GApp::processEvent to handle your own events.
     */
    virtual void doUserInput();

    /**
     Called from run.
     For a networked app, override this to implement your
     network message polling.
     */
    virtual void doNetwork() {}

    /**
     Update any state you need to here.  This is a good place for
     AI code, for example.  Called after network and user input,
     before simulation.
     */
    virtual void doLogic() {}

    /**
     Override if you need to explicitly handle events.
     Note that the userInput contains a record of all
     keys pressed/held, mouse, and joystick state, so 
     you do not have to override this method to handle
     basic input events.

     The default implementation does nothing.
     */
    virtual void processEvent(const SDL_Event& event) {};

    /**
     Invoked every time run is called.  Default implementation
     does nothing.
     */
    virtual void init() {}

    /**
     Invoked at the end of every run call.  Default implementation
     does nothing.
     */
    virtual void cleanup() {}

    /**
      Run until app->endProgram or endApplet is set to true. 
      The default implementation sets endApplet to false,
      calls init(), then calls the doXXX methods.  
      Invokes cleanup() before exiting.
      It is not usually necessary to override this method.
    */
    virtual void run();
};

}

#endif
