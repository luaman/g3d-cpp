/**
  @file Quat.h
 
  Quaternion
  
  @maintainer Morgan McGuire, matrix@graphics3d.com
  
  @created 2002-01-23
  @edited  2004-01-23
 */

#ifndef G3D_QUAT_H
#define G3D_QUAT_H

#include "G3D/g3dmath.h"
#include "G3D/Vector3.h"
#include "G3D/Matrix3.h"
#include <string>

namespace G3D {

/**
  Unit quaternions are used in computer graphics to represent
  rotation about an axis.  Any 3x3 rotation matrix can
  be stored as a quaternion.

  A quaternion represents the sum of a real scalar and
  an imaginary vector: ix + jy + kz + w.

  Do not subclass.
 */
class Quat {
public:

    /**
     q = [sin(angle / 2) * axis, cos(angle / 2)]
    
     In Watt & Watt's notation, s = w, v = (x, y, z)
     In the Real-Time Rendering notation, u = (x, y, z), w = w 
     */
    float x, y, z, w;

    /**
     Initializes to a zero degree rotation.
     */
    inline Quat() : x(0), y(0), z(0), w(1) {}

    Quat(
        const Matrix3& rot);

    inline Quat(double _x, double _y, double _z, double _w) :
        x(_x), y(_y), z(_z), w(_w) {}

    /** Defaults to a pure vector quaternion */
    inline Quat(const Vector3& v, double _w = 0) : x(v.x), y(v.y), z(v.z), w(_w) {
    }

    /**
     The real part of the quaternion.
     */
    inline const float& real() const {
        return w;
    }

    inline float& real() {
        return w;
    }

    /**
     Returns the imaginary part (x, y, z)
     */
    inline const Vector3& imag() const {
        return *(reinterpret_cast<const Vector3*>(this));
    }

    inline Vector3& imag() {
        return *(reinterpret_cast<Vector3*>(this));
    }

    static Quat Quat::fromAxisAngle(
        const Vector3&      axis,
        double              angle);

    void toAxisAngle(
        Vector3&            axis,
        double&             angle) const;

    Matrix3 toRotationMatrix() const;

    void toRotationMatrix(
        Matrix3&            rot) const;
    
    /**
     Computes the linear interpolation of this to
     other at time alpha.
     */
    Quat slerp(
        const Quat&         other,
        double              alpha) const;

    /**
     Raise this quaternion to a power.  For a rotation, this is
     the effect of rotating x times as much as the original
     quaterion.
     */
    inline Quat pow(double x) const;

    /**
     Negates the imaginary part.
     */
    inline Quat conjugate() const {
        return Quat(-x, -y, -z, w);
    }

    /**
     Quaternion multiplication (composition of rotations).
     Note that this does not commute.
     */
    Quat operator*(const Quat& other) const;

    inline double dot(const Quat& other) const {
        return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
    }

    /**
     Quaternion magnitude (sum squares; no sqrt).
     */
    inline float magnitude() const;

    Quat operator-(const Quat& other) const;
    Quat operator+(const Quat& other) const;

    // access quaternion as q[0] = q.x, q[1] = q.y, q[2] = q.z, q[3] = q.w
    //
    // WARNING.  These member functions rely on
    // (1) Quat not having virtual functions
    // (2) the data packed in a 4*sizeof(float) memory block
    float& operator[] (int i) const;
    operator float* ();
    operator const float* () const;
};

}

#include "Quat.inl"


#endif

