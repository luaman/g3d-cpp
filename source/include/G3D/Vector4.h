/**
  @file Vector4.h
 
  Homogeneous vector class
 
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

/**
 Do not subclass-- this implementation makes assumptions about the
 memory layout.
 */
class Vector4 {
    
public:
    // construction
    Vector4();
    Vector4(Real fX, Real fY, Real fZ, Real fW);
    Vector4(Real afCoordinate[4]);
    Vector4(const Vector4& rkVector);
    Vector4(const class Color4& c);
    Vector4(const Vector3& rkVector, Real fW);
    Vector4(class BinaryInput& b);
    void serialize(class BinaryOutput& b) const;
    void deserialize(class BinaryInput& b);

    // coordinates
    Real x, y, z, w;

    // access vector V as V[0] = V.x, V[1] = V.y, V[2] = V.z, etc.
    //
    // WARNING.  These member functions rely on
    // (1) Vector4 not having virtual functions
    // (2) the data packed in a 4*sizeof(Real) memory block
    Real& operator[] (int i) const; 
    operator Real* ();
    operator const Real* () const;

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
    Vector4 operator* (Real fScalar) const;
    Vector4 operator/ (Real fScalar) const;
    Vector4 operator- () const;
    friend Vector4 operator* (Real fScalar, const Vector4& rkVector);

    // arithmetic updates
    Vector4& operator+= (const Vector4& rkVector);
    Vector4& operator-= (const Vector4& rkVector);
    Vector4& operator*= (Real fScalar);
    Vector4& operator/= (Real fScalar);

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

    Real dot (const Vector4& rkVector) const;

    Vector4 min(const Vector4& v) const;
    Vector4 max(const Vector4& v) const;

    std::string toString() const;

    /**
     Linear interpolation
     */
    Vector4 lerp(const Vector4& v, double alpha) const;

    /** Swizzle for Cg programs */
    inline Vector3 xyz() const {
        return Vector3(x, y, z);
    }

    /** Swizzle for Cg programs */
    inline Vector2 xy() const {
        return Vector2(x, y);
    }

    /** Swizzle for Cg programs */
    inline Vector2 zw() const {
        return Vector2(z, w);
    }
};

}

unsigned int hashCode(const G3D::Vector4& v);

#include "Vector4.inl"

#endif
