/** 
  @file Color4uint8.h
 
  @maintainer Morgan McGuire, graphics3d.com
 
  @created 2003-04-07
  @edited  2003-04-08

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.
 */

#ifndef Color4UINT8_H
#define Color4UINT8_H

#include "G3D/g3dmath.h"

namespace G3D {

/**
 Represents a Color4 as a packed integer.  Convenient
 for creating unsigned int vertex arrays.  Used by
 CImage as the underlying format.

 <B>WARNING</B>: Integer color formats are different than
 integer vertex formats.  The color channels are automatically
 scaled by 255 (because OpenGL automatically scales integer
 colors back by this factor).  So Color4(1,1,1) == Color4uint8(255,255,255)
 but Vector3(1,1,1) == Vector3int16(1,1,1).

 */
// Switch to tight alignment
#pragma pack(push, 1)
class Color4uint8 {
public:
    uint8       r;
    uint8       g;
    uint8       b;
    uint8       a;

    Color4uint8() : r(0), g(0), b(0), a(0) {}

    Color4uint8(const class Color4& c);

    Color4uint8(class BinaryInput& bi);

    void serialize(class BinaryOutput& bo) const;

    void deserialize(class BinaryInput& bi);

};
#pragma pack(pop)

}

#endif
