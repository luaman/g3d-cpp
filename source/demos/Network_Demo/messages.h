/**
  @file Network_Demo/messages.h

  Messages sent between client and server.

  @author Morgan McGuire, matrix@graphics3d.com
  @created 2004-03-29
  @edited  2004-03-30
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include <G3DAll.h>
#include "Entity.h"

enum MessageType {
    NO_MSG                  = 0,
    SignOnMessage_MSG       = 2000,
    CreateEntityMessage_MSG = 2001,
    EntityStateMessage_MSG  = 2002,
};


/**
 Sent from the server to the client to tell them their ID.
 */
class SignOnMessage : public NetMessage {
public:
    /**
     The ID assigned to the player.
     */
    ID                                  id;

    SignOnMessage() : id(NO_ID) {}
    SignOnMessage(ID _id) : id(_id) {}

    virtual uint32 type() const {
        return SignOnMessage_MSG;
    }

    virtual void serialize(BinaryOutput& b) const {
        b.writeInt32(id);
    }

    virtual void deserialize(BinaryInput& b) {
        id = b.readInt32();
    }
};


class CreateEntityMessage : public NetMessage {
public:

    /** On the sending side, set this to point to 
        the entity before sending.  On the receiving
        side, point into an entity that will receive
        the data before receiving; an entity will <B>not</B>
        be allocated for you.  This is not deleted on
        destruction of the message class. */
    class Entity*                 entity;

    CreateEntityMessage() : entity(NULL) {}
    CreateEntityMessage(class Entity* _e) : entity(_e) {}

    virtual uint32 type() const {
        return CreateEntityMessage_MSG;
    }

    virtual void serialize(BinaryOutput& b) const;

    virtual void deserialize(BinaryInput& b);
};


/**
 Sent from the client to the server to indicate
 new control settings, and from the server back
 to the clients to distribute that information.
 */
class EntityStateMessage : public NetMessage {
public:

    /** ID of the object for which these controls apply */
    ID                          id;
    Controls                    controls;
    EulerFrame                  frame;
    Vector3                     velocity;

    virtual uint32 type() const {
        return EntityStateMessage_MSG;
    }

    virtual void serialize(BinaryOutput& b) const;

    virtual void deserialize(BinaryInput& b);

};

#endif
