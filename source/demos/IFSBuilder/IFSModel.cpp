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

XIFSModel::XIFSModel(const std::string& filename) {
    if (! fileExists(filename)) {
        error("Critical Error", std::string("File not found: \"") + filename + "\"", true);
        exit(-1);
    }

    std::string f = toLower(filename);

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


void XIFSModel::createPolygon() {
    IFSModelBuilder builder;

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
    IFSModelBuilder builder;

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
    IFSModelBuilder builder;
    
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
    IFSModelBuilder builder;

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
    IFSModelBuilder builder;
    
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
    IFSModelBuilder builder;
    
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
    IFSModelBuilder builder;
    
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

        renderDevice->setColor(Color3::WHITE);
        renderDevice->setLineWidth(.5);

        renderDevice->setPolygonOffset(1);
        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        for (j = 0; j < 2; ++j) {
            renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
                for (t = 0; t < triangleArray.size(); ++t) {
                    for (i = 0; i < 3; ++i) {
                        renderDevice->sendVertex(geometry.vertexArray[triangleArray[t].index[i]]);
                    }
                }
            renderDevice->endPrimitive();
            renderDevice->setPolygonOffset(0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            renderDevice->setColor(Color3::BLACK);
            glEnable(GL_LINE_SMOOTH);
            renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Label vertices (when there are too many, don't draw all)
        const double S = 10;
        int step = iMax(1, geometry.vertexArray.size() / 50);
        for (int v = 0; v < geometry.vertexArray.size(); v += step) {
            drawBillboardString(font, format("%d", v), geometry.vertexArray[v], S, Color3::YELLOW, Color3::BLACK);
        }

//        Draw::vertexNormals(geometry, renderDevice);

        // Label edges
        //for (int e = 0; e < edgeArray.size(); ++e) {
        //    const Vector3 pos = (geometry.vertexArray[edgeArray[e].vertexIndex[0]] + geometry.vertexArray[edgeArray[e].vertexIndex[1]]) / 2; 
        //    drawBillboardString(font, format("%d", e), pos, D * 20, Color3::BLUE, Color3::BLACK);
        //}


        // Show broken edges
        renderDevice->setColor(Color3::RED);
        renderDevice->setLineWidth(3);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int b = 0; b < brokenEdgeArray.size(); ++b) {
                for (int j = 0; j < 2; ++j) {
                    renderDevice->sendVertex(geometry.vertexArray[brokenEdgeArray[b].vertexIndex[j]]);
                }
            }
        renderDevice->endPrimitive();

        Draw::axes(renderDevice);
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

