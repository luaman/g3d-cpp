/**
  @file Network_Demo/Entity.h

  @author Morgan McGuire, matrix@graphics3d.com
  @created 2004-03-29
  @edited  2004-03-29
 */

#ifndef ENTITY_H
#define ENTITY_H

#include <G3DAll.h>

typedef int ID;
enum {NO_ID = 0};

/**
 Entity controls.
 */
class Controls {
public:
    double              throttle;

    /** Effective rudder setting relative to heading. */
    double              yaw;

    /** Effective elevator setting relative to heading. */
    double              pitch;

    /** Initializes all values to zero */
    Controls();

    inline bool operator==(const Controls& other) const {
        return 
            (throttle == other.throttle) &&
            (yaw == other.yaw) &&
            (pitch == other.pitch);
    }

    inline bool operator!=(const Controls& other) const {
        return ! (*this == other);
    }

    void serialize(BinaryOutput&) const;

    void deserialize(BinaryInput&);
};


class EulerFrame {
public:

    double              yaw;
    double              roll;
    double              pitch;
    Vector3             translation;

    EulerFrame();

    EulerFrame(double r, double y, double p, const Vector3& t);

    EulerFrame operator-(const EulerFrame& other) const;

    EulerFrame operator+(const EulerFrame& other) const;

    EulerFrame operator*(double s) const;

    CoordinateFrame toCoordinateFrame() const;

    EulerFrame lerp(const EulerFrame& other, double alpha) const;

    void serialize(BinaryOutput&) const;

    void deserialize(BinaryInput&);
};


/**
 One of the objects in the world.
 */
class Entity  {
public:
    /**
      Time period over which positions are lerped between
      the client value and the server value when there
      is a discrepancy.
      */
    static const RealTime     networkLerpTime;

    ID                  id;

    Color3              color;

    std::string         modelFilename;

    std::string         name;

    EulerFrame          frame;

    /** The difference of the new and old values due to network
        synchronization.  Used in smoothCoordinateFrame.
        On the  client side, this is used to prevent jerky motion
        due to discrepancies from server information... we 
        interpolate between old and new positions over a short
        period of time to hide the jump (although simulation
        proceeds with the new frame immediately).  We store the
        difference between old and new frames so that we aren't
        interpolating from a static frame.

        Not used on the server side.  Not sent when the Entity
        is serialized. */
    EulerFrame          oldDeltaFrame;

    /** System::getTick() time at which oldFrame was frozen */
    RealTime            oldFrameTime;

    Vector3             velocity;

    Controls            controls;

    Entity();

    /** Up-to-date coordinate frame */
    CoordinateFrame coordinateFrame() const;

    /** Coordinate frame interpolated between frame and oldFrame.
        @param now System::getTick*/
    CoordinateFrame smoothCoordinateFrame(RealTime now = System::getTick()) const;

    /** Makes an update message that can be sent between client and server or vice versa. */
    void makeStateMessage(class EntityStateMessage& msg) const;

    /** Update this entity's state from the message on the client side.
        (The server side only updates the controls, not the state.)
        If localID == id, the controls are not updated because this
        entity is being controlled locally. */
    void clientUpdateFromStateMessage(class EntityStateMessage& msg, ID localID);

    void doSimulation(SimTime dt);

    /** Called from CreateEntityMessage::serialize */
    virtual void serialize(BinaryOutput&) const;

    virtual void deserialize(BinaryInput&);
};


typedef Table<ID, Entity> EntityTable;

void simulateEntities(EntityTable& entityTable, SimTime dt);

#endif
