/**
  @file Quat.h
 
  Quaternion
  
  @maintainer Morgan McGuire, matrix@graphics3d.com
  
  @created 2002-01-23
  @edited  2004-01-26
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
  an imaginary vector: ix + jy + kz + w.  A unit quaternion
  representing a rotation by A about axis v has the form 
  [sin(A/2)*v, cos(A/2)].  For a unit quaternion, q.conj() == q.inverse()
  is a rotation by -A about v.  -q is the same rotation as q
  (negate both the axis and angle).  
  
  A non-unit quaterion q represents the same rotation as
  q.unitize() (Dam98 pg 28).

  Although quaternion-vector operations (eg. Quat + Vector3) are
  well defined, they are not supported by this class because
  they typically are bugs when they appear in code.

  Do not subclass.

  <B>BETA API -- subject to change</B>
  @cite Erik B. Dam, Martin Koch, Martin Lillholm, Quaternions, Interpolation and Animation.  Technical Report DIKU-TR-98/5, Department of Computer Science, University of Copenhagen, Denmark.  1998.
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

    /** q = [sin(angle/2)*axis, cos(angle/2)] */
    static Quat Quat::fromAxisAngleRotation(
        const Vector3&      axis,
        double              angle);

    /** Returns the axis and angle of rotation represented
        by this quaternion (i.e. q = [sin(angle/2)*axis, cos(angle/2)]) */
    void toAxisAngleRotation(
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
     Negates the imaginary part.
     */
    inline Quat conj() const {
        return Quat(-x, -y, -z, w);
    }

    inline double sum() const {
        return x + y + z + w;
    }

    inline double average() const {
        return sum() / 4.0;
    }

    inline Quat operator*(double s) const {
        return Quat(x * s, y * s, z * s, w * s);
    }

    inline Quat operator/(double s) const {
        return Quat(x / s, y / s, z / s, w / s);
    }

    inline double dot(const Quat& other) const {
        return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
    }

    /** Note that q<SUP>-1</SUP> = q.conj() for a unit quaternion. 
        @cite Dam99 page 13 */
    inline Quat inverse() const {
        return conj() / dot(*this);
    }

    Quat operator-(const Quat& other) const;

    Quat operator+(const Quat& other) const;

    /**
     Quaternion multiplication (composition of rotations).
     Note that this does not commute.
     */
    Quat operator*(const Quat& other) const;

    /* (*this) * other.inverse() */
    Quat operator/(const Quat& other) const {
        return (*this) * other.inverse();
    }


    /** Is the magnitude nearly 1.0? */
    inline bool isUnit(double tolerance = 1e-5) const {
        return abs(dot(*this) - 1.0) < tolerance * tolerance;
    }
    

    inline double magnitude() const {
        return sqrt(dot(*this));
    }


    /** log q = [Av, 0] where q = [sin(A) * v, cos(A)].
        Only for unit quaternions */
    inline Quat log() const {
        debugAssertM(isUnit(), "Log only defined for unit quaternions");
        // Solve for A in q = [sin(A)*v, cos(A)]
        Vector3 u(x, y, z);
        double len = u.magnitude();
        double A = atan2(w, len);
        Vector3 v = u / len;
        
        return Quat(v * A, 0);
    }

    /** exp q = [sin(A) * v, cos(A)] where q = [Av, 0].
        Only defined for pure-vector quaternions */
    inline Quat exp() const {
        debugAssertM(w == 0, "exp only defined for vector quaternions");
        Vector3 u(x, y, z);
        double A = u.magnitude();
        Vector3 v = u / A;
        return Quat(sin(A) * v, cos(A));
    }


    /**
     Raise this quaternion to a power.  For a rotation, this is
     the effect of rotating x times as much as the original
     quaterion.

     Note that q.pow(a).pow(b) == q.pow(a + b)
     @cite Dam98 pg 21
     */
    inline Quat pow(double x) const {
        return (log() * x).exp();
    }


    inline Quat unitize() const {
        double mag2 = dot(*this);
        if (fuzzyEq(mag2, 1.0)) {
            return *this;
        } else {
            return *this / sqrt(mag2);
        }
    }

    /**
     Returns a unit quaterion obtained by dividing through by
     the magnitude.
     */
    inline Quat toUnit() const {
        return unitize();
    }

    /**
     The linear algebra 2-norm, sqrt(q dot q).  This matches
     the value used in Dam's 1998 tech report but differs from the
     n(q) value used in Eberly's 1999 paper, which is the square of the
     norm.
     */
    inline double norm() const {
        return magnitude();
    }

    // access quaternion as q[0] = q.x, q[1] = q.y, q[2] = q.z, q[3] = q.w
    //
    // WARNING.  These member functions rely on
    // (1) Quat not having virtual functions
    // (2) the data packed in a 4*sizeof(float) memory block
    float& operator[] (int i) const;
    operator float* ();
    operator const float* () const;
};

inline Quat exp(const Quat& q) {
    return q.exp();
}

inline Quat log(const Quat& q) {
    return q.log();
}

} // Namespace G3D

// Outside the namespace to avoid overloading confusion for C++
inline G3D::Quat pow(const G3D::Quat& q, double x) {
    return q.pow(x);
}

#include "Quat.inl"

#endif
