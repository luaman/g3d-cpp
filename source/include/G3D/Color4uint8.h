/** 
  @file Color4uint8.h
 
  @maintainer Morgan McGuire, graphics3d.com
 
  @created 2003-04-07
  @edited  2003-06-24

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.
 */

#ifndef COLOR4UINT8_H
#define COLOR4UINT8_H

#include "G3D/g3dmath.h"
#include "G3D/platform.h"

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

#ifdef G3D_WIN32
    // Switch to tight alignment
    #pragma pack(push, 1)
#endif 

class Color4uint8 {
public:
    uint8       r;
    uint8       g;
    uint8       b;
    uint8       a;

    Color4uint8() : r(0), g(0), b(0), a(0) {}

    Color4uint8(const class Color4& c);

    Color4uint8(class BinaryInput& bi);

    // access vector V as V[0] = V.r, V[1] = V.g, V[2] = V.b
    //
    // WARNING.  These member functions rely on
    // (1) Color4uint8 not having virtual functions
    // (2) the data packed in a 3*sizeof(uint8) memory block
    G3D::uint8& operator[] (int i) const;
    operator G3D::uint8* ();
    operator const G3D::uint8* () const;

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


inline G3D::uint8& Color4uint8::operator[] (int i) const {
    return ((G3D::uint8*)this)[i];
}

//----------------------------------------------------------------------------
inline Color4uint8::operator G3D::uint8* () {
    return (G3D::uint8*)this;
}

inline Color4uint8::operator const G3D::uint8* () const {
    return (G3D::uint8*)this;
}

}

#endif
