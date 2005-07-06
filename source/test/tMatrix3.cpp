#include "../include/G3DAll.h"

void mul(float (&A)[3][3], float (&B)[3][3], float (&C)[3][3]) {
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            float sum = 0;
            for (int i = 0; i < 3; ++i) {
                sum += A[r][i] * B[i][c];
            }
            C[r][c] = sum;
        }
    }
}


void perfMatrix3() {
    printf("Matrix3:\n");
    uint64 raw, opt, overhead, naive;

    // 10 million operations
    int n = 10 * 1024 * 1024;

    // Use two copies to avoid nice cache behavior
    Matrix3 A = Matrix3::fromAxisAngle(Vector3(1, 2, 1), 1.2);
    Matrix3 B = Matrix3::fromAxisAngle(Vector3(0, 1, -1), .2);
    Matrix3 C = Matrix3::zero();

    Matrix3 D = Matrix3::fromAxisAngle(Vector3(1, 2, 1), 1.2);
    Matrix3 E = Matrix3::fromAxisAngle(Vector3(0, 1, -1), .2);
    Matrix3 F = Matrix3::zero();

    int i;
    System::beginCycleCount(overhead);
    for (i = n - 1; i >= 0; --i) {
    }
    System::endCycleCount(overhead);

    System::beginCycleCount(raw);
    for (i = n - 1; i >= 0; --i) {
        C = A.transpose();
        F = D.transpose();
        C = B.transpose();
    }
    System::endCycleCount(raw);

    System::beginCycleCount(opt);
    for (i = n - 1; i >= 0; --i) {
        Matrix3::transpose(A, C);
        Matrix3::transpose(D, F);
        Matrix3::transpose(B, C);
    }
    System::endCycleCount(opt);

    raw -= overhead;
    opt -= overhead;

    printf("   C = A.transpose(): %g cycles/mul\n", 
        (double)raw / (3*n));
    printf("     transpose(A, C): %g cycles/mul\n\n", 
        (double)opt / (3*n));
    
    /////////////////////////////////


    printf(" Matrix-Matrix Multiplication\n");
    System::beginCycleCount(raw);
    for (i = n - 1; i >= 0; --i) {
        C = A * B;
        F = D * E;
        C = A * D;
    }
    System::endCycleCount(raw);

    System::beginCycleCount(opt);
    for (i = n - 1; i >= 0; --i) {
        Matrix3::mul(A, B, C);
        Matrix3::mul(D, E, F);
        Matrix3::mul(A, D, C);
    }
    System::endCycleCount(opt);

    
    {
        float A[3][3], B[3][3], C[3][3], D[3][3], E[3][3], F[3][3];

        System::beginCycleCount(naive);
        for (i = n - 1; i >= 0; --i) {
            mul(A, B, C);
            mul(D, E, F);
            mul(A, D, C);
        }
        System::endCycleCount(naive);
    }

    raw -= overhead;
    opt -= overhead;
    
    printf("     C = A * B          %g cycles/mul\n", (double)raw / (3*n));
    printf("  mul(A, B, C)          %g cycles/mul\n", (double)opt / (3*n));
    printf("  naive for-loops       %g cycles/mul\n", (double)naive / (3*n));

    printf("\n\n");
}
