/**
 @file ArticulatedModel.cpp
 @author Morgan McGuire morgan3d@sf.net
 */

#include "ArticulatedModel.h"
#include "Load3DS.h"

const ArticulatedModel::Pose ArticulatedModel::DEFAULT_POSE;

ArticulatedModel::GraphicsProfile ArticulatedModel::profile() {
    static GraphicsProfile p = UNKNOWN;

    if (p == UNKNOWN) {
        if (GLCaps::supports_GL_ARB_shader_objects()) {
            p = PS20;

            if (! fileExists("NonShadowedPass.glsl.vrt") &&
                ! fileExists("../NonShadowedPass.glsl.vrt")) {
                p = UNKNOWN;
                Log::common()->printf("\n\nWARNING: ArticulatedModel could not enter PS20 mode because"
                    "NonShadowedPass.glsl.vrt was not found.\n\n");
            }
        }

        
        if (p == UNKNOWN) {
            if (GLCaps::supports("GL_ARB_texture_env_crossbar") &&
                GLCaps::supports("GL_ARB_texture_env_combine") &&
                GLCaps::supports("GL_EXT_texture_env_add") &&
                (GLCaps::numTextureUnits() >= 4)) {
                p = PS14;
            } else {
                p = FIXED_FUNCTION;
            }
        }
    }

    // TODO: Remove (here for testing)
    //p = FIXED_FUNCTION;

    return p;
}


ArticulatedModelRef ArticulatedModel::fromFile(const std::string& filename, const CoordinateFrame& xform) {
    ArticulatedModel* model = new ArticulatedModel();

    if (endsWith(toLower(filename), ".3ds")) {
        model->init3DS(filename, xform);
    } else if (endsWith(toLower(filename), ".ifs") || endsWith(toLower(filename), ".ply2")) {
        model->initIFS(filename, xform);
    }

    model->updateAll();

    return model;
}


ArticulatedModelRef ArticulatedModel::createEmpty() {
    return new ArticulatedModel();
}


void ArticulatedModel::init3DS(const std::string& filename, const CoordinateFrame& xform) {
    // Note: vertices are actually mutated by scale; it is not carried along as
    // part of the scene graph transformation.

    Load3DS load;

    TextureManager textureManager;

    std::string path = filenamePath(filename);
    load.load(filename);

    partArray.resize(load.objectArray.size());
    for (int p = 0; p < load.objectArray.size(); ++p) {
        const Load3DS::Object& object = load.objectArray[p];

        Part& part = partArray[p];

        // Process geometry
        part.geometry.vertexArray = object.vertexArray;
        std::string name = object.name;
        int count = 0;
        while (partNameToIndex.containsKey(name)) {
            ++count;
            name = object.name + format("_#%d", count);
        }

        part.cframe = object.keyframe.approxCoordinateFrame();
        part.cframe.translation = xform.pointToWorldSpace(part.cframe.translation);

        part.name = name;
        partNameToIndex.set(part.name, p);

        // All 3DS parts are promoted to the root in the current implementation.
        part.parent = -1;

//debugPrintf("%s %d %d\n", object.name.c_str(), object.hierarchyIndex, object.nodeID);

        if (part.hasGeometry()) {

            // Convert to object space (there is no normal data at this point)
            debugAssert(part.geometry.normalArray.size() == 0);
            for (int v = 0; v < part.geometry.vertexArray.size(); ++v) {
                part.geometry.vertexArray[v] = part.cframe.pointToObjectSpace(
                    xform.pointToWorldSpace(part.geometry.vertexArray[v]));
            }

            part.texCoordArray = object.texCoordArray;

            if (object.faceMatArray.size() == 0) {

                // Lump everything into one part
                Part::TriList& triList = part.triListArray.next();
                triList.indexArray = object.indexArray;
                triList.computeBounds(part);

            } else {
                for (int m = 0; m < object.faceMatArray.size(); ++m) {
                    const Load3DS::FaceMat& faceMat = object.faceMatArray[m];

                    Part::TriList& triList = part.triListArray.next();
            
                    // Construct an index array for this part
                    for (int i = 0; i < faceMat.faceIndexArray.size(); ++i) {
                        // 3*f is an index into object.indexArray
                        int f = faceMat.faceIndexArray[i];
                        for (int v = 0; v < 3; ++v) {
                            triList.indexArray.append(object.indexArray[3*f + v]);
                        }
                    }                                

                    const std::string& materialName = faceMat.materialName;

                    if (load.materialNameToIndex.containsKey(materialName)) {
                        int i = load.materialNameToIndex[materialName];
                        const Load3DS::Material& material = load.materialArray[i];

                        const Load3DS::Map& texture1 = material.texture1;

                        std::string textureFile = texture1.filename;

                        if (texture1.filename != "") {
                            if (endsWith(toUpper(textureFile), "GIF")) {
                                // Load PNG instead of GIF, since we can't load GIF
                                textureFile = textureFile.substr(0, textureFile.length() - 3) + "png";
                            }

                            if (! fileExists(textureFile) && fileExists(path + textureFile)) {
                                textureFile = path + textureFile;
                            }

                            if (fileExists(textureFile)) {
                                triList.material.diffuse.map = textureManager.loadTexture(textureFile);
                            } else {
                                Log::common()->printf("Could not load texture '%s'\n", textureFile.c_str());
                            }
                        }

			            triList.material.diffuse.constant = material.diffuse;
                        triList.twoSided = material.twoSided;
                        triList.computeBounds(part);

                    } else {
                        Log::common()->printf("Referenced unknown material '%s'\n", materialName.c_str());
                    }
                } // for m
            } // if has materials 
        }
    }
}


void ArticulatedModel::Part::computeNormalsAndTangentSpace() {
    Array<MeshAlg::Face>    faceArray;
    Array<MeshAlg::Vertex>  vertexArray;
    Array<MeshAlg::Edge>    edgeArray;
    Array<Vector3>          faceNormalArray;

    MeshAlg::computeAdjacency(geometry.vertexArray, indexArray, faceArray, edgeArray, vertexArray);

    MeshAlg::computeNormals(geometry.vertexArray, faceArray, vertexArray, 
                   geometry.normalArray, faceNormalArray);

    // Compute a tangent space basis
    if (texCoordArray.size() > 0) {
        // We throw away the binormals and recompute
        // them in the vertex shader.
        Array<Vector3> empty;
        MeshAlg::computeTangentSpaceBasis(
            geometry.vertexArray,
            texCoordArray,
            geometry.normalArray,
            faceArray,
            tangentArray,
            empty);
        
    }
}


void ArticulatedModel::Part::updateVAR() {
    if (geometry.vertexArray.size() == 0) {
        // Has no geometry
        return;
    }

    size_t vtxSize = sizeof(Vector3) * geometry.vertexArray.size();
    size_t texSize = sizeof(Vector2) * texCoordArray.size();
    size_t tanSize = sizeof(Vector3) * tangentArray.size();

    if ((vertexVAR.maxSize() >= vtxSize) &&
        (normalVAR.maxSize() >= vtxSize) &&
        ((tanSize == 0) || (tangentVAR.maxSize() >= tanSize)) &&
        ((texSize == 0) || (texCoord0VAR.maxSize() >= texSize))) {
        
        // Update existing VARs
        vertexVAR.update(geometry.vertexArray);
        normalVAR.update(geometry.normalArray);

        if (texCoordArray.size() > 0) {
            texCoord0VAR.update(texCoordArray);
        }

        if (tangentArray.size() > 0) {
            tangentVAR.update(tangentArray);
        }

    } else {

        // Allocate new VARs
        VARAreaRef varArea = VARArea::create(vtxSize * 2 + texSize + tanSize + 32, VARArea::WRITE_ONCE);
        vertexVAR    = VAR(geometry.vertexArray, varArea);
        normalVAR    = VAR(geometry.normalArray, varArea);
        texCoord0VAR = VAR(texCoordArray, varArea);
        tangentVAR   = VAR(tangentArray, varArea);
    }
}


void ArticulatedModel::Part::updateShaders() {
    if (ArticulatedModel::profile() != PS20) {
        return;
    }

    for (int t = 0; t < triListArray.size(); ++t) {
        SuperShader::createShaders(
            triListArray[t].material,
            triListArray[t].nonShadowedShader, 
            triListArray[t].shadowMappedShader);
    }
}


void ArticulatedModel::updateAll() {
    for (int p = 0; p < partArray.size(); ++p) {
        Part& part = partArray[p];
        part.computeIndexArray();
        part.computeNormalsAndTangentSpace();
        part.updateVAR();
        part.updateShaders();
    }
}


void ArticulatedModel::initIFS(const std::string& filename, const CoordinateFrame& xform) {
    Array<int>   	index;
    Array<Vector3>  vertex;
    Array<Vector2>  texCoord;
    
    IFSModel::load(filename, name, index, vertex, texCoord);

    // Transform vertices
    for (int v = 0; v < vertex.size(); ++v) {
        vertex[v] = xform.pointToWorldSpace(vertex[v]);
    }

    // Convert to a Part
    Part& part = partArray.next();

    part.cframe = CoordinateFrame();
    part.name = "root";
    part.parent = -1;
    part.geometry.vertexArray = vertex;
    part.texCoordArray = texCoord;

    Part::TriList& triList = part.triListArray.next();
    triList.indexArray = index;
    triList.computeBounds(part);
}


void ArticulatedModel::Part::TriList::computeBounds(const Part& parentPart) {
    MeshAlg::computeBounds(parentPart.geometry.vertexArray, indexArray, boxBounds, sphereBounds);
}


void ArticulatedModel::Part::computeIndexArray() {
    indexArray.clear();
    for (int t = 0; t < triListArray.size(); ++t) {
        indexArray.append(triListArray[t].indexArray);
    }
}


const char* toString(ArticulatedModel::GraphicsProfile p) {
    switch (p) {
    case ArticulatedModel::UNKNOWN:
        return "Unknown";

    case ArticulatedModel::FIXED_FUNCTION:
        return "Fixed Function";

    case ArticulatedModel::PS14:
        return "PS 1.4";

    case ArticulatedModel::PS20:
        return "PS 2.0";

    default:
        return "Error!";
    }
}

