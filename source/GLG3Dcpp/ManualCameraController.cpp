/**
  @file ManualCameraController.cpp

  @maintainer Morgan McGuire, morgan@cs.brown.edu

  @created 2002-07-28
  @edited  2003-04-07
*/

#include "G3D/platform.h"
#if defined(G3D_OSX)
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "GLG3D/ManualCameraController.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/UserInput.h"

namespace G3D {

ManualCameraController::ManualCameraController(RenderDevice* device) :
    renderDevice(device) {

    debugAssert(renderDevice);
    debugAssertM(renderDevice->initialized(), "You must call RenderDevice::init before constructing a ManualCameraController");

    mCenterX    = renderDevice->getWidth() / 2;
    mCenterY    = renderDevice->getHeight() / 2;

    reset();
}


void ManualCameraController::reset() {
    yaw         = -G3D::PI/2;
    pitch       = 0;
	translation = Vector3::ZERO;
    setMoveRate(10);
	setTurnRate(G3D::PI / 4);
    oldMouseX   = mCenterX;
    oldMouseY   = mCenterY;
    SDL_WarpMouse(mCenterX, mCenterY);
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
    double              elapsedTime, 
    UserInput&          userInput) {

    Vector2 direction(userInput.getX(), userInput.getY());
    direction.unitize();

	// Translate forward
	translation += (getLookVector() * direction.y + getStrafeVector() * direction.x) * elapsedTime * maxMoveRate;
	
    double mouseX = userInput.getMouseX();
    double mouseY = userInput.getMouseY();
    
    if ((mouseX < 0) || (mouseX > 10000)) {
        // Sometimes we get bad values on the first frame
        mouseX = mCenterX;
        mouseY = mCenterY;
    }

	double dx = (mouseX - oldMouseX) / 100.0;
	double dy = (mouseY - oldMouseY) / 100.0;

    // Reset the mouse periodically.  We can't do this every
    // frame or the mouse won't be able to move substantially
    // at high frame rates.
    if ((mouseX < mCenterX / 2) || (mouseX > mCenterX * 1.5) ||
        (mouseY < mCenterY / 2) || (mouseY > mCenterY * 1.5)) {
        oldMouseX = mCenterX;
        oldMouseY = mCenterY;
        userInput.setMouseXY(mCenterX, mCenterY);
    } else {
        oldMouseX = mouseX;
        oldMouseY = mouseY;
    }

    // Turn rate limiter
    if (G3D::abs(dx) > maxTurnRate) {
        dx = maxTurnRate * G3D::sign(dx);
    }

    if (G3D::abs(dy) > maxTurnRate) {
        dy = maxTurnRate * G3D::sign(dy);
    }

    yaw   += dx;
	pitch += dy;

    // Clamp pitch (looking straight up or down)
	if (pitch < -G3D::PI / 2) {
		pitch = -G3D::PI / 2;
    } else if (pitch > G3D::PI / 2) {
		pitch = G3D::PI / 2;
	} 
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

