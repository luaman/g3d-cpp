/**
  @file FPCameraController.cpp

  @maintainer Morgan McGuire, morgan@cs.brown.edu

  @created 2002-07-28
  @edited  2004-02-28
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
    _active      = false;
    yaw         = -G3D_PI/2;
    pitch       = 0;
	translation = Vector3::ZERO;
    setMoveRate(10);
	setTurnRate(G3D_PI / 4);
}


void FPCameraController::setActive(bool a) {
    _active = a;
    userInput->setPureDeltaMouse(a);
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
           
    // Translation direction
    Vector2 direction(userInput->getX(), userInput->getY());
    direction.unitize();

	// Translate forward
	translation += (getLookVector() * direction.y + 
        getStrafeVector() * direction.x) * elapsedTime * maxMoveRate;
    
    Vector2 delta = userInput->mouseDXY() / 100.0;

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

