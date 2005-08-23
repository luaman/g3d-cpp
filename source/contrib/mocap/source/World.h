#ifndef WORLD_H
#define WORLD_H

#include <G3DAll.h>
#include "Entity.h"

class World {
    ToneMap                         toneMap;

    class Physics {
    public:
        /** Subset of entities that receive physics */
        Array<EntityRef>            simArray;

        dWorldID                    ID;
        dSpaceID                    spaceID;
        dJointGroupID               contactGroup;

        Vector3                     gravity;

        Physics();
    };

    Physics                         physics;

    Array<EntityRef>                entityArray;

    /** Called from doGraphics for debugging purposes.  Modifies sort keys */
    void renderPhysicsModels(RenderDevice* rd);

    /** Invoked by ODE when objects o1 and o2 are in contact or colliding. */
    static void ODENearCallback(void *data, dGeomID o1, dGeomID o2);

public:

    enum RenderMode {RENDER_NORMAL = 0, RENDER_PHYSICS, RENDER_MAX};
    RenderMode                      renderMode;

    SkyRef                          sky;
    LightingParameters              skyParameters;

    LightingRef                     lighting;

    ~World();


    void setGravity(const Vector3& g);

    inline Vector3 gravity() const {
        return physics.gravity;
    }

    /** Insert this new entity into the world.*/
    void insert(EntityRef& e);

    /** Set up the empty world */
    void init();

    void cleanup();

    void doSimulation();

    void doGraphics(RenderDevice* rd);
};

#endif
