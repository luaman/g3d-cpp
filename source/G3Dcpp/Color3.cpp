/**
 @file Color3.cpp

 Color class.

 @author Morgan McGuire, matrix@graphics3d.com
 @cite Portions by Laura Wollstadt, graphics3d.com
 @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com


 @created 2001-06-02
 @edited  2005-11-13
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

        return Color3(G3D::inf(), G3D::inf(), G3D::inf());
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
        r = G3D::inf();
        g = G3D::inf();
        b = G3D::inf();
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
Color3 Color3::fromHSV(const Vector3& _hsv) {
	debugAssertM((_hsv.x <= 1.0 && _hsv.x >= 0.0)
			&& (_hsv.y <= 1.0 && _hsv.y >= 0.0) 
			&& ( _hsv.z <= 1.0 && _hsv.z >= 0.0), "H,S,V must be between [0,1]");
	const int i = G3D::iFloor(6.0*_hsv.x);
	const double f = 6.0*_hsv.x - i;
	const double m = _hsv.z * (1.0 - (_hsv.y));
	const double n = _hsv.z * (1.0 - (_hsv.y * f));
	const double k = _hsv.z * (1.0 - (_hsv.y * (1 - f)));
	switch(i) {
		case 0:
			return Color3(_hsv.z, k, m);
		break;

		case 1:
			return Color3(n, _hsv.z, m);
		break;

		case 2:
			return Color3(m, _hsv.z, k);
		break;

		case 3:
			return Color3(m, n, _hsv.z);
		break;

		case 4:
			return Color3(k, m, _hsv.z);
		break;

		case 5:
			return Color3(_hsv.z, m, n);
		break;
		debugAssertM(false, "fell through switch..");
	}
	return Color3::black();
}

Vector3 Color3::toHSV(const Color3& _rgb) {
	debugAssertM((_rgb.r <= 1.0 && _rgb.r >= 0.0) 
			&& (_rgb.g <= 1.0 && _rgb.g >= 0.0)
			&& (_rgb.b <= 1.0 && _rgb.b >= 0.0), "R,G,B must be between [0,1]");
	Vector3 hsv = Vector3::zero();
	hsv.z = G3D::max(G3D::max(_rgb.r, _rgb.g), _rgb.b);
	if (G3D::fuzzyEq(hsv.z, 0.0f)) {
		return hsv;
	}
	const double x =  G3D::min(G3D::min(_rgb.r, _rgb.g), _rgb.b);
	hsv.y = (hsv.z - x) / hsv.z; 
	if (G3D::fuzzyEq(hsv.y, 0.0f)) {
		return hsv;
	}
	Vector3 rgbN;
	rgbN.x = (hsv.z - _rgb.r)/(hsv.z - x);
	rgbN.y = (hsv.z - _rgb.g)/(hsv.z - x);
	rgbN.z = (hsv.z - _rgb.b)/(hsv.z - x);

	if (_rgb.r == hsv.z) {  // note from the max we know that it exactly equals one of the three.
		hsv.x = (_rgb.g == x)? 5.0f + rgbN.z : 1.0f - rgbN.y;
	} else if (_rgb.g == hsv.z) {
		hsv.x = (_rgb.b == x)? 1.0f + rgbN.x : 3.0f - rgbN.z;
	} else {
		hsv.x = (_rgb.r == x)? 3.0f + rgbN.y : 5.0f - rgbN.x;
	}
	hsv.x /= 6.0; 
	return hsv;
}

Color3 Color3::jetColorMap(const float& val) {
	debugAssertM(val <= 1.0 && val >= 0.0 , "value should be in [0,1]");

	//truncated triangles where sides have slope 4
	Color3 jet;

	jet.r = G3D::min(4.0f * val - 1.5f,-4.0f * val + 4.5f) ;
	jet.g = G3D::min(4.0f * val - 0.5f,-4.0f * val + 3.5f) ;
	jet.b = G3D::min(4.0f * val + 0.5f,-4.0f * val + 2.5f) ;


	jet.r = G3D::clamp(jet.r, 0.0f, 1.0f);
	jet.g = G3D::clamp(jet.g, 0.0f, 1.0f);
	jet.b = G3D::clamp(jet.b, 0.0f, 1.0f);

	return jet;
}





std::string Color3::toString() const {
    return G3D::format("(%g, %g, %g)", r, g, b);
}

//----------------------------------------------------------------------------

}; // namespace

