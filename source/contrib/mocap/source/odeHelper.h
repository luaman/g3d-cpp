/**
 @file odeHelper.h

  Routines that apply ODE functions to G3D types. 
 */
#ifndef ODEHELPER_H
#define ODEHELPER_H

#include <G3DAll.h>
#include <ode/ode.h>

/** Used when creating objects that are not in a collision space. */
#define ODE_NO_SPACE 0

void dGeomSetPositionAndRotation(dGeomID id, const CoordinateFrame& cframe);

void dBodySetPositionAndRotation(dBodyID id, const CoordinateFrame& cframe);

void dBodySetLinearVel(dBodyID id, const Vector3& v);
void dBodySetAngularVel(dBodyID id, const Vector3& v);

void dBodyGetLinearVel(dBodyID id, Vector3& v);
void dBodyGetAngularVel(dBodyID id, Vector3& v);
void dBodyGetLinearAndAngularVel(dBodyID id, Vector3& L, Vector3& A);

void dBodyGetPositionAndRotation(dBodyID id, CoordinateFrame& c);

#endif
