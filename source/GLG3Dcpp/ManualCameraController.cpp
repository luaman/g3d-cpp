/**
  @file FPCameraController.cpp

  @maintainer Morgan McGuire, morgan@cs.brown.edu

  @created 2002-07-28
  @edited  2004-02-15
*/

#include "G3D/platform.h"

#include "GLG3D/ManualCameraController.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/UserInput.h"

namespace G3D {

FPCameraController::FPCameraController(){}

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
    center      = Vector2(renderDevice->getWidth() / 2.0, renderDevice->getHeight() / 2.0);
    cameraMouse = center;
    SDL_ShowCursor(SDL_ENABLE);
    guiMouse    = userInput->getMouseXY();
    _active      = false;
    yaw         = -G3D_PI/2;
    pitch       = 0;
	translation = Vector3::ZERO;
    setMoveRate(10);
	setTurnRate(G3D_PI / 4);
}


void FPCameraController::setActive(bool a) {

    if (a != _active) {

        _active = a;

        if (_active) {
            guiMouse = userInput->getMouseXY();
            SDL_ShowCursor(SDL_DISABLE);
            userInput->setMouseXY(cameraMouse);

            #ifndef DEBUG
                // In debug mode, don't grab the cursor because
                // it is annoying when you hit a breakpoint and
                // can't move the mouse.
                SDL_WM_GrabInput(SDL_GRAB_ON);
            #endif

        } else {
            #ifndef DEBUG
                // In debug mode, don't grab the cursor because
                // it is annoying when you hit a breakpoint and
                // can't move the mouse.
                SDL_WM_GrabInput(SDL_GRAB_OFF);
            #endif
            userInput->setMouseXY(guiMouse);
            SDL_ShowCursor(SDL_ENABLE);
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
    pitch = clamp(pitch, -G3D_PI / 2, G3D_PI / 2);

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

