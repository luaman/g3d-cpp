#include <G3DAll.h>

// All return FPS
float measureBeginEndPerformance(class Model&);
float measureDrawElementsRAMPerformance(class Model&);
float measureDrawElementsVBOPerformance(class Model&);


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

        PosedModelRef m = IFSModel::create(filename)->pose();
        tex = Texture::fromFile("gears.jpg");
        textureID = tex->getOpenGLID();

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
    }
};

static GWindow* window = NULL;
void glSwapBuffers() {
    window->swapGLBuffers();
}

void measurePerformance(GWindow* w) {

    float beginEndFPS;

    window = w;
    Model model("bunny.ifs");

    beginEndFPS = measureBeginEndPerformance(model);
    Log::common()->printf("glBegin/glEndFPS: %g\n", beginEndFPS);
}


float measureBeginEndPerformance(Model& model) {

    int N = model.cpuIndex.size();
    const int*   index   = model.cpuIndex.begin();
    const float* vertex  = reinterpret_cast<const float*>(model.cpuVertex.begin());
    const float* normal  = reinterpret_cast<const float*>(model.cpuNormal.begin());
    const float* color   = reinterpret_cast<const float*>(model.cpuColor.begin());
    const float* texCoord= reinterpret_cast<const float*>(model.cpuTexCoord.begin());

    glPushAttrib(GL_ALL_ATTRIB_BITS);

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
    
    // number of models per frame
    int count = 3;

    float k = 0;

    // Number of frames to render
    int frames = 100;

    double t0 = System::time();
    for (int j = 0; j < frames; ++j) {
        k += 1;
        glClearColor(1.0f, 1.0f, 1.0f, 0.04f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, model.textureID);

        for (int c = 0; c < count; ++c) {
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glTranslatef(c - (count - 1) / 2.0, 0, -2);
            glRotatef(k * ((c & 1) * 2 - 1), 0, 1, 0);

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

