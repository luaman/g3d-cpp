/**
  @file Vector2.h
 
  2D vector class
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @cite Portions by Laura Wollstadt
  @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com
 
  @created 2001-06-02
  @edited  2004-01-22
  Copyright 2000-2004, Morgan McGuire.
  All rights reserved.
*/

#ifndef G3D_VECTOR2_H
#define G3D_VECTOR2_H

#include "G3D/g3dmath.h"
#include <string>

namespace G3D {

class Vector2;    
class Vector3;
class Vector4;

/**
 Do not subclass-- this implementation makes assumptions about the
 memory layout.
 */
class Vector2 {
public:
    // coordinates
    float x, y;

    // construction
    Vector2();
    Vector2(class BinaryInput& b);
    Vector2(double x, double y);
    Vector2(float coordinate[2]);
    Vector2(double coordinate[2]);
    Vector2(const Vector2& rkVector);
    Vector2(const class Vector2int16& v); 

    void serialize(class BinaryOutput& b) const;
    void deserialize(class BinaryInput& b);


    float& operator[] (int i) const;
    operator float* ();
    operator const float* () const;

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
    Vector2 operator* (double fScalar) const;
    Vector2 operator* (const Vector2& rkVector) const;
    Vector2 operator/ (const Vector2& rkVector) const;
    Vector2 operator/ (double fScalar) const;
    Vector2 operator- () const;

    inline double sum() const {
        return x + y;
    }

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
    Vector2& operator*= (double fScalar);
    Vector2& operator/= (double fScalar);
    Vector2& operator*= (const Vector2& rkVector);
    Vector2& operator/= (const Vector2& rkVector);

    // vector operations
    double length() const;
    Vector2 direction() const;
    double squaredLength () const;
    double dot (const Vector2& rkVector) const;
    double unitize (double fTolerance = 1e-06);

    Vector2 min(const Vector2 &v) const;
    Vector2 max(const Vector2 &v) const;

    // Random unit vector
    static Vector2 random();

    // Special values.
    // Intentionally not inlined: see Matrix3::identity() for details.
    static const Vector2& zero();
    static const Vector2& unitX();
    static const Vector2& unitY();

    // Deprecated. See Matrix3::identity() for details.
    /** @deprecated Use Vector2::zero() */
    static const Vector2 ZERO;
    /** @deprecated Use Vector2::unitX() */
    static const Vector2 UNIT_S;
    /** @deprecated Use Vector2::unitY() */
    static const Vector2 UNIT_T;

    std::string toString() const;

    // 2-char swizzles

    Vector2 xx() const;
    Vector2 yx() const;
    Vector2 xy() const;
    Vector2 yy() const;

    // 3-char swizzles

    Vector3 xxx() const;
    Vector3 yxx() const;
    Vector3 xyx() const;
    Vector3 yyx() const;
    Vector3 xxy() const;
    Vector3 yxy() const;
    Vector3 xyy() const;
    Vector3 yyy() const;

    // 4-char swizzles

    Vector4 xxxx() const;
    Vector4 yxxx() const;
    Vector4 xyxx() const;
    Vector4 yyxx() const;
    Vector4 xxyx() const;
    Vector4 yxyx() const;
    Vector4 xyyx() const;
    Vector4 yyyx() const;
    Vector4 xxxy() const;
    Vector4 yxxy() const;
    Vector4 xyxy() const;
    Vector4 yyxy() const;
    Vector4 xxyy() const;
    Vector4 yxyy() const;
    Vector4 xyyy() const;
    Vector4 yyyy() const;

};

}

// Intentionally outside namespace to avoid operator overloading confusion
inline G3D::Vector2 operator*(double s, const G3D::Vector2& v) {
    return v * s;
}


inline unsigned int hashCode(const G3D::Vector2& v);

#include "Vector2.inl"

#endif
