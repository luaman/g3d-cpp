/**
 @file Vector3uint16.cpp
 
 @author Morgan McGuire, matrix@graphics3d.com
  
 @created 2003-04-07
 @edited  2003-04-07
 */

#include "G3D/g3dmath.h"
#include "G3D/Vector3uint16.h"
#include "G3D/Vector3.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"

namespace G3D {

Vector3uint16::Vector3uint16(const class Vector3& v) {
    x = iMin(255, iRound(v.x * 65536));
    y = iMin(255, iRound(v.y * 65536));
    z = iMin(255, iRound(v.z * 65536));
}


Vector3uint16::Vector3uint16(class BinaryInput& bi) {
    deserialize(bi);
}


void Vector3uint16::serialize(class BinaryOutput& bo) const {
    bo.writeUInt16(x);
    bo.writeUInt16(y);
    bo.writeUInt16(z);
}


void Vector3uint16::deserialize(class BinaryInput& bi) {
    x = bi.readUInt16();
    y = bi.readUInt16();
    z = bi.readUInt16();
}


}
