/**
  @file Vector4.h
 
  Homogeneous vector class.
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @created 2002-07-09
  @edited  2003-09-29
 */

#ifndef G3D_VECTOR4_H
#define G3D_VECTOR4_H

#include "G3D/g3dmath.h"
#include "G3D/Vector3.h"
#include "G3D/Vector2.h"
#include <string>

namespace G3D {

class Vector2;    
class Vector3;
class Vector4;
class VectorSwizzle2;
class VectorSwizzle3;
class VectorSwizzle4;

/**
 Do not subclass-- this implementation makes assumptions about the
 memory layout.
 */
class Vector4 {
    
public:
    // construction
    Vector4();
    Vector4(float fX, float fY, float fZ, float fW);
    Vector4(float afCoordinate[4]);
    Vector4(const Vector4& rkVector);
    Vector4(const class Color4& c);
    Vector4(const Vector3& rkVector, float fW);
    Vector4(const Vector2& v1, const Vector2& v2);
    Vector4(const Vector2& v1, float fz, float fw);

    Vector4(const VectorSwizzle2& v1, const VectorSwizzle2& v2);
    Vector4(const VectorSwizzle3& rkVector, float fW);

    Vector4(class BinaryInput& b);
    void serialize(class BinaryOutput& b) const;
    void deserialize(class BinaryInput& b);

    // coordinates
    float x, y, z, w;

    // access vector V as V[0] = V.x, V[1] = V.y, V[2] = V.z, etc.
    //
    // WARNING.  These member functions rely on
    // (1) Vector4 not having virtual functions
    // (2) the data packed in a 4*sizeof(float) memory block
    float& operator[] (int i) const; 
    operator float* ();
    operator const float* () const;

    // assignment and comparison
    Vector4& operator= (const Vector4& rkVector);
    bool operator== (const Vector4& rkVector) const;
    bool operator!= (const Vector4& rkVector) const;
    unsigned int hashCode() const;
    bool fuzzyEq(const Vector4& other) const;
    bool fuzzyNe(const Vector4& other) const;

    /** sqrt(this->dot(*this)) */
    double length() const;
    double squaredLength() const;

    /** Returns true if this vector has finite length */
    bool isFinite() const;

    /** Returns true if this vector has length == 0 */
    bool isZero() const;

    /** Returns true if this vector has length == 1 */
    bool isUnit() const;

    // arithmetic operations
    Vector4 operator+ (const Vector4& rkVector) const;
    Vector4 operator- (const Vector4& rkVector) const;
    Vector4 operator* (float fScalar) const;
    Vector4 operator/ (float fScalar) const;
    Vector4 operator- () const;
    friend Vector4 operator* (float fScalar, const Vector4& rkVector);

    // arithmetic updates
    Vector4& operator+= (const Vector4& rkVector);
    Vector4& operator-= (const Vector4& rkVector);
    Vector4& operator*= (float fScalar);
    Vector4& operator/= (float fScalar);

    inline Vector4 clamp(const Vector4& low, const Vector4& high) const {
        return Vector4(
            G3D::clamp(x, low.x, high.x),
            G3D::clamp(y, low.y, high.y),
            G3D::clamp(z, low.z, high.z),
            G3D::clamp(w, low.w, high.w));
    }

    inline Vector4 clamp(double low, double high) const {
        return Vector4(
            G3D::clamp(x, low, high),
            G3D::clamp(y, low, high),
            G3D::clamp(z, low, high),
            G3D::clamp(w, low, high));
    }

    float dot (const Vector4& rkVector) const;

    Vector4 min(const Vector4& v) const;
    Vector4 max(const Vector4& v) const;

    std::string toString() const;

    /**
     Linear interpolation
     */
    Vector4 lerp(const Vector4& v, double alpha) const;

    // 2-char swizzles

    Vector2 xx() const;
    VectorSwizzle2 xx();
    Vector2 yx() const;
    VectorSwizzle2 yx();
    Vector2 zx() const;
    VectorSwizzle2 zx();
    Vector2 wx() const;
    VectorSwizzle2 wx();
    Vector2 xy() const;
    VectorSwizzle2 xy();
    Vector2 yy() const;
    VectorSwizzle2 yy();
    Vector2 zy() const;
    VectorSwizzle2 zy();
    Vector2 wy() const;
    VectorSwizzle2 wy();
    Vector2 xz() const;
    VectorSwizzle2 xz();
    Vector2 yz() const;
    VectorSwizzle2 yz();
    Vector2 zz() const;
    VectorSwizzle2 zz();
    Vector2 wz() const;
    VectorSwizzle2 wz();
    Vector2 xw() const;
    VectorSwizzle2 xw();
    Vector2 yw() const;
    VectorSwizzle2 yw();
    Vector2 zw() const;
    VectorSwizzle2 zw();
    Vector2 ww() const;
    VectorSwizzle2 ww();

    // 3-char swizzles

    Vector3 xxx() const;
    VectorSwizzle3 xxx();
    Vector3 yxx() const;
    VectorSwizzle3 yxx();
    Vector3 zxx() const;
    VectorSwizzle3 zxx();
    Vector3 wxx() const;
    VectorSwizzle3 wxx();
    Vector3 xyx() const;
    VectorSwizzle3 xyx();
    Vector3 yyx() const;
    VectorSwizzle3 yyx();
    Vector3 zyx() const;
    VectorSwizzle3 zyx();
    Vector3 wyx() const;
    VectorSwizzle3 wyx();
    Vector3 xzx() const;
    VectorSwizzle3 xzx();
    Vector3 yzx() const;
    VectorSwizzle3 yzx();
    Vector3 zzx() const;
    VectorSwizzle3 zzx();
    Vector3 wzx() const;
    VectorSwizzle3 wzx();
    Vector3 xwx() const;
    VectorSwizzle3 xwx();
    Vector3 ywx() const;
    VectorSwizzle3 ywx();
    Vector3 zwx() const;
    VectorSwizzle3 zwx();
    Vector3 wwx() const;
    VectorSwizzle3 wwx();
    Vector3 xxy() const;
    VectorSwizzle3 xxy();
    Vector3 yxy() const;
    VectorSwizzle3 yxy();
    Vector3 zxy() const;
    VectorSwizzle3 zxy();
    Vector3 wxy() const;
    VectorSwizzle3 wxy();
    Vector3 xyy() const;
    VectorSwizzle3 xyy();
    Vector3 yyy() const;
    VectorSwizzle3 yyy();
    Vector3 zyy() const;
    VectorSwizzle3 zyy();
    Vector3 wyy() const;
    VectorSwizzle3 wyy();
    Vector3 xzy() const;
    VectorSwizzle3 xzy();
    Vector3 yzy() const;
    VectorSwizzle3 yzy();
    Vector3 zzy() const;
    VectorSwizzle3 zzy();
    Vector3 wzy() const;
    VectorSwizzle3 wzy();
    Vector3 xwy() const;
    VectorSwizzle3 xwy();
    Vector3 ywy() const;
    VectorSwizzle3 ywy();
    Vector3 zwy() const;
    VectorSwizzle3 zwy();
    Vector3 wwy() const;
    VectorSwizzle3 wwy();
    Vector3 xxz() const;
    VectorSwizzle3 xxz();
    Vector3 yxz() const;
    VectorSwizzle3 yxz();
    Vector3 zxz() const;
    VectorSwizzle3 zxz();
    Vector3 wxz() const;
    VectorSwizzle3 wxz();
    Vector3 xyz() const;
    VectorSwizzle3 xyz();
    Vector3 yyz() const;
    VectorSwizzle3 yyz();
    Vector3 zyz() const;
    VectorSwizzle3 zyz();
    Vector3 wyz() const;
    VectorSwizzle3 wyz();
    Vector3 xzz() const;
    VectorSwizzle3 xzz();
    Vector3 yzz() const;
    VectorSwizzle3 yzz();
    Vector3 zzz() const;
    VectorSwizzle3 zzz();
    Vector3 wzz() const;
    VectorSwizzle3 wzz();
    Vector3 xwz() const;
    VectorSwizzle3 xwz();
    Vector3 ywz() const;
    VectorSwizzle3 ywz();
    Vector3 zwz() const;
    VectorSwizzle3 zwz();
    Vector3 wwz() const;
    VectorSwizzle3 wwz();
    Vector3 xxw() const;
    VectorSwizzle3 xxw();
    Vector3 yxw() const;
    VectorSwizzle3 yxw();
    Vector3 zxw() const;
    VectorSwizzle3 zxw();
    Vector3 wxw() const;
    VectorSwizzle3 wxw();
    Vector3 xyw() const;
    VectorSwizzle3 xyw();
    Vector3 yyw() const;
    VectorSwizzle3 yyw();
    Vector3 zyw() const;
    VectorSwizzle3 zyw();
    Vector3 wyw() const;
    VectorSwizzle3 wyw();
    Vector3 xzw() const;
    VectorSwizzle3 xzw();
    Vector3 yzw() const;
    VectorSwizzle3 yzw();
    Vector3 zzw() const;
    VectorSwizzle3 zzw();
    Vector3 wzw() const;
    VectorSwizzle3 wzw();
    Vector3 xww() const;
    VectorSwizzle3 xww();
    Vector3 yww() const;
    VectorSwizzle3 yww();
    Vector3 zww() const;
    VectorSwizzle3 zww();
    Vector3 www() const;
    VectorSwizzle3 www();

    // 4-char swizzles

    Vector4 xxxx() const;
    VectorSwizzle4 xxxx();
    Vector4 yxxx() const;
    VectorSwizzle4 yxxx();
    Vector4 zxxx() const;
    VectorSwizzle4 zxxx();
    Vector4 wxxx() const;
    VectorSwizzle4 wxxx();
    Vector4 xyxx() const;
    VectorSwizzle4 xyxx();
    Vector4 yyxx() const;
    VectorSwizzle4 yyxx();
    Vector4 zyxx() const;
    VectorSwizzle4 zyxx();
    Vector4 wyxx() const;
    VectorSwizzle4 wyxx();
    Vector4 xzxx() const;
    VectorSwizzle4 xzxx();
    Vector4 yzxx() const;
    VectorSwizzle4 yzxx();
    Vector4 zzxx() const;
    VectorSwizzle4 zzxx();
    Vector4 wzxx() const;
    VectorSwizzle4 wzxx();
    Vector4 xwxx() const;
    VectorSwizzle4 xwxx();
    Vector4 ywxx() const;
    VectorSwizzle4 ywxx();
    Vector4 zwxx() const;
    VectorSwizzle4 zwxx();
    Vector4 wwxx() const;
    VectorSwizzle4 wwxx();
    Vector4 xxyx() const;
    VectorSwizzle4 xxyx();
    Vector4 yxyx() const;
    VectorSwizzle4 yxyx();
    Vector4 zxyx() const;
    VectorSwizzle4 zxyx();
    Vector4 wxyx() const;
    VectorSwizzle4 wxyx();
    Vector4 xyyx() const;
    VectorSwizzle4 xyyx();
    Vector4 yyyx() const;
    VectorSwizzle4 yyyx();
    Vector4 zyyx() const;
    VectorSwizzle4 zyyx();
    Vector4 wyyx() const;
    VectorSwizzle4 wyyx();
    Vector4 xzyx() const;
    VectorSwizzle4 xzyx();
    Vector4 yzyx() const;
    VectorSwizzle4 yzyx();
    Vector4 zzyx() const;
    VectorSwizzle4 zzyx();
    Vector4 wzyx() const;
    VectorSwizzle4 wzyx();
    Vector4 xwyx() const;
    VectorSwizzle4 xwyx();
    Vector4 ywyx() const;
    VectorSwizzle4 ywyx();
    Vector4 zwyx() const;
    VectorSwizzle4 zwyx();
    Vector4 wwyx() const;
    VectorSwizzle4 wwyx();
    Vector4 xxzx() const;
    VectorSwizzle4 xxzx();
    Vector4 yxzx() const;
    VectorSwizzle4 yxzx();
    Vector4 zxzx() const;
    VectorSwizzle4 zxzx();
    Vector4 wxzx() const;
    VectorSwizzle4 wxzx();
    Vector4 xyzx() const;
    VectorSwizzle4 xyzx();
    Vector4 yyzx() const;
    VectorSwizzle4 yyzx();
    Vector4 zyzx() const;
    VectorSwizzle4 zyzx();
    Vector4 wyzx() const;
    VectorSwizzle4 wyzx();
    Vector4 xzzx() const;
    VectorSwizzle4 xzzx();
    Vector4 yzzx() const;
    VectorSwizzle4 yzzx();
    Vector4 zzzx() const;
    VectorSwizzle4 zzzx();
    Vector4 wzzx() const;
    VectorSwizzle4 wzzx();
    Vector4 xwzx() const;
    VectorSwizzle4 xwzx();
    Vector4 ywzx() const;
    VectorSwizzle4 ywzx();
    Vector4 zwzx() const;
    VectorSwizzle4 zwzx();
    Vector4 wwzx() const;
    VectorSwizzle4 wwzx();
    Vector4 xxwx() const;
    VectorSwizzle4 xxwx();
    Vector4 yxwx() const;
    VectorSwizzle4 yxwx();
    Vector4 zxwx() const;
    VectorSwizzle4 zxwx();
    Vector4 wxwx() const;
    VectorSwizzle4 wxwx();
    Vector4 xywx() const;
    VectorSwizzle4 xywx();
    Vector4 yywx() const;
    VectorSwizzle4 yywx();
    Vector4 zywx() const;
    VectorSwizzle4 zywx();
    Vector4 wywx() const;
    VectorSwizzle4 wywx();
    Vector4 xzwx() const;
    VectorSwizzle4 xzwx();
    Vector4 yzwx() const;
    VectorSwizzle4 yzwx();
    Vector4 zzwx() const;
    VectorSwizzle4 zzwx();
    Vector4 wzwx() const;
    VectorSwizzle4 wzwx();
    Vector4 xwwx() const;
    VectorSwizzle4 xwwx();
    Vector4 ywwx() const;
    VectorSwizzle4 ywwx();
    Vector4 zwwx() const;
    VectorSwizzle4 zwwx();
    Vector4 wwwx() const;
    VectorSwizzle4 wwwx();
    Vector4 xxxy() const;
    VectorSwizzle4 xxxy();
    Vector4 yxxy() const;
    VectorSwizzle4 yxxy();
    Vector4 zxxy() const;
    VectorSwizzle4 zxxy();
    Vector4 wxxy() const;
    VectorSwizzle4 wxxy();
    Vector4 xyxy() const;
    VectorSwizzle4 xyxy();
    Vector4 yyxy() const;
    VectorSwizzle4 yyxy();
    Vector4 zyxy() const;
    VectorSwizzle4 zyxy();
    Vector4 wyxy() const;
    VectorSwizzle4 wyxy();
    Vector4 xzxy() const;
    VectorSwizzle4 xzxy();
    Vector4 yzxy() const;
    VectorSwizzle4 yzxy();
    Vector4 zzxy() const;
    VectorSwizzle4 zzxy();
    Vector4 wzxy() const;
    VectorSwizzle4 wzxy();
    Vector4 xwxy() const;
    VectorSwizzle4 xwxy();
    Vector4 ywxy() const;
    VectorSwizzle4 ywxy();
    Vector4 zwxy() const;
    VectorSwizzle4 zwxy();
    Vector4 wwxy() const;
    VectorSwizzle4 wwxy();
    Vector4 xxyy() const;
    VectorSwizzle4 xxyy();
    Vector4 yxyy() const;
    VectorSwizzle4 yxyy();
    Vector4 zxyy() const;
    VectorSwizzle4 zxyy();
    Vector4 wxyy() const;
    VectorSwizzle4 wxyy();
    Vector4 xyyy() const;
    VectorSwizzle4 xyyy();
    Vector4 yyyy() const;
    VectorSwizzle4 yyyy();
    Vector4 zyyy() const;
    VectorSwizzle4 zyyy();
    Vector4 wyyy() const;
    VectorSwizzle4 wyyy();
    Vector4 xzyy() const;
    VectorSwizzle4 xzyy();
    Vector4 yzyy() const;
    VectorSwizzle4 yzyy();
    Vector4 zzyy() const;
    VectorSwizzle4 zzyy();
    Vector4 wzyy() const;
    VectorSwizzle4 wzyy();
    Vector4 xwyy() const;
    VectorSwizzle4 xwyy();
    Vector4 ywyy() const;
    VectorSwizzle4 ywyy();
    Vector4 zwyy() const;
    VectorSwizzle4 zwyy();
    Vector4 wwyy() const;
    VectorSwizzle4 wwyy();
    Vector4 xxzy() const;
    VectorSwizzle4 xxzy();
    Vector4 yxzy() const;
    VectorSwizzle4 yxzy();
    Vector4 zxzy() const;
    VectorSwizzle4 zxzy();
    Vector4 wxzy() const;
    VectorSwizzle4 wxzy();
    Vector4 xyzy() const;
    VectorSwizzle4 xyzy();
    Vector4 yyzy() const;
    VectorSwizzle4 yyzy();
    Vector4 zyzy() const;
    VectorSwizzle4 zyzy();
    Vector4 wyzy() const;
    VectorSwizzle4 wyzy();
    Vector4 xzzy() const;
    VectorSwizzle4 xzzy();
    Vector4 yzzy() const;
    VectorSwizzle4 yzzy();
    Vector4 zzzy() const;
    VectorSwizzle4 zzzy();
    Vector4 wzzy() const;
    VectorSwizzle4 wzzy();
    Vector4 xwzy() const;
    VectorSwizzle4 xwzy();
    Vector4 ywzy() const;
    VectorSwizzle4 ywzy();
    Vector4 zwzy() const;
    VectorSwizzle4 zwzy();
    Vector4 wwzy() const;
    VectorSwizzle4 wwzy();
    Vector4 xxwy() const;
    VectorSwizzle4 xxwy();
    Vector4 yxwy() const;
    VectorSwizzle4 yxwy();
    Vector4 zxwy() const;
    VectorSwizzle4 zxwy();
    Vector4 wxwy() const;
    VectorSwizzle4 wxwy();
    Vector4 xywy() const;
    VectorSwizzle4 xywy();
    Vector4 yywy() const;
    VectorSwizzle4 yywy();
    Vector4 zywy() const;
    VectorSwizzle4 zywy();
    Vector4 wywy() const;
    VectorSwizzle4 wywy();
    Vector4 xzwy() const;
    VectorSwizzle4 xzwy();
    Vector4 yzwy() const;
    VectorSwizzle4 yzwy();
    Vector4 zzwy() const;
    VectorSwizzle4 zzwy();
    Vector4 wzwy() const;
    VectorSwizzle4 wzwy();
    Vector4 xwwy() const;
    VectorSwizzle4 xwwy();
    Vector4 ywwy() const;
    VectorSwizzle4 ywwy();
    Vector4 zwwy() const;
    VectorSwizzle4 zwwy();
    Vector4 wwwy() const;
    VectorSwizzle4 wwwy();
    Vector4 xxxz() const;
    VectorSwizzle4 xxxz();
    Vector4 yxxz() const;
    VectorSwizzle4 yxxz();
    Vector4 zxxz() const;
    VectorSwizzle4 zxxz();
    Vector4 wxxz() const;
    VectorSwizzle4 wxxz();
    Vector4 xyxz() const;
    VectorSwizzle4 xyxz();
    Vector4 yyxz() const;
    VectorSwizzle4 yyxz();
    Vector4 zyxz() const;
    VectorSwizzle4 zyxz();
    Vector4 wyxz() const;
    VectorSwizzle4 wyxz();
    Vector4 xzxz() const;
    VectorSwizzle4 xzxz();
    Vector4 yzxz() const;
    VectorSwizzle4 yzxz();
    Vector4 zzxz() const;
    VectorSwizzle4 zzxz();
    Vector4 wzxz() const;
    VectorSwizzle4 wzxz();
    Vector4 xwxz() const;
    VectorSwizzle4 xwxz();
    Vector4 ywxz() const;
    VectorSwizzle4 ywxz();
    Vector4 zwxz() const;
    VectorSwizzle4 zwxz();
    Vector4 wwxz() const;
    VectorSwizzle4 wwxz();
    Vector4 xxyz() const;
    VectorSwizzle4 xxyz();
    Vector4 yxyz() const;
    VectorSwizzle4 yxyz();
    Vector4 zxyz() const;
    VectorSwizzle4 zxyz();
    Vector4 wxyz() const;
    VectorSwizzle4 wxyz();
    Vector4 xyyz() const;
    VectorSwizzle4 xyyz();
    Vector4 yyyz() const;
    VectorSwizzle4 yyyz();
    Vector4 zyyz() const;
    VectorSwizzle4 zyyz();
    Vector4 wyyz() const;
    VectorSwizzle4 wyyz();
    Vector4 xzyz() const;
    VectorSwizzle4 xzyz();
    Vector4 yzyz() const;
    VectorSwizzle4 yzyz();
    Vector4 zzyz() const;
    VectorSwizzle4 zzyz();
    Vector4 wzyz() const;
    VectorSwizzle4 wzyz();
    Vector4 xwyz() const;
    VectorSwizzle4 xwyz();
    Vector4 ywyz() const;
    VectorSwizzle4 ywyz();
    Vector4 zwyz() const;
    VectorSwizzle4 zwyz();
    Vector4 wwyz() const;
    VectorSwizzle4 wwyz();
    Vector4 xxzz() const;
    VectorSwizzle4 xxzz();
    Vector4 yxzz() const;
    VectorSwizzle4 yxzz();
    Vector4 zxzz() const;
    VectorSwizzle4 zxzz();
    Vector4 wxzz() const;
    VectorSwizzle4 wxzz();
    Vector4 xyzz() const;
    VectorSwizzle4 xyzz();
    Vector4 yyzz() const;
    VectorSwizzle4 yyzz();
    Vector4 zyzz() const;
    VectorSwizzle4 zyzz();
    Vector4 wyzz() const;
    VectorSwizzle4 wyzz();
    Vector4 xzzz() const;
    VectorSwizzle4 xzzz();
    Vector4 yzzz() const;
    VectorSwizzle4 yzzz();
    Vector4 zzzz() const;
    VectorSwizzle4 zzzz();
    Vector4 wzzz() const;
    VectorSwizzle4 wzzz();
    Vector4 xwzz() const;
    VectorSwizzle4 xwzz();
    Vector4 ywzz() const;
    VectorSwizzle4 ywzz();
    Vector4 zwzz() const;
    VectorSwizzle4 zwzz();
    Vector4 wwzz() const;
    VectorSwizzle4 wwzz();
    Vector4 xxwz() const;
    VectorSwizzle4 xxwz();
    Vector4 yxwz() const;
    VectorSwizzle4 yxwz();
    Vector4 zxwz() const;
    VectorSwizzle4 zxwz();
    Vector4 wxwz() const;
    VectorSwizzle4 wxwz();
    Vector4 xywz() const;
    VectorSwizzle4 xywz();
    Vector4 yywz() const;
    VectorSwizzle4 yywz();
    Vector4 zywz() const;
    VectorSwizzle4 zywz();
    Vector4 wywz() const;
    VectorSwizzle4 wywz();
    Vector4 xzwz() const;
    VectorSwizzle4 xzwz();
    Vector4 yzwz() const;
    VectorSwizzle4 yzwz();
    Vector4 zzwz() const;
    VectorSwizzle4 zzwz();
    Vector4 wzwz() const;
    VectorSwizzle4 wzwz();
    Vector4 xwwz() const;
    VectorSwizzle4 xwwz();
    Vector4 ywwz() const;
    VectorSwizzle4 ywwz();
    Vector4 zwwz() const;
    VectorSwizzle4 zwwz();
    Vector4 wwwz() const;
    VectorSwizzle4 wwwz();
    Vector4 xxxw() const;
    VectorSwizzle4 xxxw();
    Vector4 yxxw() const;
    VectorSwizzle4 yxxw();
    Vector4 zxxw() const;
    VectorSwizzle4 zxxw();
    Vector4 wxxw() const;
    VectorSwizzle4 wxxw();
    Vector4 xyxw() const;
    VectorSwizzle4 xyxw();
    Vector4 yyxw() const;
    VectorSwizzle4 yyxw();
    Vector4 zyxw() const;
    VectorSwizzle4 zyxw();
    Vector4 wyxw() const;
    VectorSwizzle4 wyxw();
    Vector4 xzxw() const;
    VectorSwizzle4 xzxw();
    Vector4 yzxw() const;
    VectorSwizzle4 yzxw();
    Vector4 zzxw() const;
    VectorSwizzle4 zzxw();
    Vector4 wzxw() const;
    VectorSwizzle4 wzxw();
    Vector4 xwxw() const;
    VectorSwizzle4 xwxw();
    Vector4 ywxw() const;
    VectorSwizzle4 ywxw();
    Vector4 zwxw() const;
    VectorSwizzle4 zwxw();
    Vector4 wwxw() const;
    VectorSwizzle4 wwxw();
    Vector4 xxyw() const;
    VectorSwizzle4 xxyw();
    Vector4 yxyw() const;
    VectorSwizzle4 yxyw();
    Vector4 zxyw() const;
    VectorSwizzle4 zxyw();
    Vector4 wxyw() const;
    VectorSwizzle4 wxyw();
    Vector4 xyyw() const;
    VectorSwizzle4 xyyw();
    Vector4 yyyw() const;
    VectorSwizzle4 yyyw();
    Vector4 zyyw() const;
    VectorSwizzle4 zyyw();
    Vector4 wyyw() const;
    VectorSwizzle4 wyyw();
    Vector4 xzyw() const;
    VectorSwizzle4 xzyw();
    Vector4 yzyw() const;
    VectorSwizzle4 yzyw();
    Vector4 zzyw() const;
    VectorSwizzle4 zzyw();
    Vector4 wzyw() const;
    VectorSwizzle4 wzyw();
    Vector4 xwyw() const;
    VectorSwizzle4 xwyw();
    Vector4 ywyw() const;
    VectorSwizzle4 ywyw();
    Vector4 zwyw() const;
    VectorSwizzle4 zwyw();
    Vector4 wwyw() const;
    VectorSwizzle4 wwyw();
    Vector4 xxzw() const;
    VectorSwizzle4 xxzw();
    Vector4 yxzw() const;
    VectorSwizzle4 yxzw();
    Vector4 zxzw() const;
    VectorSwizzle4 zxzw();
    Vector4 wxzw() const;
    VectorSwizzle4 wxzw();
    Vector4 xyzw() const;
    VectorSwizzle4 xyzw();
    Vector4 yyzw() const;
    VectorSwizzle4 yyzw();
    Vector4 zyzw() const;
    VectorSwizzle4 zyzw();
    Vector4 wyzw() const;
    VectorSwizzle4 wyzw();
    Vector4 xzzw() const;
    VectorSwizzle4 xzzw();
    Vector4 yzzw() const;
    VectorSwizzle4 yzzw();
    Vector4 zzzw() const;
    VectorSwizzle4 zzzw();
    Vector4 wzzw() const;
    VectorSwizzle4 wzzw();
    Vector4 xwzw() const;
    VectorSwizzle4 xwzw();
    Vector4 ywzw() const;
    VectorSwizzle4 ywzw();
    Vector4 zwzw() const;
    VectorSwizzle4 zwzw();
    Vector4 wwzw() const;
    VectorSwizzle4 wwzw();
    Vector4 xxww() const;
    VectorSwizzle4 xxww();
    Vector4 yxww() const;
    VectorSwizzle4 yxww();
    Vector4 zxww() const;
    VectorSwizzle4 zxww();
    Vector4 wxww() const;
    VectorSwizzle4 wxww();
    Vector4 xyww() const;
    VectorSwizzle4 xyww();
    Vector4 yyww() const;
    VectorSwizzle4 yyww();
    Vector4 zyww() const;
    VectorSwizzle4 zyww();
    Vector4 wyww() const;
    VectorSwizzle4 wyww();
    Vector4 xzww() const;
    VectorSwizzle4 xzww();
    Vector4 yzww() const;
    VectorSwizzle4 yzww();
    Vector4 zzww() const;
    VectorSwizzle4 zzww();
    Vector4 wzww() const;
    VectorSwizzle4 wzww();
    Vector4 xwww() const;
    VectorSwizzle4 xwww();
    Vector4 ywww() const;
    VectorSwizzle4 ywww();
    Vector4 zwww() const;
    VectorSwizzle4 zwww();
    Vector4 wwww() const;
    VectorSwizzle4 wwww();


};

}

unsigned int hashCode(const G3D::Vector4& v);

#include "Vector4.inl"

#endif
