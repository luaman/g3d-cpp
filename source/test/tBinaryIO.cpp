#include "../include/G3DAll.h"

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

static void testBitSerialization() {
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


static void testCompression() {
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


static void measureSerializerPerformance() {
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
    printf("BinaryOutput time with BinaryOutput::reset: %gs\n\n", resetTime);
    
}


void perfBinaryIO() {
    measureSerializerPerformance();
}

void testBinaryIO() {
    testBitSerialization();
    testCompression();
}