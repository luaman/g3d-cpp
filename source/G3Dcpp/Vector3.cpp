/**
 @file Vector3.cpp
 
 3D vector class
 
 @author Morgan McGuire and Laura Wollstadt, graphics3d.com
 
 @cite Portions based on Dave Eberly's Magic Software Library at http://www.magic-software.com
 
 
 @created 2001-06-02
 @edited  2003-04-06
 */

#include <limits>
#include <stdlib.h>
#include "G3D/Vector3.h"
#include "G3D/g3dmath.h"
#include "G3D/stringutils.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"

namespace G3D {

const Vector3 Vector3::ZERO(0, 0, 0);
const Vector3 Vector3::ZERO3(0, 0, 0);
const Vector3 Vector3::UNIT_X(1, 0, 0);
const Vector3 Vector3::UNIT_Y(0, 1, 0);
const Vector3 Vector3::UNIT_Z(0, 0, 1);
const Vector3 Vector3::INF3(infReal, infReal, infReal);
const Vector3 Vector3::NAN3(nan, nan, nan);

Vector3::Axis Vector3::primaryAxis() const {
    
    Axis a = X_AXIS;

    double nx = abs(x);
    double ny = abs(y);
    double nz = abs(z);

    if (nx > ny) {
        if (nx > nz) {
            a = X_AXIS;
        } else {
            a = Z_AXIS;
        }
    } else {
        if (ny > nz) {
            a = Y_AXIS;
        } else {
            a = Z_AXIS;
        }
    }

    return a;
}


unsigned int Vector3::hashCode() const {
    unsigned int xhash = (*(int*)(void*)(&x));
    unsigned int yhash = (*(int*)(void*)(&y));
    unsigned int zhash = (*(int*)(void*)(&z));

    return xhash + (yhash * 37) + (zhash * 101);
}

//----------------------------------------------------------------------------

double frand() {
    return rand() / (double) RAND_MAX;
}


Vector3::Vector3(BinaryInput& b) {
    deserialize(b);
}


void Vector3::deserialize(BinaryInput& b) {
    x = b.readFloat32();
    y = b.readFloat32();
    z = b.readFloat32();
}


void Vector3::serialize(BinaryOutput& b) const {
    b.writeFloat32(x);
    b.writeFloat32(y);
    b.writeFloat32(z);
}


Vector3 Vector3::random() {
    Vector3 result;

    do {
        result = Vector3(symmetricRandom(), 
                         symmetricRandom(),
                         symmetricRandom());
    } while ((result.squaredLength() > 1) || (result.squaredLength() == 1.0));

    result.unitize();

    return result;
}

//----------------------------------------------------------------------------
Vector3 Vector3::operator/ (G3D::Real fScalar) const {
    Vector3 kQuot;

    if ( fScalar != 0.0 ) {
		G3D::Real fInvScalar = 1.0 / fScalar;
        kQuot.x = fInvScalar * x;
        kQuot.y = fInvScalar * y;
        kQuot.z = fInvScalar * z;
        return kQuot;
    } else {
        return Vector3(G3D::infReal, G3D::infReal,
                       G3D::infReal);
    }
}

//----------------------------------------------------------------------------
Vector3& Vector3::operator/= (G3D::Real fScalar) {
    if (fScalar != 0.0) {
		G3D::Real fInvScalar = 1.0 / fScalar;
        x *= fInvScalar;
        y *= fInvScalar;
        z *= fInvScalar;
    } else {
        x = G3D::infReal;
        y = G3D::infReal;
        z = G3D::infReal;
    }

    return *this;
}

//----------------------------------------------------------------------------
G3D::Real Vector3::unitize (G3D::Real fTolerance) {
	G3D::Real fLength = length();

    if (fLength > fTolerance) {
		G3D::Real fInvLength = 1.0 / fLength;
        x *= fInvLength;
        y *= fInvLength;
        z *= fInvLength;
    } else {
        fLength = 0.0;
    }

    return fLength;
}

//----------------------------------------------------------------------------
void Vector3::orthonormalize (Vector3 akVector[3]) {
    // If the input vectors are v0, v1, and v2, then the Gram-Schmidt
    // orthonormalization produces vectors u0, u1, and u2 as follows,
    //
    //   u0 = v0/|v0|
    //   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
    //   u2 = (v2-(u0*v2)u0-(u1*v2)u1)/|v2-(u0*v2)u0-(u1*v2)u1|
    //
    // where |A| indicates length of vector A and A*B indicates dot
    // product of vectors A and B.

    // compute u0
    akVector[0].unitize();

    // compute u1
	G3D::Real fDot0 = akVector[0].dot(akVector[1]);
    akVector[1] -= fDot0 * akVector[0];
    akVector[1].unitize();

    // compute u2
	G3D::Real fDot1 = akVector[1].dot(akVector[2]);
    fDot0 = akVector[0].dot(akVector[2]);
    akVector[2] -= fDot0 * akVector[0] + fDot1 * akVector[1];
    akVector[2].unitize();
}

//----------------------------------------------------------------------------
void Vector3::generateOrthonormalBasis (Vector3& rkU, Vector3& rkV,
                                        Vector3& rkW, bool bUnitLengthW) {
    if ( !bUnitLengthW )
        rkW.unitize();

    if ( G3D::abs(rkW.x) >= G3D::abs(rkW.y)
            && G3D::abs(rkW.x) >= G3D::abs(rkW.z) ) {
        rkU.x = -rkW.y;
        rkU.y = + rkW.x;
        rkU.z = 0.0;
    } else {
        rkU.x = 0.0;
        rkU.y = + rkW.z;
        rkU.z = -rkW.y;
    }

    rkU.unitize();
    rkV = rkW.cross(rkU);
}

//----------------------------------------------------------------------------

std::string Vector3::toString() const {
    return G3D::format("(%g, %g, %g)", x, y, z);
}
//----------------------------------------------------------------------------

}; // namespace
