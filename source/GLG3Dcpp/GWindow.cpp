/**
 @file GWindow.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2004-11-16
 @edited  2004-11-16
 */

#include "GLG3D/GWindow.h"
#include "GLG3D/GApp.h"

namespace G3D {

void GWindow::executeLoopBody() {
    if (notDone()) {
        if (loopBodyStack.last().isGApplet) {
            loopBodyStack.last().applet->oneFrame();
        } else {                
            loopBodyStack.last().func(loopBodyStack.last().arg);
        }
    }
}


void GWindow::popLoopBody() {
    if (loopBodyStack.last().isGApplet) {
        loopBodyStack.last().applet->endRun();
        loopBodyStack.pop();
    }
}

}