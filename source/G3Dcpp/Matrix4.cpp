/**
  @file Matrix4.cpp
 
 
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-10-02
  @edited  2003-11-19
 */

#include "G3D/Matrix4.h"
#include "G3D/Vector4.h"
#include "G3D/CoordinateFrame.h"

namespace G3D {

const Matrix4 Matrix4::IDENTITY
  = Matrix4(1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1);

const Matrix4 Matrix4::ZERO
  = Matrix4(0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0,
            0, 0, 0, 0);

Matrix4::Matrix4(const class CoordinateFrame& cframe) {
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            elt[r][c] = cframe.rotation[r][c];
        }
        elt[r][3] = cframe.translation[r];
    }
    elt[3][0] = 0.0f;
    elt[3][1] = 0.0f;
    elt[3][2] = 0.0f;
    elt[3][3] = 1.0f;
}


Matrix4 Matrix4::orthogonalProjection(
    double            left,
    double            right,
    double            bottom,
    double            top,
    double            nearval,
    double            farval) {

    // Adapted from Mesa
    double x, y, z;
    double tx,  ty, tz;

    x = 2.0 / (right-left);
    y = 2.0 / (top-bottom);
    z = -2.0 / (farval-nearval);
    tx = -(right+left) / (right-left);
    ty = -(top+bottom) / (top-bottom);
    tz = -(farval+nearval) / (farval-nearval);

    return 
        Matrix4( x , 0.0, 0.0,  tx,
                0.0,  y , 0.0,  ty,
                0.0, 0.0,  z ,  tz,
                0.0, 0.0, 0.0, 1.0);
}


Matrix4 Matrix4::perspectiveProjection(
    double left,    
    double right,
    double bottom,  
    double top,
    double nearval, 
    double farval) {

    double x, y, a, b, c, d;

    x = (2.0*nearval) / (right-left);
    y = (2.0*nearval) / (top-bottom);
    a = (right+left) / (right-left);
    b = (top+bottom) / (top-bottom);

    if ((float)farval >= (float)inf) {
       // Infinite view frustum
       c = -1.0;
       d = -2.0 * nearval;
    } else {
       c = -(farval+nearval) / (farval-nearval);
       d = -(2.0*farval*nearval) / (farval-nearval);
    }

    return Matrix4(
        x,  0,  a,  0,
        0,  y,  b,  0,
        0,  0,  c,  d,
        0,  0, -1,  0);
}


Matrix4::Matrix4(
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
Matrix4::Matrix4(const float* init) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            elt[r][c] = init[r * 4 + c];
        }
    }
}


Matrix4::Matrix4(const double* init) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            elt[r][c] = init[r * 4 + c];
        }
    }
}


Matrix4::Matrix4() {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            elt[r][c] = 0;
        }
    }
}


void Matrix4::setRow(int r, const Vector4& v) {
    for (int c = 0; c < 4; ++c) {
        elt[r][c] = v[c];
    }
}


void Matrix4::setColumn(int c, const Vector4& v) {
    for (int r = 0; r < 4; ++r) {
        elt[r][c] = v[r];
    }
}


Vector4 Matrix4::getRow(int r) const {
    Vector4 v;
    for (int c = 0; c < 4; ++c) {
        v[c] = elt[r][c];
    }
    return v;
}



Vector4 Matrix4::getColumn(int c) const {
    Vector4 v;
    for (int r = 0; r < 4; ++r) {
        v[r] = elt[r][c];
    }
    return v;
}


const float* Matrix4::operator[](int r) const {
    debugAssert(r >= 0);
    debugAssert(r < 4);
    return elt[r];
}


float* Matrix4::operator[](int r) {
    debugAssert(r >= 0);
    debugAssert(r < 4);
    return elt[r];
}


Matrix4 Matrix4::operator*(const Matrix4& other) const {
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


Matrix4 Matrix4::operator*(const double s) const {
    Matrix4 result;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            result.elt[r][c] = elt[r][c] * s;
        }
    }

    return result;
}


Vector4 Matrix4::operator*(const Vector4& vector) const {
    Vector4 result(0,0,0,0);
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            result[r] += elt[r][c] * vector[c];
        }
    }

    return result;
}


Matrix4 Matrix4::transpose() const {
    Matrix4 result;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            result.elt[c][r] = elt[r][c];
        }
    }

    return result;
}


bool Matrix4::operator!=(const Matrix4& other) const {
    return ! (*this == other);
}


bool Matrix4::operator==(const Matrix4& other) const {

    // If the bit patterns are identical, they must be
    // the same matrix.  If not, they *might* still have
    // equal elements due to floating point weirdness.
    if (memcmp(this, &other, sizeof(Matrix4) == 0)) {
        return true;
    } 

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (elt[r][c] != other.elt[r][c]) {
                return false;
            }
        }
    }

    return true;
}

}
