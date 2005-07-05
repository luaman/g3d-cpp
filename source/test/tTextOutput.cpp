#include "../include/G3DAll.h"

void perfTextOutput() {
    printf("TextOutput\n");

    // printf
    {
        TextOutput t;

        uint64 tp;
        const int N = 1000;

        System::beginCycleCount(tp);
        for (int i = 0; i < N; ++i){
            t.printf("%d, %d\n", i, i + 1);
        }
        System::endCycleCount(tp);

        std::string s;
        t.commitString(s);

        printf(" Cycles to print int32\n");
        printf("   TextOutput::printf         %g\n", (double)tp / (2.0 * N));

    }
    printf("\n\n");
    while(true);
}


