/**
 @file Vector2.cpp
 
 2D vector class, used for texture coordinates primarily.
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @cite Portions based on Dave Eberly'x Magic Software Library
  at http://www.magic-software.com
 
 @created 2001-06-02
 @edited  2003-09-29
 */

#include <stdlib.h>
#include "G3D/Vector2.h"
#include "G3D/g3dmath.h"
#include "G3D/format.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/VectorSwizzle.h"

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


// 2-char swizzles

Vector2 Vector2::xx() const  { return Vector2       (x, x); }
VectorSwizzle2 Vector2::xx() { return VectorSwizzle2(x, x); }
Vector2 Vector2::yx() const  { return Vector2       (y, x); }
VectorSwizzle2 Vector2::yx() { return VectorSwizzle2(y, x); }
Vector2 Vector2::xy() const  { return Vector2       (x, y); }
VectorSwizzle2 Vector2::xy() { return VectorSwizzle2(x, y); }
Vector2 Vector2::yy() const  { return Vector2       (y, y); }
VectorSwizzle2 Vector2::yy() { return VectorSwizzle2(y, y); }

// 3-char swizzles

Vector3 Vector2::xxx() const  { return Vector3       (x, x, x); }
VectorSwizzle3 Vector2::xxx() { return VectorSwizzle3(x, x, x); }
Vector3 Vector2::yxx() const  { return Vector3       (y, x, x); }
VectorSwizzle3 Vector2::yxx() { return VectorSwizzle3(y, x, x); }
Vector3 Vector2::xyx() const  { return Vector3       (x, y, x); }
VectorSwizzle3 Vector2::xyx() { return VectorSwizzle3(x, y, x); }
Vector3 Vector2::yyx() const  { return Vector3       (y, y, x); }
VectorSwizzle3 Vector2::yyx() { return VectorSwizzle3(y, y, x); }
Vector3 Vector2::xxy() const  { return Vector3       (x, x, y); }
VectorSwizzle3 Vector2::xxy() { return VectorSwizzle3(x, x, y); }
Vector3 Vector2::yxy() const  { return Vector3       (y, x, y); }
VectorSwizzle3 Vector2::yxy() { return VectorSwizzle3(y, x, y); }
Vector3 Vector2::xyy() const  { return Vector3       (x, y, y); }
VectorSwizzle3 Vector2::xyy() { return VectorSwizzle3(x, y, y); }
Vector3 Vector2::yyy() const  { return Vector3       (y, y, y); }
VectorSwizzle3 Vector2::yyy() { return VectorSwizzle3(y, y, y); }

// 4-char swizzles

Vector4 Vector2::xxxx() const  { return Vector4       (x, x, x, x); }
VectorSwizzle4 Vector2::xxxx() { return VectorSwizzle4(x, x, x, x); }
Vector4 Vector2::yxxx() const  { return Vector4       (y, x, x, x); }
VectorSwizzle4 Vector2::yxxx() { return VectorSwizzle4(y, x, x, x); }
Vector4 Vector2::xyxx() const  { return Vector4       (x, y, x, x); }
VectorSwizzle4 Vector2::xyxx() { return VectorSwizzle4(x, y, x, x); }
Vector4 Vector2::yyxx() const  { return Vector4       (y, y, x, x); }
VectorSwizzle4 Vector2::yyxx() { return VectorSwizzle4(y, y, x, x); }
Vector4 Vector2::xxyx() const  { return Vector4       (x, x, y, x); }
VectorSwizzle4 Vector2::xxyx() { return VectorSwizzle4(x, x, y, x); }
Vector4 Vector2::yxyx() const  { return Vector4       (y, x, y, x); }
VectorSwizzle4 Vector2::yxyx() { return VectorSwizzle4(y, x, y, x); }
Vector4 Vector2::xyyx() const  { return Vector4       (x, y, y, x); }
VectorSwizzle4 Vector2::xyyx() { return VectorSwizzle4(x, y, y, x); }
Vector4 Vector2::yyyx() const  { return Vector4       (y, y, y, x); }
VectorSwizzle4 Vector2::yyyx() { return VectorSwizzle4(y, y, y, x); }
Vector4 Vector2::xxxy() const  { return Vector4       (x, x, x, y); }
VectorSwizzle4 Vector2::xxxy() { return VectorSwizzle4(x, x, x, y); }
Vector4 Vector2::yxxy() const  { return Vector4       (y, x, x, y); }
VectorSwizzle4 Vector2::yxxy() { return VectorSwizzle4(y, x, x, y); }
Vector4 Vector2::xyxy() const  { return Vector4       (x, y, x, y); }
VectorSwizzle4 Vector2::xyxy() { return VectorSwizzle4(x, y, x, y); }
Vector4 Vector2::yyxy() const  { return Vector4       (y, y, x, y); }
VectorSwizzle4 Vector2::yyxy() { return VectorSwizzle4(y, y, x, y); }
Vector4 Vector2::xxyy() const  { return Vector4       (x, x, y, y); }
VectorSwizzle4 Vector2::xxyy() { return VectorSwizzle4(x, x, y, y); }
Vector4 Vector2::yxyy() const  { return Vector4       (y, x, y, y); }
VectorSwizzle4 Vector2::yxyy() { return VectorSwizzle4(y, x, y, y); }
Vector4 Vector2::xyyy() const  { return Vector4       (x, y, y, y); }
VectorSwizzle4 Vector2::xyyy() { return VectorSwizzle4(x, y, y, y); }
Vector4 Vector2::yyyy() const  { return Vector4       (y, y, y, y); }
VectorSwizzle4 Vector2::yyyy() { return VectorSwizzle4(y, y, y, y); }



} // namespace
