/**
  @file Vector2.h
 
  2D vector class
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @cite Portions by Laura Wollstadt
  @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com
 
  @created 2001-06-02
  @edited  2003-09-29
*/

#ifndef G3D_VECTOR2_H
#define G3D_VECTOR2_H

#include "G3D/g3dmath.h"
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
class Vector2 {

public:
    // construction
    Vector2();
    Vector2(class BinaryInput& b);
    Vector2(Real x, Real y);
    Vector2(Real coordinate[2]);
    Vector2(const Vector2& rkVector);

    void serialize(class BinaryOutput& b) const;
    void deserialize(class BinaryInput& b);

    // coordinates
    Real x, y;

    Real& operator[] (int i) const;
    operator Real* ();
    operator const Real* () const;

    // assignment and comparison
    Vector2& operator= (const Vector2& rkVector);
    bool operator== (const Vector2& rkVector) const;
    bool operator!= (const Vector2& rkVector) const;
    unsigned int hashCode() const;
    bool fuzzyEq(const Vector2& other) const;
    bool fuzzyNe(const Vector2& other) const;
    /** Returns true if this vector has finite length */
    bool isFinite() const;

    /** Returns true if this vector has length == 0 */
    bool isZero() const;

    /** Returns true if this vector has length == 1 */
    bool isUnit() const;

    // arithmetic operations
    Vector2 operator+ (const Vector2& rkVector) const;
    Vector2 operator- (const Vector2& rkVector) const;
    Vector2 operator* (Real fScalar) const;
    Vector2 operator* (const Vector2& rkVector) const;
    Vector2 operator/ (const Vector2& rkVector) const;
    Vector2 operator/ (Real fScalar) const;
    Vector2 operator- () const;
    friend Vector2 operator* (Real fScalar, const Vector2& rkVector);
    
    /**
     Linear interpolation
     */
    inline Vector2 lerp(const Vector2& v, double alpha) const {
        return (*this) + (v - *this) * alpha; 
    }

    inline Vector2 clamp(const Vector2& low, const Vector2& high) const {
        return Vector2(
            G3D::clamp(x, low.x, high.x),
            G3D::clamp(y, low.y, high.y));
    }

    inline Vector2 clamp(double low, double high) const {
        return Vector2(
            G3D::clamp(x, low, high),
            G3D::clamp(y, low, high));
    }

    // arithmetic updates
    Vector2& operator+= (const Vector2& rkVector);
    Vector2& operator-= (const Vector2& rkVector);
    Vector2& operator*= (Real fScalar);
    Vector2& operator/= (Real fScalar);
    Vector2& operator*= (const Vector2& rkVector);
    Vector2& operator/= (const Vector2& rkVector);

    // vector operations
    Real length () const;
    Vector2 direction() const;
    Real squaredLength () const;
    Real dot (const Vector2& rkVector) const;
    Real unitize (Real fTolerance = 1e-06);

    Vector2 min(const Vector2 &v) const;
    Vector2 max(const Vector2 &v) const;

    // Random unit vector
    static Vector2 random();

    // special points
    static const Vector2 ZERO;
    static const Vector2 UNIT_S;
    static const Vector2 UNIT_T;

    std::string toString() const;

    // 2-char swizzles

    Vector2 xx() const;
    VectorSwizzle2 xx();
    Vector2 yx() const;
    VectorSwizzle2 yx();
    Vector2 xy() const;
    VectorSwizzle2 xy();
    Vector2 yy() const;
    VectorSwizzle2 yy();

    // 3-char swizzles

    Vector3 xxx() const;
    VectorSwizzle3 xxx();
    Vector3 yxx() const;
    VectorSwizzle3 yxx();
    Vector3 xyx() const;
    VectorSwizzle3 xyx();
    Vector3 yyx() const;
    VectorSwizzle3 yyx();
    Vector3 xxy() const;
    VectorSwizzle3 xxy();
    Vector3 yxy() const;
    VectorSwizzle3 yxy();
    Vector3 xyy() const;
    VectorSwizzle3 xyy();
    Vector3 yyy() const;
    VectorSwizzle3 yyy();

    // 4-char swizzles

    Vector4 xxxx() const;
    VectorSwizzle4 xxxx();
    Vector4 yxxx() const;
    VectorSwizzle4 yxxx();
    Vector4 xyxx() const;
    VectorSwizzle4 xyxx();
    Vector4 yyxx() const;
    VectorSwizzle4 yyxx();
    Vector4 xxyx() const;
    VectorSwizzle4 xxyx();
    Vector4 yxyx() const;
    VectorSwizzle4 yxyx();
    Vector4 xyyx() const;
    VectorSwizzle4 xyyx();
    Vector4 yyyx() const;
    VectorSwizzle4 yyyx();
    Vector4 xxxy() const;
    VectorSwizzle4 xxxy();
    Vector4 yxxy() const;
    VectorSwizzle4 yxxy();
    Vector4 xyxy() const;
    VectorSwizzle4 xyxy();
    Vector4 yyxy() const;
    VectorSwizzle4 yyxy();
    Vector4 xxyy() const;
    VectorSwizzle4 xxyy();
    Vector4 yxyy() const;
    VectorSwizzle4 yxyy();
    Vector4 xyyy() const;
    VectorSwizzle4 xyyy();
    Vector4 yyyy() const;
    VectorSwizzle4 yyyy();

};

}

inline unsigned int hashCode(const G3D::Vector2& v);

#include "Vector2.inl"

#endif
