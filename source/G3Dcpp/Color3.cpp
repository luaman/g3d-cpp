/**
 @file Color3.cpp

 Color class.

 @author Morgan McGuire, matrix@graphics3d.com
 @cite Portions by Laura Wollstadt, graphics3d.com
 @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com


 @created 2001-06-02
 @edited  2003-04-07
 */

#include <stdlib.h>
#include "G3D/Color3.h"
#include "G3D/Vector3.h"
#include "G3D/format.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/Color3uint8.h"

namespace G3D {

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



Color3::Color3(BinaryInput& bi) {
    deserialize(bi);
}


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
Color3 Color3::operator/ (G3D::Real fScalar) const {
    Color3 kQuot;

    if (fScalar != 0.0) {
		G3D::Real fInvScalar = 1.0 / fScalar;
        kQuot.r = fInvScalar * r;
        kQuot.g = fInvScalar * g;
        kQuot.b = fInvScalar * b;
        return kQuot;

    } else {

        return Color3(G3D::infReal, G3D::infReal, G3D::infReal);
    }
}

//----------------------------------------------------------------------------
Color3& Color3::operator/= (G3D::Real fScalar) {
    if (fScalar != 0.0) {
		G3D::Real fInvScalar = 1.0 / fScalar;
        r *= fInvScalar;
        g *= fInvScalar;
        b *= fInvScalar;
    } else {
        r = G3D::infReal;
        g = G3D::infReal;
        b = G3D::infReal;
    }

    return *this;
}

//----------------------------------------------------------------------------
G3D::Real Color3::unitize (G3D::Real fTolerance) {
	G3D::Real fLength = length();

    if ( fLength > fTolerance ) {
		G3D::Real fInvLength = 1.0 / fLength;
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

