/**
  @file ManualCameraController.h

  @maintainer Morgan McGuire, morgan@cs.brown.edu

  @created 2002-07-28
  @edited  2002-12-07
*/

#ifndef G3D_MANUALCAMERACONTROLLER_H
#define G3D_MANUALCAMERACONTROLLER_H

#include "graphics3D.h"

namespace G3D {

/**
 Uses a Quake-style mapping to translate keyboard and mouse input
 into a flying camera position.  The result is an Euler-angle ManualCameraController
 suitable for games.  To use:

  <OL>
    <LI> Create a G3D::RenderDevice
    <LI> Create a UserInput object (set the keyboard controls when creating it)
    <LI> Create a ManualCameraController
    <LI> Invoke ManualCameraController::reset immediately before the rendering loop
    <LI> Invoke ManualCameraController::doSimulation every time simulation is invoked (e.g. once per rendering iteration)
    <LI> Use ManualCameraController::getCoordinateFrame() to set the camera's position
  </OL>
 */
class ManualCameraController {
	
	/** m/s */
	double                      maxMoveRate;

	/** rad/s */
	double                      maxTurnRate;

	double                      yaw;
    double                      pitch;
	Vector3                     translation;

    /** Used for tracking relative mouse movement.
        These are in pixels. */
    double                      oldMouseX;
    double                      oldMouseY;

    class RenderDevice*         renderDevice;

    /** Mouse center in pixels */
    int                         mCenterX;
    int                         mCenterY;

public:

    /** @param device The window that will lock the mouse. */
	ManualCameraController(class RenderDevice* device);

    /** Initial value is 10 */
    void setMoveRate(double metersPerSecond);

    /** Initial value is PI / 2 */
    void setTurnRate(double radiansPerSecond);

    virtual ~ManualCameraController() {}

    /** Invoke immediately before entering the main game loop. */
    void reset();

	/**
	 Increments the ManualCameraController's orientation and position.
     Invoke once per simulation step.
	 */
	void doSimulation(
        double                  elapsedTime,
        class UserInput&        userInput);

	void setPosition(const Vector3& t) {
		translation = t;
	}

    void lookAt(const Vector3& position);

    double getYaw() const {
        return yaw;
    }

    double getPitch() const {
        return pitch;
    }

	const Vector3& getPosition() const {
		return translation;
	}

	Vector3 getLookVector() const {
		return getCoordinateFrame().getLookVector();
	}

    /** Right vector */
	Vector3 getStrafeVector() const {
		return getCoordinateFrame().getRightVector();
	}

	CoordinateFrame getCoordinateFrame() const;

	void getCoordinateFrame(CoordinateFrame& c) const;
};

}
#endif
