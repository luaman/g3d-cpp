/**
  @file glcalls.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2002-08-07
  @edited  2003-11-05
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




/**
 Takes an object space point to screen space using the current MODELVIEW and
 PROJECTION matrices. The resulting xy values are in <B>pixels</B>, the z 
 value is on the glDepthRange scale, and the w value contains rhw (-1/z for
 camera space z), which is useful for scaling line and point size.
 */
Vector4 glToScreen(const Vector4& v) {
    
    // Get the matrices and viewport
    double modelView[16];
    double projection[16];
    double viewport[4];
    double depthRange[2];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetDoublev(GL_VIEWPORT, viewport);
    glGetDoublev(GL_DEPTH_RANGE, depthRange);

    // Compose the matrices into a single row-major transformation
    Vector4 T[4];
    int r, c, i;
    for (r = 0; r < 4; ++r) {
        for (c = 0; c < 4; ++c) {
            T[r][c] = 0;
            for (i = 0; i < 4; ++i) {
                // OpenGL matrices are column major
                T[r][c] += projection[r + i * 4] * modelView[i + c * 4];
            }
        }
    }

    // Transform the vertex
    Vector4 result;
    for (r = 0; r < 4; ++r) {
        result[r] = T[r].dot(v);
    }

    // Homogeneous divide
    const double rhw = 1 / result.w;

    return Vector4(
        (1 + result.x * rhw) * viewport[2] / 2 + viewport[0],
        (1 - result.y * rhw) * viewport[3] / 2 + viewport[1],
        (result.z * rhw) * (depthRange[1] - depthRange[0]) + depthRange[0],
        rhw);
}
} // namespace

