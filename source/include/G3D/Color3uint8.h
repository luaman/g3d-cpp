/** 
  @file Color3uint8.h
 
  @maintainer Morgan McGuire, graphics3d.com
 
  @created 2003-04-07
  @edited  2003-06-24

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.
 */

#ifndef COLOR3UINT8_H
#define COLOR3UINT8_H

#include "G3D/platform.h"
#include "G3D/g3dmath.h"

namespace G3D {

/**
 Represents a Color3 as a packed integer.  Convenient
 for creating unsigned int vertex arrays.  Used by
 CImage as the underlying format.

 <B>WARNING</B>: Integer color formats are different than
 integer vertex formats.  The color channels are automatically
 scaled by 255 (because OpenGL automatically scales integer
 colors back by this factor).  So Color3(1,1,1) == Color3uint8(255,255,255)
 but Vector3(1,1,1) == Vector3int16(1,1,1).

 */

#if defined(G3D_WIN32)
    // Switch to tight alignment
    #pragma pack(push, 1)
#endif

class Color3uint8 {
public:
    uint8       r;
    uint8       g;
    uint8       b;

    Color3uint8() : r(0), g(0), b(0) {}

    Color3uint8(const class Color3& c);

    Color3uint8(class BinaryInput& bi);

    void serialize(class BinaryOutput& bo) const;

    void deserialize(class BinaryInput& bi);

}
#if defined(G3D_LINUX) || defined(G3D_OSX)
    __attribute((aligned(1)))
#endif

;

#ifdef G3D_WIN32
  #pragma pack(pop)
#endif
}

#endif
