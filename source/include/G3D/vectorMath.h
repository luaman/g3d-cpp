/**
  @file vectorMath.h
 
  Function aliases for popular vector methods.
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @created: 2001-06-02
  @edited:  2003-09-28
 */

#ifndef G3D_VECTORMATH_H
#define G3D_VECTORMATH_H

#include "G3D/g3dmath.h"
#include "G3D/Vector2.h"
#include "G3D/Vector3.h"
#include "G3D/Vector4.h"
#include "G3D/Matrix3.h"

namespace G3D {

typedef Matrix4 float4x4;
typedef Matrix3 float3x3;
typedef Vector4 float4;
typedef Vector3 float3;
typedef Vector2 float2;

inline Matrix4 mul(const Matrix4& a, const Matrix4& b) {
    return a * b;
}

inline Vector4 mul(const Matrix4& m, const Vector4& v) {
    return m * v;
}

inline Vector3 mul(const Matrix3& m, const Vector3& v) {
    return m * v;
}

inline Matrix3 mul(const Matrix3& a, const Matrix3& b) {
    return a * b;
}

inline float dot(const Vector2& a, const Vector2& b) {
    return a.dot(b);
}

inline float dot(const Vector3& a, const Vector3& b) {
    return a.dot(b);
}

inline float dot(const Vector4& a, const Vector4& b) {
    return a.dot(b);
}

inline Vector2 normalize(const Vector2& v) {
    return v / v.length();
}

inline Vector3 normalize(const Vector3& v) {
    return v / v.length();
}

inline Vector4 normalize(const Vector4& v) {
    return v / v.length();
}

inline Vector2 abs(const Vector2& v) {
    return Vector2(abs(v.x), abs(v.y));
}

inline Vector3 abs(const Vector3& v) {
    return Vector3(abs(v.x), abs(v.y), abs(v.z));
}

inline Vector4 abs(const Vector4& v) {
    return Vector4(abs(v.x), abs(v.y), abs(v.z), abs(v.w));
}

inline bool all(const Vector2& v) {
    return (v.x != 0) && (v.y != 0);
}

inline bool all(const Vector3& v) {
    return (v.x != 0) && (v.y != 0) && (v.z != 0);
}

inline bool all(const Vector4& v) {
    return (v.x != 0) && (v.y != 0) && (v.z != 0) && (v.w != 0);
}

inline bool any(const Vector2& v) {
    return (v.x != 0) || (v.y != 0);
}

inline bool any(const Vector3& v) {
    return (v.x != 0) || (v.y != 0) || (v.z != 0);
}

inline bool any(const Vector4& v) {
    return (v.x != 0) || (v.y != 0) || (v.z != 0) || (v.w != 0);
}

inline Vector2 clamp(const Vector2& v, const Vector2& a, const Vector2& b) {
    return v.clamp(a, b);
}

inline Vector3 clamp(const Vector3& v, const Vector3& a, const Vector3& b) {
    return v.clamp(a, b);
}

inline Vector4 clamp(const Vector4& v, const Vector4& a, const Vector4& b) {
    return v.clamp(a, b);
}

inline Vector2 lerp(const Vector2& v1, const Vector2& v2, float f) {
    return v1.lerp(v2, f);
}

inline Vector3 lerp(const Vector3& v1, const Vector3& v2, float f) {
    return v1.lerp(v2, f);
}

inline Vector4 lerp(const Vector4& v1, const Vector4& v2, float f) {
    return v1.lerp(v2, f);
}

inline Vector3 cross(const Vector3& v1, const Vector3& v2) {
    return v1.cross(v2);
}

inline double degrees(double rad) {
    return toDegrees(rad);
}

inline double radians(double deg) {
    return toRadians(deg);
}

inline float determinant(const Matrix3& m) {
    return m.determinant();
}

inline Vector2 min(const Vector2& v1, const Vector2& v2) {
    return v1.min(v2);
}

inline Vector3 min(const Vector3& v1, const Vector3& v2) {
    return v1.min(v2);
}

inline Vector4 min(const Vector4& v1, const Vector4& v2) {
    return v1.min(v2);
}


}

#endif
