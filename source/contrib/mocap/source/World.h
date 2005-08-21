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

        Physics();
    };

    Physics                         physics;

    Array<EntityRef>                entityArray;

    /** Called from doGraphics for debugging purposes */
    void renderPhysicsModels(RenderDevice* rd) const;

public:

    enum RenderMode {RENDER_NORMAL = 0, RENDER_PHYSICS, RENDER_MAX};
    RenderMode                      renderMode;

    SkyRef                          sky;
    LightingParameters              skyParameters;

    LightingRef                     lighting;

    /** Insert this new entity into the world.*/
    void insert(EntityRef& e);

    /** Set up the empty world */
    void init();

    void doSimulation();

    void doGraphics(RenderDevice* rd);
};

#endif
