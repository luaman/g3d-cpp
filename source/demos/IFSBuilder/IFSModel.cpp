/**
  @file IFSBuilder/IFSModel.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite 3DS loader based on code by Max McGuire and Matthew Fairfax
  @cite MD2 format by id software

  @created 2002-02-27
  @edited  2002-04-03
 */

#include "IFSModel.h"
#include "IFSModelBuilder.h"
#include "MD2.h"

extern RenderDevice* renderDevice;

IFSModel::IFSModel(const std::string& filename) {
    if (! fileExists(filename)) {
        error("Critical Error", std::string("File not found: \"") + filename + "\"", true);
        exit(-1);
    }

    std::string f = toLower(filename);

    if (endsWith(f, ".ifs")) {
        loadIFS(filename);
    } else if (endsWith(f, ".md2")) {
        loadMD2(filename);
    } else if (endsWith(f, ".3ds")) {
        load3DS(filename);
    } else if (endsWith(f, ".obj")) {
        loadOBJ(filename);
    } else {
        debugAssert(false);
    }
}


void IFSModel::loadIFS(const std::string& filename) {
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


void IFSModel::loadMD2(const std::string& filename) {
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


void IFSModel::load3DS(const std::string& filename) {
    IFSModelBuilder builder;
    
    BinaryInput b(filename, G3D_LITTLE_ENDIAN);

    const int MAIN3DS   = 0x4D4D;
    const int EDIT3DS   = 0x3D3D;
    const int OBJECT    = 0x4000;
    const int TRIG_MESH	= 0x4100;
    const int VERT_LIST	= 0x4110;
    const int FACE_DESC = 0x4120;

    unsigned short chunkType;

    long chunkLength;
    long chunkEnd;
    long fileLength;

    chunkType = b.readUInt16();
    fileLength = b.readUInt32();

    debugAssert(chunkType == MAIN3DS);

    Array<Vector3> va;

    int numPrevVerts = 0;

    while (b.hasMore()) {
        chunkType   = b.readUInt16();
        chunkLength = b.readInt32();
        debugAssert(chunkLength < fileLength);

        chunkEnd = b.getPosition() + chunkLength - 6;

        int index;
        switch (chunkType) {
        case MAIN3DS:
        case EDIT3DS:
        case TRIG_MESH:
            break;

            
        case OBJECT:
            b.readString();
            break;

        case VERT_LIST:
            for (index = b.readUInt16() - 1; index >= 0; --index) {
                va.append(b.readVector3());
            }
            b.setPosition(chunkEnd);
            break;

        case FACE_DESC:
            // Triangle
            for (index = b.readUInt16() - 1; index >= 0; --index) {
                int v0 = b.readUInt16();
                int v1 = b.readUInt16();
                int v2 = b.readUInt16();
                b.skip(2);

                builder.addTriangle(va[v0], va[v1], va[2]);
            }
            b.setPosition(chunkEnd);
            break;

        default:
            b.setPosition(chunkEnd);
        }
    }
    
    builder.setName(filename);

    builder.commit(this);
}



void IFSModel::loadOBJ(const std::string& filename) {
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


void IFSModel::render() {
    renderDevice->pushState();

        int t, i, j;

        renderDevice->setColor(Color3::WHITE);

        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        for (j = 0; j < 2; ++j) {
            renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
                for (t = 0; t < triangleArray.size(); ++t) {
                    for (i = 0; i < 3; ++i) {
                        renderDevice->sendVertex(vertexArray[triangleArray[t].index[i]]);
                    }
                }
            renderDevice->endPrimitive();
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            renderDevice->setColor(Color3::BLACK);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    renderDevice->popState();
}


void IFSModel::save(const std::string& filename) {
    BinaryOutput out(filename, G3D_LITTLE_ENDIAN);

    out.writeString32("IFS");
    out.writeFloat32(1.0);
    out.writeString32(name);

    out.writeString32("VERTICES");
    out.writeUInt32(vertexArray.size());
    int i;
    for (i = 0; i < vertexArray.size(); ++i) {
        out.writeVector3(vertexArray[i]);
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

