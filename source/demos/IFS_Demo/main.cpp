/**
 @file IFS_Demo/main.cpp

 Loads and displays an IFS file supplied on the command line.
 Press "W" to see wireframe, ESC to exit.

 This demo shows how to use OpenGL and SDL with G3D <B>without</B>
 using the GLG3D library (and classes like G3D::RenderDevice).
 The GLG3D demo shows how to use G3D::RenderDevice.

 You need SDL.DLL in the same directory as the executable.

 Requires:
   OpenGL
   SDL
   G3D
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 @cite       Written by Nate Miller, nathanm@uci.edu
 @created    2002-08-10
 @edited     2003-04-03
 */

#include "IFSReader.h"
#include "glheaders.h"

bool wire = 0;

IFSModel mesh;
const float meshScale = 100.0f;

Vector3 eyePos(0.0f, 0.0f, 100.0f);
Vector3 rot(0.0f, 0.0f, 0.0f);
const float spinRate = 45.0f;

void draw(const float& dtime) {
    rot[1] += (spinRate * dtime);

    if (rot[1] > 360.0f) {
        rot[1] -= 360.0f;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(-eyePos[0], -eyePos[1], -eyePos[2]);
    glRotatef(rot[1], 0.0f, 1.0f, 0.0f);

    const IFSVertex*     v   = mesh.getVerts();
    const IFSTriangle*   f   = mesh.getTriangles();
    uint32               num = mesh.getNumTriangles();

    glColor(Color3::WHITE);

    for (int i = 0; i < num; ++i) {
        const uint32* ndx = f[i].getIndices();

        glBegin(GL_TRIANGLES);
            glNormal(v[ndx[0]].getNormal());
            glVertex(v[ndx[0]].getPosition());
            glNormal(v[ndx[1]].getNormal());
            glVertex(v[ndx[1]].getPosition());
            glNormal(v[ndx[2]].getNormal());
            glVertex(v[ndx[2]].getPosition());
        glEnd();
    }

    SDL_GL_SwapBuffers();
}


void setup() {
    SDL_Surface* surf = SDL_GetVideoSurface();
    
    if (!surf) {
        return;
    }
    
    int w = surf->w;
    int h = surf->h;
    
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(90, (double) w / h, 1, 4096);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat ambPosition[]  = {100.0f, 100.0f, 100.0f, 1.0f};
    GLfloat ambColor[]     = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat surfShiny      = 64.0f;
    GLfloat surfSpecular[] = {0.75f, 0.75f, 0.75f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambColor);
    glLightfv(GL_LIGHT0, GL_POSITION, ambPosition);
    glMaterialfv(GL_FRONT, GL_SHININESS, &surfShiny);
    glMaterialfv(GL_FRONT, GL_SPECULAR, surfSpecular);
    
    if (wire) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}


void dumpInfo(void) {
    printf("w          - Toggle wireframe\n\n");
    printf("Mesh Name  - %s\n", mesh.getMeshName().c_str());
    printf("Vertices   - %d\n", mesh.getNumVerts());
    printf("Triangles  - %d\n", mesh.getNumTriangles());
}


int main(int argc, char** argv) {
    
    try {
        if (argc < 2) {
            throw std::string("You must supply a command line argument that is the name of an IFS file.  Usage: viewer <mesh_name>");
        }
        
        IFSReader reader;
        
        reader.load(argv[1], mesh);
        
        dumpInfo();
        
        mesh.scaleMesh(meshScale);
    } catch (const std::string& s) {
        error("Critical Error", s, true);
        SDL_Quit();
        exit(-1);
    }
    
    initGL();
    setup();
    
    bool _endProgram = false;
    Uint32 curr      = SDL_GetTicks();
    float dtime      = 0.0f;
    
    do {
        SDL_Event event;
        
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_QUIT:
                _endProgram = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    _endProgram = true;
                    break;
                case 'w':
                case 'W':
                    wire = !wire;
                    
                    if (wire) {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    } else {
                        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    }
                    break;
                }
                break;
            }
        }
        
        dtime = (SDL_GetTicks() - curr) / 1000.0f;
        curr = SDL_GetTicks();
        
        draw(dtime);
        
    } while (! _endProgram);
    
    return 0;
}
