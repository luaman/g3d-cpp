/**
  @file Box.h
 
  Box class
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @cite Portions based on Dave Eberly's Magic Software Library at <A HREF="http://www.magic-software.com">http://www.magic-software.com</A>
  @created 2001-06-02
  @edited  2003-12-22

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.

 */

#ifndef G3D_BOX_H
#define G3D_BOX_H

#include "G3D/Vector3.h"
#include "G3D/CoordinateFrame.h"

namespace G3D {

/**
 An arbitrary 3D box, useful as a bounding box. 
 @author Morgan McGuire and Laura Wollstadt, graphics3d.com
 */
class Box {
private:

    friend class CoordinateFrame;

    /**
      <PRE>
       3    2       7    6
    
       0    1       4    5

       front    back (seen through front)
      </PRE>
     */
    Vector3 _corner[8];

    /**
     Unit axes.
     */
    Vector3 _axis[3];
   
    Vector3 _center;

    /**
     Extent along each axis.
     */
    Vector3 _extent;

    double  _area;
    double  _volume;

public:

    /**
     Does not initialize the fields.
     */
    Box();

    /**
      Constructs a box from two opposite corners.
     */
    Box(
        const Vector3&      min,
        const Vector3&      max);

	Box(class BinaryInput& b);

	void serialize(class BinaryOutput& b) const;
	void deserialize(class BinaryInput& b);

    virtual ~Box() {}

    /**
      Returns the centroid of the box.
     */
    inline Vector3 center() const {
        return _center;
    }

    inline Vector3 getCenter() const {
        return center();
    }

    /**
     Returns a corner (0 <= i < 8)
     @deprecated
     */
    inline Vector3 getCorner(int i) const {
        debugAssert(i < 8);
        return _corner[i];
    }

    inline Vector3 corner(int i) const {
        debugAssert(i < 8);
        return _corner[i];
    }

    /**
     Unit length.
     */
    inline Vector3 axis(int a) const {
        debugAssert(a < 3);
        return _axis[a];
    }

    /**
     Distance from corner(0) to the next corner along axis a.
     */
    inline float extent(int a) const {
        debugAssert(a < 3);
        return _extent[a];
    }

    /**
     Returns the four corners of a face (0 <= f < 6).
     The corners are returned to form a counter clockwise quad facing outwards.
     */
    void getFaceCorners(
        int                 f,
        Vector3&            v0,
        Vector3&            v1,
        Vector3&            v2,
        Vector3&            v3) const;

    /**
     Returns true if this box is culled by the provided set of 
     planes.  The box is culled if there exists at least one plane
     whose halfspace the entire box is not in.
     */
    bool culledBy(
        const class Plane*  plane,
        int                 numPlanes) const;

    bool contains(
        const Vector3&      point) const;

    double surfaceArea() const;
    double volume() const;
};

}

#endif
