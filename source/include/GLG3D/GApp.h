/**
 @file GApp.h
 
 @maintainer Morgan McGuire, morgan@graphics3d.com

 @created 2003-11-03
 @edited  2004-04-27
 */

#ifndef G3D_GAPP_H
#define G3D_GAPP_H

#include "GLG3D/GFont.h"
#include "G3D/GCamera.h"
#include "GLG3D/ManualCameraController.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/TextureManager.h"

namespace G3D {

class RenderDevice;
class UserInput;

class GAppSettings {
public:
    RenderDeviceSettings    window;

    /**
     If "<AUTO>", G3D will search for the standard
     data files.  It is recommended that you override this
     default and set dataDir to a directory relative
     to your executable (e.g. "./data/")
     so your programs can be distributed to users who
     do not have full the G3D data directory.
     */
    std::string             dataDir;

    /**
     Can be relative to the G3D data directory (e.g. "font/dominant.fnt")
     or relative to the current directory.
     Default is "console-small.fnt"
     */
    std::string             debugFontName;

    std::string             logFilename;

    /** 
      When true, GAapp ensures that g3d-license.txt exists in the current
      directory.  That file is written from the return value of G3D::license() */
    bool                    writeLicenseFile;

    /** When true, the networkDevice is initialized.  Defaults to true. */
    bool                    useNetwork;

    GAppSettings() : dataDir("<AUTO>"), debugFontName("console-small.fnt"), 
        logFilename("log.txt"), writeLicenseFile(true), useNetwork(true) {
    }
};

/**
  See @link guideapp @endlink for the philosophy of GApp and GApplet. 
 */
class GApp {
private:
    bool                    _debugMode;

    /**
     Tracks whether the debug controller (activated with TAB) was
     active when last we were in debug mode.
     */
    bool                    _debugControllerWasActive;

    /** Called from init. */
    void loadFont(const std::string& fontName);

    GWindow*                _window;

protected:

    /**
     Called from run.  This is invoked inside of several
     exception handlers so that any G3D uncaught exceptions
     can be logged instead of crashing the application.
     */
    virtual void main() = 0;

public:

    /** Initialized to GAppSettings::dataDir, or if that is "<AUTO>", 
        to System::demoFindData(). To make your program
        distributable, override the default 
        and copy all data files you need to a local directory.
        Recommended setting is "data/" or "./", depending on where
        you put your data relative to the executable.

        Your data directory must contain the default debugging font, 
        "console-small.fnt", unless you change it.
    */
    std::string             dataDir;
    Log*                    debugLog;
    RenderDevice*           renderDevice;
    NetworkDevice*          networkDevice;

    /**
     NULL if not loaded
     */
    CFontRef                debugFont;
    UserInput*              userInput;
    bool                    endProgram;

    /**
     A global texture manager.
     */
    TextureManager          textureManager;

    /**
     A default camera that is driven by the debugController.
     */
    GCamera					debugCamera;

    /**
     When in debugMode
     */
    ManualCameraController  debugController;

    /**
     Strings that have been printed with debugPrint.
     */
    Array<std::string>      debugText;

    inline GWindow* window() const {
        return _window;
    }

    /** Returns the state of debugMode.
        All debugX options are only in effect
        when debugMode is also true.  Default is false*/
    bool debugMode() const;

    /**
      Changes the state of debugMode. 
      You must <B>separately</B> activate the debugController 
      if you want events to go to it (by default, the TAB key
      activates it).
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
      When true, there is an assertion failure if an exception is 
      thrown during GApp::main().

      Default is true.
      */
    bool                    catchCommonExceptions;

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

    /**
     @param window If null, a SDLWindow will be created for you. This
         argument is useful for substituting a different window
         system (e.g. GlutWindow)
     */
    GApp(const GAppSettings& settings = GAppSettings(), GWindow* window = NULL);

    virtual ~GApp();

    /**
     Call this to run the app.  Subclasses should override main(), not run.
     */
    void run();

};

/**

 To invoke a GApplet and let it control the main loop, call
 run().  To control the main loop explicitly, invoke beginRun on
 initialization, call oneFrame() from the main loop, and call endRun on cleanup.
 */ 
class GApplet {
private:
    GApp*               app;

    RealTime            now, lastTime;

public:

    /** @param _app This is usually your own subclass of GApp.*/
    GApplet(GApp* _app);

    /**
      Run until app->endProgram or endApplet is set to true. 
      Calls beginRun(), then oneFrame in a loop, then endRun().
      
      For use with GWindows that do not require a main loop.
    */
    void run();

    /**
     Prepare for running.
      The default implementation sets endApplet to false,
      calls init(), copies the debug camera position to the debug camera controller,
    */
    void beginRun();

    /** 
      A single frame of rendering, simulation, AI, events, networking, etc. 
      Invokes the doXXX methods.  For use with GWindows that require a main loop.
      This if your GWindow does not require control of the main loop, GApp will
      call this for you.
    */
    void oneFrame();

    /**
      Invokes cleanup().
    */
    void endRun();

protected:

    /**
     Set to false to break out of the run() loop.
     */
    bool                endApplet;

    /**
     Override this with your simulation code.
     Called from GApp::run.
        
     Default implementation does nothing.

     @param rdt Elapsed real-world time since the last call to doSimulation.
     */
    virtual void doSimulation(RealTime rdt) {};

    /**
     Override and implement.  The debugCamera's projection and object to world
     matrices are set by default; you can set other cameras as desired. 
     RenderDevice::beginFrame and endFrame are called for you.
     
	 See <A HREF="../demos/main.cpp">demos/main.cpp</A> for an example of
	 overriding lights.
      
     */
    virtual void doGraphics() = 0;

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
     Override if you need to explicitly handle events.
     Note that the userInput contains a record of all
     keys pressed/held, mouse, and joystick state, so 
     you do not have to override this method to handle
     basic input events.

     The default implementation does nothing.
     */
    virtual void processEvent(const SDL_Event& event) {};

    /**
     Updates the userInput.  Called from run.  Rarely needs to be
     called by user programs.

     Never overriden by a subclass.
     Instead, override GApp::processEvent to handle your own events.
     */
    void doUserInput();
};

}

#endif
