/**
  @file Matrix3.h
 
  3x3 matrix class
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @cite Portions based on Dave Eberly's Magic Software Library at <A HREF="http://www.magic-software.com">http://www.magic-software.com</A>
 
  @created 2001-06-02
  @edited  2003-09-28
 */

#ifndef G3D_MATRIX3_H
#define G3D_MATRIX3_H

#include "G3D/Vector3.h"
#include "G3D/Vector4.h"
#include "G3D/debugAssert.h"

namespace G3D {

/**
  3x3 matrix.  Do not subclass.
 */
class Matrix3 {
private:
    /**
     Constructor.  Private so there is no confusion about whether
     if is initialized, zero, or identity.
     */
    Matrix3 ();

public:

    Matrix3(class BinaryInput& b);
    Matrix3 (const G3D::Real aafEntry[3][3]);
    Matrix3 (const Matrix3& rkMatrix);
    Matrix3 (G3D::Real fEntry00, G3D::Real fEntry01, G3D::Real fEntry02,
             G3D::Real fEntry10, G3D::Real fEntry11, G3D::Real fEntry12,
             G3D::Real fEntry20, G3D::Real fEntry21, G3D::Real fEntry22);

    void serialize(class BinaryOutput& b) const;
    void deserialize(class BinaryInput& b);

    /**
     Sets all elements.
     */
    void set(G3D::Real fEntry00, G3D::Real fEntry01, G3D::Real fEntry02,
             G3D::Real fEntry10, G3D::Real fEntry11, G3D::Real fEntry12,
             G3D::Real fEntry20, G3D::Real fEntry21, G3D::Real fEntry22);

    /**
     * member access, allows use of construct mat[r][c]
     */
    G3D::Real* operator[] (int iRow) const;
    operator G3D::Real* ();
    Vector3 getColumn (int iCol) const;
    void setColumn(int iCol, const Vector3 &vector);
    void setRow(int iRow, const Vector3 &vector);

    // assignment and comparison
    Matrix3& operator= (const Matrix3& rkMatrix);
    bool operator== (const Matrix3& rkMatrix) const;
    bool operator!= (const Matrix3& rkMatrix) const;

    // arithmetic operations
    Matrix3 operator+ (const Matrix3& rkMatrix) const;
    Matrix3 operator- (const Matrix3& rkMatrix) const;
    Matrix3 operator* (const Matrix3& rkMatrix) const;
    Matrix3 operator- () const;

    /**
     * matrix * vector [3x3 * 3x1 = 3x1]
     */
    Vector3 operator* (const Vector3& rkVector) const;

    /**
     * vector * matrix [1x3 * 3x3 = 1x3]
     */
    friend Vector3 operator* (const Vector3& rkVector,
                              const Matrix3& rkMatrix);

    /**
     * matrix * scalar
     */
    Matrix3 operator* (G3D::Real fScalar) const;

    /** scalar * matrix */
    friend Matrix3 operator* (G3D::Real fScalar, const Matrix3& rkMatrix);

    // utilities
    Matrix3 transpose () const;
    bool inverse (Matrix3& rkInverse, G3D::Real fTolerance = 1e-06) const;
    Matrix3 inverse (G3D::Real fTolerance = 1e-06) const;
    G3D::Real determinant () const;

    /** singular value decomposition */
    void singularValueDecomposition (Matrix3& rkL, Vector3& rkS,
                                     Matrix3& rkR) const;
    /** singular value decomposition */
    void singularValueComposition (const Matrix3& rkL,
                                   const Vector3& rkS, const Matrix3& rkR);

    /** Gram-Schmidt orthonormalization (applied to columns of rotation matrix) */
    void orthonormalize ();

    /** orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12) */
    void qDUDecomposition (Matrix3& rkQ, Vector3& rkD,
                           Vector3& rkU) const;

    G3D::Real spectralNorm () const;

    /** matrix must be orthonormal */
    void toAxisAngle (Vector3& rkAxis, G3D::Real& rfRadians) const;
    void fromAxisAngle (const Vector3& rkAxis, G3D::Real fRadians);

    /**
     * The matrix must be orthonormal.  The decomposition is yaw*pitch*roll
     * where yaw is rotation about the Up vector, pitch is rotation about the
     * right axis, and roll is rotation about the Direction axis.
     */
    bool toEulerAnglesXYZ (float& rfYAngle, float& rfPAngle,
                           float& rfRAngle) const;
    bool toEulerAnglesXZY (float& rfYAngle, float& rfPAngle,
                           float& rfRAngle) const;
    bool toEulerAnglesYXZ (float& rfYAngle, float& rfPAngle,
                           float& rfRAngle) const;
    bool toEulerAnglesYZX (float& rfYAngle, float& rfPAngle,
                           float& rfRAngle) const;
    bool toEulerAnglesZXY (float& rfYAngle, float& rfPAngle,
                           float& rfRAngle) const;
    bool toEulerAnglesZYX (float& rfYAngle, float& rfPAngle,
                           float& rfRAngle) const;
    void fromEulerAnglesXYZ (float fYAngle, float fPAngle, float fRAngle);
    void fromEulerAnglesXZY (float fYAngle, float fPAngle, float fRAngle);
    void fromEulerAnglesYXZ (float fYAngle, float fPAngle, float fRAngle);
    void fromEulerAnglesYZX (float fYAngle, float fPAngle, float fRAngle);
    void fromEulerAnglesZXY (float fYAngle, float fPAngle, float fRAngle);
    void fromEulerAnglesZYX (float fYAngle, float fPAngle, float fRAngle);

    /** eigensolver, matrix must be symmetric */
    void eigenSolveSymmetric (G3D::Real afEigenvalue[3],
                              Vector3 akEigenvector[3]) const;

    static void tensorProduct (const Vector3& rkU, const Vector3& rkV,
                               Matrix3& rkProduct);

    static const G3D::Real EPSILON;
    static const Matrix3 ZERO;
    static const Matrix3 IDENTITY;

protected:
    // support for eigensolver
    void tridiagonal (G3D::Real afDiag[3], G3D::Real afSubDiag[3]);
    bool qLAlgorithm (G3D::Real afDiag[3], G3D::Real afSubDiag[3]);

    // support for singular value decomposition
    static const G3D::Real ms_fSvdEpsilon;
    static const int ms_iSvdMaxIterations;
    static void bidiagonalize (Matrix3& kA, Matrix3& kL,
                               Matrix3& kR);
    static void golubKahanStep (Matrix3& kA, Matrix3& kL,
                                Matrix3& kR);

    // support for spectral norm
    static G3D::Real maxCubicRoot (G3D::Real afCoeff[3]);

    G3D::Real m_aafEntry[3][3];
};


/**
 Provided for DirectX/Cg support, not full featured.  Consider G3D::CoordinateFrame
 instead.
 */
class Matrix4 {
private:

    float elt[4][4];

public:
    Matrix4(
        float r1c1, float r1c2, float r1c3, float r1c4,
        float r2c1, float r2c2, float r2c3, float r2c4,
        float r3c1, float r3c2, float r3c3, float r3c4,
        float r4c1, float r4c2, float r4c3, float r4c4) {
        elt[0][0] = r1c1;  elt[0][1] = r1c2;  elt[0][2] = r1c3;  elt[0][3] = r1c4;
        elt[1][0] = r2c1;  elt[1][1] = r2c2;  elt[1][2] = r2c3;  elt[1][3] = r2c4;
        elt[2][0] = r3c1;  elt[2][1] = r3c2;  elt[2][2] = r3c3;  elt[2][3] = r3c4;
        elt[3][0] = r4c1;  elt[3][1] = r4c2;  elt[3][2] = r4c3;  elt[3][3] = r4c4;
    }

    /**
     init should be <B>row major</B>.
     */
    Matrix4(const float* init) {
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                elt[r][c] = init[r * 4 + c];
            }
        }
    }

    Matrix4(const double* init) {
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                elt[r][c] = init[r * 4 + c];
            }
        }
    }

    Matrix4() {
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                elt[r][c] = 0;
            }
        }
    }

    const float* operator[](int r) const {
        debugAssert(r >= 0);
        debugAssert(r < 4);
        return elt[r];
    }

    float* operator[](int r) {
        debugAssert(r >= 0);
        debugAssert(r < 4);
        return elt[r];
    }

    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result;
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                for (int i = 0; i < 4; ++i) {
                    result.elt[r][c] += elt[r][i] * other.elt[i][c];
                }
            }
        }

        return result;
    }

    Matrix4 operator*(const double s) const {
        Matrix4 result;
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                result.elt[r][c] = elt[r][c] * s;
            }
        }

        return result;
    }

    Vector4 operator*(const Vector4& vector) const {
        Vector4 result(0,0,0,0);
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                result[r] += elt[r][c] * vector[c];
            }
        }

        return result;
    }
};

}; // namespace

#endif

