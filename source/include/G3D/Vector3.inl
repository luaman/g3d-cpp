/** 
  @file Vector3.inl

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com

  @created 2001-06-02
  @edited  2004-01-30
  Copyright 2000-2004, Morgan McGuire.
  All rights reserved.
 */

//----------------------------------------------------------------------------
#ifdef SSE
    // MMX & SSE extensions
    #include <xmmintrin.h>
#endif

inline unsigned int hashCode(const G3D::Vector3& v) {
     return v.hashCode();
}

namespace G3D {

//----------------------------------------------------------------------------
inline Vector3::Vector3() : x(0.0), y(0.0), z(0.0) {
}

//----------------------------------------------------------------------------

inline Vector3::Vector3 (double fX, double fY, double fZ) : x(fX), y(fY), z(fZ) {
}

//----------------------------------------------------------------------------
inline Vector3::Vector3 (float V[3]) : x(V[0]), y(V[1]), z(V[2]){
}
//----------------------------------------------------------------------------
inline Vector3::Vector3 (double V[3]) : x(V[0]), y(V[1]), z(V[2]){
}

//----------------------------------------------------------------------------
inline Vector3::Vector3 (const Vector3& V) : x(V.x), y(V.y), z(V.z) {
}

//----------------------------------------------------------------------------

//inline Vector3::Vector3 (const __m128& m) {
    // Cast from SSE packed floats
//    *this = *(Vector3*)&m;
//}

//----------------------------------------------------------------------------
inline const float& Vector3::operator[] (int i) const {
    return ((float*)this)[i];
}

inline float& Vector3::operator[] (int i) {
    return ((float*)this)[i];
}


//----------------------------------------------------------------------------
inline Vector3& Vector3::operator= (const Vector3& rkVector) {
    x = rkVector.x;
    y = rkVector.y;
    z = rkVector.z;
    return *this;
}

//----------------------------------------------------------------------------

inline bool Vector3::fuzzyEq(const Vector3& other) const {
    return G3D::fuzzyEq((*this - other).squaredLength(), 0);
}

//----------------------------------------------------------------------------

inline bool Vector3::fuzzyNe(const Vector3& other) const {
    return G3D::fuzzyNe((*this - other).squaredLength(), 0);
}

//----------------------------------------------------------------------------

inline bool Vector3::isFinite() const {
    return G3D::isFinite(x) && G3D::isFinite(y) && G3D::isFinite(z);
}

//----------------------------------------------------------------------------
inline bool Vector3::operator== (const Vector3& rkVector) const {
    return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
}

//----------------------------------------------------------------------------
inline bool Vector3::operator!= (const Vector3& rkVector) const {
    return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::operator+ (const Vector3& rkVector) const {
    return Vector3(x + rkVector.x, y + rkVector.y, z + rkVector.z);
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::operator- (const Vector3& rkVector) const {
    return Vector3(x - rkVector.x, y - rkVector.y, z - rkVector.z);
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::operator* (double fScalar) const {
    return Vector3(fScalar*x, fScalar*y, fScalar*z);
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::operator* (const Vector3& rkVector) const {
    return Vector3(x * rkVector.x, y * rkVector.y, z * rkVector.z);
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::operator/ (const Vector3& rkVector) const {
    return Vector3(x / rkVector.x, y / rkVector.y, z / rkVector.z);
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::operator- () const {
    return Vector3(-x, -y, -z);
}

//----------------------------------------------------------------------------
inline Vector3& Vector3::operator+= (const Vector3& rkVector) {
    x += rkVector.x;
    y += rkVector.y;
    z += rkVector.z;
    return *this;
}

//----------------------------------------------------------------------------
inline Vector3& Vector3::operator-= (const Vector3& rkVector) {
    x -= rkVector.x;
    y -= rkVector.y;
    z -= rkVector.z;
    return *this;
}

//----------------------------------------------------------------------------
inline Vector3& Vector3::operator*= (double fScalar) {
    x *= fScalar;
    y *= fScalar;
    z *= fScalar;
    return *this;
}

//----------------------------------------------------------------------------
inline Vector3& Vector3::operator*= (const Vector3& rkVector) {
    x *= rkVector.x;
    y *= rkVector.y;
    z *= rkVector.z;
    return *this;
}

//----------------------------------------------------------------------------
inline Vector3& Vector3::operator/= (const Vector3& rkVector) {
    x /= rkVector.x;
    y /= rkVector.y;
    z /= rkVector.z;
    return *this;
}

//----------------------------------------------------------------------------
inline double Vector3::squaredLength () const {
    return x*x + y*y + z*z;
}

//----------------------------------------------------------------------------
inline double Vector3::length() const {
    return sqrt(x*x + y*y + z*z);
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::direction () const {
    float lenSquared = x * x + y * y + z * z;
    float invSqrt = 1/sqrt(lenSquared);
    return Vector3(x * invSqrt, y * invSqrt, z * invSqrt);
}

//----------------------------------------------------------------------------

inline Vector3 Vector3::fastDirection () const {
    float lenSquared = x * x + y * y + z * z;
    float invSqrt = rsq(lenSquared);
    return Vector3(x * invSqrt, y * invSqrt, z * invSqrt);
}

//----------------------------------------------------------------------------
inline double Vector3::dot (const Vector3& rkVector) const {
    return x*rkVector.x + y*rkVector.y + z*rkVector.z;
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::cross (const Vector3& rkVector) const {
    return Vector3(y*rkVector.z - z*rkVector.y, z*rkVector.x - x*rkVector.z,
                   x*rkVector.y - y*rkVector.x);
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::unitCross (const Vector3& rkVector) const {
    Vector3 kCross(y*rkVector.z - z*rkVector.y, z*rkVector.x - x*rkVector.z,
                   x*rkVector.y - y*rkVector.x);
    kCross.unitize();
    return kCross;
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::min(const Vector3 &v) const {
    return Vector3(G3D::min(v.x, x), G3D::min(v.y, y), G3D::min(v.z, z));
}

//----------------------------------------------------------------------------
inline Vector3 Vector3::max(const Vector3 &v) const {
    return Vector3(G3D::max(v.x, x), G3D::max(v.y, y), G3D::max(v.z, z));
}

//----------------------------------------------------------------------------
inline bool Vector3::isZero() const {
    return G3D::fuzzyEq(squaredLength(), 0.0);
}

//----------------------------------------------------------------------------

inline bool Vector3::isUnit() const {
    return G3D::fuzzyEq(squaredLength(), 1.0);
}

}
