/**
  @file ConvexPolyhedron.h
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2001-11-11
  @edited  2006-01-10
 
  Copyright 2000-2006, Morgan McGuire.
  All rights reserved.
 */

#ifndef G3D_CONVEXPOLYHEDRON_H
#define G3D_CONVEXPOLYHEDRON_H

#include "G3D/platform.h"
#include "G3D/Vector3.h"
#include "G3D/CoordinateFrame.h"
#include "G3D/Plane.h"
#include "G3D/Line.h"
#include "G3D/Array.h"

namespace G3D {

class DirectedEdge {
public:
    Vector3 start;
    Vector3 stop;
};

class ConvexPolygon {
private:

	friend class ConvexPolyhedron;

    Array<Vector3>			_vertex;

public:

    ConvexPolygon() {}
    ConvexPolygon(const Array<Vector3>& __vertex);
    virtual ~ConvexPolygon() {}

    /**
     Counter clockwise winding order.  
     */
	inline const Vector3& vertex(int i) const {
		return _vertex[i];
	}

	inline void setVertex(int i, const Vector3& v) {
		_vertex[i] = v;
	}

	/**
	 Zero vertices indicates an empty polygon (zero area).
	 */
	inline int numVertices() const {
		return _vertex.size();
	}

	inline void setNumVertices(int n) {
		_vertex.resize(n);
	}

    /**
     O(n) in the number of edges
     */
    bool isEmpty() const;

    /**
       Cuts the polygon at the plane. If the polygon is entirely above or below
       the plane, one of the returned polygons will be empty.
     
       @param above The part of the polygon above (on the side the
              normal points to or in the plane) the plane
       @param below The part of the polygon below the plane.
       @param newEdge  If a new edge was introduced, this is that edge (on the above portion; the below portion is the opposite winding.
    */
    void cut(const Plane& plane, ConvexPolygon &above, ConvexPolygon &below, DirectedEdge& newEdge);
    void cut(const Plane& plane, ConvexPolygon &above, ConvexPolygon &below);

	/**
		When a cut plane grazes a vertex in the polygon, two near-identical vertices may be created.
		The closeness of these two points can cause a number of problems, such as ConvexPolygon::normal() 
		returning an infinite vector.  It should be noted, however, that not all applications are 
		sensitive to near-identical vertices.

		removeDuplicateVertices() detects and eliminates redundant vertices.
	*/
	void removeDuplicateVertices();

    /**
     O(n) in the number of edges
     */
    float getArea() const;

    inline Vector3 normal() const {
        debugAssert(_vertex.length() >= 3);
        return (_vertex[1] - _vertex[0]).cross(_vertex[2] - _vertex[0]).direction();
    }

    /**
     Returns the same polygon with inverse winding.
     */
    ConvexPolygon inverse() const;
};



class ConvexPolyhedron {
public:
    /**
     Zero faces indicates an empty polyhedron
     */
    Array<ConvexPolygon> face;
    
    ConvexPolyhedron() {}
    ConvexPolyhedron(const Array<ConvexPolygon>& _face);

    /**
     O(n) in the number of edges
     */
    bool isEmpty() const;

    /**
     O(n) in the number of edges
     */
    float getVolume() const;

    /**
       Cuts the polyhedron at the plane. If the polyhedron is entirely above or below
       the plane, one of the returned polyhedra will be empty.
     
       @param above The part of the polyhedron above (on the side the
              normal points to or in the plane) the plane
       @param below The part of the polyhedron below the plane.
     */
    void cut(const Plane& plane, ConvexPolyhedron &above, ConvexPolyhedron &below);
};

} // namespace
#endif
