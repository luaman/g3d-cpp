/**
 @file Color3.inl

 Color functions

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @cite Portions written by Laura Wollstadt, graphics3d.com
 @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com

 @created 2001-06-02
 @edited  2002-10-09

 Copyright 2000-2003, Morgan McGuire.
 All rights reserved.
*/

namespace G3D {

//----------------------------------------------------------------------------
inline Color3::Color3 () {
    // For efficiency in construction of large arrays of vectors, the
    // default constructor does not initialize the vector.
}

//----------------------------------------------------------------------------

inline Color3::Color3 (G3D::Real fX, G3D::Real fY, G3D::Real fZ) {
    r = fX;
    g = fY;
    b = fZ;
}

//----------------------------------------------------------------------------
inline Color3::Color3 (G3D::Real afCoordinate[3]) {
    r = afCoordinate[0];
    g = afCoordinate[1];
    b = afCoordinate[2];
}

//----------------------------------------------------------------------------
inline Color3::Color3 (const Color3& rkVector) {
    r = rkVector.r;
    g = rkVector.g;
    b = rkVector.b;
}

//----------------------------------------------------------------------------
inline G3D::Real& Color3::operator[] (int i) const {
    return ((G3D::Real*)this)[i];
}

//----------------------------------------------------------------------------
inline Color3::operator G3D::Real* () {
    return (G3D::Real*)this;
}

inline Color3::operator const G3D::Real* () const {
    return (G3D::Real*)this;
}

//----------------------------------------------------------------------------

inline bool Color3::fuzzyEq(const Color3& other) const {
    return G3D::fuzzyEq((*this - other).squaredLength(), 0);
}

//----------------------------------------------------------------------------

inline bool Color3::fuzzyNe(const Color3& other) const {
    return G3D::fuzzyNe((*this - other).squaredLength(), 0);
}


//----------------------------------------------------------------------------
inline Color3& Color3::operator= (const Color3& rkVector) {
    r = rkVector.r;
    g = rkVector.g;
    b = rkVector.b;
    return *this;
}

//----------------------------------------------------------------------------
inline bool Color3::operator== (const Color3& rkVector) const {
    return ( r == rkVector.r && g == rkVector.g && b == rkVector.b );
}

//----------------------------------------------------------------------------
inline bool Color3::operator!= (const Color3& rkVector) const {
    return ( r != rkVector.r || g != rkVector.g || b != rkVector.b );
}

//----------------------------------------------------------------------------
inline Color3 Color3::operator+ (const Color3& rkVector) const {
    return Color3(r + rkVector.r, g + rkVector.g, b + rkVector.b);
}

//----------------------------------------------------------------------------
inline Color3 Color3::operator- (const Color3& rkVector) const {
    return Color3(r -rkVector.r, g - rkVector.g, b - rkVector.b);
}

//----------------------------------------------------------------------------
inline Color3 Color3::operator* (G3D::Real fScalar) const {
    return Color3(fScalar*r, fScalar*g, fScalar*b);
}

//----------------------------------------------------------------------------
inline Color3 Color3::operator- () const {
    return Color3( -r, -g, -b);
}

//----------------------------------------------------------------------------
inline Color3 operator* (G3D::Real fScalar, const Color3& rkVector) {
    return Color3(fScalar*rkVector.r, fScalar*rkVector.g,
                  fScalar*rkVector.b);
}

//----------------------------------------------------------------------------
inline Color3& Color3::operator+= (const Color3& rkVector) {
    r += rkVector.r;
    g += rkVector.g;
    b += rkVector.b;
    return *this;
}

//----------------------------------------------------------------------------
inline Color3& Color3::operator-= (const Color3& rkVector) {
    r -= rkVector.r;
    g -= rkVector.g;
    b -= rkVector.b;
    return *this;
}

//----------------------------------------------------------------------------
inline Color3& Color3::operator*= (G3D::Real fScalar) {
    r *= fScalar;
    g *= fScalar;
    b *= fScalar;
    return *this;
}

//----------------------------------------------------------------------------
inline G3D::Real Color3::squaredLength () const {
    return r*r + g*g + b*b;
}

//----------------------------------------------------------------------------
inline G3D::Real Color3::length () const {
    return sqrt(r*r + g*g + b*b);
}

//----------------------------------------------------------------------------
inline Color3 Color3::direction () const {
    G3D::Real lenSquared = r * r + g * g + b * b;

    if (lenSquared != 1.0) {
        return *this / sqrt(lenSquared);
    } else {
        return *this;
    }
}

//----------------------------------------------------------------------------
inline G3D::Real Color3::dot (const Color3& rkVector) const {
    return r*rkVector.r + g*rkVector.g + b*rkVector.b;
}

//----------------------------------------------------------------------------
inline Color3 Color3::cross (const Color3& rkVector) const {
    return Color3(g*rkVector.b - b*rkVector.g, b*rkVector.r - r*rkVector.b,
                  r*rkVector.g - g*rkVector.r);
}

//----------------------------------------------------------------------------
inline Color3 Color3::unitCross (const Color3& rkVector) const {
    Color3 kCross(g*rkVector.b - b*rkVector.g, b*rkVector.r - r*rkVector.b,
                  r*rkVector.g - g*rkVector.r);
    kCross.unitize();
    return kCross;
}

}


