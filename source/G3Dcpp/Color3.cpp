/**
 @file Color3.cpp

 Color class.

 @author Morgan McGuire, matrix@graphics3d.com
 @cite Portions by Laura Wollstadt, graphics3d.com
 @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com


 @created 2001-06-02
 @edited  2004-05-03
 */

#include <stdlib.h>
#include "G3D/Color3.h"
#include "G3D/Vector3.h"
#include "G3D/format.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/Color3uint8.h"

namespace G3D {

const Color3& Color3::red() {
    static Color3 c(1, 0, 0);
    return c;
}

const Color3& Color3::green() {
    static Color3 c(0, 1, 0);
    return c;
}

const Color3& Color3::blue() {
    static Color3 c(0, 0, 1);
    return c;
}

const Color3& Color3::purple() {
    static Color3 c(0.7, 0, 1);
    return c;
}

const Color3& Color3::cyan() {
    static Color3 c(0, .7, 1);
    return c;
}

const Color3& Color3::yellow() {
    static Color3 c(1, 1, 0);
    return c;
}

const Color3& Color3::brown() {
    static Color3 c(.5, .5, 0);
    return c;
}

const Color3& Color3::orange() {
    static Color3 c(1, 0.5, 0);
    return c;
}

const Color3& Color3::black() {
    static Color3 c(0, 0, 0);
    return c;
}

const Color3& Color3::gray() {
    static Color3 c(.7, .7, .7);
    return c;
}

const Color3& Color3::white() {
    static Color3 c(1, 1, 1);
    return c;
}

Color3::Color3(BinaryInput& bi) {
    deserialize(bi);
}

// Deprecated.
const Color3 Color3::RED(1, 0, 0);
const Color3 Color3::GREEN(0, 1, 0);
const Color3 Color3::BLUE(0, 0, 1);
const Color3 Color3::PURPLE(0.7, 0, 1);
const Color3 Color3::CYAN(0, .7, 1);
const Color3 Color3::YELLOW(1, 1, 0);
const Color3 Color3::BROWN(.5, .5, 0);
const Color3 Color3::ORANGE(1, 0.5, 0);
const Color3 Color3::BLACK(0, 0, 0);
const Color3 Color3::GRAY(.7, .7, .7);
const Color3 Color3::WHITE(1, 1, 1);


void Color3::deserialize(BinaryInput& bi) {
    r = bi.readFloat32();
    g = bi.readFloat32();
    b = bi.readFloat32();
}


void Color3::serialize(BinaryOutput& bo) const {
    bo.writeFloat32(r);
    bo.writeFloat32(g);
    bo.writeFloat32(b);
}


const Color3& Color3::wheelRandom() {
    static const Color3 colorArray[8] =
    {Color3::BLUE,   Color3::RED,    Color3::GREEN,
     Color3::ORANGE, Color3::YELLOW, 
     Color3::CYAN,   Color3::PURPLE, Color3::BROWN};

    return colorArray[iRandom(0, 7)];
}


unsigned int Color3::hashCode() const {
    unsigned int rhash = (*(int*)(void*)(&r));
    unsigned int ghash = (*(int*)(void*)(&g));
    unsigned int bhash = (*(int*)(void*)(&b));

    return rhash + (ghash * 37) + (bhash * 101);
}


Color3::Color3(const Vector3& v) {
    r = v.x;
    g = v.y;
    b = v.z;
}


Color3::Color3(const class Color3uint8& other) {
    r = other.r / 255.0;
    g = other.g / 255.0;
    b = other.b / 255.0;
}


Color3 Color3::fromARGB(uint32 x) {
    return Color3((x >> 16) & 0xFF, (x >> 8) & 0xFF, x & 0xFF) / 255.0;
}

//----------------------------------------------------------------------------


Color3 Color3::random() {
    return Color3(G3D::unitRandom(), 
                  G3D::unitRandom(),
                  G3D::unitRandom()).direction();
}

//----------------------------------------------------------------------------
Color3 Color3::operator/ (double fScalar) const {
    Color3 kQuot;

    if (fScalar != 0.0) {
		float fInvScalar = 1.0 / fScalar;
        kQuot.r = fInvScalar * r;
        kQuot.g = fInvScalar * g;
        kQuot.b = fInvScalar * b;
        return kQuot;

    } else {

        return Color3(inf, inf, inf);
    }
}

//----------------------------------------------------------------------------
Color3& Color3::operator/= (double fScalar) {
    if (fScalar != 0.0) {
		double fInvScalar = 1.0 / fScalar;
        r *= fInvScalar;
        g *= fInvScalar;
        b *= fInvScalar;
    } else {
        r = inf;
        g = inf;
        b = inf;
    }

    return *this;
}

//----------------------------------------------------------------------------
float Color3::unitize (float fTolerance) {
	float fLength = length();

    if ( fLength > fTolerance ) {
		float fInvLength = 1.0 / fLength;
        r *= fInvLength;
        g *= fInvLength;
        b *= fInvLength;
    } else {
        fLength = 0.0;
    }

    return fLength;
}

//----------------------------------------------------------------------------

std::string Color3::toString() const {
    return G3D::format("(%g, %g, %g)", r, g, b);
}

//----------------------------------------------------------------------------

}; // namespace

