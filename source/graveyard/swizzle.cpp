
//
// Creates:
//  header#.h: prototype methods for all swizzles on Vector#
//  cpp#.cpp:  implementation of all swizzles on Vector#
//  begin.h:   #defines for all swizzles
//  end.h:     #undefs for all swizzles
int main(int argc, char* argv[]) {

    const char XYZW[] = {'x', 'y', 'z', 'w'}; 

    for (int v = 2; v <= 4; ++v) {
        char filename[1024];

        sprintf(filename, "c:/tmp/header%d.h", v);
        FILE* header = fopen(filename, "w");

        sprintf(filename, "c:/tmp/cpp%d.cpp", v);
        FILE* cpp = fopen(filename, "w");

        for (int num = 2; num <= 4; ++num) {
            fprintf(header, "    // %d-char swizzles\n\n", num);
            fprintf(cpp, "// %d-char swizzles\n\n", num);
            for (int i = 0; i < pow(v, num); ++i) {

                char pos[4];
                for (int j = 0; j < num; ++j) {
                    pos[j] = XYZW[(i / (int)pow(v, j)) % v];
                }

                // Header (const)
                fprintf(header, "    Vector%d ", num);
                for (int j = 0; j < num; ++j) {
                    fprintf(header, "%c", pos[j]);
                }
                fprintf(header, "() const;\n");

                /*
                // Header (swizzle)
                fprintf(header, "    VectorSwizzle%d ", num);
                for (int j = 0; j < num; ++j) {
                    fprintf(header, "%c", pos[j]);
                }
                fprintf(header, "();\n");
                */

                // Implementation (const)
                fprintf(cpp, "Vector%d Vector%d::", num, v);
                for (int j = 0; j < num; ++j) {
                    fprintf(cpp, "%c", pos[j]);
                }
                fprintf(cpp, "() const  { return Vector%d       (", num);
                for (int j = 0; j < num; ++j) {
                    fprintf(cpp, "%c", pos[j]);
                    if (j < num - 1) {
                        fprintf(cpp, ", ");
                    }
                }
                fprintf(cpp, "); }\n");

                /*
                // Implementation (swizzle)
                fprintf(cpp, "VectorSwizzle%d Vector%d::", num, v);
                for (int j = 0; j < num; ++j) {
                    fprintf(cpp, "%c", pos[j]);
                }
                fprintf(cpp, "() { return VectorSwizzle%d(", num);
                for (int j = 0; j < num; ++j) {
                    fprintf(cpp, "%c", pos[j]);
                    if (j < num - 1) {
                        fprintf(cpp, ", ");
                    }
                }
                fprintf(cpp, "); }\n");
                */

            }
            fprintf(header, "\n");
            fprintf(cpp, "\n");
        }

        fclose(header);
        fclose(cpp);
    }

    FILE* begin = fopen("c:/tmp/begin.h", "w");
    FILE* end = fopen("c:/tmp/end.h", "w");

    for (int num = 2; num <= 4; ++num) {
        for (int i = 0; i < pow(4, num); ++i) {
            char pos[5];
            pos[0] = '\0';
            pos[1] = '\0';
            pos[2] = '\0';
            pos[3] = '\0';
            pos[4] = '\0';
            for (int j = 0; j < num; ++j) {
                pos[j] = XYZW[(i / (int)pow(4, j)) % 4];
            }

            fprintf(begin, "#define %s %s()\n", pos, pos);
            fprintf(end, "#undef %s\n", pos);
        }
    }

    fclose(begin);
    fclose(end);

	return 0;
}
