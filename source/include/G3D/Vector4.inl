/** 
  @file Vector4.inl

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-07-09
  @edited  2003-02-10
 */

//----------------------------------------------------------------------------

inline unsigned int hashCode(const G3D::Vector4& v) {
     return v.hashCode();
}

namespace G3D {

//----------------------------------------------------------------------------
inline Vector4::Vector4 () {
    x = y = z = w = 0;
}

//----------------------------------------------------------------------------

inline Vector4::Vector4 (Real fX, Real fY, Real fZ, Real fW) {
    x = fX;
    y = fY;
    z = fZ;
    w = fW;
}

//----------------------------------------------------------------------------
inline Vector4::Vector4 (Real afCoordinate[4]) {
    x = afCoordinate[0];
    y = afCoordinate[1];
    z = afCoordinate[2];
    w = afCoordinate[3];
}

//----------------------------------------------------------------------------
inline Vector4::Vector4(const Vector4& rkVector) {
    x = rkVector.x;
    y = rkVector.y;
    z = rkVector.z;
    w = rkVector.w;
}
//----------------------------------------------------------------------------
inline Vector4::Vector4(const Vector3& rkVector, Real fW) {
    x = rkVector.x;
    y = rkVector.y;
    z = rkVector.z;
    w = fW;
}

//----------------------------------------------------------------------------
inline Real& Vector4::operator[] (int i) const {
    return ((Real*)this)[i];
}

//----------------------------------------------------------------------------
inline Vector4::operator Real* () {
    return (Real*)this;
}

inline Vector4::operator const Real* () const {
    return (Real*)this;
}

//----------------------------------------------------------------------------
inline Vector4& Vector4::operator= (const Vector4& rkVector) {
    x = rkVector.x;
    y = rkVector.y;
    z = rkVector.z;
    w = rkVector.w;
    return *this;
}

//----------------------------------------------------------------------------
inline bool Vector4::operator== (const Vector4& rkVector) const {
    return ( (x == rkVector.x) && (y == rkVector.y) && (z == rkVector.z) && (w == rkVector.w));
}

//----------------------------------------------------------------------------
inline bool Vector4::operator!= (const Vector4& rkVector) const {
    return ( x != rkVector.x || y != rkVector.y || z != rkVector.z || w != rkVector.w);
}

//----------------------------------------------------------------------------
inline Vector4 Vector4::operator+ (const Vector4& rkVector) const {
    return Vector4(x + rkVector.x, y + rkVector.y, z + rkVector.z, w + rkVector.w);
}

//----------------------------------------------------------------------------
inline Vector4 Vector4::operator- (const Vector4& rkVector) const {
    return Vector4(x - rkVector.x, y - rkVector.y, z - rkVector.z, w - rkVector.w);
}

//----------------------------------------------------------------------------
inline Vector4 Vector4::operator* (Real fScalar) const {
    return Vector4(fScalar*x, fScalar*y, fScalar*z, fScalar*w);
}

//----------------------------------------------------------------------------
inline Vector4 Vector4::operator- () const {
    return Vector4( -x, -y, -z, -w);
}

//----------------------------------------------------------------------------
inline Vector4 operator* (Real fScalar, const Vector4& rkVector) {
    return Vector4(fScalar*rkVector.x, fScalar*rkVector.y,
                   fScalar*rkVector.z, fScalar*rkVector.w);
}

//----------------------------------------------------------------------------
inline Vector4& Vector4::operator+= (const Vector4& rkVector) {
    x += rkVector.x;
    y += rkVector.y;
    z += rkVector.z;
    w += rkVector.w;
    return *this;
}

//----------------------------------------------------------------------------
inline Vector4& Vector4::operator-= (const Vector4& rkVector) {
    x -= rkVector.x;
    y -= rkVector.y;
    z -= rkVector.z;
    w -= rkVector.w;
    return *this;
}

//----------------------------------------------------------------------------

inline Vector4 Vector4::lerp(double alpha, const Vector4& v) const {
    return (*this) + (v - *this) * alpha; 
}


//----------------------------------------------------------------------------
inline Vector4& Vector4::operator*= (Real fScalar) {
    x *= fScalar;
    y *= fScalar;
    z *= fScalar;
    w *= fScalar;
    return *this;
}


//----------------------------------------------------------------------------
inline Real Vector4::dot (const Vector4& rkVector) const {
    return x*rkVector.x + y*rkVector.y + z*rkVector.z + w*rkVector.w;
}

//----------------------------------------------------------------------------
inline Vector4 Vector4::min(const Vector4 &v) const {
    return Vector4(G3D::min(v.x, x), G3D::min(v.y, y), G3D::min(v.z, z), G3D::min(v.w, w));
}

//----------------------------------------------------------------------------
inline Vector4 Vector4::max(const Vector4 &v) const {
    return Vector4(G3D::max(v.x, x), G3D::max(v.y, y), G3D::max(v.z, z), G3D::max(v.w, w));
}

//----------------------------------------------------------------------------
inline bool Vector4::isZero() const {
    return (x == 0.0) && (y == 0.0) && (z == 0.0) && (w == 0.0);
}

//----------------------------------------------------------------------------

inline bool Vector4::isFinite() const {
    return G3D::isFinite(x) && G3D::isFinite(y) && G3D::isFinite(z) && G3D::isFinite(w);
}

//----------------------------------------------------------------------------

inline bool Vector4::isUnit() const {
    return squaredLength() == 1.0;
}

//----------------------------------------------------------------------------

inline double Vector4::length() const {
    return sqrt(squaredLength());
}

//----------------------------------------------------------------------------

inline double Vector4::squaredLength() const {
    return x * x + y * y + z * z + w * w;
}

}
