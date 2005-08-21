#ifndef ENTITY_H
#define ENTITY_H

#include <G3DAll.h>
#include "ModelData.h"
#include <ode/ode.h>


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
    PhysicsFrame                frame;

    /** Physics uses simplified models for objects.
        TODO: extend for articulated bodies to a pose 
        heirarchy in parallel to the graphics model */
    Shape*                      physicsModel;

    float                       mass;

    ~Entity();

    /** Renders labels on the sub-parts */
    void drawLabels(RenderDevice* rd);

    /** For debug */
    void renderPhysicsModel(RenderDevice* rd);

    /** Allows construction from any kind of model supported by ModelData. */
    template<class ModelTypeRef> EntityRef static create(
        const ModelTypeRef&        model,
        const PhysicsFrame&        f) {

        Entity* e = new Entity();

        e->modelData = ModelData::create(model);
        e->frame = f;

        return e;
    }


    /** Converts the PhysicsFrame to a CoordinateFrame */
    CoordinateFrame cframe() const {
        return frame.toCoordinateFrame();
    }

    virtual void pose(Array<PosedModelRef>& posedModels);
};

#endif
