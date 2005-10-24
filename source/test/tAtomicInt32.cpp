#include "../include/G3DAll.h"

void testAtomicInt32() {
    
    {
        AtomicInt32 a(1);
        debugAssert(a.value() == 1);

        a.increment();
        debugAssert(a.value() == 2);

        a.decrement();
        debugAssert(a.value() == 1);

        a = 10;
        debugAssert(a.value() == 10);

        a.compareAndSet(10, 1);
        debugAssert(a.value() == 1);

        a.compareAndSet(10, 15);
        debugAssert(a.value() == 1);
    }

}

