/**
 @file Matrix.cpp
 @author Morgan McGuire, matrix@graphics3d.com
 */
#include "Matrix.h"

int Matrix::debugNumCopyOps = 0;
int Matrix::debugNumAllocOps = 0;


Matrix::Matrix(const Matrix3& M) {
    // TODO
}

Matrix(const Matrix4& M) {
    // TODO
}


#define INPLACE(OP)\
    ImplRef A = impl;\
\
    if (! A.isLastReference()) {\
        impl = new Impl(A->R, A->C);\
    }\
\
    A->OP(B, *impl);

Matrix& Matrix::operator*=(const T& B) {
    INPLACE(mul)
    return *this;
}


Matrix& Matrix::operator-=(const T& B) {
    INPLACE(sub)
    return *this;
}


Matrix& Matrix::operator+=(const T& B) {
    INPLACE(add)
    return *this;
}


Matrix& Matrix::operator/=(const T& B) {
    INPLACE(div)
    return *this;
}


Matrix& Matrix::operator*=(const Matrix& B) {
    // We can't optimize this one
    *this = *this * B;
    return *this;
}


Matrix& Matrix::operator-=(const Matrix& _B) {
    const Impl& B = *_B.impl;
    INPLACE(sub)
    return *this;
}


Matrix& Matrix::operator+=(const Matrix& _B) {
    const Impl& B = *_B.impl;
    INPLACE(add)
    return *this;
}


void Matrix::arrayMulInPlace(const Matrix& _B) {
    const Impl& B = *_B.impl;
    INPLACE(arrayMul)
}


void Matrix::arrayDivInPlace(const Matrix& _B) {
    const Impl& B = *_B.impl;
    INPLACE(arrayDiv)
}

#undef INPLACE

void Matrix::set(int r, int c, T v) {
    if (! impl.isLastReference()) {
        // Copy the data before mutating; this object is shared
        impl = new Impl(*impl);
    }
    impl->set(r, c, v);
}


Matrix::T Matrix::get(int r, int c) const {
    return impl->get(r, c);
}


Matrix Matrix::zero(int R, int C) {
    Impl* A = new Impl(R, C);
    A->setZero();
    return Matrix(A);
}


Matrix Matrix::one(int R, int C) {
    Impl* A = new Impl(R, C);
    for (int i = R * C - 1; i >= 0; --i) {
        A->data[i] = 1.0;
    }
    return Matrix(A);
}


Matrix Matrix::random(int R, int C) {
    Impl* A = new Impl(R, C);
    for (int i = R * C - 1; i >= 0; --i) {
        A->data[i] = G3D::random(0.0, 1.0);
    }
    return Matrix(A);
}


Matrix Matrix::identity(int N) {
    Impl* m = new Impl(N, N);
    m->setZero();
    for (int i = 0; i < N; ++i) {
        m->elt[i][i] = 1.0;
    }
    return Matrix(m);
}


void Matrix::negate(Matrix& out) {
    // TODO
}


void Matrix::transpose(Matrix& out) const {
    // TODO
}


Matrix3 Matrix::toMatrix3() const {
    // TODO
    return Matrix3::zero();
}


Matrix4 toMatrix4() const {
    // TODO
    return Matrix4::zero();
}


Vector2 Matrix::toVector2() const {
    // TODO
    return Vector2();
}


Vector3 toVector3() const {
    // TODO
    return Vector3();
}


Vector4 toVector4() const {
    // TODO
    return Vector4();
}


///////////////////////////////////////////////////////////

void Matrix::Impl::setSize(int newRows, int newCols) {
    if ((R == newRows) && (C == newCols)) {
        // Nothing to do
        return;
    }

    int newSize = newRows * newCols;

    R = newRows; C = newCols;

    // Only allocate if we need more space
    // or the size difference is ridiculous
    if ((newSize > dataSize) || (newSize < dataSize / 4)) {
        System::alignedFree(data);
        data = (float*)System::alignedMalloc(R * C * sizeof(T), 16);
        ++Matrix::debugNumAllocOps;
        dataSize = newSize;
    }

    // Construct the row pointers
    delete elt;
    elt = new T*[R];

    for (int r = 0; r < R; ++ r) {
        elt[r] = data + r * C;
    }
}


Matrix::Impl::~Impl() {
    delete elt;
    System::alignedFree(data);
}


Matrix::Impl& Matrix::Impl::operator=(const Impl& m) {
    setSize(m.R, m.C);
    System::memcpy(data, m.data, R * C * sizeof(T));
    ++Matrix::debugNumCopyOps;
    return *this;
}


void Matrix::Impl::setZero() {
    System::memset(data, 0, R * C * sizeof(T));
}


void Matrix::Impl::mul(const Impl& B, Impl& out) const {
    const Impl& A = *this;

    debugAssertM(
        (this != &out) && (&B != &out),
        "Output argument to mul cannot be the same as an input argument.");

    debugAssert(A.C == B.R);
    debugAssert(A.R == out.R);
    debugAssert(B.C == out.C);

    for (int r = 0; r < out.R; ++r) {
        for (int c = 0; c < out.C; ++c) {
            T sum = 0.0;
            for (int i = 0; i < A.C; ++i) {
                sum += A.get(r, i) * B.get(i, c);
            }
            out.set(r, c, sum);
        }
    }
}


// We're about to define several similar methods,
// so use a macro to share implementations.  This
// must be a macro because the difference between
// the macros is the operation in the inner loop.
#define POINTWISE(OP)\
    const Impl& A = *this;\
                            \
    debugAssert(A.C == B.C);\
    debugAssert(A.R == B.R);\
    debugAssert(A.C == out.C);\
    debugAssert(A.R == out.R);\
                            \
    for (int i = R * C - 1; i >= 0; --i) {\
        out.data[i] = A.data[i] OP B.data[i];\
    }


#define POINTWISECONST(OP)\
    const Impl& A = *this;\
                            \
    debugAssert(A.C == out.C);\
    debugAssert(A.R == out.R);\
                            \
    for (int i = R * C - 1; i >= 0; --i) {\
        out.data[i] = A.data[i] OP B;\
    }


void Matrix::Impl::add(const Impl& B, Impl& out) const {
    POINTWISE(+);
}


void Matrix::Impl::add(Matrix::T B, Impl& out) const {
    POINTWISECONST(+);
}


void Matrix::Impl::sub(const Impl& B, Impl& out) const {
    POINTWISE(-);
}


void Matrix::Impl::sub(Matrix::T B, Impl& out) const {
    POINTWISECONST(-);
}


void Matrix::Impl::arrayMul(const Impl& B, Impl& out) const {
    POINTWISE(*);
}


void Matrix::Impl::mul(Matrix::T B, Impl& out) const {
    POINTWISECONST(*);
}


void Matrix::Impl::arrayDiv(const Impl& B, Impl& out) const {
    POINTWISE(/);
}


void Matrix::Impl::div(Matrix::T B, Impl& out) const {
    POINTWISECONST(/);
}


#undef POINTWISECONST
#undef POINTWISE


void Matrix::Impl::lsub(Matrix::T B, Impl& out) const {
    const Impl& A = *this;

    debugAssert(A.C == out.C);
    debugAssert(A.R == out.R);

    for (int i = R * C - 1; i >= 0; --i) {
        out.data[i] = B - A.data[i];
    }
}

void Matrix::Impl::inverseViaAdjoint(Impl& out) const {
    debugAssert(&out != this);

    // Inverse = adjoint / determinant

    adjoint(out);

    // Don't call the determinant method when we already have an
    // adjoint matrix; there's a faster way of computing it: the dot
    // product of the first row and the adjoint's first col.
    double det = 0.0;
    for (int r = R - 1; r >= 0; --r) {
        det += elt[0][r] * out.elt[r][0];
    }

    out.div(det, out);
}


void Matrix::Impl::transpose(Impl& out) const {
    debugAssert(out.R == C);
    debugAssert(out.C == R);

    if (&out == this) {
        // Square matrix in place
        for (int r = 0; r < R; ++r) {
            for (int c = r + 1; c < C; ++c) {
                T temp = get(r, c);
                out.set(r, c, get(c, r));
                out.set(c, r, temp);
            }
        }
    } else {
        for (int r = 0; r < R; ++r) {
            for (int c = 0; c < C; ++c) {
                out.set(c, r, get(r, c)); 
            }
        }
    }
}


void Matrix::Impl::adjoint(Impl& out) const {    
    cofactor(out);
    // Transpose is safe to perform in place
    out.transpose(out);
}


void Matrix::Impl::cofactor(Impl& out) const {
    debugAssert(&out != this);
    for(int r = 0; r < R; ++r) {
        for(int c = 0; c < C; ++c) {
            out.set(r, c, cofactor(r, c));
        }
    } 
}


Matrix::T Matrix::Impl::cofactor(int r, int c) const {
    // Strang p. 217
    float s = isEven(r + c) ? 1 : -1;

    return s * determinant(r, c);
}


Matrix::T Matrix::Impl::determinant(int nr, int nc) const {
    debugAssert(R > 0);
    debugAssert(C > 0);
    Impl A(R - 1, C - 1);
    withoutRowAndCol(nr, nc, A);
    return A.determinant();
}


Matrix::T Matrix::Impl::determinant() const {

    debugAssert(R == C);

    // Compute using cofactors
    switch(R) {
    case 0:
        return 0;

    case 1:
        // Determinant of a 1x1 is the element
        return elt[0][0];

    case 2:
        // Determinant of a 2x2 is ad-bc
        return elt[0][0] * elt[1][1] - elt[0][1] * elt[1][0];

    case 3:
        {
          // Determinant of an nxn matrix is the dot product of the first
          // row with the first row of cofactors.  The base cases of this
          // method get called a lot, so we spell out the implementation
          // for the 3x3 case.

          double cofactor00 = elt[1][1] * elt[2][2] - elt[1][2] * elt[2][1];
          double cofactor10 = elt[1][2] * elt[2][0] - elt[1][0] * elt[2][2];
          double cofactor20 = elt[1][0] * elt[2][1] - elt[1][1] * elt[2][0];
      
          return 
            elt[0][0] * cofactor00 +
            elt[0][1] * cofactor10 +
            elt[0][2] * cofactor20;
        }
      
    default:
        {
            // Determinant of an n x n matrix is the dot product of the first
            // row with the first row of cofactors
            T det = 0.0;

            for (int c = 0; c < C; ++c) {
                det += elt[0][c] * cofactor(0, c);
            }

            return det;
        }
    }
}


void Matrix::Impl::withoutRowAndCol(int excludeRow, int excludeCol, Impl& out) const {
    debugAssert(out.R == R - 1);
    debugAssert(out.C == C - 1);

    for (int r = 0; r < out.R; ++r) {
        for (int c = 0; c < out.C; ++c) {
            out.elt[r][c] = elt[r + ((r >= excludeRow) ? 1 : 0)][c + ((c >= excludeCol) ? 1 : 0)];
        }
    }
}


void Matrix::Impl::inverseInPlaceGaussJordan() {
    debugAssert(R == C);

    // Exchange to float elements
#   define SWAP(x, y) {float temp = x; x = y; y = temp;}

    // The integer arrays pivot, rowIndex, and colIndex are
    // used for bookkeeping on the pivoting
    static Array<int> colIndex, rowIndex, pivot;

    int col, row;

    colIndex.resize(R);
    rowIndex.resize(R);
    pivot.resize(R);

    static const int NO_PIVOT = -1;

    // Initialize the pivot array to default values.
    for (int i = 0; i < R; ++i) {
        pivot[i] = NO_PIVOT;
    }

    // This is the main loop over the columns to be reduced
    // Loop over the columns.
    for (int c = 0; c < R; ++c) {

        // Find the largest element and use that as a pivot
        float largestMagnitude = 0.0;

        // This is the outer loop of the search for a pivot element
        for (int r = 0; r < R; ++r) {

            // Unless we've already found the pivot, keep going
            if (pivot[r] != 0) {

                // Find the largest pivot
                for (int k = 0; k < R; ++k) {
                    if (pivot[k] == NO_PIVOT) {
                        const float mag = fabs(elt[r][k]);

                        if (mag >= largestMagnitude) {
                            largestMagnitude = mag;
                            row = r; col = k;
                        }
                    }
                }
            }
        }

        pivot[col] += 1;

        // Interchange columns so that the pivot element is on the diagonal (we'll have to undo this
        // at the end)
        if (row != col) {
            for (int k = 0; k < R; ++k) {
                SWAP(elt[row][k], elt[col][k])
            }
        }

        // The pivot is now at [row, col]
        rowIndex[c] = row; 
        colIndex[c] = col;
    
        double piv = elt[col][col];

        debugAssertM(piv != 0.0, "Matrix is singular");

        // Divide everything by the pivot (avoid computing the division
        // multiple times).
        const double pivotInverse = 1.0 / piv;
        elt[col][col] = 1.0;

        for (int k = 0; k < R; ++k) {
            elt[col][k] *= pivotInverse;
        }

        // Reduce all rows
        for (int r = 0; r < R; ++r) {
            // Skip over the pivot row
            if (r != col) {

                double oldValue = elt[r][col];
                elt[r][col] = 0.0;

                for (int k = 0; k < R; ++k) {
                    elt[r][k] -= elt[col][k] * oldValue;
                }
            }
        }
    }


    // Put the columns back in the correct locations
    for (int i = R - 1; i >= 0; --i) {
        if (rowIndex[i] != colIndex[i]) {
            for (int k = 0; k < R; ++k) {
                SWAP(elt[k][rowIndex[i]], elt[k][colIndex[i]]);
            }
        }
    } 
    
#   undef SWAP
}

#if 0
/* 
 * svdcomp - SVD decomposition routine. 
 * Takes an mxn matrix a and decomposes it into udv, where u,v are
 * left and right orthogonal transformation matrices, and d is a 
 * diagonal matrix of singular values.
 *
 * @cite Based on Dianne Cook's implementation, which is adapted from 
 * svdecomp.c in XLISP-STAT 2.1, which is code from Numerical Recipes 
 * adapted by Luke Tierney and David Betz.  The Numerical Recipes code 
 * is adapted from Forsythe et al, who based their code on Golub and
 * Reinsch's original implementation.
 *
 * Input to dsvd is as follows:
 *   a = mxn matrix to be decomposed, gets overwritten with u
 *   m = row dimension of a
 *   n = column dimension of a
 *   w = returns the vector of singular values of a
 *   v = returns the right orthogonal transformation matrix
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "defs_and_types.h"
 
static double PYTHAG(double a, double b)
{
    double at = fabs(a), bt = fabs(b), ct, result;

    if (at > bt)       { ct = bt / at; result = at * sqrt(1.0 + ct * ct); }
    else if (bt > 0.0) { ct = at / bt; result = bt * sqrt(1.0 + ct * ct); }
    else result = 0.0;
    return(result);
}


int dsvd(float **a, int m, int n, float *w, float **v)
{
    int flag, i, its, j, jj, k, l, nm;
    double c, f, h, s, x, y, z;
    double anorm = 0.0, g = 0.0, scale = 0.0;
    double *rv1;
  
    if (m < n) 
    {
        fprintf(stderr, "#rows must be > #cols \n");
        return(0);
    }
  
    rv1 = (double *)malloc((unsigned int) n*sizeof(double));

/* Householder reduction to bidiagonal form */
    for (i = 0; i < n; i++) 
    {
        /* left-hand reduction */
        l = i + 1;
        rv1[i] = scale * g;
        g = s = scale = 0.0;
        if (i < m) 
        {
            for (k = i; k < m; k++) 
                scale += fabs((double)a[k][i]);
            if (scale) 
            {
                for (k = i; k < m; k++) 
                {
                    a[k][i] = (float)((double)a[k][i]/scale);
                    s += ((double)a[k][i] * (double)a[k][i]);
                }
                f = (double)a[i][i];
                g = -SIGN(sqrt(s), f);
                h = f * g - s;
                a[i][i] = (float)(f - g);
                if (i != n - 1) 
                {
                    for (j = l; j < n; j++) 
                    {
                        for (s = 0.0, k = i; k < m; k++) 
                            s += ((double)a[k][i] * (double)a[k][j]);
                        f = s / h;
                        for (k = i; k < m; k++) 
                            a[k][j] += (float)(f * (double)a[k][i]);
                    }
                }
                for (k = i; k < m; k++) 
                    a[k][i] = (float)((double)a[k][i]*scale);
            }
        }
        w[i] = (float)(scale * g);
    
        /* right-hand reduction */
        g = s = scale = 0.0;
        if (i < m && i != n - 1) 
        {
            for (k = l; k < n; k++) 
                scale += fabs((double)a[i][k]);
            if (scale) 
            {
                for (k = l; k < n; k++) 
                {
                    a[i][k] = (float)((double)a[i][k]/scale);
                    s += ((double)a[i][k] * (double)a[i][k]);
                }
                f = (double)a[i][l];
                g = -SIGN(sqrt(s), f);
                h = f * g - s;
                a[i][l] = (float)(f - g);
                for (k = l; k < n; k++) 
                    rv1[k] = (double)a[i][k] / h;
                if (i != m - 1) 
                {
                    for (j = l; j < m; j++) 
                    {
                        for (s = 0.0, k = l; k < n; k++) 
                            s += ((double)a[j][k] * (double)a[i][k]);
                        for (k = l; k < n; k++) 
                            a[j][k] += (float)(s * rv1[k]);
                    }
                }
                for (k = l; k < n; k++) 
                    a[i][k] = (float)((double)a[i][k]*scale);
            }
        }
        anorm = MAX(anorm, (fabs((double)w[i]) + fabs(rv1[i])));
    }
  
    /* accumulate the right-hand transformation */
    for (i = n - 1; i >= 0; i--) 
    {
        if (i < n - 1) 
        {
            if (g) 
            {
                for (j = l; j < n; j++)
                    v[j][i] = (float)(((double)a[i][j] / (double)a[i][l]) / g);
                    /* double division to avoid underflow */
                for (j = l; j < n; j++) 
                {
                    for (s = 0.0, k = l; k < n; k++) 
                        s += ((double)a[i][k] * (double)v[k][j]);
                    for (k = l; k < n; k++) 
                        v[k][j] += (float)(s * (double)v[k][i]);
                }
            }
            for (j = l; j < n; j++) 
                v[i][j] = v[j][i] = 0.0;
        }
        v[i][i] = 1.0;
        g = rv1[i];
        l = i;
    }
  
    /* accumulate the left-hand transformation */
    for (i = n - 1; i >= 0; i--) 
    {
        l = i + 1;
        g = (double)w[i];
        if (i < n - 1) 
            for (j = l; j < n; j++) 
                a[i][j] = 0.0;
        if (g) 
        {
            g = 1.0 / g;
            if (i != n - 1) 
            {
                for (j = l; j < n; j++) 
                {
                    for (s = 0.0, k = l; k < m; k++) 
                        s += ((double)a[k][i] * (double)a[k][j]);
                    f = (s / (double)a[i][i]) * g;
                    for (k = i; k < m; k++) 
                        a[k][j] += (float)(f * (double)a[k][i]);
                }
            }
            for (j = i; j < m; j++) 
                a[j][i] = (float)((double)a[j][i]*g);
        }
        else 
        {
            for (j = i; j < m; j++) 
                a[j][i] = 0.0;
        }
        ++a[i][i];
    }

    /* diagonalize the bidiagonal form */
    for (k = n - 1; k >= 0; k--) 
    {                             /* loop over singular values */
        for (its = 0; its < 30; its++) 
        {                         /* loop over allowed iterations */
            flag = 1;
            for (l = k; l >= 0; l--) 
            {                     /* test for splitting */
                nm = l - 1;
                if (fabs(rv1[l]) + anorm == anorm) 
                {
                    flag = 0;
                    break;
                }
                if (fabs((double)w[nm]) + anorm == anorm) 
                    break;
            }
            if (flag) 
            {
                c = 0.0;
                s = 1.0;
                for (i = l; i <= k; i++) 
                {
                    f = s * rv1[i];
                    if (fabs(f) + anorm != anorm) 
                    {
                        g = (double)w[i];
                        h = PYTHAG(f, g);
                        w[i] = (float)h; 
                        h = 1.0 / h;
                        c = g * h;
                        s = (- f * h);
                        for (j = 0; j < m; j++) 
                        {
                            y = (double)a[j][nm];
                            z = (double)a[j][i];
                            a[j][nm] = (float)(y * c + z * s);
                            a[j][i] = (float)(z * c - y * s);
                        }
                    }
                }
            }
            z = (double)w[k];
            if (l == k) 
            {                  /* convergence */
                if (z < 0.0) 
                {              /* make singular value nonnegative */
                    w[k] = (float)(-z);
                    for (j = 0; j < n; j++) 
                        v[j][k] = (-v[j][k]);
                }
                break;
            }
            if (its >= 30) {
                free((void*) rv1);
                fprintf(stderr, "No convergence after 30,000! iterations \n");
                return(0);
            }
    
            /* shift from bottom 2 x 2 minor */
            x = (double)w[l];
            nm = k - 1;
            y = (double)w[nm];
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
            g = PYTHAG(f, 1.0);
            f = ((x - z) * (x + z) + h * ((y / (f + SIGN(g, f))) - h)) / x;
          
            /* next QR transformation */
            c = s = 1.0;
            for (j = l; j <= nm; j++) 
            {
                i = j + 1;
                g = rv1[i];
                y = (double)w[i];
                h = s * g;
                g = c * g;
                z = PYTHAG(f, h);
                rv1[j] = z;
                c = f / z;
                s = h / z;
                f = x * c + g * s;
                g = g * c - x * s;
                h = y * s;
                y = y * c;
                for (jj = 0; jj < n; jj++) 
                {
                    x = (double)v[jj][j];
                    z = (double)v[jj][i];
                    v[jj][j] = (float)(x * c + z * s);
                    v[jj][i] = (float)(z * c - x * s);
                }
                z = PYTHAG(f, h);
                w[j] = (float)z;
                if (z) 
                {
                    z = 1.0 / z;
                    c = f * z;
                    s = h * z;
                }
                f = (c * g) + (s * y);
                x = (c * y) - (s * g);
                for (jj = 0; jj < m; jj++) 
                {
                    y = (double)a[jj][j];
                    z = (double)a[jj][i];
                    a[jj][j] = (float)(y * c + z * s);
                    a[jj][i] = (float)(z * c - y * s);
                }
            }
            rv1[l] = 0.0;
            rv1[k] = f;
            w[k] = (float)x;
        }
    }
    free((void*) rv1);
    return(1);
}

#endif

