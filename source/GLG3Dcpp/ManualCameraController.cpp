/**
  @file FPCameraController.cpp

  @maintainer Morgan McGuire, morgan@cs.brown.edu

  @created 2002-07-28
  @edited  2004-02-22
*/

#include "G3D/platform.h"

#include "GLG3D/ManualCameraController.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/UserInput.h"

namespace G3D {

FPCameraController::FPCameraController() {}


FPCameraController::FPCameraController(
    RenderDevice* rd, UserInput* ui) {
    init(rd, ui);
}


void FPCameraController::init(class RenderDevice* device, class UserInput* input) {
    renderDevice = device;
    userInput    = input;

    debugAssert(renderDevice);
    debugAssertM(renderDevice->initialized(), "You must call RenderDevice::init before constructing a FPCameraController");

    reset();

}


FPCameraController::~FPCameraController() {
}


bool FPCameraController::active() const {
    return _active;
}


void FPCameraController::reset() {
    center      = Vector2(iRound(renderDevice->getWidth() / 2.0), 
                          iRound(renderDevice->getHeight() / 2.0));

    cameraMouse = center;
    userInput->window()->setMouseVisible(true);
    guiMouse    = userInput->getMouseXY();
    _active      = false;
    yaw         = -G3D_PI/2;
    pitch       = 0;
	translation = Vector3::ZERO;
    setMoveRate(10);
	setTurnRate(G3D_PI / 4);
}


void FPCameraController::grabMouse() {
    // Save the old mouse position for when we deactivate
    guiMouse = userInput->getMouseXY();
    userInput->window()->setMouseVisible(false);

    #ifndef _DEBUG
        // In debug mode, don't grab the cursor because
        // it is annoying when you hit a breakpoint and
        // can't move the mouse.
        userInput->window()->setMouseCapture(true);
    #endif
    cameraMouse = center;
    userInput->setMouseXY(cameraMouse);
}


void FPCameraController::releaseMouse() {
    #ifndef _DEBUG
        // In debug mode, don't grab the cursor because
        // it is annoying when you hit a breakpoint and
        // cannot move the mouse.
        SDL_WM_GrabInput(SDL_GRAB_OFF);
    #endif

    cameraMouse = center;

    // Restore the old mouse position
    userInput->setMouseXY(guiMouse);

    userInput->window()->setMouseVisible(true);
}


void FPCameraController::setActive(bool a) {

    if (a != _active) {

        _active = a;

        if (_active) {
            grabMouse();
        } else {
            releaseMouse();
        }
    }

}


void FPCameraController::setMoveRate(double metersPerSecond) {
    maxMoveRate = metersPerSecond;
}


void FPCameraController::setTurnRate(double radiansPerSecond) {
    maxTurnRate = radiansPerSecond;
}


void FPCameraController::lookAt(
    const Vector3&      position) {

    const Vector3 look = (position - translation);

    yaw   = aTan2(look.x, -look.z);
    pitch = -aTan2(look.y, distance(look.x, look.z));
}


void FPCameraController::doSimulation(
    double              elapsedTime) {

    if (! _active) {
        return;
    }
            
    center      = Vector2(iRound(renderDevice->getWidth() / 2.0), 
                          iRound(renderDevice->getHeight() / 2.0));

    bool focus = userInput->appHasFocus();

    if (focus && ! appHadFocus) {
        // We just gained focus (this is the same as the activate/deactivate code).
        grabMouse();
    } else if (! focus && appHadFocus) {
        releaseMouse();
    }

    // Translation direction
    Vector2 direction(userInput->getX(), userInput->getY());
    direction.unitize();

	// Translate forward
	translation += (getLookVector() * direction.y + 
        getStrafeVector() * direction.x) * elapsedTime * maxMoveRate;
	
    Vector2& oldMouse = cameraMouse;
    Vector2 newMouse;
    
    if (focus) {
        newMouse = userInput->getMouseXY();
    } else {
        // We don't have focus; don't move the mouse
        newMouse = oldMouse;
    }
    
    if ((newMouse.x < 0) || (newMouse.x > 10000)) {
        // Sometimes we get bad values on the first frame;
        // ignore them.
        newMouse = oldMouse;
    }

    Vector2 delta = (newMouse - oldMouse) / 100.0;

    // Reset the mouse periodically.  We can't do this every
    // frame or the mouse won't be able to move substantially
    // at high frame rates.
    if ((newMouse.x < center.x * 0.5) || (newMouse.x > center.x * 1.5) ||
        (newMouse.x < center.y * 0.5) || (newMouse.y > center.y * 1.5)) {
        
        newMouse = center;
        if (userInput->appHasFocus()) {
            userInput->setMouseXY(newMouse);
        }
    }

    // Turn rate limiter
    if (G3D::abs(delta.x) > maxTurnRate) {
        delta.x = maxTurnRate * G3D::sign(delta.x);
    }

    if (G3D::abs(delta.y) > maxTurnRate) {
        delta.y = maxTurnRate * G3D::sign(delta.y);
    }

    yaw   += delta.x;
	pitch += delta.y;

    // Clamp pitch (looking straight up or down)
    pitch = clamp(pitch, -G3D_PI / 2, G3D_PI / 2);

    // Update for the next frame
    oldMouse = newMouse;
    appHadFocus = focus;
}


CoordinateFrame FPCameraController::getCoordinateFrame() const {
	CoordinateFrame c;
	getCoordinateFrame(c);
	return c;
}


void FPCameraController::getCoordinateFrame(CoordinateFrame& c) const {
	c.translation = translation;
    c.rotation = Matrix3::fromEulerAnglesZYX(0, -yaw, -pitch);
}


void FPCameraController::setCoordinateFrame(const CoordinateFrame& c) {
    setPosition(c.translation);
    lookAt(c.translation + c.getLookVector());
}


}

