/**
  @file Network_Demo/Entity.cpp

  @author Morgan McGuire, matrix@graphics3d.com
  @created 2004-03-28
  @edited  2004-03-31
 */

#include "Entity.h"
#include "messages.h"
#include "App.h"

extern App* app;

const RealTime Entity::networkLerpTime = 0.2;

Entity::Entity() : id(NO_ID), velocity(Vector3::ZERO), oldFrameTime(-inf()) {
}


void Entity::serialize(BinaryOutput& b) const {
    b.writeInt32(id);
    color.serialize(b);
    b.writeString(modelFilename);
    frame.serialize(b);
    b.writeString(name);
    velocity.serialize(b);
    controls.serialize(b);
}


void Entity::deserialize(BinaryInput& b) {
    id = b.readInt32();
    color.deserialize(b);
    modelFilename = b.readString();
    frame.deserialize(b);
    name = b.readString();
    velocity.deserialize(b);
    controls.deserialize(b);
    oldFrameTime = -inf();
}


void Entity::makeStateMessage(class EntityStateMessage& msg) const {
    msg.id       = id;
    msg.frame    = frame;
    msg.velocity = velocity;
    msg.controls = controls;
}


void Entity::clientUpdateFromStateMessage(class EntityStateMessage& msg, ID localID) {
    debugAssert(id == msg.id);

    oldDeltaFrame = msg.frame - frame;
    oldFrameTime = System::getTick();
    frame        = msg.frame;
    velocity     = msg.velocity;
    if (localID != id) {
        controls = msg.controls;
    }
}


CoordinateFrame Entity::coordinateFrame() const {
    return frame.toCoordinateFrame();
}


CoordinateFrame Entity::smoothCoordinateFrame(RealTime now) const {
    RealTime alpha = clamp(1.0 - (now - oldFrameTime) / networkLerpTime, 0.0, 1.0);

    if (alpha == 0.0) {
        // No lerping necessary; the lerp-period has expired
        return frame.toCoordinateFrame();
    } else {
        EulerFrame lerpFrame = frame - (oldDeltaFrame * alpha);
        return lerpFrame.toCoordinateFrame();
    }
}


void Entity::doSimulation(SimTime dt) {
    // The simulation and control scheme for the demo is ridiculous-- it is
    // just for demonstration purposes and needs to be replaced for any
    // reasonable simulator.

    CoordinateFrame cframe = coordinateFrame();

    // MSVC 6.0 miscompiles this code in release mode if clampVel is computed
    // before clampYaw.
    const double clampVel = clamp(velocity.length() * 0.1, -2.0, 2.0);
    const double clampYaw = clamp(controls.yaw, -1.0, 1.0);
    const double yawVel   = clampYaw * toRadians(45) * clampVel * 
                            -sign(velocity.dot(cframe.rotation.getColumn(2)));
    const double dYaw     = yawVel * dt;

    frame.yaw += dYaw;

    // Slowly roll to an angle based on the change in yaw
    double desiredRoll = yawVel / 3.0;
    double rollVel = sign(desiredRoll - frame.roll) *
        clamp(abs(desiredRoll - frame.roll) * 10, -toRadians(180), toRadians(180)); 

    frame.roll += rollVel * dt;

    // Rotate velocity
    velocity = Matrix3::fromAxisAngle(Vector3::UNIT_Y, dYaw) * velocity;

    cframe = coordinateFrame();

    // Engine thrust
    const double  thrustMag = clamp(controls.throttle * 15.0, -15.0, 15.0);
    const Vector3 thrustDir = -cframe.rotation.getColumn(2);

    // Drag: at low speeds, ~speed.  At high speed, ~speed^2
    double speed = velocity.length();
    double  dragMag;
    if (speed > 7) {
        dragMag = 0.01 * speed * speed;
    } else if (speed > 1) {
        dragMag = speed * 1.3;
    } else {
        dragMag = speed;
    }

    const Vector3 dragDir = -velocity.directionOrZero();

    const Vector3 lift = Vector3(0, clamp(controls.pitch * 10.0, -10.0, 10.0), 0);

    // Slowly roll to an angle based on the change in elevation
    double desiredPitch = lift.y / 10.0;
    double pitchVel = sign(desiredPitch - frame.pitch) *
        clamp(abs(desiredPitch - frame.pitch) * 3, -toRadians(45), toRadians(45)); 

    frame.pitch += pitchVel * dt;


    const Vector3 acceleration =
        (thrustMag * thrustDir) +
        (dragMag * dragDir) +
        lift;

    velocity += acceleration * dt;

    frame.translation += velocity * dt;

}


////////////////////////////////////////////////////////////////////////

void simulateEntities(EntityTable& entityTable, SimTime dt) {
    EntityTable::Iterator end = entityTable.end();
    for (EntityTable::Iterator e = entityTable.begin(); e != end; ++e) {
        e->value.doSimulation(dt);
    }
}

////////////////////////////////////////////////////////////////////////

EulerFrame::EulerFrame() : roll(0), yaw(0), pitch(0), translation(Vector3::ZERO) {
}


EulerFrame::EulerFrame(double r, double y, double p, const Vector3& t) :
    roll(r), yaw(y), pitch(p), translation(t) {
}


void EulerFrame::serialize(BinaryOutput& b) const {
    b.writeFloat32(roll);
    b.writeFloat32(yaw);
    b.writeFloat32(pitch);
    translation.serialize(b);
}


void EulerFrame::deserialize(BinaryInput& b) {
    roll = b.readFloat32();
    yaw  = b.readFloat32();
    pitch = b.readFloat32();
    translation.deserialize(b);
}


EulerFrame EulerFrame::lerp(const EulerFrame& other, double alpha) const {
    return EulerFrame(
        G3D::lerp(roll, other.roll, alpha),
        G3D::lerp(yaw, other.yaw, alpha),
        G3D::lerp(pitch, other.pitch, alpha),
        translation.lerp(other.translation, alpha));
}


CoordinateFrame EulerFrame::toCoordinateFrame() const {
    return CoordinateFrame(
        Matrix3::fromEulerAnglesYXZ(yaw, pitch, roll), translation);
}


EulerFrame EulerFrame::operator-(const EulerFrame& other) const {
    return EulerFrame(roll - other.roll, yaw - other.yaw,
        pitch - other.pitch, translation - other.translation);
}


EulerFrame EulerFrame::operator+(const EulerFrame& other) const {
    return EulerFrame(roll + other.roll, yaw + other.yaw,
        pitch + other.pitch, translation + other.translation);
}


EulerFrame EulerFrame::operator*(double s) const {
    return EulerFrame(roll * s, yaw * s, pitch * s, translation * s);
}


////////////////////////////////////////////////////////////////////////

Controls::Controls() : throttle(0), yaw(0), pitch(0) {
}


void Controls::serialize(BinaryOutput& b) const {
    b.writeFloat32(throttle);
    b.writeFloat32(yaw);
    b.writeFloat32(pitch);
}


void Controls::deserialize(BinaryInput& b) {
    throttle = b.readFloat32();
    yaw = b.readFloat32();
    pitch = b.readFloat32();
}
