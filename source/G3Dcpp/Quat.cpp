/**
  @file Quat.cpp
 
  Quaternion implementation based on Watt & Watt page 363
  
  @author Morgan McGuire, graphics3d.com
  
  @created 2002-01-23
  @edited  2004-01-26
 */

#include "G3D/Quat.h"

namespace G3D {

Quat Quat::fromAxisAngleRotation(
    const Vector3&      axis,
    double              angle) {

    Quat q;

    q.w = cos(angle / 2);
    q.imag() = axis.direction() * sin(angle / 2); 

    return q;
}


Quat::Quat(
    const Matrix3& rot) {

    static const int plus1mod3[] = {1, 2, 0};

    // Trace of the matrix
    double tr = rot[0][0] + rot[1][1] + rot[2][2];

    if (tr > 0.0) {
        double c = sqrt(tr + 1.0);
        w = (float) c * 0.5;
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

        float* v = (float*)(*this);

        v[i] = c * 0.5;
        c    = 0.5 / c;
        w    = (rot[j][k] - rot[k][j]) * c;
        v[j] = (rot[i][j] + rot[j][i]) * c;
        v[k] = (rot[i][k] + rot[k][i]) * c;
    }
}


void Quat::toAxisAngleRotation(
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

    rot = Matrix3(*this);
}

    
Quat Quat::slerp(
    const Quat&         quat1,
    double              alpha) const {

    // From: http://www.darwin3d.com/gamedev/articles/col0498.pdf

    const Quat& quat0 = *this;
    double omega, cosom, sinom, scale0, scale1;

    // Compute the cosine of the angle
    // between the quaternions
    
    cosom = quat0.dot(quat1);

    // Choose the shorter path
    if ((1.0 + cosom) > 0.001) {

        if ((1.0 - cosom) > 0.001) {
            // For large angles, slerp
            omega = acos(cosom);
            sinom = sin(omega);
            scale0 = sin((1.0 - alpha) * omega) / sinom;
            scale1 = sin(alpha * omega) / sinom;
        } else {
            // For small angles, linear interpolate
            scale0 = 1.0 - alpha;
            scale1 = alpha;
        }

        return quat0 * scale0 + quat1 * scale1;

    } else {

        // I don't understand why this is correct at all.
        scale0 = sin((1.0 - alpha) * G3D_PI);
        scale1 = sin(alpha * G3D_PI);

        return scale0 * quat0 + scale1 * Quat(-quat1.y, quat1.x, -quat1.w, quat1.z);
    }
}

Quat Quat::operator*(const Quat& other) const {

    // Following Watt & Watt, page 360
    const Vector3& v1 = imag();
    const Vector3& v2 = other.imag();
    double         s1 = w;
    double         s2 = other.w;

    return Quat(s1*v2 + s2*v1 + v1.cross(v2), s1*s2 - v1.dot(v2));
}

}

