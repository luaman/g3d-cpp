/**
 @file PhysicsFrame.cpp

 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2002-07-09
 @edited  2003-02-15
*/

#include "G3D/PhysicsFrame.h"

namespace G3D {

PhysicsFrame::PhysicsFrame() {
    translation = Vector3::ZERO;
    rotation    = Quat();
}


PhysicsFrame::PhysicsFrame(
    const CoordinateFrame& coordinateFrame) {

    translation = coordinateFrame.translation;
    rotation    = Quat(coordinateFrame.rotation);
}


CoordinateFrame PhysicsFrame::toCoordinateFrame() const {
    CoordinateFrame f;
    
    f.translation = translation;
    f.rotation    = rotation.toRotationMatrix();

    return f;
}


PhysicsFrame PhysicsFrame::lerp(
    double                  alpha,
    const PhysicsFrame&     other) {

    PhysicsFrame result;

    result.translation = translation.lerp(alpha, other.translation);
    result.rotation    = rotation.lerp(alpha, other.rotation);

    return result;
}


PhysicsFrame PhysicsFrame::integrate(
    double                  t,
    const PhysicsFrame&     dx) {

    PhysicsFrame result;

    result.translation = translation + t * dx.translation;
    result.rotation    = rotation * dx.rotation.pow(t); 

    return result;
}


PhysicsFrame PhysicsFrame::integrate(
    double                  t,
    const PhysicsFrame&     dx,
    const PhysicsFrame&     ddx) {

    PhysicsFrame result;

    // TODO: is this correct?
    result.translation = translation + t * dx.translation + t * t * ddx.translation;
    result.rotation    = rotation * dx.rotation.pow(t) * ddx.rotation.pow(t * t);

    return result;
}

}; // namespace

