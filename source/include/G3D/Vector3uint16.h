/**
  @file Vector3uint16.cpp
  
  @maintainer Morgan McGuire, matrix@brown.edu

  @created 2003-04-07
  @edited  2003-04-07
 */

#ifndef VECTOR3UINT16_H
#define VECTOR3UINT16_H

#include "G3D/g3dmath.h"

namespace G3D {

/**
 A Vector3 that packs its fields into uint16s.
 */
// Switch to tight alignment
#pragma pack(push, Vector3uint16_align)
#pragma pack(2)
class Vector3uint16 {
public:
    uint16              x;
    uint16              y;
    uint16              z;

    Vector3uint16() : x(0), y(0), z(0) {}
    Vector3uint16(uint16 _x, uint16 _y, uint16 _z) : x(_x), y(_y), z(_z) {}
    Vector3uint16(const class Vector3& v);
    Vector3uint16(class BinaryInput& bi);
    void serialize(class BinaryOutput& bo) const;
    void deserialize(class BinaryInput& bi);

};
#pragma pack(pop, Vector3uint16_align)

}
#endif
