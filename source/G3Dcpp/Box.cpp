/**
  @file Box.cpp
  Box class

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2001-06-02
  @edited  2003-12-22
*/

#include "G3D/Box.h"
#include "G3D/debug.h"
#include "G3D/Plane.h"

namespace G3D {

/**
 Sets a field on four vertices.  Used by the constructor.
 */
#define setMany(i0, i1, i2, i3, field, extreme) \
    _corner[i0].field = _corner[i1].field = \
    _corner[i2].field = _corner[i3].field = \
    (extreme).field

Box::Box() {
}


Box::Box(class BinaryInput& b) {
	deserialize(b);	
}


void Box::serialize(class BinaryOutput& b) const {
	int i;
	for (i = 0; i < 8; ++i) {
		_corner[i].serialize(b);
	}

    // Other state can be reconstructed
}


void Box::deserialize(class BinaryInput& b) {
	int i;

    _center = Vector3::ZERO;
    for (i = 0; i < 8; ++i) {
		_corner[i].deserialize(b);
        _center += _corner[i];
	}

    _center = _center / 8;
    
    // Reconstruct other state from the corners
    _axis[0] = _corner[5] - _corner[4];
    _axis[1] = _corner[7] - _corner[4];
    _axis[2] = _corner[0] - _corner[4];

    for (i = 0; i < 3; ++i) {
        _extent[i] = _axis[i].length();
        _axis[i] /= _extent[i];
    }

    _volume = _extent.x * _extent.y * _extent.z;

    _area = 2 * 
        (_extent.x * _extent.y +
         _extent.y * _extent.z +
         _extent.z * _extent.x);
}


Box::Box(
    const Vector3& min,
    const Vector3& max) {

    setMany(0, 1, 2, 3, z, max);
    setMany(4, 5, 6, 7, z, min);

    setMany(1, 2, 5, 6, x, max);
    setMany(0, 3, 4, 7, x, min);

    setMany(3, 2, 6, 7, y, max);
    setMany(0, 1, 5, 4, y, min);

    _extent = max - min;

    _axis[0] = Vector3::UNIT_X;
    _axis[1] = Vector3::UNIT_Y;
    _axis[2] = Vector3::UNIT_Z;

    _volume = _extent.x * _extent.y * _extent.z;
    _area = 2 * 
        (_extent.x * _extent.y +
         _extent.y * _extent.z +
         _extent.z * _extent.x);

    _center = (max + min) / 2;
}


double Box::volume() const {
    return _volume;
}


double Box::surfaceArea() const {
    return _area;
}


void Box::getLocalFrame(CoordinateFrame& frame) const {

    frame.rotation = Matrix3(
        _axis[0][0], _axis[1][0], _axis[2][0],
        _axis[0][1], _axis[1][1], _axis[2][1],
        _axis[0][2], _axis[1][2], _axis[2][2]);

    frame.translation = _center;
}


CoordinateFrame Box::localFrame() const {
    CoordinateFrame out;
    getLocalFrame(out);
    return out;
}


void Box::getFaceCorners(int f, Vector3& v0, Vector3& v1, Vector3& v2, Vector3& v3) const {
    switch (f) {
    case 0:
        v0 = _corner[0]; v1 = _corner[1]; v2 = _corner[2]; v3 = _corner[3];
        break;

    case 1:
        v0 = _corner[1]; v1 = _corner[5]; v2 = _corner[6]; v3 = _corner[2];
        break;

    case 2:
        v0 = _corner[7]; v1 = _corner[6]; v2 = _corner[5]; v3 = _corner[4];
        break;

    case 3:
        v0 = _corner[2]; v1 = _corner[6]; v2 = _corner[7]; v3 = _corner[3];
        break;

    case 4:
        v0 = _corner[3]; v1 = _corner[7]; v2 = _corner[4]; v3 = _corner[0];
        break;

    case 5:
        v0 = _corner[1]; v1 = _corner[0]; v2 = _corner[4]; v3 = _corner[5];
        break;

    default:
        debugAssert((f >= 0) && (f < 6));
    }
}


bool Box::culledBy(const Plane* plane, int numPlanes) const {

    // See if there is one plane for which all
    // of the vertices are on the wrong side
    for (int p = 0; p < numPlanes; p++) {
        bool culled = true;
        int v = 0;

        // Assume this plane culls all points.  See if there is a point
        // not culled by the plane.
        while ((v < 8) && culled) {
            culled = !plane[p].halfSpaceContains(_corner[v]);
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



bool Box::contains(
    const Vector3&      point) const {

    // Form axes from three edges, transform the point into that
    // space, and perform 3 interval tests

    Vector3 u = _corner[4] - _corner[0];
    Vector3 v = _corner[3] - _corner[0];
    Vector3 w = _corner[1] - _corner[0];

    Matrix3 M = Matrix3(u.x, v.x, w.x,
                        u.y, v.y, w.y,
                        u.z, v.z, w.z);

    // M^-1 * (point - _corner[0]) = point in unit cube's object space
    // compute the inverse of M
    Vector3 osPoint = M.inverse() * (point - _corner[0]);

    return
        (osPoint.x >= 0) && 
        (osPoint.y >= 0) &&
        (osPoint.z >= 0) &&
        (osPoint.x <= 1) &&
        (osPoint.y <= 1) &&
        (osPoint.z <= 1);
}

#undef setMany

} // namespace
