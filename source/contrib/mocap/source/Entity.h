#ifndef ENTITY_H
#define ENTITY_H

#include <G3DAll.h>
#include "ModelData.h"

#include "CollisionProxy.h"

typedef ReferenceCountedPointer<class Entity> EntityRef;
/** An object in the world that is simulated and poseable. Its geometry is 
    described by the ModelData class. */
class Entity : public ReferenceCountedObject {
private:

    Entity();

public:

    /** The Entity creates this modelData on creation and deletes it on destruction. 
        Note that pose information is contained in the modelData. */
    ModelData*                  modelData;

    /** Root frame */
    CoordinateFrame             cframe;

    ~Entity();

    /** Renders labels on the sub-parts */
    void drawLabels(RenderDevice* rd);

    /** Allows construction from any kind of model supported by ModelData. */
    template<class ModelTypeRef> EntityRef static create(
        const ModelTypeRef&        model,
        const CoordinateFrame&     c) {

        Entity* e = new Entity();

        e->modelData = ModelData::create(model);
        e->cframe = c;

        return e;
    }

    virtual void pose(Array<PosedModelRef>& posedModels);
};

#endif
