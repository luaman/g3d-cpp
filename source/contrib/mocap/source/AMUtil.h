/**
  Helper methods for ArticulatedModels.
 */

#ifndef AMUtil_h
#define AMUtil_h

#include <G3DAll.h>
#include "World.h"

/**
 Creates a two-sided, diffuse-textured ground plane containing many polygons for good vertex lighting
 and depth interpolation.
 */
ArticulatedModelRef createPlaneModel(const std::string& textureFile, float side = 10, float tilePeriod = 1);

/**
 Creates a stick-figure from an animated skeleton file.  The joints are named
 and ready for animation.
 */
ArticulatedModelRef createASFModel(const std::string& filename);

SuperShader::Material brass();

// TODO: Append another tri-list onto this one.

/** Creates a cone with ellipsoids at each end and sets it into the part.  
    The cone consists of three pieces: a top, a bottom, and the sides.
    The top has radius r1, the bottom has radius r2.
    The centers of the top and bottom are at c1 and c2.  Note that
    the top and bottom faces are always in planes of constant z and 
    are not necessarily perpendicular to the cone axis.
 */
void conoid(
    const Vector3&              c1,
    const Vector2&              r1,
    const Vector3&              c2,
    const Vector2&              r2,
    const Vector3&              direction,
    const SuperShader::Material& material,
    ArticulatedModel::Part&     part);

#endif

