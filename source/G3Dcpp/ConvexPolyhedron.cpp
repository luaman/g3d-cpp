/**
  @file ConvexPolyhedron.cpp
  
  @author Morgan McGuire, morgan@graphics3d.com

  @created 2001-11-11
  @edited  2003-02-15
 
  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.
 */

#include "G3D/ConvexPolyhedron.h"
#include "G3D/debug.h"

namespace G3D {

ConvexPolygon::ConvexPolygon(const Array<Vector3>& _vertex) : vertex(_vertex) {
    // Intentionally empty
}


bool ConvexPolygon::isEmpty() const {
    return (vertex.length() == 0) || (getArea() <= fuzzyEpsilon);
}


Real ConvexPolygon::getArea() const {

    if (vertex.length() < 3) {
        return 0;
    }

    Real sum = 0;

    int length = vertex.length();
    // Split into triangle fan, compute individual area
    for (int v = 2; v < length; v++) {
        int i0 = 0;
        int i1 = v - 1;
        int i2 = v;

        sum += (vertex[i1] - vertex[i0]).cross(vertex[i2] - vertex[i0]).length() / 2; 
    }

    return sum;
}

void ConvexPolygon::cut(const Plane& plane, ConvexPolygon &above, ConvexPolygon &below) {
    DirectedEdge edge;
    cut(plane, above, below, edge);
}

void ConvexPolygon::cut(const Plane& plane, ConvexPolygon &above, ConvexPolygon &below, DirectedEdge &newEdge) {
    above.vertex.resize(0);
    below.vertex.resize(0);

    if (isEmpty()) {
        //debugPrintf("Empty\n");
        return;
    }

    int v = 0;
    int length = vertex.length();


    Vector3 polyNormal = getNormal();
    Vector3 planeNormal= plane.getNormal();

    // See if the polygon is *in* the plane.
    if (planeNormal.fuzzyEq(polyNormal) || planeNormal.fuzzyEq(-polyNormal)) {
        // Polygon is parallel to the plane.  It must be either above,
        // below, or in the plane.

        Real a,b,c,d;
        Vector3 pt = vertex[0];

        plane.getEquation(a,b,c,d);
        Real r = (a * pt.x + b * pt.y + c * pt.z + d);

        if (fuzzyGe(r, 0)) {
            // The polygon is entirely in the plane.
            //debugPrintf("Entirely above\n");
            above = *this;
            return;
        } else {
            //debugPrintf("Entirely below (1)\n");
            below = *this;
            return;
        }
    }


    // Number of edges crossing the plane.  Used for 
    // debug assertions.
    int count = 0;

    // True when the last vertex we looked at was above the plane
    bool lastAbove = plane.halfSpaceContains(vertex[v]);

    if (lastAbove) {
        above.vertex.append(vertex[v]);
    } else {
        below.vertex.append(vertex[v]);
    }

    for (v = 1; v < length; v++) {
        bool isAbove = plane.halfSpaceContains(vertex[v]);
    
        if (lastAbove ^ isAbove) {
            // Switched sides.
            // Create an interpolated point that lies
            // in the plane, between the two points.
            Line line = Line::fromTwoPoints(vertex[v - 1], vertex[v]);
            Vector3 interp = line.intersection(plane);
            
            if (! interp.isFinite()) {

                // Since the polygon is not in the plane (we checked above), 
                // it must be the case that this edge (and only this edge)
                // is in the plane.  This only happens when the polygon is
                // entirely below the plane except for one edge.  This edge
                // forms a degenerate polygon, so just treat the whole polygon
                // as below the plane.
                below = *this;
                above.vertex.resize(0);
                //debugPrintf("Entirely below\n");
                return;
            }                

            above.vertex.append(interp);
            below.vertex.append(interp);
            if (lastAbove) {
                newEdge.stop = interp;
            } else {
                newEdge.start = interp;
            }
            count++;
        }

        lastAbove = isAbove;
        if (lastAbove) {
            above.vertex.append(vertex[v]);
        } else {
            below.vertex.append(vertex[v]);
        }
    }

    // Loop back to the first point, seeing if an interpolated point is
    // needed.
    bool isAbove = plane.halfSpaceContains(vertex[0]);
    if (lastAbove ^ isAbove) {
        Line line = Line::fromTwoPoints(vertex[length - 1], vertex[0]);
        Vector3 interp = line.intersection(plane);
        if (! interp.isFinite()) {
            // Since the polygon is not in the plane (we checked above), 
            // it must be the case that this edge (and only this edge)
            // is in the plane.  This only happens when the polygon is
            // entirely below the plane except for one edge.  This edge
            // forms a degenerate polygon, so just treat the whole polygon
            // as below the plane.
            below = *this;
            above.vertex.resize(0);
            //debugPrintf("Entirely below\n");
            return;
        }                

        above.vertex.append(interp);
        below.vertex.append(interp);
        debugAssertM(count < 2, "Convex polygons may only intersect planes at two edges.");
        if (lastAbove) {
            newEdge.stop = interp;
        } else {
            newEdge.start = interp;
        }
        count++;
    }

    debugAssertM((count == 2) || (count == 0), "Convex polygons may only intersect planes at two edges.");
    //debugPrintf("split \n");
}

ConvexPolygon ConvexPolygon::inverse() const {
    ConvexPolygon result;
    int length = vertex.length();
    result.vertex.resize(length);
    
    for (int v = 0; v < length; v++) {
        result.vertex[v] = vertex[length - v - 1];
    }

    return result;
}


//////////////////////////////////////////////////////////////////////////////

ConvexPolyhedron::ConvexPolyhedron(const Array<ConvexPolygon>& _face) : face(_face) {
    // Intentionally empty
}

Real ConvexPolyhedron::getVolume() const {

    if (face.length() < 4) {
        return 0;
    }

    // The volume of any pyramid is 1/3 * h * base area.
    // Discussion at: http://nrich.maths.org/mathsf/journalf/oct01/art1/

    Real sum = 0;

    // Choose the first vertex of the first face as the origin.
    // This lets us skip one face, too, and avoids negative heights.
    Vector3 v0 = face[0].vertex[0];
    for (int f = 1; f < face.length(); f++) {        
        const ConvexPolygon& poly = face[f];
        
        Real height = (poly.vertex[0] - v0).dot(poly.getNormal());
        Real base   = poly.getArea();

        sum += height * base;
    }

    return sum / 3;
}

bool ConvexPolyhedron::isEmpty() const {
    return (face.length() == 0) || (getVolume() <= fuzzyEpsilon);
}

void ConvexPolyhedron::cut(const Plane& plane, ConvexPolyhedron &above, ConvexPolyhedron &below) {
    above.face.resize(0);
    below.face.resize(0);

    Array<DirectedEdge> edge;

    int f;
    
    // See if the plane cuts this polyhedron at all.  Detect when
    // the polyhedron is entirely to one side or the other.
    //{
        int numAbove = 0, numIn = 0, numBelow = 0;
        bool ruledOut = false;
        Real d;
        Vector3 abc;
        plane.getEquation(abc, d);

        // This number has to be fairly large to prevent precision problems down
        // the road.
        const Real eps = 0.005;
        for (f = face.length() - 1; (f >= 0) && (!ruledOut); f--) {
            const ConvexPolygon& poly = face[f];
            for (int v = poly.vertex.length() - 1; (v >= 0) && (!ruledOut); v--) { 
                double r = abc.dot(poly.vertex[v]) + d;
                if (r > eps) {
                    numAbove++;
                } else if (r < -eps) {
                    numBelow++;
                } else {
                    numIn++;
                }

                ruledOut = (numAbove != 0) && (numBelow !=0);
            }
        }

        if (numBelow == 0) {
            above = *this;
            return;
        } else if (numAbove == 0) {
            below = *this;
            return;
        }
    //}

    // Clip each polygon, collecting split edges.
    for (f = face.length() - 1; f >= 0; f--) {
        ConvexPolygon a, b;
        DirectedEdge e;
        face[f].cut(plane, a, b, e);

        bool aEmpty = a.isEmpty();
        bool bEmpty = b.isEmpty();

        //debugPrintf("\n");
        if (! aEmpty) {
            //debugPrintf(" Above %f\n", a.getArea());
            above.face.append(a);
        }

        if (! bEmpty) {
            //debugPrintf(" Below %f\n", b.getArea());
            below.face.append(b);
        }

        if (! aEmpty && ! bEmpty) {
            //debugPrintf(" == Split\n");
            edge.append(e);
        } else {
            // Might be the case that the polygon is entirely on 
            // one side of the plane yet there is an edge we need
            // because it touches the plane.
            // 
            // Extract the non-empty vertex list and examine it.
            // If we find exactly one edge in the plane, add that edge.
            const Array<Vector3>& vertex = (aEmpty ? b.vertex : a.vertex);
            int L = vertex.length();
            int count = 0;
            for (int v = 0; v < L; v++) {
                if (plane.fuzzyContains(vertex[v]) && plane.fuzzyContains(vertex[(v + 1) % L])) {
                    e.start = vertex[v];
                    e.stop = vertex[(v + 1) % L];
                    count++;
                }
            }

            if (count == 1) {
                edge.append(e);
            }
        }
    }

    if (above.face.length() == 1) {
        // Only one face above means that this entire 
        // polyhedron is below the plane.  Move that face over.
        below.face.append(above.face[0]);
        above.face.resize(0);
    } else if (below.face.length() == 1) {
        // This shouldn't happen, but it arises in practice
        // from numerical imprecision.
        above.face.append(below.face[0]);
        below.face.resize(0);
    }

    if ((above.face.length() > 0) && (below.face.length() > 0)) {
        // The polyhedron was actually cut; create a cap polygon
        ConvexPolygon cap;

        // Collect the final polgyon by sorting the edges
        int numVertices = edge.length();
/*debugPrintf("\n");
for (int xx=0; xx < numVertices; xx++) {
    std::string s1 = edge[xx].start.toString();
    std::string s2 = edge[xx].stop.toString();
    debugPrintf("%s -> %s\n", s1.c_str(), s2.c_str());
}
*/

        // Need at least three points to make a polygon
        debugAssert(numVertices >= 3);

        Vector3 lastVertex = edge.last().stop;
        cap.vertex.append(lastVertex);

        // Search for the next vertex.  Because of accumulating
        // numerical error, we have to find the closest match, not 
        // just the one we expect.
        for (int v = numVertices - 1; v >= 0; v--) {
            // matching edge index
            int index = 0;
            int num = edge.length();
            double distance = (edge[index].start - lastVertex).squaredLength();
            for (int e = 1; e < num; e++) {
                double d = (edge[e].start - lastVertex).squaredLength();

                if (d < distance) {
                    // This is the new closest one
                    index = e;
                    distance = d;
                }
            }

            // Don't tolerate ridiculous error.
            debugAssertM(distance < 0.02, "Edge missing while closing polygon.");

            lastVertex = edge[index].stop;
            cap.vertex.append(lastVertex);
        }
        
        //debugPrintf("\n");
        //debugPrintf("Cap (both) %f\n", cap.getArea());
        above.face.append(cap);
        below.face.append(cap.inverse());
    }

    // Make sure we put enough faces on each polyhedra
    debugAssert((above.face.length() == 0) || (above.face.length() >= 4));   
    debugAssert((below.face.length() == 0) || (below.face.length() >= 4));
}

}

