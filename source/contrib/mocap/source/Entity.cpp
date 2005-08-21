#include "Entity.h"
#include "ASFModel.h"

Entity::Entity() {}


Entity::~Entity() {
    delete modelData;
}


void Entity::pose(Array<PosedModelRef>& posedModels) {
    modelData->pose(posedModels, cframe);
}


void Entity::drawLabels(RenderDevice* rd) {
    modelData->drawLabels(rd, CoordinateFrame(cframe.rotation, cframe.translation + Vector3(0,0,-.25)));
}

