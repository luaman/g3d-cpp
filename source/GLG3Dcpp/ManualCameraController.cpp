/**
  @file ManualCameraController.cpp

  @maintainer Morgan McGuire, morgan@cs.brown.edu

  @created 2002-07-28
  @edited  2003-09-27
*/

#include "G3D/platform.h"

#include "GLG3D/ManualCameraController.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/UserInput.h"

namespace G3D {

ManualCameraController::ManualCameraController(RenderDevice* device, UserInput* input) :
    renderDevice(device), userInput(input) {

    debugAssert(renderDevice);
    debugAssertM(renderDevice->initialized(), "You must call RenderDevice::init before constructing a ManualCameraController");

    reset();
}


ManualCameraController::~ManualCameraController() {
    setActive(false);
}


void ManualCameraController::reset() {
    center      = Vector2(renderDevice->getWidth() / 2.0, renderDevice->getHeight() / 2.0);
    cameraMouse = center;
    SDL_ShowCursor(SDL_ENABLE);
    guiMouse    = userInput->getMouseXY();
    active      = false;
    yaw         = -G3D::PI/2;
    pitch       = 0;
	translation = Vector3::ZERO;
    setMoveRate(10);
	setTurnRate(G3D::PI / 4);
}


void ManualCameraController::setActive(bool a) {

    if (a != active) {

        active = a;

        if (active) {
            guiMouse = userInput->getMouseXY();
            SDL_ShowCursor(SDL_DISABLE);
            userInput->setMouseXY(cameraMouse);
        } else {
            userInput->setMouseXY(guiMouse);
            SDL_ShowCursor(SDL_ENABLE);
        }
    }

}


void ManualCameraController::setMoveRate(double metersPerSecond) {
    maxMoveRate = metersPerSecond;
}


void ManualCameraController::setTurnRate(double radiansPerSecond) {
    maxTurnRate = radiansPerSecond;
}


void ManualCameraController::lookAt(
    const Vector3&      position) {

    const Vector3 look = (position - translation);

    yaw   = aTan2(look.x, -look.z);
    pitch = -aTan2(look.y, distance(look.x, look.z));
}


void ManualCameraController::doSimulation(
    double              elapsedTime) {

    if (! active) {
        return;
    }
            
    bool focus = userInput->appHasFocus();

    if (focus && ! appHadFocus) {
        // We just gained focus.
        guiMouse = userInput->getMouseXY();
        
        // Restore our mouse position
        userInput->setMouseXY(cameraMouse);
    }

    Vector2 direction(userInput->getX(), userInput->getY());
    direction.unitize();

	// Translate forward
	translation += (getLookVector() * direction.y + getStrafeVector() * direction.x) * elapsedTime * maxMoveRate;
	
    Vector2 mouse;
    
    if (focus) {
        mouse = userInput->getMouseXY();
    } else {
        mouse = cameraMouse;
    }
    
    if ((mouse.x < 0) || (mouse.x > 10000)) {
        // Sometimes we get bad values on the first frame
        mouse = center;
    }

    Vector2 delta = (mouse - cameraMouse) / 100.0;

    // Reset the mouse periodically.  We can't do this every
    // frame or the mouse won't be able to move substantially
    // at high frame rates.
    if ((mouse.x < center.x / 2) || (mouse.x > center.x * 1.5) ||
        (mouse.x < center.y / 2) || (mouse.y > center.y * 1.5)) {
        cameraMouse = center;
        if (userInput->appHasFocus()) {
            userInput->setMouseXY(cameraMouse);
        }
    } else {
        cameraMouse = mouse;
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
	if (pitch < -G3D::PI / 2) {
		pitch = -G3D::PI / 2;
    } else if (pitch > G3D::PI / 2) {
		pitch = G3D::PI / 2;
	}

    appHadFocus = focus;
}


CoordinateFrame ManualCameraController::getCoordinateFrame() const {
	CoordinateFrame c;
	getCoordinateFrame(c);
	return c;
}


void ManualCameraController::getCoordinateFrame(CoordinateFrame& c) const {
	c.translation = translation;
	c.rotation.fromEulerAnglesZYX(0, -yaw, -pitch);
}

}

