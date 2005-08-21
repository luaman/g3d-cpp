#ifndef MODELDATA_H
#define MODELDATA_H

#include <G3DAll.h>
#include "ASFModel.h"

/**
 A wrapper that pairs various model classes and their pose information.
 1:1 relationship with Entity.  
 */
// Implementation could use inheritance, but then we'd just have to add down-casts
// everywhere in the code that uses it.
class ModelData {
protected:

    ModelData();

public:
    /** There is no IFSModel support; use ArticulatedModel to load IFS and PLY2 files. */
    enum Type {ASF = 1, ARTICULATED, MD2};

    /** Indicates which of the ___model fields is in use. */
    Type                        type;

    ArticulatedModelRef         artmodel;
    ArticulatedModel::Pose      artpose;

    ASFModelRef                 asfmodel;

    MD2ModelRef                 md2model;
    MD2Model::Pose              md2pose;
    GMaterial                   md2material;

    virtual ~ModelData();

    void drawLabels(RenderDevice* rd, const CoordinateFrame& cframe) const;

    static ModelData* create(const ArticulatedModelRef& model);

    static ModelData* create(const ASFModelRef& model);

    static ModelData* create(const MD2ModelRef& model);

    virtual void pose(Array<PosedModelRef>& posedModels, const CoordinateFrame& cframe);
};

#endif
