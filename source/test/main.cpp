/**
 @file Test/main.cpp

 This file runs unit conformance and performance tests for G3D.  
 To write a new test, add a file named t<class>.cpp to the project
 and provide two entry points: test<class> and perf<class> (even if they are
 empty).

 Call those two methods from main() in main.cpp.

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-01-01
 @edited  2005-06-28
 */

#include "../include/G3DAll.h"
#include <iostream>
#include "../contrib/Matrix/Matrix.cpp"
//#include "../contrib/GChunk/GChunk.h"

using namespace G3D;

#ifdef G3D_WIN32
	#include "conio.h"
#endif
#include <string>


// Forward declarations
void perfArray();
void testArray();

void perfSystemMemcpy();
void testSystemMemcpy();


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
    printf("WeakReferenceCountedPointer...");

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
}

void testGChunk() {
    /*
    enum {HEADER, NAME, BODY, NUM, DATA};
    {
        BinaryOutput b("file.dat", G3D_LITTLE_ENDIAN);

        {
            GChunk c(b, HEADER);

            {
                GChunk c(b, NAME, STRING_BINFMT, 1);
                b.writeString("abcdefg");
                c.finish(b);
            }
            c.finish(b);
        }

        {
            GChunk c(b, BODY);

            {
                GChunk c(b, NUM, INT32_BINFMT, 1);
                b.writeInt32(10);
                c.finish(b);
            }

            {
                GChunk c(b, DATA, FLOAT32_BINFMT, 10);
                for (int i = 0; i < 10; ++i) {
                    b.writeFloat32(sqrt(i));
                }
                c.finish(b);
            }
            c.finish(b);
        }

        b.commit();
    }

    {

        BinaryInput b("file.dat", G3D_LITTLE_ENDIAN);

        {
            GChunk c(b, HEADER);

            {
                GChunk c(b, NAME, STRING_BINFMT, 1);
                alwaysAssertM(b.readString() == "abcdefg", "");
                c.finish(b);
            }
            c.finish(b);
        }

        {
            GChunk c(b, BODY);

            {
                GChunk c(b, NUM, INT32_BINFMT, 1);
                alwaysAssertM(b.readInt32 == 10, "");
                c.finish(b);
            }

            {
                GChunk c(b, DATA, FLOAT32_BINFMT, 10);
                for (int i = 0; i < 10; ++i) {
                    alwaysAssertM(b.readFloat32() == sqrt(i), "");
                }
                c.finish(b);
            }
            c.finish(b);
        }
    }
    */
}

void testTangentSpace() {
    printf("MeshAlg::computeTangentSpaceBasis ");

    MeshAlg::Geometry geometry;
    Array<MeshAlg::Face> face;
    Array<MeshAlg::Edge> edge;
    Array<MeshAlg::Vertex> vertex;

	Array<Vector2> texCoord;
    Array<Vector3> tangent;
    Array<Vector3> binormal;
	Array<int>	   index;

    // Create a quad
	double s = 2;
	geometry.vertexArray.append(Vector3(-s, -s, 0));
	geometry.vertexArray.append(Vector3( s, -s, 0));
	geometry.vertexArray.append(Vector3( s,  s, 0));
	geometry.vertexArray.append(Vector3(-s,  s, 0));

    texCoord.append(Vector2(0, 1));
	texCoord.append(Vector2(1, 1));
	texCoord.append(Vector2(1, 0));
	texCoord.append(Vector2(0, 0));

	index.append(0, 1, 2);
	index.append(0, 2, 3);

    MeshAlg::computeAdjacency(geometry.vertexArray, index, face, edge, vertex);
    MeshAlg::computeNormals(geometry, index);

    for (int i = 0; i < 4; ++i) {
        debugAssert(geometry.normalArray[i].fuzzyEq(Vector3::unitZ()));
    }

    MeshAlg::computeTangentSpaceBasis(geometry.vertexArray, texCoord, geometry.normalArray, face, tangent, binormal);

    for (int i = 0; i < 4; ++i) {
        debugAssert(tangent[i].fuzzyEq(Vector3::unitX()));
        debugAssert(binormal[i].fuzzyEq(-Vector3::unitY()));
    }

    printf("done!\n");
}


void testMatrix() {
    printf("Matrix ");
    // Zeros
    {
        Matrix M(3, 4);
        debugAssert(M.rows() == 3);
        debugAssert(M.cols() == 4);
        float x = M.get(0, 0);
        debugAssert(M.get(0, 0) == 0);
        debugAssert(M.get(1, 1) == 0);
    }

    // Identity
    {
        Matrix M = Matrix::identity(4);
        debugAssert(M.rows() == 4);
        debugAssert(M.cols() == 4);
        debugAssert(M.get(0, 0) == 1);
        float x = M.get(0, 1);
        debugAssert(M.get(0, 1) == 0);
    }

    // Add
    {
        Matrix A = Matrix::random(2, 3);
        Matrix B = Matrix::random(2, 3);
        Matrix C = A + B;
    
        for (int r = 0; r < 2; ++r) {
            for (int c = 0; c < 3; ++c) {
                debugAssert(fuzzyEq(C.get(r, c), A.get(r, c) + B.get(r, c)));
            }
        }
    }

    // Matrix multiply
    {
        Matrix A(2, 2);
        Matrix B(2, 2);

        A.set(0, 0, 1); A.set(0, 1, 3);
        A.set(1, 0, 4); A.set(1, 1, 2);

        B.set(0, 0, -6); B.set(0, 1, 9);
        B.set(1, 0, 1); B.set(1, 1, 7);

        Matrix C = A * B;

        debugAssert(fuzzyEq(C.get(0, 0), -3));
        debugAssert(fuzzyEq(C.get(0, 1), 30));
        debugAssert(fuzzyEq(C.get(1, 0), -22));
        debugAssert(fuzzyEq(C.get(1, 1), 50));
    }

    // Transpose
    {
        Matrix A(2, 2);

        A.set(0, 0, 1); A.set(0, 1, 3);
        A.set(1, 0, 4); A.set(1, 1, 2);

        Matrix C = A.transpose();

        debugAssert(fuzzyEq(C.get(0, 0), 1));
        debugAssert(fuzzyEq(C.get(0, 1), 4));
        debugAssert(fuzzyEq(C.get(1, 0), 3));
        debugAssert(fuzzyEq(C.get(1, 1), 2));

        A = Matrix::random(3, 4);
        A = A.transpose();

        debugAssert(A.rows() == 4);        
        debugAssert(A.cols() == 3);
    }

    // Copy-on-mutate
    {

        Matrix::debugNumCopyOps = Matrix::debugNumAllocOps = 0;

        Matrix A = Matrix::identity(2);

        debugAssert(Matrix::debugNumAllocOps == 1);
        debugAssert(Matrix::debugNumCopyOps == 0);

        Matrix B = A;
        debugAssert(Matrix::debugNumAllocOps == 1);
        debugAssert(Matrix::debugNumCopyOps == 0);

        B.set(0,0,4);
        debugAssert(B.get(0,0) == 4);
        debugAssert(A.get(0,0) == 1);
        debugAssert(Matrix::debugNumAllocOps == 2);
        debugAssert(Matrix::debugNumCopyOps == 1);
    }

    // Inverse
    {
        Matrix A(2, 2);

        A.set(0, 0, 1); A.set(0, 1, 3);
        A.set(1, 0, 4); A.set(1, 1, 2);

        Matrix C = A.inverse();

        float x = C.get(0, 0);
        debugAssert(fuzzyEq(C.get(0, 0), -0.2));
        debugAssert(fuzzyEq(C.get(0, 1), 0.3));
        debugAssert(fuzzyEq(C.get(1, 0), 0.4));
        debugAssert(fuzzyEq(C.get(1, 1), -0.1));
    }

    {
        Matrix A = Matrix::random(10, 10);
        Matrix B = A.inverse();

        B = B * A;

        for (int r = 0; r < B.rows(); ++r) {
            for (int c = 0; c < B.cols(); ++c) {
//                debugPrintf("%f ", B.get(r, c));

                float v = B.get(r, c);
                // The precision isn't great on our inverse, so be tolerant
                if (r == c) {
                    debugAssert(abs(v - 1) < 1e-4);
                } else {
                    debugAssert(abs(v) < 1e-4);
                }
            }
//            debugPrintf("\n");
        }
    }

    // Negate
    {
        Matrix A = Matrix::random(2, 2);
        Matrix B = -A;

        for (int r = 0; r < A.rows(); ++r) {
            for (int c = 0; c < A.cols(); ++c) {
                debugAssert(B.get(r, c) == -A.get(r, c));
            }
        }
    }

    // Transpose
    {
        Matrix A = Matrix::random(3,2);
        Matrix B = A.transpose();
        debugAssert(B.rows() == A.cols());
        debugAssert(B.cols() == A.rows());

        for (int r = 0; r < A.rows(); ++r) {
            for (int c = 0; c < A.cols(); ++c) {
                debugAssert(B.get(c, r) == A.get(r, c));
            }
        }
    }

    // SVD
    {
        Matrix A = Matrix::random(7, 7);//Matrix(3, 3);
        A.set(0, 0,  1.0);  A.set(0, 1,  2.0);  A.set(0, 2,  1.0);
        A.set(1, 0, -3.0);  A.set(1, 1,  7.0);  A.set(1, 2, -6.0);
        A.set(2, 0,  4.0);  A.set(2, 1, -4.0);  A.set(2, 2, 10.0);

        Matrix U, D, V;

        A.svd(U, D, V);

        // Verify that we can reconstruct
        Matrix B = U * D * V.transpose();

        Matrix test = abs(A - B) < 0.0001;

        debugAssert(test.allNonZero());

        double m = (A - B).norm() / A.norm();
        debugAssert(m < 0.01);
    }
}


void testHugeBinaryOutput() {
    printf("BinaryOutput Huge Files\n");
    if (fileExists("huge.bin")) {
        system("del huge.bin");
    }

    size_t s = 1024 * 1024 * 2;
    uint8* giantBuffer = (uint8*) malloc(s);
    debugAssert(giantBuffer);

    BinaryOutput* b = new BinaryOutput("huge.bin", G3D_LITTLE_ENDIAN);
    for (int i = 0; i = 1024 * 1024 * 1024 / s; ++i) {
        b->writeBytes(giantBuffer, s);
    }

    delete b;

    b = NULL;

}

void testBitSerialization() {
    printf("Bit Serialization\n");
    uint8 x[100];

    {
        BinaryOutput b("<memory>", G3D_LITTLE_ENDIAN);

        b.beginBits();
            b.writeBits(0, 1);
            b.writeBits(1, 1);
        b.endBits();

        b.commit(x);

        debugAssert(x[0] == 2);
    }

    {
        BinaryInput b(x, 1, G3D_LITTLE_ENDIAN);
        b.beginBits();
            
            uint8 a = b.readBits(1);
            debugAssert(a == 0);
            
            a = b.readBits(1);
            debugAssert(a == 1);
        b.endBits();
    }

    {
        BinaryOutput b("<memory>", G3D_LITTLE_ENDIAN);
        b.beginBits();
            b.writeBits(0xF1234567, 32);
        b.endBits();

        b.commit(x);

        debugAssert(x[0] == 0x67);
        debugAssert(x[1] == 0x45);
        debugAssert(x[2] == 0x23);
        debugAssert(x[3] == 0xF1);
    }

    {
        BinaryInput b(x, 4, G3D_LITTLE_ENDIAN);
        b.beginBits();
            
            uint8 a = b.readBits(8);
            debugAssert(a == 0x67);
            
            a = b.readBits(8);
            debugAssert(a == 0x45);
            
            a = b.readBits(8);
            debugAssert(a == 0x23);

            a = b.readBits(8);
            debugAssert(a == 0xF1);

        b.endBits();
    }

    {
        BinaryOutput b("<memory>", G3D_LITTLE_ENDIAN);

        b.beginBits();
            b.writeBits(0, 3);
            b.writeBits(3, 3);
            b.writeBits(4, 3);
            b.writeBits(7, 3);
        b.endBits();

        b.commit(x);
    }

    {
        BinaryInput b(x, 2, G3D_LITTLE_ENDIAN);
        b.beginBits();
            
            uint8 a = b.readBits(3);
            debugAssert(a == 0);
            
            a = b.readBits(3);
            debugAssert(a == 3);

            a = b.readBits(3);
            debugAssert(a == 4);

            a = b.readBits(3);
            debugAssert(a == 7);
        b.endBits();
    }

}


void measureSerializerPerformance() {
    Array<uint8> x(1024);
    RealTime t0 = System::time();
    Matrix4 M(Matrix4::identity());
    
    for (int i = 0; i < 100; ++i) {
        BinaryOutput b("<memory>", G3D_LITTLE_ENDIAN);
        b.writeInt32(1);
        b.writeInt32(2);
        b.writeInt32(8);
        M.serialize(b);
        b.commit(x.getCArray());
    }
    RealTime reallocTime = (System::time() - t0) / 100.0;
    printf("BinaryOutput time with re-allocation: %gs\n", reallocTime);

    BinaryOutput b("<memory>", G3D_LITTLE_ENDIAN);
    t0 = System::time();    
    for (int i = 0; i < 100; ++i) {
        b.writeInt32(1);
        b.writeInt32(2);
        b.writeInt32(8);
        M.serialize(b);
        b.commit(x.getCArray());
        b.reset();
    }
    RealTime resetTime = (System::time() - t0) / 100.0;
    printf("BinaryOutput time with BinaryOutpu::reset: %gs\n\n", resetTime);
    
}


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


int numRCPFoo = 0;
class RCPFoo : public G3D::ReferenceCountedObject {
public:
    int x;
    RCPFoo() {
        ++numRCPFoo;
    }
    ~RCPFoo() {
        --numRCPFoo;
    }
};

typedef G3D::ReferenceCountedPointer<RCPFoo> RCPFooRef;




class Reftest : public ReferenceCountedObject {
public:
    static Array<std::string> sequence;
    const char* s;
    Reftest(const char* s) : s(s){
        debugPrintf("alloc 0x%x (%s)\n", this, s);
        sequence.append(format("%s", s));
    }
    ~Reftest() {
        debugPrintf("free 0x%x (~%s)\n", this, s);
        sequence.append(format("~%s", s));
    }
};
class Reftest2 : public Reftest {
public:
    Reftest2() : Reftest("2") {
    }
};
typedef ReferenceCountedPointer<Reftest> ARef;
typedef ReferenceCountedPointer<Reftest2> ARef2;
Array<std::string> Reftest::sequence;


void testRCP() {
    printf("ReferenceCountedPointer");

    debugAssert(numRCPFoo == 0);
    RCPFooRef a = new RCPFoo();
    debugAssert(numRCPFoo == 1);
    debugAssert(a.isLastReference());

    {
        RCPFooRef b = new RCPFoo();
        debugAssert(numRCPFoo == 2);
        b = a;
        debugAssert(numRCPFoo == 1);
        debugAssert(! a.isLastReference());
        debugAssert(! b.isLastReference());
    }

    debugAssert(a.isLastReference());
    debugAssert(numRCPFoo == 1);


    // Test allocation and deallocation of 
    // reference counted values.
    {
        ARef a = new Reftest("a");
        ARef b = new Reftest("b");

        a = b;
        Reftest::sequence.append("--");
        debugPrintf("---------\n");
        b = NULL;
        Reftest::sequence.append("--");
        debugPrintf("---------\n");
    }

    debugAssert(Reftest::sequence[0] == "a");
    debugAssert(Reftest::sequence[1] == "b");
    debugAssert(Reftest::sequence[2] == "~a");
    debugAssert(Reftest::sequence[3] == "--");
    debugAssert(Reftest::sequence[4] == "--");
    debugAssert(Reftest::sequence[5] == "~b");

    Reftest::sequence.clear();

    // Test type hierarchies with reference counted values.

    {
        ARef one = new Reftest("1");
        ARef2 two = new Reftest2();

        one = (ARef)two;
    }
    debugAssert(Reftest::sequence[0] == "1");
    debugAssert(Reftest::sequence[1] == "2");
    debugAssert(Reftest::sequence[2] == "~1");
    debugAssert(Reftest::sequence[3] == "~2");
    Reftest::sequence.clear();

    {
        ARef one = new Reftest2();
    }
    debugAssert(Reftest::sequence[0] == "2");
    debugAssert(Reftest::sequence[1] == "~2");
    Reftest::sequence.clear();

    {
        // Should not compile
//        ARef2 one = new Reftest("1");
    }
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

        double aaTime = CollisionDetection::collisionTimeForMovingPointFixedAABox(
            pt1, vel1, aabox, aaboxlocation);

        Ray ray = Ray::fromOriginAndDirection(pt1, vel1);
        double rayboxTime = ray.intersectionTime(box);

        double rayaaTime = ray.intersectionTime(aabox);

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


void measureMatrix3Performance() {
    printf("----------------------------------------------------------\n");
    printf("Matrix3 performance:\n");
    uint64 raw, opt, overhead;
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

    raw -= overhead;
    opt -= overhead;

    printf("     C = A * B: %g cycles/mul\n", 
        (double)raw / (3*n));
    printf("  mul(A, B, C): %g cycles/mul\n\n", 
        (double)opt / (3*n));
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


void testTextInput() {
    printf("TextInput\n");

	{
 		TextInput ti(TextInput::FROM_STRING, "a \'foo\' bar");

		ti.readSymbol("a");
		Token t = ti.read();
		debugAssert(t.extendedType() == Token::SINGLE_QUOTED_TYPE);
		debugAssert(t.string() == "foo");
		ti.readSymbol("bar");
	}

    {
        TextInput ti(TextInput::FROM_STRING, "2.x");

        debugAssert(ti.readNumber() == 2);
        ti.readSymbol("x");
    }
    {
        TextInput ti(TextInput::FROM_STRING, "1.E7");
        debugAssert(ti.readNumber() == 1.E7);
    }

    {
        TextInput ti(TextInput::FROM_STRING, "\\123");
        Token t;
        t = ti.read();
        debugAssert(t.type() == Token::SYMBOL);
        debugAssert(t.string() == "\\");
        t = ti.read();
        debugAssert(t.type() == Token::NUMBER);
        debugAssert(t.number() == 123);
    }

    {
        TextInput::Options options;
        options.otherCommentCharacter = '#';

        TextInput ti(TextInput::FROM_STRING, "1#23\nA\\#2", options);
        Token t;
        t = ti.read();
        debugAssert(t.type() == Token::NUMBER);
        debugAssert(t.number() == 1);

        // skip the comment
        t = ti.read();
        debugAssert(t.type() == Token::SYMBOL);
        debugAssert(t.string() == "A");

        // Read escaped comment character
        t = ti.read();
        debugAssert(t.type() == Token::SYMBOL);
        debugAssert(t.string() == "#");

        t = ti.read();
        debugAssert(t.type() == Token::NUMBER);
        debugAssert(t.number() == 2);
    }

    {
        TextInput ti(TextInput::FROM_STRING, "0xFEED");

        Token t;
   
        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        double n = ti.readNumber();
        debugAssert((int)n == 0xFEED);

        t = ti.read();
        debugAssert(t.type() == Token::END);
    }
    

    TextInput::Options opt;
    opt.cppComments = false;
    TextInput ti(TextInput::FROM_STRING, 
                 "if/*comment*/(x->y==-1e6){cout<<\"hello world\"}; // foo\nbar",
                 opt);

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
    Token q = ti.read();
    Token r = ti.read();
    Token s = ti.read();
    Token t = ti.read();

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

    debugAssert(p.type() == Token::SYMBOL);
    debugAssert(p.string() == "/");

    debugAssert(q.type() == Token::SYMBOL);
    debugAssert(q.string() == "/");

    debugAssert(r.type() == Token::SYMBOL);
    debugAssert(r.string() == "foo");

    debugAssert(s.type() == Token::SYMBOL);
    debugAssert(s.string() == "bar");

    debugAssert(t.type() == Token::END);
    
    {
        TextInput ti(TextInput::FROM_STRING, "-1 +1 2.6");

        Token t;
   
        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        double n = ti.readNumber();
        debugAssert(n == -1);

        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        n = ti.readNumber();
        debugAssert(n == 1);

        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        n = ti.readNumber();
        debugAssert(n == 2.6);
    }

    {
        TextInput ti(TextInput::FROM_STRING, "- 1 ---.51");

        Token t;
   
        t = ti.peek();
        debugAssert(t.type() == Token::SYMBOL);
        ti.readSymbol("-");

        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        double n = ti.readNumber();
        debugAssert(n == 1);

        t = ti.peek();
        debugAssert(t.type() == Token::SYMBOL);
        ti.readSymbol("--");

        t = ti.peek();
        debugAssert(t.type() == Token::NUMBER);
        n = ti.readNumber();
        debugAssert(n == -.51);
    }
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
    debugAssert(y[0] == 60);
    debugAssert(y[1] == 61);
    debugAssert(y[2] == 62);
    debugAssert(y[3] == 63);
    debugAssert(y[4] == 64);
    debugAssert(y[5] == 65);
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
    debugAssert(i == 1234); (void)i;
    double j = g.readFloat64();
    debugAssert(j == 1.234); (void)j;
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


void testSort() {
    printf("Array::Sort\n");

    {
        Array<int> array;
        array.append(12, 7, 1);
        array.append(2, 3, 10);
    
        array.sort();

        debugAssert(array[0] == 1);
        debugAssert(array[1] == 2);
        debugAssert(array[2] == 3);
        debugAssert(array[3] == 7);
        debugAssert(array[4] == 10);
        debugAssert(array[5] == 12);
    }

    {
        Array<int> array;
        array.append(12, 7, 1);
        array.append(2, 3, 10);
    
        array.sortSubArray(0, 2);

        debugAssert(array[0] == 1);
        debugAssert(array[1] == 7);
        debugAssert(array[2] == 12);
        debugAssert(array[3] == 2);
        debugAssert(array[4] == 3);
        debugAssert(array[5] == 10);
    }
}


void testCollision() {
    printf("CollisionDetection\n");


    {
        Sphere s(Vector3(0,1,0), 1);
        Plane  p = Plane::fromEquation(0,1,0,0);
        Array<Vector3> contact;
        Array<Vector3> outNormal;

        float depth = CollisionDetection::penetrationDepthForFixedSphereFixedPlane(
            s, p, contact, outNormal);

        debugAssert(outNormal[0] == -p.normal());
        debugAssert(depth == 0);

        s = Sphere(Vector3(0,2,0), 1);

        depth = CollisionDetection::penetrationDepthForFixedSphereFixedPlane(
            s, p, contact, outNormal);

        debugAssert(depth < 0);
    }

    {
        Sphere s(Vector3(0,1,0), 1);
        Sphere r(Vector3(0,-.5,0), 1);
        Array<Vector3> contact;
        Array<Vector3> outNormal;

        float depth = CollisionDetection::penetrationDepthForFixedSphereFixedSphere(
            s, r, contact, outNormal);

        debugAssert(outNormal[0] == Vector3(0,-1,0));
        debugAssert(depth == .5);
    }

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

    {
        
        Vector3 v0(0, 0, 0);
        Vector3 v1(0, 0, -1);
        Vector3 v2(-1, 0, 0);
        Ray ray = Ray::fromOriginAndDirection(Vector3(-.15, 1,-.15), Vector3(0, -1, 0));
        Vector3 location, normal;
        double t = ray.intersectionTime(v0, v1, v2);
        debugAssert(t == 1.0);

        ray.origin.y = -ray.origin.y;
        t = ray.intersectionTime(v0, v1, v2);
        debugAssert(t == inf());

        // One-sided test
        ray.direction.y = -ray.direction.y;
        ray.origin.y = -ray.origin.y;
        t = ray.intersectionTime(v0, v1, v2);
        debugAssert(t == inf());

        // Time scale
        ray = Ray::fromOriginAndDirection(Vector3(-.15, 1,-.15), Vector3(0, -2, 0));
        t = ray.intersectionTime(v0, v1, v2);

        debugAssert(t == 0.5);
    }
}


void testAdjacency() {
    printf("MeshAlg::computeAdjacency\n");

    {
        //          2
        //        /|
        //       / |
        //      /  |
        //     /___|
        //    0     1
        //
        //


        MeshAlg::Geometry       geometry;
        Array<int>              index;
        Array<MeshAlg::Face>    faceArray;
        Array<MeshAlg::Edge>    edgeArray;
        Array<MeshAlg::Vertex>  vertexArray;

        geometry.vertexArray.append(Vector3(0,0,0));
        geometry.vertexArray.append(Vector3(1,0,0));
        geometry.vertexArray.append(Vector3(1,1,0));

        index.append(0, 1, 2);

        MeshAlg::computeAdjacency(
            geometry.vertexArray,
            index,
            faceArray,
            edgeArray,
            vertexArray);

        debugAssert(faceArray.size() == 1);
        debugAssert(edgeArray.size() == 3);

        debugAssert(faceArray[0].containsVertex(0));
        debugAssert(faceArray[0].containsVertex(1));
        debugAssert(faceArray[0].containsVertex(2));

        debugAssert(faceArray[0].containsEdge(0));
        debugAssert(faceArray[0].containsEdge(1));
        debugAssert(faceArray[0].containsEdge(2));

        debugAssert(edgeArray[0].inFace(0));
        debugAssert(edgeArray[1].inFace(0));
        debugAssert(edgeArray[2].inFace(0));

        MeshAlg::debugCheckConsistency(faceArray, edgeArray, vertexArray);

        // Severely weld, creating a degenerate face
        MeshAlg::weldAdjacency(geometry.vertexArray, faceArray, edgeArray, vertexArray, 1.1);

        MeshAlg::debugCheckConsistency(faceArray, edgeArray, vertexArray);
        debugAssert(! faceArray[0].containsVertex(0));

    }

    {
        //      
        //    0====1
        //  (degenerate face)
        //

        MeshAlg::Geometry       geometry;
        Array<int>              index;
        Array<MeshAlg::Face>    faceArray;
        Array<MeshAlg::Edge>    edgeArray;
        Array<MeshAlg::Vertex>  vertexArray;

        geometry.vertexArray.append(Vector3(0,0,0));
        geometry.vertexArray.append(Vector3(1,0,0));

        index.append(0, 1, 0);

        MeshAlg::computeAdjacency(
            geometry.vertexArray,
            index,
            faceArray,
            edgeArray,
            vertexArray);

        debugAssert(faceArray.size() == 1);
        debugAssert(edgeArray.size() == 2);

        debugAssert(faceArray[0].containsVertex(0));
        debugAssert(faceArray[0].containsVertex(1));

        debugAssert(faceArray[0].containsEdge(0));
        debugAssert(faceArray[0].containsEdge(1));

        debugAssert(edgeArray[0].inFace(0));
        debugAssert(edgeArray[1].inFace(0));
        MeshAlg::debugCheckConsistency(faceArray, edgeArray, vertexArray);
    }

    {
        //          2                       .
        //        /|\                       .
        //       / | \                      .
        //      /  |  \                     . 
        //     /___|___\                    . 
        //    0     1    3
        //
        //


        MeshAlg::Geometry       geometry;
        Array<int>              index;
        Array<MeshAlg::Face>    faceArray;
        Array<MeshAlg::Edge>    edgeArray;
        Array<MeshAlg::Vertex>  vertexArray;

        geometry.vertexArray.append(Vector3(0,0,0));
        geometry.vertexArray.append(Vector3(1,0,0));
        geometry.vertexArray.append(Vector3(1,1,0));
        geometry.vertexArray.append(Vector3(2,0,0));

        index.append(0, 1, 2);
        index.append(1, 3, 2);

        MeshAlg::computeAdjacency(
            geometry.vertexArray,
            index,
            faceArray,
            edgeArray,
            vertexArray);

        debugAssert(faceArray.size() == 2);
        debugAssert(edgeArray.size() == 5);
        debugAssert(vertexArray.size() == 4);

        debugAssert(faceArray[0].containsVertex(0));
        debugAssert(faceArray[0].containsVertex(1));
        debugAssert(faceArray[0].containsVertex(2));

        debugAssert(faceArray[1].containsVertex(3));
        debugAssert(faceArray[1].containsVertex(1));
        debugAssert(faceArray[1].containsVertex(2));

        // The non-boundary edge must be first
        debugAssert(! edgeArray[0].boundary());
        debugAssert(edgeArray[1].boundary());
        debugAssert(edgeArray[2].boundary());
        debugAssert(edgeArray[3].boundary());
        debugAssert(edgeArray[4].boundary());

        MeshAlg::debugCheckConsistency(faceArray, edgeArray, vertexArray);

        MeshAlg::weldAdjacency(geometry.vertexArray, faceArray, edgeArray, vertexArray);

        MeshAlg::debugCheckConsistency(faceArray, edgeArray, vertexArray);

        debugAssert(faceArray.size() == 2);
        debugAssert(edgeArray.size() == 5);
        debugAssert(vertexArray.size() == 4);
    }


    {
        // Test Welding


        //         2                  .
        //        /|\                 .
        //       / | \                .
        //      /  |  \               .
        //     /___|___\              .
        //    0   1,4   3
        //


        MeshAlg::Geometry       geometry;
        Array<int>              index;
        Array<MeshAlg::Face>    faceArray;
        Array<MeshAlg::Edge>    edgeArray;
        Array<MeshAlg::Vertex>  vertexArray;

        geometry.vertexArray.append(Vector3(0,0,0));
        geometry.vertexArray.append(Vector3(1,0,0));
        geometry.vertexArray.append(Vector3(1,1,0));
        geometry.vertexArray.append(Vector3(2,0,0));
        geometry.vertexArray.append(Vector3(1,0,0));

        index.append(0, 1, 2);
        index.append(2, 4, 3);

        MeshAlg::computeAdjacency(
            geometry.vertexArray,
            index,
            faceArray,
            edgeArray,
            vertexArray);

        debugAssert(faceArray.size() == 2);
        debugAssert(edgeArray.size() == 6);
        debugAssert(vertexArray.size() == 5);

        debugAssert(edgeArray[0].boundary());
        debugAssert(edgeArray[1].boundary());
        debugAssert(edgeArray[2].boundary());
        debugAssert(edgeArray[3].boundary());
        debugAssert(edgeArray[4].boundary());
        debugAssert(edgeArray[5].boundary());

        debugAssert(faceArray[0].containsVertex(0));
        debugAssert(faceArray[0].containsVertex(1));
        debugAssert(faceArray[0].containsVertex(2));

        debugAssert(faceArray[1].containsVertex(2));
        debugAssert(faceArray[1].containsVertex(3));
        debugAssert(faceArray[1].containsVertex(4));

        MeshAlg::debugCheckConsistency(faceArray, edgeArray, vertexArray);

        MeshAlg::weldAdjacency(geometry.vertexArray, faceArray, edgeArray, vertexArray);

        MeshAlg::debugCheckConsistency(faceArray, edgeArray, vertexArray);

        debugAssert(faceArray.size() == 2);
        debugAssert(edgeArray.size() == 5);
        debugAssert(vertexArray.size() == 5);

        debugAssert(! edgeArray[0].boundary());
        debugAssert(edgeArray[1].boundary());
        debugAssert(edgeArray[2].boundary());
        debugAssert(edgeArray[3].boundary());
        debugAssert(edgeArray[4].boundary());
    }
    {
        // Test Welding


        //        2,5
        //        /|\
        //       / | \
        //      /  |  \
        //     /___|___\
        //    0   1,4   3
        //


        MeshAlg::Geometry       geometry;
        Array<int>              index;
        Array<MeshAlg::Face>    faceArray;
        Array<MeshAlg::Edge>    edgeArray;
        Array<MeshAlg::Vertex>  vertexArray;

        geometry.vertexArray.append(Vector3(0,0,0));
        geometry.vertexArray.append(Vector3(1,0,0));
        geometry.vertexArray.append(Vector3(1,1,0));
        geometry.vertexArray.append(Vector3(2,0,0));
        geometry.vertexArray.append(Vector3(1,0,0));
        geometry.vertexArray.append(Vector3(1,1,0));

        index.append(0, 1, 2);
        index.append(5, 4, 3);

        MeshAlg::computeAdjacency(
            geometry.vertexArray,
            index,
            faceArray,
            edgeArray,
            vertexArray);

        debugAssert(faceArray.size() == 2);
        debugAssert(edgeArray.size() == 6);
        debugAssert(vertexArray.size() == 6);

        debugAssert(edgeArray[0].boundary());
        debugAssert(edgeArray[1].boundary());
        debugAssert(edgeArray[2].boundary());
        debugAssert(edgeArray[3].boundary());
        debugAssert(edgeArray[4].boundary());
        debugAssert(edgeArray[5].boundary());

        debugAssert(faceArray[0].containsVertex(0));
        debugAssert(faceArray[0].containsVertex(1));
        debugAssert(faceArray[0].containsVertex(2));

        debugAssert(faceArray[1].containsVertex(5));
        debugAssert(faceArray[1].containsVertex(3));
        debugAssert(faceArray[1].containsVertex(4));

        MeshAlg::debugCheckConsistency(faceArray, edgeArray, vertexArray);

        MeshAlg::weldAdjacency(geometry.vertexArray, faceArray, edgeArray, vertexArray);

        MeshAlg::debugCheckConsistency(faceArray, edgeArray, vertexArray);

        debugAssert(faceArray.size() == 2);
        debugAssert(edgeArray.size() == 5);
        debugAssert(vertexArray.size() == 6);

        debugAssert(! edgeArray[0].boundary());
        debugAssert(edgeArray[1].boundary());
        debugAssert(edgeArray[2].boundary());
        debugAssert(edgeArray[3].boundary());
        debugAssert(edgeArray[4].boundary());

    }
    
}

void testRandom() {
    printf("Random number generators\n");

    int num0 = 0;
    int num1 = 0;
    for (int i = 0; i < 10000; ++i) {
        switch (iRandom(0, 1)) {
        case 0:
            ++num0;
            break;

        case 1:
            ++num1;
            break;

        default:
            debugAssertM(false, "Random number outside the range [0, 1] from iRandom(0,1)");
        }
    }
    int difference = iAbs(num0 - num1);
    debugAssertM(difference < 300, "Integer random number generator appears skewed.");

    for (int i = 0; i < 100; ++i) {
        double r = random(0, 1);
        debugAssert(r >= 0.0);
        debugAssert(r <= 1.0);
    }


    // Triangle.randomPoint
    Triangle tri(Vector3(0,0,0), Vector3(1,0,0), Vector3(0,1,0));
    for (int i = 0; i < 1000; ++i) {
        Vector3 p = tri.randomPoint();
        debugAssert(p.z == 0);
        debugAssert(p.x <= 1.0 - p.y);
        debugAssert(p.x >= 0);
        debugAssert(p.y >= 0);
        debugAssert(p.x <= 1.0);
        debugAssert(p.y <= 1.0);
    }


    for (int i = 0; i < 100; i++) {
         Vector3 point = tri.randomPoint();
         debugAssert(CollisionDetection::isPointInsideTriangle(
                 tri.vertex(0),
                 tri.vertex(1),
                 tri.vertex(2),
                 tri.normal(),
                 point));
    }

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

    #ifndef _DEBUG
        printf("Performance analysis:\n\n");

        perfSystemMemcpy();

        while(true);
        perfArray();

        measureBSPPerformance();
        measureTriangleCollisionPerformance();
        measureAABoxCollisionPerformance();
        measureAABoxCollisionPerformance();
        measureMatrix3Performance();

        measureMemsetPerformance();
        measureNormalizationPerformance();
        measureSerializerPerformance();

        GWindowSettings settings;
        settings.width = 800;
        settings.height = 600;
        settings.alphaBits = 0;
        settings.rgbBits = 8;
        settings.stencilBits = 0;
        settings.fsaaSamples = 1;

        RenderDevice* renderDevice = new RenderDevice();
        renderDevice->init(settings);
        measureRDPushPopPerformance(renderDevice);
        renderDevice->cleanup();
        delete renderDevice;

        return 0;
    #endif

    printf("\n\nTests:\n\n");

    testArray();

    testTangentSpace();
    printf("  passed\n");

    testMatrix();
    printf("  passed\n");

    testAABSPTreeSerialize();
    printf("  passed\n");

#   ifdef RUN_SLOW_TESTS
        testHugeBinaryOutput();
        printf("  passed\n");
#   endif

    testBitSerialization();
    printf("  passed\n");

    testWeakPointer();
    printf("  passed\n");

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
    testSort();
    printf("  passed\n");
    testWildcards();
    printf("  passed\n");
    testRCP();
    printf("  passed\n");
    testFloat();
    printf("  passed\n");
	testMemset();
    printf("  passed\n");

    testSystemMemcpy();

    testCompression();
    printf("  passed\n");
    testTextInput();
    printf("  passed\n");
    testTable();
    printf("  passed\n");
    testBox();    
    printf("  passed\n");
    testCollision();    
    printf("  passed\n");
    testColor3uint8Array();
    printf("  passed\n");
    testglFormatOf();
    printf("  passed\n");
    testSwizzle();

    printf("\nAll tests succeeded.\n");

    return 0;
}

