/**
 @file ArticulatedModel.cpp
 @author Morgan McGuire morgan3d@sf.net
 */

#include "ArticulatedModel.h"
#include "Load3DS.h"

const ArticulatedModel::Pose ArticulatedModel::DEFAULT_POSE;

ArticulatedModelRef ArticulatedModel::fromFile(const std::string& filename, const Vector3& scale) {
    ArticulatedModel* model = new ArticulatedModel();

    if (endsWith(toLower(filename), ".3ds")) {
        model->init3DS(filename, scale);
    } else if (endsWith(toLower(filename), ".ifs") || endsWith(toLower(filename), ".ifs")) {
        model->initIFS(filename, scale);
    }

    return model;
}


void ArticulatedModel::init3DS(const std::string& filename, const Vector3& scale) {
    Load3DS                     load;

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
        part.keyframe = object.keyframe.approxCoordinateFrame();
        part.keyframe.translation *= scale;
        part.name = name;
        partNameToIndex.set(part.name, p);

debugPrintf("%s %d %d\n", object.name.c_str(), object.hierarchyIndex, object.nodeID);

        if (part.hasGeometry()) {
            for (int v = 0; v < part.geometry.vertexArray.size(); ++v) {
                part.geometry.vertexArray[v] *= scale;
            }

            part.texCoordArray = object.texCoordArray;
            MeshAlg::computeNormals(part.geometry, object.indexArray);

            VARAreaRef varArea = VARArea::create(
                sizeof(Vector3) * part.geometry.vertexArray.size() * 2 +
                sizeof(Vector2) * part.texCoordArray.size() + 32, VARArea::WRITE_ONCE);

            part.vertexVAR    = VAR(part.geometry.vertexArray, varArea);
            part.normalVAR    = VAR(part.geometry.normalArray, varArea);
            part.texCoord0VAR = VAR(part.texCoordArray, varArea);

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

                        if (! fileExists(textureFile) && fileExists(path + textureFile)) {
                            textureFile = path + textureFile;
                        }

                        if (fileExists(textureFile)) {
                            triList.material.diffuse.map = textureManager.loadTexture(textureFile);
                        } else {
                            Log::common()->printf("Could not load texture '%s'\n", textureFile.c_str());
                        }

			            triList.material.diffuse.constant = material.diffuse;
                        triList.cullFace = material.twoSided ? RenderDevice::CULL_NONE : RenderDevice::CULL_BACK;
                        triList.computeBounds(part);

                    } else {
                        Log::common()->printf("Referenced unknown material '%s'\n", materialName.c_str());
                    }
                } // for m
            } // if has materials 
        }
    }
}


void ArticulatedModel::initIFS(const std::string& filename, const Vector3& scale) {
    Array<int>   	index;
    Array<Vector3>  vertex;
    Array<Vector2>  texCoord;
    
    IFSModel::load(filename, name, index, vertex, texCoord);

    // Transform vertices
    for (int v = 0; v < vertex.size(); ++v) {
        vertex[v] *= scale;
    }

    // Convert to a part
    Part& part = partArray.next();

    part.cframe = CoordinateFrame();
    part.name = "root";
    part.geometry.vertexArray = vertex;
    part.texCoordArray = texCoord;
    MeshAlg::computeNormals(part.geometry, index);

    VARAreaRef varArea = VARArea::create(
        sizeof(Vector3) * part.geometry.vertexArray.size() * 2 +
        sizeof(Vector2) * part.texCoordArray.size() + 32, VARArea::WRITE_ONCE);

    part.vertexVAR    = VAR(part.geometry.vertexArray, varArea);
    part.normalVAR    = VAR(part.geometry.normalArray, varArea);
    part.texCoord0VAR = VAR(part.texCoordArray, varArea);

    part.keyframe = CoordinateFrame();

    Part::TriList& triList = part.triListArray.next();
    triList.indexArray = index;
    triList.cullFace = RenderDevice::CULL_BACK;
    triList.computeBounds(part);
}


void ArticulatedModel::Part::TriList::computeBounds(const Part& parentPart) {
    MeshAlg::computeBounds(parentPart.geometry.vertexArray, indexArray, boxBounds, sphereBounds);
}





