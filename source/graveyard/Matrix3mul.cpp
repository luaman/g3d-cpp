
/* 53 cycles
void Matrix3::_mul(const Matrix3& A, const Matrix3& B, Matrix3& out) {
    const float* const BPtr   = reinterpret_cast<const float*>(B.m_aafEntry);
    const float* const APtr   = reinterpret_cast<const float*>(A.m_aafEntry);
    float* const       outPtr = reinterpret_cast<float*>(out.m_aafEntry);

    outPtr[0] =
        APtr[0] * BPtr[0] +
        APtr[1] * BPtr[3] +
        APtr[2] * BPtr[6];
    outPtr[1] =
        APtr[0] * BPtr[1] +
        APtr[1] * BPtr[4] +
        APtr[2] * BPtr[7];
    outPtr[2] =
        APtr[0] * BPtr[2] +
        APtr[1] * BPtr[5] +
        APtr[2] * BPtr[8];

    outPtr[3] =
        APtr[3] * BPtr[0] +
        APtr[4] * BPtr[3] +
        APtr[5] * BPtr[6];
    outPtr[4] =
        APtr[3] * BPtr[1] +
        APtr[4] * BPtr[4] +
        APtr[5] * BPtr[7];
    outPtr[5] =
        APtr[3] * BPtr[2] +
        APtr[4] * BPtr[5] +
        APtr[5] * BPtr[8];

    outPtr[6] =
        APtr[6] * BPtr[0] +
        APtr[7] * BPtr[3] +
        APtr[8] * BPtr[6];
    outPtr[7] =
        APtr[6] * BPtr[1] +
        APtr[7] * BPtr[4] +
        APtr[8] * BPtr[7];
    outPtr[8] =
        APtr[6] * BPtr[2] +
        APtr[7] * BPtr[5] +
        APtr[8] * BPtr[8];
}
*/
/* 52 cycles
void Matrix3::_mul(const Matrix3& A, const Matrix3& B, Matrix3& out) {
    const float* ARowPtr = A.m_aafEntry[0];
    float* outRowPtr     = out.m_aafEntry[0];
        outRowPtr[0] =
            ARowPtr[0] * B.m_aafEntry[0][0] +
            ARowPtr[1] * B.m_aafEntry[1][0] +
            ARowPtr[2] * B.m_aafEntry[2][0];
        outRowPtr[1] =
            ARowPtr[0] * B.m_aafEntry[0][1] +
            ARowPtr[1] * B.m_aafEntry[1][1] +
            ARowPtr[2] * B.m_aafEntry[2][1];
        outRowPtr[2] =
            ARowPtr[0] * B.m_aafEntry[0][2] +
            ARowPtr[1] * B.m_aafEntry[1][2] +
            ARowPtr[2] * B.m_aafEntry[2][2];

    ARowPtr       = A.m_aafEntry[1];
    outRowPtr     = out.m_aafEntry[1];

        outRowPtr[0] =
            ARowPtr[0] * B.m_aafEntry[0][0] +
            ARowPtr[1] * B.m_aafEntry[1][0] +
            ARowPtr[2] * B.m_aafEntry[2][0];
        outRowPtr[1] =
            ARowPtr[0] * B.m_aafEntry[0][1] +
            ARowPtr[1] * B.m_aafEntry[1][1] +
            ARowPtr[2] * B.m_aafEntry[2][1];
        outRowPtr[2] =
            ARowPtr[0] * B.m_aafEntry[0][2] +
            ARowPtr[1] * B.m_aafEntry[1][2] +
            ARowPtr[2] * B.m_aafEntry[2][2];

    ARowPtr       = A.m_aafEntry[2];
    outRowPtr     = out.m_aafEntry[2];

        outRowPtr[0] =
            ARowPtr[0] * B.m_aafEntry[0][0] +
            ARowPtr[1] * B.m_aafEntry[1][0] +
            ARowPtr[2] * B.m_aafEntry[2][0];
        outRowPtr[1] =
            ARowPtr[0] * B.m_aafEntry[0][1] +
            ARowPtr[1] * B.m_aafEntry[1][1] +
            ARowPtr[2] * B.m_aafEntry[2][1];
        outRowPtr[2] =
            ARowPtr[0] * B.m_aafEntry[0][2] +
            ARowPtr[1] * B.m_aafEntry[1][2] +
            ARowPtr[2] * B.m_aafEntry[2][2];
}
*/

/* 60 cycles
void Matrix3::_mul(const Matrix3& A, const Matrix3& B, Matrix3& out) {
    for (int iRow = 0; iRow < 3; iRow++) {
        const float* const ARowPtr = A.m_aafEntry[iRow];
        float* const outRowPtr     = out.m_aafEntry[iRow];
        outRowPtr[0] =
            ARowPtr[0] * B.m_aafEntry[0][0] +
            ARowPtr[1] * B.m_aafEntry[1][0] +
            ARowPtr[2] * B.m_aafEntry[2][0];
        outRowPtr[1] =
            ARowPtr[0] * B.m_aafEntry[0][1] +
            ARowPtr[1] * B.m_aafEntry[1][1] +
            ARowPtr[2] * B.m_aafEntry[2][1];
        outRowPtr[2] =
            ARowPtr[0] * B.m_aafEntry[0][2] +
            ARowPtr[1] * B.m_aafEntry[1][2] +
            ARowPtr[2] * B.m_aafEntry[2][2];
    }
}
*/

/* 60 cycles
void Matrix3::_mul(const Matrix3& A, const Matrix3& B, Matrix3& out) {
    for (int iRow = 0; iRow < 3; iRow++) {
        const float* const row = A.m_aafEntry[iRow]; 
        out.m_aafEntry[iRow][0] =
            row[0] * B.m_aafEntry[0][0] +
            row[1] * B.m_aafEntry[1][0] +
            row[2] * B.m_aafEntry[2][0];
        out.m_aafEntry[iRow][1] =
            row[0] * B.m_aafEntry[0][1] +
            row[1] * B.m_aafEntry[1][1] +
            row[2] * B.m_aafEntry[2][1];
        out.m_aafEntry[iRow][2] =
            row[0] * B.m_aafEntry[0][2] +
            row[1] * B.m_aafEntry[1][2] +
            row[2] * B.m_aafEntry[2][2];
    }
}
*/

/* 75 cycles
void Matrix3::_mul(const Matrix3& A, const Matrix3& B, Matrix3& out) {
    for (int iRow = 0; iRow < 3; iRow++) {
        const float* const ARowPtr = A.m_aafEntry[iRow];
        float* const outRowPtr = out.m_aafEntry[iRow]; 
        for (int iCol = 0; iCol < 3; iCol++) {
            outRowPtr[iCol] =
                ARowPtr[0] * B.m_aafEntry[0][iCol] +
                ARowPtr[1] * B.m_aafEntry[1][iCol] +
                ARowPtr[2] * B.m_aafEntry[2][iCol];
        }
    }
}
*/

/* 69 cycles

void Matrix3::_mul(const Matrix3& A, const Matrix3& B, Matrix3& out) {
    for (int iRow = 0; iRow < 3; iRow++) {
        const float* const row = A.m_aafEntry[iRow]; 
        for (int iCol = 0; iCol < 3; iCol++) {
            out.m_aafEntry[iRow][iCol] =
                row[0] * B.m_aafEntry[0][iCol] +
                row[1] * B.m_aafEntry[1][iCol] +
                row[2] * B.m_aafEntry[2][iCol];
        }
    }
}

 */

/* 85 cycles
void Matrix3::_mul(const Matrix3& A, const Matrix3& B, Matrix3& out) {
    for (int iCol = 0; iCol < 3; iCol++) {
        // Save some dereferences and improve cache behavior.
        // B works by columns, so it has worse cache behavior than A.
        // pre-cache the elements of a B column on the stack.
        const double x = B.m_aafEntry[0][iCol];
        const double y = B.m_aafEntry[1][iCol];
        const double z = B.m_aafEntry[2][iCol];
        for (int iRow = 0; iRow < 3; iRow++) {
            const float* const row = A.m_aafEntry[iRow]; 
            out.m_aafEntry[iRow][iCol] =
                row[0] * x +
                row[1] * y +
                row[2] * z;
        }
    }
}

*/

/* 71 cycles
void Matrix3::_mul(const Matrix3& A, const Matrix3& B, Matrix3& out) {
    for (int iRow = 0; iRow < 3; iRow++) {
        for (int iCol = 0; iCol < 3; iCol++) {
            out.m_aafEntry[iRow][iCol] =
                A.m_aafEntry[iRow][0] * B.m_aafEntry[0][iCol] +
                A.m_aafEntry[iRow][1] * B.m_aafEntry[1][iCol] +
                A.m_aafEntry[iRow][2] * B.m_aafEntry[2][iCol];
        }
    }
}
*/
