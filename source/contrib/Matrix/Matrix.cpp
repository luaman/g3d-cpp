#include "Matrix.h"

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

///////////////////////////////////////////////////////////

void Matrix::Impl::setSize(int newRows, int newCols) {
    System::alignedFree(data);
    delete elt;

    R = newRows; C = newCols;
    data = (float*)System::alignedMalloc(R * C * sizeof(T), 16);

    // Construct the row pointers
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
    return *this;
}


void Matrix::Impl::setZero() {
    System::memset(data, 0, R * C * sizeof(T));
}


void Matrix::Impl::mul(const Impl& B, Impl& out) const {
    const Impl& A = *this;

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


// We're about to define several similar methods
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

void Matrix::Impl::inverse(Impl& out) const {
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
