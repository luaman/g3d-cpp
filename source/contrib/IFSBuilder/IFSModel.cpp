/**
  @file IFSBuilder/XIFSModel.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite 3DS loader based on code by Max McGuire and Matthew Fairfax
  @cite MD2 format by id software

  @created 2002-02-27
  @edited  2003-12-09
 */

#include "IFSModel.h"
#include "IFSModelBuilder.h"
#include "MD2.h"
#include "Load3DS.h"

extern RenderDevice* renderDevice;
extern GCamera       camera;
extern CFontRef      font;



static double gaussian2D(double x, double z) {
    return exp(-4 * (square(x) + square(z)));
}


static double bump2D(double x, double z) {
    double t = min(G3D_PI, 7 * sqrt(square(x) + square(z)) / sqrt(0.5));
    return cos(t) * .15;
}

static double saddle2D(double x, double z) {
    return (square(x*1.5) - square(z*1.5)) * .75;
}

static double flat2D(double x, double z) {
    return 0.0;
}


static double sin2D(double x, double z) {
    static double angle = toRadians(-25);
    double t = G3D_TWO_PI * (cos(angle) * x + sin(angle) * z);
    return sin(t * 2) * 0.1;
}

static double lumpy2D(double x, double z) {
    x *= G3D_TWO_PI;
    z *= G3D_TWO_PI;
    return 
        (cos(x) * 0.1 + cos(2*x + 1) * 0.15 + cos(3*x - 2) * 0.1 + cos(5*x + 4) * 0.05 - x * .1 +
         cos(z) * 0.2 + cos(2.5*z - 1) * 0.15 + cos(4*z + 2) * 0.15 + cos(5*z + 4) * 0.05 - z * .1) / 2;
}


static double cliff2D(double x, double z) {
	if (x + z > .25) {
		return 0.5;
	} else {
		return 0.0;
	}
}


XIFSModel::XIFSModel(const std::string& filename, bool t) : _twoSided(t) {
    if (! fileExists(filename)) {
        error("Critical Error", std::string("File not found: \"") + filename + "\"", true);
        exit(-1);
    }

    std::string f = toLower(filename);

    //createGrid(flat2D, 1024, true); return;
    //createGrid(lumpy2D, 1024, true); return;
    //createGrid(cliff2D, 1024, true); return;
    //createIsoGrid(cliff2D, 1024); return;
    //createGrid(bump2D, 900, true); return;
    //createIsoGrid(lumpy2D, 800); return;
    //createGrid(lumpy2D, 900, true); return;
    //createPolygon(); return;
    //createRing();  return;

    if (endsWith(f, ".ifs")) {
        loadIFS(filename);
    } else if (endsWith(f, ".md2")) {
        loadMD2(filename);
    } else if (endsWith(f, ".3ds")) {
        load3DS(filename);
    } else if (endsWith(f, ".obj")) {
        loadOBJ(filename);
    } else if (endsWith(f, ".sm")) {
        loadSM(filename);
    } else {
        debugAssert(false);
    }
}


static Vector3 isoToObjectSpace(int r, int c, int R, int C) {
    
    // We don't have the invisible, 1-vertex border so the
    // scale becaomes S = (1/(2(C-2)), 1, 1/(R-1)) and the edge
    // is one vertex closer to the body

    double edge = 0.0;
    
    if (isEven(r)) {
        if (c == 0) {
            edge = 1.0;
        } else if (c == C) {
            edge = -1.0;
        }
    }

    return 
        Vector3(2 * c + (r & 1) + edge - 2, 0, r) * 
        Vector3(0.5 / (C - 2.0), 1, 1.0 / (R - 1.0)) - Vector3(0.5, 0.0, 0.5);
}

void XIFSModel::createIsoGrid(double(*func)(double, double), int n) {
    IFSModelBuilder builder(_twoSided);

    double sin60 = sin(G3D_PI / 3.0);
    
    int C = iFloor(sqrt(n / 4.0));

    int R = iFloor(C / sin60);
    if (! isOdd(R)) {
        ++R;
    }


    Vector3 BOTTOM(0,-0.25,0);

    // Top & bottom
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {

            Vector3 D = isoToObjectSpace(r, c, R, C);
            D.y = func(D.x, D.z);

            Vector3 E = isoToObjectSpace(r, c + 1, R, C);
            E.y = func(E.x, E.z);

            Vector3 F = isoToObjectSpace(r + 1, c, R, C);
            F.y = func(F.x, F.z);

            Vector3 G = isoToObjectSpace(r + 1, c + 1, R, C);
            G.y = func(G.x, G.z);

            if (isOdd(r)) {
                //  D-----E
                //  |\    |
                //  |  \  |
                //  |    \|
                //  F-----G

                if (c < C - 1) {
                    builder.addTriangle(D, G, E);
                    builder.addTriangle(E + BOTTOM, G + BOTTOM, D + BOTTOM);
                }

                builder.addTriangle(D, F, G);
                builder.addTriangle(G + BOTTOM, F + BOTTOM, D + BOTTOM);
            } else {
                //  D-----E
                //  |    /|
                //  |  /  |
                //  |/    |
                //  F-----G
                builder.addTriangle(D, F, E);
                builder.addTriangle(E + BOTTOM, F + BOTTOM, D + BOTTOM);
                if (c < C - 1) {
                    builder.addTriangle(E, F, G);
                    builder.addTriangle(G + BOTTOM, F + BOTTOM, E + BOTTOM);
                }
            }
        }
    }

    // East and West edges
    for (int r = 0; r < R; ++r) {

        int c = 0;
        // West
        Vector3 D = isoToObjectSpace(r, c, R, C);
        D.y = func(D.x, D.z);

        Vector3 E = isoToObjectSpace(r + 1, c, R, C);
        E.y = func(E.x, E.z);

        builder.addTriangle(E, D, D + BOTTOM);
        builder.addTriangle(E + BOTTOM, E, D + BOTTOM);

        // East
        int s = r & 1;
        c = C;
        D = isoToObjectSpace(r, c - s, R, C);
        D.y = func(D.x, D.z);

        E = isoToObjectSpace(r + 1, c - 1 + s, R, C);
        E.y = func(E.x, E.z);

        builder.addTriangle(D + BOTTOM, D, E);
        builder.addTriangle(D + BOTTOM, E, E + BOTTOM);
    }

    // North and South edges
    for (int c = 0; c < C; ++c) {
        int r = 0;

        // North
        Vector3 D = isoToObjectSpace(r, c, R, C);
        D.y = func(D.x, D.z);

        Vector3 E = isoToObjectSpace(r, c + 1, R, C);
        E.y = func(E.x, E.z);

        builder.addTriangle(D + BOTTOM, D, E);
        builder.addTriangle(D + BOTTOM, E, E + BOTTOM);

        // South
        if (c < C - 1) {
            r = R;
            D = isoToObjectSpace(r, c, R, C);
            D.y = func(D.x, D.z);

            E = isoToObjectSpace(r, c + 1, R, C);
            E.y = func(E.x, E.z);
            builder.addTriangle(E, D, D + BOTTOM);
            builder.addTriangle(E + BOTTOM, E, D + BOTTOM);
        }
    }
    builder.commit(this);
}


void XIFSModel::createGrid(double(*func)(double, double), int n, bool consistentDiagonal) {
    IFSModelBuilder builder(_twoSided);

    int X = iFloor(sqrt(n / 4.0));
    int Z = X;
    double x, y, z;

    Vector3 BOTTOM(0,-0.25,0);

    // Top & bottom
    for (int ix = 0; ix < X; ++ix) {
        for (int iz = 0; iz < Z; ++iz) {

            x = ix / (double)X - 0.5;
            z = iz / (double)Z - 0.5;
            y = func(x, z);

            Vector3 A(x, y, z);

            x = (ix + 1) / (double)X - 0.5;
            z = (iz) / (double)Z - 0.5;
            y = func(x, z);

            Vector3 B(x, y, z);

            x = (ix + 1) / (double)X - 0.5;
            z = (iz + 1) / (double)Z - 0.5;
            y = func(x, z);

            Vector3 C(x, y, z);

            x = (ix) / (double)X - 0.5;
            z = (iz + 1) / (double)Z - 0.5;
            y = func(x, z);

            Vector3 D(x, y, z);

            if (! consistentDiagonal && ((A - C).length() > (D - B).length())) {
                // Cut the longer diagonal
                Vector3 a = A, b = B, c = C, d = D;
                A = d; B = a; C = b; D = c;
            }

            builder.addTriangle(A, D, C);
            builder.addTriangle(A, C, B);

            A += BOTTOM;
            B += BOTTOM;
            C += BOTTOM;
            D += BOTTOM;

            builder.addTriangle(C, D, A);
            builder.addTriangle(B, C, A);
        }
    }

    // Stitch up edges
    for (int ix = 0; ix < X; ++ix) {
        int iz = 0;
        x = ix / (double)X - 0.5;
        z = iz / (double)Z - 0.5;
        y = func(x, z);

        Vector3 A(x, y, z);
        Vector3 B(x, y-0.25, z);

        x = (ix + 1) / (double)X - 0.5;
        y = func(x, z);

        Vector3 C(x, y-0.25, z);
        Vector3 D(x, y, z);

        builder.addTriangle(A, D, C);
        builder.addTriangle(A, C, B);

        iz = Z;
        x = ix / (double)X - 0.5;
        z = iz / (double)Z - 0.5;
        y = func(x, z);

        A = Vector3(x, y, z);
        B = Vector3(x, y-0.25, z);

        x = (ix + 1) / (double)X - 0.5;
        y = func(x, z);

        C = Vector3(x, y-0.25, z);
        D = Vector3(x, y, z);

        builder.addTriangle(C, D, A);
        builder.addTriangle(B, C, A);
    }

    // Stitch up edges
    for (int iz = 0; iz < Z; ++iz) {
        int ix = X;
        x = ix / (double)X - 0.5;
        z = iz / (double)Z - 0.5;
        y = func(x, z);

        Vector3 A(x, y, z);
        Vector3 B(x, y-0.25, z);

        z = (iz + 1) / (double)Z - 0.5;
        y = func(x, z);

        Vector3 C(x, y-0.25, z);
        Vector3 D(x, y, z);

        builder.addTriangle(A, D, C);
        builder.addTriangle(A, C, B);

        ix = 0;
        x = ix / (double)X - 0.5;
        z = iz / (double)Z - 0.5;
        y = func(x, z);

        A = Vector3(x, y, z);
        B = Vector3(x, y-0.25, z);

        z = (iz + 1) / (double)Z - 0.5;
        y = func(x, z);

        C = Vector3(x, y-0.25, z);
        D = Vector3(x, y, z);

        builder.addTriangle(C, D, A);
        builder.addTriangle(B, C, A);
    }


    builder.commit(this);
}


void XIFSModel::createPolygon() {
    IFSModelBuilder builder(_twoSided);

    int sides = 8;

    double shift = isEven(sides) ? (G3D_PI / sides) : 0;
    for (int i = 1; i < sides - 1; ++i) {
        double angle0 = 2 * G3D_PI * i / sides + shift;
        double angle1 = 2 * G3D_PI * (i + 1) / sides + shift;

        Vector3 a(sin(shift), 0, -cos(shift));
        Vector3 b(sin(angle1), 0, -cos(angle1));
        Vector3 c(sin(angle0), 0, -cos(angle0));

        builder.addTriangle(a, b, c);
    }

    builder.commit(this);

}


/** Called from createRing */
static void addSubdividedQuad(
    IFSModelBuilder& builder,
    const Vector3&   A,
    const Vector3&   B,
    const Vector3&   C,
    const Vector3&   D,
    int              numSubdivisions) {

    if (numSubdivisions == 0) {
        // Base case
        builder.addTriangle(A, B, C);
        builder.addTriangle(A, C, D);
    } else {
        
        //   D----C
        //   |__--|
        //   F----E
        //   |__--|
        //   A----B

        Vector3 F = (A + D) / 2;
        Vector3 E = (C + B) / 2;
        // Cut in half along the long axis
        addSubdividedQuad(builder, A, B, E, F, numSubdivisions - 1);
        addSubdividedQuad(builder, F, E, C, D, numSubdivisions - 1);
    }
}


void XIFSModel::createRing() {
    IFSModelBuilder builder(_twoSided);

    int quads = 150;

    // Creates 2^n triangles
    int subdivisions = 3;

    // Radius is fixed at 1.0
    // Half width
    double w = .25;

    // 3x half twist torus
    double numTwists = 1.5;

    for (int i = 0; i < quads; ++i) {
        double angle0 = 2 * G3D_PI * i / quads;
        double angle1 = 2 * G3D_PI * (i + 1) / quads;

        double twist0 = numTwists * 2 * G3D_PI * i / quads;
        double twist1 = numTwists * 2 * G3D_PI * (i + 1) / quads;
        
        // Midline of ring
        Vector3 a, b;

        // Normals
        Vector3 c, d;

        // Outer (twisted) vertices
        Vector3 e, f, g, h;

        a = Vector3(cos(angle0), 0, sin(angle0));
        b = Vector3(cos(angle1), 0, sin(angle1));

        c = a;
        d = b;

        // center of ring
        //
        //   -f--h- 
        //    | /|
        //    |/ |
        //  --e--g--
        //
        //   outside

        e = a + w * (cos(twist0) * Vector3::UNIT_Y + sin(twist0) * c);
        f = a - w * (cos(twist0) * Vector3::UNIT_Y + sin(twist0) * c);
        g = b + w * (cos(twist1) * Vector3::UNIT_Y + sin(twist1) * c);
        h = b - w * (cos(twist1) * Vector3::UNIT_Y + sin(twist1) * c);

        // Want to add quad eghf, but we need to subdivide it along edges
        // ef and hg.

        addSubdividedQuad(builder, e, g, h, f, subdivisions);
    }

    builder.commit(this);
}


void XIFSModel::loadIFS(const std::string& filename) {
    IFSModelBuilder builder(_twoSided);
    
    BinaryInput b(filename, G3D_LITTLE_ENDIAN);

    std::string fmt         = b.readString32();
    float       version     = b.readFloat32();
    
    builder.setName(b.readString32());
    debugAssert(fmt     == "IFS");
    debugAssert(version == 1.0);

    std::string vertexHeader = b.readString32();

    Array<Vector3> va (b.readUInt32());

    for (int v = 0; v < va.size(); ++v) {
        va[v] = b.readVector3();
    }

    b.readString32();
    int n = b.readUInt32();

    int t;
    for (t = 0; t < n; ++t) {
        int v0 = b.readUInt32();
        int v1 = b.readUInt32();
        int v2 = b.readUInt32();

        builder.addTriangle(va[v0], va[v1], va[v2]);
    }

    builder.commit(this);
}



void XIFSModel::loadSM(const std::string& filename) {
    IFSModelBuilder builder(_twoSided);

    TextInput ti(filename);

    // Read vertices
    Array<Vector3> va(ti.readNumber());
    for (int v = 0; v < va.size(); ++v) {
        for (int i = 0; i < 3; ++i) {
            va[v][i] = ti.readNumber();
        }
    }

    // Read faces
    int n = ti.readNumber();

    int t;
    for (t = 0; t < n; ++t) {
        int v0 = ti.readNumber();
        int v1 = ti.readNumber();
        int v2 = ti.readNumber();

        builder.addTriangle(va[v0], va[v1], va[v2]);
    }

    builder.commit(this);
}


void XIFSModel::loadMD2(const std::string& filename) {
    IFSModelBuilder builder(_twoSided);
    
    BinaryInput b(filename, G3D_LITTLE_ENDIAN);

    MD2ModelHeader header;

    // This code only works on little endian machines
    
    b.readBytes(sizeof(header), (unsigned char*)&header);
    debugAssert(header.version == 8);
    debugAssert(header.numVertices <= 4096);

    // One frame of animation
    MD2Frame    md2Frame;

    // Read a single frame
    const int frameNum = 0;
    b.setPosition(header.offsetFrames + frameNum * header.frameSize);
    b.readBytes(sizeof(MD2Frame), (unsigned char*)&md2Frame);

    // Read the vertices for the frame
    Vector3     va[2048];

    // Quake's axes are permuted and scaled
    double scale[3]   = {.07, .07, -.07};
    int    permute[3] = {2, 0, 1};
    int v, i;
    for (v = 0; v < header.numVertices; ++v) {
        for (i = 0; i < 3; ++i) {
            va[v][permute[i]] = (b.readUInt8() * md2Frame.scale[i] + md2Frame.translate[i]) * scale[permute[i]];
        }
        b.skip(1);
    }

    // Read the triangles
    int t;
    b.setPosition(header.offsetTriangles);
    for (t = 0; t < header.numTriangles; ++t) {
        int v0 = b.readUInt16();
        int v1 = b.readUInt16();
        int v2 = b.readUInt16();
 
        builder.addTriangle(va[v0], va[v1], va[v2]);
        // Skip the texture indices
        b.skip(6);
    }


    builder.commit(this);
}



void XIFSModel::load3DS(const std::string& filename) {
    IFSModelBuilder builder(_twoSided);
    
    BinaryInput b(filename, G3D_LITTLE_ENDIAN);

    Load3DS loader(b);

    for (int obj = 0; obj < loader.objectArray.size(); ++obj) {
        const Matrix4& keyframe = loader.objectArray[obj].keyframe;
        
        Vector3 pivot = loader.objectArray[obj].pivot;

        const Array<int>&     index   = loader.objectArray[obj].indexArray;
        const Array<Vector3>& vertex  = loader.objectArray[obj].vertexArray;

        // The cframe has already been applied in the 3DS file.  If
        // we change the Load3DS object representation to move
        // coordinates back to object space, this is the code
        // that will promote them to world space before constructing
        // the mesh.

        const Matrix4&        cframe  = loader.objectArray[obj].cframe;
        //#define vert(V) (keyframe * cframe * Vector4(vertex[index[V]], 1)).xyz()

        // TODO: Figure out correct transformation
        #define vert(V) \
          ((keyframe * \
           ( Vector4(vertex[index[V]], 1) - Vector4(pivot, 0)) \
           )).xyz() 

        for (int i = 0; i < index.size(); i += 3) {
            builder.addTriangle(
                vert(i),
                vert(i + 1),
                vert(i + 2));
        }

        #undef vert
    }
      
    builder.setName(loader.objectArray[0].name);

    builder.commit(this);
}



void XIFSModel::loadOBJ(const std::string& filename) {
    IFSModelBuilder builder(_twoSided);
    
    TextInput::Options options;
    options.cppComments = false;
    TextInput t(filename, options);

    Array<Vector3> va;

    try {
        while (t.hasMore()) {
            Token tag = t.read();

            if (tag.type() == Token::SYMBOL) {
                if (tag.string() == "v") {
                    // Vertex
                    Vector3 vertex;
                    vertex.x = t.readNumber();
                    vertex.y = t.readNumber();
                    vertex.z = t.readNumber();

                    va.append(vertex);

                } else if (tag.string() == "f") {
                    // Face (may have an arbitrary number of vertices)
                    Array<int> vertexIndex;
            
                    while ((t.peek().line() == tag.line()) && t.hasMore()) {  
                        vertexIndex.append(t.readNumber());
                        debugAssert(vertexIndex.last() > 0);
                        --vertexIndex.last();

                        // Fix illegal vertex numbers
                        if ((vertexIndex.last() <= 0) || vertexIndex.last() >= va.size()) {
                            vertexIndex[vertexIndex.size() - 1] = 0;
                        }

                        // Skip over the slashes
                        Token next = t.peek();
                        while ((next.type() == Token::SYMBOL) && (next.string() == "/")) {
                            next = t.read();
                        }

                        if (vertexIndex.size() >= 3) {
                            int i = vertexIndex.size() - 1;
                            builder.addTriangle(va[vertexIndex[0]],
                                                va[vertexIndex[i - 1]],
                                                va[vertexIndex[i]]);
                        }
                    }

                }
            }

            // Read to the next line
            while ((t.peek().line() == tag.line()) && t.hasMore()) {
                t.read();
            }
        }
    } catch (TextInput::WrongTokenType& e) {
        debugAssert(e.expected != e.actual);
        e;
    }
    
    builder.setName(filename);

    builder.commit(this);
}


static void drawBillboardString(
    CFontRef            font,
    const std::string&  s,
    const Vector3&      pos,
    double              screenSize,
    const Color4&       color,
    const Color4&       outline = Color4::CLEAR) {

    // Find the point

    Vector3 screenPos = camera.project(pos, renderDevice->getViewport());

    renderDevice->push2D();
        font->draw2D(s, screenPos.xy(), screenSize, color, outline, GFont::XALIGN_CENTER, GFont::YALIGN_CENTER);
    renderDevice->pop2D();
}


void XIFSModel::render() {
    renderDevice->pushState();

        int t, i, j;

        glEnable(GL_LINE_SMOOTH);
        renderDevice->setColor(Color3::WHITE);
        renderDevice->setLineWidth(.5);

        renderDevice->setPolygonOffset(.5);
        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        // First iteration draws surfaces, 2nd draws wireframe
        for (j = 0; j < 2; ++j) {
            renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
                for (t = 0; t < triangleArray.size(); ++t) {
                    renderDevice->setNormal(faceNormalArray[t]);
                    for (i = 0; i < 3; ++i) {
                        const int idx = triangleArray[t].index[i];
                        renderDevice->sendVertex(geometry.vertexArray[idx]);
                    }
                }
            renderDevice->endPrimitive();
            renderDevice->setPolygonOffset(0);
            renderDevice->setRenderMode(RenderDevice::RENDER_WIREFRAME);
            renderDevice->setColor(Color3::BLACK);
            renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        }
        renderDevice->setRenderMode(RenderDevice::RENDER_SOLID);

        /*
        // Label vertices (when there are too many, don't draw all)
        const double S = 10;
        int step = iMax(1, geometry.vertexArray.size() / 50);
        for (int v = 0; v < geometry.vertexArray.size(); v += step) {
            drawBillboardString(font, format("%d", v), geometry.vertexArray[v], S, Color3::YELLOW, Color3::BLACK);
        }
        */

        // Draw::vertexNormals(geometry, renderDevice);

        // Label edges
        //for (int e = 0; e < edgeArray.size(); ++e) {
        //    const Vector3 pos = (geometry.vertexArray[edgeArray[e].vertexIndex[0]] + geometry.vertexArray[edgeArray[e].vertexIndex[1]]) / 2; 
        //    drawBillboardString(font, format("%d", e), pos, D * 20, Color3::BLUE, Color3::BLACK);
        //}


        // Show broken edges
        renderDevice->disableLighting();
        renderDevice->setColor(Color3::RED);
        renderDevice->setLineWidth(3);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int b = 0; b < brokenEdgeArray.size(); ++b) {
                for (int j = 0; j < 2; ++j) {
                    renderDevice->sendVertex(geometry.vertexArray[brokenEdgeArray[b].vertexIndex[j]]);
                }
            }
        renderDevice->endPrimitive();

        //Draw::axes(renderDevice);
        glDisable(GL_LINE_SMOOTH);

    renderDevice->popState();
}


void XIFSModel::save(const std::string& filename) {
    BinaryOutput out(filename, G3D_LITTLE_ENDIAN);

    out.writeString32("IFS");
    out.writeFloat32(1.0);
    out.writeString32(name);

    out.writeString32("VERTICES");
    out.writeUInt32(geometry.vertexArray.size());
    int i;
    for (i = 0; i < geometry.vertexArray.size(); ++i) {
        out.writeVector3(geometry.vertexArray[i]);
    }

    out.writeString32("TRIANGLES");
    out.writeUInt32(triangleArray.size());
    for (i = 0; i < triangleArray.size(); ++i) {
        out.writeUInt32(triangleArray[i].index[0]);
        out.writeUInt32(triangleArray[i].index[1]);
        out.writeUInt32(triangleArray[i].index[2]);
    }
    
    out.commit();
}

