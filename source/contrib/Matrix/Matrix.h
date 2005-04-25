//#include <G3DAll.h>

#ifndef MATRIX_H
#define MATRIX_H

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


 */
class Matrix {
public:
    /** 
      Internal precision.  Currently float, but this may become a templated class in the future
      to allow operations like Matrix<double> and Matrix<ComplexFloat>.

      Not necessarily a plain-old-data type (e.g., could ComplexFloat), but must be something 
      with no constructor that can be safely memcpyd and that has a bit pattern of all zeros
      when zero.*/
    typedef float T;

private:

    class Impl : public ReferenceCountedObject {
    public:

        int                 R;
        int                 C;

        /** elt[r][c] = the element.  Pointers into data.*/
        T**         elt;

        /** Row major data */
        T*          data;

        /** Deletes all previous data and resets to random data, even if the size matches the current one.*/
        void setSize(int newRows, int newCols);

    public:

        Impl() : elt(NULL), data(NULL), R(0), C(0) {}

        Impl(int r, int c) : elt(NULL), data(NULL), R(0), C(0) {
            setSize(r, c);
        }

        Impl(const Impl& B) : data(NULL), elt(NULL), R(0), C(0) {
            *this = B;
        }

        Impl& operator=(const Impl& m);

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

        void inverse(Impl& out) const;

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

    Matrix(int R, int C) : impl(new Impl(R, C)) {
        impl->setZero();
    }

    static Matrix zero(int R, int C);

    static Matrix one(int R, int C);

    static Matrix identity(int N);

    /** Uniformly distributed values between zero and one) */
    static Matrix random(int R, int C);

    inline int rows() const {
        return impl->R;
    }

    inline int cols() const {
        return impl->C;
    }

    Matrix& operator*=(const T& B);
    Matrix& operator/=(const T& B);
    Matrix& operator+=(const T& B);
    Matrix& operator-=(const T& B);

    Matrix& operator*=(const Matrix& B);
    Matrix& operator+=(const Matrix& B);
    Matrix& operator-=(const Matrix& B);


    /** Matrix multiplication.  See also arrayMul */
    Matrix operator*(const Matrix& B) const {
        Matrix C(impl->R, B.impl->C);
        impl->mul(*B.impl, *C.impl);
        return C;
    }

    Matrix operator*(const T& B) const {
        Matrix C(impl->R, impl->C);
        impl->mul(B, *C.impl);
        return C;
    }

    Matrix operator+(const Matrix& B) const {
        Matrix C(impl->R, impl->C);
        impl->add(*B.impl, *C.impl);
        return C;
    }

    Matrix operator+(const T& v) const {
        Matrix C(impl->R, impl->C);
        impl->add(v, *C.impl);
        return C;
    }

    Matrix operator-(const T& v) const {
        Matrix C(impl->R, impl->C);
        impl->sub(v, *C.impl);
        return C;
    }

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

    /** Mutates this */
    void arrayMulInPlace(const Matrix& B);

    /** Mutates this */
    void arrayDivInPlace(const Matrix& B);

    /**
     A<SUP>-1</SUP>
     */
    Matrix inverse() const {
        Impl* A = new Impl(rows(), cols());
        impl->inverse(*A);
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

