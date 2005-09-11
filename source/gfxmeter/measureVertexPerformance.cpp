/**
  @file measurePerformance.cpp
  
  Measures the performance of rendering in various modes on this machine.

 */
#include <G3DAll.h>

// All return FPS
float measureBeginEndPerformance(class Model&);
float measureDrawElementsRAMPerformance(class Model&);
float measureDrawElementsVBOPerformance(class Model&);
float measureDrawElementsVBO16Performance(class Model&);
float measureDrawElementsVBOIPerformance(class Model&);
float measureDrawElementsVBOPeakPerformance(class Model&);

/** Number of frames to render in tests */
static const int frames = 16;


/** Number of models per frame */
int count = 4;

class Model {
private:

    TextureRef          tex;

public:

    class Vec2 {
    public:
        float x, y;
    };

    class Vec3 {
    public:
        float x, y, z;
    };

    std::vector<int>    cpuIndex;
    std::vector<Vec3>   cpuVertex;
    std::vector<Vec2>   cpuTexCoord;
    std::vector<Vec3>   cpuColor;
    std::vector<Vec3>   cpuNormal;

    GLuint              textureID;


    /** Only the constructor uses G3D */
    Model(const std::string& filename) {

        tex = Texture::fromFile("tiny.jpg");
        textureID = tex->getOpenGLID();

        // The gear shape produces better memory coherence so 
        // it renders faster.
        enum {GEAR, BUNNY} shape = GEAR;

        if (shape == BUNNY) {
            PosedModelRef m = IFSModel::create(filename)->pose();
            // Copy fields out into arrays
            {
                const Array<int>& index = m->triangleIndices();
                int N = index.size();
                cpuIndex.resize(N);
                System::memcpy(cpuIndex.begin(), index.getCArray(), sizeof(int) * N);
            }

            {
                const MeshAlg::Geometry& g = m->objectSpaceGeometry(); 
                int N = g.vertexArray.size();
                cpuVertex.resize(N);
                cpuNormal.resize(N);
                cpuColor.resize(N);
                cpuTexCoord.resize(N);
                System::memcpy(cpuVertex.begin(), g.vertexArray.getCArray(), N * sizeof(Vec3));
                System::memcpy(cpuNormal.begin(), g.normalArray.getCArray(), N * sizeof(Vec3));

                for (int i = 0; i < N; ++i) {
                    // Copy the normals over the colors, too
                    cpuColor[i].x = g.normalArray[i].x * 0.5 + 0.5;
                    cpuColor[i].y = g.normalArray[i].y * 0.5 + 0.5;
                    cpuColor[i].z = g.normalArray[i].z * 0.5 + 0.5;

                    // Cylindrical projection to get tex coords
                    Vector3 dir = g.vertexArray[i].direction();
                    cpuTexCoord[i].x = (atan2(dir.x, dir.z) / G3D_TWO_PI + 0.5) * 5;
                    cpuTexCoord[i].y = (0.5 - dir.y * 0.5) * 5;
                }
            }
        } else {
            // Vertices per side
            const int sq = 187;

            // Number of indices
            const int N = (sq - 1) * (sq - 1) * 3 * 2;

            // Number of vertices
            const int V = sq * sq;

            // Make a grid of triangles
            cpuIndex.resize(N);
            {
                int k = 0;
                for (int i = 0; i < sq - 1; ++i) {
                    for (int j = 0; j < sq - 1; ++j) {
                        debugAssert(k < N - 5);

                        // Bottom triangle
                        cpuIndex[k + 0] = i + j * sq;
                        cpuIndex[k + 1] = i + (j + 1) * sq;
                        cpuIndex[k + 2] = (i + 1) + (j + 1) * sq;

                        // Top triangle
                        cpuIndex[k + 3] = i + j * sq;
                        cpuIndex[k + 4] = (i + 1) + (j + 1) * sq;
                        cpuIndex[k + 5] = (i + 1) + j * sq;

                        k += 6;
                    }
                }
            }

            // Create data
            cpuVertex.resize(V);
            cpuNormal.resize(V);
            cpuTexCoord.resize(V);
            cpuColor.resize(V);

            // Map V indices to a sq x sq grid
            for (int i = 0; i < sq; ++i) {
                for (int j = 0; j < sq; ++j) {

                    int v = (i + j * sq);

                    float x = (i / (float)sq - 0.5) * 2;
                    float y = 0.5 - j / (float)sq;
                    float a = x * 2 * 3.1415927;
                    float r = ceil(cos(a * 10)) * 0.05 + 0.3;

                    cpuVertex[v].x = -cos(a) * r;
                    cpuVertex[v].y = y;
                    cpuVertex[v].z = sin(a) * r;

                    // Scale the normal
                    float s = 1.0 / sqrt(0.0001 + square(cpuVertex[v].x) + square(cpuVertex[v].y) + square(cpuVertex[v].z));
                    cpuNormal[v].y = cpuVertex[v].x * s;
                    cpuNormal[v].x = cpuVertex[v].y * s;
                    cpuNormal[v].z = cpuVertex[v].z * s;

                    cpuColor[v].x = r + 0.7;
                    cpuColor[v].y = 0.5;
                    cpuColor[v].z = 1.0 - r;

                    cpuTexCoord[v].x = i / (float)sq;
                    cpuTexCoord[v].y = j / (float)sq;
                }
            }
        }
    }
};

static GWindow* window = NULL;
void glSwapBuffers() {
    window->swapGLBuffers();
}

void measureVertexPerformance(
    GWindow* w,     
    int&   numTris,
    float& beginEndFPS,
    float& drawElementsRAMFPS, 
    float& drawElementsVBOFPS, 
    float& drawElementsVBO16FPS, 
    float& drawElementsVBOIFPS,
    float& drawElementsVBOPeakFPS) {

    window = w;
    Model model("bunny.ifs");

    beginEndFPS = measureBeginEndPerformance(model);
    drawElementsRAMFPS = measureDrawElementsRAMPerformance(model);
    drawElementsVBOFPS = measureDrawElementsVBOPerformance(model);
    drawElementsVBO16FPS = measureDrawElementsVBO16Performance(model);
    drawElementsVBOIFPS = measureDrawElementsVBOIPerformance(model);
    drawElementsVBOPeakFPS = measureDrawElementsVBOPeakPerformance(model);

    numTris = count * model.cpuIndex.size() / 3;
}


static void configureCameraAndLights() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float w = 0.8, h = 0.6;
    glFrustum(-w/2, w/2, -h/2, h/2, 0.5, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glShadeModel(GL_SMOOTH);

    glCullFace(GL_BACK);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    {
        float pos[4] = {-1.0, 1.0, 1.0, 0.0};
        glLightfv(GL_LIGHT0, GL_POSITION, pos);

        float col[4] = {1.0, 1.0, 1.0, 1.0};
        glLightfv(GL_LIGHT0, GL_DIFFUSE, col);

    }

    glEnable(GL_LIGHT1);
    {
        float pos[4] = {1.0, -1.0, 1.0, 0.0};
        glLightfv(GL_LIGHT1, GL_POSITION, pos);

        float col[4] = {0.4, 0.1, 0.1, 1.0};
        glLightfv(GL_LIGHT1, GL_DIFFUSE, col);
    }

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    {
        float amb[4] = {0.5, 0.5, 0.5, 1.0};
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
    }

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}


float measureBeginEndPerformance(Model& model) {

    int N = model.cpuIndex.size();
    const int*   index   = model.cpuIndex.begin();
    const float* vertex  = reinterpret_cast<const float*>(model.cpuVertex.begin());
    const float* normal  = reinterpret_cast<const float*>(model.cpuNormal.begin());
    const float* color   = reinterpret_cast<const float*>(model.cpuColor.begin());
    const float* texCoord= reinterpret_cast<const float*>(model.cpuTexCoord.begin());

    glPushAttrib(GL_ALL_ATTRIB_BITS);


    configureCameraAndLights();
    

    float k = 0;

    double t0 = System::time();
    for (int j = 0; j < frames; ++j) {
        k += 3;
        glClearColor(1.0f, 1.0f, 1.0f, 0.04f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, model.textureID);

        for (int c = 0; c < count; ++c) {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslatef(c - (count - 1) / 2.0, 0, -2);
            glRotatef(k * ((c & 1) * 2 - 1) + 90, 0, 1, 0);

            glBegin(GL_TRIANGLES);
            for (int i = 0; i < N; ++i) {
                const int v = index[i];
                const int v3 = 3 * v;
                glNormal3fv(normal + v3);
                glColor3fv(color + v3);
                glTexCoord2fv(texCoord + v + v);
                glVertex3fv(vertex + v3);
            }
            glEnd();
        }

        glSwapBuffers();

    }

    glPopAttrib();

    return frames / (System::time() - t0);
}


float measureDrawElementsRAMPerformance(Model& model) {

    // Number of indices
    const int N = model.cpuIndex.size();
    // Number of vertices
    const int V = model.cpuVertex.size();
    const int*   index   = model.cpuIndex.begin();
    const float* vertex  = reinterpret_cast<const float*>(model.cpuVertex.begin());
    const float* normal  = reinterpret_cast<const float*>(model.cpuNormal.begin());
    const float* color   = reinterpret_cast<const float*>(model.cpuColor.begin());
    const float* texCoord= reinterpret_cast<const float*>(model.cpuTexCoord.begin());

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

    configureCameraAndLights();
    
    float k = 0;

    double t0 = System::time();
    for (int j = 0; j < frames; ++j) {
        k += 3;
        glClearColor(1.0f, 1.0f, 1.0f, 0.04f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, model.textureID);

        for (int c = 0; c < count; ++c) {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslatef(c - (count - 1) / 2.0, 0, -2);
            glRotatef(k * ((c & 1) * 2 - 1) + 90, 0, 1, 0);

            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);

            glNormalPointer(GL_FLOAT, 0, normal);
            glColorPointer(3, GL_FLOAT, 0, color);
            glTexCoordPointer(2, GL_FLOAT, 0, texCoord);
            glVertexPointer(3, GL_FLOAT, 0,vertex);

            glDrawElements(GL_TRIANGLES, N, GL_UNSIGNED_INT, index);
        }

        glSwapBuffers();

    }

    glPopClientAttrib();
    glPopAttrib();
    glFinish();

    return frames / (System::time() - t0);
}


float measureDrawElementsVBOPerformance(Model& model) {
    
    bool hasVBO = 
        (strstr((char*)glGetString(GL_EXTENSIONS), "GL_ARB_vertex_buffer_object") != NULL) &&
            (glGenBuffersARB != NULL) && 
            (glBufferDataARB != NULL) &&
            (glDeleteBuffersARB != NULL);

    if (! hasVBO) {
        return 0.0;
    }

    // Load the vertex arrays

    // Number of indices
    const int N = model.cpuIndex.size();
    // Number of vertices
    const int V = model.cpuVertex.size();
    const int*   index   = model.cpuIndex.begin();
    const float* vertex  = reinterpret_cast<const float*>(model.cpuVertex.begin());
    const float* normal  = reinterpret_cast<const float*>(model.cpuNormal.begin());
    const float* color   = reinterpret_cast<const float*>(model.cpuColor.begin());
    const float* texCoord= reinterpret_cast<const float*>(model.cpuTexCoord.begin());

    GLuint vbo, indexBuffer;
    glGenBuffersARB(1, &vbo);
    glGenBuffersARB(1, &indexBuffer);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

    size_t vertexSize   = V * sizeof(float) * 3;
    size_t normalSize   = V * sizeof(float) * 3;
    size_t texCoordSize = V * sizeof(float) * 2;
    size_t colorSize    = V * sizeof(float) * 3;   
    size_t totalSize    = vertexSize + normalSize + texCoordSize + colorSize;

    size_t indexSize    = N * sizeof(int);

    // Pointers relative to the start of the vbo in video memory
    // (would interleaving be faster?)
    GLintptrARB vertexPtr   = 0;
    GLintptrARB normalPtr   = vertexSize + vertexPtr;
    GLintptrARB colorPtr    = normalSize + normalPtr;
    GLintptrARB texCoordPtr = colorSize  + colorPtr;

    GLintptrARB indexPtr    = 0;

    // Upload data
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexSize, index, GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, totalSize, NULL, GL_STATIC_DRAW_ARB);

    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, vertexPtr,   vertexSize, vertex);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, colorPtr,    colorSize, color);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, normalPtr,   normalSize, normal);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, texCoordPtr, texCoordSize, texCoord);

    configureCameraAndLights();
    
    float k = 0;

    double t0 = System::time();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, model.textureID);

        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glNormalPointer(GL_FLOAT, 0, (void*)normalPtr);
        glColorPointer(3, GL_FLOAT, 0, (void*)colorPtr);
        glTexCoordPointer(2, GL_FLOAT, 0, (void*)texCoordPtr);
        glVertexPointer(3, GL_FLOAT, 0, (void*)vertexPtr);
    for (int j = 0; j < frames; ++j) {
        k += 3;
        glClearColor(1.0f, 1.0f, 1.0f, 0.04f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int c = 0; c < count; ++c) {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslatef(c - (count - 1) / 2.0, 0, -2);
            glRotatef(k * ((c & 1) * 2 - 1) + 90, 0, 1, 0);

            glDrawElements(GL_TRIANGLES, N, GL_UNSIGNED_INT, (void*)indexPtr);
        }

        glSwapBuffers();

    }


    glPopClientAttrib();
    glPopAttrib();

    glDeleteBuffersARB(1, &indexBuffer);
    glDeleteBuffersARB(1, &vbo);
    glFinish();

    return frames / (System::time() - t0);
}



float measureDrawElementsVBO16Performance(Model& model) {
    
    bool hasVBO = 
        (strstr((char*)glGetString(GL_EXTENSIONS), "GL_ARB_vertex_buffer_object") != NULL) &&
            (glGenBuffersARB != NULL) && 
            (glBufferDataARB != NULL) &&
            (glDeleteBuffersARB != NULL);

    if (! hasVBO) {
        return 0.0;
    }

    // Load the vertex arrays

    // Number of indices
    const int N = model.cpuIndex.size();
    // Number of vertices
    const int V = model.cpuVertex.size();
    const int*   index   = model.cpuIndex.begin();
    const float* vertex  = reinterpret_cast<const float*>(model.cpuVertex.begin());
    const float* normal  = reinterpret_cast<const float*>(model.cpuNormal.begin());
    const float* color   = reinterpret_cast<const float*>(model.cpuColor.begin());
    const float* texCoord= reinterpret_cast<const float*>(model.cpuTexCoord.begin());

    std::vector<unsigned short> index16(N);
    for (int i = 0; i < N; ++i) {
        index16[i] = (uint16)index[i];
    }
    
    GLuint vbo, indexBuffer;
    glGenBuffersARB(1, &vbo);
    glGenBuffersARB(1, &indexBuffer);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

    size_t vertexSize   = V * sizeof(float) * 3;
    size_t normalSize   = V * sizeof(float) * 3;
    size_t texCoordSize = V * sizeof(float) * 2;
    size_t colorSize    = V * sizeof(float) * 3;   
    size_t totalSize    = vertexSize + normalSize + texCoordSize + colorSize;

    size_t indexSize    = N * sizeof(unsigned short);

    // Pointers relative to the start of the vbo in video memory
    // (would interleaving be faster?)
    GLintptrARB vertexPtr   = 0;
    GLintptrARB normalPtr   = vertexSize + vertexPtr;
    GLintptrARB colorPtr    = normalSize + normalPtr;
    GLintptrARB texCoordPtr = colorSize  + colorPtr;

    GLintptrARB indexPtr    = 0;

    // Upload data
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexSize, index16.begin(), GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, totalSize, NULL, GL_STATIC_DRAW_ARB);

    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, vertexPtr,  vertexSize, vertex);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, colorPtr,       colorSize, color);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, normalPtr,      normalSize, normal);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, texCoordPtr, texCoordSize, texCoord);

    configureCameraAndLights();
    
    float k = 0;

    double t0 = System::time();
    for (int j = 0; j < frames; ++j) {
        k += 3;
        glClearColor(1.0f, 1.0f, 1.0f, 0.04f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, model.textureID);

        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glNormalPointer(GL_FLOAT, 0, (void*)normalPtr);
        glColorPointer(3, GL_FLOAT, 0, (void*)colorPtr);
        glTexCoordPointer(2, GL_FLOAT, 0, (void*)texCoordPtr);
        glVertexPointer(3, GL_FLOAT, 0, (void*)vertexPtr);

        for (int c = 0; c < count; ++c) {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslatef(c - (count - 1) / 2.0, 0, -2);
            glRotatef(k * ((c & 1) * 2 - 1) + 90, 0, 1, 0);

            glDrawElements(GL_TRIANGLES, N, GL_UNSIGNED_SHORT, (void*)indexPtr);
        }

        glSwapBuffers();

    }


    glPopClientAttrib();
    glPopAttrib();

    glDeleteBuffersARB(1, &indexBuffer);
    glDeleteBuffersARB(1, &vbo);
    glFinish();

    return frames / (System::time() - t0);
}



float measureDrawElementsVBOIPerformance(Model& model) {
    
    bool hasVBO = 
        (strstr((char*)glGetString(GL_EXTENSIONS), "GL_ARB_vertex_buffer_object") != NULL) &&
            (glGenBuffersARB != NULL) && 
            (glBufferDataARB != NULL) &&
            (glDeleteBuffersARB != NULL);

    if (! hasVBO) {
        return 0.0;
    }

    // Load the vertex arrays

    // Number of indices
    const int N = model.cpuIndex.size();
    // Number of vertices
    const int V = model.cpuVertex.size();
    const int*   index   = model.cpuIndex.begin();
    const float* vertex  = reinterpret_cast<const float*>(model.cpuVertex.begin());
    const float* normal  = reinterpret_cast<const float*>(model.cpuNormal.begin());
    const float* color   = reinterpret_cast<const float*>(model.cpuColor.begin());
    const float* texCoord= reinterpret_cast<const float*>(model.cpuTexCoord.begin());

    GLuint vbo, indexBuffer;
    glGenBuffersARB(1, &vbo);
    glGenBuffersARB(1, &indexBuffer);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

    size_t vertexSize   = V * sizeof(float) * 3;
    size_t normalSize   = V * sizeof(float) * 3;
    size_t colorSize    = V * sizeof(float) * 4;   
    size_t texCoordSize = V * sizeof(float) * 2;
    size_t totalSize    = vertexSize + normalSize + texCoordSize + colorSize;

    size_t indexSize    = N * sizeof(int);

    // Pointers relative to the start of the vbo in video memory
    // (would interleaving be faster?)
    GLintptrARB texCoordPtr = 0;
    GLintptrARB colorPtr    = texCoordPtr + 2 * sizeof(float);
    GLintptrARB normalPtr   = colorPtr  + 4 * sizeof(float);
    GLintptrARB vertexPtr   = normalPtr + 3 * sizeof(float);

    GLintptrARB indexPtr    = 0;

    // Upload data
    std::vector<unsigned short> index16(N);
    for (int i = 0; i < N; ++i) {
        index16[i] = (uint16)index[i];
    }
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, N * sizeof(unsigned short), index16.begin(), GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
    float* interleave = (float*)malloc(totalSize);
    // Interleave the buffers in memory
    float* ptr = interleave;
    for (int i = 0; i < V; ++i) {
        for (int j = 0; j < 2; ++j) {
            ptr[0] = texCoord[i * 2 + j];
            ++ptr;
        }

        for (int j = 0; j < 3; ++j) {
            ptr[0] = color[i * 3 + j];
            ++ptr;
        }
        ptr[0] = 1.0f;  // alpha
        ++ptr;

        for (int j = 0; j < 3; ++j) {
            ptr[0] = normal[i * 3 + j];
            ++ptr;
        }

        for (int j = 0; j < 3; ++j) {
            ptr[0] = vertex[i * 3 + j];
            ++ptr;
        }
    }
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, totalSize, interleave, GL_STATIC_DRAW_ARB);
    free(interleave);

    configureCameraAndLights();
    
    float k = 0;

    double t0 = System::time();
    for (int j = 0; j < frames; ++j) {
        k += 3;
        glClearColor(1.0f, 1.0f, 1.0f, 0.04f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, model.textureID);

        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glInterleavedArrays(GL_T2F_C4F_N3F_V3F, 0, (void*)0);

        for (int c = 0; c < count; ++c) {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslatef(c - (count - 1) / 2.0, 0, -2);
            glRotatef(k * ((c & 1) * 2 - 1) + 90, 0, 1, 0);

            glDrawElements(GL_TRIANGLES, N, GL_UNSIGNED_SHORT, (void*)indexPtr);
        }

        glSwapBuffers();

    }


    glPopClientAttrib();
    glPopAttrib();

    glDeleteBuffersARB(1, &indexBuffer);
    glDeleteBuffersARB(1, &vbo);
    glFinish();

    return frames / (System::time() - t0);
}


float measureDrawElementsVBOPeakPerformance(Model& model) {
    
    bool hasVBO = 
        (strstr((char*)glGetString(GL_EXTENSIONS), "GL_ARB_vertex_buffer_object") != NULL) &&
            (glGenBuffersARB != NULL) && 
            (glBufferDataARB != NULL) &&
            (glDeleteBuffersARB != NULL);

    if (! hasVBO) {
        return 0.0;
    }

    // Load the vertex arrays

    // Number of indices
    const int N = model.cpuIndex.size();
    // Number of vertices
    const int V = model.cpuVertex.size();
    const int*   index   = model.cpuIndex.begin();
    const float* vertex  = reinterpret_cast<const float*>(model.cpuVertex.begin());

    std::vector<unsigned short> index16(N);
    for (int i = 0; i < N; ++i) {
        index16[i] = (uint16)index[i];
    }
    
    GLuint vbo, indexBuffer;
    glGenBuffersARB(1, &vbo);
    glGenBuffersARB(1, &indexBuffer);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_CLIENT_ATTRIB_BITS);

    size_t vertexSize   = V * sizeof(float) * 3;
    size_t totalSize    = vertexSize;
    size_t indexSize    = N * sizeof(unsigned short);

    // Pointers relative to the start of the vbo in video memory
    // (would interleaving be faster?)
    GLintptrARB vertexPtr   = 0;

    GLintptrARB indexPtr    = 0;

    // Upload data
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBuffer);
    glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexSize, index16.begin(), GL_STATIC_DRAW_ARB);

    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, vertexSize, vertex, GL_STATIC_DRAW_ARB);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float w = 0.8, h = 0.6;
    glFrustum(-w/2, w/2, -h/2, h/2, 0.5, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glShadeModel(GL_SMOOTH);
    glDisable(GL_NORMALIZE);
    glDisable(GL_COLOR_MATERIAL);

    glCullFace(GL_BACK);
    
    float k = 0;

    double t0 = System::time();
    for (int j = 0; j < frames; ++j) {
        k += 3;
        glClearColor(1.0f, 1.0f, 1.0f, 0.04f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnableClientState(GL_VERTEX_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, (void*)vertexPtr);

        for (int c = 0; c < count; ++c) {
            static const float col[] = {1, 0, 0, 1, 0, 0};
            glColor3fv(col + (c % 3));

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslatef(c - (count - 1) / 2.0, 0, -2);
            glRotatef(k * ((c & 1) * 2 - 1) + 90, 0, 1, 0);

            glDrawElements(GL_TRIANGLES, N, GL_UNSIGNED_SHORT, (void*)indexPtr);
        }

        glSwapBuffers();
    }


    glPopClientAttrib();
    glPopAttrib();

    glDeleteBuffersARB(1, &indexBuffer);
    glDeleteBuffersARB(1, &vbo);
    glFinish();

    return frames / (System::time() - t0);
}
