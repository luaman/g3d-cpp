#include "../include/G3DAll.h"

void memcpySSE2(void* dst, const void* src, int nbytes) {
    int remainingBytes = nbytes;

    if (nbytes > 128) {

        // Number of chunks
        int N = nbytes / 128;

        float* restrict d = (float*)dst;
        const float* restrict s = (const float*)src;
    
        // Finish when the destination pointer has moved 8N elements 
        float* stop = d + (N * 8 * 4);

        while (d < stop) {
            // Inner loop unrolled 8 times
            const __m128 r0 = _mm_loadu_ps(s);
            const __m128 r1 = _mm_loadu_ps(s + 4);
            const __m128 r2 = _mm_loadu_ps(s + 8);
            const __m128 r3 = _mm_loadu_ps(s + 12);
            const __m128 r4 = _mm_loadu_ps(s + 16);
            const __m128 r5 = _mm_loadu_ps(s + 20);
            const __m128 r6 = _mm_loadu_ps(s + 24);
            const __m128 r7 = _mm_loadu_ps(s + 28);

            _mm_storeu_ps(d, r0);
            _mm_storeu_ps(d + 4, r1);
            _mm_storeu_ps(d + 8, r2);
            _mm_storeu_ps(d + 12, r3);
            _mm_storeu_ps(d + 16, r4);
            _mm_storeu_ps(d + 20, r5);
            _mm_storeu_ps(d + 24, r6);
            _mm_storeu_ps(d + 28, r7);

            s += 32;
            d += 32;
        }

        remainingBytes -= N * 8 * 4 * 4; 
    }

    if (remainingBytes > 0) {
        // Memcpy the rest
        memcpy((uint8*)dst + (nbytes - remainingBytes), (const uint8*)src + (nbytes - remainingBytes), remainingBytes); 
    }
}


void perfSystemMemcpy() {
    printf("----------------------------------------------------------\n");

    // Number of memory sizes to test
    static const int M = 8;

    //  Repeats per memory size
    static const int trials = 200;

    size_t size[M];
    for (int i = 0; i < M; ++i) {
        size[i] = 1024 * (size_t)pow((i + 1), 4);
    }

    printf("System::memcpy Performance:\n");
    printf("  Measured in cycles/kb at various copy sizes\n\n");
    uint64 native[M], g3d[M];

    for (int m = 0; m < M; ++m) {
        int n = size[m];
        void* m1 = System::alignedMalloc(n, 1024*4);
        void* m2 = System::alignedMalloc(n, 1024*4);

        // First iteration just primes the system
        ::memcpy(m1, m2, n);
        System::beginCycleCount(native[m]);
            for (int j = 0; j < trials; ++j) {
                ::memcpy(m1, m2, n);
            }
        System::endCycleCount(native[m]);

        System::memcpy(m1, m2, n);
        System::beginCycleCount(g3d[m]);
            for (int j = 0; j < trials; ++j) {
                memcpySSE2(m1, m2, n);
                // System::memcpy(m1, m2, n);
            }
        System::endCycleCount(g3d[m]);

        System::alignedFree(m1);
        System::alignedFree(m2);
    }


    printf("         Size       ");
    for (int i = 0; i < M; ++i) {
        printf("%6dk", size[i] / 1024);
    }
    printf("\n");

    printf("    ::memcpy        ");
    for (int m = 0; m < M; ++m) {
        double k = trials * (double)size[m] / 1024;
        printf(" %6d", (int)(native[m] / k));
    }
    printf("\n");

    printf("    System::memcpy* ");
    for (int m = 0; m < M; ++m) {
        double k = trials * (double)size[m] / 1024;
        printf(" %6d", (int)(g3d[m] / k));
    }
    printf("\n");
    

    if (System::hasSSE2() && System::hasMMX()) {
        printf("      * SSE2 on this machine\n");
    } else if (System::hasSSE() && System::hasMMX()) {
        printf("      * MMX on this machine\n");
    } else {
        printf("      * memcpy on this machine\n");
    }
    printf("\n");

    while(true);
}


void testSystemMemcpy() {
    printf("System::memcpy ");
	static const int k = 50000;
	static uint8 a[k];
	static uint8 b[k];

	int i;
	
	for (i = 0; i < k; ++i) {
		a[i] = i & 255;
	}

	System::memcpy(b, a, k);

	for (i = 0; i < k; ++i) {
		debugAssert(b[i] == (i & 255));
		debugAssert(a[i] == (i & 255));
	}
    printf("passed\n");

}

