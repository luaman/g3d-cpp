/**
 @file Color4.cpp

 Color class.

 @author Morgan McGuire, matrix@graphics3d.com
 @cite Portions by Laura Wollstadt, graphics3d.com
 @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com


 @created 2002-06-25
 @edited  2004-02-23
 */

#include <stdlib.h>
#include "G3D/Color4.h"
#include "G3D/Color4uint8.h"
#include "G3D/Vector4.h"
#include "G3D/format.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"

namespace G3D {

const Color4& Color4::zero() {
    static Color4 c(0.0, 0.0, 0.0, 0.0);
    return c;
}

const Color4& Color4::clear() {
    static Color4 c(0.0, 0.0, 0.0, 0.0);
    return c;
}

Color4::Color4(const Vector4& v) {
    r = v.x;
    g = v.y;
    b = v.z;
    a = v.w;
}

// Deprecated.
const Color4 Color4::ZERO   (0.0, 0.0, 0.0, 0.0);
const Color4 Color4::CLEAR  (0.0, 0.0, 0.0, 0.0);


Color4::Color4(const Color4uint8& c) {
    r = c.r / 255.0;
    g = c.g / 255.0;
    b = c.b / 255.0;
    a = c.a / 255.0;
}

unsigned int Color4::hashCode() const {
    unsigned int rhash = (*(int*)(void*)(&r));
    unsigned int ghash = (*(int*)(void*)(&g));
    unsigned int bhash = (*(int*)(void*)(&b));
    unsigned int ahash = (*(int*)(void*)(&a));

    return rhash + (ghash * 37) + (bhash * 101) + (ahash * 241);
}

Color4 Color4::fromARGB(uint32 x) {
    return Color4((x >> 16) & 0xFF, 
                  (x >> 8) & 0xFF,
                  x & 0xFF, 
                  (x >> 24) & 0xFF) / 255.0;
}


Color4::Color4(BinaryInput& bi) {
    deserialize(bi);
}


void Color4::deserialize(BinaryInput& bi) {
    r = bi.readFloat32();
    g = bi.readFloat32();
    b = bi.readFloat32();
    a = bi.readFloat32();
}


void Color4::serialize(BinaryOutput& bo) const {
    bo.writeFloat32(r);
    bo.writeFloat32(g);
    bo.writeFloat32(b);
    bo.writeFloat32(a);
}


//----------------------------------------------------------------------------

Color4 Color4::operator/ (double fScalar) const {
    Color4 kQuot;

    if (fScalar != 0.0) {
		float fInvScalar = 1.0 / fScalar;
        kQuot.r = fInvScalar * r;
        kQuot.g = fInvScalar * g;
        kQuot.b = fInvScalar * b;
        kQuot.a = fInvScalar * a;
        return kQuot;

    } else {

        return Color4(inf, inf, inf, inf);
    }
}

//----------------------------------------------------------------------------

Color4& Color4::operator/= (double fScalar) {
    if (fScalar != 0.0) {
		float fInvScalar = 1.0 / fScalar;
        r *= fInvScalar;
        g *= fInvScalar;
        b *= fInvScalar;
        a *= fInvScalar;
    } else {
        r = inf;
        g = inf;
        b = inf;
        a = inf;
    }

    return *this;
}

//----------------------------------------------------------------------------

std::string Color4::toString() const {
    return G3D::format("(%g, %g, %g, %g)", r, g, b, a);
}

//----------------------------------------------------------------------------

}; // namespace

