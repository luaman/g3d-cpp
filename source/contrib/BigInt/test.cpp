
#include <G3DAll.h>
#include "BigInt.h"
//#include "BigInt.cpp"

using namespace G3D;

void testParser() {
    // Test hex parsing
    {
        BigInt a("0xFF");
        debugAssert(a == 255);
    }
    {
        BigInt a("0x0F");
        debugAssert(a == 15);
    }
    {
        BigInt a("0xdEaDBeEf");
        debugAssert(a == 0xdeadbeef);
    }
}


void testCompare() {
    {
        BigInt a(10);
        BigInt b(11);
        debugAssert(a < b);
    }

    {
        BigInt a(0);
        debugAssert(a == 0);
        debugAssert(a != 1);
    }
}


void testAdd() {
    // Two positives
    {
        BigInt a(1);
        debugAssert(a == 1);
        debugAssert(a != 0);
        debugAssert(a != 11);
        a += 10;
        debugAssert(a == 11);
        debugAssert(a != 1);

        a = 834245;
        a += 1023123;
        debugAssert(a == (834245 + 1023123));

    }

    // Pos neg
    {
        BigInt a(1);
        BigInt b(-1);
        BigInt c = a + b;
        debugAssert(c == 0);
    }

    // Neg pos
    {
        BigInt a(-5);
        BigInt b(1);
        BigInt c = a + b;
        debugAssert(c == -4);
    }

    // Neg neg
    {
        BigInt a(-5);
        BigInt b(-3);
        BigInt c = a + b;
        debugAssert(c == -8);
    }

    // Subtraction
    {
        BigInt a = 0xFFFF;
        BigInt b = 0x0047;
        BigInt c = a - b;
        debugAssert(c == (int)0xFFFF - (int)0x0047);
    }
}


void testMul() {
    {
        BigInt a(1);
        BigInt b(10);
        BigInt c = a * b;
        debugAssert(c == 10);
    }

    {
        BigInt a(2);
        BigInt b(10);
        BigInt c = a * b;
        debugAssert(c == 20);
    }

    {
        BigInt a(-5135);
        BigInt b(1243);
        BigInt c = a * b;
        debugAssert(c == (-5135 * 1243));
    }
}


int main(int argc, const char* argv) {

    testParser();
    testCompare();
    testAdd();
    testMul();

    return 0;
}


