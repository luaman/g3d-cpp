/**
 @file Color4.h
 
 Color class
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 @cite Portions based on Dave Eberly's Magic Software Library
      at <A HREF="http://www.magic-software.com">http://www.magic-software.com</A>
 
 @created 2002-06-25
 @edited  2003-04-08

 Copyright 2000-2003, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_COLOR4_H
#define G3D_COLOR4_H

#include "G3D/g3dmath.h"
#include "G3D/Color3.h"
#include <string>

namespace G3D {

/**
 Do not subclass-- this implementation makes assumptions about the
 memory layout.
 */
class Color4 {

public:

    /**
     * Does not initialize fields.
     */
    Color4 ();

    Color4(const Color3& c3, G3D::Real a = 1.0);

    Color4(class BinaryInput& bi);

    Color4(const class Vector4& v);

    /**
     * Initialize from G3D::Reals.
     */
    Color4(G3D::Real r, G3D::Real g, G3D::Real b, G3D::Real a = 1.0);
    
    /**
     * Initialize from array of G3D::Reals.
     */
    Color4(G3D::Real value[4]);

    /**
     * Initialize from another color.
     */
    Color4(const Color4& other);

    void serialize(class BinaryOutput& bo) const;
    void deserialize(class BinaryInput& bi);

    /**
     Initialize from an HTML-style color (e.g. 0xFFFF0000 == RED)
     */
    static Color4 fromARGB(uint32);

    /**
     * Channel values.
     */
    G3D::Real r, g, b, a;

    // access vector V as V[0] = V.r, V[1] = V.g, V[2] = V.b, v[3] = V.a
    //
    // WARNING.  These member functions rely on
    // (1) Color4 not having virtual functions
    // (2) the data packed in a 3*sizeof(G3D::Real) memory block
    G3D::Real& operator[] (int i) const;
    operator G3D::Real* ();
    operator const G3D::Real* () const;

    // assignment and comparison
    Color4& operator= (const Color4& rkVector);
    bool operator== (const Color4& rkVector) const;
    bool operator!= (const Color4& rkVector) const;
    unsigned int hashCode() const;

    // arithmetic operations
    Color4 operator+ (const Color4& rkVector) const;
    Color4 operator- (const Color4& rkVector) const;
    Color4 operator* (G3D::Real fScalar) const;
    Color4 operator/ (G3D::Real fScalar) const;
    Color4 operator- () const;
    friend Color4 operator* (G3D::Real fScalar, const Color4& rkVector);

    // arithmetic updates
    Color4& operator+= (const Color4& rkVector);
    Color4& operator-= (const Color4& rkVector);
    Color4& operator*= (G3D::Real fScalar);
    Color4& operator/= (G3D::Real fScalar);

    bool fuzzyEq(const Color4& other) const;
    bool fuzzyNe(const Color4& other) const;

    std::string toString() const;


    // special colors
    static const Color4 ZERO;
    static const Color4 CLEAR;
};

/**
 Extends the c3 with alpha = 1.0
 */
Color4 operator*(const Color3& c3, const Color4& c4);

} // namespace

#include "Color4.inl"

#endif
