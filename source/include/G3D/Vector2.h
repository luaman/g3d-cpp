/**
  @file Vector2.h
 
  2D vector class
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @cite   Portions by Laura Wollstadt
  @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com
 
  @created 2001-06-02
  @edited  2003-05-13
*/

#ifndef G3D_VECTOR2_H
#define G3D_VECTOR2_H

#include "G3D/g3dmath.h"
#include <string>

namespace G3D {

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

};

}

inline unsigned int hashCode(const G3D::Vector2& v);

#include "Vector2.inl"

#endif
