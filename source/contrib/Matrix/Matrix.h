/**
  @file Matrix.h
  @author Morgan McGuire, matrix@graphics3d.com
 */

#ifndef MATRIX_H
#define MATRIX_H
#include <G3DAll.h>

/** 
 N x M matrix.  
 
 Although this class has the semantics
 The actual data is tracked internally by a reference counted pointer;
 it is efficient to pass and assign Matrix objects because no data is actually copied.
 This avoids the headache of pointers and allows natural math notation:

  <PRE>
    Matrix A, B, C;
    // ...

    C = A * f(B);
    C = C.inverse();

    A = Matrix::identity(4);
    C = A;
    C.set(0, 0, 2.0); // Triggers a copy of the data so that A remains unchanged.

    // etc.

  </PRE>

  The Matrix::debugNumCopyOps and Matrix::debugNumAllocOps counters
  increment every time an operation forces the copy and allocation of matrices.  You
  can use these to detect slow operations when efficiency is a major concern.

  Some methods accept an output argument instead of returning a value.  For example,
  <CODE>A = B.transpose()</CODE> can also be invoked as <CODE>B.transpose(A)</CODE>.
  The latter may be more efficient, since Matrix may be able to re-use the storage of
  A (if it has approximatly the right size and isn't currently shared with another matrix).
  

 */
class Matrix {
public:
    /** 
      Internal precision.  Currently float, but this may become a templated class in the future
      to allow operations like Matrix<double> and Matrix<ComplexFloat>.

      Not necessarily a plain-old-data type (e.g., could ComplexFloat), but must be something 
      with no constructor, that can be safely memcpyd, and that has a bit pattern of all zeros
      when zero.*/
    typedef float T;

    static int                     debugNumCopyOps;
    static int                     debugNumAllocOps;

private:

    /** Does not throw exceptions-- assumes the caller has taken care of 
        argument checking.*/
    class Impl : public ReferenceCountedObject {
    public:

        /** The number of rows */
        int                 R;

        /** The number of columns */
        int                 C;

        /** elt[r][c] = the element.  Pointers into data.*/
        T**                 elt;

        /** Row major data */
        T*                  data;

        size_t              dataSize;

        /** If R*C is much larger or smaller than the current, deletes all previous data
            and resets to random data.  Otherwise re-uses existing memory and just resets
            R, C, and the row pointers. */
        void setSize(int newRows, int newCols);

    public:

        Impl() : elt(NULL), data(NULL), R(0), C(0), dataSize(0) {}

        Impl(int r, int c) : elt(NULL), data(NULL), R(0), C(0), dataSize(0) {
            setSize(r, c);
        }

        Impl& operator=(const Impl& m);

        Impl(const Impl& B) : data(NULL), elt(NULL), R(0), C(0), dataSize(0) {
            // Defer to the assignment operator.
            *this = B;
        }

        ~Impl();

        void setZero();

        inline void set(int r, int c, T v) {
            debugAssert((uint)r < R);
            debugAssert((uint)c < C);
            elt[r][c] = v;
        }

        inline T get(int r, int c) const {
            debugAssert((uint)r < R);
            debugAssert((uint)c < C);
            return elt[r][c];
        }

        /** Multiplies this by B and puts the result in out. */
        void mul(const Impl& B, Impl& out) const;

        /** Ok if out == this or out == B */
        void add(const Impl& B, Impl& out) const;

        /** Ok if out == this or out == B */
        void add(T B, Impl& out) const;

        /** Ok if out == this or out == B */
        void sub(const Impl& B, Impl& out) const;

        /** Ok if out == this or out == B */
        void sub(T B, Impl& out) const;

        /** B - this */
        void lsub(T B, Impl& out) const;

        /** Ok if out == this or out == B */
        void arrayMul(const Impl& B, Impl& out) const;

        /** Ok if out == this or out == B */
        void mul(T B, Impl& out) const;

        /** Ok if out == this or out == B */
        void arrayDiv(const Impl& B, Impl& out) const;

        /** Ok if out == this or out == B */
        void div(T B, Impl& out) const;

        /** Slow way of computing an inverse; for reference */
        void inverseViaAdjoint(Impl& out) const;

        /** Use Gaussian elimination with pivots to solve for the inverse destructively in place. */
        void inverseInPlaceGaussJordan();

        void adjoint(Impl& out) const;

        /** Matrix of all cofactors */
        void cofactor(Impl& out) const;

        /**
         Cofactor [r][c] is defined as C[r][c] = -1 ^(r+c) * det(A[r][c]),
         where A[r][c] is the (R-1)x(C-1) matrix formed by removing row r and
         column c from the original matrix. 
        */
        T cofactor(int r, int c) const;

        /** Ok if out == this or out == B */
        void transpose(Impl& out) const;

        T determinant() const;

        /** Determinant computed without the given row and column */
        T determinant(int r, int c) const;

        /** Makes a (R-1)x(C-1) copy of this matrix */
        void withoutRowAndCol(int excludeRow, int excludeCol, Impl& out) const;
    };

    typedef ReferenceCountedPointer<Impl> ImplRef;

    ImplRef       impl;

    inline Matrix(ImplRef i) : impl(i) {}
    inline Matrix(Impl* i) : impl(ImplRef(i)) {}

public:

    Matrix() : impl(new Impl(0, 0)) {}

    Matrix(const Matrix3& M);

    Matrix(const Matrix4& M);

    /** Returns a new matrix that is all zero. */
    Matrix(int R, int C) : impl(new Impl(R, C)) {
        impl->setZero();
    }

    /** Returns a new matrix that is all zero. */
    static Matrix zero(int R, int C);

    /** Returns a new matrix that is all one. */
    static Matrix one(int R, int C);

    /** Returns a new identity matrix */
    static Matrix identity(int N);

    /** Uniformly distributed values between zero and one. */
    static Matrix random(int R, int C);

    /** The number of rows */
    inline int rows() const {
        return impl->R;
    }

    /** Number of columns */
    inline int cols() const {
        return impl->C;
    }

    /** Generally more efficient than A * B */
    Matrix& operator*=(const T& B);

    /** Generally more efficient than A / B */
    Matrix& operator/=(const T& B);

    /** Generally more efficient than A + B */
    Matrix& operator+=(const T& B);

    /** Generally more efficient than A - B */
    Matrix& operator-=(const T& B);

    /** No performance advantage over A * B because
        matrix multiplication requires intermediate
        storage. */
    Matrix& operator*=(const Matrix& B);

    /** Generally more efficient than A + B */
    Matrix& operator+=(const Matrix& B);

    /** Generally more efficient than A - B */
    Matrix& operator-=(const Matrix& B);

    /** Generally more efficient than A = -A; */
    void negate(Matrix& out);

    /** Returns a new matrix that is a subset of this one,
        from r1:r2 to c1:c2, inclusive.*/
    Matrix subMatrix(int r1, int r2, int c1, int c2) const;

    /** Matrix multiplication.  To perform element-by-element multiplication, 
        see arrayMul. */
    Matrix operator*(const Matrix& B) const {
        Matrix C(impl->R, B.impl->C);
        impl->mul(*B.impl, *C.impl);
        return C;
    }

    /** See also A *= B, which is more efficient in many cases */
    Matrix operator*(const T& B) const {
        Matrix C(impl->R, impl->C);
        impl->mul(B, *C.impl);
        return C;
    }

    /** See also A += B, which is more efficient in many cases */
    Matrix operator+(const Matrix& B) const {
        Matrix C(impl->R, impl->C);
        impl->add(*B.impl, *C.impl);
        return C;
    }

    /** See also A += B, which is more efficient in many cases */
    Matrix operator+(const T& v) const {
        Matrix C(impl->R, impl->C);
        impl->add(v, *C.impl);
        return C;
    }

    /** See also A -= B, which is more efficient in many cases */
    Matrix operator-(const T& v) const {
        Matrix C(impl->R, impl->C);
        impl->sub(v, *C.impl);
        return C;
    }

    /** Unary minus; negation. See also A.negateInPlace(), which is more efficient in many cases */
    Matrix operator-() const {
        Matrix C(impl->R, impl->C);
        impl->lsub(0, *C.impl);
        return C;
    }

    /** B - this */
    Matrix lsub(const T& B) const {
        Matrix C(impl->R, impl->C);
        impl->lsub(B, *C.impl);
        return C;
    }

    Matrix operator-(const Matrix& B) const {
        Matrix C(impl->R, impl->C);
        impl->sub(*B.impl, *C.impl);
        return C;
    }


    Matrix arrayMul(const Matrix& B) const {
        Matrix C(impl->R, impl->C);
        impl->arrayMul(*B.impl, *C.impl);
        return C;
    }

    Matrix3 toMatrix3() const;

    Matrix4 toMatrix4() const;
    
    Vector2 toVector2() const;

    Vector3 toVector3() const;

    Vector4 toVector4() const;

    /** Mutates this */
    void arrayMulInPlace(const Matrix& B);

    /** Mutates this */
    void arrayDivInPlace(const Matrix& B);

    /**
     A<SUP>-1</SUP>
     */
    Matrix inverse() const {
        Impl* A = new Impl(*impl);
        A->inverseInPlaceGaussJordan();
        return Matrix(A);
    }

    T determinant() const {
        return impl->determinant();
    }

    /**
     A<SUP>T</SUP>
     */
    Matrix transpose() const {
        Impl* A = new Impl(cols(), rows());
        impl->transpose(*A);
        return Matrix(A);
    }

    void transpose(Matrix& out) const;

    Matrix adjoint() const {
        Impl* A = new Impl(cols(), rows());
        impl->adjoint(*A);
        return Matrix(A);
    }

    /**
     (A<SUP>T</SUP>A)<SUP>-1</SUP>A<SUP>T</SUP>
     */
    Matrix pseudoInverse() const {
        Matrix trans = transpose();
        return (trans * (*this)).inverse() * trans;
    }

    void set(int r, int c, T v);

    T get(int r, int c) const;

    Vector2int16 size() const {
        return Vector2int16(rows(), cols());
    }

    int numElements() const {
        return rows() * cols();
    }
};

inline Matrix operator-(const Matrix::T& v, const Matrix& M) {
    return M.lsub(v);
}

inline Matrix operator*(const Matrix::T& v, const Matrix& M) {
    return M * v;
}

inline Matrix operator+(const Matrix::T& v, const Matrix& M) {
    return M + v;
}


#endif

