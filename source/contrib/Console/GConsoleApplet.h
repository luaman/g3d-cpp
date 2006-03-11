/**
 @file GConsole/GConsoleApplet.h
 */

#ifndef G3D_GCONSOLEAPPLET_H
#define G3D_GCONSOLEAPPLET_H

#include "GLG3D/GApp.h"
#include "GConsole.h"

namespace G3D {

/** 
 This is an example of how to hook a GConsole up to a GApplet.  You can either subclass this
 sample class or write your own program that calls GConsole's doGraphics and onEvent methods.
 */
class GConsoleApplet : public GApplet {
private:

    GApp*           m_app;

protected:

    GConsole*       m_console;

    virtual void consoleCallback(const std::string& cmd);

    static void _consoleCallback(const std::string& cmd, void* me) {
        static_cast<GConsoleApplet*>(me)->consoleCallback(cmd);
    }

    virtual void printHelp();

public:

    GConsoleApplet(GApp* app);

    virtual ~GConsoleApplet();

    virtual void onInit();

    virtual void onLogic();

    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt);

    virtual void onGraphics(RenderDevice* rd);

    virtual void onUserInput(UserInput* ui);

    virtual void onCleanup();

    virtual bool onEvent(const GEvent &event);
};

} // G3D

#endif
