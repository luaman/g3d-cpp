#ifndef G3D_VECTORSWIZZLE_H
#define G3D_VECTORSWIZZLE_H

#include "G3D/Vector2.h"
#include "G3D/Vector3.h"
#include "G3D/Vector4.h"

namespace G3D {

/**
 See G3D::VectorSwizzle4.
 */
class VectorSwizzle2 {
    friend class Vector2;
    friend class Vector3;
    friend class Vector4;

public:

    float&         x;
    float&         y;

private:

    inline VectorSwizzle2(float& a, float& b) : x(a), y(b) {}

public:

    inline operator Vector2() const {
        return Vector2(x, y);
    }

    inline VectorSwizzle2& operator=(const Vector2& v) {
        x = v.x;
        y = v.y;
        return *this;
    }
};


/**
 See G3D::VectorSwizzle4.
 */
class VectorSwizzle3 {
    friend class Vector2;
    friend class Vector3;
    friend class Vector4;

public:

    float&         x;
    float&         y;
    float&         z;

private:

    inline VectorSwizzle3(float& a, float& b, float& c) : x(a), y(b), z(c) {}

public:

    inline VectorSwizzle3& operator=(const Vector3& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
};


/**
 In order to support write masks on the various Vector types, they return
 these proxy "swizzle" objects.  When a swizzle object is written to, it 
 writes back to the underlying Vector object.
 */
class VectorSwizzle4 {
    friend class Vector2;
    friend class Vector3;
    friend class Vector4;

public:

    float&         x;
    float&         y;
    float&         z;
    float&         w;

private:

    inline VectorSwizzle4(float& a, float& b, float& c, float& d) : x(a), y(b), z(c), w(d) {}

public:

    inline VectorSwizzle4& operator=(const Vector4& v) {
        x = v.x;
        y = v.y;
        z = v.z;
        w = v.w;
        return *this;
    }
};

}

#endif
