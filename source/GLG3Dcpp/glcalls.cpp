/**
  @file glcalls.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2002-08-07
  @edited  2003-02-15
*/

#include "GLG3D/glcalls.h"

namespace G3D {

/**
 Sets up matrix m from rot and trans
 */
static void _getGLMatrix(GLdouble* m, const Matrix3& rot, const Vector3& trans) {
    // GL wants a column major matrix
    m[0] = rot[0][0];
    m[1] = rot[1][0];
    m[2] = rot[2][0];
    m[3] = 0.0;

    m[4] = rot[0][1];
    m[5] = rot[1][1];
    m[6] = rot[2][1];
    m[7] = 0.0;

    m[8] = rot[0][2];
    m[9] = rot[1][2];
    m[10] = rot[2][2];
    m[11] = 0.0;

    m[12] = trans[0];
    m[13] = trans[1];
    m[14] = trans[2];
    m[15] = 1.0;
}

void glLoadMatrix(const CoordinateFrame &cf) {
    GLdouble matrix[16];
    _getGLMatrix(matrix, cf.rotation, cf.translation);
    glLoadMatrixd(matrix);
}

void glLoadInvMatrix(const CoordinateFrame &cf) {
    Matrix3 rotInv = cf.rotation.transpose();

    GLdouble matrix[16];
    _getGLMatrix(matrix, rotInv, rotInv * -cf.translation);
    glLoadMatrixd(matrix);
}

void glMultInvMatrix(const CoordinateFrame &cf) {
    Matrix3 rotInv = cf.rotation.transpose();

    GLdouble matrix[16];
    _getGLMatrix(matrix, rotInv, rotInv * -cf.translation);
    glMultMatrixd(matrix);
}

void glMultMatrix(const CoordinateFrame &cf) {
    GLdouble matrix[16];
    _getGLMatrix(matrix, cf.rotation, cf.translation);
    glMultMatrixd(matrix);
}

} // namespace

