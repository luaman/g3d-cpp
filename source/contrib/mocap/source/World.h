#ifndef WORLD_H
#define WORLD_H

#include <G3DAll.h>
#include "Entity.h"


class World {
public:

    SkyRef                      sky;
    LightingParameters          skyParameters;

    LightingRef                 lighting;

    Array<EntityRef>            entityArray;

    /** Set up the empty world */
    void init();

    void render(RenderDevice* rd);
};


#endif
