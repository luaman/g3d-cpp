/**
 @file Vector2.cpp
 
 2D vector class, used for texture coordinates primarily.
 
 @author Morgan McGuire and Laura Wollstadt, graphics3d.com
 
 @cite Portions based on Dave Eberly'x Magic Software Library
  at http://www.magic-software.com
 
 
 @created 2001-06-02
 @edited  2003-02-15
 */

#include <stdlib.h>
#include "G3D/Vector2.h"
#include "G3D/g3dmath.h"
#include "G3D/format.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"

namespace G3D {

const Vector2 Vector2::ZERO(0, 0);
const Vector2 Vector2::UNIT_S(1, 0);
const Vector2 Vector2::UNIT_T(0, 1);


unsigned int Vector2::hashCode() const {
    unsigned int xhash = (*(int*)(void*)(&x));
    unsigned int yhash = (*(int*)(void*)(&y));

    return xhash + (yhash * 37);
}

Vector2::Vector2(BinaryInput& b) {
    deserialize(b);
}

void Vector2::deserialize(BinaryInput& b) {
    x = b.readFloat32();
    y = b.readFloat32();
}

void Vector2::serialize(BinaryOutput& b) const {
    b.writeFloat32(x);
    b.writeFloat32(y);
}


//----------------------------------------------------------------------------

Vector2 Vector2::random() {
    Vector2 result;

    do {
        result = Vector2(symmetricRandom(), symmetricRandom());
    } while ((result.squaredLength() > 1) || (result.squaredLength() == 1.0));

    result.unitize();

    return result;
}

//----------------------------------------------------------------------------
Vector2 Vector2::operator/ (G3D::Real fScalar) const {
    Vector2 kQuot;

    if ( fScalar != 0.0 ) {
		G3D::Real fInvScalar = 1.0 / fScalar;
        kQuot.x = fInvScalar * x;
        kQuot.y = fInvScalar * y;
        return kQuot;
    } else {
        return Vector2(G3D::infReal, G3D::infReal);
    }
}

//----------------------------------------------------------------------------
Vector2& Vector2::operator/= (G3D::Real fScalar) {
    if (fScalar != 0.0) {
		G3D::Real fInvScalar = 1.0 / fScalar;
        x *= fInvScalar;
        y *= fInvScalar;
    } else {
        x = G3D::infReal;
        y = G3D::infReal;
    }

    return *this;
}

//----------------------------------------------------------------------------
G3D::Real Vector2::unitize (G3D::Real fTolerance) {
	G3D::Real fLength = length();

    if (fLength > fTolerance) {
		G3D::Real fInvLength = 1.0 / fLength;
        x *= fInvLength;
        y *= fInvLength;
    } else {
        fLength = 0.0;
    }

    return fLength;
}

//----------------------------------------------------------------------------

std::string Vector2::toString() const {
    return G3D::format("(%g, %g)", x, y);
}
}; // namespace
