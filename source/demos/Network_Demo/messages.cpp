/**
  @file Network_Demo/messages.cpp

  @author Morgan McGuire, matrix@graphics3d.com
  @created 2004-03-30
  @edited  2004-03-30
 */

#include "messages.h"
#include "Entity.h"


void CreateEntityMessage::serialize(BinaryOutput& b) const {
    debugAssert(entity);
    entity->serialize(b);
}


void CreateEntityMessage::deserialize(BinaryInput& b) {
    debugAssert(entity);
    entity->deserialize(b);
}


void EntityStateMessage::serialize(BinaryOutput& b) const {
    b.writeInt32(id);
    controls.serialize(b);
    frame.serialize(b);
    velocity.serialize(b);
}


void EntityStateMessage::deserialize(BinaryInput& b) {
    id = b.readInt32();
    controls.deserialize(b);
    frame.deserialize(b);
    velocity.deserialize(b);
}
