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

namespace G3D {

const Vector2& Vector2::zero() {
    static Vector2 v(0, 0);
    return v;
}

const Vector2& Vector2::unitX() {
    static Vector2 v(1, 0);
    return v;
}

const Vector2& Vector2::unitY() {
    static Vector2 v(0, 1);
    return v;
}

// Deprecated.
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
Vector2 Vector2::operator/ (double fScalar) const {
    Vector2 kQuot;

    if ( fScalar != 0.0 ) {
		double fInvScalar = 1.0 / fScalar;
        kQuot.x = fInvScalar * x;
        kQuot.y = fInvScalar * y;
        return kQuot;
    } else {
        return Vector2(inf, inf);
    }
}

//----------------------------------------------------------------------------
Vector2& Vector2::operator/= (double fScalar) {
    if (fScalar != 0.0) {
		double fInvScalar = 1.0 / fScalar;
        x *= fInvScalar;
        y *= fInvScalar;
    } else {
        x = inf;
        y = inf;
    }

    return *this;
}

//----------------------------------------------------------------------------
double Vector2::unitize (double fTolerance) {
	double fLength = length();

    if (fLength > fTolerance) {
		double fInvLength = 1.0 / fLength;
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
Vector2 Vector2::yx() const  { return Vector2       (y, x); }
Vector2 Vector2::xy() const  { return Vector2       (x, y); }
Vector2 Vector2::yy() const  { return Vector2       (y, y); }

// 3-char swizzles

Vector3 Vector2::xxx() const  { return Vector3       (x, x, x); }
Vector3 Vector2::yxx() const  { return Vector3       (y, x, x); }
Vector3 Vector2::xyx() const  { return Vector3       (x, y, x); }
Vector3 Vector2::yyx() const  { return Vector3       (y, y, x); }
Vector3 Vector2::xxy() const  { return Vector3       (x, x, y); }
Vector3 Vector2::yxy() const  { return Vector3       (y, x, y); }
Vector3 Vector2::xyy() const  { return Vector3       (x, y, y); }
Vector3 Vector2::yyy() const  { return Vector3       (y, y, y); }

// 4-char swizzles

Vector4 Vector2::xxxx() const  { return Vector4       (x, x, x, x); }
Vector4 Vector2::yxxx() const  { return Vector4       (y, x, x, x); }
Vector4 Vector2::xyxx() const  { return Vector4       (x, y, x, x); }
Vector4 Vector2::yyxx() const  { return Vector4       (y, y, x, x); }
Vector4 Vector2::xxyx() const  { return Vector4       (x, x, y, x); }
Vector4 Vector2::yxyx() const  { return Vector4       (y, x, y, x); }
Vector4 Vector2::xyyx() const  { return Vector4       (x, y, y, x); }
Vector4 Vector2::yyyx() const  { return Vector4       (y, y, y, x); }
Vector4 Vector2::xxxy() const  { return Vector4       (x, x, x, y); }
Vector4 Vector2::yxxy() const  { return Vector4       (y, x, x, y); }
Vector4 Vector2::xyxy() const  { return Vector4       (x, y, x, y); }
Vector4 Vector2::yyxy() const  { return Vector4       (y, y, x, y); }
Vector4 Vector2::xxyy() const  { return Vector4       (x, x, y, y); }
Vector4 Vector2::yxyy() const  { return Vector4       (y, x, y, y); }
Vector4 Vector2::xyyy() const  { return Vector4       (x, y, y, y); }
Vector4 Vector2::yyyy() const  { return Vector4       (y, y, y, y); }



} // namespace
