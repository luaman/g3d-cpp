#include "odeHelper.h"

void dGeomSetPositionAndRotation(dGeomID id, const CoordinateFrame& cframe) {
    const Matrix3& g3dR = cframe.rotation;
    dMatrix3 odeR;
         
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            odeR[i * 4 + j] = g3dR[i][j];
        }
        odeR[i * 4 + 3] = 0.0;
    }
    
    const Vector3& c = cframe.translation;

    dGeomSetPosition(id, c.x, c.y, c.z);
    dGeomSetRotation(id, odeR);
}


void dBodyGetPositionAndRotation(dBodyID id, CoordinateFrame& c) {

    // dReal may be either single or double
    const dReal* t = dBodyGetPosition(id);
    const dReal* r = dBodyGetRotation(id);    

    for (int i = 0; i < 3; ++i) {
        c.translation[i] = t[i];
        for (int j = 0; j < 3; ++j) {
            c.rotation[i][j] = r[i * 4 + j];
        }
    }
}
