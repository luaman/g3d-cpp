/** 
  @file PackedColor3.h
 
  @maintainer Morgan McGuire, graphics3d.com
 
  @created 2003-04-07
  @edited  2003-04-07

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.
 */

#ifndef PACKEDCOLOR3_H
#define PACKEDCOLOR3_H

#include "G3D/g3dmath.h"

namespace G3D {

/**
 Represents a Color3 as a packed integer.  Convenient
 for creating unsigned int vertex arrays.  Used by
 CImage as the underlying format.

 This is provided because there is no natural 'uint24'
 format.  There is no PackedColor4 because there is no single
 convention for which end to stick the alpha value on
 and it is relatively easy to abuse uint32 for a 4-channel
 value.
 */
// Switch to tight alignment
#pragma pack(push, PackedColor3_align)
#pragma pack(1)
class PackedColor3 {
public:
    uint8       r;
    uint8       g;
    uint8       b;

    PackedColor3() : r(0), g(0), b(0) {}

    PackedColor3(const class Color3& c);

    PackedColor3(class BinaryInput& bi);

    void serialize(class BinaryOutput& bo) const;

    void deserialize(class BinaryInput& bi);
};
#pragma pack(pop, PackedColor3_align)

}

#endif