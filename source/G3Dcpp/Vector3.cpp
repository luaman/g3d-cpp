/**
 @file Vector3.cpp
 
 3D vector class
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com
 
 @created 2001-06-02
 @edited  2003-09-29
 */

#include <limits>
#include <stdlib.h>
#include "G3D/Vector3.h"
#include "G3D/g3dmath.h"
#include "G3D/stringutils.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/Vector3int16.h"
#include "G3D/VectorSwizzle.h"

namespace G3D {

const Vector3 Vector3::ZERO(0, 0, 0);
const Vector3 Vector3::ZERO3(0, 0, 0);
const Vector3 Vector3::UNIT_X(1, 0, 0);
const Vector3 Vector3::UNIT_Y(0, 1, 0);
const Vector3 Vector3::UNIT_Z(0, 0, 1);
const Vector3 Vector3::INF3(infReal, infReal, infReal);
const Vector3 Vector3::NAN3(nan, nan, nan);

Vector3::Axis Vector3::primaryAxis() const {
    
    Axis a = X_AXIS;

    double nx = abs(x);
    double ny = abs(y);
    double nz = abs(z);

    if (nx > ny) {
        if (nx > nz) {
            a = X_AXIS;
        } else {
            a = Z_AXIS;
        }
    } else {
        if (ny > nz) {
            a = Y_AXIS;
        } else {
            a = Z_AXIS;
        }
    }

    return a;
}


unsigned int Vector3::hashCode() const {
    unsigned int xhash = (*(int*)(void*)(&x));
    unsigned int yhash = (*(int*)(void*)(&y));
    unsigned int zhash = (*(int*)(void*)(&z));

    return xhash + (yhash * 37) + (zhash * 101);
}

//----------------------------------------------------------------------------

double frand() {
    return rand() / (double) RAND_MAX;
}


Vector3::Vector3(BinaryInput& b) {
    deserialize(b);
}


Vector3::Vector3(const class Vector3int16& v) {
    x = v.x;
    y = v.y;
    z = v.z;
}


void Vector3::deserialize(BinaryInput& b) {
    x = b.readFloat32();
    y = b.readFloat32();
    z = b.readFloat32();
}


void Vector3::serialize(BinaryOutput& b) const {
    b.writeFloat32(x);
    b.writeFloat32(y);
    b.writeFloat32(z);
}


Vector3 Vector3::random() {
    Vector3 result;

    do {
        result = Vector3(symmetricRandom(), 
                         symmetricRandom(),
                         symmetricRandom());
    } while ((result.squaredLength() > 1) || (result.squaredLength() == 1.0));

    result.unitize();

    return result;
}

//----------------------------------------------------------------------------
Vector3 Vector3::operator/ (G3D::Real fScalar) const {
    Vector3 kQuot;

    if ( fScalar != 0.0 ) {
		G3D::Real fInvScalar = 1.0 / fScalar;
        kQuot.x = fInvScalar * x;
        kQuot.y = fInvScalar * y;
        kQuot.z = fInvScalar * z;
        return kQuot;
    } else {
        return Vector3(G3D::infReal, G3D::infReal,
                       G3D::infReal);
    }
}

//----------------------------------------------------------------------------
Vector3& Vector3::operator/= (G3D::Real fScalar) {
    if (fScalar != 0.0) {
		G3D::Real fInvScalar = 1.0 / fScalar;
        x *= fInvScalar;
        y *= fInvScalar;
        z *= fInvScalar;
    } else {
        x = G3D::infReal;
        y = G3D::infReal;
        z = G3D::infReal;
    }

    return *this;
}

//----------------------------------------------------------------------------
G3D::Real Vector3::unitize (G3D::Real fTolerance) {
	G3D::Real fLength = length();

    if (fLength > fTolerance) {
		G3D::Real fInvLength = 1.0 / fLength;
        x *= fInvLength;
        y *= fInvLength;
        z *= fInvLength;
    } else {
        fLength = 0.0;
    }

    return fLength;
}

//----------------------------------------------------------------------------
void Vector3::orthonormalize (Vector3 akVector[3]) {
    // If the input vectors are v0, v1, and v2, then the Gram-Schmidt
    // orthonormalization produces vectors u0, u1, and u2 as follows,
    //
    //   u0 = v0/|v0|
    //   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
    //   u2 = (v2-(u0*v2)u0-(u1*v2)u1)/|v2-(u0*v2)u0-(u1*v2)u1|
    //
    // where |A| indicates length of vector A and A*B indicates dot
    // product of vectors A and B.

    // compute u0
    akVector[0].unitize();

    // compute u1
	G3D::Real fDot0 = akVector[0].dot(akVector[1]);
    akVector[1] -= fDot0 * akVector[0];
    akVector[1].unitize();

    // compute u2
	G3D::Real fDot1 = akVector[1].dot(akVector[2]);
    fDot0 = akVector[0].dot(akVector[2]);
    akVector[2] -= fDot0 * akVector[0] + fDot1 * akVector[1];
    akVector[2].unitize();
}

//----------------------------------------------------------------------------
void Vector3::generateOrthonormalBasis (Vector3& rkU, Vector3& rkV,
                                        Vector3& rkW, bool bUnitLengthW) {
    if ( !bUnitLengthW )
        rkW.unitize();

    if ( G3D::abs(rkW.x) >= G3D::abs(rkW.y)
            && G3D::abs(rkW.x) >= G3D::abs(rkW.z) ) {
        rkU.x = -rkW.y;
        rkU.y = + rkW.x;
        rkU.z = 0.0;
    } else {
        rkU.x = 0.0;
        rkU.y = + rkW.z;
        rkU.z = -rkW.y;
    }

    rkU.unitize();
    rkV = rkW.cross(rkU);
}

//----------------------------------------------------------------------------

std::string Vector3::toString() const {
    return G3D::format("(%g, %g, %g)", x, y, z);
}
//----------------------------------------------------------------------------
// 2-char swizzles

Vector2 Vector3::xx() const  { return Vector2       (x, x); }
VectorSwizzle2 Vector3::xx() { return VectorSwizzle2(x, x); }
Vector2 Vector3::yx() const  { return Vector2       (y, x); }
VectorSwizzle2 Vector3::yx() { return VectorSwizzle2(y, x); }
Vector2 Vector3::zx() const  { return Vector2       (z, x); }
VectorSwizzle2 Vector3::zx() { return VectorSwizzle2(z, x); }
Vector2 Vector3::xy() const  { return Vector2       (x, y); }
VectorSwizzle2 Vector3::xy() { return VectorSwizzle2(x, y); }
Vector2 Vector3::yy() const  { return Vector2       (y, y); }
VectorSwizzle2 Vector3::yy() { return VectorSwizzle2(y, y); }
Vector2 Vector3::zy() const  { return Vector2       (z, y); }
VectorSwizzle2 Vector3::zy() { return VectorSwizzle2(z, y); }
Vector2 Vector3::xz() const  { return Vector2       (x, z); }
VectorSwizzle2 Vector3::xz() { return VectorSwizzle2(x, z); }
Vector2 Vector3::yz() const  { return Vector2       (y, z); }
VectorSwizzle2 Vector3::yz() { return VectorSwizzle2(y, z); }
Vector2 Vector3::zz() const  { return Vector2       (z, z); }
VectorSwizzle2 Vector3::zz() { return VectorSwizzle2(z, z); }

// 3-char swizzles

Vector3 Vector3::xxx() const  { return Vector3       (x, x, x); }
VectorSwizzle3 Vector3::xxx() { return VectorSwizzle3(x, x, x); }
Vector3 Vector3::yxx() const  { return Vector3       (y, x, x); }
VectorSwizzle3 Vector3::yxx() { return VectorSwizzle3(y, x, x); }
Vector3 Vector3::zxx() const  { return Vector3       (z, x, x); }
VectorSwizzle3 Vector3::zxx() { return VectorSwizzle3(z, x, x); }
Vector3 Vector3::xyx() const  { return Vector3       (x, y, x); }
VectorSwizzle3 Vector3::xyx() { return VectorSwizzle3(x, y, x); }
Vector3 Vector3::yyx() const  { return Vector3       (y, y, x); }
VectorSwizzle3 Vector3::yyx() { return VectorSwizzle3(y, y, x); }
Vector3 Vector3::zyx() const  { return Vector3       (z, y, x); }
VectorSwizzle3 Vector3::zyx() { return VectorSwizzle3(z, y, x); }
Vector3 Vector3::xzx() const  { return Vector3       (x, z, x); }
VectorSwizzle3 Vector3::xzx() { return VectorSwizzle3(x, z, x); }
Vector3 Vector3::yzx() const  { return Vector3       (y, z, x); }
VectorSwizzle3 Vector3::yzx() { return VectorSwizzle3(y, z, x); }
Vector3 Vector3::zzx() const  { return Vector3       (z, z, x); }
VectorSwizzle3 Vector3::zzx() { return VectorSwizzle3(z, z, x); }
Vector3 Vector3::xxy() const  { return Vector3       (x, x, y); }
VectorSwizzle3 Vector3::xxy() { return VectorSwizzle3(x, x, y); }
Vector3 Vector3::yxy() const  { return Vector3       (y, x, y); }
VectorSwizzle3 Vector3::yxy() { return VectorSwizzle3(y, x, y); }
Vector3 Vector3::zxy() const  { return Vector3       (z, x, y); }
VectorSwizzle3 Vector3::zxy() { return VectorSwizzle3(z, x, y); }
Vector3 Vector3::xyy() const  { return Vector3       (x, y, y); }
VectorSwizzle3 Vector3::xyy() { return VectorSwizzle3(x, y, y); }
Vector3 Vector3::yyy() const  { return Vector3       (y, y, y); }
VectorSwizzle3 Vector3::yyy() { return VectorSwizzle3(y, y, y); }
Vector3 Vector3::zyy() const  { return Vector3       (z, y, y); }
VectorSwizzle3 Vector3::zyy() { return VectorSwizzle3(z, y, y); }
Vector3 Vector3::xzy() const  { return Vector3       (x, z, y); }
VectorSwizzle3 Vector3::xzy() { return VectorSwizzle3(x, z, y); }
Vector3 Vector3::yzy() const  { return Vector3       (y, z, y); }
VectorSwizzle3 Vector3::yzy() { return VectorSwizzle3(y, z, y); }
Vector3 Vector3::zzy() const  { return Vector3       (z, z, y); }
VectorSwizzle3 Vector3::zzy() { return VectorSwizzle3(z, z, y); }
Vector3 Vector3::xxz() const  { return Vector3       (x, x, z); }
VectorSwizzle3 Vector3::xxz() { return VectorSwizzle3(x, x, z); }
Vector3 Vector3::yxz() const  { return Vector3       (y, x, z); }
VectorSwizzle3 Vector3::yxz() { return VectorSwizzle3(y, x, z); }
Vector3 Vector3::zxz() const  { return Vector3       (z, x, z); }
VectorSwizzle3 Vector3::zxz() { return VectorSwizzle3(z, x, z); }
Vector3 Vector3::xyz() const  { return Vector3       (x, y, z); }
VectorSwizzle3 Vector3::xyz() { return VectorSwizzle3(x, y, z); }
Vector3 Vector3::yyz() const  { return Vector3       (y, y, z); }
VectorSwizzle3 Vector3::yyz() { return VectorSwizzle3(y, y, z); }
Vector3 Vector3::zyz() const  { return Vector3       (z, y, z); }
VectorSwizzle3 Vector3::zyz() { return VectorSwizzle3(z, y, z); }
Vector3 Vector3::xzz() const  { return Vector3       (x, z, z); }
VectorSwizzle3 Vector3::xzz() { return VectorSwizzle3(x, z, z); }
Vector3 Vector3::yzz() const  { return Vector3       (y, z, z); }
VectorSwizzle3 Vector3::yzz() { return VectorSwizzle3(y, z, z); }
Vector3 Vector3::zzz() const  { return Vector3       (z, z, z); }
VectorSwizzle3 Vector3::zzz() { return VectorSwizzle3(z, z, z); }

// 4-char swizzles

Vector4 Vector3::xxxx() const  { return Vector4       (x, x, x, x); }
VectorSwizzle4 Vector3::xxxx() { return VectorSwizzle4(x, x, x, x); }
Vector4 Vector3::yxxx() const  { return Vector4       (y, x, x, x); }
VectorSwizzle4 Vector3::yxxx() { return VectorSwizzle4(y, x, x, x); }
Vector4 Vector3::zxxx() const  { return Vector4       (z, x, x, x); }
VectorSwizzle4 Vector3::zxxx() { return VectorSwizzle4(z, x, x, x); }
Vector4 Vector3::xyxx() const  { return Vector4       (x, y, x, x); }
VectorSwizzle4 Vector3::xyxx() { return VectorSwizzle4(x, y, x, x); }
Vector4 Vector3::yyxx() const  { return Vector4       (y, y, x, x); }
VectorSwizzle4 Vector3::yyxx() { return VectorSwizzle4(y, y, x, x); }
Vector4 Vector3::zyxx() const  { return Vector4       (z, y, x, x); }
VectorSwizzle4 Vector3::zyxx() { return VectorSwizzle4(z, y, x, x); }
Vector4 Vector3::xzxx() const  { return Vector4       (x, z, x, x); }
VectorSwizzle4 Vector3::xzxx() { return VectorSwizzle4(x, z, x, x); }
Vector4 Vector3::yzxx() const  { return Vector4       (y, z, x, x); }
VectorSwizzle4 Vector3::yzxx() { return VectorSwizzle4(y, z, x, x); }
Vector4 Vector3::zzxx() const  { return Vector4       (z, z, x, x); }
VectorSwizzle4 Vector3::zzxx() { return VectorSwizzle4(z, z, x, x); }
Vector4 Vector3::xxyx() const  { return Vector4       (x, x, y, x); }
VectorSwizzle4 Vector3::xxyx() { return VectorSwizzle4(x, x, y, x); }
Vector4 Vector3::yxyx() const  { return Vector4       (y, x, y, x); }
VectorSwizzle4 Vector3::yxyx() { return VectorSwizzle4(y, x, y, x); }
Vector4 Vector3::zxyx() const  { return Vector4       (z, x, y, x); }
VectorSwizzle4 Vector3::zxyx() { return VectorSwizzle4(z, x, y, x); }
Vector4 Vector3::xyyx() const  { return Vector4       (x, y, y, x); }
VectorSwizzle4 Vector3::xyyx() { return VectorSwizzle4(x, y, y, x); }
Vector4 Vector3::yyyx() const  { return Vector4       (y, y, y, x); }
VectorSwizzle4 Vector3::yyyx() { return VectorSwizzle4(y, y, y, x); }
Vector4 Vector3::zyyx() const  { return Vector4       (z, y, y, x); }
VectorSwizzle4 Vector3::zyyx() { return VectorSwizzle4(z, y, y, x); }
Vector4 Vector3::xzyx() const  { return Vector4       (x, z, y, x); }
VectorSwizzle4 Vector3::xzyx() { return VectorSwizzle4(x, z, y, x); }
Vector4 Vector3::yzyx() const  { return Vector4       (y, z, y, x); }
VectorSwizzle4 Vector3::yzyx() { return VectorSwizzle4(y, z, y, x); }
Vector4 Vector3::zzyx() const  { return Vector4       (z, z, y, x); }
VectorSwizzle4 Vector3::zzyx() { return VectorSwizzle4(z, z, y, x); }
Vector4 Vector3::xxzx() const  { return Vector4       (x, x, z, x); }
VectorSwizzle4 Vector3::xxzx() { return VectorSwizzle4(x, x, z, x); }
Vector4 Vector3::yxzx() const  { return Vector4       (y, x, z, x); }
VectorSwizzle4 Vector3::yxzx() { return VectorSwizzle4(y, x, z, x); }
Vector4 Vector3::zxzx() const  { return Vector4       (z, x, z, x); }
VectorSwizzle4 Vector3::zxzx() { return VectorSwizzle4(z, x, z, x); }
Vector4 Vector3::xyzx() const  { return Vector4       (x, y, z, x); }
VectorSwizzle4 Vector3::xyzx() { return VectorSwizzle4(x, y, z, x); }
Vector4 Vector3::yyzx() const  { return Vector4       (y, y, z, x); }
VectorSwizzle4 Vector3::yyzx() { return VectorSwizzle4(y, y, z, x); }
Vector4 Vector3::zyzx() const  { return Vector4       (z, y, z, x); }
VectorSwizzle4 Vector3::zyzx() { return VectorSwizzle4(z, y, z, x); }
Vector4 Vector3::xzzx() const  { return Vector4       (x, z, z, x); }
VectorSwizzle4 Vector3::xzzx() { return VectorSwizzle4(x, z, z, x); }
Vector4 Vector3::yzzx() const  { return Vector4       (y, z, z, x); }
VectorSwizzle4 Vector3::yzzx() { return VectorSwizzle4(y, z, z, x); }
Vector4 Vector3::zzzx() const  { return Vector4       (z, z, z, x); }
VectorSwizzle4 Vector3::zzzx() { return VectorSwizzle4(z, z, z, x); }
Vector4 Vector3::xxxy() const  { return Vector4       (x, x, x, y); }
VectorSwizzle4 Vector3::xxxy() { return VectorSwizzle4(x, x, x, y); }
Vector4 Vector3::yxxy() const  { return Vector4       (y, x, x, y); }
VectorSwizzle4 Vector3::yxxy() { return VectorSwizzle4(y, x, x, y); }
Vector4 Vector3::zxxy() const  { return Vector4       (z, x, x, y); }
VectorSwizzle4 Vector3::zxxy() { return VectorSwizzle4(z, x, x, y); }
Vector4 Vector3::xyxy() const  { return Vector4       (x, y, x, y); }
VectorSwizzle4 Vector3::xyxy() { return VectorSwizzle4(x, y, x, y); }
Vector4 Vector3::yyxy() const  { return Vector4       (y, y, x, y); }
VectorSwizzle4 Vector3::yyxy() { return VectorSwizzle4(y, y, x, y); }
Vector4 Vector3::zyxy() const  { return Vector4       (z, y, x, y); }
VectorSwizzle4 Vector3::zyxy() { return VectorSwizzle4(z, y, x, y); }
Vector4 Vector3::xzxy() const  { return Vector4       (x, z, x, y); }
VectorSwizzle4 Vector3::xzxy() { return VectorSwizzle4(x, z, x, y); }
Vector4 Vector3::yzxy() const  { return Vector4       (y, z, x, y); }
VectorSwizzle4 Vector3::yzxy() { return VectorSwizzle4(y, z, x, y); }
Vector4 Vector3::zzxy() const  { return Vector4       (z, z, x, y); }
VectorSwizzle4 Vector3::zzxy() { return VectorSwizzle4(z, z, x, y); }
Vector4 Vector3::xxyy() const  { return Vector4       (x, x, y, y); }
VectorSwizzle4 Vector3::xxyy() { return VectorSwizzle4(x, x, y, y); }
Vector4 Vector3::yxyy() const  { return Vector4       (y, x, y, y); }
VectorSwizzle4 Vector3::yxyy() { return VectorSwizzle4(y, x, y, y); }
Vector4 Vector3::zxyy() const  { return Vector4       (z, x, y, y); }
VectorSwizzle4 Vector3::zxyy() { return VectorSwizzle4(z, x, y, y); }
Vector4 Vector3::xyyy() const  { return Vector4       (x, y, y, y); }
VectorSwizzle4 Vector3::xyyy() { return VectorSwizzle4(x, y, y, y); }
Vector4 Vector3::yyyy() const  { return Vector4       (y, y, y, y); }
VectorSwizzle4 Vector3::yyyy() { return VectorSwizzle4(y, y, y, y); }
Vector4 Vector3::zyyy() const  { return Vector4       (z, y, y, y); }
VectorSwizzle4 Vector3::zyyy() { return VectorSwizzle4(z, y, y, y); }
Vector4 Vector3::xzyy() const  { return Vector4       (x, z, y, y); }
VectorSwizzle4 Vector3::xzyy() { return VectorSwizzle4(x, z, y, y); }
Vector4 Vector3::yzyy() const  { return Vector4       (y, z, y, y); }
VectorSwizzle4 Vector3::yzyy() { return VectorSwizzle4(y, z, y, y); }
Vector4 Vector3::zzyy() const  { return Vector4       (z, z, y, y); }
VectorSwizzle4 Vector3::zzyy() { return VectorSwizzle4(z, z, y, y); }
Vector4 Vector3::xxzy() const  { return Vector4       (x, x, z, y); }
VectorSwizzle4 Vector3::xxzy() { return VectorSwizzle4(x, x, z, y); }
Vector4 Vector3::yxzy() const  { return Vector4       (y, x, z, y); }
VectorSwizzle4 Vector3::yxzy() { return VectorSwizzle4(y, x, z, y); }
Vector4 Vector3::zxzy() const  { return Vector4       (z, x, z, y); }
VectorSwizzle4 Vector3::zxzy() { return VectorSwizzle4(z, x, z, y); }
Vector4 Vector3::xyzy() const  { return Vector4       (x, y, z, y); }
VectorSwizzle4 Vector3::xyzy() { return VectorSwizzle4(x, y, z, y); }
Vector4 Vector3::yyzy() const  { return Vector4       (y, y, z, y); }
VectorSwizzle4 Vector3::yyzy() { return VectorSwizzle4(y, y, z, y); }
Vector4 Vector3::zyzy() const  { return Vector4       (z, y, z, y); }
VectorSwizzle4 Vector3::zyzy() { return VectorSwizzle4(z, y, z, y); }
Vector4 Vector3::xzzy() const  { return Vector4       (x, z, z, y); }
VectorSwizzle4 Vector3::xzzy() { return VectorSwizzle4(x, z, z, y); }
Vector4 Vector3::yzzy() const  { return Vector4       (y, z, z, y); }
VectorSwizzle4 Vector3::yzzy() { return VectorSwizzle4(y, z, z, y); }
Vector4 Vector3::zzzy() const  { return Vector4       (z, z, z, y); }
VectorSwizzle4 Vector3::zzzy() { return VectorSwizzle4(z, z, z, y); }
Vector4 Vector3::xxxz() const  { return Vector4       (x, x, x, z); }
VectorSwizzle4 Vector3::xxxz() { return VectorSwizzle4(x, x, x, z); }
Vector4 Vector3::yxxz() const  { return Vector4       (y, x, x, z); }
VectorSwizzle4 Vector3::yxxz() { return VectorSwizzle4(y, x, x, z); }
Vector4 Vector3::zxxz() const  { return Vector4       (z, x, x, z); }
VectorSwizzle4 Vector3::zxxz() { return VectorSwizzle4(z, x, x, z); }
Vector4 Vector3::xyxz() const  { return Vector4       (x, y, x, z); }
VectorSwizzle4 Vector3::xyxz() { return VectorSwizzle4(x, y, x, z); }
Vector4 Vector3::yyxz() const  { return Vector4       (y, y, x, z); }
VectorSwizzle4 Vector3::yyxz() { return VectorSwizzle4(y, y, x, z); }
Vector4 Vector3::zyxz() const  { return Vector4       (z, y, x, z); }
VectorSwizzle4 Vector3::zyxz() { return VectorSwizzle4(z, y, x, z); }
Vector4 Vector3::xzxz() const  { return Vector4       (x, z, x, z); }
VectorSwizzle4 Vector3::xzxz() { return VectorSwizzle4(x, z, x, z); }
Vector4 Vector3::yzxz() const  { return Vector4       (y, z, x, z); }
VectorSwizzle4 Vector3::yzxz() { return VectorSwizzle4(y, z, x, z); }
Vector4 Vector3::zzxz() const  { return Vector4       (z, z, x, z); }
VectorSwizzle4 Vector3::zzxz() { return VectorSwizzle4(z, z, x, z); }
Vector4 Vector3::xxyz() const  { return Vector4       (x, x, y, z); }
VectorSwizzle4 Vector3::xxyz() { return VectorSwizzle4(x, x, y, z); }
Vector4 Vector3::yxyz() const  { return Vector4       (y, x, y, z); }
VectorSwizzle4 Vector3::yxyz() { return VectorSwizzle4(y, x, y, z); }
Vector4 Vector3::zxyz() const  { return Vector4       (z, x, y, z); }
VectorSwizzle4 Vector3::zxyz() { return VectorSwizzle4(z, x, y, z); }
Vector4 Vector3::xyyz() const  { return Vector4       (x, y, y, z); }
VectorSwizzle4 Vector3::xyyz() { return VectorSwizzle4(x, y, y, z); }
Vector4 Vector3::yyyz() const  { return Vector4       (y, y, y, z); }
VectorSwizzle4 Vector3::yyyz() { return VectorSwizzle4(y, y, y, z); }
Vector4 Vector3::zyyz() const  { return Vector4       (z, y, y, z); }
VectorSwizzle4 Vector3::zyyz() { return VectorSwizzle4(z, y, y, z); }
Vector4 Vector3::xzyz() const  { return Vector4       (x, z, y, z); }
VectorSwizzle4 Vector3::xzyz() { return VectorSwizzle4(x, z, y, z); }
Vector4 Vector3::yzyz() const  { return Vector4       (y, z, y, z); }
VectorSwizzle4 Vector3::yzyz() { return VectorSwizzle4(y, z, y, z); }
Vector4 Vector3::zzyz() const  { return Vector4       (z, z, y, z); }
VectorSwizzle4 Vector3::zzyz() { return VectorSwizzle4(z, z, y, z); }
Vector4 Vector3::xxzz() const  { return Vector4       (x, x, z, z); }
VectorSwizzle4 Vector3::xxzz() { return VectorSwizzle4(x, x, z, z); }
Vector4 Vector3::yxzz() const  { return Vector4       (y, x, z, z); }
VectorSwizzle4 Vector3::yxzz() { return VectorSwizzle4(y, x, z, z); }
Vector4 Vector3::zxzz() const  { return Vector4       (z, x, z, z); }
VectorSwizzle4 Vector3::zxzz() { return VectorSwizzle4(z, x, z, z); }
Vector4 Vector3::xyzz() const  { return Vector4       (x, y, z, z); }
VectorSwizzle4 Vector3::xyzz() { return VectorSwizzle4(x, y, z, z); }
Vector4 Vector3::yyzz() const  { return Vector4       (y, y, z, z); }
VectorSwizzle4 Vector3::yyzz() { return VectorSwizzle4(y, y, z, z); }
Vector4 Vector3::zyzz() const  { return Vector4       (z, y, z, z); }
VectorSwizzle4 Vector3::zyzz() { return VectorSwizzle4(z, y, z, z); }
Vector4 Vector3::xzzz() const  { return Vector4       (x, z, z, z); }
VectorSwizzle4 Vector3::xzzz() { return VectorSwizzle4(x, z, z, z); }
Vector4 Vector3::yzzz() const  { return Vector4       (y, z, z, z); }
VectorSwizzle4 Vector3::yzzz() { return VectorSwizzle4(y, z, z, z); }
Vector4 Vector3::zzzz() const  { return Vector4       (z, z, z, z); }
VectorSwizzle4 Vector3::zzzz() { return VectorSwizzle4(z, z, z, z); }






} // namespace
