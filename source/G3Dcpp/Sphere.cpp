/**
 @file Sphere.cpp
 
 Sphere class
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2001-04-17
 @edited  2003-10-04
 */

#include "G3D/Sphere.h"
#include "G3D/stringutils.h"
#include "G3D/Plane.h"
#include "G3D/BinaryOutput.h"
#include "G3D/BinaryInput.h"

namespace G3D {

Sphere::Sphere(class BinaryInput& b) {
	deserialize(b);
}


void Sphere::serialize(class BinaryOutput& b) const {
	center.serialize(b);
	b.writeFloat64(radius);
}


void Sphere::deserialize(class BinaryInput& b) {
	center.deserialize(b);
	radius = b.readFloat64();
}


std::string Sphere::toString() const {
    return format("Sphere(<%g, %g, %g>, %g)", 
        center.x, center.y, center.z, radius);
}


bool Sphere::contains(const Vector3& point) const {
    double distance = (center - point).squaredLength();
    return distance <= (radius * radius);
}


bool Sphere::culledBy(
    const Plane*        plane,
    int                 numPlanes) const {

    // Try to find one plane that culls the sphere
    for (int p = 0; p < numPlanes; ++p) {
        // Shift the center towards the plane by the
        // radius.
        if (! plane[p].halfSpaceContains(center + radius * plane[p].normal())) {
            return true;
        }
    }

    return false;
}


Vector3 Sphere::randomSurfacePoint() const {
    return Vector3::random() * radius + center;
}


Vector3 Sphere::randomInteriorPoint() const {
    Vector3 result;
    do {
        result = Vector3(symmetricRandom(), 
                         symmetricRandom(),
                         symmetricRandom());
    } while (result.squaredLength() >= 1);

    return result * radius + center;
}

} // namespace
