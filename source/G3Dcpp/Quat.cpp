/**
  @file Quat.cpp
 
  Quaternion implementation based on Watt & Watt page 363
  
  @author Morgan McGuire, graphics3d.com
  
  @created 2002-01-23
  @edited  2003-02-15
 */

#include "G3D/Quat.h"

namespace G3D {

Quat::Quat(
    const Vector3&      axis,
    double              angle) {

    w = cos(angle / 2);
    Vector3 v = axis.direction() * sin(angle / 2); 

    x = v.x;
    y = v.y;
    z = v.z;
}


Quat::Quat(
    const Matrix3& rot) {

    static const int plus1mod3[] = {1, 2, 0};

    // Trace of the matrix
    double tr = rot[0][0] + rot[1][1] + rot[2][2];

    if (tr > 0.0) {
        double c = sqrt(tr + 1.0);
        w = (Real) c * 0.5;
        c = 0.5 / c;

        x = (rot[1][2] - rot[2][1]) * c;
        y = (rot[2][0] - rot[0][2]) * c;
        z = (rot[0][1] - rot[1][0]) * c;
    } else {

        // Find the largest diagonal component
        int i = 0;
        
        if (rot[1][1] > rot[0][0]) {
            i = 1;
        }

        if (rot[2][2] > rot[i][i]) {
            i = 2;
        }

        int j = plus1mod3[i];
        int k = plus1mod3[j];

        double c = sqrt((rot[i][i] - (rot[j][j] + rot[k][k])) + 1.0);

        Real* v = (Real*)(*this);

        v[i] = c * 0.5;
        c    = 0.5 / c;
        w    = (rot[j][k] - rot[k][j]) * c;
        v[j] = (rot[i][j] + rot[j][i]) * c;
        v[k] = (rot[i][k] + rot[k][i]) * c;
    }
}


void Quat::toAxisAngle(
    Vector3&            axis,
    double&             angle) const {
   


    // Decompose the quaternion into an angle and an axis.
    
    axis = Vector3(x, y, z);
    angle = 2 * acos(w);

    double len = sqrt(1.0f - w * w);

    if (fuzzyGt(abs(len), 0)) {
        axis /= len;
    }

    // Reduce the range of the angle.

    if (angle < 0) {		
		angle = -angle;
		axis = -axis;
    }

	while (angle > 2 * G3D_PI) {
        angle -= 2 * G3D_PI;
    }

	if (abs(angle) > G3D_PI) {
		angle -= 2 * G3D_PI;
    }

    // Make the angle positive.

	if (angle < 0) {		
		angle = -angle;
        axis = -axis;
	}
}


Matrix3 Quat::toRotationMatrix() const {
    Matrix3 out = Matrix3::ZERO;

    toRotationMatrix(out);

    return out;
}


void Quat::toRotationMatrix(
    Matrix3&            rot) const {

    // Note: for a unit quaternion, s*s + v.dot(v) == 1
    //    c = 2.0 / (s * s + v.dot(v));

    const double c = 2.0;

    double xc = x * c;
    double yc = y * c;
    double zc = z * c;

    double xx = x * xc;
    double xy = x * yc;
    double xz = x * zc;

    double wx = w * xc;
    double wy = w * yc;
    double wz = w * zc;

    double yy = y * yc;
    double yz = y * zc;
    double zz = z * zc;

    rot.set(1.0 - (yy + zz),          xy + wz,         xz - wy,
                    xy - wz,  1.0 - (xx + zz),         yz + wx,
                    xz + wy,            yz-wx, 1.0 - (xx + yy));
}

    
Quat Quat::lerp(
    const Quat&         other,
    double              alpha) const {

    // We might need to negate one in order to ensure that
    // we're going the short way

    Quat out;
    double a, b;

    a = (1.0 - alpha);
    b = alpha;

    // See if (p - q) dot (p - q) > (p + q) dot (p + q) in order to know
    // if we're interpolating around the sphere the long way.  Note that this
    // comparison expands to: 
    //
    //  sum(p[i]^2 - p[i]q[i] + q[i]^2) > sum(p[i]^2 + 2p[i]q[i] + q[i]^2)
    //
    // The squared terms cancel, leaving:
    //
    //                   sum(-p[i]q[i]) > sum(p[i]q[i])
    //
    //                   0 > 2 sum(p[i]q[i])
    //         
    //                   p dot q < 0
    //
    if (dot(other) < 0) {
        // The difference between the orientations is greater than
        // if we flip one by 2*PI (recall that quaternions have a 
        // period of FOUR PI, not TWO PI).  This means we'll interpolate
        // the long way around the sphere if we don't negate the second.
        b *= -1;
    }

    // Interpolate and normalize
    double mag = 0.0;
    int i;
    const Real* p = *this;
    const Real* q = other;
    for (i = 0; i < 4; i++) {
        out[i] = p[i] * a + q[i] * b;
        mag += out[i] * out[i];
    }

    // If mag == 0, the result is ambiguous; choose one.
    if (mag == 0.0) {

        return Quat();
    
    } else {

        double invMag = 1 / mag;
        for (i = 0; i < 4; i++) {
            out[i] *= invMag;
        }
        return out;
    }
}

Quat Quat::operator*(const Quat& other) const {

    // Following Watt & Watt, page 360
    Vector3 v1(x, y, z);
    Vector3 v2(other.x, other.y, other.z);
    double  s1 = w;
    double  s2 = other.w;

    Vector3 v3 = s1*v2 + s2*v1 + v1.cross(v2);
    double  s3 = s1*s2 - v1.dot(v2);
    
    return Quat(v3.x, v3.y, v3.z, s3);
}

}

