/**
  @file glcalls.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2002-08-07
  @edited  2003-09-28
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


void glGetMatrix(GLenum name, Matrix4& m) {
    float f[16];
    glGetFloatv(name, f);
    m = Matrix4(f).transpose();
}


CoordinateFrame reflectionMatrix() {
	CoordinateFrame cframe;

	// Reflect (mirror)
	Matrix3 refl(Matrix3::IDENTITY);
	refl[0][0] = -1;

    // Read back the OpenGL transformation matrix.
    Matrix3 rot(Matrix3::ZERO);
    double glRot[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, glRot);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            rot[i][j] = glRot[i * 4 + j];
        }
    }

	cframe.rotation = refl * rot;

    return cframe;
}


void glLoadMatrix(const CoordinateFrame &cf) {
    GLdouble matrix[16];
    _getGLMatrix(matrix, cf.rotation, cf.translation);
    glLoadMatrixd(matrix);
}

void glLoadMatrix(const Matrix4& m) {
    GLfloat matrix[16];
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            // Transpose
            matrix[c * 4 + r] = m[r][c];
        }
    }

    glLoadMatrixf(matrix);
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

