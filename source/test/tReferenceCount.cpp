#include "../include/G3DAll.h"

class WKFoo : public ReferenceCountedObject {
public:

    std::string name;

    WKFoo(const std::string& x) : name(x) {
        //printf("Foo(\"%s\") = 0x%x\n", name.c_str(), this);
    }

    ~WKFoo() {
        //printf("~Foo(\"%s\") = 0x%x\n", name.c_str(), this);
    }
};
typedef ReferenceCountedPointer<WKFoo>     WKFooRef;
typedef WeakReferenceCountedPointer<WKFoo> WKFooWeakRef;


void testWeakPointer() {
    printf("WeakReferenceCountedPointer ");

    WKFooWeakRef wB;
    {
        WKFooRef A = new WKFoo("A");

        WKFooWeakRef wA(A);

        debugAssert(wA.createStrongPtr().isNull() == false);

        A = NULL;

        debugAssert(wA.createStrongPtr().isNull() == true);

        
        WKFooRef B = new WKFoo("B");
        
        A = B;
        debugAssert(wA.createStrongPtr().isNull() == true);
        debugAssert(wB.createStrongPtr().isNull() == true);
        
        wA = A;

        debugAssert(wA.createStrongPtr().isNull() == false);

        wB = B;

        debugAssert(A == B);
        debugAssert(wA == wB);

        wA = NULL;
        debugAssert(wA.createStrongPtr().isNull() == true);

        {
            WKFooRef C = new WKFoo("C");
        }

        debugAssert(wB.createStrongPtr().isNull() == false);
    }
    debugAssert(wB.createStrongPtr().isNull() == true);

    printf("passed\n");
}


void testReferenceCount() {
    testWeakPointer();
}
