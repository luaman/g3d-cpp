/**
  @file Vector3.h
 
  3D vector class
 
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com
 
  @created 2001-06-02
  @edited  2003-04-06
 */

#ifndef G3D_VECTOR3_H
#define G3D_VECTOR3_H

#include "G3D/g3dmath.h"
#include <string>

namespace G3D {

/**
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

    /**
     Linear interpolation
     */
    Vector3 lerp(double alpha, const Vector3& v) const {
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
};

}

unsigned int hashCode(const G3D::Vector3& v);

#include "Vector3.inl"

#endif
