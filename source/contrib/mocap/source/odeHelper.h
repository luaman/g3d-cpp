#ifndef ODEHELPER_H
#define ODEHELPER_H

#include <G3DAll.h>
#include <ode/ode.h>

void dGeomSetPositionAndRotation(dGeomID id, const CoordinateFrame& cframe);

void dBodyGetPositionAndRotation(dBodyID id, CoordinateFrame& c);

#endif
