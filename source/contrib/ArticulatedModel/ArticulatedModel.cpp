#include "ArticulatedModel.h"

const ArticulatedModel::Pose ArticulatedModel::DEFAULT_POSE;

ArticulatedModelRef ArticulatedModel::fromFile(const std::string& filename) {
    ArticulatedModel* model = new ArticulatedModel();
    model->init(filename);
    return model;
}


void ArticulatedModel::init(const std::string& filename) {
    TextureManager textureManager;

    std::string path = filenamePath(filename);
    load.load(filename);

    partArray.resize(load.objectArray.size());
    for (int p = 0; p < load.objectArray.size(); ++p) {
        const Load3DS::Object& object = load.objectArray[p];
        Part& part = partArray[p];
        part.vertexArray = object.vertexArray;
        part.indexArray = object.indexArray;
        part.texCoordArray = object.texCoordArray;

        std::string name = object.name;
        int count = 0;
        while (partNameToIndex.containsKey(name)) {
            ++count;
            name = object.name + format("_#%d", count);
        }
        part.keyframe = object.keyframe.approxCoordinateFrame();
        part.name = name;
        partNameToIndex.set(part.name, p);

        if (object.faceMatArray.size() > 0) {
            const std::string& materialName = object.faceMatArray[0].materialName;

            if (load.materialNameToIndex.containsKey(materialName)) {
                int i = load.materialNameToIndex[materialName];
                const Load3DS::Material& material = load.materialArray[i];

                const Load3DS::Map& texture1 = material.texture1;

                std::string textureFile = texture1.filename;

                // Assume the first material applies to the entire part
                if (! fileExists(textureFile) && fileExists(path + textureFile)) {
                    textureFile = path + textureFile;
                }

                if (fileExists(textureFile)) {
                    part.texture1 = textureManager.loadTexture(textureFile);
                } else {
                    Log::common()->printf("Could not load texture '%s'\n", textureFile.c_str());
                }
            } else {
                Log::common()->printf("Referenced unknown material '%s'\n", materialName.c_str());
            }
        }
    }
}


void ArticulatedModel::render(RenderDevice* rd, const Pose& pose) {
    for (int p = 0; p < partArray.size(); ++p) {
        const Part& part = partArray[p];
        part.render(rd, CoordinateFrame(), pose);
    }
}


void ArticulatedModel::Part::render(
    RenderDevice*           rd,
    const CoordinateFrame&  parent, 
    const Pose&             pose) const {

    app->debugPrintf("%s", name.c_str());

    CoordinateFrame frame;

    if (pose.cframe.containsKey(name)) {
        frame = parent * keyframe * pose.cframe[name] * keyframe.inverse();
    } else {
        frame = parent;
    }

    rd->setObjectToWorldMatrix(frame);
    rd->setTexture(0, texture1);

    rd->beginPrimitive(RenderDevice::TRIANGLES);
    for (int i = 0; i < indexArray.size(); ++i) {
        int v = indexArray[i];
        if (texCoordArray.size() > 0) {
            rd->setTexCoord(0, texCoordArray[v]);
        }
        rd->sendVertex(vertexArray[v]);
    }
    rd->endPrimitive();

    // TODO: sub-parts
}

