/**
 @file edgeFeatures.h
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2004-04-20
 @edited  2004-04-20
 */

#ifndef G3D_EDGEFEATURES_H
#define G3D_EDGEFEATURES_H

#include "graphics3D.h"
#include "GLG3D/PosedModel.h"

namespace G3D {

/**
 Renders boundaries, contours, and sharp creases as lines. 
 The caller should set the line width, color, and enable
 alpha blending (for smooth lines).

 Primarily useful for cartoon rendering.

 <B>BETA API</B> -- subject to change.
 */
void drawEdgeFeatures(class RenderDevice* renderDevice, const PosedModelRef& model);

}

#endif
