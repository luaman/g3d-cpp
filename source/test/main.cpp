/**
 @file Test/main.cpp

 This file contains some unit tests for G3D, as well as commented 
 out scratch code useful when debugging.

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-01-01
 @edited  2003-10-06
 */


#include "../include/G3DAll.h"
#include <iostream>

using namespace G3D;

#ifdef _WIN32
	#include "conio.h"
    #include "Window.h"
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

    printf("Array alloc:                        %d cycles/elt\n", (int)(g3dNew / size));
    printf("int* alloc:                         %d cycles/elt\n", (int)(nativeNew / size));
    printf("Array free:                         %d cycles/elt\n", (int)(g3dDel / size));
    printf("int* free:                          %d cycles/elt\n", (int)(nativeDel / size));
    printf("Array<int>[]:                       %g cycles\n", (g3d / (size * 2.0)));
    printf("int*[]:                             %g cycles\n", (native / (size * 2.0)));
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

    printf("System::memcpy:                     %d cycles/kb\n", (int)(g3d / (n / 1024)));
    printf("::memcpy      :                     %d cycles/kb\n", (int)(native / (n / 1024)));
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
    Triangle triangle(v0, v1, v2);
    int n = 1024;
    int i;

    System::beginCycleCount(raw);
    for (i = 0; i < n; ++i) {
        double t = CollisionDetection::collisionTimeForMovingSphereFixedTriangle(sphere, vel, Triangle(v0, v1, v2), location, normal);
    }
    System::endCycleCount(raw);

    System::beginCycleCount(opt);
    for (i = 0; i < n; ++i) {
        double t = CollisionDetection::collisionTimeForMovingSphereFixedTriangle(sphere, vel, triangle, location, normal);
    }
    System::endCycleCount(opt);

    printf("Sphere-Triangle collision detection on 3 vertices: %d cycles\n", (int)(raw / n));
    printf("Sphere-Triangle collision detection on CDTriangle: %d cycles\n", (int)(opt / n));
}


void testTextInput() {
    printf("TextInput\n");

    

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

    exit(-1);
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
    debugAssert(i == 1234); i;
    double j = g.readFloat64();
    debugAssert(j == 1.234); j;
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

void testglFormatOf() {
    printf("glFormatOf\n");

    debugAssert(glFormatOf(Color3) == GL_FLOAT);
    debugAssert(glFormatOf(Color3uint8) == GL_UNSIGNED_BYTE);
    debugAssert(glFormatOf(Vector3int16) == GL_SHORT);
    debugAssert(glFormatOf(float) == GL_FLOAT);
    debugAssert(glFormatOf(int16) == GL_SHORT);

    debugAssert(sizeOfGLFormat(GL_FLOAT) == 32);
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


void testSwizzle() {
    Vector4 v1(1,2,3,4);
    Vector2 v2;

    v2 = v1.xy() + v1.yz();
}


/*
int main(int argc, char* argv[]) {

    #ifndef _DEBUG
        printf("Performance analysis:\n\n");
        measureArrayPerformance();
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
    testColor3uint8Array();
    printf("  passed\n");
    testglFormatOf();
    printf("  passed\n");
    testSwizzle();

    printf("\nAll tests succeeded.\n");
	
    while(true);

    return 0;
}
*/

std::string             DATA_DIR;

Log*                    debugLog		= NULL;
RenderDevice*           renderDevice	= NULL;
CFontRef                font			= NULL;
UserInput*              userInput		= NULL;
Camera*					camera			= NULL;
ManualCameraController* controller      = NULL;
bool                    endProgram		= false;


RealTime getTime() {
    return SDL_GetTicks() / 1000.0;
}

void doSimulation(GameTime timeStep);
void doGraphics();
void doUserInput();


int main(int argc, char** argv) {

    // Initialize
    DATA_DIR     = demoFindData();
    debugLog	 = new Log();
    renderDevice = new RenderDevice();
    renderDevice->init(800, 600, debugLog, 1.0, false, 0, true, 8, 0, 24, 0);
    camera 	     = new Camera(renderDevice);

    font         = CFont::fromFile(renderDevice, DATA_DIR + "font/dominant.fnt");

    userInput    = new UserInput();

    controller   = new ManualCameraController(renderDevice, userInput);
    controller->setMoveRate(10);

    controller->setPosition(Vector3(15, 20, 15));
    controller->lookAt(Vector3(-2,3,-5));

    renderDevice->resetState();
	renderDevice->setColorClearValue(Color3(.1, .5, 1));

    controller->setActive(true);

    RealTime now = getTime() - 0.001, lastTime;

    // Main loop
    do {
        lastTime = now;
        now = getTime();
        RealTime timeStep = now - lastTime;

        doUserInput();

        doSimulation(timeStep);

        doGraphics();
   
    } while (! endProgram);


    // Cleanup
    delete controller;
    delete userInput;
    renderDevice->cleanup();
    delete renderDevice;
    delete debugLog;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////


void doSimulation(GameTime timeStep) {
    // Simulation
    controller->doSimulation(clamp(timeStep, 0.0, 0.1));
	camera->setCoordinateFrame(controller->getCoordinateFrame());
}


class Draw {
private:

    /** Called from wireSphere, wireCapsule */
    static void wireSphereSection(
        const Sphere&       sphere,
        RenderDevice*       renderDevice,
        const Color4&       color,
        bool                top,
        bool                bottom);


    static void sphereSection(
        const Sphere&       sphere,
        RenderDevice*       renderDevice,
        const Color4&       color,
        bool                top,
        bool                bottom);

    /**
     Returns the scale due to perspective at
     a point for a line.
     */
    static double perspectiveLineThickness(
        RenderDevice*       rd,
        const Vector3&      pt);

public:

    static void wireBox(
        const Box&          box,
        RenderDevice*       rd,
        const Color4&       color = Color3::RED);

    static void box(
        const Box&          box,
        RenderDevice*       rd,
        const Color4&       color = Color3::RED);

    static void wireSphere(
        const Sphere&       sphere,
        RenderDevice*       rd,
        const Color4&       color = Color3::YELLOW);

    static void sphere(
        const Sphere&       sphere,
        RenderDevice*       rd,
        const Color4&       color);

    /** TODO */
    static void wirePlane(
        const Plane&        plane,
        RenderDevice*       rd,
        const Color4&       color = Color3::BLUE);

    static void line(
        const Line&         line,
        RenderDevice*       rd,
        const Color4&       color = Color3::BLACK);

    static void lineSegment(
        const LineSegment&  lineSegment,
        RenderDevice*       rd,
        const Color4&       color = Color3::BLACK);

    /**
     Renders per-vertex normals as thin arrows.  The length
     of the normals is scaled inversely to the number of normals
     rendered.
     */
    static void vertexNormals(
        const MeshAlg::Geometry&    geometry,
        RenderDevice*               renderDevice,
        const Color4&               color = Color3::GREEN * .5,
        double                      scale = 1);

    void wireCapsule(
       const Capsule&       capsule, 
       RenderDevice*        renderDevice,
       const Color4&        color = Color3::PURPLE);

    void capsule(
       const Capsule&       capsule, 
       RenderDevice*        renderDevice,
       const Color4&        color = Color3::PURPLE);

    static void ray(
        const Ray&          ray,
        RenderDevice*       renderDevice,
        const Color4&       color = Color3::ORANGE,
        double              scale = 1);
    
    static void arrow(
        const Vector3&      start,
        const Vector3&      direction,
        RenderDevice*       renderDevice,
        const Color4&       color = Color3::ORANGE,
        double              scale = 1.0) {
        ray(Ray::fromOriginAndDirection(start, direction * scale), renderDevice, color, scale);
    }

    static void axes(
        const CoordinateFrame& cframe,
        RenderDevice*       renderDevice,
        double              scale = 1.0);

    static void axes(
        RenderDevice*       renderDevice,
        double              scale = 1.0) {
        axes(CoordinateFrame(), renderDevice, scale);
    }
};



void Draw::axes(
    const CoordinateFrame& cframe,
    RenderDevice*       renderDevice,
    double              scale) {

    // TODO: set line thickness and text label scale based on
    // distance to camera

    Vector3 c = cframe.translation;
    Vector3 x = cframe.rotation.getColumn(0).direction();
    Vector3 y = cframe.rotation.getColumn(1).direction();
    Vector3 z = cframe.rotation.getColumn(2).direction();

    Draw::arrow(c, x, renderDevice, Color3::RED, scale);
    Draw::arrow(c, y, renderDevice, Color3::GREEN, scale);
    Draw::arrow(c, z, renderDevice, Color3::BLUE, scale);
  
    // Text label scale
    const double xx = -3;
    const double yy = xx * 1.4;

    Vector3 xc2D = renderDevice->project(c + x * 1.2);
    Vector3 yc2D = renderDevice->project(c + y * 1.2);
    Vector3 zc2D = renderDevice->project(c + z * 1.2);

    // If coordinates are behind the viewer, transform off screen
    Vector2 x2D = (xc2D.z > 0) ? xc2D.xy() : Vector2(-1000, -1000);
    Vector2 y2D = (yc2D.z > 0) ? yc2D.xy() : Vector2(-1000, -1000);
    Vector2 z2D = (zc2D.z > 0) ? zc2D.xy() : Vector2(-1000, -1000);

    double xS = (xc2D.z > 0) ? clamp(10 * xc2D.z, .1, 5) : 0;
    double yS = (yc2D.z > 0) ? clamp(10 * yc2D.z, .1, 5) : 0;
    double zS = (zc2D.z > 0) ? clamp(10 * zc2D.z, .1, 5) : 0;

    renderDevice->push2D();
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        renderDevice->setLineWidth(2);

        renderDevice->beginPrimitive(RenderDevice::LINES);
            // X
            renderDevice->setColor(Color3::RED);
            renderDevice->sendVertex(Vector2(-xx,  yy) * xS + x2D);
            renderDevice->sendVertex(Vector2( xx, -yy) * xS + x2D);
            renderDevice->sendVertex(Vector2( xx,  yy) * xS + x2D);
            renderDevice->sendVertex(Vector2(-xx, -yy) * xS + x2D);

            // Y
            renderDevice->setColor(Color3::GREEN);
            renderDevice->sendVertex(Vector2(-xx,  yy) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0, -yy) * yS + y2D);
            renderDevice->sendVertex(Vector2( xx,  yy) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0) * yS + y2D);
        renderDevice->endPrimitive();

        renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
            // Z
            renderDevice->setColor(Color3::BLUE);    
            renderDevice->sendVertex(Vector2( xx,  yy) * zS + z2D);
            renderDevice->sendVertex(Vector2(-xx,  yy) * zS + z2D);
            renderDevice->sendVertex(Vector2( xx, -yy) * zS + z2D);
            renderDevice->sendVertex(Vector2(-xx, -yy) * zS + z2D);
        renderDevice->endPrimitive();
    renderDevice->pop2D();
}


void Draw::ray(
    const Ray&          ray,
    RenderDevice*       renderDevice,
    const Color4&       color,
    double              scale) {

    Vector3 tip = ray.origin + ray.direction;
    // Create a coordinate frame at the tip
    Vector3 u = ray.direction.direction();
    Vector3 v;
    if (u.x == 0) {
        v = Vector3::UNIT_X;
    } else {
        v = Vector3::UNIT_Y;
    }
    Vector3 w = u.cross(v).direction();
    v = w.cross(u).direction();
    Vector3 back = tip - u * 0.3 * scale;

    lineSegment(LineSegment::fromTwoPoints(ray.origin, back), renderDevice, color);

    renderDevice->pushState();
        renderDevice->setColor(color);


        double r = scale * .1;
        // Arrow head
        renderDevice->beginPrimitive(RenderDevice::TRIANGLE_FAN);
            renderDevice->setNormal(u);
            renderDevice->sendVertex(tip);
            for (int a = 0; a <= 12; ++a) {
                double angle = a * G3D_PI / 6;
                Vector3 dir = cos(angle)  * v + sin(angle) * w;
                renderDevice->setNormal(dir);
                renderDevice->sendVertex(back + dir * r);
            }
        renderDevice->endPrimitive();

        // Back of arrow head
        renderDevice->beginPrimitive(RenderDevice::TRIANGLE_FAN);
            renderDevice->setNormal(-u);
            for (int a = 0; a <= 12; ++a) {
                double angle = a * G3D_PI / 6;
                Vector3 dir = sin(angle) * v + cos(angle) * w;
                renderDevice->sendVertex(back + dir * r);
            }
        renderDevice->endPrimitive();

    renderDevice->popState();
}


void Draw::capsule(
   const Capsule&       capsule, 
   RenderDevice*        renderDevice,
   const Color4&        color) {
    // TODO
}


void Draw::wireCapsule(
   const Capsule&       capsule, 
   RenderDevice*        renderDevice,
   const Color4&        color) {

    // TODO
    /*
    pushState();

        setDepthTest(RenderDevice::DEPTH_LEQUAL);
        setCullFace(RenderDevice::CULL_BACK);

        // Cylinder radius
        const double cylRadius = edgeScale * 0.010;

        CoordinateFrame cframe(capsule.getPoint1());

        Vector3 Y = (capsule.getPoint2() - capsule.getPoint1()).direction();
        Vector3 X = (abs(Y.dot(Vector3::UNIT_X)) > 0.9) ? Vector3::UNIT_Y : Vector3::UNIT_X;
        Vector3 Z = X.cross(Y).direction();
        X = Y.cross(Z);        
        cframe.rotation.setColumn(0, X);
        cframe.rotation.setColumn(1, Y);
        cframe.rotation.setColumn(2, Z);

        setObjectToWorldMatrix(getObjectToWorldMatrix() * cframe);
  
        double radius = capsule.getRadius();
        double height = (capsule.getPoint2() - capsule.getPoint1()).length();

        Sphere sphere1(Vector3::ZERO, radius);
        Sphere sphere2(Vector3(0, height, 0), radius);

        drawWireSphereSection(sphere1, cylRadius, color, false, true);
        drawWireSphereSection(sphere2, cylRadius, color, true, false);

        // Line around center
        setColor(color);
        Vector3 center(0, height / 2, 0);
        double pitch = 0;
        int y;
        for (y = 0; y < 26; ++y) {
            const double yaw0 = y * G3D_PI / 13;
            const double yaw1 = (y + 1) * G3D_PI / 13;

            Vector3 v0 = Vector3(cos(yaw0), sin(pitch), sin(yaw0)) * radius + center;
            Vector3 v1 = Vector3(cos(yaw1), sin(pitch), sin(yaw1)) * radius + center;

            drawCylinder(this, v0, v1, cylRadius, 6, false);
        }

        Vector3 top(0,height,0);
        // Edge lines
        for (y = 0; y < 8; ++y) {
            const double yaw = y * G3D_PI / 4;
            const Vector3 x(cos(yaw) * radius, 0, sin(yaw) * radius);
        
            drawCylinder(this, x, x + top, cylRadius, 6, false);        
        }

        drawFaceSphereSection(sphere1, color, false, true);
        drawFaceSphereSection(sphere2, color, true, false);

        // Cylinder faces
        setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        setColor(Color4(color, .12));
        setCullFace(RenderDevice::CULL_FRONT);
        disableDepthWrite();

        for (int i = 0; i < 2; ++i) {
            beginPrimitive(RenderDevice::QUAD_STRIP);
                for (y = 0; y <= 26; ++y) {
                    const double yaw0 = y * G3D_PI / 13;
                    Vector3 v0 = Vector3(cos(yaw0), sin(pitch), sin(yaw0)) * radius;

                    sendVertex(v0);
                    sendVertex(v0 + top);
                }
            endPrimitive();

            setCullFace(RenderDevice::CULL_BACK);
       }

    popState();
    */
}


void Draw::vertexNormals(
    const MeshAlg::Geometry&    geometry,
    RenderDevice*               renderDevice,
    const Color4&               color,
    double                      scale) {

    renderDevice->pushState();
        renderDevice->setColor(color);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        const Array<Vector3>& vertexArray = geometry.vertexArray;
        const Array<Vector3>& normalArray = geometry.normalArray;

        const double D = clamp(5.0 / pow(vertexArray.size(), .25), 0.1, .8) * scale;
        
        renderDevice->setLineWidth(1);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D);
                renderDevice->sendVertex(vertexArray[v]);
            }
        renderDevice->endPrimitive();
        
        renderDevice->setLineWidth(2);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .96);
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .84);
            }
        renderDevice->endPrimitive();

        renderDevice->setLineWidth(3);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .92);
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .84);
            }
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::line(
    const Line&         line,
    RenderDevice*       rd,
    const Color4&       color) {

    renderDevice->pushState();
        renderDevice->setColor(color);
        renderDevice->setLineWidth(2);
        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        Vector3 v0 = line.point();
        Vector3 d  = line.direction();
        renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
            // Off to infinity
            renderDevice->sendVertex(Vector4(-d, 0));

            for (int i = -10; i <= 10; i += 2) {
                renderDevice->sendVertex(v0 + i * d * 100);
            }

            // Off to infinity
            renderDevice->sendVertex(Vector4(d, 0));
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::lineSegment(
    const LineSegment&  lineSegment,
    RenderDevice*       rd,
    const Color4&       color) {

    renderDevice->pushState();
        renderDevice->setColor(color);
        
        // Compute perspective line width
        Vector3 v0 = lineSegment.endPoint(0);
        Vector3 v1 = lineSegment.endPoint(1);

        Vector3 s0 = renderDevice->project(v0);
        Vector3 s1 = renderDevice->project(v1);

        double L = 2;
        if ((s0.z > 0) && (s1.z > 0)) {
            L = 15 * (s0.z + s1.z) / 2;
        } else if (s0.z > 0) {
            L = max(15 * s0.z, 10);
        } else if (s1.z > 0) {
            L = max(15 * s1.z, 10);
        }

        renderDevice->setLineWidth(L);

        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        renderDevice->beginPrimitive(RenderDevice::LINES);
            renderDevice->sendVertex(v0);
            renderDevice->sendVertex(v1);
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::wireBox(
    const Box&              box,
    RenderDevice*           rd,
    const Color4&           color) {

    renderDevice->pushState();
        renderDevice->setColor(color);
        renderDevice->setLineWidth(2);
        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        renderDevice->setCullFace(RenderDevice::CULL_BACK);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        renderDevice->beginPrimitive(RenderDevice::LINES);

            // Wire frame
            for (int i = 0; i < 8; i += 4) {
                for (int j = 0; j < 4; ++j) {
                    renderDevice->sendVertex(box.getCorner(i + j));
                    renderDevice->sendVertex(box.getCorner(i + ((j + 1) % 4)));
                }
            }

            for (int i = 0; i < 4; ++i) {
                renderDevice->sendVertex(box.getCorner(i));
                renderDevice->sendVertex(box.getCorner(4 + i));
            }

        renderDevice->endPrimitive();

        // Faces
        renderDevice->disableDepthWrite();
        renderDevice->setCullFace(RenderDevice::CULL_NONE);

        Draw::box(box, renderDevice, Color4(color.r, color.g, color.b, min(color.a, 0.12)));

    renderDevice->popState();
}


void Draw::box(
    const Box&          box,
    RenderDevice*       renderDevice,
    const Color4&       color) {

    renderDevice->pushState();
        renderDevice->setColor(color);
        renderDevice->beginPrimitive(RenderDevice::QUADS);
        for (int i = 0; i < 6; ++i) {
            Vector3 v0, v1, v2, v3;
            box.getFaceCorners(i, v0, v1, v2, v3);

            Vector3 n = (v1 - v0).cross(v3 - v0);
            renderDevice->setNormal(n.direction());
            renderDevice->sendVertex(v0);
            renderDevice->sendVertex(v1);
            renderDevice->sendVertex(v2);
            renderDevice->sendVertex(v3);
        }
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::wireSphereSection(
    const Sphere&       sphere,
    RenderDevice*       renderDevice,
    const Color4&       color,
    bool                top,
    bool                bottom) {
    
    int sections = 32;
    int start = top ? 0 : (sections / 2);
    int stop = bottom ? sections : (sections / 2);

    renderDevice->pushState();
        renderDevice->setColor(color);
        renderDevice->setLineWidth(2);
        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        renderDevice->setCullFace(RenderDevice::CULL_BACK);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        double radius = sphere.radius;
        const Vector3& center = sphere.center;

        // Wire frame
        for (int y = 0; y < 8; ++y) {
            const double yaw = y * G3D_PI / 4;
            const Vector3 x(cos(yaw) * radius, 0, sin(yaw) * radius);
            //const Vector3 z(-sin(yaw) * radius, 0, cos(yaw) * radius);

            renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
                for (int p = start; p <= stop; ++p) {
                    const double pitch0 = p * G3D_PI / (sections * 0.5);

                    Vector3 v0 = cos(pitch0) * x + Vector3::UNIT_Y * radius * sin(pitch0) + center;
                    renderDevice->sendVertex(v0);
                }
            renderDevice->endPrimitive();
        }


        int a = bottom ? -1 : 0;
        int b = top ? 1 : 0; 
        for (int p = a; p <= b; ++p) {
            const double pitch = p * G3D_PI / 6;

            renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
                for (int y = 0; y <= sections; ++y) {
                    const double yaw0 = y * G3D_PI / 13;
                    Vector3 v0 = Vector3(cos(yaw0) * cos(pitch), sin(pitch), sin(yaw0) * cos(pitch)) * radius + center;
                    renderDevice->sendVertex(v0);
                }
            renderDevice->endPrimitive();
        }

    renderDevice->popState();
}


void Draw::sphereSection(
    const Sphere&       sphere,
    RenderDevice*       renderDevice,
    const Color4&       color,
    bool                top,
    bool                bottom) {

    int sections = 26;
    int start = top ? 0 : (sections / 2);
    int stop = bottom ? sections : (sections / 2);

    renderDevice->pushState();
        renderDevice->setColor(color);

        for (int p = start; p < stop; ++p) {
            const double pitch0 = p * G3D_PI / (double)sections;
            const double pitch1 = (p + 1) * G3D_PI / (double)sections;

            renderDevice->beginPrimitive(RenderDevice::QUAD_STRIP);
            for (int y = 0; y <= 26; ++y) {
                const double yaw = -y * G3D_PI / 13;

                Vector3 v0 = Vector3(cos(yaw) * sin(pitch0), cos(pitch0), sin(yaw) * sin(pitch0));
                Vector3 v1 = Vector3(cos(yaw) * sin(pitch1), cos(pitch1), sin(yaw) * sin(pitch1));

                renderDevice->setNormal(v0);
                renderDevice->sendVertex(v0 * sphere.radius + sphere.center);

                renderDevice->setNormal(v1);
                renderDevice->sendVertex(v1 * sphere.radius + sphere.center);
            }
            renderDevice->endPrimitive();
        }

    renderDevice->popState();       
}


void Draw::wireSphere(
    const Sphere&       sphere,
    RenderDevice*       rd,
    const Color4&       color) {

    wireSphereSection(sphere, rd, color, true, true);

    renderDevice->pushState();
        renderDevice->disableDepthWrite();
        renderDevice->setCullFace(RenderDevice::CULL_NONE);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        sphereSection(sphere, rd, Color4(color.r, color.g, color.b, min(color.a, 0.12)), true, true);
    renderDevice->popState();
}


void Draw::sphere(
    const Sphere&       sphere,
    RenderDevice*       rd,
    const Color4&       color) {

    sphereSection(sphere, rd, color, true, true);
}

void doGraphics() {
    renderDevice->beginFrame();
        renderDevice->clear(true, true, true);
        renderDevice->pushState();
			    
		    camera->setProjectionAndCameraMatrix();

            renderDevice->debugDrawAxes(2);

            Draw::axes(CoordinateFrame(Vector3(0,4,0)), renderDevice);

            Draw::wireBox(Box(Vector3(-1,-1,-1), Vector3(1,1,1)), renderDevice);

            Draw::wireSphere(Sphere(Vector3(2,2,2), 1), renderDevice);

            Draw::arrow(Vector3(-2, 2, 0), Vector3(1,1,1), renderDevice);

        renderDevice->popState();
	    
    renderDevice->endFrame();
}


void doUserInput() {

    userInput->beginEvents();

    // Event handling
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
	    endProgram = true;
	    break;

	    case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                endProgram = true;
                break;

            // Add other key handlers here
            default:;
            }
            break;

        // Add other event handlers here

        default:;
        }

        userInput->processEvent(event);
    }

    userInput->endEvents();
}
