/**
  @file ManualCameraController.h

  @maintainer Morgan McGuire, morgan@cs.brown.edu

  @created 2002-07-28
  @edited  2006-02-28
*/

#ifndef G3D_FPCAMERACONTROLLER_H
#define G3D_FPCAMERACONTROLLER_H

#include "G3D/platform.h"
#include "G3D/Vector3.h"
#include "G3D/CoordinateFrame.h"

namespace G3D {

class CoordinateFrame;

/**
 Uses a First Person (Quake-style) mapping to translate keyboard and mouse input
 into a flying camera position.  The result is an Euler-angle 
 camera controller suitable for games and fly-throughs.  

  To use without G3D::GApp:
  <OL>
    <LI> Create a G3D::RenderDevice
    <LI> Create a UserInput object (set the keyboard controls when creating it)
    <LI> Create a ManualCameraController
    <LI> Call ManualCameraController::setActive(true)
    <LI> Invoke ManualCameraController::doSimulation every time simulation is invoked (e.g. once per rendering iteration)
    <LI> Use ManualCameraController::getCoordinateFrame() to set the camera's position
  </OL>

 */
class FPCameraController {
	
	/** m/s */
	double                      maxMoveRate;

	/** rad/s */
	double                      maxTurnRate;

	double                      yaw;
    double                      pitch;
	Vector3                     translation;

    class RenderDevice*         renderDevice;

    bool                        _active;

    class UserInput*            userInput;

public:

	FPCameraController();

    /** Creates and initializes */
	FPCameraController(class RenderDevice*, class UserInput*);
    
    /** You need to call setActive(true) before the controller will work. */
    void init(class RenderDevice* device, class UserInput* input);

    /** Deactivates the controller */
    virtual ~FPCameraController();

    /** When active, the FPCameraController takes over the mouse.  It turns
        off the mouse cursor and switches to first person controller style.
        Use this to toggle between your menu system and first person camera control.

        When deactivated, the mouse cursor is restored and the mouse is located
        where it was when the camera controller was activated.

        In release mode, the cursor movement is restricted to the window
        while the controller is active.  This does not occur in debug mode because
        you might hit a breakpoint while the controller is active and it
        would be annoying to not be able to move the mouse.*/
    void setActive(bool a);

    bool active() const;

    /** Initial value is 10 */
    void setMoveRate(double metersPerSecond);

    /** Initial value is PI / 2 */
    void setTurnRate(double radiansPerSecond);

    /** Invoke immediately before entering the main game loop. */
    void reset();

	/**
	 Increments the ManualCameraController's orientation and position.
     Invoke once per simulation step.
	 */
	void doSimulation(
        double                  elapsedTime);

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

    /**
      Sets to the closest legal controller orientation to the coordinate frame.
    */
    void setCoordinateFrame(const CoordinateFrame& c);
};

/** Use FPCameraController instead 
    @deprecated */
typedef FPCameraController ManualCameraController;

}
#endif
