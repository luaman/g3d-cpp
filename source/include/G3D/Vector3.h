/**
  @file Vector3.h
 
  3D vector class
 
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com
 
  @created 2001-06-02
  @edited  2003-09-29
 */

#ifndef G3D_VECTOR3_H
#define G3D_VECTOR3_H

#include "G3D/g3dmath.h"
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
 Vector classes have swizzle operators, e.g. <CODE>v.xy()</CODE>, that
 allow selection of arbitrary sub-fields.  These can be used as write 
 masks, although the implementation is a hack using proxy objects and
 can lead to confusing error messages.

 Do not subclass-- this implementation makes assumptions about the
 memory layout.
 */
class Vector3 {    
public:
    // construction
    Vector3();
    Vector3(class BinaryInput& b);
    Vector3(Real _x, Real _y, Real _z);
    Vector3(Real coordinate[3]);
    Vector3(const Vector3& rkVector);
    Vector3(const class Vector3int16& v);

    void serialize(class BinaryOutput& b) const;
    void deserialize(class BinaryInput& b);
    
    // coordinates
    Real x, y, z;

    // access vector V as V[0] = V.x, V[1] = V.y, V[2] = V.z
    //
    // WARNING.  These member functions rely on
    // (1) Vector3 not having virtual functions
    // (2) the data packed in a 3*sizeof(Real) memory block
    Real& operator[] (int i) const; 
    operator Real* ();
    operator const Real* () const;

    enum Axis {X_AXIS=0, Y_AXIS=1, Z_AXIS=2, DETECT_AXIS=-1};

    /**
     Returns the largest dimension.  Particularly convenient for determining
     which plane to project a triangle onto for point-in-polygon tests.
     */
    Axis primaryAxis() const;

    // assignment and comparison
    Vector3& operator= (const Vector3& rkVector);
    bool operator== (const Vector3& rkVector) const;
    bool operator!= (const Vector3& rkVector) const;
    unsigned int hashCode() const;
    bool fuzzyEq(const Vector3& other) const;
    bool fuzzyNe(const Vector3& other) const;

    /** Returns true if this vector has finite length. */
    bool isFinite() const;

    /** Returns true if this vector has length ~= 0 */
    bool isZero() const;

    /** Returns true if this vector has length ~= 1 */
    bool isUnit() const;
    
    // arithmetic operations
    Vector3 operator+ (const Vector3& rkVector) const;
    Vector3 operator- (const Vector3& rkVector) const;
    Vector3 operator* (Real fScalar) const;
    Vector3 operator/ (Real fScalar) const;
    Vector3 operator* (const Vector3& rkVecto) const;
    Vector3 operator/ (const Vector3& rkVecto) const;
    Vector3 operator- () const;
    friend Vector3 operator* (Real fScalar, const Vector3& rkVector);

    // arithmetic updates
    Vector3& operator+= (const Vector3& rkVector);
    Vector3& operator-= (const Vector3& rkVector);
    Vector3& operator*= (Real fScalar);
    Vector3& operator/= (Real fScalar);
    Vector3& operator*= (const Vector3& rkVector);
    Vector3& operator/= (const Vector3& rkVector);

    Real length () const;
    
    /**
     The result is a nan vector if the length is almost zero.
     */
    Vector3 direction() const;

    /**
     Potentially less accurate but faster than direction().
     Only works if System::hasSSE is true.
     */
    Vector3 fastDirection() const;
    
    inline Vector3 unit() const {
        return direction();
    }

    inline Vector3 fastUnit() const {
        return fastDirection();
    }

    Real squaredLength () const;
    Real dot (const Vector3& rkVector) const;
    Real unitize (Real fTolerance = 1e-06);
    Vector3 cross (const Vector3& rkVector) const;
    Vector3 unitCross (const Vector3& rkVector) const;

    Vector3 min(const Vector3 &v) const;
    Vector3 max(const Vector3 &v) const;

    std::string toString() const;

    inline Vector3 clamp(const Vector3& low, const Vector3& high) const {
        return Vector3(
            G3D::clamp(x, low.x, high.x),
            G3D::clamp(y, low.y, high.y),
            G3D::clamp(z, low.z, high.z));
    }

    inline Vector3 clamp(double low, double high) const {
        return Vector3(
            G3D::clamp(x, low, high),
            G3D::clamp(y, low, high),
            G3D::clamp(z, low, high));
    }

    /**
     Linear interpolation
     */
    inline Vector3 lerp(const Vector3& v, double alpha) const {
        return (*this) + (v - *this) * alpha; 
    }

    /** Gram-Schmidt orthonormalization. */
    static void orthonormalize (Vector3 akVector[3]);

    /** Random unit vector */
    static Vector3 random();

    // Input W must be initialize to a nonzero vector, output is {U,V,W}
    // an orthonormal basis.  A hint is provided about whether or not W
    // is already unit length.
    static void generateOrthonormalBasis (Vector3& rkU, Vector3& rkV,
                                          Vector3& rkW, bool bUnitLengthW = true);

    // special points
    static const Vector3 ZERO;
    static const Vector3 ZERO3;
    static const Vector3 UNIT_X;
    static const Vector3 UNIT_Y;
    static const Vector3 UNIT_Z;
    static const Vector3 INF3;
    static const Vector3 NAN3;

    // 2-char swizzles

    Vector2 xx() const;
    VectorSwizzle2 xx();
    Vector2 yx() const;
    VectorSwizzle2 yx();
    Vector2 zx() const;
    VectorSwizzle2 zx();
    Vector2 xy() const;
    VectorSwizzle2 xy();
    Vector2 yy() const;
    VectorSwizzle2 yy();
    Vector2 zy() const;
    VectorSwizzle2 zy();
    Vector2 xz() const;
    VectorSwizzle2 xz();
    Vector2 yz() const;
    VectorSwizzle2 yz();
    Vector2 zz() const;
    VectorSwizzle2 zz();

    // 3-char swizzles

    Vector3 xxx() const;
    VectorSwizzle3 xxx();
    Vector3 yxx() const;
    VectorSwizzle3 yxx();
    Vector3 zxx() const;
    VectorSwizzle3 zxx();
    Vector3 xyx() const;
    VectorSwizzle3 xyx();
    Vector3 yyx() const;
    VectorSwizzle3 yyx();
    Vector3 zyx() const;
    VectorSwizzle3 zyx();
    Vector3 xzx() const;
    VectorSwizzle3 xzx();
    Vector3 yzx() const;
    VectorSwizzle3 yzx();
    Vector3 zzx() const;
    VectorSwizzle3 zzx();
    Vector3 xxy() const;
    VectorSwizzle3 xxy();
    Vector3 yxy() const;
    VectorSwizzle3 yxy();
    Vector3 zxy() const;
    VectorSwizzle3 zxy();
    Vector3 xyy() const;
    VectorSwizzle3 xyy();
    Vector3 yyy() const;
    VectorSwizzle3 yyy();
    Vector3 zyy() const;
    VectorSwizzle3 zyy();
    Vector3 xzy() const;
    VectorSwizzle3 xzy();
    Vector3 yzy() const;
    VectorSwizzle3 yzy();
    Vector3 zzy() const;
    VectorSwizzle3 zzy();
    Vector3 xxz() const;
    VectorSwizzle3 xxz();
    Vector3 yxz() const;
    VectorSwizzle3 yxz();
    Vector3 zxz() const;
    VectorSwizzle3 zxz();
    Vector3 xyz() const;
    VectorSwizzle3 xyz();
    Vector3 yyz() const;
    VectorSwizzle3 yyz();
    Vector3 zyz() const;
    VectorSwizzle3 zyz();
    Vector3 xzz() const;
    VectorSwizzle3 xzz();
    Vector3 yzz() const;
    VectorSwizzle3 yzz();
    Vector3 zzz() const;
    VectorSwizzle3 zzz();

    // 4-char swizzles

    Vector4 xxxx() const;
    VectorSwizzle4 xxxx();
    Vector4 yxxx() const;
    VectorSwizzle4 yxxx();
    Vector4 zxxx() const;
    VectorSwizzle4 zxxx();
    Vector4 xyxx() const;
    VectorSwizzle4 xyxx();
    Vector4 yyxx() const;
    VectorSwizzle4 yyxx();
    Vector4 zyxx() const;
    VectorSwizzle4 zyxx();
    Vector4 xzxx() const;
    VectorSwizzle4 xzxx();
    Vector4 yzxx() const;
    VectorSwizzle4 yzxx();
    Vector4 zzxx() const;
    VectorSwizzle4 zzxx();
    Vector4 xxyx() const;
    VectorSwizzle4 xxyx();
    Vector4 yxyx() const;
    VectorSwizzle4 yxyx();
    Vector4 zxyx() const;
    VectorSwizzle4 zxyx();
    Vector4 xyyx() const;
    VectorSwizzle4 xyyx();
    Vector4 yyyx() const;
    VectorSwizzle4 yyyx();
    Vector4 zyyx() const;
    VectorSwizzle4 zyyx();
    Vector4 xzyx() const;
    VectorSwizzle4 xzyx();
    Vector4 yzyx() const;
    VectorSwizzle4 yzyx();
    Vector4 zzyx() const;
    VectorSwizzle4 zzyx();
    Vector4 xxzx() const;
    VectorSwizzle4 xxzx();
    Vector4 yxzx() const;
    VectorSwizzle4 yxzx();
    Vector4 zxzx() const;
    VectorSwizzle4 zxzx();
    Vector4 xyzx() const;
    VectorSwizzle4 xyzx();
    Vector4 yyzx() const;
    VectorSwizzle4 yyzx();
    Vector4 zyzx() const;
    VectorSwizzle4 zyzx();
    Vector4 xzzx() const;
    VectorSwizzle4 xzzx();
    Vector4 yzzx() const;
    VectorSwizzle4 yzzx();
    Vector4 zzzx() const;
    VectorSwizzle4 zzzx();
    Vector4 xxxy() const;
    VectorSwizzle4 xxxy();
    Vector4 yxxy() const;
    VectorSwizzle4 yxxy();
    Vector4 zxxy() const;
    VectorSwizzle4 zxxy();
    Vector4 xyxy() const;
    VectorSwizzle4 xyxy();
    Vector4 yyxy() const;
    VectorSwizzle4 yyxy();
    Vector4 zyxy() const;
    VectorSwizzle4 zyxy();
    Vector4 xzxy() const;
    VectorSwizzle4 xzxy();
    Vector4 yzxy() const;
    VectorSwizzle4 yzxy();
    Vector4 zzxy() const;
    VectorSwizzle4 zzxy();
    Vector4 xxyy() const;
    VectorSwizzle4 xxyy();
    Vector4 yxyy() const;
    VectorSwizzle4 yxyy();
    Vector4 zxyy() const;
    VectorSwizzle4 zxyy();
    Vector4 xyyy() const;
    VectorSwizzle4 xyyy();
    Vector4 yyyy() const;
    VectorSwizzle4 yyyy();
    Vector4 zyyy() const;
    VectorSwizzle4 zyyy();
    Vector4 xzyy() const;
    VectorSwizzle4 xzyy();
    Vector4 yzyy() const;
    VectorSwizzle4 yzyy();
    Vector4 zzyy() const;
    VectorSwizzle4 zzyy();
    Vector4 xxzy() const;
    VectorSwizzle4 xxzy();
    Vector4 yxzy() const;
    VectorSwizzle4 yxzy();
    Vector4 zxzy() const;
    VectorSwizzle4 zxzy();
    Vector4 xyzy() const;
    VectorSwizzle4 xyzy();
    Vector4 yyzy() const;
    VectorSwizzle4 yyzy();
    Vector4 zyzy() const;
    VectorSwizzle4 zyzy();
    Vector4 xzzy() const;
    VectorSwizzle4 xzzy();
    Vector4 yzzy() const;
    VectorSwizzle4 yzzy();
    Vector4 zzzy() const;
    VectorSwizzle4 zzzy();
    Vector4 xxxz() const;
    VectorSwizzle4 xxxz();
    Vector4 yxxz() const;
    VectorSwizzle4 yxxz();
    Vector4 zxxz() const;
    VectorSwizzle4 zxxz();
    Vector4 xyxz() const;
    VectorSwizzle4 xyxz();
    Vector4 yyxz() const;
    VectorSwizzle4 yyxz();
    Vector4 zyxz() const;
    VectorSwizzle4 zyxz();
    Vector4 xzxz() const;
    VectorSwizzle4 xzxz();
    Vector4 yzxz() const;
    VectorSwizzle4 yzxz();
    Vector4 zzxz() const;
    VectorSwizzle4 zzxz();
    Vector4 xxyz() const;
    VectorSwizzle4 xxyz();
    Vector4 yxyz() const;
    VectorSwizzle4 yxyz();
    Vector4 zxyz() const;
    VectorSwizzle4 zxyz();
    Vector4 xyyz() const;
    VectorSwizzle4 xyyz();
    Vector4 yyyz() const;
    VectorSwizzle4 yyyz();
    Vector4 zyyz() const;
    VectorSwizzle4 zyyz();
    Vector4 xzyz() const;
    VectorSwizzle4 xzyz();
    Vector4 yzyz() const;
    VectorSwizzle4 yzyz();
    Vector4 zzyz() const;
    VectorSwizzle4 zzyz();
    Vector4 xxzz() const;
    VectorSwizzle4 xxzz();
    Vector4 yxzz() const;
    VectorSwizzle4 yxzz();
    Vector4 zxzz() const;
    VectorSwizzle4 zxzz();
    Vector4 xyzz() const;
    VectorSwizzle4 xyzz();
    Vector4 yyzz() const;
    VectorSwizzle4 yyzz();
    Vector4 zyzz() const;
    VectorSwizzle4 zyzz();
    Vector4 xzzz() const;
    VectorSwizzle4 xzzz();
    Vector4 yzzz() const;
    VectorSwizzle4 yzzz();
    Vector4 zzzz() const;
    VectorSwizzle4 zzzz();

};

}

unsigned int hashCode(const G3D::Vector3& v);

#include "Vector3.inl"

#endif
