/**
 @file Color3.h
 
 Color class
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 @cite Portions by Laura Wollstadt
 @cite Portions based on Dave Eberly's Magic Software Library
      at <A HREF="http://www.magic-software.com">http://www.magic-software.com</A>
 
 @created 2001-06-02
 @edited  2003-02-03

 Copyright 2000-2003, Morgan McGuire.
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
     * Does not initialize fields.
     */
    Color3();

    Color3(class BinaryInput& bi);

    /**
     * Initialize from G3D::Reals.
     */
    Color3(G3D::Real r, G3D::Real g, G3D::Real b);

    Color3(const class Vector3& v);
    
    /**
     * Initialize from array of G3D::Reals.
     */
    Color3 (G3D::Real value[3]);

    /**
     * Initialize from another color.
     */
    Color3 (const Color3& other);

    /**
     Initialize from an HTML-style color (e.g. 0xFF0000 == RED)
     */
    static Color3 fromARGB(uint32);

    /**
     * Channel value.
     */
    G3D::Real r, g, b;

    void serialize(class BinaryOutput& bo) const;
    void deserialize(class BinaryInput& bi);

    // access vector V as V[0] = V.r, V[1] = V.g, V[2] = V.b
    //
    // WARNING.  These member functions rely on
    // (1) Color3 not having virtual functions
    // (2) the data packed in a 3*sizeof(G3D::Real) memory block
    G3D::Real& operator[] (int i) const;
    operator G3D::Real* ();
    operator const G3D::Real* () const;

    // assignment and comparison
    Color3& operator= (const Color3& rkVector);
    bool operator== (const Color3& rkVector) const;
    bool operator!= (const Color3& rkVector) const;
    unsigned int hashCode() const;

    // arithmetic operations
    Color3 operator+ (const Color3& rkVector) const;
    Color3 operator- (const Color3& rkVector) const;
    Color3 operator* (G3D::Real fScalar) const;
    Color3 operator* (const Color3& rkVector) const;
    Color3 operator/ (G3D::Real fScalar) const;
    Color3 operator- () const;
    friend Color3 operator* (G3D::Real fScalar, const Color3& rkVector);

    // arithmetic updates
    Color3& operator+= (const Color3& rkVector);
    Color3& operator-= (const Color3& rkVector);
    Color3& operator*= (const Color3& rkVector);
    Color3& operator*= (G3D::Real fScalar);
    Color3& operator/= (G3D::Real fScalar);

    bool fuzzyEq(const Color3& other) const;
    bool fuzzyNe(const Color3& other) const;

    // vector operations
    G3D::Real length () const;
    Color3 direction() const;
    G3D::Real squaredLength () const;
    G3D::Real dot (const Color3& rkVector) const;
    G3D::Real unitize (G3D::Real fTolerance = 1e-06);
    Color3 cross (const Color3& rkVector) const;
    Color3 unitCross (const Color3& rkVector) const;

    std::string toString() const;

    /** Random unit vector */
    static Color3 random();

    // special colors
    static const Color3 RED;
    static const Color3 GREEN;
    static const Color3 BLUE;

    static const Color3 PURPLE;
    static const Color3 CYAN;
    static const Color3 YELLOW;
    static const Color3 BROWN;
    static const Color3 ORANGE;

    static const Color3 BLACK;
    static const Color3 GRAY;
    static const Color3 WHITE;

};

} // namespace

#include "Color3.inl"

#endif
