/**
 @file Vector4.cpp
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
  
 @created 2001-07-09
 @edited  2003-08-04
 */

#include <stdlib.h>
#include <limits>
#include "G3D/Vector4.h"
#include "G3D/Color4.h"
#include "G3D/g3dmath.h"
#include "G3D/stringutils.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"

namespace G3D {

unsigned int Vector4::hashCode() const {
    unsigned int xhash = (*(int*)(void*)(&x));
    unsigned int yhash = (*(int*)(void*)(&y));
    unsigned int zhash = (*(int*)(void*)(&z));
    unsigned int whash = (*(int*)(void*)(&w));

    return xhash + (yhash * 37) + (zhash * 101) + (whash * 241);
}

Vector4::Vector4(const class Color4& c) {
    x = c.r;
    y = c.g;
    z = c.b;
    w = c.a;
}


Vector4::Vector4(BinaryInput& b) {
    deserialize(b);
}


void Vector4::deserialize(BinaryInput& b) {
    x = b.readFloat32();
    y = b.readFloat32();
    z = b.readFloat32();
    w = b.readFloat32();
}


void Vector4::serialize(BinaryOutput& b) const {
    b.writeFloat32(x);
    b.writeFloat32(y);
    b.writeFloat32(z);
    b.writeFloat32(w);
}

//----------------------------------------------------------------------------

Vector4 Vector4::operator/ (G3D::Real fScalar) const {
    Vector4 kQuot;

    if ( fScalar != 0.0 ) {
		G3D::Real fInvScalar = 1.0 / fScalar;
        kQuot.x = fInvScalar * x;
        kQuot.y = fInvScalar * y;
        kQuot.z = fInvScalar * z;
        kQuot.w = fInvScalar * w;
        return kQuot;
    } else {
        return Vector4(G3D::infReal, G3D::infReal,
            G3D::infReal, G3D::infReal);
    }
}

//----------------------------------------------------------------------------
Vector4& Vector4::operator/= (G3D::Real fScalar) {
    if (fScalar != 0.0) {
		G3D::Real fInvScalar = 1.0 / fScalar;
        x *= fInvScalar;
        y *= fInvScalar;
        z *= fInvScalar;
        w *= fInvScalar;
    } else {
        x = G3D::infReal;
        y = G3D::infReal;
        z = G3D::infReal;
        w = G3D::infReal;
    }

    return *this;
}


//----------------------------------------------------------------------------

std::string Vector4::toString() const {
    return G3D::format("(%g, %g, %g, %g)", x, y, z, w);
}

}; // namespace
