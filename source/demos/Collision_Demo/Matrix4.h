/**
 @file Collision_Demo/Matrix4.h

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2003-02-07
 @edited  2003-02-07
 */

#ifndef MATRIX4_H
#define MATRIX4_H

/**
 General 4x4 matrix class.
 */
class Matrix4 {
private:
    /** r, c */
    float           elt[4][4];

public:
    
    /** Initializes to the identity matrix. */
    Matrix4() {    
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                elt[r][c] = (r == c) ? 1.0f : 0.0f;
            }
        }
    }

    /**
     Constructs an orthogonal projection matrix from the given parameters.
     */
    static Matrix4 orthogonalProjectionMatrix(
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

    Matrix4(
        double r0c0, double r0c1, double r0c2, double r0c3,
        double r1c0, double r1c1, double r1c2, double r1c3,
        double r2c0, double r2c1, double r2c2, double r2c3,
        double r3c0, double r3c1, double r3c2, double r3c3) {

        elt[0][0] = r0c0; elt[0][1] = r0c1; elt[0][2] = r0c2; elt[0][3] = r0c3;
        elt[1][0] = r1c0; elt[1][1] = r1c1; elt[1][2] = r1c2; elt[1][3] = r1c3;
        elt[2][0] = r2c0; elt[2][1] = r2c1; elt[2][2] = r2c2; elt[2][3] = r2c3;
        elt[3][0] = r3c0; elt[3][1] = r3c1; elt[3][2] = r3c2; elt[3][3] = r3c3;
    }


    Matrix4(const CoordinateFrame& cframe) {
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

    void setColumn(int c, double x, double y, double z, double w) {
        elt[0][c] = x;
        elt[1][c] = y;
        elt[2][c] = z;
        elt[3][c] = w;
    }


    void setColumn(int c, const Vector4& col) {
        for (int r = 0; r < 4; ++r) {
            elt[r][c] = col[r];
        }
    }

    void setRow(int r, const Vector4& row) {
        for (int c = 0; c < 4; ++c) {
            elt[r][c] = row[c];
        }
    }

    Vector4 getRow(int r) const {
        debugAssert(r >= 0 && r <= 3);
        return Vector4(elt[r][0], elt[r][1], elt[r][2], elt[r][3]);
    }

    Vector4 getColumn(int c) const {
        debugAssert(c >= 0 && c <= 3);
        return Vector4(elt[0][c], elt[1][c], elt[2][c], elt[3][c]);
    }

    /** Returns a pointer to a row */
    inline float* operator[] (int r) {
        return elt[r];
    }

    inline const float* operator[] (int r) const {
        return elt[r];
    }

    Matrix4 operator*(const double s) const {
        Matrix4 result;
        
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                result[r][c] = elt[r][c] * s;
            }
        }

        return result;
    }


    Matrix4 operator*(const Matrix4& b) const {
        Matrix4 result;
        
        for (int r = 0; r < 4; ++r) {
            for (int c = 0; c < 4; ++c) {
                float sum = 0.0;
                for (int i = 0; i < 4; ++i) {
                    sum += elt[r][i] * b.elt[i][c]; 
                }
                result[r][c] = sum;
            }
        }

        return result;
    }
};

#endif
