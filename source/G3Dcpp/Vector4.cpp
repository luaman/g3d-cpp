/**
 @file Vector4.cpp
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
  
 @created 2001-07-09
 @edited  2003-09-29
 */

#include <stdlib.h>
#include <limits>
#include "G3D/Vector4.h"
#include "G3D/Color4.h"
#include "G3D/g3dmath.h"
#include "G3D/stringutils.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/VectorSwizzle.h"

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


Vector4::Vector4(const Vector2& v1, const Vector2& v2) {
    x = v1.x;
    y = v1.y;
    z = v2.x;
    w = v2.y;
}


Vector4::Vector4(const Vector2& v1, float fz, float fw) {
    x = v1.x;
    y = v1.y;
    z = fz;
    w = fw;
}


Vector4::Vector4(const VectorSwizzle2& v1, const VectorSwizzle2& v2) {
    x = v1.x;
    y = v1.y;
    z = v2.x;
    w = v2.y;
}

Vector4::Vector4(const VectorSwizzle3& v, float fW) {
    x = v.x;
    y = v.y;
    z = v.z;
    w = fw;
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

// 2-char swizzles

Vector2 Vector4::xx() const  { return Vector2       (x, x); }
VectorSwizzle2 Vector4::xx() { return VectorSwizzle2(x, x); }
Vector2 Vector4::yx() const  { return Vector2       (y, x); }
VectorSwizzle2 Vector4::yx() { return VectorSwizzle2(y, x); }
Vector2 Vector4::zx() const  { return Vector2       (z, x); }
VectorSwizzle2 Vector4::zx() { return VectorSwizzle2(z, x); }
Vector2 Vector4::wx() const  { return Vector2       (w, x); }
VectorSwizzle2 Vector4::wx() { return VectorSwizzle2(w, x); }
Vector2 Vector4::xy() const  { return Vector2       (x, y); }
VectorSwizzle2 Vector4::xy() { return VectorSwizzle2(x, y); }
Vector2 Vector4::yy() const  { return Vector2       (y, y); }
VectorSwizzle2 Vector4::yy() { return VectorSwizzle2(y, y); }
Vector2 Vector4::zy() const  { return Vector2       (z, y); }
VectorSwizzle2 Vector4::zy() { return VectorSwizzle2(z, y); }
Vector2 Vector4::wy() const  { return Vector2       (w, y); }
VectorSwizzle2 Vector4::wy() { return VectorSwizzle2(w, y); }
Vector2 Vector4::xz() const  { return Vector2       (x, z); }
VectorSwizzle2 Vector4::xz() { return VectorSwizzle2(x, z); }
Vector2 Vector4::yz() const  { return Vector2       (y, z); }
VectorSwizzle2 Vector4::yz() { return VectorSwizzle2(y, z); }
Vector2 Vector4::zz() const  { return Vector2       (z, z); }
VectorSwizzle2 Vector4::zz() { return VectorSwizzle2(z, z); }
Vector2 Vector4::wz() const  { return Vector2       (w, z); }
VectorSwizzle2 Vector4::wz() { return VectorSwizzle2(w, z); }
Vector2 Vector4::xw() const  { return Vector2       (x, w); }
VectorSwizzle2 Vector4::xw() { return VectorSwizzle2(x, w); }
Vector2 Vector4::yw() const  { return Vector2       (y, w); }
VectorSwizzle2 Vector4::yw() { return VectorSwizzle2(y, w); }
Vector2 Vector4::zw() const  { return Vector2       (z, w); }
VectorSwizzle2 Vector4::zw() { return VectorSwizzle2(z, w); }
Vector2 Vector4::ww() const  { return Vector2       (w, w); }
VectorSwizzle2 Vector4::ww() { return VectorSwizzle2(w, w); }

// 3-char swizzles

Vector3 Vector4::xxx() const  { return Vector3       (x, x, x); }
VectorSwizzle3 Vector4::xxx() { return VectorSwizzle3(x, x, x); }
Vector3 Vector4::yxx() const  { return Vector3       (y, x, x); }
VectorSwizzle3 Vector4::yxx() { return VectorSwizzle3(y, x, x); }
Vector3 Vector4::zxx() const  { return Vector3       (z, x, x); }
VectorSwizzle3 Vector4::zxx() { return VectorSwizzle3(z, x, x); }
Vector3 Vector4::wxx() const  { return Vector3       (w, x, x); }
VectorSwizzle3 Vector4::wxx() { return VectorSwizzle3(w, x, x); }
Vector3 Vector4::xyx() const  { return Vector3       (x, y, x); }
VectorSwizzle3 Vector4::xyx() { return VectorSwizzle3(x, y, x); }
Vector3 Vector4::yyx() const  { return Vector3       (y, y, x); }
VectorSwizzle3 Vector4::yyx() { return VectorSwizzle3(y, y, x); }
Vector3 Vector4::zyx() const  { return Vector3       (z, y, x); }
VectorSwizzle3 Vector4::zyx() { return VectorSwizzle3(z, y, x); }
Vector3 Vector4::wyx() const  { return Vector3       (w, y, x); }
VectorSwizzle3 Vector4::wyx() { return VectorSwizzle3(w, y, x); }
Vector3 Vector4::xzx() const  { return Vector3       (x, z, x); }
VectorSwizzle3 Vector4::xzx() { return VectorSwizzle3(x, z, x); }
Vector3 Vector4::yzx() const  { return Vector3       (y, z, x); }
VectorSwizzle3 Vector4::yzx() { return VectorSwizzle3(y, z, x); }
Vector3 Vector4::zzx() const  { return Vector3       (z, z, x); }
VectorSwizzle3 Vector4::zzx() { return VectorSwizzle3(z, z, x); }
Vector3 Vector4::wzx() const  { return Vector3       (w, z, x); }
VectorSwizzle3 Vector4::wzx() { return VectorSwizzle3(w, z, x); }
Vector3 Vector4::xwx() const  { return Vector3       (x, w, x); }
VectorSwizzle3 Vector4::xwx() { return VectorSwizzle3(x, w, x); }
Vector3 Vector4::ywx() const  { return Vector3       (y, w, x); }
VectorSwizzle3 Vector4::ywx() { return VectorSwizzle3(y, w, x); }
Vector3 Vector4::zwx() const  { return Vector3       (z, w, x); }
VectorSwizzle3 Vector4::zwx() { return VectorSwizzle3(z, w, x); }
Vector3 Vector4::wwx() const  { return Vector3       (w, w, x); }
VectorSwizzle3 Vector4::wwx() { return VectorSwizzle3(w, w, x); }
Vector3 Vector4::xxy() const  { return Vector3       (x, x, y); }
VectorSwizzle3 Vector4::xxy() { return VectorSwizzle3(x, x, y); }
Vector3 Vector4::yxy() const  { return Vector3       (y, x, y); }
VectorSwizzle3 Vector4::yxy() { return VectorSwizzle3(y, x, y); }
Vector3 Vector4::zxy() const  { return Vector3       (z, x, y); }
VectorSwizzle3 Vector4::zxy() { return VectorSwizzle3(z, x, y); }
Vector3 Vector4::wxy() const  { return Vector3       (w, x, y); }
VectorSwizzle3 Vector4::wxy() { return VectorSwizzle3(w, x, y); }
Vector3 Vector4::xyy() const  { return Vector3       (x, y, y); }
VectorSwizzle3 Vector4::xyy() { return VectorSwizzle3(x, y, y); }
Vector3 Vector4::yyy() const  { return Vector3       (y, y, y); }
VectorSwizzle3 Vector4::yyy() { return VectorSwizzle3(y, y, y); }
Vector3 Vector4::zyy() const  { return Vector3       (z, y, y); }
VectorSwizzle3 Vector4::zyy() { return VectorSwizzle3(z, y, y); }
Vector3 Vector4::wyy() const  { return Vector3       (w, y, y); }
VectorSwizzle3 Vector4::wyy() { return VectorSwizzle3(w, y, y); }
Vector3 Vector4::xzy() const  { return Vector3       (x, z, y); }
VectorSwizzle3 Vector4::xzy() { return VectorSwizzle3(x, z, y); }
Vector3 Vector4::yzy() const  { return Vector3       (y, z, y); }
VectorSwizzle3 Vector4::yzy() { return VectorSwizzle3(y, z, y); }
Vector3 Vector4::zzy() const  { return Vector3       (z, z, y); }
VectorSwizzle3 Vector4::zzy() { return VectorSwizzle3(z, z, y); }
Vector3 Vector4::wzy() const  { return Vector3       (w, z, y); }
VectorSwizzle3 Vector4::wzy() { return VectorSwizzle3(w, z, y); }
Vector3 Vector4::xwy() const  { return Vector3       (x, w, y); }
VectorSwizzle3 Vector4::xwy() { return VectorSwizzle3(x, w, y); }
Vector3 Vector4::ywy() const  { return Vector3       (y, w, y); }
VectorSwizzle3 Vector4::ywy() { return VectorSwizzle3(y, w, y); }
Vector3 Vector4::zwy() const  { return Vector3       (z, w, y); }
VectorSwizzle3 Vector4::zwy() { return VectorSwizzle3(z, w, y); }
Vector3 Vector4::wwy() const  { return Vector3       (w, w, y); }
VectorSwizzle3 Vector4::wwy() { return VectorSwizzle3(w, w, y); }
Vector3 Vector4::xxz() const  { return Vector3       (x, x, z); }
VectorSwizzle3 Vector4::xxz() { return VectorSwizzle3(x, x, z); }
Vector3 Vector4::yxz() const  { return Vector3       (y, x, z); }
VectorSwizzle3 Vector4::yxz() { return VectorSwizzle3(y, x, z); }
Vector3 Vector4::zxz() const  { return Vector3       (z, x, z); }
VectorSwizzle3 Vector4::zxz() { return VectorSwizzle3(z, x, z); }
Vector3 Vector4::wxz() const  { return Vector3       (w, x, z); }
VectorSwizzle3 Vector4::wxz() { return VectorSwizzle3(w, x, z); }
Vector3 Vector4::xyz() const  { return Vector3       (x, y, z); }
VectorSwizzle3 Vector4::xyz() { return VectorSwizzle3(x, y, z); }
Vector3 Vector4::yyz() const  { return Vector3       (y, y, z); }
VectorSwizzle3 Vector4::yyz() { return VectorSwizzle3(y, y, z); }
Vector3 Vector4::zyz() const  { return Vector3       (z, y, z); }
VectorSwizzle3 Vector4::zyz() { return VectorSwizzle3(z, y, z); }
Vector3 Vector4::wyz() const  { return Vector3       (w, y, z); }
VectorSwizzle3 Vector4::wyz() { return VectorSwizzle3(w, y, z); }
Vector3 Vector4::xzz() const  { return Vector3       (x, z, z); }
VectorSwizzle3 Vector4::xzz() { return VectorSwizzle3(x, z, z); }
Vector3 Vector4::yzz() const  { return Vector3       (y, z, z); }
VectorSwizzle3 Vector4::yzz() { return VectorSwizzle3(y, z, z); }
Vector3 Vector4::zzz() const  { return Vector3       (z, z, z); }
VectorSwizzle3 Vector4::zzz() { return VectorSwizzle3(z, z, z); }
Vector3 Vector4::wzz() const  { return Vector3       (w, z, z); }
VectorSwizzle3 Vector4::wzz() { return VectorSwizzle3(w, z, z); }
Vector3 Vector4::xwz() const  { return Vector3       (x, w, z); }
VectorSwizzle3 Vector4::xwz() { return VectorSwizzle3(x, w, z); }
Vector3 Vector4::ywz() const  { return Vector3       (y, w, z); }
VectorSwizzle3 Vector4::ywz() { return VectorSwizzle3(y, w, z); }
Vector3 Vector4::zwz() const  { return Vector3       (z, w, z); }
VectorSwizzle3 Vector4::zwz() { return VectorSwizzle3(z, w, z); }
Vector3 Vector4::wwz() const  { return Vector3       (w, w, z); }
VectorSwizzle3 Vector4::wwz() { return VectorSwizzle3(w, w, z); }
Vector3 Vector4::xxw() const  { return Vector3       (x, x, w); }
VectorSwizzle3 Vector4::xxw() { return VectorSwizzle3(x, x, w); }
Vector3 Vector4::yxw() const  { return Vector3       (y, x, w); }
VectorSwizzle3 Vector4::yxw() { return VectorSwizzle3(y, x, w); }
Vector3 Vector4::zxw() const  { return Vector3       (z, x, w); }
VectorSwizzle3 Vector4::zxw() { return VectorSwizzle3(z, x, w); }
Vector3 Vector4::wxw() const  { return Vector3       (w, x, w); }
VectorSwizzle3 Vector4::wxw() { return VectorSwizzle3(w, x, w); }
Vector3 Vector4::xyw() const  { return Vector3       (x, y, w); }
VectorSwizzle3 Vector4::xyw() { return VectorSwizzle3(x, y, w); }
Vector3 Vector4::yyw() const  { return Vector3       (y, y, w); }
VectorSwizzle3 Vector4::yyw() { return VectorSwizzle3(y, y, w); }
Vector3 Vector4::zyw() const  { return Vector3       (z, y, w); }
VectorSwizzle3 Vector4::zyw() { return VectorSwizzle3(z, y, w); }
Vector3 Vector4::wyw() const  { return Vector3       (w, y, w); }
VectorSwizzle3 Vector4::wyw() { return VectorSwizzle3(w, y, w); }
Vector3 Vector4::xzw() const  { return Vector3       (x, z, w); }
VectorSwizzle3 Vector4::xzw() { return VectorSwizzle3(x, z, w); }
Vector3 Vector4::yzw() const  { return Vector3       (y, z, w); }
VectorSwizzle3 Vector4::yzw() { return VectorSwizzle3(y, z, w); }
Vector3 Vector4::zzw() const  { return Vector3       (z, z, w); }
VectorSwizzle3 Vector4::zzw() { return VectorSwizzle3(z, z, w); }
Vector3 Vector4::wzw() const  { return Vector3       (w, z, w); }
VectorSwizzle3 Vector4::wzw() { return VectorSwizzle3(w, z, w); }
Vector3 Vector4::xww() const  { return Vector3       (x, w, w); }
VectorSwizzle3 Vector4::xww() { return VectorSwizzle3(x, w, w); }
Vector3 Vector4::yww() const  { return Vector3       (y, w, w); }
VectorSwizzle3 Vector4::yww() { return VectorSwizzle3(y, w, w); }
Vector3 Vector4::zww() const  { return Vector3       (z, w, w); }
VectorSwizzle3 Vector4::zww() { return VectorSwizzle3(z, w, w); }
Vector3 Vector4::www() const  { return Vector3       (w, w, w); }
VectorSwizzle3 Vector4::www() { return VectorSwizzle3(w, w, w); }

// 4-char swizzles

Vector4 Vector4::xxxx() const  { return Vector4       (x, x, x, x); }
VectorSwizzle4 Vector4::xxxx() { return VectorSwizzle4(x, x, x, x); }
Vector4 Vector4::yxxx() const  { return Vector4       (y, x, x, x); }
VectorSwizzle4 Vector4::yxxx() { return VectorSwizzle4(y, x, x, x); }
Vector4 Vector4::zxxx() const  { return Vector4       (z, x, x, x); }
VectorSwizzle4 Vector4::zxxx() { return VectorSwizzle4(z, x, x, x); }
Vector4 Vector4::wxxx() const  { return Vector4       (w, x, x, x); }
VectorSwizzle4 Vector4::wxxx() { return VectorSwizzle4(w, x, x, x); }
Vector4 Vector4::xyxx() const  { return Vector4       (x, y, x, x); }
VectorSwizzle4 Vector4::xyxx() { return VectorSwizzle4(x, y, x, x); }
Vector4 Vector4::yyxx() const  { return Vector4       (y, y, x, x); }
VectorSwizzle4 Vector4::yyxx() { return VectorSwizzle4(y, y, x, x); }
Vector4 Vector4::zyxx() const  { return Vector4       (z, y, x, x); }
VectorSwizzle4 Vector4::zyxx() { return VectorSwizzle4(z, y, x, x); }
Vector4 Vector4::wyxx() const  { return Vector4       (w, y, x, x); }
VectorSwizzle4 Vector4::wyxx() { return VectorSwizzle4(w, y, x, x); }
Vector4 Vector4::xzxx() const  { return Vector4       (x, z, x, x); }
VectorSwizzle4 Vector4::xzxx() { return VectorSwizzle4(x, z, x, x); }
Vector4 Vector4::yzxx() const  { return Vector4       (y, z, x, x); }
VectorSwizzle4 Vector4::yzxx() { return VectorSwizzle4(y, z, x, x); }
Vector4 Vector4::zzxx() const  { return Vector4       (z, z, x, x); }
VectorSwizzle4 Vector4::zzxx() { return VectorSwizzle4(z, z, x, x); }
Vector4 Vector4::wzxx() const  { return Vector4       (w, z, x, x); }
VectorSwizzle4 Vector4::wzxx() { return VectorSwizzle4(w, z, x, x); }
Vector4 Vector4::xwxx() const  { return Vector4       (x, w, x, x); }
VectorSwizzle4 Vector4::xwxx() { return VectorSwizzle4(x, w, x, x); }
Vector4 Vector4::ywxx() const  { return Vector4       (y, w, x, x); }
VectorSwizzle4 Vector4::ywxx() { return VectorSwizzle4(y, w, x, x); }
Vector4 Vector4::zwxx() const  { return Vector4       (z, w, x, x); }
VectorSwizzle4 Vector4::zwxx() { return VectorSwizzle4(z, w, x, x); }
Vector4 Vector4::wwxx() const  { return Vector4       (w, w, x, x); }
VectorSwizzle4 Vector4::wwxx() { return VectorSwizzle4(w, w, x, x); }
Vector4 Vector4::xxyx() const  { return Vector4       (x, x, y, x); }
VectorSwizzle4 Vector4::xxyx() { return VectorSwizzle4(x, x, y, x); }
Vector4 Vector4::yxyx() const  { return Vector4       (y, x, y, x); }
VectorSwizzle4 Vector4::yxyx() { return VectorSwizzle4(y, x, y, x); }
Vector4 Vector4::zxyx() const  { return Vector4       (z, x, y, x); }
VectorSwizzle4 Vector4::zxyx() { return VectorSwizzle4(z, x, y, x); }
Vector4 Vector4::wxyx() const  { return Vector4       (w, x, y, x); }
VectorSwizzle4 Vector4::wxyx() { return VectorSwizzle4(w, x, y, x); }
Vector4 Vector4::xyyx() const  { return Vector4       (x, y, y, x); }
VectorSwizzle4 Vector4::xyyx() { return VectorSwizzle4(x, y, y, x); }
Vector4 Vector4::yyyx() const  { return Vector4       (y, y, y, x); }
VectorSwizzle4 Vector4::yyyx() { return VectorSwizzle4(y, y, y, x); }
Vector4 Vector4::zyyx() const  { return Vector4       (z, y, y, x); }
VectorSwizzle4 Vector4::zyyx() { return VectorSwizzle4(z, y, y, x); }
Vector4 Vector4::wyyx() const  { return Vector4       (w, y, y, x); }
VectorSwizzle4 Vector4::wyyx() { return VectorSwizzle4(w, y, y, x); }
Vector4 Vector4::xzyx() const  { return Vector4       (x, z, y, x); }
VectorSwizzle4 Vector4::xzyx() { return VectorSwizzle4(x, z, y, x); }
Vector4 Vector4::yzyx() const  { return Vector4       (y, z, y, x); }
VectorSwizzle4 Vector4::yzyx() { return VectorSwizzle4(y, z, y, x); }
Vector4 Vector4::zzyx() const  { return Vector4       (z, z, y, x); }
VectorSwizzle4 Vector4::zzyx() { return VectorSwizzle4(z, z, y, x); }
Vector4 Vector4::wzyx() const  { return Vector4       (w, z, y, x); }
VectorSwizzle4 Vector4::wzyx() { return VectorSwizzle4(w, z, y, x); }
Vector4 Vector4::xwyx() const  { return Vector4       (x, w, y, x); }
VectorSwizzle4 Vector4::xwyx() { return VectorSwizzle4(x, w, y, x); }
Vector4 Vector4::ywyx() const  { return Vector4       (y, w, y, x); }
VectorSwizzle4 Vector4::ywyx() { return VectorSwizzle4(y, w, y, x); }
Vector4 Vector4::zwyx() const  { return Vector4       (z, w, y, x); }
VectorSwizzle4 Vector4::zwyx() { return VectorSwizzle4(z, w, y, x); }
Vector4 Vector4::wwyx() const  { return Vector4       (w, w, y, x); }
VectorSwizzle4 Vector4::wwyx() { return VectorSwizzle4(w, w, y, x); }
Vector4 Vector4::xxzx() const  { return Vector4       (x, x, z, x); }
VectorSwizzle4 Vector4::xxzx() { return VectorSwizzle4(x, x, z, x); }
Vector4 Vector4::yxzx() const  { return Vector4       (y, x, z, x); }
VectorSwizzle4 Vector4::yxzx() { return VectorSwizzle4(y, x, z, x); }
Vector4 Vector4::zxzx() const  { return Vector4       (z, x, z, x); }
VectorSwizzle4 Vector4::zxzx() { return VectorSwizzle4(z, x, z, x); }
Vector4 Vector4::wxzx() const  { return Vector4       (w, x, z, x); }
VectorSwizzle4 Vector4::wxzx() { return VectorSwizzle4(w, x, z, x); }
Vector4 Vector4::xyzx() const  { return Vector4       (x, y, z, x); }
VectorSwizzle4 Vector4::xyzx() { return VectorSwizzle4(x, y, z, x); }
Vector4 Vector4::yyzx() const  { return Vector4       (y, y, z, x); }
VectorSwizzle4 Vector4::yyzx() { return VectorSwizzle4(y, y, z, x); }
Vector4 Vector4::zyzx() const  { return Vector4       (z, y, z, x); }
VectorSwizzle4 Vector4::zyzx() { return VectorSwizzle4(z, y, z, x); }
Vector4 Vector4::wyzx() const  { return Vector4       (w, y, z, x); }
VectorSwizzle4 Vector4::wyzx() { return VectorSwizzle4(w, y, z, x); }
Vector4 Vector4::xzzx() const  { return Vector4       (x, z, z, x); }
VectorSwizzle4 Vector4::xzzx() { return VectorSwizzle4(x, z, z, x); }
Vector4 Vector4::yzzx() const  { return Vector4       (y, z, z, x); }
VectorSwizzle4 Vector4::yzzx() { return VectorSwizzle4(y, z, z, x); }
Vector4 Vector4::zzzx() const  { return Vector4       (z, z, z, x); }
VectorSwizzle4 Vector4::zzzx() { return VectorSwizzle4(z, z, z, x); }
Vector4 Vector4::wzzx() const  { return Vector4       (w, z, z, x); }
VectorSwizzle4 Vector4::wzzx() { return VectorSwizzle4(w, z, z, x); }
Vector4 Vector4::xwzx() const  { return Vector4       (x, w, z, x); }
VectorSwizzle4 Vector4::xwzx() { return VectorSwizzle4(x, w, z, x); }
Vector4 Vector4::ywzx() const  { return Vector4       (y, w, z, x); }
VectorSwizzle4 Vector4::ywzx() { return VectorSwizzle4(y, w, z, x); }
Vector4 Vector4::zwzx() const  { return Vector4       (z, w, z, x); }
VectorSwizzle4 Vector4::zwzx() { return VectorSwizzle4(z, w, z, x); }
Vector4 Vector4::wwzx() const  { return Vector4       (w, w, z, x); }
VectorSwizzle4 Vector4::wwzx() { return VectorSwizzle4(w, w, z, x); }
Vector4 Vector4::xxwx() const  { return Vector4       (x, x, w, x); }
VectorSwizzle4 Vector4::xxwx() { return VectorSwizzle4(x, x, w, x); }
Vector4 Vector4::yxwx() const  { return Vector4       (y, x, w, x); }
VectorSwizzle4 Vector4::yxwx() { return VectorSwizzle4(y, x, w, x); }
Vector4 Vector4::zxwx() const  { return Vector4       (z, x, w, x); }
VectorSwizzle4 Vector4::zxwx() { return VectorSwizzle4(z, x, w, x); }
Vector4 Vector4::wxwx() const  { return Vector4       (w, x, w, x); }
VectorSwizzle4 Vector4::wxwx() { return VectorSwizzle4(w, x, w, x); }
Vector4 Vector4::xywx() const  { return Vector4       (x, y, w, x); }
VectorSwizzle4 Vector4::xywx() { return VectorSwizzle4(x, y, w, x); }
Vector4 Vector4::yywx() const  { return Vector4       (y, y, w, x); }
VectorSwizzle4 Vector4::yywx() { return VectorSwizzle4(y, y, w, x); }
Vector4 Vector4::zywx() const  { return Vector4       (z, y, w, x); }
VectorSwizzle4 Vector4::zywx() { return VectorSwizzle4(z, y, w, x); }
Vector4 Vector4::wywx() const  { return Vector4       (w, y, w, x); }
VectorSwizzle4 Vector4::wywx() { return VectorSwizzle4(w, y, w, x); }
Vector4 Vector4::xzwx() const  { return Vector4       (x, z, w, x); }
VectorSwizzle4 Vector4::xzwx() { return VectorSwizzle4(x, z, w, x); }
Vector4 Vector4::yzwx() const  { return Vector4       (y, z, w, x); }
VectorSwizzle4 Vector4::yzwx() { return VectorSwizzle4(y, z, w, x); }
Vector4 Vector4::zzwx() const  { return Vector4       (z, z, w, x); }
VectorSwizzle4 Vector4::zzwx() { return VectorSwizzle4(z, z, w, x); }
Vector4 Vector4::wzwx() const  { return Vector4       (w, z, w, x); }
VectorSwizzle4 Vector4::wzwx() { return VectorSwizzle4(w, z, w, x); }
Vector4 Vector4::xwwx() const  { return Vector4       (x, w, w, x); }
VectorSwizzle4 Vector4::xwwx() { return VectorSwizzle4(x, w, w, x); }
Vector4 Vector4::ywwx() const  { return Vector4       (y, w, w, x); }
VectorSwizzle4 Vector4::ywwx() { return VectorSwizzle4(y, w, w, x); }
Vector4 Vector4::zwwx() const  { return Vector4       (z, w, w, x); }
VectorSwizzle4 Vector4::zwwx() { return VectorSwizzle4(z, w, w, x); }
Vector4 Vector4::wwwx() const  { return Vector4       (w, w, w, x); }
VectorSwizzle4 Vector4::wwwx() { return VectorSwizzle4(w, w, w, x); }
Vector4 Vector4::xxxy() const  { return Vector4       (x, x, x, y); }
VectorSwizzle4 Vector4::xxxy() { return VectorSwizzle4(x, x, x, y); }
Vector4 Vector4::yxxy() const  { return Vector4       (y, x, x, y); }
VectorSwizzle4 Vector4::yxxy() { return VectorSwizzle4(y, x, x, y); }
Vector4 Vector4::zxxy() const  { return Vector4       (z, x, x, y); }
VectorSwizzle4 Vector4::zxxy() { return VectorSwizzle4(z, x, x, y); }
Vector4 Vector4::wxxy() const  { return Vector4       (w, x, x, y); }
VectorSwizzle4 Vector4::wxxy() { return VectorSwizzle4(w, x, x, y); }
Vector4 Vector4::xyxy() const  { return Vector4       (x, y, x, y); }
VectorSwizzle4 Vector4::xyxy() { return VectorSwizzle4(x, y, x, y); }
Vector4 Vector4::yyxy() const  { return Vector4       (y, y, x, y); }
VectorSwizzle4 Vector4::yyxy() { return VectorSwizzle4(y, y, x, y); }
Vector4 Vector4::zyxy() const  { return Vector4       (z, y, x, y); }
VectorSwizzle4 Vector4::zyxy() { return VectorSwizzle4(z, y, x, y); }
Vector4 Vector4::wyxy() const  { return Vector4       (w, y, x, y); }
VectorSwizzle4 Vector4::wyxy() { return VectorSwizzle4(w, y, x, y); }
Vector4 Vector4::xzxy() const  { return Vector4       (x, z, x, y); }
VectorSwizzle4 Vector4::xzxy() { return VectorSwizzle4(x, z, x, y); }
Vector4 Vector4::yzxy() const  { return Vector4       (y, z, x, y); }
VectorSwizzle4 Vector4::yzxy() { return VectorSwizzle4(y, z, x, y); }
Vector4 Vector4::zzxy() const  { return Vector4       (z, z, x, y); }
VectorSwizzle4 Vector4::zzxy() { return VectorSwizzle4(z, z, x, y); }
Vector4 Vector4::wzxy() const  { return Vector4       (w, z, x, y); }
VectorSwizzle4 Vector4::wzxy() { return VectorSwizzle4(w, z, x, y); }
Vector4 Vector4::xwxy() const  { return Vector4       (x, w, x, y); }
VectorSwizzle4 Vector4::xwxy() { return VectorSwizzle4(x, w, x, y); }
Vector4 Vector4::ywxy() const  { return Vector4       (y, w, x, y); }
VectorSwizzle4 Vector4::ywxy() { return VectorSwizzle4(y, w, x, y); }
Vector4 Vector4::zwxy() const  { return Vector4       (z, w, x, y); }
VectorSwizzle4 Vector4::zwxy() { return VectorSwizzle4(z, w, x, y); }
Vector4 Vector4::wwxy() const  { return Vector4       (w, w, x, y); }
VectorSwizzle4 Vector4::wwxy() { return VectorSwizzle4(w, w, x, y); }
Vector4 Vector4::xxyy() const  { return Vector4       (x, x, y, y); }
VectorSwizzle4 Vector4::xxyy() { return VectorSwizzle4(x, x, y, y); }
Vector4 Vector4::yxyy() const  { return Vector4       (y, x, y, y); }
VectorSwizzle4 Vector4::yxyy() { return VectorSwizzle4(y, x, y, y); }
Vector4 Vector4::zxyy() const  { return Vector4       (z, x, y, y); }
VectorSwizzle4 Vector4::zxyy() { return VectorSwizzle4(z, x, y, y); }
Vector4 Vector4::wxyy() const  { return Vector4       (w, x, y, y); }
VectorSwizzle4 Vector4::wxyy() { return VectorSwizzle4(w, x, y, y); }
Vector4 Vector4::xyyy() const  { return Vector4       (x, y, y, y); }
VectorSwizzle4 Vector4::xyyy() { return VectorSwizzle4(x, y, y, y); }
Vector4 Vector4::yyyy() const  { return Vector4       (y, y, y, y); }
VectorSwizzle4 Vector4::yyyy() { return VectorSwizzle4(y, y, y, y); }
Vector4 Vector4::zyyy() const  { return Vector4       (z, y, y, y); }
VectorSwizzle4 Vector4::zyyy() { return VectorSwizzle4(z, y, y, y); }
Vector4 Vector4::wyyy() const  { return Vector4       (w, y, y, y); }
VectorSwizzle4 Vector4::wyyy() { return VectorSwizzle4(w, y, y, y); }
Vector4 Vector4::xzyy() const  { return Vector4       (x, z, y, y); }
VectorSwizzle4 Vector4::xzyy() { return VectorSwizzle4(x, z, y, y); }
Vector4 Vector4::yzyy() const  { return Vector4       (y, z, y, y); }
VectorSwizzle4 Vector4::yzyy() { return VectorSwizzle4(y, z, y, y); }
Vector4 Vector4::zzyy() const  { return Vector4       (z, z, y, y); }
VectorSwizzle4 Vector4::zzyy() { return VectorSwizzle4(z, z, y, y); }
Vector4 Vector4::wzyy() const  { return Vector4       (w, z, y, y); }
VectorSwizzle4 Vector4::wzyy() { return VectorSwizzle4(w, z, y, y); }
Vector4 Vector4::xwyy() const  { return Vector4       (x, w, y, y); }
VectorSwizzle4 Vector4::xwyy() { return VectorSwizzle4(x, w, y, y); }
Vector4 Vector4::ywyy() const  { return Vector4       (y, w, y, y); }
VectorSwizzle4 Vector4::ywyy() { return VectorSwizzle4(y, w, y, y); }
Vector4 Vector4::zwyy() const  { return Vector4       (z, w, y, y); }
VectorSwizzle4 Vector4::zwyy() { return VectorSwizzle4(z, w, y, y); }
Vector4 Vector4::wwyy() const  { return Vector4       (w, w, y, y); }
VectorSwizzle4 Vector4::wwyy() { return VectorSwizzle4(w, w, y, y); }
Vector4 Vector4::xxzy() const  { return Vector4       (x, x, z, y); }
VectorSwizzle4 Vector4::xxzy() { return VectorSwizzle4(x, x, z, y); }
Vector4 Vector4::yxzy() const  { return Vector4       (y, x, z, y); }
VectorSwizzle4 Vector4::yxzy() { return VectorSwizzle4(y, x, z, y); }
Vector4 Vector4::zxzy() const  { return Vector4       (z, x, z, y); }
VectorSwizzle4 Vector4::zxzy() { return VectorSwizzle4(z, x, z, y); }
Vector4 Vector4::wxzy() const  { return Vector4       (w, x, z, y); }
VectorSwizzle4 Vector4::wxzy() { return VectorSwizzle4(w, x, z, y); }
Vector4 Vector4::xyzy() const  { return Vector4       (x, y, z, y); }
VectorSwizzle4 Vector4::xyzy() { return VectorSwizzle4(x, y, z, y); }
Vector4 Vector4::yyzy() const  { return Vector4       (y, y, z, y); }
VectorSwizzle4 Vector4::yyzy() { return VectorSwizzle4(y, y, z, y); }
Vector4 Vector4::zyzy() const  { return Vector4       (z, y, z, y); }
VectorSwizzle4 Vector4::zyzy() { return VectorSwizzle4(z, y, z, y); }
Vector4 Vector4::wyzy() const  { return Vector4       (w, y, z, y); }
VectorSwizzle4 Vector4::wyzy() { return VectorSwizzle4(w, y, z, y); }
Vector4 Vector4::xzzy() const  { return Vector4       (x, z, z, y); }
VectorSwizzle4 Vector4::xzzy() { return VectorSwizzle4(x, z, z, y); }
Vector4 Vector4::yzzy() const  { return Vector4       (y, z, z, y); }
VectorSwizzle4 Vector4::yzzy() { return VectorSwizzle4(y, z, z, y); }
Vector4 Vector4::zzzy() const  { return Vector4       (z, z, z, y); }
VectorSwizzle4 Vector4::zzzy() { return VectorSwizzle4(z, z, z, y); }
Vector4 Vector4::wzzy() const  { return Vector4       (w, z, z, y); }
VectorSwizzle4 Vector4::wzzy() { return VectorSwizzle4(w, z, z, y); }
Vector4 Vector4::xwzy() const  { return Vector4       (x, w, z, y); }
VectorSwizzle4 Vector4::xwzy() { return VectorSwizzle4(x, w, z, y); }
Vector4 Vector4::ywzy() const  { return Vector4       (y, w, z, y); }
VectorSwizzle4 Vector4::ywzy() { return VectorSwizzle4(y, w, z, y); }
Vector4 Vector4::zwzy() const  { return Vector4       (z, w, z, y); }
VectorSwizzle4 Vector4::zwzy() { return VectorSwizzle4(z, w, z, y); }
Vector4 Vector4::wwzy() const  { return Vector4       (w, w, z, y); }
VectorSwizzle4 Vector4::wwzy() { return VectorSwizzle4(w, w, z, y); }
Vector4 Vector4::xxwy() const  { return Vector4       (x, x, w, y); }
VectorSwizzle4 Vector4::xxwy() { return VectorSwizzle4(x, x, w, y); }
Vector4 Vector4::yxwy() const  { return Vector4       (y, x, w, y); }
VectorSwizzle4 Vector4::yxwy() { return VectorSwizzle4(y, x, w, y); }
Vector4 Vector4::zxwy() const  { return Vector4       (z, x, w, y); }
VectorSwizzle4 Vector4::zxwy() { return VectorSwizzle4(z, x, w, y); }
Vector4 Vector4::wxwy() const  { return Vector4       (w, x, w, y); }
VectorSwizzle4 Vector4::wxwy() { return VectorSwizzle4(w, x, w, y); }
Vector4 Vector4::xywy() const  { return Vector4       (x, y, w, y); }
VectorSwizzle4 Vector4::xywy() { return VectorSwizzle4(x, y, w, y); }
Vector4 Vector4::yywy() const  { return Vector4       (y, y, w, y); }
VectorSwizzle4 Vector4::yywy() { return VectorSwizzle4(y, y, w, y); }
Vector4 Vector4::zywy() const  { return Vector4       (z, y, w, y); }
VectorSwizzle4 Vector4::zywy() { return VectorSwizzle4(z, y, w, y); }
Vector4 Vector4::wywy() const  { return Vector4       (w, y, w, y); }
VectorSwizzle4 Vector4::wywy() { return VectorSwizzle4(w, y, w, y); }
Vector4 Vector4::xzwy() const  { return Vector4       (x, z, w, y); }
VectorSwizzle4 Vector4::xzwy() { return VectorSwizzle4(x, z, w, y); }
Vector4 Vector4::yzwy() const  { return Vector4       (y, z, w, y); }
VectorSwizzle4 Vector4::yzwy() { return VectorSwizzle4(y, z, w, y); }
Vector4 Vector4::zzwy() const  { return Vector4       (z, z, w, y); }
VectorSwizzle4 Vector4::zzwy() { return VectorSwizzle4(z, z, w, y); }
Vector4 Vector4::wzwy() const  { return Vector4       (w, z, w, y); }
VectorSwizzle4 Vector4::wzwy() { return VectorSwizzle4(w, z, w, y); }
Vector4 Vector4::xwwy() const  { return Vector4       (x, w, w, y); }
VectorSwizzle4 Vector4::xwwy() { return VectorSwizzle4(x, w, w, y); }
Vector4 Vector4::ywwy() const  { return Vector4       (y, w, w, y); }
VectorSwizzle4 Vector4::ywwy() { return VectorSwizzle4(y, w, w, y); }
Vector4 Vector4::zwwy() const  { return Vector4       (z, w, w, y); }
VectorSwizzle4 Vector4::zwwy() { return VectorSwizzle4(z, w, w, y); }
Vector4 Vector4::wwwy() const  { return Vector4       (w, w, w, y); }
VectorSwizzle4 Vector4::wwwy() { return VectorSwizzle4(w, w, w, y); }
Vector4 Vector4::xxxz() const  { return Vector4       (x, x, x, z); }
VectorSwizzle4 Vector4::xxxz() { return VectorSwizzle4(x, x, x, z); }
Vector4 Vector4::yxxz() const  { return Vector4       (y, x, x, z); }
VectorSwizzle4 Vector4::yxxz() { return VectorSwizzle4(y, x, x, z); }
Vector4 Vector4::zxxz() const  { return Vector4       (z, x, x, z); }
VectorSwizzle4 Vector4::zxxz() { return VectorSwizzle4(z, x, x, z); }
Vector4 Vector4::wxxz() const  { return Vector4       (w, x, x, z); }
VectorSwizzle4 Vector4::wxxz() { return VectorSwizzle4(w, x, x, z); }
Vector4 Vector4::xyxz() const  { return Vector4       (x, y, x, z); }
VectorSwizzle4 Vector4::xyxz() { return VectorSwizzle4(x, y, x, z); }
Vector4 Vector4::yyxz() const  { return Vector4       (y, y, x, z); }
VectorSwizzle4 Vector4::yyxz() { return VectorSwizzle4(y, y, x, z); }
Vector4 Vector4::zyxz() const  { return Vector4       (z, y, x, z); }
VectorSwizzle4 Vector4::zyxz() { return VectorSwizzle4(z, y, x, z); }
Vector4 Vector4::wyxz() const  { return Vector4       (w, y, x, z); }
VectorSwizzle4 Vector4::wyxz() { return VectorSwizzle4(w, y, x, z); }
Vector4 Vector4::xzxz() const  { return Vector4       (x, z, x, z); }
VectorSwizzle4 Vector4::xzxz() { return VectorSwizzle4(x, z, x, z); }
Vector4 Vector4::yzxz() const  { return Vector4       (y, z, x, z); }
VectorSwizzle4 Vector4::yzxz() { return VectorSwizzle4(y, z, x, z); }
Vector4 Vector4::zzxz() const  { return Vector4       (z, z, x, z); }
VectorSwizzle4 Vector4::zzxz() { return VectorSwizzle4(z, z, x, z); }
Vector4 Vector4::wzxz() const  { return Vector4       (w, z, x, z); }
VectorSwizzle4 Vector4::wzxz() { return VectorSwizzle4(w, z, x, z); }
Vector4 Vector4::xwxz() const  { return Vector4       (x, w, x, z); }
VectorSwizzle4 Vector4::xwxz() { return VectorSwizzle4(x, w, x, z); }
Vector4 Vector4::ywxz() const  { return Vector4       (y, w, x, z); }
VectorSwizzle4 Vector4::ywxz() { return VectorSwizzle4(y, w, x, z); }
Vector4 Vector4::zwxz() const  { return Vector4       (z, w, x, z); }
VectorSwizzle4 Vector4::zwxz() { return VectorSwizzle4(z, w, x, z); }
Vector4 Vector4::wwxz() const  { return Vector4       (w, w, x, z); }
VectorSwizzle4 Vector4::wwxz() { return VectorSwizzle4(w, w, x, z); }
Vector4 Vector4::xxyz() const  { return Vector4       (x, x, y, z); }
VectorSwizzle4 Vector4::xxyz() { return VectorSwizzle4(x, x, y, z); }
Vector4 Vector4::yxyz() const  { return Vector4       (y, x, y, z); }
VectorSwizzle4 Vector4::yxyz() { return VectorSwizzle4(y, x, y, z); }
Vector4 Vector4::zxyz() const  { return Vector4       (z, x, y, z); }
VectorSwizzle4 Vector4::zxyz() { return VectorSwizzle4(z, x, y, z); }
Vector4 Vector4::wxyz() const  { return Vector4       (w, x, y, z); }
VectorSwizzle4 Vector4::wxyz() { return VectorSwizzle4(w, x, y, z); }
Vector4 Vector4::xyyz() const  { return Vector4       (x, y, y, z); }
VectorSwizzle4 Vector4::xyyz() { return VectorSwizzle4(x, y, y, z); }
Vector4 Vector4::yyyz() const  { return Vector4       (y, y, y, z); }
VectorSwizzle4 Vector4::yyyz() { return VectorSwizzle4(y, y, y, z); }
Vector4 Vector4::zyyz() const  { return Vector4       (z, y, y, z); }
VectorSwizzle4 Vector4::zyyz() { return VectorSwizzle4(z, y, y, z); }
Vector4 Vector4::wyyz() const  { return Vector4       (w, y, y, z); }
VectorSwizzle4 Vector4::wyyz() { return VectorSwizzle4(w, y, y, z); }
Vector4 Vector4::xzyz() const  { return Vector4       (x, z, y, z); }
VectorSwizzle4 Vector4::xzyz() { return VectorSwizzle4(x, z, y, z); }
Vector4 Vector4::yzyz() const  { return Vector4       (y, z, y, z); }
VectorSwizzle4 Vector4::yzyz() { return VectorSwizzle4(y, z, y, z); }
Vector4 Vector4::zzyz() const  { return Vector4       (z, z, y, z); }
VectorSwizzle4 Vector4::zzyz() { return VectorSwizzle4(z, z, y, z); }
Vector4 Vector4::wzyz() const  { return Vector4       (w, z, y, z); }
VectorSwizzle4 Vector4::wzyz() { return VectorSwizzle4(w, z, y, z); }
Vector4 Vector4::xwyz() const  { return Vector4       (x, w, y, z); }
VectorSwizzle4 Vector4::xwyz() { return VectorSwizzle4(x, w, y, z); }
Vector4 Vector4::ywyz() const  { return Vector4       (y, w, y, z); }
VectorSwizzle4 Vector4::ywyz() { return VectorSwizzle4(y, w, y, z); }
Vector4 Vector4::zwyz() const  { return Vector4       (z, w, y, z); }
VectorSwizzle4 Vector4::zwyz() { return VectorSwizzle4(z, w, y, z); }
Vector4 Vector4::wwyz() const  { return Vector4       (w, w, y, z); }
VectorSwizzle4 Vector4::wwyz() { return VectorSwizzle4(w, w, y, z); }
Vector4 Vector4::xxzz() const  { return Vector4       (x, x, z, z); }
VectorSwizzle4 Vector4::xxzz() { return VectorSwizzle4(x, x, z, z); }
Vector4 Vector4::yxzz() const  { return Vector4       (y, x, z, z); }
VectorSwizzle4 Vector4::yxzz() { return VectorSwizzle4(y, x, z, z); }
Vector4 Vector4::zxzz() const  { return Vector4       (z, x, z, z); }
VectorSwizzle4 Vector4::zxzz() { return VectorSwizzle4(z, x, z, z); }
Vector4 Vector4::wxzz() const  { return Vector4       (w, x, z, z); }
VectorSwizzle4 Vector4::wxzz() { return VectorSwizzle4(w, x, z, z); }
Vector4 Vector4::xyzz() const  { return Vector4       (x, y, z, z); }
VectorSwizzle4 Vector4::xyzz() { return VectorSwizzle4(x, y, z, z); }
Vector4 Vector4::yyzz() const  { return Vector4       (y, y, z, z); }
VectorSwizzle4 Vector4::yyzz() { return VectorSwizzle4(y, y, z, z); }
Vector4 Vector4::zyzz() const  { return Vector4       (z, y, z, z); }
VectorSwizzle4 Vector4::zyzz() { return VectorSwizzle4(z, y, z, z); }
Vector4 Vector4::wyzz() const  { return Vector4       (w, y, z, z); }
VectorSwizzle4 Vector4::wyzz() { return VectorSwizzle4(w, y, z, z); }
Vector4 Vector4::xzzz() const  { return Vector4       (x, z, z, z); }
VectorSwizzle4 Vector4::xzzz() { return VectorSwizzle4(x, z, z, z); }
Vector4 Vector4::yzzz() const  { return Vector4       (y, z, z, z); }
VectorSwizzle4 Vector4::yzzz() { return VectorSwizzle4(y, z, z, z); }
Vector4 Vector4::zzzz() const  { return Vector4       (z, z, z, z); }
VectorSwizzle4 Vector4::zzzz() { return VectorSwizzle4(z, z, z, z); }
Vector4 Vector4::wzzz() const  { return Vector4       (w, z, z, z); }
VectorSwizzle4 Vector4::wzzz() { return VectorSwizzle4(w, z, z, z); }
Vector4 Vector4::xwzz() const  { return Vector4       (x, w, z, z); }
VectorSwizzle4 Vector4::xwzz() { return VectorSwizzle4(x, w, z, z); }
Vector4 Vector4::ywzz() const  { return Vector4       (y, w, z, z); }
VectorSwizzle4 Vector4::ywzz() { return VectorSwizzle4(y, w, z, z); }
Vector4 Vector4::zwzz() const  { return Vector4       (z, w, z, z); }
VectorSwizzle4 Vector4::zwzz() { return VectorSwizzle4(z, w, z, z); }
Vector4 Vector4::wwzz() const  { return Vector4       (w, w, z, z); }
VectorSwizzle4 Vector4::wwzz() { return VectorSwizzle4(w, w, z, z); }
Vector4 Vector4::xxwz() const  { return Vector4       (x, x, w, z); }
VectorSwizzle4 Vector4::xxwz() { return VectorSwizzle4(x, x, w, z); }
Vector4 Vector4::yxwz() const  { return Vector4       (y, x, w, z); }
VectorSwizzle4 Vector4::yxwz() { return VectorSwizzle4(y, x, w, z); }
Vector4 Vector4::zxwz() const  { return Vector4       (z, x, w, z); }
VectorSwizzle4 Vector4::zxwz() { return VectorSwizzle4(z, x, w, z); }
Vector4 Vector4::wxwz() const  { return Vector4       (w, x, w, z); }
VectorSwizzle4 Vector4::wxwz() { return VectorSwizzle4(w, x, w, z); }
Vector4 Vector4::xywz() const  { return Vector4       (x, y, w, z); }
VectorSwizzle4 Vector4::xywz() { return VectorSwizzle4(x, y, w, z); }
Vector4 Vector4::yywz() const  { return Vector4       (y, y, w, z); }
VectorSwizzle4 Vector4::yywz() { return VectorSwizzle4(y, y, w, z); }
Vector4 Vector4::zywz() const  { return Vector4       (z, y, w, z); }
VectorSwizzle4 Vector4::zywz() { return VectorSwizzle4(z, y, w, z); }
Vector4 Vector4::wywz() const  { return Vector4       (w, y, w, z); }
VectorSwizzle4 Vector4::wywz() { return VectorSwizzle4(w, y, w, z); }
Vector4 Vector4::xzwz() const  { return Vector4       (x, z, w, z); }
VectorSwizzle4 Vector4::xzwz() { return VectorSwizzle4(x, z, w, z); }
Vector4 Vector4::yzwz() const  { return Vector4       (y, z, w, z); }
VectorSwizzle4 Vector4::yzwz() { return VectorSwizzle4(y, z, w, z); }
Vector4 Vector4::zzwz() const  { return Vector4       (z, z, w, z); }
VectorSwizzle4 Vector4::zzwz() { return VectorSwizzle4(z, z, w, z); }
Vector4 Vector4::wzwz() const  { return Vector4       (w, z, w, z); }
VectorSwizzle4 Vector4::wzwz() { return VectorSwizzle4(w, z, w, z); }
Vector4 Vector4::xwwz() const  { return Vector4       (x, w, w, z); }
VectorSwizzle4 Vector4::xwwz() { return VectorSwizzle4(x, w, w, z); }
Vector4 Vector4::ywwz() const  { return Vector4       (y, w, w, z); }
VectorSwizzle4 Vector4::ywwz() { return VectorSwizzle4(y, w, w, z); }
Vector4 Vector4::zwwz() const  { return Vector4       (z, w, w, z); }
VectorSwizzle4 Vector4::zwwz() { return VectorSwizzle4(z, w, w, z); }
Vector4 Vector4::wwwz() const  { return Vector4       (w, w, w, z); }
VectorSwizzle4 Vector4::wwwz() { return VectorSwizzle4(w, w, w, z); }
Vector4 Vector4::xxxw() const  { return Vector4       (x, x, x, w); }
VectorSwizzle4 Vector4::xxxw() { return VectorSwizzle4(x, x, x, w); }
Vector4 Vector4::yxxw() const  { return Vector4       (y, x, x, w); }
VectorSwizzle4 Vector4::yxxw() { return VectorSwizzle4(y, x, x, w); }
Vector4 Vector4::zxxw() const  { return Vector4       (z, x, x, w); }
VectorSwizzle4 Vector4::zxxw() { return VectorSwizzle4(z, x, x, w); }
Vector4 Vector4::wxxw() const  { return Vector4       (w, x, x, w); }
VectorSwizzle4 Vector4::wxxw() { return VectorSwizzle4(w, x, x, w); }
Vector4 Vector4::xyxw() const  { return Vector4       (x, y, x, w); }
VectorSwizzle4 Vector4::xyxw() { return VectorSwizzle4(x, y, x, w); }
Vector4 Vector4::yyxw() const  { return Vector4       (y, y, x, w); }
VectorSwizzle4 Vector4::yyxw() { return VectorSwizzle4(y, y, x, w); }
Vector4 Vector4::zyxw() const  { return Vector4       (z, y, x, w); }
VectorSwizzle4 Vector4::zyxw() { return VectorSwizzle4(z, y, x, w); }
Vector4 Vector4::wyxw() const  { return Vector4       (w, y, x, w); }
VectorSwizzle4 Vector4::wyxw() { return VectorSwizzle4(w, y, x, w); }
Vector4 Vector4::xzxw() const  { return Vector4       (x, z, x, w); }
VectorSwizzle4 Vector4::xzxw() { return VectorSwizzle4(x, z, x, w); }
Vector4 Vector4::yzxw() const  { return Vector4       (y, z, x, w); }
VectorSwizzle4 Vector4::yzxw() { return VectorSwizzle4(y, z, x, w); }
Vector4 Vector4::zzxw() const  { return Vector4       (z, z, x, w); }
VectorSwizzle4 Vector4::zzxw() { return VectorSwizzle4(z, z, x, w); }
Vector4 Vector4::wzxw() const  { return Vector4       (w, z, x, w); }
VectorSwizzle4 Vector4::wzxw() { return VectorSwizzle4(w, z, x, w); }
Vector4 Vector4::xwxw() const  { return Vector4       (x, w, x, w); }
VectorSwizzle4 Vector4::xwxw() { return VectorSwizzle4(x, w, x, w); }
Vector4 Vector4::ywxw() const  { return Vector4       (y, w, x, w); }
VectorSwizzle4 Vector4::ywxw() { return VectorSwizzle4(y, w, x, w); }
Vector4 Vector4::zwxw() const  { return Vector4       (z, w, x, w); }
VectorSwizzle4 Vector4::zwxw() { return VectorSwizzle4(z, w, x, w); }
Vector4 Vector4::wwxw() const  { return Vector4       (w, w, x, w); }
VectorSwizzle4 Vector4::wwxw() { return VectorSwizzle4(w, w, x, w); }
Vector4 Vector4::xxyw() const  { return Vector4       (x, x, y, w); }
VectorSwizzle4 Vector4::xxyw() { return VectorSwizzle4(x, x, y, w); }
Vector4 Vector4::yxyw() const  { return Vector4       (y, x, y, w); }
VectorSwizzle4 Vector4::yxyw() { return VectorSwizzle4(y, x, y, w); }
Vector4 Vector4::zxyw() const  { return Vector4       (z, x, y, w); }
VectorSwizzle4 Vector4::zxyw() { return VectorSwizzle4(z, x, y, w); }
Vector4 Vector4::wxyw() const  { return Vector4       (w, x, y, w); }
VectorSwizzle4 Vector4::wxyw() { return VectorSwizzle4(w, x, y, w); }
Vector4 Vector4::xyyw() const  { return Vector4       (x, y, y, w); }
VectorSwizzle4 Vector4::xyyw() { return VectorSwizzle4(x, y, y, w); }
Vector4 Vector4::yyyw() const  { return Vector4       (y, y, y, w); }
VectorSwizzle4 Vector4::yyyw() { return VectorSwizzle4(y, y, y, w); }
Vector4 Vector4::zyyw() const  { return Vector4       (z, y, y, w); }
VectorSwizzle4 Vector4::zyyw() { return VectorSwizzle4(z, y, y, w); }
Vector4 Vector4::wyyw() const  { return Vector4       (w, y, y, w); }
VectorSwizzle4 Vector4::wyyw() { return VectorSwizzle4(w, y, y, w); }
Vector4 Vector4::xzyw() const  { return Vector4       (x, z, y, w); }
VectorSwizzle4 Vector4::xzyw() { return VectorSwizzle4(x, z, y, w); }
Vector4 Vector4::yzyw() const  { return Vector4       (y, z, y, w); }
VectorSwizzle4 Vector4::yzyw() { return VectorSwizzle4(y, z, y, w); }
Vector4 Vector4::zzyw() const  { return Vector4       (z, z, y, w); }
VectorSwizzle4 Vector4::zzyw() { return VectorSwizzle4(z, z, y, w); }
Vector4 Vector4::wzyw() const  { return Vector4       (w, z, y, w); }
VectorSwizzle4 Vector4::wzyw() { return VectorSwizzle4(w, z, y, w); }
Vector4 Vector4::xwyw() const  { return Vector4       (x, w, y, w); }
VectorSwizzle4 Vector4::xwyw() { return VectorSwizzle4(x, w, y, w); }
Vector4 Vector4::ywyw() const  { return Vector4       (y, w, y, w); }
VectorSwizzle4 Vector4::ywyw() { return VectorSwizzle4(y, w, y, w); }
Vector4 Vector4::zwyw() const  { return Vector4       (z, w, y, w); }
VectorSwizzle4 Vector4::zwyw() { return VectorSwizzle4(z, w, y, w); }
Vector4 Vector4::wwyw() const  { return Vector4       (w, w, y, w); }
VectorSwizzle4 Vector4::wwyw() { return VectorSwizzle4(w, w, y, w); }
Vector4 Vector4::xxzw() const  { return Vector4       (x, x, z, w); }
VectorSwizzle4 Vector4::xxzw() { return VectorSwizzle4(x, x, z, w); }
Vector4 Vector4::yxzw() const  { return Vector4       (y, x, z, w); }
VectorSwizzle4 Vector4::yxzw() { return VectorSwizzle4(y, x, z, w); }
Vector4 Vector4::zxzw() const  { return Vector4       (z, x, z, w); }
VectorSwizzle4 Vector4::zxzw() { return VectorSwizzle4(z, x, z, w); }
Vector4 Vector4::wxzw() const  { return Vector4       (w, x, z, w); }
VectorSwizzle4 Vector4::wxzw() { return VectorSwizzle4(w, x, z, w); }
Vector4 Vector4::xyzw() const  { return Vector4       (x, y, z, w); }
VectorSwizzle4 Vector4::xyzw() { return VectorSwizzle4(x, y, z, w); }
Vector4 Vector4::yyzw() const  { return Vector4       (y, y, z, w); }
VectorSwizzle4 Vector4::yyzw() { return VectorSwizzle4(y, y, z, w); }
Vector4 Vector4::zyzw() const  { return Vector4       (z, y, z, w); }
VectorSwizzle4 Vector4::zyzw() { return VectorSwizzle4(z, y, z, w); }
Vector4 Vector4::wyzw() const  { return Vector4       (w, y, z, w); }
VectorSwizzle4 Vector4::wyzw() { return VectorSwizzle4(w, y, z, w); }
Vector4 Vector4::xzzw() const  { return Vector4       (x, z, z, w); }
VectorSwizzle4 Vector4::xzzw() { return VectorSwizzle4(x, z, z, w); }
Vector4 Vector4::yzzw() const  { return Vector4       (y, z, z, w); }
VectorSwizzle4 Vector4::yzzw() { return VectorSwizzle4(y, z, z, w); }
Vector4 Vector4::zzzw() const  { return Vector4       (z, z, z, w); }
VectorSwizzle4 Vector4::zzzw() { return VectorSwizzle4(z, z, z, w); }
Vector4 Vector4::wzzw() const  { return Vector4       (w, z, z, w); }
VectorSwizzle4 Vector4::wzzw() { return VectorSwizzle4(w, z, z, w); }
Vector4 Vector4::xwzw() const  { return Vector4       (x, w, z, w); }
VectorSwizzle4 Vector4::xwzw() { return VectorSwizzle4(x, w, z, w); }
Vector4 Vector4::ywzw() const  { return Vector4       (y, w, z, w); }
VectorSwizzle4 Vector4::ywzw() { return VectorSwizzle4(y, w, z, w); }
Vector4 Vector4::zwzw() const  { return Vector4       (z, w, z, w); }
VectorSwizzle4 Vector4::zwzw() { return VectorSwizzle4(z, w, z, w); }
Vector4 Vector4::wwzw() const  { return Vector4       (w, w, z, w); }
VectorSwizzle4 Vector4::wwzw() { return VectorSwizzle4(w, w, z, w); }
Vector4 Vector4::xxww() const  { return Vector4       (x, x, w, w); }
VectorSwizzle4 Vector4::xxww() { return VectorSwizzle4(x, x, w, w); }
Vector4 Vector4::yxww() const  { return Vector4       (y, x, w, w); }
VectorSwizzle4 Vector4::yxww() { return VectorSwizzle4(y, x, w, w); }
Vector4 Vector4::zxww() const  { return Vector4       (z, x, w, w); }
VectorSwizzle4 Vector4::zxww() { return VectorSwizzle4(z, x, w, w); }
Vector4 Vector4::wxww() const  { return Vector4       (w, x, w, w); }
VectorSwizzle4 Vector4::wxww() { return VectorSwizzle4(w, x, w, w); }
Vector4 Vector4::xyww() const  { return Vector4       (x, y, w, w); }
VectorSwizzle4 Vector4::xyww() { return VectorSwizzle4(x, y, w, w); }
Vector4 Vector4::yyww() const  { return Vector4       (y, y, w, w); }
VectorSwizzle4 Vector4::yyww() { return VectorSwizzle4(y, y, w, w); }
Vector4 Vector4::zyww() const  { return Vector4       (z, y, w, w); }
VectorSwizzle4 Vector4::zyww() { return VectorSwizzle4(z, y, w, w); }
Vector4 Vector4::wyww() const  { return Vector4       (w, y, w, w); }
VectorSwizzle4 Vector4::wyww() { return VectorSwizzle4(w, y, w, w); }
Vector4 Vector4::xzww() const  { return Vector4       (x, z, w, w); }
VectorSwizzle4 Vector4::xzww() { return VectorSwizzle4(x, z, w, w); }
Vector4 Vector4::yzww() const  { return Vector4       (y, z, w, w); }
VectorSwizzle4 Vector4::yzww() { return VectorSwizzle4(y, z, w, w); }
Vector4 Vector4::zzww() const  { return Vector4       (z, z, w, w); }
VectorSwizzle4 Vector4::zzww() { return VectorSwizzle4(z, z, w, w); }
Vector4 Vector4::wzww() const  { return Vector4       (w, z, w, w); }
VectorSwizzle4 Vector4::wzww() { return VectorSwizzle4(w, z, w, w); }
Vector4 Vector4::xwww() const  { return Vector4       (x, w, w, w); }
VectorSwizzle4 Vector4::xwww() { return VectorSwizzle4(x, w, w, w); }
Vector4 Vector4::ywww() const  { return Vector4       (y, w, w, w); }
VectorSwizzle4 Vector4::ywww() { return VectorSwizzle4(y, w, w, w); }
Vector4 Vector4::zwww() const  { return Vector4       (z, w, w, w); }
VectorSwizzle4 Vector4::zwww() { return VectorSwizzle4(z, w, w, w); }
Vector4 Vector4::wwww() const  { return Vector4       (w, w, w, w); }
VectorSwizzle4 Vector4::wwww() { return VectorSwizzle4(w, w, w, w); }

}; // namespace
