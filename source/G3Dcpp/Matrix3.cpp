/**
 @file Matrix3.cpp

 3x3 matrix class

 @author Morgan McGuire, graphics3d.com

 @created 2001-06-02
 @edited  2005-06-08
*/

#include <memory.h>
#include <assert.h>
#include "G3D/Matrix3.h"
#include "G3D/g3dmath.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/Quat.h"

namespace G3D {

const float Matrix3::EPSILON = 1e-06;

const Matrix3& Matrix3::zero() {
    static Matrix3 m(0, 0, 0, 0, 0, 0, 0, 0, 0);
    return m;
}

const Matrix3& Matrix3::identity() {
    static Matrix3 m(1, 0, 0, 0, 1, 0, 0, 0, 1);
    return m;
}

// Deprecated.
const Matrix3 Matrix3::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0);
const Matrix3 Matrix3::IDENTITY(1, 0, 0, 0, 1, 0, 0, 0, 1);

const float Matrix3::ms_fSvdEpsilon = 1e-04;
const int Matrix3::ms_iSvdMaxIterations = 32;

Matrix3::Matrix3(BinaryInput& b) {
    deserialize(b);
}

bool Matrix3::fuzzyEq(const Matrix3& b) const {
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (! G3D::fuzzyEq(m_aafEntry[r][c], b[r][c])) {
                return false;
            }
        }
    }
    return true;
}


//----------------------------------------------------------------------------
Matrix3::Matrix3(const Quat& _q) {
    // implementation from Watt and Watt, pg 362
    Quat q = _q.unitize();
    double xx = 2.0 * q.x * q.x;
    double xy = 2.0 * q.x * q.y;
    double xz = 2.0 * q.x * q.z;
    double xw = 2.0 * q.x * q.w;

    double yy = 2.0 * q.y * q.y;
    double yz = 2.0 * q.y * q.z;
    double yw = 2.0 * q.y * q.w;

    double zz = 2.0 * q.z * q.z;
    double zw = 2.0 * q.z * q.w;

    set(1.0 - yy - zz,    xy - zw,        xz + yw,
        xy + zw,          1.0 - xx - zz,  yz - xw,
        xz - yw,          yz + xw,        1.0 - xx - yy);
}

//----------------------------------------------------------------------------

Matrix3::Matrix3 (const float aafEntry[3][3]) {
    memcpy(m_aafEntry, aafEntry, 9*sizeof(float));
}

//----------------------------------------------------------------------------
Matrix3::Matrix3 (const Matrix3& rkMatrix) {
    memcpy(m_aafEntry, rkMatrix.m_aafEntry, 9*sizeof(float));
}

//----------------------------------------------------------------------------
Matrix3::Matrix3(
          float fEntry00, float fEntry01, float fEntry02,
          float fEntry10, float fEntry11, float fEntry12,
          float fEntry20, float fEntry21, float fEntry22) {
    set(fEntry00, fEntry01, fEntry02,
        fEntry10, fEntry11, fEntry12,
        fEntry20, fEntry21, fEntry22);
}

void Matrix3::set(
          float fEntry00, float fEntry01, float fEntry02,
          float fEntry10, float fEntry11, float fEntry12, 
          float fEntry20, float fEntry21, float fEntry22) {

    m_aafEntry[0][0] = fEntry00;
    m_aafEntry[0][1] = fEntry01;
    m_aafEntry[0][2] = fEntry02;
    m_aafEntry[1][0] = fEntry10;
    m_aafEntry[1][1] = fEntry11;
    m_aafEntry[1][2] = fEntry12;
    m_aafEntry[2][0] = fEntry20;
    m_aafEntry[2][1] = fEntry21;
    m_aafEntry[2][2] = fEntry22;
}


void Matrix3::deserialize(BinaryInput& b) {
    int r,c;
    for (c = 0; c < 3; ++c) {
        for (r = 0; r < 3; ++r) {
            m_aafEntry[r][c] = b.readFloat32();
        }
    }
}


void Matrix3::serialize(BinaryOutput& b) const {
    int r,c;
    for (c = 0; c < 3; ++c) {
        for (r = 0; r < 3; ++r) {
            b.writeFloat32(m_aafEntry[r][c]);
        }
    }
}


//----------------------------------------------------------------------------
Vector3 Matrix3::getColumn (int iCol) const {
    assert((0 <= iCol) && (iCol < 3));
    return Vector3(m_aafEntry[0][iCol], m_aafEntry[1][iCol],
                   m_aafEntry[2][iCol]);
}

Vector3 Matrix3::getRow (int iRow) const {
    return Vector3(m_aafEntry[iRow][0], m_aafEntry[iRow][1], m_aafEntry[iRow][2]);
}

void Matrix3::setColumn(int iCol, const Vector3 &vector) {
    debugAssert((iCol >= 0) && (iCol < 3));
    m_aafEntry[0][iCol] = vector.x;
    m_aafEntry[1][iCol] = vector.y;
    m_aafEntry[2][iCol] = vector.z;
}


void Matrix3::setRow(int iRow, const Vector3 &vector) {
    debugAssert((iRow >= 0) && (iRow < 3));
    m_aafEntry[iRow][0] = vector.x;
    m_aafEntry[iRow][1] = vector.y;
    m_aafEntry[iRow][2] = vector.z;
}


//----------------------------------------------------------------------------
bool Matrix3::operator== (const Matrix3& rkMatrix) const {
    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++) {
            if ( m_aafEntry[iRow][iCol] != rkMatrix.m_aafEntry[iRow][iCol] )
                return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
bool Matrix3::operator!= (const Matrix3& rkMatrix) const {
    return !operator==(rkMatrix);
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::operator+ (const Matrix3& rkMatrix) const {
    Matrix3 kSum;

    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++) {
            kSum.m_aafEntry[iRow][iCol] = m_aafEntry[iRow][iCol] +
                                          rkMatrix.m_aafEntry[iRow][iCol];
        }
    }

    return kSum;
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::operator- (const Matrix3& rkMatrix) const {
    Matrix3 kDiff;

    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++) {
            kDiff.m_aafEntry[iRow][iCol] = m_aafEntry[iRow][iCol] -
                                           rkMatrix.m_aafEntry[iRow][iCol];
        }
    }

    return kDiff;
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::operator* (const Matrix3& rkMatrix) const {
    Matrix3 kProd;

    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++) {
            kProd.m_aafEntry[iRow][iCol] =
                m_aafEntry[iRow][0] * rkMatrix.m_aafEntry[0][iCol] +
                m_aafEntry[iRow][1] * rkMatrix.m_aafEntry[1][iCol] +
                m_aafEntry[iRow][2] * rkMatrix.m_aafEntry[2][iCol];
        }
    }

    return kProd;
}

//----------------------------------------------------------------------------
Vector3 Matrix3::operator* (const Vector3& rkPoint) const {
    Vector3 kProd;

    for (int iRow = 0; iRow < 3; iRow++) {
        kProd[iRow] =
            m_aafEntry[iRow][0] * rkPoint[0] +
            m_aafEntry[iRow][1] * rkPoint[1] +
            m_aafEntry[iRow][2] * rkPoint[2];
    }

    return kProd;
}

//----------------------------------------------------------------------------
Vector3 operator* (const Vector3& rkPoint, const Matrix3& rkMatrix) {
    Vector3 kProd;

    for (int iRow = 0; iRow < 3; iRow++) {
        kProd[iRow] =
            rkPoint[0] * rkMatrix.m_aafEntry[0][iRow] +
            rkPoint[1] * rkMatrix.m_aafEntry[1][iRow] +
            rkPoint[2] * rkMatrix.m_aafEntry[2][iRow];
    }

    return kProd;
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::operator- () const {
    Matrix3 kNeg;

    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++)
            kNeg[iRow][iCol] = -m_aafEntry[iRow][iCol];
    }

    return kNeg;
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::operator* (float fScalar) const {
    Matrix3 kProd;

    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++)
            kProd[iRow][iCol] = fScalar * m_aafEntry[iRow][iCol];
    }

    return kProd;
}

//----------------------------------------------------------------------------
Matrix3 operator* (double fScalar, const Matrix3& rkMatrix) {
    Matrix3 kProd;

    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++) {
            kProd[iRow][iCol] = fScalar * rkMatrix.m_aafEntry[iRow][iCol];
        }
    }

    return kProd;
}

Matrix3 operator* (float fScalar, const Matrix3& rkMatrix) {
    return (double)fScalar * rkMatrix;
}


Matrix3 operator* (int fScalar, const Matrix3& rkMatrix) {
    return (double)fScalar * rkMatrix;
}
//----------------------------------------------------------------------------
Matrix3 Matrix3::transpose () const {
    Matrix3 kTranspose;

    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++) {
            kTranspose[iRow][iCol] = m_aafEntry[iCol][iRow];
        }
    }

    return kTranspose;
}

//----------------------------------------------------------------------------
bool Matrix3::inverse (Matrix3& rkInverse, float fTolerance) const {
    // Invert a 3x3 using cofactors.  This is about 8 times faster than
    // the Numerical Recipes code which uses Gaussian elimination.

    rkInverse[0][0] = m_aafEntry[1][1] * m_aafEntry[2][2] -
                      m_aafEntry[1][2] * m_aafEntry[2][1];
    rkInverse[0][1] = m_aafEntry[0][2] * m_aafEntry[2][1] -
                      m_aafEntry[0][1] * m_aafEntry[2][2];
    rkInverse[0][2] = m_aafEntry[0][1] * m_aafEntry[1][2] -
                      m_aafEntry[0][2] * m_aafEntry[1][1];
    rkInverse[1][0] = m_aafEntry[1][2] * m_aafEntry[2][0] -
                      m_aafEntry[1][0] * m_aafEntry[2][2];
    rkInverse[1][1] = m_aafEntry[0][0] * m_aafEntry[2][2] -
                      m_aafEntry[0][2] * m_aafEntry[2][0];
    rkInverse[1][2] = m_aafEntry[0][2] * m_aafEntry[1][0] -
                      m_aafEntry[0][0] * m_aafEntry[1][2];
    rkInverse[2][0] = m_aafEntry[1][0] * m_aafEntry[2][1] -
                      m_aafEntry[1][1] * m_aafEntry[2][0];
    rkInverse[2][1] = m_aafEntry[0][1] * m_aafEntry[2][0] -
                      m_aafEntry[0][0] * m_aafEntry[2][1];
    rkInverse[2][2] = m_aafEntry[0][0] * m_aafEntry[1][1] -
                      m_aafEntry[0][1] * m_aafEntry[1][0];

    float fDet =
        m_aafEntry[0][0] * rkInverse[0][0] +
        m_aafEntry[0][1] * rkInverse[1][0] +
        m_aafEntry[0][2] * rkInverse[2][0];

    if ( G3D::abs(fDet) <= fTolerance )
        return false;

    float fInvDet = 1.0 / fDet;

    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++)
            rkInverse[iRow][iCol] *= fInvDet;
    }

    return true;
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::inverse (float fTolerance) const {
    Matrix3 kInverse = Matrix3::zero();
    inverse(kInverse, fTolerance);
    return kInverse;
}

//----------------------------------------------------------------------------
float Matrix3::determinant () const {
    float fCofactor00 = m_aafEntry[1][1] * m_aafEntry[2][2] -
                       m_aafEntry[1][2] * m_aafEntry[2][1];
    float fCofactor10 = m_aafEntry[1][2] * m_aafEntry[2][0] -
                       m_aafEntry[1][0] * m_aafEntry[2][2];
    float fCofactor20 = m_aafEntry[1][0] * m_aafEntry[2][1] -
                       m_aafEntry[1][1] * m_aafEntry[2][0];

    float fDet =
        m_aafEntry[0][0] * fCofactor00 +
        m_aafEntry[0][1] * fCofactor10 +
        m_aafEntry[0][2] * fCofactor20;

    return fDet;
}

//----------------------------------------------------------------------------
void Matrix3::bidiagonalize (Matrix3& kA, Matrix3& kL,
                             Matrix3& kR) {
    float afV[3], afW[3];
    float fLength, fSign, fT1, fInvT1, fT2;
    bool bIdentity;

    // map first column to (*,0,0)
    fLength = sqrt(kA[0][0] * kA[0][0] + kA[1][0] * kA[1][0] +
                         kA[2][0] * kA[2][0]);

    if ( fLength > 0.0 ) {
        fSign = (kA[0][0] > 0.0 ? 1.0 : -1.0);
        fT1 = kA[0][0] + fSign * fLength;
        fInvT1 = 1.0 / fT1;
        afV[1] = kA[1][0] * fInvT1;
        afV[2] = kA[2][0] * fInvT1;

        fT2 = -2.0 / (1.0 + afV[1] * afV[1] + afV[2] * afV[2]);
        afW[0] = fT2 * (kA[0][0] + kA[1][0] * afV[1] + kA[2][0] * afV[2]);
        afW[1] = fT2 * (kA[0][1] + kA[1][1] * afV[1] + kA[2][1] * afV[2]);
        afW[2] = fT2 * (kA[0][2] + kA[1][2] * afV[1] + kA[2][2] * afV[2]);
        kA[0][0] += afW[0];
        kA[0][1] += afW[1];
        kA[0][2] += afW[2];
        kA[1][1] += afV[1] * afW[1];
        kA[1][2] += afV[1] * afW[2];
        kA[2][1] += afV[2] * afW[1];
        kA[2][2] += afV[2] * afW[2];

        kL[0][0] = 1.0 + fT2;
        kL[0][1] = kL[1][0] = fT2 * afV[1];
        kL[0][2] = kL[2][0] = fT2 * afV[2];
        kL[1][1] = 1.0 + fT2 * afV[1] * afV[1];
        kL[1][2] = kL[2][1] = fT2 * afV[1] * afV[2];
        kL[2][2] = 1.0 + fT2 * afV[2] * afV[2];
        bIdentity = false;
    } else {
        kL = Matrix3::identity();
        bIdentity = true;
    }

    // map first row to (*,*,0)
    fLength = sqrt(kA[0][1] * kA[0][1] + kA[0][2] * kA[0][2]);

    if ( fLength > 0.0 ) {
        fSign = (kA[0][1] > 0.0 ? 1.0 : -1.0);
        fT1 = kA[0][1] + fSign * fLength;
        afV[2] = kA[0][2] / fT1;

        fT2 = -2.0 / (1.0 + afV[2] * afV[2]);
        afW[0] = fT2 * (kA[0][1] + kA[0][2] * afV[2]);
        afW[1] = fT2 * (kA[1][1] + kA[1][2] * afV[2]);
        afW[2] = fT2 * (kA[2][1] + kA[2][2] * afV[2]);
        kA[0][1] += afW[0];
        kA[1][1] += afW[1];
        kA[1][2] += afW[1] * afV[2];
        kA[2][1] += afW[2];
        kA[2][2] += afW[2] * afV[2];

        kR[0][0] = 1.0;
        kR[0][1] = kR[1][0] = 0.0;
        kR[0][2] = kR[2][0] = 0.0;
        kR[1][1] = 1.0 + fT2;
        kR[1][2] = kR[2][1] = fT2 * afV[2];
        kR[2][2] = 1.0 + fT2 * afV[2] * afV[2];
    } else {
        kR = Matrix3::identity();
    }

    // map second column to (*,*,0)
    fLength = sqrt(kA[1][1] * kA[1][1] + kA[2][1] * kA[2][1]);

    if ( fLength > 0.0 ) {
        fSign = (kA[1][1] > 0.0 ? 1.0 : -1.0);
        fT1 = kA[1][1] + fSign * fLength;
        afV[2] = kA[2][1] / fT1;

        fT2 = -2.0 / (1.0 + afV[2] * afV[2]);
        afW[1] = fT2 * (kA[1][1] + kA[2][1] * afV[2]);
        afW[2] = fT2 * (kA[1][2] + kA[2][2] * afV[2]);
        kA[1][1] += afW[1];
        kA[1][2] += afW[2];
        kA[2][2] += afV[2] * afW[2];

        float fA = 1.0 + fT2;
        float fB = fT2 * afV[2];
        float fC = 1.0 + fB * afV[2];

        if ( bIdentity ) {
            kL[0][0] = 1.0;
            kL[0][1] = kL[1][0] = 0.0;
            kL[0][2] = kL[2][0] = 0.0;
            kL[1][1] = fA;
            kL[1][2] = kL[2][1] = fB;
            kL[2][2] = fC;
        } else {
            for (int iRow = 0; iRow < 3; iRow++) {
                float fTmp0 = kL[iRow][1];
                float fTmp1 = kL[iRow][2];
                kL[iRow][1] = fA * fTmp0 + fB * fTmp1;
                kL[iRow][2] = fB * fTmp0 + fC * fTmp1;
            }
        }
    }
}

//----------------------------------------------------------------------------
void Matrix3::golubKahanStep (Matrix3& kA, Matrix3& kL,
                              Matrix3& kR) {
    float fT11 = kA[0][1] * kA[0][1] + kA[1][1] * kA[1][1];
    float fT22 = kA[1][2] * kA[1][2] + kA[2][2] * kA[2][2];
    float fT12 = kA[1][1] * kA[1][2];
    float fTrace = fT11 + fT22;
    float fDiff = fT11 - fT22;
    float fDiscr = sqrt(fDiff * fDiff + 4.0 * fT12 * fT12);
    float fRoot1 = 0.5 * (fTrace + fDiscr);
    float fRoot2 = 0.5 * (fTrace - fDiscr);

    // adjust right
    float fY = kA[0][0] - (G3D::abs(fRoot1 - fT22) <=
                          G3D::abs(fRoot2 - fT22) ? fRoot1 : fRoot2);
    float fZ = kA[0][1];
    float fInvLength = 1.0 / sqrt(fY * fY + fZ * fZ);
    float fSin = fZ * fInvLength;
    float fCos = -fY * fInvLength;

    float fTmp0 = kA[0][0];
    float fTmp1 = kA[0][1];
    kA[0][0] = fCos * fTmp0 - fSin * fTmp1;
    kA[0][1] = fSin * fTmp0 + fCos * fTmp1;
    kA[1][0] = -fSin * kA[1][1];
    kA[1][1] *= fCos;

    int iRow;

    for (iRow = 0; iRow < 3; iRow++) {
        fTmp0 = kR[0][iRow];
        fTmp1 = kR[1][iRow];
        kR[0][iRow] = fCos * fTmp0 - fSin * fTmp1;
        kR[1][iRow] = fSin * fTmp0 + fCos * fTmp1;
    }

    // adjust left
    fY = kA[0][0];

    fZ = kA[1][0];

    fInvLength = 1.0 / sqrt(fY * fY + fZ * fZ);

    fSin = fZ * fInvLength;

    fCos = -fY * fInvLength;

    kA[0][0] = fCos * kA[0][0] - fSin * kA[1][0];

    fTmp0 = kA[0][1];

    fTmp1 = kA[1][1];

    kA[0][1] = fCos * fTmp0 - fSin * fTmp1;

    kA[1][1] = fSin * fTmp0 + fCos * fTmp1;

    kA[0][2] = -fSin * kA[1][2];

    kA[1][2] *= fCos;

    int iCol;

    for (iCol = 0; iCol < 3; iCol++) {
        fTmp0 = kL[iCol][0];
        fTmp1 = kL[iCol][1];
        kL[iCol][0] = fCos * fTmp0 - fSin * fTmp1;
        kL[iCol][1] = fSin * fTmp0 + fCos * fTmp1;
    }

    // adjust right
    fY = kA[0][1];

    fZ = kA[0][2];

    fInvLength = 1.0 / sqrt(fY * fY + fZ * fZ);

    fSin = fZ * fInvLength;

    fCos = -fY * fInvLength;

    kA[0][1] = fCos * kA[0][1] - fSin * kA[0][2];

    fTmp0 = kA[1][1];

    fTmp1 = kA[1][2];

    kA[1][1] = fCos * fTmp0 - fSin * fTmp1;

    kA[1][2] = fSin * fTmp0 + fCos * fTmp1;

    kA[2][1] = -fSin * kA[2][2];

    kA[2][2] *= fCos;

    for (iRow = 0; iRow < 3; iRow++) {
        fTmp0 = kR[1][iRow];
        fTmp1 = kR[2][iRow];
        kR[1][iRow] = fCos * fTmp0 - fSin * fTmp1;
        kR[2][iRow] = fSin * fTmp0 + fCos * fTmp1;
    }

    // adjust left
    fY = kA[1][1];

    fZ = kA[2][1];

    fInvLength = 1.0 / sqrt(fY * fY + fZ * fZ);

    fSin = fZ * fInvLength;

    fCos = -fY * fInvLength;

    kA[1][1] = fCos * kA[1][1] - fSin * kA[2][1];

    fTmp0 = kA[1][2];

    fTmp1 = kA[2][2];

    kA[1][2] = fCos * fTmp0 - fSin * fTmp1;

    kA[2][2] = fSin * fTmp0 + fCos * fTmp1;

    for (iCol = 0; iCol < 3; iCol++) {
        fTmp0 = kL[iCol][1];
        fTmp1 = kL[iCol][2];
        kL[iCol][1] = fCos * fTmp0 - fSin * fTmp1;
        kL[iCol][2] = fSin * fTmp0 + fCos * fTmp1;
    }
}

//----------------------------------------------------------------------------
void Matrix3::singularValueDecomposition (Matrix3& kL, Vector3& kS,
        Matrix3& kR) const {
    int iRow, iCol;

    Matrix3 kA = *this;
    bidiagonalize(kA, kL, kR);

    for (int i = 0; i < ms_iSvdMaxIterations; i++) {
        float fTmp, fTmp0, fTmp1;
        float fSin0, fCos0, fTan0;
        float fSin1, fCos1, fTan1;

        bool bTest1 = (G3D::abs(kA[0][1]) <=
                       ms_fSvdEpsilon * (G3D::abs(kA[0][0]) + G3D::abs(kA[1][1])));
        bool bTest2 = (G3D::abs(kA[1][2]) <=
                       ms_fSvdEpsilon * (G3D::abs(kA[1][1]) + G3D::abs(kA[2][2])));

        if ( bTest1 ) {
            if ( bTest2 ) {
                kS[0] = kA[0][0];
                kS[1] = kA[1][1];
                kS[2] = kA[2][2];
                break;
            } else {
                // 2x2 closed form factorization
                fTmp = (kA[1][1] * kA[1][1] - kA[2][2] * kA[2][2] +
                        kA[1][2] * kA[1][2]) / (kA[1][2] * kA[2][2]);
                fTan0 = 0.5 * (fTmp + sqrt(fTmp * fTmp + 4.0));
                fCos0 = 1.0 / sqrt(1.0 + fTan0 * fTan0);
                fSin0 = fTan0 * fCos0;

                for (iCol = 0; iCol < 3; iCol++) {
                    fTmp0 = kL[iCol][1];
                    fTmp1 = kL[iCol][2];
                    kL[iCol][1] = fCos0 * fTmp0 - fSin0 * fTmp1;
                    kL[iCol][2] = fSin0 * fTmp0 + fCos0 * fTmp1;
                }

                fTan1 = (kA[1][2] - kA[2][2] * fTan0) / kA[1][1];
                fCos1 = 1.0 / sqrt(1.0 + fTan1 * fTan1);
                fSin1 = -fTan1 * fCos1;

                for (iRow = 0; iRow < 3; iRow++) {
                    fTmp0 = kR[1][iRow];
                    fTmp1 = kR[2][iRow];
                    kR[1][iRow] = fCos1 * fTmp0 - fSin1 * fTmp1;
                    kR[2][iRow] = fSin1 * fTmp0 + fCos1 * fTmp1;
                }

                kS[0] = kA[0][0];
                kS[1] = fCos0 * fCos1 * kA[1][1] -
                        fSin1 * (fCos0 * kA[1][2] - fSin0 * kA[2][2]);
                kS[2] = fSin0 * fSin1 * kA[1][1] +
                        fCos1 * (fSin0 * kA[1][2] + fCos0 * kA[2][2]);
                break;
            }
        } else {
            if ( bTest2 ) {
                // 2x2 closed form factorization
                fTmp = (kA[0][0] * kA[0][0] + kA[1][1] * kA[1][1] -
                        kA[0][1] * kA[0][1]) / (kA[0][1] * kA[1][1]);
                fTan0 = 0.5 * ( -fTmp + sqrt(fTmp * fTmp + 4.0));
                fCos0 = 1.0 / sqrt(1.0 + fTan0 * fTan0);
                fSin0 = fTan0 * fCos0;

                for (iCol = 0; iCol < 3; iCol++) {
                    fTmp0 = kL[iCol][0];
                    fTmp1 = kL[iCol][1];
                    kL[iCol][0] = fCos0 * fTmp0 - fSin0 * fTmp1;
                    kL[iCol][1] = fSin0 * fTmp0 + fCos0 * fTmp1;
                }

                fTan1 = (kA[0][1] - kA[1][1] * fTan0) / kA[0][0];
                fCos1 = 1.0 / sqrt(1.0 + fTan1 * fTan1);
                fSin1 = -fTan1 * fCos1;

                for (iRow = 0; iRow < 3; iRow++) {
                    fTmp0 = kR[0][iRow];
                    fTmp1 = kR[1][iRow];
                    kR[0][iRow] = fCos1 * fTmp0 - fSin1 * fTmp1;
                    kR[1][iRow] = fSin1 * fTmp0 + fCos1 * fTmp1;
                }

                kS[0] = fCos0 * fCos1 * kA[0][0] -
                        fSin1 * (fCos0 * kA[0][1] - fSin0 * kA[1][1]);
                kS[1] = fSin0 * fSin1 * kA[0][0] +
                        fCos1 * (fSin0 * kA[0][1] + fCos0 * kA[1][1]);
                kS[2] = kA[2][2];
                break;
            } else {
                golubKahanStep(kA, kL, kR);
            }
        }
    }

    // positize diagonal
    for (iRow = 0; iRow < 3; iRow++) {
        if ( kS[iRow] < 0.0 ) {
            kS[iRow] = -kS[iRow];

            for (iCol = 0; iCol < 3; iCol++)
                kR[iRow][iCol] = -kR[iRow][iCol];
        }
    }
}

//----------------------------------------------------------------------------
void Matrix3::singularValueComposition (const Matrix3& kL,
                                        const Vector3& kS, const Matrix3& kR) {
    int iRow, iCol;
    Matrix3 kTmp;

    // product S*R
    for (iRow = 0; iRow < 3; iRow++) {
        for (iCol = 0; iCol < 3; iCol++)
            kTmp[iRow][iCol] = kS[iRow] * kR[iRow][iCol];
    }

    // product L*S*R
    for (iRow = 0; iRow < 3; iRow++) {
        for (iCol = 0; iCol < 3; iCol++) {
            m_aafEntry[iRow][iCol] = 0.0;

            for (int iMid = 0; iMid < 3; iMid++)
                m_aafEntry[iRow][iCol] += kL[iRow][iMid] * kTmp[iMid][iCol];
        }
    }
}

//----------------------------------------------------------------------------
void Matrix3::orthonormalize () {
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // compute q0
    float fInvLength = 1.0 / sqrt(m_aafEntry[0][0] * m_aafEntry[0][0]
                                       + m_aafEntry[1][0] * m_aafEntry[1][0] +
                                       m_aafEntry[2][0] * m_aafEntry[2][0]);

    m_aafEntry[0][0] *= fInvLength;
    m_aafEntry[1][0] *= fInvLength;
    m_aafEntry[2][0] *= fInvLength;

    // compute q1
    float fDot0 =
        m_aafEntry[0][0] * m_aafEntry[0][1] +
        m_aafEntry[1][0] * m_aafEntry[1][1] +
        m_aafEntry[2][0] * m_aafEntry[2][1];

    m_aafEntry[0][1] -= fDot0 * m_aafEntry[0][0];
    m_aafEntry[1][1] -= fDot0 * m_aafEntry[1][0];
    m_aafEntry[2][1] -= fDot0 * m_aafEntry[2][0];

    fInvLength = 1.0 / sqrt(m_aafEntry[0][1] * m_aafEntry[0][1] +
                                  m_aafEntry[1][1] * m_aafEntry[1][1] +
                                  m_aafEntry[2][1] * m_aafEntry[2][1]);

    m_aafEntry[0][1] *= fInvLength;
    m_aafEntry[1][1] *= fInvLength;
    m_aafEntry[2][1] *= fInvLength;

    // compute q2
    float fDot1 =
        m_aafEntry[0][1] * m_aafEntry[0][2] +
        m_aafEntry[1][1] * m_aafEntry[1][2] +
        m_aafEntry[2][1] * m_aafEntry[2][2];

    fDot0 =
        m_aafEntry[0][0] * m_aafEntry[0][2] +
        m_aafEntry[1][0] * m_aafEntry[1][2] +
        m_aafEntry[2][0] * m_aafEntry[2][2];

    m_aafEntry[0][2] -= fDot0 * m_aafEntry[0][0] + fDot1 * m_aafEntry[0][1];
    m_aafEntry[1][2] -= fDot0 * m_aafEntry[1][0] + fDot1 * m_aafEntry[1][1];
    m_aafEntry[2][2] -= fDot0 * m_aafEntry[2][0] + fDot1 * m_aafEntry[2][1];

    fInvLength = 1.0 / sqrt(m_aafEntry[0][2] * m_aafEntry[0][2] +
                                  m_aafEntry[1][2] * m_aafEntry[1][2] +
                                  m_aafEntry[2][2] * m_aafEntry[2][2]);

    m_aafEntry[0][2] *= fInvLength;
    m_aafEntry[1][2] *= fInvLength;
    m_aafEntry[2][2] *= fInvLength;
}

//----------------------------------------------------------------------------
void Matrix3::qDUDecomposition (Matrix3& kQ,
                                Vector3& kD, Vector3& kU) const {
    // Factor M = QR = QDU where Q is orthogonal, D is diagonal,
    // and U is upper triangular with ones on its diagonal.  Algorithm uses
    // Gram-Schmidt orthogonalization (the QR algorithm).
    //
    // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.  The matrix R has entries
    //
    //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
    //   r10 = 0      r11 = q1*m1  r12 = q1*m2
    //   r20 = 0      r21 = 0      r22 = q2*m2
    //
    // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
    // u02 = r02/r00, and u12 = r12/r11.

    // Q = rotation
    // D = scaling
    // U = shear

    // D stores the three diagonal entries r00, r11, r22
    // U stores the entries U[0] = u01, U[1] = u02, U[2] = u12

    // build orthogonal matrix Q
    float fInvLength = 1.0 / sqrt(m_aafEntry[0][0] * m_aafEntry[0][0]
                                       + m_aafEntry[1][0] * m_aafEntry[1][0] +
                                       m_aafEntry[2][0] * m_aafEntry[2][0]);
    kQ[0][0] = m_aafEntry[0][0] * fInvLength;
    kQ[1][0] = m_aafEntry[1][0] * fInvLength;
    kQ[2][0] = m_aafEntry[2][0] * fInvLength;

    float fDot = kQ[0][0] * m_aafEntry[0][1] + kQ[1][0] * m_aafEntry[1][1] +
                kQ[2][0] * m_aafEntry[2][1];
    kQ[0][1] = m_aafEntry[0][1] - fDot * kQ[0][0];
    kQ[1][1] = m_aafEntry[1][1] - fDot * kQ[1][0];
    kQ[2][1] = m_aafEntry[2][1] - fDot * kQ[2][0];
    fInvLength = 1.0 / sqrt(kQ[0][1] * kQ[0][1] + kQ[1][1] * kQ[1][1] +
                                  kQ[2][1] * kQ[2][1]);
    kQ[0][1] *= fInvLength;
    kQ[1][1] *= fInvLength;
    kQ[2][1] *= fInvLength;

    fDot = kQ[0][0] * m_aafEntry[0][2] + kQ[1][0] * m_aafEntry[1][2] +
           kQ[2][0] * m_aafEntry[2][2];
    kQ[0][2] = m_aafEntry[0][2] - fDot * kQ[0][0];
    kQ[1][2] = m_aafEntry[1][2] - fDot * kQ[1][0];
    kQ[2][2] = m_aafEntry[2][2] - fDot * kQ[2][0];
    fDot = kQ[0][1] * m_aafEntry[0][2] + kQ[1][1] * m_aafEntry[1][2] +
           kQ[2][1] * m_aafEntry[2][2];
    kQ[0][2] -= fDot * kQ[0][1];
    kQ[1][2] -= fDot * kQ[1][1];
    kQ[2][2] -= fDot * kQ[2][1];
    fInvLength = 1.0 / sqrt(kQ[0][2] * kQ[0][2] + kQ[1][2] * kQ[1][2] +
                                  kQ[2][2] * kQ[2][2]);
    kQ[0][2] *= fInvLength;
    kQ[1][2] *= fInvLength;
    kQ[2][2] *= fInvLength;

    // guarantee that orthogonal matrix has determinant 1 (no reflections)
    float fDet = kQ[0][0] * kQ[1][1] * kQ[2][2] + kQ[0][1] * kQ[1][2] * kQ[2][0] +
                kQ[0][2] * kQ[1][0] * kQ[2][1] - kQ[0][2] * kQ[1][1] * kQ[2][0] -
                kQ[0][1] * kQ[1][0] * kQ[2][2] - kQ[0][0] * kQ[1][2] * kQ[2][1];

    if ( fDet < 0.0 ) {
        for (int iRow = 0; iRow < 3; iRow++)
            for (int iCol = 0; iCol < 3; iCol++)
                kQ[iRow][iCol] = -kQ[iRow][iCol];
    }

    // build "right" matrix R
    Matrix3 kR;

    kR[0][0] = kQ[0][0] * m_aafEntry[0][0] + kQ[1][0] * m_aafEntry[1][0] +
               kQ[2][0] * m_aafEntry[2][0];

    kR[0][1] = kQ[0][0] * m_aafEntry[0][1] + kQ[1][0] * m_aafEntry[1][1] +
               kQ[2][0] * m_aafEntry[2][1];

    kR[1][1] = kQ[0][1] * m_aafEntry[0][1] + kQ[1][1] * m_aafEntry[1][1] +
               kQ[2][1] * m_aafEntry[2][1];

    kR[0][2] = kQ[0][0] * m_aafEntry[0][2] + kQ[1][0] * m_aafEntry[1][2] +
               kQ[2][0] * m_aafEntry[2][2];

    kR[1][2] = kQ[0][1] * m_aafEntry[0][2] + kQ[1][1] * m_aafEntry[1][2] +
               kQ[2][1] * m_aafEntry[2][2];

    kR[2][2] = kQ[0][2] * m_aafEntry[0][2] + kQ[1][2] * m_aafEntry[1][2] +
               kQ[2][2] * m_aafEntry[2][2];

    // the scaling component
    kD[0] = kR[0][0];

    kD[1] = kR[1][1];

    kD[2] = kR[2][2];

    // the shear component
    float fInvD0 = 1.0 / kD[0];

    kU[0] = kR[0][1] * fInvD0;

    kU[1] = kR[0][2] * fInvD0;

    kU[2] = kR[1][2] / kD[1];
}

//----------------------------------------------------------------------------
float Matrix3::maxCubicRoot (float afCoeff[3]) {
    // Spectral norm is for A^T*A, so characteristic polynomial
    // P(x) = c[0]+c[1]*x+c[2]*x^2+x^3 has three positive float roots.
    // This yields the assertions c[0] < 0 and c[2]*c[2] >= 3*c[1].

    // quick out for uniform scale (triple root)
    const float fOneThird = 1.0 / 3.0;
    const float fEpsilon = 1e-06;
    float fDiscr = afCoeff[2] * afCoeff[2] - 3.0 * afCoeff[1];

    if ( fDiscr <= fEpsilon )
        return -fOneThird*afCoeff[2];

    // Compute an upper bound on roots of P(x).  This assumes that A^T*A
    // has been scaled by its largest entry.
    float fX = 1.0;

    float fPoly = afCoeff[0] + fX * (afCoeff[1] + fX * (afCoeff[2] + fX));

    if ( fPoly < 0.0 ) {
        // uses a matrix norm to find an upper bound on maximum root
        fX = G3D::abs(afCoeff[0]);
        float fTmp = 1.0 + G3D::abs(afCoeff[1]);

        if ( fTmp > fX )
            fX = fTmp;

        fTmp = 1.0 + G3D::abs(afCoeff[2]);

        if ( fTmp > fX )
            fX = fTmp;
    }

    // Newton's method to find root
    float fTwoC2 = 2.0 * afCoeff[2];

    for (int i = 0; i < 16; i++) {
        fPoly = afCoeff[0] + fX * (afCoeff[1] + fX * (afCoeff[2] + fX));

        if ( G3D::abs(fPoly) <= fEpsilon )
            return fX;

        float fDeriv = afCoeff[1] + fX * (fTwoC2 + 3.0 * fX);

        fX -= fPoly / fDeriv;
    }

    return fX;
}

//----------------------------------------------------------------------------
float Matrix3::spectralNorm () const {
    Matrix3 kP;
    int iRow, iCol;
    float fPmax = 0.0;

    for (iRow = 0; iRow < 3; iRow++) {
        for (iCol = 0; iCol < 3; iCol++) {
            kP[iRow][iCol] = 0.0;

            for (int iMid = 0; iMid < 3; iMid++) {
                kP[iRow][iCol] +=
                    m_aafEntry[iMid][iRow] * m_aafEntry[iMid][iCol];
            }

            if ( kP[iRow][iCol] > fPmax )
                fPmax = kP[iRow][iCol];
        }
    }

    float fInvPmax = 1.0 / fPmax;

    for (iRow = 0; iRow < 3; iRow++) {
        for (iCol = 0; iCol < 3; iCol++)
            kP[iRow][iCol] *= fInvPmax;
    }

    float afCoeff[3];
    afCoeff[0] = -(kP[0][0] * (kP[1][1] * kP[2][2] - kP[1][2] * kP[2][1]) +
                   kP[0][1] * (kP[2][0] * kP[1][2] - kP[1][0] * kP[2][2]) +
                   kP[0][2] * (kP[1][0] * kP[2][1] - kP[2][0] * kP[1][1]));
    afCoeff[1] = kP[0][0] * kP[1][1] - kP[0][1] * kP[1][0] +
                 kP[0][0] * kP[2][2] - kP[0][2] * kP[2][0] +
                 kP[1][1] * kP[2][2] - kP[1][2] * kP[2][1];
    afCoeff[2] = -(kP[0][0] + kP[1][1] + kP[2][2]);

    float fRoot = maxCubicRoot(afCoeff);
    float fNorm = sqrt(fPmax * fRoot);
    return fNorm;
}

//----------------------------------------------------------------------------
void Matrix3::toAxisAngle (Vector3& rkAxis, float& rfRadians) const {
    // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
    // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 -z +y |
    //       | +z  0 -x |
    //       | -y +x  0 |
    //       +-        -+
    //
    // If A > 0, R represents a counterclockwise rotation about the axis in
    // the sense of looking from the tip of the axis vector towards the
    // origin.  Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
    //
    // In the event that A = pi, R-R^t = 0 which prevents us from extracting
    // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
    // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
    // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
    // it does not matter which sign you choose on the square roots.

    float fTrace = m_aafEntry[0][0] + m_aafEntry[1][1] + m_aafEntry[2][2];
    float fCos = 0.5 * (fTrace - 1.0);
    rfRadians = G3D::aCos(fCos);  // in [0,PI]

    if ( rfRadians > 0.0 ) {
        if ( rfRadians < G3D_PI ) {
            rkAxis.x = m_aafEntry[2][1] - m_aafEntry[1][2];
            rkAxis.y = m_aafEntry[0][2] - m_aafEntry[2][0];
            rkAxis.z = m_aafEntry[1][0] - m_aafEntry[0][1];
            rkAxis.unitize();
        } else {
            // angle is PI
            float fHalfInverse;

            if ( m_aafEntry[0][0] >= m_aafEntry[1][1] ) {
                // r00 >= r11
                if ( m_aafEntry[0][0] >= m_aafEntry[2][2] ) {
                    // r00 is maximum diagonal term
                    rkAxis.x = 0.5 * sqrt(m_aafEntry[0][0] -
                                                m_aafEntry[1][1] - m_aafEntry[2][2] + 1.0);
                    fHalfInverse = 0.5 / rkAxis.x;
                    rkAxis.y = fHalfInverse * m_aafEntry[0][1];
                    rkAxis.z = fHalfInverse * m_aafEntry[0][2];
                } else {
                    // r22 is maximum diagonal term
                    rkAxis.z = 0.5 * sqrt(m_aafEntry[2][2] -
                                                m_aafEntry[0][0] - m_aafEntry[1][1] + 1.0);
                    fHalfInverse = 0.5 / rkAxis.z;
                    rkAxis.x = fHalfInverse * m_aafEntry[0][2];
                    rkAxis.y = fHalfInverse * m_aafEntry[1][2];
                }
            } else {
                // r11 > r00
                if ( m_aafEntry[1][1] >= m_aafEntry[2][2] ) {
                    // r11 is maximum diagonal term
                    rkAxis.y = 0.5 * sqrt(m_aafEntry[1][1] -
                                                m_aafEntry[0][0] - m_aafEntry[2][2] + 1.0);
                    fHalfInverse = 0.5 / rkAxis.y;
                    rkAxis.x = fHalfInverse * m_aafEntry[0][1];
                    rkAxis.z = fHalfInverse * m_aafEntry[1][2];
                } else {
                    // r22 is maximum diagonal term
                    rkAxis.z = 0.5 * sqrt(m_aafEntry[2][2] -
                                                m_aafEntry[0][0] - m_aafEntry[1][1] + 1.0);
                    fHalfInverse = 0.5 / rkAxis.z;
                    rkAxis.x = fHalfInverse * m_aafEntry[0][2];
                    rkAxis.y = fHalfInverse * m_aafEntry[1][2];
                }
            }
        }
    } else {
        // The angle is 0 and the matrix is the identity.  Any axis will
        // work, so just use the x-axis.
        rkAxis.x = 1.0;
        rkAxis.y = 0.0;
        rkAxis.z = 0.0;
    }
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::fromAxisAngle (const Vector3& rkAxis, float fRadians) {
    Matrix3 m;

    float fCos = cos(fRadians);
    float fSin = sin(fRadians);
    float fOneMinusCos = 1.0 - fCos;
    float fX2 = rkAxis.x * rkAxis.x;
    float fY2 = rkAxis.y * rkAxis.y;
    float fZ2 = rkAxis.z * rkAxis.z;
    float fXYM = rkAxis.x * rkAxis.y * fOneMinusCos;
    float fXZM = rkAxis.x * rkAxis.z * fOneMinusCos;
    float fYZM = rkAxis.y * rkAxis.z * fOneMinusCos;
    float fXSin = rkAxis.x * fSin;
    float fYSin = rkAxis.y * fSin;
    float fZSin = rkAxis.z * fSin;

    m.m_aafEntry[0][0] = fX2 * fOneMinusCos + fCos;
    m.m_aafEntry[0][1] = fXYM - fZSin;
    m.m_aafEntry[0][2] = fXZM + fYSin;
    m.m_aafEntry[1][0] = fXYM + fZSin;
    m.m_aafEntry[1][1] = fY2 * fOneMinusCos + fCos;
    m.m_aafEntry[1][2] = fYZM - fXSin;
    m.m_aafEntry[2][0] = fXZM - fYSin;
    m.m_aafEntry[2][1] = fYZM + fXSin;
    m.m_aafEntry[2][2] = fZ2 * fOneMinusCos + fCos;

    return m;
}

//----------------------------------------------------------------------------
bool Matrix3::toEulerAnglesXYZ (float& rfXAngle, float& rfYAngle,
                                float& rfZAngle) const {
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    if ( m_aafEntry[0][2] < 1.0 ) {
        if ( m_aafEntry[0][2] > -1.0 ) {
            rfXAngle = G3D::aTan2( -m_aafEntry[1][2], m_aafEntry[2][2]);
            rfYAngle = (float) G3D::aSin(m_aafEntry[0][2]);
            rfZAngle = G3D::aTan2( -m_aafEntry[0][1], m_aafEntry[0][0]);
            return true;
        } else {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            rfXAngle = -G3D::aTan2(m_aafEntry[1][0], m_aafEntry[1][1]);
            rfYAngle = -G3D_HALF_PI;
            rfZAngle = 0.0;
            return false;
        }
    } else {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        rfXAngle = G3D::aTan2(m_aafEntry[1][0], m_aafEntry[1][1]);
        rfYAngle = G3D_HALF_PI;
        rfZAngle = 0.0;
        return false;
    }
}

//----------------------------------------------------------------------------
bool Matrix3::toEulerAnglesXZY (float& rfXAngle, float& rfZAngle,
                                float& rfYAngle) const {
    // rot =  cy*cz          -sz              cz*sy
    //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
    //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

    if ( m_aafEntry[0][1] < 1.0 ) {
        if ( m_aafEntry[0][1] > -1.0 ) {
            rfXAngle = G3D::aTan2(m_aafEntry[2][1], m_aafEntry[1][1]);
            rfZAngle = (float) asin( -m_aafEntry[0][1]);
            rfYAngle = G3D::aTan2(m_aafEntry[0][2], m_aafEntry[0][0]);
            return true;
        } else {
            // WARNING.  Not unique.  XA - YA = atan2(r20,r22)
            rfXAngle = G3D::aTan2(m_aafEntry[2][0], m_aafEntry[2][2]);
            rfZAngle = G3D_HALF_PI;
            rfYAngle = 0.0;
            return false;
        }
    } else {
        // WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
        rfXAngle = G3D::aTan2( -m_aafEntry[2][0], m_aafEntry[2][2]);
        rfZAngle = -G3D_HALF_PI;
        rfYAngle = 0.0;
        return false;
    }
}

//----------------------------------------------------------------------------
bool Matrix3::toEulerAnglesYXZ (float& rfYAngle, float& rfXAngle,
                                float& rfZAngle) const {
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

    if ( m_aafEntry[1][2] < 1.0 ) {
        if ( m_aafEntry[1][2] > -1.0 ) {
            rfYAngle = G3D::aTan2(m_aafEntry[0][2], m_aafEntry[2][2]);
            rfXAngle = (float) asin( -m_aafEntry[1][2]);
            rfZAngle = G3D::aTan2(m_aafEntry[1][0], m_aafEntry[1][1]);
            return true;
        } else {
            // WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
            rfYAngle = G3D::aTan2(m_aafEntry[0][1], m_aafEntry[0][0]);
            rfXAngle = G3D_HALF_PI;
            rfZAngle = 0.0;
            return false;
        }
    } else {
        // WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
        rfYAngle = G3D::aTan2( -m_aafEntry[0][1], m_aafEntry[0][0]);
        rfXAngle = -G3D_HALF_PI;
        rfZAngle = 0.0;
        return false;
    }
}

//----------------------------------------------------------------------------
bool Matrix3::toEulerAnglesYZX (float& rfYAngle, float& rfZAngle,
                                float& rfXAngle) const {
    // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
    //        sz              cx*cz          -cz*sx
    //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

    if ( m_aafEntry[1][0] < 1.0 ) {
        if ( m_aafEntry[1][0] > -1.0 ) {
            rfYAngle = G3D::aTan2( -m_aafEntry[2][0], m_aafEntry[0][0]);
            rfZAngle = (float) asin(m_aafEntry[1][0]);
            rfXAngle = G3D::aTan2( -m_aafEntry[1][2], m_aafEntry[1][1]);
            return true;
        } else {
            // WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
            rfYAngle = -G3D::aTan2(m_aafEntry[2][1], m_aafEntry[2][2]);
            rfZAngle = -G3D_HALF_PI;
            rfXAngle = 0.0;
            return false;
        }
    } else {
        // WARNING.  Not unique.  YA + XA = atan2(r21,r22)
        rfYAngle = G3D::aTan2(m_aafEntry[2][1], m_aafEntry[2][2]);
        rfZAngle = G3D_HALF_PI;
        rfXAngle = 0.0;
        return false;
    }
}

//----------------------------------------------------------------------------
bool Matrix3::toEulerAnglesZXY (float& rfZAngle, float& rfXAngle,
                                float& rfYAngle) const {
    // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
    //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
    //       -cx*sy           sx              cx*cy

    if ( m_aafEntry[2][1] < 1.0 ) {
        if ( m_aafEntry[2][1] > -1.0 ) {
            rfZAngle = G3D::aTan2( -m_aafEntry[0][1], m_aafEntry[1][1]);
            rfXAngle = (float) asin(m_aafEntry[2][1]);
            rfYAngle = G3D::aTan2( -m_aafEntry[2][0], m_aafEntry[2][2]);
            return true;
        } else {
            // WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
            rfZAngle = -G3D::aTan2(m_aafEntry[0][2], m_aafEntry[0][0]);
            rfXAngle = -G3D_HALF_PI;
            rfYAngle = 0.0;
            return false;
        }
    } else {
        // WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
        rfZAngle = G3D::aTan2(m_aafEntry[0][2], m_aafEntry[0][0]);
        rfXAngle = G3D_HALF_PI;
        rfYAngle = 0.0;
        return false;
    }
}

//----------------------------------------------------------------------------
bool Matrix3::toEulerAnglesZYX (float& rfZAngle, float& rfYAngle,
                                float& rfXAngle) const {
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy

    if ( m_aafEntry[2][0] < 1.0 ) {
        if ( m_aafEntry[2][0] > -1.0 ) {
            rfZAngle = G3D::aTan2(m_aafEntry[1][0], m_aafEntry[0][0]);
            rfYAngle = G3D::aSin(-(double)m_aafEntry[2][1]);
            rfXAngle = G3D::aTan2(m_aafEntry[2][1], m_aafEntry[2][2]);
            return true;
        } else {
            // WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
            rfZAngle = -G3D::aTan2(m_aafEntry[0][1], m_aafEntry[0][2]);
            rfYAngle = G3D_HALF_PI;
            rfXAngle = 0.0;
            return false;
        }
    } else {
        // WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
        rfZAngle = G3D::aTan2( -m_aafEntry[0][1], -m_aafEntry[0][2]);
        rfYAngle = -G3D_HALF_PI;
        rfXAngle = 0.0;
        return false;
    }
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::fromEulerAnglesXYZ (float fYAngle, float fPAngle,
                                  float fRAngle) {
    float fCos, fSin;

    fCos = cos(fYAngle);
    fSin = sin(fYAngle);
    Matrix3 kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

    fCos = cos(fPAngle);
    fSin = sin(fPAngle);
    Matrix3 kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

    fCos = cos(fRAngle);
    fSin = sin(fRAngle);
    Matrix3 kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

    return kXMat * (kYMat * kZMat);
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::fromEulerAnglesXZY (float fYAngle, float fPAngle,
                                  float fRAngle) {

    float fCos, fSin;

    fCos = cos(fYAngle);
    fSin = sin(fYAngle);
    Matrix3 kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

    fCos = cos(fPAngle);
    fSin = sin(fPAngle);
    Matrix3 kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

    fCos = cos(fRAngle);
    fSin = sin(fRAngle);
    Matrix3 kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

    return kXMat * (kZMat * kYMat);
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::fromEulerAnglesYXZ (float fYAngle, float fPAngle,
                                  float fRAngle) {
    float fCos, fSin;

    fCos = cos(fYAngle);
    fSin = sin(fYAngle);
    Matrix3 kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

    fCos = cos(fPAngle);
    fSin = sin(fPAngle);
    Matrix3 kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

    fCos = cos(fRAngle);
    fSin = sin(fRAngle);
    Matrix3 kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

    return kYMat * (kXMat * kZMat);
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::fromEulerAnglesYZX (float fYAngle, float fPAngle,
                                  float fRAngle) {
    float fCos, fSin;

    fCos = cos(fYAngle);
    fSin = sin(fYAngle);
    Matrix3 kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

    fCos = cos(fPAngle);
    fSin = sin(fPAngle);
    Matrix3 kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

    fCos = cos(fRAngle);
    fSin = sin(fRAngle);
    Matrix3 kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

    return kYMat * (kZMat * kXMat);
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::fromEulerAnglesZXY (float fYAngle, float fPAngle,
                                  float fRAngle) {
    float fCos, fSin;

    fCos = cos(fYAngle);
    fSin = sin(fYAngle);
    Matrix3 kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

    fCos = cos(fPAngle);
    fSin = sin(fPAngle);
    Matrix3 kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

    fCos = cos(fRAngle);
    fSin = sin(fRAngle);
    Matrix3 kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

    return kZMat * (kXMat * kYMat);
}

//----------------------------------------------------------------------------
Matrix3 Matrix3::fromEulerAnglesZYX (float fYAngle, float fPAngle,
                                  float fRAngle) {
    float fCos, fSin;

    fCos = cos(fYAngle);
    fSin = sin(fYAngle);
    Matrix3 kZMat(fCos, -fSin, 0.0, fSin, fCos, 0.0, 0.0, 0.0, 1.0);

    fCos = cos(fPAngle);
    fSin = sin(fPAngle);
    Matrix3 kYMat(fCos, 0.0, fSin, 0.0, 1.0, 0.0, -fSin, 0.0, fCos);

    fCos = cos(fRAngle);
    fSin = sin(fRAngle);
    Matrix3 kXMat(1.0, 0.0, 0.0, 0.0, fCos, -fSin, 0.0, fSin, fCos);

    return kZMat * (kYMat * kXMat);
}

//----------------------------------------------------------------------------
void Matrix3::tridiagonal (float afDiag[3], float afSubDiag[3]) {
    // Householder reduction T = Q^t M Q
    //   Input:
    //     mat, symmetric 3x3 matrix M
    //   Output:
    //     mat, orthogonal matrix Q
    //     diag, diagonal entries of T
    //     subd, subdiagonal entries of T (T is symmetric)

    float fA = m_aafEntry[0][0];
    float fB = m_aafEntry[0][1];
    float fC = m_aafEntry[0][2];
    float fD = m_aafEntry[1][1];
    float fE = m_aafEntry[1][2];
    float fF = m_aafEntry[2][2];

    afDiag[0] = fA;
    afSubDiag[2] = 0.0;

    if ( G3D::abs(fC) >= EPSILON ) {
        float fLength = sqrt(fB * fB + fC * fC);
        float fInvLength = 1.0 / fLength;
        fB *= fInvLength;
        fC *= fInvLength;
        float fQ = 2.0 * fB * fE + fC * (fF - fD);
        afDiag[1] = fD + fC * fQ;
        afDiag[2] = fF - fC * fQ;
        afSubDiag[0] = fLength;
        afSubDiag[1] = fE - fB * fQ;
        m_aafEntry[0][0] = 1.0;
        m_aafEntry[0][1] = 0.0;
        m_aafEntry[0][2] = 0.0;
        m_aafEntry[1][0] = 0.0;
        m_aafEntry[1][1] = fB;
        m_aafEntry[1][2] = fC;
        m_aafEntry[2][0] = 0.0;
        m_aafEntry[2][1] = fC;
        m_aafEntry[2][2] = -fB;
    } else {
        afDiag[1] = fD;
        afDiag[2] = fF;
        afSubDiag[0] = fB;
        afSubDiag[1] = fE;
        m_aafEntry[0][0] = 1.0;
        m_aafEntry[0][1] = 0.0;
        m_aafEntry[0][2] = 0.0;
        m_aafEntry[1][0] = 0.0;
        m_aafEntry[1][1] = 1.0;
        m_aafEntry[1][2] = 0.0;
        m_aafEntry[2][0] = 0.0;
        m_aafEntry[2][1] = 0.0;
        m_aafEntry[2][2] = 1.0;
    }
}

//----------------------------------------------------------------------------
bool Matrix3::qLAlgorithm (float afDiag[3], float afSubDiag[3]) {
    // QL iteration with implicit shifting to reduce matrix from tridiagonal
    // to diagonal

    for (int i0 = 0; i0 < 3; i0++) {
        const int iMaxIter = 32;
        int iIter;

        for (iIter = 0; iIter < iMaxIter; iIter++) {
            int i1;

            for (i1 = i0; i1 <= 1; i1++) {
                float fSum = G3D::abs(afDiag[i1]) +
                            G3D::abs(afDiag[i1 + 1]);

                if ( G3D::abs(afSubDiag[i1]) + fSum == fSum )
                    break;
            }

            if ( i1 == i0 )
                break;

            float fTmp0 = (afDiag[i0 + 1] - afDiag[i0]) / (2.0 * afSubDiag[i0]);

            float fTmp1 = sqrt(fTmp0 * fTmp0 + 1.0);

            if ( fTmp0 < 0.0 )
                fTmp0 = afDiag[i1] - afDiag[i0] + afSubDiag[i0] / (fTmp0 - fTmp1);
            else
                fTmp0 = afDiag[i1] - afDiag[i0] + afSubDiag[i0] / (fTmp0 + fTmp1);

            float fSin = 1.0;

            float fCos = 1.0;

            float fTmp2 = 0.0;

            for (int i2 = i1 - 1; i2 >= i0; i2--) {
                float fTmp3 = fSin * afSubDiag[i2];
                float fTmp4 = fCos * afSubDiag[i2];

                if (G3D::abs(fTmp3) >= G3D::abs(fTmp0)) {
                    fCos = fTmp0 / fTmp3;
                    fTmp1 = sqrt(fCos * fCos + 1.0);
                    afSubDiag[i2 + 1] = fTmp3 * fTmp1;
                    fSin = 1.0 / fTmp1;
                    fCos *= fSin;
                } else {
                    fSin = fTmp3 / fTmp0;
                    fTmp1 = sqrt(fSin * fSin + 1.0);
                    afSubDiag[i2 + 1] = fTmp0 * fTmp1;
                    fCos = 1.0 / fTmp1;
                    fSin *= fCos;
                }

                fTmp0 = afDiag[i2 + 1] - fTmp2;
                fTmp1 = (afDiag[i2] - fTmp0) * fSin + 2.0 * fTmp4 * fCos;
                fTmp2 = fSin * fTmp1;
                afDiag[i2 + 1] = fTmp0 + fTmp2;
                fTmp0 = fCos * fTmp1 - fTmp4;

                for (int iRow = 0; iRow < 3; iRow++) {
                    fTmp3 = m_aafEntry[iRow][i2 + 1];
                    m_aafEntry[iRow][i2 + 1] = fSin * m_aafEntry[iRow][i2] +
                                               fCos * fTmp3;
                    m_aafEntry[iRow][i2] = fCos * m_aafEntry[iRow][i2] -
                                           fSin * fTmp3;
                }
            }

            afDiag[i0] -= fTmp2;
            afSubDiag[i0] = fTmp0;
            afSubDiag[i1] = 0.0;
        }

        if ( iIter == iMaxIter ) {
            // should not get here under normal circumstances
            return false;
        }
    }

    return true;
}

//----------------------------------------------------------------------------
void Matrix3::eigenSolveSymmetric (float afEigenvalue[3],
                                   Vector3 akEigenvector[3]) const {
    Matrix3 kMatrix = *this;
    float afSubDiag[3];
    kMatrix.tridiagonal(afEigenvalue, afSubDiag);
    kMatrix.qLAlgorithm(afEigenvalue, afSubDiag);

    for (int i = 0; i < 3; i++) {
        akEigenvector[i][0] = kMatrix[0][i];
        akEigenvector[i][1] = kMatrix[1][i];
        akEigenvector[i][2] = kMatrix[2][i];
    }

    // make eigenvectors form a right--handed system
    Vector3 kCross = akEigenvector[1].cross(akEigenvector[2]);

    float fDet = akEigenvector[0].dot(kCross);

    if ( fDet < 0.0 ) {
        akEigenvector[2][0] = - akEigenvector[2][0];
        akEigenvector[2][1] = - akEigenvector[2][1];
        akEigenvector[2][2] = - akEigenvector[2][2];
    }
}

//----------------------------------------------------------------------------
void Matrix3::tensorProduct (const Vector3& rkU, const Vector3& rkV,
                             Matrix3& rkProduct) {
    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++) {
            rkProduct[iRow][iCol] = rkU[iRow] * rkV[iCol];
        }
    }
}

//----------------------------------------------------------------------------

// Runs in 52 cycles on AMD, 76 cycles on Intel Centrino
//
// The loop unrolling is necessary for performance. 
// I was unable to improve performance further by flattening the matrices
// into float*'s instead of 2D arrays.  
//
// -morgan
void Matrix3::_mul(const Matrix3& A, const Matrix3& B, Matrix3& out) {
    const float* ARowPtr = A.m_aafEntry[0];
    float* outRowPtr     = out.m_aafEntry[0];
        outRowPtr[0] =
            ARowPtr[0] * B.m_aafEntry[0][0] +
            ARowPtr[1] * B.m_aafEntry[1][0] +
            ARowPtr[2] * B.m_aafEntry[2][0];
        outRowPtr[1] =
            ARowPtr[0] * B.m_aafEntry[0][1] +
            ARowPtr[1] * B.m_aafEntry[1][1] +
            ARowPtr[2] * B.m_aafEntry[2][1];
        outRowPtr[2] =
            ARowPtr[0] * B.m_aafEntry[0][2] +
            ARowPtr[1] * B.m_aafEntry[1][2] +
            ARowPtr[2] * B.m_aafEntry[2][2];

    ARowPtr       = A.m_aafEntry[1];
    outRowPtr     = out.m_aafEntry[1];

        outRowPtr[0] =
            ARowPtr[0] * B.m_aafEntry[0][0] +
            ARowPtr[1] * B.m_aafEntry[1][0] +
            ARowPtr[2] * B.m_aafEntry[2][0];
        outRowPtr[1] =
            ARowPtr[0] * B.m_aafEntry[0][1] +
            ARowPtr[1] * B.m_aafEntry[1][1] +
            ARowPtr[2] * B.m_aafEntry[2][1];
        outRowPtr[2] =
            ARowPtr[0] * B.m_aafEntry[0][2] +
            ARowPtr[1] * B.m_aafEntry[1][2] +
            ARowPtr[2] * B.m_aafEntry[2][2];

    ARowPtr       = A.m_aafEntry[2];
    outRowPtr     = out.m_aafEntry[2];

        outRowPtr[0] =
            ARowPtr[0] * B.m_aafEntry[0][0] +
            ARowPtr[1] * B.m_aafEntry[1][0] +
            ARowPtr[2] * B.m_aafEntry[2][0];
        outRowPtr[1] =
            ARowPtr[0] * B.m_aafEntry[0][1] +
            ARowPtr[1] * B.m_aafEntry[1][1] +
            ARowPtr[2] * B.m_aafEntry[2][1];
        outRowPtr[2] =
            ARowPtr[0] * B.m_aafEntry[0][2] +
            ARowPtr[1] * B.m_aafEntry[1][2] +
            ARowPtr[2] * B.m_aafEntry[2][2];
}

//----------------------------------------------------------------------------
void Matrix3::_transpose(const Matrix3& A, Matrix3& out) {
    out[0][0] = A.m_aafEntry[0][0];
    out[0][1] = A.m_aafEntry[1][0];
    out[0][2] = A.m_aafEntry[2][0];
    out[1][0] = A.m_aafEntry[0][1];
    out[1][1] = A.m_aafEntry[1][1];
    out[1][2] = A.m_aafEntry[2][1];
    out[2][0] = A.m_aafEntry[0][2];
    out[2][1] = A.m_aafEntry[1][2];
    out[2][2] = A.m_aafEntry[2][2];
}

} // namespace

