/**
 @file Test/main.cpp

 This file runs unit conformance and performance tests for G3D.  
 To write a new test, add a file named t<class>.cpp to the project
 and provide two entry points: test<class> and perf<class> (even if they are
 empty).

 Call those two methods from main() in main.cpp.

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-01-01
 @edited  2005-08-20
 */

#include "../include/G3DAll.h"
#include <iostream>

using namespace G3D;

#ifdef G3D_WIN32
	#include "conio.h"
#endif
#include <string>


// Forward declarations
void perfArray();
void testArray();

void testMatrix();

void testMatrix3();
void perfMatrix3();

void testCollisionDetection();

void testGChunk();

void perfSystemMemcpy();
void testSystemMemcpy();

void testReferenceCount();

void testRandom();

void perfTextOutput();

void testMeshAlgTangentSpace();

void perfQueue();
void testQueue();

void testBinaryIO();
void testHugeBinaryOutput();
void perfBinaryIO();

void testTextInput();

void testAdjacency();

void measureBSPPerformance() {

    Array<AABox>                array;
    AABSPTree<AABox>            tree;
    
    const int NUM_POINTS = 100000;
    
    for (int i = 0; i < NUM_POINTS; ++i) {
        Vector3 pt = Vector3(random(-10, 10), random(-10, 10), random(-10, 10));
        AABox box(pt, pt + Vector3(.1, .1, .1));
        array.append(box);
        tree.insert(box);
    }

    tree.balance();

    uint64 bspcount, arraycount, boxcount;

    // Run twice to get cache issues out of the way
    for (int it = 0; it < 2; ++it) {
        Array<Plane> plane;
        plane.append(Plane(Vector3(-1, 0, 0), Vector3(3, 1, 1)));
        plane.append(Plane(Vector3(1, 0, 0), Vector3(1, 1, 1)));
        plane.append(Plane(Vector3(0, 0, -1), Vector3(1, 1, 3)));
        plane.append(Plane(Vector3(0, 0, 1), Vector3(1, 1, 1)));
        plane.append(Plane(Vector3(0,-1, 0), Vector3(1, 3, 1)));
        plane.append(Plane(Vector3(0, 1, 0), Vector3(1, -3, 1)));

        AABox box(Vector3(1, 1, 1), Vector3(3,3,3));

        Array<AABox> point;

        System::beginCycleCount(bspcount);
        tree.getIntersectingMembers(plane, point);
        System::endCycleCount(bspcount);

        point.clear();

        System::beginCycleCount(boxcount);
        tree.getIntersectingMembers(box, point);
        System::endCycleCount(boxcount);

        point.clear();

        System::beginCycleCount(arraycount);
        for (int i = 0; i < array.size(); ++i) {
            if (! array[i].culledBy(plane)) {
                point.append(array[i]);
            }
        }
        System::endCycleCount(arraycount);
    }

    printf("AABSPTree<AABox>::getIntersectingMembers(plane) %g Mcycles\n"
           "AABSPTree<AABox>::getIntersectingMembers(box)   %g Mcycles\n"
           "Culled by on Array<AABox>                       %g Mcycles\n\n", 
           bspcount / 1e6, 
           boxcount / 1e6,
           arraycount / 1e6);
}





void testWildcards() {
    printf("filenameContainsWildcards\n");
    debugAssert(!filenameContainsWildcards("file1.exe"));
	debugAssert(filenameContainsWildcards("file?.exe"));
	debugAssert(filenameContainsWildcards("f*.exe"));
	debugAssert(filenameContainsWildcards("f*.e?e"));
	debugAssert(filenameContainsWildcards("*1.exe"));
	debugAssert(filenameContainsWildcards("?ile1.exe"));
}


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
    (void)n;

    int i;
    for (i = 0; i < 2; ++i) {
        box.getFaceCorners(i, v0, v1, v2, v3);
        n1 = (v1 - v0).cross(v3 - v0);
        n2 = (v2 - v1).cross(v0 - v1);

        debugAssert(n1 == n2);
        debugAssert(n1 == n[i]);
    }

}



void testAABoxCollision() {
    printf("intersectionTimeForMovingPointFixedAABox\n");

    Vector3 boxlocation, aaboxlocation, normal;

    for (int i = 0; i < 1000; ++i) {

        Vector3 pt1 = Vector3::random() * random(0, 10);
        Vector3 vel1 = Vector3::random();

        Vector3 low = Vector3::random() * 5;
        Vector3 extent(random(0,4), random(0,4), random(0,4));
        AABox aabox(low, low + extent);
        Box   box = aabox.toBox();

        double boxTime = CollisionDetection::collisionTimeForMovingPointFixedBox(
            pt1, vel1, box, boxlocation, normal);
        (void)boxTime;

        double aaTime = CollisionDetection::collisionTimeForMovingPointFixedAABox(
            pt1, vel1, aabox, aaboxlocation);
        (void)aaTime;

        Ray ray = Ray::fromOriginAndDirection(pt1, vel1);
        double rayboxTime = ray.intersectionTime(box);
        (void)rayboxTime;

        double rayaaTime = ray.intersectionTime(aabox);
        (void)rayaaTime;

        debugAssert(fuzzyEq(boxTime, aaTime));
        if (boxTime < inf()) {
            debugAssert(boxlocation.fuzzyEq(aaboxlocation));
        }

        debugAssert(fuzzyEq(rayboxTime, rayaaTime));
    }
}


void testPlane() {
    printf("Plane\n");
    {
        Plane p(Vector3(1, 0, 0),
                Vector3(0, 1, 0),
                Vector3(0, 0, 0));

        Vector3 n = p.normal();
        debugAssert(n == Vector3(0,0,1));
    }

    {
        Plane p(Vector3(4, 6, .1),
                Vector3(-.2, 6, .1),
                Vector3(-.2, 6, -.1));

        Vector3 n = p.normal();
        debugAssert(n.fuzzyEq(Vector3(0,-1,0)));
    }

    {
        Plane p(Vector4(1,0,0,0),
                Vector4(0,1,0,0),
                Vector4(0,0,0,1));
        Vector3 n = p.normal();
        debugAssert(n.fuzzyEq(Vector3(0,0,1)));
    }

    {
        Plane p(
                Vector4(0,0,0,1),
                Vector4(1,0,0,0),
                Vector4(0,1,0,0));
        Vector3 n = p.normal();
        debugAssert(n.fuzzyEq(Vector3(0,0,1)));
    }

    {
        Plane p(Vector4(0,1,0,0),
                Vector4(0,0,0,1),
                Vector4(1,0,0,0));
        Vector3 n = p.normal();
        debugAssert(n.fuzzyEq(Vector3(0,0,1)));
    }
}


void testAABoxCulledBy() {
    const uint32 b00000000 = 0;
    const uint32 b00000001 = 1;
    const uint32 b00000010 = 2;
    const uint32 b00000011 = 3;
    const uint32 b00000100 = 4;
    const uint32 b00000101 = 5;
    const uint32 b00000110 = 6;
    const uint32 b00000111 = 7;
    const uint32 b00001000 = 8;

    // Avoid unused variable warnings
    (void)b00000000;
    (void)b00000001;
    (void)b00000010;
    (void)b00000011;
    (void)b00000100;
    (void)b00000101;
    (void)b00000110;
    (void)b00000111;
    (void)b00001000;

    printf("AABox::culledBy\n");

    Array<Plane> planes;

    // Planes at +/- 1
    planes.append(Plane(Vector3(-1,0,0), Vector3(1,0,0)));
    planes.append(Plane(Vector3(1,0,0), Vector3(-1,0,0)));

    AABox box(Vector3(-0.5, 0, 0), Vector3(0.5, 1, 1));
    
    uint32 parentMask, childMask;
    int index = 0;
    bool culled;

    // Contained case
    parentMask = (uint32)-1; childMask = 0; index = 0;
    culled = box.culledBy(planes, index, parentMask, childMask);
    debugAssert(index == -1);
    debugAssert(! culled);
    debugAssert(childMask == b00000000);

    // Positive straddle
    box = AABox(Vector3(0.5, 0, 0), Vector3(1.5, 1, 1));
    parentMask = (uint32)-1; childMask = 0; index = 0;
    culled = box.culledBy(planes, index, parentMask, childMask);
    debugAssert(index == -1);
    debugAssert(! culled);
    debugAssert(childMask == b00000001);
    
    // Negative straddle
    box = AABox(Vector3(-1.5, 0, 0), Vector3(0.5, 1, 1));
    parentMask = (uint32)-1; childMask = 0; index = 0;
    culled = box.culledBy(planes, index, parentMask, childMask);
    debugAssert(index == -1);
    debugAssert(! culled);
    debugAssert(childMask == b00000010);

    // Full straddle
    box = AABox(Vector3(-1.5, 0, 0), Vector3(1.5, 1, 1));
    parentMask = (uint32)-1; childMask = 0; index = 0;
    culled = box.culledBy(planes, index, parentMask, childMask);
    debugAssert(index == -1);
    debugAssert(! culled);
    debugAssert(childMask == b00000011);

    // Negative culled 
    box = AABox(Vector3(-2.5, 0, 0), Vector3(-1.5, 1, 1));
    parentMask = (uint32)-1; childMask = 0; index = 0;
    culled = box.culledBy(planes, index, parentMask, childMask);
    debugAssert(index == 1);
    debugAssert(culled);

    // Positive culled 
    box = AABox(Vector3(1.5, 0, 0), Vector3(2.5, 1, 1));
    parentMask = (uint32)-1; childMask = 0; index = 0;
    culled = box.culledBy(planes, index, parentMask, childMask);
    debugAssert(index == 0);
    debugAssert(culled);

    GCamera camera;
    camera.getClipPlanes(Rect2D::xywh(0,0,640,480), planes);
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

    printf("System::memset:                     %d cycles/kb\n", (int)(g3d / (n / 1024)));
    printf("::memset      :                     %d cycles/kb\n", (int)(native / (n / 1024)));
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
		y = 0;
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
    {
    uint64 raw, opt;

    Vector3 v0(0, 0, 0);
    Vector3 v1(0, 0, -1);
    Vector3 v2(-1, 0, 0);
    Sphere sphere(Vector3(.5,1,-.5), 1);
    Vector3 vel(0, -1, 0);
    Vector3 location, normal;
    Triangle triangle(v0, v1, v2);
    int n = 1024;
    int i;

    System::beginCycleCount(raw);
    for (i = 0; i < n; ++i) {
        double t = CollisionDetection::collisionTimeForMovingSphereFixedTriangle(sphere, vel, Triangle(v0, v1, v2), location, normal);
        (void)t;
    }
    System::endCycleCount(raw);

    System::beginCycleCount(opt);
    for (i = 0; i < n; ++i) {
        double t = CollisionDetection::collisionTimeForMovingSphereFixedTriangle(sphere, vel, triangle, location, normal);
        (void)t;
    }
    System::endCycleCount(opt);

    printf("Sphere-Triangle collision detection on 3 vertices: %d cycles\n", (int)(raw / n));
    printf("Sphere-Triangle collision detection on Triangle:   %d cycles\n", (int)(opt / n));
    }
    {
        uint64 raw;

        Vector3 v0(0, 0, 0);
        Vector3 v1(0, 0, -1);
        Vector3 v2(-1, 0, 0);
        Sphere sphere(Vector3(.5,1,-.5), 1);
        Vector3 vel(0, -1, 0);
        Vector3 location, normal;
        Triangle triangle(v0, v1, v2);
        int n = 1024;
        int i;
        Ray ray = Ray::fromOriginAndDirection(Vector3(3,-1,-.25), vel);

        System::beginCycleCount(raw);
        for (i = 0; i < n; ++i) {
            double t = ray.intersectionTime(triangle);
                //CollisionDetection::collisionTimeForMovingPointFixedTriangle(
                //sphere, vel, triangle, location, normal);
            (void)t;
        }
        System::endCycleCount(raw);
        printf("Miss:\n");
        printf("ray.intersectionTime(triangle): %d cycles\n", (int)(raw / n));
    }
    {
        uint64 raw;

        Vector3 v0(0, 0, 0);
        Vector3 v1(0, 0, -1);
        Vector3 v2(-1, 0, 0);
        Vector3 vel(0, -1, 0);
        Vector3 location, normal;
        Triangle triangle(v0, v1, v2);
        int n = 1024;
        int i;
        Ray ray = Ray::fromOriginAndDirection(Vector3(-.15,1,-.15), vel);

        System::beginCycleCount(raw);
        for (i = 0; i < n; ++i) {
            double t = ray.intersectionTime(triangle);
            (void)t;
        }
        System::endCycleCount(raw);

        printf("Hit:\n");
        printf("ray.intersectionTime(triangle): %d cycles\n", (int)(raw / n));
    }
}


void measureAABoxCollisionPerformance() {
    printf("----------------------------------------------------------\n");

    uint64 raw, opt;

    AABox aabox(Vector3(-1, -1, -1), Vector3(1,2,3));
    Box   box = aabox.toBox();

    Vector3 pt1(0,10,0);
    Vector3 vel1(0,-1,0);
    Vector3 location, normal;
    int n = 1024;
    int i;

    System::beginCycleCount(raw);
    for (i = 0; i < n; ++i) {
        double t = CollisionDetection::collisionTimeForMovingPointFixedBox(
            pt1, vel1, box, location, normal);
        (void)t;
    }
    System::endCycleCount(raw);

    System::beginCycleCount(opt);
    for (i = 0; i < n; ++i) {
        double t = CollisionDetection::collisionTimeForMovingPointFixedAABox(
            pt1, vel1, aabox, location);
        (void)t;
    }
    System::endCycleCount(opt);

    printf("Ray-Box:   %d cycles\n", (int)(raw / n));
    printf("Ray-AABox: %d cycles\n", (int)(opt / n));
}





void testColor3uint8Array() {
    printf("Array<Color3uint8>\n");
    Array<Color3uint8> x(2);

    debugAssert(sizeof(Color3uint8) == 3);
    x[0].r = 60;
    x[0].g = 61;
    x[0].b = 62;
    x[1].r = 63;
    x[1].g = 64;
    x[1].b = 65;

    uint8* y = (uint8*)x.getCArray();
    (void)y;
    debugAssert(y[0] == 60);
    debugAssert(y[1] == 61);
    debugAssert(y[2] == 62);
    debugAssert(y[3] == 63);
    debugAssert(y[4] == 64);
    debugAssert(y[5] == 65);
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
    double x = nan();
    bool b1  = (x < 0.0);
    bool b2  = (x >= 0.0);
    bool b3  = !(b1 || b2);
    (void)b1;
    (void)b2;
    (void)b3;
    debugAssert(isNaN(nan()));
    debugAssert(! isNaN(4));
    debugAssert(! isNaN(0));
    debugAssert(! isNaN(inf()));
    debugAssert(! isNaN(-inf()));
    debugAssert(! isFinite(nan()));
    debugAssert(! isFinite(-inf()));
    debugAssert(! isFinite(inf()));
    debugAssert(isFinite(0));
		    
}

void testglFormatOf() {
    printf("glFormatOf\n");

    debugAssert(glFormatOf(Color3) == GL_FLOAT);
    debugAssert(glFormatOf(Color3uint8) == GL_UNSIGNED_BYTE);
    debugAssert(glFormatOf(Vector3int16) == GL_SHORT);
    debugAssert(glFormatOf(float) == GL_FLOAT);
    debugAssert(glFormatOf(int16) == GL_SHORT);
    debugAssert(glFormatOf(int32) == GL_INT);

    debugAssert(sizeOfGLFormat(GL_FLOAT) == 4);
}



void testSwizzle() {
    Vector4 v1(1,2,3,4);
    Vector2 v2;

    v2 = v1.xy() + v1.yz();
}

void measureRDPushPopPerformance(RenderDevice* rd) {
    uint64 identityCycles;

    int N = 500;
    rd->pushState();
    rd->popState();

    System::beginCycleCount(identityCycles);
    for (int i = 0; i < N; ++i) {
        rd->pushState();
        rd->popState();
    }
    System::endCycleCount(identityCycles);

    printf("RenderDevice::push+pop:             %g cycles\n", identityCycles / (double)N);
}



void testAABSPTreeSerialize() {
    printf("AABSPTree::serializeStructure\n");

    AABSPTree<Vector3> tree;
    int N = 1000;

    for (int i = 0; i < N; ++i) {
        tree.insert(Vector3::random());
    }
    tree.balance();

    // Save the struture
    BinaryOutput b("test-bsp.dat", G3D_LITTLE_ENDIAN);
    tree.serializeStructure(b);
    b.commit();

}


int main(int argc, char* argv[]) {    

/*
    BinaryInput b("C:/morgan/DefocusMatting/figures/aparatus.eps", G3D_LITTLE_ENDIAN);

    RealTime t0 = System::time();

    for (int i = 0; i < 1000000; ++i) {
        long L = b.readInt64();
    }

    printf("%g s\n", System::time() - t0);

    while(true);
    exit(-1);
*/

    RenderDevice* renderDevice = NULL;
    

    NetworkDevice* networkDevice = NULL;//new NetworkDevice();
    if (networkDevice) {networkDevice->init();}

    std::string s;
    System::describeSystem(s);
    printf("%s\n", s.c_str());

    if (networkDevice) {
        networkDevice->describeSystem(s);
        printf("%s\n", s.c_str());
    }

    if (networkDevice) {networkDevice->cleanup();}
    delete networkDevice;


#    ifndef _DEBUG
        printf("Performance analysis:\n\n");

        perfQueue();

        perfMatrix3();
        
        perfTextOutput();

        perfSystemMemcpy();

        perfArray();

        perfBinaryIO();

        measureBSPPerformance();
        measureTriangleCollisionPerformance();
        measureAABoxCollisionPerformance();
        measureAABoxCollisionPerformance();

        measureMemsetPerformance();
        measureNormalizationPerformance();

        GWindowSettings settings;
        settings.width = 800;
        settings.height = 600;
        settings.alphaBits = 0;
        settings.rgbBits = 8;
        settings.stencilBits = 0;
        settings.fsaaSamples = 1;

        if (!renderDevice) {
            renderDevice = new RenderDevice();
        }
    
        if (renderDevice) {
            renderDevice->describeSystem(s);
            printf("%s\n", s.c_str());
        }

        renderDevice->init(settings);
        measureRDPushPopPerformance(renderDevice);

#   else

    printf("\n\nTests:\n\n");

    testQueue();

    testMatrix();

    testArray();

    testMeshAlgTangentSpace();

    testGChunk();

    testAABSPTreeSerialize();
    printf("  passed\n");

#   ifdef RUN_SLOW_TESTS
        testHugeBinaryOutput();
        printf("  passed\n");
#   endif

    testBinaryIO();

    testReferenceCount();

    testPlane();
    printf("  passed\n");

    testAABoxCulledBy();
    printf("  passed\n");

    testRandom();
    printf("  passed\n");

    testAABoxCollision();
    printf("  passed\n");
    testAdjacency();
    printf("  passed\n");
    testWildcards();
    printf("  passed\n");

    testFloat();
    printf("  passed\n");
	testMemset();
    printf("  passed\n");

    testRandom();

    testSystemMemcpy();

    testTextInput();
    printf("  passed\n");
    testTable();
    printf("  passed\n");
    testBox();    
    printf("  passed\n");

    testCollisionDetection();    

    testColor3uint8Array();
    printf("  passed\n");
    testglFormatOf();
    printf("  passed\n");
    testSwizzle();

    printf("\nAll tests succeeded.\n");
#endif

    if (renderDevice) {
        renderDevice->cleanup();
        delete renderDevice;
    }
    return 0;
}

