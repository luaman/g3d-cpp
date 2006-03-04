/**
 @file GConsole/GConsoleApplet.cpp
 */

#include "GConsoleApplet.h"
#include "GLG3D/UserInput.h"
#include "GLG3D/RenderDevice.h"

namespace G3D {

void GConsoleApplet::consoleCallback(const std::string& cmd) {
    // Extremely simple interpreter.  You could easily connect to a scripting language
    // like lua or Python, or write your own language here.  See G3D::TextInput
    // for an easy way of parsing this string into useful tokens.

    if (cmd == "exit") {
        endApplet = true;
        m_app->endProgram = true;
        return;
    }

    if (cmd == "help") {
        printHelp();
    }
}

void GConsoleApplet::printHelp() {
    m_console->printf(" -----------------------------------------\n");
    m_console->printf(" PAGEUP/PAGEDOWN    Scroll console buffer\n");
    m_console->printf(" LEFT/RIGHT         Move cursor\n");
    m_console->printf(" ESCAPE             Close console\n");
    m_console->printf(" ~                  Open console\n");
    m_console->printf(" ENTER              Execute command\n");
    m_console->printf(" exit               Quit program\n");
    m_console->printf(" help               Print this information\n");
    m_console->printf(" -----------------------------------------\n");
}

GConsoleApplet::GConsoleApplet(GApp* _app) : GApplet(_app), m_app(_app) {
    m_console = new GConsole(m_app->debugFont, _consoleCallback, this);
    printHelp();
}


GConsoleApplet::~GConsoleApplet() {
    delete m_console;
    m_console = NULL;
}


void GConsoleApplet::onInit()  {
    GApplet::onInit();
}


void GConsoleApplet::onCleanup() {
    GApplet::onCleanup();
}


void GConsoleApplet::onLogic() {
    GApplet::onLogic();
}



void GConsoleApplet::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
	// Add physical simulation here.  You can make your time advancement
    // based on any of the three arguments.
}


void GConsoleApplet::onUserInput(UserInput* ui) {
    if (ui->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        m_app->endProgram = true;
    }

	// Add other key handling here
}


void GConsoleApplet::onGraphics(RenderDevice* rd) {

    m_console->onGraphics(rd);

}

bool GConsoleApplet::onEvent(const GEvent &event) {
    if (m_console->onEvent(event)) {
        return true;
    } else {
        return GApplet::onEvent(event);
    }

}

} // G3D
