/**
  @file AABox.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2004-01-10
  @edited  2004-01-10
*/

#include "G3D/AABox.h"
#include "G3D/Box.h"
#include "G3D/Plane.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"

namespace G3D {

Box AABox::toBox() const {
    return Box(lo, hi);
}


void AABox::serialize(class BinaryOutput& b) const {
    b.writeVector3(lo);
    b.writeVector3(hi);
}


void AABox::deserialize(class BinaryInput& b) {
    lo = b.readVector3();
    hi = b.readVector3();
}


Vector3 AABox::randomSurfacePoint() const {
    Vector3 extent = hi - lo;
    double aXY = extent.x * extent.y;
    double aYZ = extent.y * extent.z;
    double aZX = extent.z * extent.x;

    double r = random(0, aXY + aYZ + aZX);

    // Choose evenly between positive and negative face planes
    double d = (random(0, 1) < 0.5) ? 0 : 1;

    // The probability of choosing a given face is proportional to
    // its area.
    if (r < aXY) {
        return 
            lo + 
            Vector3(
                random(0, extent.x),
                random(0, extent.y),
                d * extent.z);
    } else if (r < aYZ) {
        return 
            lo + 
            Vector3(
                d * extent.x,
                random(0, extent.y),
                random(0, extent.z));
    } else {
        return 
            lo + 
            Vector3(
                random(0, extent.x),
                d * extent.y,
                random(0, extent.z));
    }
}


Vector3 AABox::randomInteriorPoint() const {
    return Vector3(random(lo.x, hi.x), random(lo.y, hi.y), random(lo.z, hi.z));
}


bool AABox::intersects(const AABox& other) const {
    // Must be overlap along all three axes.
    // Try to find a separating axis.

    for (int a = 0; a < 3; ++a) {

        //     |--------|
        // |------|

        if ((lo[a] > other.hi[a]) ||
            (hi[a] < other.lo[a])) {
            return false;
        }
    }

    return true;
}



bool AABox::culledBy(
    const class Plane*  plane,
    int                 numPlanes) const {

    // See if there is one plane for which all
    // of the vertices are on the wrong side
    for (int p = 0; p < numPlanes; p++) {
        bool culled = true;
        int v = 0;

        // Assume this plane culls all points.  See if there is a point
        // not culled by the plane.
        while ((v < 8) && culled) {

            Vector3 corner;

            corner.x = ((v & 1) == 0) ? lo.x : hi.x;
            corner.y = ((v & 2) == 0) ? lo.y : hi.y;
            corner.z = ((v & 4) == 0) ? lo.z : hi.z;
            
            culled = ! plane[p].halfSpaceContains(corner);
            v++;
        }

        if (culled) {
            // Plane p culled the box
            return true;
        }
    }

    // None of the planes could cull this box
    return false;
}

} // namespace
