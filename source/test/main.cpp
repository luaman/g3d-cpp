/**
 @file Test/main.cpp

 This file contains some unit tests for G3D, as well as commented 
 out scratch code useful when debugging.

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-01-01
 @edited  2003-03-20
 */


#include "../include/graphics3D.h"
#include <iostream>

using namespace G3D;

#ifdef _WIN32
	#include "conio.h"
#endif
#include <string>


// Useful for testing image loading
//#include "test/Window.h"


void testBox() {
    printf("Box\n");
    Box box = Box(Vector3(0,0,0), Vector3(1,1,1));

    debugAssert(box.contains(Vector3(0,0,0)));
    debugAssert(box.contains(Vector3(1,1,1)));
    debugAssert(box.contains(Vector3(0.5,0.5,0.5)));
    debugAssert(! box.contains(Vector3(1.5,0.5,0.5)));
    debugAssert(! box.contains(Vector3(0.5,1.5,0.5)));
    debugAssert(! box.contains(Vector3(0.5,0.5,1.5)));
    debugAssert(! box.contains(-Vector3(0.5,0.5,0.5)));
    debugAssert(! box.contains(-Vector3(1.5,0.5,0.5)));
    debugAssert(! box.contains(-Vector3(0.5,1.5,0.5)));
    debugAssert(! box.contains(-Vector3(0.5,0.5,1.5)));

    Vector3 v0, v1, v2, v3, n1, n2;

    v0 = box.getCorner(0);
    v1 = box.getCorner(1);
    v2 = box.getCorner(2);
    v3 = box.getCorner(3);

    debugAssert(v0 == Vector3(0,0,1));
    debugAssert(v1 == Vector3(1,0,1));
    debugAssert(v2 == Vector3(1,1,1));
    debugAssert(v3 == Vector3(0,1,1));

    Vector3 n[2] = {Vector3(0,0,1), Vector3(1,0,0)};

    int i;
    for (i = 0; i < 2; ++i) {
        box.getFaceCorners(i, v0, v1, v2, v3);
        n1 = (v1 - v0).cross(v3 - v0);
        n2 = (v2 - v1).cross(v0 - v1);

        debugAssert(n1 == n2);
        debugAssert(n1 == n[i]);
    }

}

void testPlane() {
    printf("Plane\n");
    {
        Plane p(Vector3(1, 0, 0),
                Vector3(0, 1, 0),
                Vector3(0, 0, 0));

        Vector3 n = p.getNormal();
        debugAssert(n == Vector3(0,0,1));
    }

    {
        Plane p(Vector3(4, 6, .1),
                Vector3(-.2, 6, .1),
                Vector3(-.2, 6, -.1));

        Vector3 n = p.getNormal();
        debugAssert(n.fuzzyEq(Vector3(0,-1,0)));
    }
}


class TableKey : public Hashable {
public:
    int value;
    
    inline bool operator==(const TableKey& other) const {
        return value == other.value;
    }
    
    inline bool operator!=(const TableKey& other) const {
        return value != other.value;
    }

    inline unsigned int hashCode() const {
        return value;
    }
};

class TableKeyB : public Hashable {
public:
    int value;
    
    inline bool operator==(const TableKey& other) const {
        return value == other.value;
    }
    
    inline bool operator!=(const TableKey& other) const {
        return value != other.value;
    }

    inline unsigned int hashCode() const {
        return 0;
    }
};

void testTable() {
    printf("Table\n");
    // Basic get/set
    {
        Table<int, int> table;
    
        table.set(10, 20);
        table.set(3, 1);
        table.set(1, 4);

        debugAssert(table[10] == 20);
        debugAssert(table[3] == 1);
        debugAssert(table[1] == 4);
        debugAssert(table.containsKey(10));
        debugAssert(!table.containsKey(0));

        debugAssert(table.debugGetDeepestBucketSize() == 1);
    }


    // Test overloaded pointer hashing
    {
        TableKey        x[4];
        x[0].value = 1;
        x[1].value = 2;
        x[2].value = 3;
        x[3].value = 4;
        Table<TableKey*, int> table;

        table.set(x, 10);
        table.set(x + 1, 20);
        table.set(x + 2, 30);
        table.set(x + 3, 40);
        debugAssert(table[x] == 10);
        debugAssert(table[x + 1] == 20);
        debugAssert(table[x + 2] == 30);
        debugAssert(table[x + 3] == 40);

        // Our current implementation should not have collisions in this case.
        debugAssert(table.debugGetDeepestBucketSize() == 1);
    }

    {
        TableKeyB        x[6];
        Table<TableKeyB*, int> table;
        for (int i = 0; i < 6; ++i) {
            x[i].value = i;
            table.set(x + i, i);
        }

        debugAssert(table.size() == 6);
        debugAssert(table.debugGetDeepestBucketSize() == 6);
        debugAssert(table.debugGetNumBuckets() == 10);
    }

}



class A {
public:
    int x;

    A() : x(0) {
        printf("Default constructor\n");
    }

    A(int y) : x(y) {
        printf("Construct %d\n", x);
    }

    A(const A& a) : x(a.x) {
        printf("Copy %d\n", x);
    }

    A& operator=(const A& other) {
        printf("Assign %d\n", other.x);
        x = other.x;
        return *this;
    }

    ~A() {
        printf("Destruct %d\n", x);
    }
};


void measureArrayPerformance() {

    uint64 native, g3d, nativeNew, nativeDel, g3dNew, g3dDel, g3dAppend, stdAppend;

    const int size = 5000;
    for (int j = 0; j < 10; ++j) {
        {
            System::beginCycleCount(nativeNew);
            int* array = new int[size];
            System::endCycleCount(nativeNew);
            System::beginCycleCount(native);
            {
                int i;
                for (i = 0; i < size; ++i) {
                    array[i] = i;
                }
                for (i = 0; i < size; ++i) {
                    ++array[i];
                }
            }
            System::endCycleCount(native);
            System::beginCycleCount(nativeDel);
            delete[] array;
            System::endCycleCount(nativeDel);
        }
        System::beginCycleCount(g3dNew);
        {
            Array<int> array;
            array.resize(size);
            System::endCycleCount(g3dNew);
            System::beginCycleCount(g3d);
            {
                int i;
		        for (i = 0; i < size; ++i) {
                    array[i] = i;
                }
                for (i = 0; i < size; ++i) {
                    ++array[i];
                }
            }
            System::endCycleCount(g3d);
            System::beginCycleCount(g3dDel);
            array.resize(0);
        }
        System::endCycleCount(g3dDel);
    }

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

    printf("Array alloc:                        %d cycles/elt\n", g3dNew / size);
    printf("int* alloc:                         %d cycles/elt\n", nativeNew / size);
    printf("Array free:                         %d cycles/elt\n", g3dDel / size);
    printf("int* free:                          %d cycles/elt\n", nativeDel / size);
    printf("Array<int>[]:                       %g cycles\n", g3d / (size * 2.0));
    printf("int*[]:                             %g cycles\n", native / (size * 2.0));
    printf("\n");
    printf("Array<int>.append:                  %g cycles\n", g3dAppend / (size * 2.0));
    printf("std::vector<int>.push_back:         %g cycles\n", stdAppend / (size * 2.0));
}


void measureMemcpyPerformance() {
    printf("----------------------------------------------------------\n");

    uint64 native, g3d;

    int n = 1024 * 1024;
    void* m1 = malloc(n);
    void* m2 = malloc(n);

    // First iteration just primes the system
    for (int i=0; i < 2; ++i) {
        System::beginCycleCount(native);
            memcpy(m1, m2, n);
        System::endCycleCount(native);

        System::beginCycleCount(g3d);
            System::memcpy(m1, m2, n);
        System::endCycleCount(g3d);
    }
    free(m1);
    free(m2);

    printf("System::memcpy:                     %d cycles/kb\n", g3d / (n / 1024));
    printf("::memcpy      :                     %d cycles/kb\n", native / (n / 1024));
}


void measureMemsetPerformance() {
    printf("----------------------------------------------------------\n");

    uint64 native, g3d;

    int n = 1024 * 1024;
    void* m1 = malloc(n);

    // First iteration just primes the system
    for (int i=0; i < 2; ++i) {
        System::beginCycleCount(native);
            memset(m1, 31, n);
        System::endCycleCount(native);
        System::beginCycleCount(g3d);
            System::memset(m1, 31, n);
        System::endCycleCount(g3d);
    }
    free(m1);

    printf("System::memset:                     %d cycles/kb\n", g3d / (n / 1024));
    printf("::memset      :                     %d cycles/kb\n", native / (n / 1024));
}

void measureNormalizationPerformance() {
    printf("----------------------------------------------------------\n");
    uint64 raw, opt, overhead;
    int n = 1024 * 1024;

    double y;
    Vector3 x = Vector3(10,-20,3);

    int i, j;

    for (j = 0; j < 2; ++j) {
        x = Vector3(10,-20,3);
        System::beginCycleCount(overhead);
        for (i = n - 1; i >= 0; --i) {
            x.z = i;
            y += x.z;
        }
        System::endCycleCount(overhead);
    }

    x = Vector3(10,-20,3);
    y = 0;
    System::beginCycleCount(raw);
    for (i = n - 1; i >= 0; --i) {
        x.z = i;
        y += x.direction().z;
        y += x.direction().z;
        y += x.direction().z;
    }
    System::endCycleCount(raw);
    
    x = Vector3(10,-20,3);
    y = 0;
    System::beginCycleCount(opt);
    for (i = n - 1; i >= 0; --i) {
        x.z = i;
        y += x.fastDirection().z;
        y += x.fastDirection().z;
        y += x.fastDirection().z;
    }
    System::endCycleCount(opt);

    double r = raw;
    double o = opt;
    double h = overhead;

    printf("%g %g %g\n", r-h, o-h, h);

    printf("Vector3::direction():               %d cycles\n", (int)((r-h)/(n*3.0)));
    printf("Vector3::fastDirection():           %d cycles\n", (int)((o-h)/(n*3.0)));

}

void measureTriangleCollisionPerformance() {
    printf("----------------------------------------------------------\n");

    uint64 raw, opt;

    Vector3 v0(0, 0, 0);
    Vector3 v1(0, 0, -1);
    Vector3 v2(-1, 0, 0);
    Sphere sphere(Vector3(.5,1,-.5), 1);
    Vector3 vel(0, -1, 0);
    Vector3 location, normal;
    CDTriangle triangle(v0, v1, v2);
    int n = 1024;
    int i;

    System::beginCycleCount(raw);
    for (i = 0; i < n; ++i) {
        double t = CollisionDetection::collisionTimeForMovingSphereFixedTriangle(sphere, vel, CDTriangle(v0, v1, v2), location, normal);
    }
    System::endCycleCount(raw);

    System::beginCycleCount(opt);
    for (i = 0; i < n; ++i) {
        double t = CollisionDetection::collisionTimeForMovingSphereFixedTriangle(sphere, vel, triangle, location, normal);
    }
    System::endCycleCount(opt);

    printf("Sphere-Triangle collision detection on 3 vertices: %d cycles\n", raw / n);
    printf("Sphere-Triangle collision detection on CDTriangle: %d cycles\n", opt / n);
}


void testTextInput() {
    printf("TextInput\n");
    TextInput ti(TextInput::FROM_STRING, 
                 "if/*comment*/(x->y==-1e6){cout<<\"hello world\"};");

    Token a = ti.read();
    Token b = ti.read();
    Token c = ti.read();
    Token d = ti.read();
    Token e = ti.read();
    Token f = ti.read();
    double g = ti.readNumber();
    Token h = ti.read();
    Token i = ti.read();
    Token j = ti.read();
    Token k = ti.read();
    Token L = ti.read();
    Token m = ti.read();
    Token n = ti.read();
    Token p = ti.read();

    debugAssert(a.type() == Token::SYMBOL);
    debugAssert(a.string() == "if");

    debugAssert(b.type() == Token::SYMBOL);
    debugAssert(b.string() == "(");

    debugAssert(c.type() == Token::SYMBOL);
    debugAssert(c.string() == "x");

    debugAssert(d.type() == Token::SYMBOL);
    debugAssert(d.string() == "->");

    debugAssert(e.type() == Token::SYMBOL);
    debugAssert(e.string() == "y");

    debugAssert(f.type() == Token::SYMBOL);
    debugAssert(f.string() == "==");

    debugAssert(g == -1e6);

    debugAssert(h.type() == Token::SYMBOL);
    debugAssert(h.string() == ")");

    debugAssert(i.type() == Token::SYMBOL);
    debugAssert(i.string() == "{");

    debugAssert(j.type() == Token::SYMBOL);
    debugAssert(j.string() == "cout");

    debugAssert(k.type() == Token::SYMBOL);
    debugAssert(k.string() == "<<");

    debugAssert(L.type() == Token::STRING);
    debugAssert(L.string() == "hello world");

    debugAssert(m.type() == Token::SYMBOL);
    debugAssert(m.string() == "}");

    debugAssert(n.type() == Token::SYMBOL);
    debugAssert(n.string() == ";");

    debugAssert(p.type() == Token::END);
}


void testCompression() {
    printf("BinaryInput & BinaryOutput\n");
    BinaryOutput f("/tmp/out.t", G3D_LITTLE_ENDIAN);

    f.writeUInt32(1234);
    f.writeFloat64(1.234);
    f.compress();
    f.commit();

    BinaryInput g("/tmp/out.t", G3D_LITTLE_ENDIAN, true);
    uint32 i = g.readUInt32();
    debugAssert(i == 1234);
    double j = g.readFloat64();
    debugAssert(j == 1.234);
}

void testMemcpy() {
    printf("System::memcpy\n");
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

}

void testMemset() {
    printf("System::memset\n");
	static const int k = 100;
	static uint8 a[k];
	
	int i;

	for (i = 0; i < k; ++i) {
		a[i] = i & 255;
	}

	System::memset(a, 4, k);

	for (i = 0; i < k; ++i) {
		debugAssert(a[i] == 4);
	}

}

void testFloat() {
    printf("Test Float\n");
    /* changed from "nan" by ekern.  does this work on windows? */
    double x = NAN;
    bool b1  = (x < 0.0);
    bool b2  = (x >= 0.0);
    bool b3  = !(b1 || b2);

    debugAssert(isNaN(NAN));
    debugAssert(! isNaN(4));
    debugAssert(! isNaN(0));
    debugAssert(! isNaN(inf));
    debugAssert(! isNaN(-inf));
    debugAssert(! isFinite(NAN));
    debugAssert(! isFinite(-inf));
    debugAssert(! isFinite(inf));
    debugAssert(isFinite(0));
		    
}


void testCollision() {
    printf("CollisionDetection\n");

    {
        Sphere s(Vector3(5, 0, 0), 1);
        Sphere f(Vector3(4.5, 0, 0), 1);
        debugAssert(CollisionDetection::fixedSolidSphereIntersectsFixedSolidSphere(s, f));
    }

    {
        Sphere s(Vector3(5, 0, 0), 1);
        Sphere f(Vector3(1, 0, 0), 2);
        debugAssert(! CollisionDetection::fixedSolidSphereIntersectsFixedSolidSphere(s, f));
    }

    {
        Sphere s(Vector3(5, 0, 0), 1);
        Box    f(Vector3(-1, -1.5, -2), Vector3(4.5, 2, 8));
        debugAssert(CollisionDetection::fixedSolidSphereIntersectsFixedSolidBox(s, f));
    }

    {
        Sphere s(Vector3(0, 0, 0), .1);
        Box    f(Vector3(-1, -1.5, -2), Vector3(4.5, 2, 8));
        debugAssert(CollisionDetection::fixedSolidSphereIntersectsFixedSolidBox(s, f));
    }

    {
        Sphere s(Vector3(6, 0, 0), 1);
        Box    f(Vector3(-1, -1.5, -2), Vector3(4.5, 2, 8));
        debugAssert(! CollisionDetection::fixedSolidSphereIntersectsFixedSolidBox(s, f));
    }
}


int main(int argc, char* argv[]) {

    #ifndef _DEBUG
        printf("Performance analysis:\n\n");
        measureArrayPerformance();
        while(true);
        exit(1);
        measureMemcpyPerformance();
        measureMemsetPerformance();
        measureTriangleCollisionPerformance();
        measureNormalizationPerformance();
        exit(1);
    #endif

    printf("\n\nTests:\n\n");

    testFloat();
    printf("  passed\n");
	testMemset();
    printf("  passed\n");
  	testMemcpy();
    printf("  passed\n");
    testCompression();
    printf("  passed\n");
    testTextInput();
    printf("  passed\n");
    testPlane();
    printf("  passed\n");
    testTable();
    printf("  passed\n");
    testBox();    
    printf("  passed\n");
    testCollision();    
    printf("  passed\n");

    printf("\nAll tests succeeded.\n");
	
    while(true);

    return 0;
}
