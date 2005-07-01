#if 0
#include "../include/G3DAll.h"
#include "../contrib/GChunk/GChunk.h"


void testGChunk() {
    printf("GChunk ");

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
                alwaysAssertM(b.readInt32() == 10, "");
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

    printf("passed\n");
}

#endif
