/**
 @file Color3.h
 
 Color class
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 @cite Portions based on Dave Eberly's Magic Software Library
      at <A HREF="http://www.magic-software.com">http://www.magic-software.com</A>
 
 @created 2001-06-02
 @edited  2004-05-03

 Copyright 2000-2004, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_COLOR3_H
#define G3D_COLOR3_H

#include "G3D/g3dmath.h"
#include <string>

namespace G3D {

/**
 Do not subclass-- this implementation makes assumptions about the
 memory layout.
 */
class Color3 {

public:
    /**
     Does not initialize fields.
     */
    Color3();

    Color3(class BinaryInput& bi);

    Color3(double r, double g, double b);

    Color3(const class Vector3& v);
    
    Color3(const float value[3]);

    /**
     Initialize from another color.
     */
    Color3 (const Color3& other);

    Color3 (const class Color3uint8& other);

    /**
     Initialize from an HTML-style color (e.g. 0xFF0000 == RED)
     */
    static Color3 fromARGB(uint32);

    /** Returns one of the color wheel colors (e.g. RED, GREEN, CYAN).
        Does not include white, black, or gray. */
    static const Color3& wheelRandom();

    /**
     * Channel value.
     */
    float r, g, b;

    void serialize(class BinaryOutput& bo) const;
    void deserialize(class BinaryInput& bi);

    // access vector V as V[0] = V.r, V[1] = V.g, V[2] = V.b
    //
    // WARNING.  These member functions rely on
    // (1) Color3 not having virtual functions
    // (2) the data packed in a 3*sizeof(float) memory block
    const float& operator[] (int i) const;
    float& operator[] (int i);

    // assignment and comparison
    Color3& operator= (const Color3& rkVector);
    bool operator== (const Color3& rkVector) const;
    bool operator!= (const Color3& rkVector) const;
    unsigned int hashCode() const;

    // arithmetic operations
    Color3 operator+ (const Color3& rkVector) const;
    Color3 operator- (const Color3& rkVector) const;
    Color3 operator* (double fScalar) const;
    Color3 operator* (const Color3& rkVector) const;
    Color3 operator/ (double fScalar) const;
    Color3 operator- () const;

    // arithmetic updates
    Color3& operator+= (const Color3& rkVector);
    Color3& operator-= (const Color3& rkVector);
    Color3& operator*= (const Color3& rkVector);
    Color3& operator*= (double fScalar);
    Color3& operator/= (double fScalar);

    bool fuzzyEq(const Color3& other) const;
    bool fuzzyNe(const Color3& other) const;

    inline operator float* () {
        return (float*)this;
    }

    operator const float* () const {
        return (float*)this;
    }

    // vector operations
    float length () const;
    Color3 direction() const;
    float squaredLength () const;
    float dot (const Color3& rkVector) const;
    float unitize (float fTolerance = 1e-06);
    Color3 cross (const Color3& rkVector) const;
    Color3 unitCross (const Color3& rkVector) const;


    inline Color3 max(const Color3& other) const {
        return Color3(G3D::max(r, other.r), G3D::max(g, other.g), G3D::max(b, other.b));
    }

    inline Color3 min(const Color3& other) const {
        return Color3(G3D::min(r, other.r), G3D::min(g, other.g), G3D::min(b, other.b));
    }

	inline Color3 lerp(const Color3& other, double a) const {
        return (*this) + (other - *this) * a; 

    }

    inline double sum() const {
        return r + g + b;
    }

    inline double average() const {
        return sum() / 3.0;
    }


    std::string toString() const;

    /** Random unit vector */
    static Color3 random();

    // Special values.
    // Intentionally not inlined: see Matrix3::identity() for details.
    static const Color3& red();
    static const Color3& green();
    static const Color3& blue();
    static const Color3& purple();
    static const Color3& cyan();
    static const Color3& yellow();
    static const Color3& brown();
    static const Color3& orange();
    static const Color3& black();
    static const Color3& gray();
    static const Color3& white();
    
    // Deprecated. See Matrix3::identity() for details.
    /** @deprecated Use Color3::red() */
    static const Color3 RED;      
    /** @deprecated Use Color3::green() */
    static const Color3 GREEN;
    /** @deprecated Use Color3::blue() */
    static const Color3 BLUE;
    /** @deprecated Use Color3::purple() */
    static const Color3 PURPLE;
    /** @deprecated Use Color3::cyan() */
    static const Color3 CYAN;
    /** @deprecated Use Color3::yellow() */
    static const Color3 YELLOW;
    /** @deprecated Use Color3::brown() */
    static const Color3 BROWN;
    /** @deprecated Use Color3::orange() */
    static const Color3 ORANGE;
    /** @deprecated Use Color3::black() */
    static const Color3 BLACK;
    /** @deprecated Use Color3::gray() */
    static const Color3 GRAY;
    /** @deprecated Use Color3::white() */
    static const Color3 WHITE;

};

inline G3D::Color3 operator* (double s, const G3D::Color3& c) {
    return c * s;
}

} // namespace




#include "Color3.inl"

#endif
