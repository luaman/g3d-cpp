/**
 @file MD2Model_load.cpp

 Code for loading an MD2Model

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2003-08-07
 @edited  2005-02-24

 */

#include "GLG3D/MD2Model.h"

namespace G3D {
Vector3 MD2Model::normalTable[162];


class MD2ModelHeader {
public:
    int magic; 
    int version; 
    int skinWidth; 
    int skinHeight; 
    int frameSize; 
    int numSkins; 
    int numVertices; 
    int numTexCoords; 
    int numTriangles; 
    int numGlCommands; 
    int numFrames; 
    int offsetSkins; 
    int offsetTexCoords; 
    int offsetTriangles; 
    int offsetFrames; 
    int offsetGlCommands; 
    int offsetEnd;

    void deserialize(BinaryInput& b) {
        magic               = b.readInt32();
        version             = b.readInt32(); 
        skinWidth           = b.readInt32(); 
        skinHeight          = b.readInt32(); 
        frameSize           = b.readInt32(); 
        numSkins            = b.readInt32(); 
        numVertices         = b.readInt32(); 
        numTexCoords        = b.readInt32(); 
        numTriangles        = b.readInt32(); 
        numGlCommands       = b.readInt32(); 
        numFrames           = b.readInt32(); 
        offsetSkins         = b.readInt32(); 
        offsetTexCoords     = b.readInt32(); 
        offsetTriangles     = b.readInt32(); 
        offsetFrames        = b.readInt32(); 
        offsetGlCommands    = b.readInt32(); 
        offsetEnd           = b.readInt32();

    }
};


struct MD2Frame {
public:
   Vector3          scale;
   Vector3          translate;
   std::string      name;

   void deserialize(BinaryInput& b) {
       scale.deserialize(b);
       translate.deserialize(b);
       name = b.readString(16);
   }
};


void MD2Model::reset() {
    _textureFilenames.clear();
    keyFrame.clear();
    primitiveArray.clear();
    indexArray.clear();
    _texCoordArray.clear();
    faceArray.clear();
    vertexArray.clear();
    edgeArray.clear();
}


void MD2Model::load(const std::string& filename, float resize) {

    // If models are being reloaded it is dangerous to trust the interpolation cache.
    interpolatedModel = NULL;

    alwaysAssertM(fileExists(filename), std::string("Can't find \"") + filename + "\"");

    setNormalTable();

    // Clear out
    reset();

    BinaryInput b(filename, G3D_LITTLE_ENDIAN);

    MD2ModelHeader header;

    header.deserialize(b);
    debugAssert(header.version == 8);
    debugAssert(header.numVertices <= 4096);

    keyFrame.resize(header.numFrames);
    Array<Vector3> frameMin(header.numFrames); 
    Array<Vector3> frameMax(header.numFrames);
    Array<double>  frameRad(header.numFrames);

    Vector3 min  = Vector3::inf();
    Vector3 max  = -Vector3::inf();
    double  rad  = 0;

    if (header.numVertices < 3) {
        Log::common()->printf("\n*****************\nWarning: \"%s\" is corrupted and is not being loaded.\n", filename.c_str());
        return;
    }

    loadTextureFilenames(b, header.numSkins, header.offsetSkins);

    for (int f = 0; f < keyFrame.size(); ++f) {
        MD2Frame md2Frame;

        b.setPosition(header.offsetFrames + f * header.frameSize);
        md2Frame.deserialize(b);

        // Read the vertices for the frame
        keyFrame[f].vertexArray.resize(header.numVertices);
        keyFrame[f].normalArray.resize(header.numVertices);

        // Per-pose bounds
        Vector3 min_1  = Vector3::inf();
        Vector3 max_1  = -Vector3::inf();
        double  rad_1  = 0;

        // Quake's axes are permuted and scaled
        double scale[3]   = {-.07, .07, -.07};
        int    permute[3] = {2, 0, 1};
        int v, i;
        for (v = 0; v < header.numVertices; ++v) {

            Vector3& vertex = keyFrame[f].vertexArray[v];
            for (i = 0; i < 3; ++i) {
                vertex[permute[i]] = (b.readUInt8() * resize * md2Frame.scale[i] + md2Frame.translate[i]) * scale[permute[i]];
            }

            uint8 normalIndex = b.readUInt8();
            debugAssert(normalIndex < 162);
            keyFrame[f].normalArray[v] = normalIndex;

            min_1 = min_1.min(vertex);
            max_1 = max_1.max(vertex);

            if (vertex.squaredLength() > rad_1) {
                rad_1 = vertex.squaredLength();
            }
        }

        frameMin[f] = min_1;
        frameMax[f] = max_1;
        frameRad[f] = sqrt(rad_1);

        min = min.min(min_1);
        max = max.max(max_1);

        if (rad_1 > rad) {
            rad = rad_1;
        }
    }

    // Compute per-animation bounds based on frame bounds
    for (int a = 0; a < JUMP; ++a) {
        const int first = animationTable[a].first;
        const int last  = animationTable[a].last;

        if ((first < header.numFrames) && (last < header.numFrames)) {
            Vector3 min = frameMin[first];
            Vector3 max = frameMax[first];
            double rad  = frameRad[first];

            for (int i = first + 1; i <= last; ++i) {
                min = min.min(frameMin[i]);
                max = max.max(frameMax[i]);
                rad = G3D::max(rad, frameRad[i]);
            }

            animationBoundingBox[a]    = Box(min, max);
            animationBoundingSphere[a] = Sphere(Vector3::zero(), rad);
        } else {
            // This animation is not supported by this model
            animationBoundingBox[a]    = Box(Vector3::zero(), Vector3::zero());
            animationBoundingSphere[a] = Sphere(Vector3::zero(), 0);
        }
    }

    animationBoundingBox[JUMP] = animationBoundingBox[JUMP_DOWN];
    animationBoundingSphere[JUMP] = animationBoundingSphere[JUMP_DOWN];

    boundingBox    = Box(min, max);
    boundingSphere = Sphere(Vector3::zero(), sqrt(rad));

    // Load the texture coords
    texFrame.rotation[0][0] = 1.0 / header.skinWidth; 
    texFrame.rotation[1][1] = 1.0 / header.skinHeight;

    Array<Vector2int16> fileTexCoords(header.numTexCoords);
    b.setPosition(header.offsetTexCoords);
    for (int t = 0; t < fileTexCoords.size(); ++t) {
        fileTexCoords[t].x = b.readUInt16();
        fileTexCoords[t].y = b.readUInt16();
    }

    // The indices for the texture coords (which don't match the
    // vertex indices originally).
    indexArray.resize(header.numTriangles * 3);
    Array<Vector2int16> index_texCoordArray(indexArray.size());

    // Read the triangles, reversing them to get triangle list order
    b.setPosition(header.offsetTriangles);
    for (int t = header.numTriangles - 1; t >= 0; --t) {

        for (int i = 2; i >= 0; --i) {
            indexArray[t * 3 + i] = b.readUInt16();
        }

        for (int i = 2; i >= 0; --i) {
            index_texCoordArray[t * 3 + i] = fileTexCoords[b.readUInt16()];
        }
    }

    computeTexCoords(index_texCoordArray);

    // Read the primitives
    {
        primitiveArray.clear();
        b.setPosition(header.offsetGlCommands);
        
        int n = b.readInt32();

        while (n != 0) {
            Primitive& primitive = primitiveArray.next();

            if (n > 0) {
                primitive.type = RenderDevice::TRIANGLE_STRIP;
            } else {
                primitive.type = RenderDevice::TRIANGLE_FAN;
                n = -n;
            }

            primitive.pvertexArray.resize(n);

            Array<Primitive::PVertex>&  pvertex = primitive.pvertexArray;

            for (int i = 0; i < pvertex.size(); ++i) {
                pvertex[i].texCoord.x = b.readFloat32();
                pvertex[i].texCoord.y = b.readFloat32();
                pvertex[i].index      = b.readInt32();
            }

            n = b.readInt32();
        }
    }


    MeshAlg::computeAdjacency(keyFrame[0].vertexArray, indexArray, faceArray, edgeArray, vertexArray);
    weldedFaceArray = faceArray;
    weldedEdgeArray = edgeArray;
    weldedVertexArray = vertexArray;
    MeshAlg::weldAdjacency(keyFrame[0].vertexArray, weldedFaceArray, weldedEdgeArray, weldedVertexArray);

    numBoundaryEdges = MeshAlg::countBoundaryEdges(edgeArray);
    numWeldedBoundaryEdges = MeshAlg::countBoundaryEdges(weldedEdgeArray);
}


void MD2Model::loadTextureFilenames(BinaryInput& b, int num, int offset) {

    _textureFilenames.resize(num);
    b.setPosition(offset);
    for (int t = 0; t < num; ++t) {
        _textureFilenames[t] = b.readString();
    }
}


void MD2Model::computeTexCoords(
    const Array<Vector2int16>&   inCoords) {

    int numVertices = keyFrame[0].vertexArray.size();

    // Table mapping original vertex indices to alternative locations
    // for that vertex (corresponding to different texture coords).
    // A zero length array means a vertex that was not yet seen.
    Array< Array<int> > cloneListArray(numVertices);

    _texCoordArray.resize(numVertices);

    // Walk through the index array and inCoords array
    for (int i = 0; i < indexArray.size(); ++i) {
        // Texture coords
        const Vector2int16& coords = inCoords[i];

        // Vertex index
        const int v = indexArray[i];

        // cloneList[vertex index] = vertex index of clone
        Array<int>& cloneList = cloneListArray[v];

        if (cloneList.size() == 0) {
            // We've never seen this vertex before, so assign it
            // the texture coordinates we already have.
            cloneList.append(v);
            _texCoordArray[v] = coords;

        } else {
            bool foundMatch = false;

            // Walk through the clones and see if one has the same tex coords
            for (int c = 0; c < cloneList.size(); ++c) {
                const int clone = cloneList[c];

                if (_texCoordArray[clone] == coords) {
                    // Found a match
                    foundMatch = true;

                    // Replace the index with the clone's index
                    indexArray[i] = clone;

                    // The texture coordinates are already assigned
                    break;
                }
            }

            if (! foundMatch) {
                // We have a new vertex.  Add it to the list of vertices
                // cloned from the original.
                const int clone = numVertices;
                cloneList.append(clone);

                // Overwrite the index array entry
                indexArray[i] = clone;

                // Add the texture coordinates
                _texCoordArray.append(coords);

                // Clone the vertex in every key pose.
                for (int k = 0; k < keyFrame.size(); ++k) {
                    keyFrame[k].vertexArray.append(keyFrame[k].vertexArray[v]);
                    keyFrame[k].normalArray.append(keyFrame[k].normalArray[v]);
                }

                ++numVertices;
            }
        }
    }
}


void MD2Model::setNormalTable() {
    if (normalTable[0].y != 0) {
        // The table has already been initialized
        return;
    }

    // Initialize the global table
    normalTable[  0] = Vector3(0.000000, 0.850651, 0.525731);
    normalTable[  1] = Vector3(-0.238856, 0.864188, 0.442863);
    normalTable[  2] = Vector3(0.000000, 0.955423, 0.295242);
    normalTable[  3] = Vector3(-0.500000, 0.809017, 0.309017);
    normalTable[  4] = Vector3(-0.262866, 0.951056, 0.162460);
    normalTable[  5] = Vector3(0.000000, 1.000000, 0.000000);
    normalTable[  6] = Vector3(-0.850651, 0.525731, 0.000000);
    normalTable[  7] = Vector3(-0.716567, 0.681718, 0.147621);
    normalTable[  8] = Vector3(-0.716567, 0.681718, -0.147621);
    normalTable[  9] = Vector3(-0.525731, 0.850651, 0.000000);
    normalTable[ 10] = Vector3(-0.500000, 0.809017, -0.309017);
    normalTable[ 11] = Vector3(0.000000, 0.850651, -0.525731);
    normalTable[ 12] = Vector3(0.000000, 0.955423, -0.295242);
    normalTable[ 13] = Vector3(-0.238856, 0.864188, -0.442863);
    normalTable[ 14] = Vector3(-0.262866, 0.951056, -0.162460);
    normalTable[ 15] = Vector3(-0.147621, 0.716567, 0.681718);
    normalTable[ 16] = Vector3(-0.309017, 0.500000, 0.809017);
    normalTable[ 17] = Vector3(-0.425325, 0.688191, 0.587785);
    normalTable[ 18] = Vector3(-0.525731, 0.000000, 0.850651);
    normalTable[ 19] = Vector3(-0.442863, 0.238856, 0.864188);
    normalTable[ 20] = Vector3(-0.681718, 0.147621, 0.716567);
    normalTable[ 21] = Vector3(-0.587785, 0.425325, 0.688191);
    normalTable[ 22] = Vector3(-0.809017, 0.309017, 0.500000);
    normalTable[ 23] = Vector3(-0.864188, 0.442863, 0.238856);
    normalTable[ 24] = Vector3(-0.688191, 0.587785, 0.425325);
    normalTable[ 25] = Vector3(-0.681718, -0.147621, 0.716567);
    normalTable[ 26] = Vector3(-0.809017, -0.309017, 0.500000);
    normalTable[ 27] = Vector3(-0.850651, 0.000000, 0.525731);
    normalTable[ 28] = Vector3(-0.850651, -0.525731, 0.000000);
    normalTable[ 29] = Vector3(-0.864188, -0.442863, 0.238856);
    normalTable[ 30] = Vector3(-0.955423, -0.295242, 0.000000);
    normalTable[ 31] = Vector3(-0.951056, -0.162460, 0.262866);
    normalTable[ 32] = Vector3(-1.000000, 0.000000, 0.000000);
    normalTable[ 33] = Vector3(-0.955423, 0.295242, 0.000000);
    normalTable[ 34] = Vector3(-0.951056, 0.162460, 0.262866);
    normalTable[ 35] = Vector3(-0.864188, 0.442863, -0.238856);
    normalTable[ 36] = Vector3(-0.951056, 0.162460, -0.262866);
    normalTable[ 37] = Vector3(-0.809017, 0.309017, -0.500000);
    normalTable[ 38] = Vector3(-0.864188, -0.442863, -0.238856);
    normalTable[ 39] = Vector3(-0.951056, -0.162460, -0.262866);
    normalTable[ 40] = Vector3(-0.809017, -0.309017, -0.500000);
    normalTable[ 41] = Vector3(-0.525731, 0.000000, -0.850651);
    normalTable[ 42] = Vector3(-0.681718, 0.147621, -0.716567);
    normalTable[ 43] = Vector3(-0.681718, -0.147621, -0.716567);
    normalTable[ 44] = Vector3(-0.850651, 0.000000, -0.525731);
    normalTable[ 45] = Vector3(-0.688191, 0.587785, -0.425325);
    normalTable[ 46] = Vector3(-0.442863, 0.238856, -0.864188);
    normalTable[ 47] = Vector3(-0.587785, 0.425325, -0.688191);
    normalTable[ 48] = Vector3(-0.309017, 0.500000, -0.809017);
    normalTable[ 49] = Vector3(-0.147621, 0.716567, -0.681718);
    normalTable[ 50] = Vector3(-0.425325, 0.688191, -0.587785);
    normalTable[ 51] = Vector3(-0.295242, 0.000000, -0.955423);
    normalTable[ 52] = Vector3(0.000000, 0.000000, -1.000000);
    normalTable[ 53] = Vector3(-0.162460, 0.262866, -0.951056);
    normalTable[ 54] = Vector3(0.525731, 0.000000, -0.850651);
    normalTable[ 55] = Vector3(0.295242, 0.000000, -0.955423);
    normalTable[ 56] = Vector3(0.442863, 0.238856, -0.864188);
    normalTable[ 57] = Vector3(0.162460, 0.262866, -0.951056);
    normalTable[ 58] = Vector3(0.309017, 0.500000, -0.809017);
    normalTable[ 59] = Vector3(0.147621, 0.716567, -0.681718);
    normalTable[ 60] = Vector3(0.000000, 0.525731, -0.850651);
    normalTable[ 61] = Vector3(-0.442863, -0.238856, -0.864188);
    normalTable[ 62] = Vector3(-0.309017, -0.500000, -0.809017);
    normalTable[ 63] = Vector3(-0.162460, -0.262866, -0.951056);
    normalTable[ 64] = Vector3(0.000000, -0.850651, -0.525731);
    normalTable[ 65] = Vector3(-0.147621, -0.716567, -0.681718);
    normalTable[ 66] = Vector3(0.147621, -0.716567, -0.681718);
    normalTable[ 67] = Vector3(0.000000, -0.525731, -0.850651);
    normalTable[ 68] = Vector3(0.309017, -0.500000, -0.809017);
    normalTable[ 69] = Vector3(0.442863, -0.238856, -0.864188);
    normalTable[ 70] = Vector3(0.162460, -0.262866, -0.951056);
    normalTable[ 71] = Vector3(-0.716567, -0.681718, -0.147621);
    normalTable[ 72] = Vector3(-0.500000, -0.809017, -0.309017);
    normalTable[ 73] = Vector3(-0.688191, -0.587785, -0.425325);
    normalTable[ 74] = Vector3(-0.238856, -0.864188, -0.442863);
    normalTable[ 75] = Vector3(-0.425325, -0.688191, -0.587785);
    normalTable[ 76] = Vector3(-0.587785, -0.425325, -0.688191);
    normalTable[ 77] = Vector3(-0.716567, -0.681718, 0.147621);
    normalTable[ 78] = Vector3(-0.500000, -0.809017, 0.309017);
    normalTable[ 79] = Vector3(-0.525731, -0.850651, 0.000000);
    normalTable[ 80] = Vector3(0.000000, -0.850651, 0.525731);
    normalTable[ 81] = Vector3(-0.238856, -0.864188, 0.442863);
    normalTable[ 82] = Vector3(0.000000, -0.955423, 0.295242);
    normalTable[ 83] = Vector3(-0.262866, -0.951056, 0.162460);
    normalTable[ 84] = Vector3(0.000000, -1.000000, 0.000000);
    normalTable[ 85] = Vector3(0.000000, -0.955423, -0.295242);
    normalTable[ 86] = Vector3(-0.262866, -0.951056, -0.162460);
    normalTable[ 87] = Vector3(0.238856, -0.864188, 0.442863);
    normalTable[ 88] = Vector3(0.500000, -0.809017, 0.309017);
    normalTable[ 89] = Vector3(0.262866, -0.951056, 0.162460);
    normalTable[ 90] = Vector3(0.850651, -0.525731, 0.000000);
    normalTable[ 91] = Vector3(0.716567, -0.681718, 0.147621);
    normalTable[ 92] = Vector3(0.716567, -0.681718, -0.147621);
    normalTable[ 93] = Vector3(0.525731, -0.850651, 0.000000);
    normalTable[ 94] = Vector3(0.500000, -0.809017, -0.309017);
    normalTable[ 95] = Vector3(0.238856, -0.864188, -0.442863);
    normalTable[ 96] = Vector3(0.262866, -0.951056, -0.162460);
    normalTable[ 97] = Vector3(0.864188, -0.442863, -0.238856);
    normalTable[ 98] = Vector3(0.809017, -0.309017, -0.500000);
    normalTable[ 99] = Vector3(0.688191, -0.587785, -0.425325);
    normalTable[100] = Vector3(0.681718, -0.147621, -0.716567);
    normalTable[101] = Vector3(0.587785, -0.425325, -0.688191);
    normalTable[102] = Vector3(0.425325, -0.688191, -0.587785);
    normalTable[103] = Vector3(0.955423, -0.295242, 0.000000);
    normalTable[104] = Vector3(1.000000, 0.000000, 0.000000);
    normalTable[105] = Vector3(0.951056, -0.162460, -0.262866);
    normalTable[106] = Vector3(0.850651, 0.525731, 0.000000);
    normalTable[107] = Vector3(0.955423, 0.295242, 0.000000);
    normalTable[108] = Vector3(0.864188, 0.442863, -0.238856);
    normalTable[109] = Vector3(0.951056, 0.162460, -0.262866);
    normalTable[110] = Vector3(0.809017, 0.309017, -0.500000);
    normalTable[111] = Vector3(0.681718, 0.147621, -0.716567);
    normalTable[112] = Vector3(0.850651, 0.000000, -0.525731);
    normalTable[113] = Vector3(0.864188, -0.442863, 0.238856);
    normalTable[114] = Vector3(0.809017, -0.309017, 0.500000);
    normalTable[115] = Vector3(0.951056, -0.162460, 0.262866);
    normalTable[116] = Vector3(0.525731, 0.000000, 0.850651);
    normalTable[117] = Vector3(0.681718, -0.147621, 0.716567);
    normalTable[118] = Vector3(0.681718, 0.147621, 0.716567);
    normalTable[119] = Vector3(0.850651, 0.000000, 0.525731);
    normalTable[120] = Vector3(0.809017, 0.309017, 0.500000);
    normalTable[121] = Vector3(0.864188, 0.442863, 0.238856);
    normalTable[122] = Vector3(0.951056, 0.162460, 0.262866);
    normalTable[123] = Vector3(0.442863, 0.238856, 0.864188);
    normalTable[124] = Vector3(0.309017, 0.500000, 0.809017);
    normalTable[125] = Vector3(0.587785, 0.425325, 0.688191);
    normalTable[126] = Vector3(0.147621, 0.716567, 0.681718);
    normalTable[127] = Vector3(0.238856, 0.864188, 0.442863);
    normalTable[128] = Vector3(0.425325, 0.688191, 0.587785);
    normalTable[129] = Vector3(0.500000, 0.809017, 0.309017);
    normalTable[130] = Vector3(0.716567, 0.681718, 0.147621);
    normalTable[131] = Vector3(0.688191, 0.587785, 0.425325);
    normalTable[132] = Vector3(0.262866, 0.951056, 0.162460);
    normalTable[133] = Vector3(0.238856, 0.864188, -0.442863);
    normalTable[134] = Vector3(0.262866, 0.951056, -0.162460);
    normalTable[135] = Vector3(0.500000, 0.809017, -0.309017);
    normalTable[136] = Vector3(0.716567, 0.681718, -0.147621);
    normalTable[137] = Vector3(0.525731, 0.850651, 0.000000);
    normalTable[138] = Vector3(0.688191, 0.587785, -0.425325);
    normalTable[139] = Vector3(0.425325, 0.688191, -0.587785);
    normalTable[140] = Vector3(0.587785, 0.425325, -0.688191);
    normalTable[141] = Vector3(-0.295242, 0.000000, 0.955423);
    normalTable[142] = Vector3(-0.162460, 0.262866, 0.951056);
    normalTable[143] = Vector3(0.000000, 0.000000, 1.000000);
    normalTable[144] = Vector3(0.000000, 0.525731, 0.850651);
    normalTable[145] = Vector3(0.295242, 0.000000, 0.955423);
    normalTable[146] = Vector3(0.162460, 0.262866, 0.951056);
    normalTable[147] = Vector3(-0.442863, -0.238856, 0.864188);
    normalTable[148] = Vector3(-0.162460, -0.262866, 0.951056);
    normalTable[149] = Vector3(-0.309017, -0.500000, 0.809017);
    normalTable[150] = Vector3(0.442863, -0.238856, 0.864188);
    normalTable[151] = Vector3(0.162460, -0.262866, 0.951056);
    normalTable[152] = Vector3(0.309017, -0.500000, 0.809017);
    normalTable[153] = Vector3(-0.147621, -0.716567, 0.681718);
    normalTable[154] = Vector3(0.147621, -0.716567, 0.681718);
    normalTable[155] = Vector3(0.000000, -0.525731, 0.850651);
    normalTable[156] = Vector3(-0.587785, -0.425325, 0.688191);
    normalTable[157] = Vector3(-0.425325, -0.688191, 0.587785);
    normalTable[158] = Vector3(-0.688191, -0.587785, 0.425325);
    normalTable[159] = Vector3(0.688191, -0.587785, 0.425325);
    normalTable[160] = Vector3(0.425325, -0.688191, 0.587785);
    normalTable[161] = Vector3(0.587785, -0.425325, 0.688191);
}

}
