/**
 @file Vector3int16.cpp
 
 @author Morgan McGuire, matrix@graphics3d.com
  
 @created 2003-04-07
 @edited  2003-04-08
 */

#include "G3D/g3dmath.h"
#include "G3D/Vector3int16.h"
#include "G3D/Vector3.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"

namespace G3D {

Vector3int16::Vector3uint16(const class Vector3& v) {
    x = iRound(v.x * 32767);
    y = iRound(v.y * 32767);
    z = iRound(v.z * 32767);
}


Vector3int16::Vector3int16(class BinaryInput& bi) {
    deserialize(bi);
}


void Vector3int16::serialize(class BinaryOutput& bo) const {
    bo.writeInt16(x);
    bo.writeInt16(y);
    bo.writeInt16(z);
}


void Vector3int16::deserialize(class BinaryInput& bi) {
    x = bi.readInt16();
    y = bi.readInt16();
    z = bi.readInt16();
}


}
