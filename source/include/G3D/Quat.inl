/**
  Quat.inl
 
  @cite Quaternion implementation based on Watt & Watt page 363.  Thanks to Max McGuire for lerp optimizations.
  
  @maintainer Morgan McGuire, matrix@graphics3d.com
  
  @created 2002-01-23
  @edited  2003-09-28
 */

namespace G3D {

inline Real& Quat::operator[] (int i) const {
    return ((Real*)this)[i];
}

inline Quat::operator Real* () {
    return (Real*)this;
}

inline Quat::operator const Real* () const {
    return (Real*)this;
}

inline Quat Quat::operator- (const Quat& other) const {
    return Quat(x - other.x, y - other.y, z - other.z, w - other.w);
}

inline double Quat::dot(const Quat& other) const {
    return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
}

inline Real Quat::magnitude() const { 
    return x*x + y*y + z*z + w*w; 
}


inline Quat Quat::pow(double x) const {

    Vector3 axis;
    double  angle;
    toAxisAngle(axis, angle);

    return Quat(axis, angle * x);
}

}

