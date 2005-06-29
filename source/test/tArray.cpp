#include "../include/G3DAll.h"

void perfArray();
void testArray();

class Big {
public:
    int x;
    /** Make this structure big */
    int dummy[100];

    Big() {
        x = 7;
        for (int i = 0; i < 100; ++i) {
            dummy[i] = i;
        }
    }

    ~Big() {
    }

    Big(const Big& a) : x(a.x) {
        for (int i = 0; i < 100; ++i) {
            dummy[i] = a.dummy[i];
        }
    }

    Big& operator=(const Big& a) {
        x = a.x;
        for (int i = 0; i < 100; ++i) {
            dummy[i] = a.dummy[i];
        }
        return *this;
    }
};


void perfArray() {
    printf("Array Performance:\n");

    // Note:
    //
    // std::vector calls the copy constructor for new elements and always calls the
    // constructor even when it doesn't exist (e.g., for int).  This makes its alloc
    // time much worse than other methods, but gives it a slight boost on the first
    // memory access because everything is in cache.  These tests work on huge arrays
    // to amortize that effect down.

    // Measure times for various operations on large arrays of small elements
    uint64 newAllocInt,     newFreeInt,     newAccessInt;
    uint64 arrayAllocInt,   arrayFreeInt,   arrayAccessInt;
    uint64 vectorAllocInt,  vectorFreeInt,  vectorAccessInt;
    uint64 mallocAllocInt,  mallocFreeInt,  mallocAccessInt;
    uint64 sysmallocAllocInt,  sysmallocFreeInt,  sysmallocAccessInt;

    // The code that generates memory accesses
#define LOOPS\
            for (int k = 0; k < 3; ++k) {\
                int i;\
		        for (i = 0; i < size; ++i) {\
                    array[i] = i;\
                }\
                for (i = 0; i < size; ++i) {\
                    ++array[i];\
                }\
                for (i = 0; i < size; ++i) {\
                    ++array[i];\
                }\
                for (i = 0; i < size; ++i) {\
                    ++array[i];\
                }\
                for (i = 0; i < size; ++i) {\
                    ++array[i];\
                }\
            }

    // 10 million
    int size = 10000000;

    // Run many times to filter out startup behavior
    for (int j = 0; j < 3; ++j) {
        System::beginCycleCount(mallocAllocInt);
        {
            int* array = (int*)malloc(sizeof(int) * size);
            System::endCycleCount(mallocAllocInt);
            System::beginCycleCount(mallocAccessInt);
            LOOPS;
            System::endCycleCount(mallocAccessInt);
            System::beginCycleCount(mallocFreeInt);
            free(array);
        }
        System::endCycleCount(mallocFreeInt);    

        System::beginCycleCount(sysmallocAllocInt);
        {
            int* array = (int*)System::alignedMalloc(sizeof(int) * size, 4096);
            System::endCycleCount(sysmallocAllocInt);
            System::beginCycleCount(sysmallocAccessInt);
            LOOPS;
            System::endCycleCount(sysmallocAccessInt);
            System::beginCycleCount(sysmallocFreeInt);
            System::alignedFree(array);
        }
        System::endCycleCount(sysmallocFreeInt);   

        System::beginCycleCount(arrayAllocInt);
        {
            Array<int> array(size);
            System::endCycleCount(arrayAllocInt);
            System::beginCycleCount(arrayAccessInt);
            LOOPS;
            System::endCycleCount(arrayAccessInt);
            System::beginCycleCount(arrayFreeInt);
        }
        System::endCycleCount(arrayFreeInt);

        {
            System::beginCycleCount(newAllocInt);
            int* array = new int[size];
            System::endCycleCount(newAllocInt);
            System::beginCycleCount(newAccessInt);
            LOOPS;
            System::endCycleCount(newAccessInt);

            System::beginCycleCount(newFreeInt);
            delete[] array;
        }
        System::endCycleCount(newFreeInt);

        System::beginCycleCount(vectorAllocInt);
        {
            std::vector<int> array(size);
            System::endCycleCount(vectorAllocInt);
            System::beginCycleCount(vectorAccessInt);
            LOOPS;
            System::endCycleCount(vectorAccessInt);
            System::beginCycleCount(vectorFreeInt);
        }
        System::endCycleCount(vectorFreeInt);
    }

#undef LOOPS

    {
        // Number of memory ops per element that LOOPS performed
        float N = 9*3;

        printf(" Int array cycles/elt\n");
        printf("                             Alloc    Access   Free\n");
        printf("  G3D::Array                 %5.02f    %5.02f   %5.02f\n", (arrayAllocInt / (float)size),   (arrayAccessInt / (float)(N * size)),   (arrayFreeInt / (float)size));
        printf("  std::vector                %5.02f    %5.02f   %5.02f\n", (vectorAllocInt / (float)size),  (vectorAccessInt / (float)(N * size)),  (vectorFreeInt / (float)size));
        printf("  new/delete                 %5.02f    %5.02f   %5.02f\n", (newAllocInt / (float)size),     (newAccessInt / (float)(N * size)),     (newFreeInt / (float)size));
        printf("  malloc/free                %5.02f    %5.02f   %5.02f\n", (mallocAllocInt / (float)size),  (mallocAccessInt / (float)(N * size)),  (mallocFreeInt / (float)size));
        printf("  System::alignedMalloc      %5.02f    %5.02f   %5.02f\n", (sysmallocAllocInt / (float)size),  (sysmallocAccessInt / (float)(N * size)),  (sysmallocFreeInt / (float)size));
        printf("\n");
    }

    ///////////////////////////////////////////////////////
    // The code that generates memory accesses
#define LOOPS\
            for (int k = 0; k < 3; ++k) {\
                int i;\
		        for (i = 0; i < size; ++i) {\
                    array[i].x = i;\
                }\
                for (i = 0; i < size; ++i) {\
                    ++array[i].x;\
                }\
                for (i = 0; i < size; ++i) {\
                    ++array[i].x;\
                }\
                for (i = 0; i < size; ++i) {\
                    ++array[i].x;\
                }\
                for (i = 0; i < size; ++i) {\
                    ++array[i].x;\
                }\
            }
    // 1 million
    size = 1000000;

    // Measure times for various operations on large arrays of small elements
    uint64 newAllocBig,     newFreeBig,     newAccessBig;
    uint64 arrayAllocBig,   arrayFreeBig,   arrayAccessBig;
    uint64 vectorAllocBig,  vectorFreeBig,  vectorAccessBig;
    uint64 mallocAllocBig,  mallocFreeBig,  mallocAccessBig;
    uint64 sysmallocAllocBig,  sysmallocFreeBig,  sysmallocAccessBig;
    // Run many times to filter out startup behavior
    for (int j = 0; j < 3; ++j) {
        System::beginCycleCount(mallocAllocBig);
        {
            Big* array = (Big*)malloc(sizeof(Big) * size);
            System::endCycleCount(mallocAllocBig);
            System::beginCycleCount(mallocAccessBig);
            LOOPS;
            System::endCycleCount(mallocAccessBig);
            System::beginCycleCount(mallocFreeBig);
            free(array);
        }
        System::endCycleCount(mallocFreeBig);    

        System::beginCycleCount(sysmallocAllocBig);
        {
            Big* array = (Big*)System::alignedMalloc(sizeof(Big) * size, 4096);
            System::endCycleCount(sysmallocAllocBig);
            System::beginCycleCount(sysmallocAccessBig);
            LOOPS;
            System::endCycleCount(sysmallocAccessBig);
            System::beginCycleCount(sysmallocFreeBig);
            System::alignedFree(array);
        }
        System::endCycleCount(sysmallocFreeBig);   

        System::beginCycleCount(arrayAllocBig);
        {
            Array<Big> array(size);
            System::endCycleCount(arrayAllocBig);
            System::beginCycleCount(arrayAccessBig);
            LOOPS;
            System::endCycleCount(arrayAccessBig);
            System::beginCycleCount(arrayFreeBig);
        }
        System::endCycleCount(arrayFreeBig);

        {
            System::beginCycleCount(newAllocBig);
            Big* array = new Big[size];
            System::endCycleCount(newAllocBig);
            System::beginCycleCount(newAccessBig);
            LOOPS;
            System::endCycleCount(newAccessBig);

            System::beginCycleCount(newFreeBig);
            delete[] array;
        }
        System::endCycleCount(newFreeBig);

        System::beginCycleCount(vectorAllocBig);
        {
            std::vector<Big> array(size);
            System::endCycleCount(vectorAllocBig);
            System::beginCycleCount(vectorAccessBig);
            LOOPS;
            System::endCycleCount(vectorAccessBig);
            System::beginCycleCount(vectorFreeBig);
        }
        System::endCycleCount(vectorFreeBig);
    }
#undef LOOPS

    {
        // Number of memory ops per element that LOOPS performed
        float N = 9*3;

        printf(" Big class array cycles/elt\n");
        printf("                             Alloc    Access   Free\n");
        printf("  G3D::Array               %7.02f    %5.02f   %5.02f\n", (arrayAllocBig / (float)size),   (arrayAccessBig / (float)(N * size)),   (arrayFreeBig / (float)size));
        printf("  std::vector              %7.02f    %5.02f   %5.02f\n", (vectorAllocBig / (float)size),  (vectorAccessBig / (float)(N * size)),  (vectorFreeBig / (float)size));
        printf("  new/delete               %7.02f    %5.02f   %5.02f\n", (newAllocBig / (float)size),     (newAccessBig / (float)(N * size)),     (newFreeBig / (float)size));
        printf("  malloc/free(*)           %7.02f    %5.02f   %5.02f\n", (mallocAllocBig / (float)size),  (mallocAccessBig / (float)(N * size)),  (mallocFreeBig / (float)size));
        printf("  System::alignedMalloc(*) %7.02f    %5.02f   %5.02f\n", (sysmallocAllocBig / (float)size),  (sysmallocAccessBig / (float)(N * size)),  (sysmallocFreeBig / (float)size));
        printf("    * does not call constructor or destructor!\n\n");
    }

/**
nativeDel, g3dNew, g3dDel;/ */
/*
, g3dAppend, stdAppend;

    {
        Array<int> array;
        System::beginCycleCount(g3dAppend);
        int i;
        for (i = 0; i < size; ++i) {
            array.append(i);
        }
        System::endCycleCount(g3dAppend);
    }
    {
        std::vector<int> array;
        System::beginCycleCount(stdAppend);
        int i;
        for (i = 0; i < size; ++i) {
            array.push_back(i);
        }
        System::endCycleCount(stdAppend);
    }

    G3D::RealTime t0 = G3D::System::time();
    
    {
        std::vector<Foo> v;
        v.resize(4000);
        v.resize(2000);
        v.resize(30000);
        v.resize(0);
        v.resize(4000);
        v.resize(10000);
    }
    printf("std::vector: %gs\n", G3D::System::time() - t0);

    t0 = G3D::System::time();
    {
        G3D::Array<Foo> v;
        v.resize(4000);
        v.resize(2000);
        v.resize(30000);
        v.resize(0);
        v.resize(4000);
        v.resize(10000);
    }
    printf("G3D::Array:  %gs\n", G3D::System::time() - t0);

*/
    /*
    printf("Array<int>.append:                  %g cycles\n", g3dAppend / (size * 2.0));
    printf("std::vector<int>.push_back:         %g cycles\n", stdAppend / (size * 2.0));
    */

    printf("\n");
}


void testArray() {
    printf("Test G3D::Array...passed\n");
}
