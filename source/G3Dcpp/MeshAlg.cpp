/**
  @file MeshAlg.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2003-09-14
  @edited  2003-10-06

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.

 */

#include "G3D/MeshAlg.h"
#include "G3D/Table.h"
#include "G3D/Set.h"

namespace G3D {

const int MeshAlg::Face::NONE             = -100;


MeshAlg::Face::Face() {
    for (int i = 0; i < 3; ++i) {
        edgeIndex[i]   = 0;
        vertexIndex[i] = 0;
    }
}


MeshAlg::Edge::Edge() {
    for (int i = 0; i < 2; ++i) {
        vertexIndex[i]   = 0;
        // Negative face indices are faces that don't exist
        faceIndex[i]     = -1;
    }
}


void MeshAlg::computeNormals(
    const Array<Vector3>&   vertexArray,
    const Array<Face>&      faceArray,
    const Array< Array<int> >& adjacentFaceArray,
    Array<Vector3>&         vertexNormalArray,
    Array<Vector3>&         faceNormalArray) {

    // Face normals
    faceNormalArray.resize(faceArray.size());
    for (int f = 0; f < faceArray.size(); ++f) {
        const Face& face = faceArray[f];

        Vector3 vertex[3];
        for (int j = 0; j < 3; ++j) {
            vertex[j] = vertexArray[face.vertexIndex[j]];
        }

        faceNormalArray[f] = (vertex[1] - vertex[0]).cross(vertex[2] - vertex[0]).direction();
    }


    // Per-vertex normals, computed by averaging
    vertexNormalArray.resize(vertexArray.size());
    for (int v = 0; v < vertexNormalArray.size(); ++v) {
        Vector3 sum = Vector3::ZERO;
        for (int k = 0; k < adjacentFaceArray[v].size(); ++k) {
            int f = adjacentFaceArray[v][k];
            sum += faceNormalArray[f];
        }
        vertexNormalArray[v] = sum.direction();
    }
}


/**
 A directed edge for edgeTable.
 */
class MeshDirectedEdgeKey {
public:
    Vector3 vertex[2];

    MeshDirectedEdgeKey() {}
    
    MeshDirectedEdgeKey(
        const Vector3& v0,
        const Vector3& v1) {
        vertex[0] = v0;
        vertex[1] = v1;
    }


    bool operator==(const G3D::MeshDirectedEdgeKey& e2) const {
        for (int i = 0; i < 2; ++i) {
            if (vertex[i] != e2.vertex[i]) {
                return false;
            }
        }
        return true;
    }
};

}

unsigned int hashCode(const G3D::MeshDirectedEdgeKey& e) {
    return (e.vertex[0].hashCode() + 1) ^ e.vertex[1].hashCode();
}

namespace G3D {

/**
 A 2-key hashtable for edges.  Used only during loading of Mesh's.
 */
class MeshEdgeTable {
private:
    
    /**
     Maps edges to edge indices.
     */
    Table<MeshDirectedEdgeKey, int>  table;

public:
    
    /**
     Index of a missing edge.
     */
    static const int        NO_EDGE;

    /**
     Clears the table.
     */
    void clear() {
        table.clear();
    }
    
    /**
     Inserts the given edge into the table.
     */
    void insert(const MeshDirectedEdgeKey& edge, int edgeIndex) {
        debugAssert(table.debugGetLoad() < 0.5 || table.debugGetNumBuckets() < 20);
        if (! table.containsKey(edge)) {
            table.set(edge, edgeIndex);
        }
    }

    /**
     Returns the index of the edge from i0 to i1, NO_EDGE if
     there is no such edge.
     */
    int get(const MeshDirectedEdgeKey& edge) {
        if (table.containsKey(edge)) {
            return table[edge];
        } else {
            return NO_EDGE;
        }
    }
};

const int MeshEdgeTable::NO_EDGE = -1;

/**
 Used during loading, cleared by MeshModel::computeAdjacency()
 */
static MeshEdgeTable         edgeTable;


/**
 Area of faces to the left and right of an edge.
 */
class MeshFaceAreas {
public:
    double area[2];

    MeshFaceAreas() {
        // Initialize to -1 so that the first face
        // will have greater area even if it is
        // degenerate.
        area[0] = -1;
        area[1] = -1;
    }
};

/**
 Parallel to the edge array.  This tracks the area of the faces
 on either side of an edge.
 Used during loading, cleared by MeshModel::computeAdjacency()
 */
static Array<MeshFaceAreas> faceAreas;

void MeshAlg::computeAdjacency(
    const Array<Vector3>&   vertexArray,
    const Array<int>&       indexArray,
    Array<Face>&            faceArray,
    Array<Edge>&            edgeArray,
    Array< Array<int> >&    adjacentFaceArray) {

    edgeArray.resize(0);
    adjacentFaceArray.resize(0);
    adjacentFaceArray.resize(vertexArray.size());

    faceArray.resize(0);
    
    // Iterate through the triangle list
    for (int q = 0; q < indexArray.size(); q += 3) {

        // Don't allow degenerate faces
        if ((indexArray[q + 0] != indexArray[q + 1]) &&
            (indexArray[q + 1] != indexArray[q + 2]) &&
            (indexArray[q + 2] != indexArray[q + 0])) {
            Vector3 vertex[3];
            int f = faceArray.size();
            MeshAlg::Face& face = faceArray.next();

            // Construct the face
            for (int j = 0; j < 3; ++j) {
                int v = indexArray[q + j];
                face.vertexIndex[j] = v;
                adjacentFaceArray[v].append(f);
                vertex[j] = vertexArray[v];
            }

            const double area = (vertex[1] - vertex[0]).cross(vertex[2] - vertex[0]).length() * 0.5;
            static const int nextIndex[] = {1, 2, 0};

            // Find the indices of edges in the face
            for (int j = 0; j < 3; ++j) {
                int v0 = indexArray[q + j];
                int v1 = indexArray[q + nextIndex[j]];

                face.edgeIndex[j] = findEdgeIndex(vertexArray, edgeArray, v0, v1, f, area);
            }
        }
    }

    edgeTable.clear();
    faceAreas.resize(0);
}


int MeshAlg::findEdgeIndex(
    const Array<Vector3>&   vertexArray,
    Array<Edge>&            edgeArray,
    int                     i0,
    int                     i1,
    int                     f,
    double                  area) {

    const Vector3& v0 = vertexArray[i0];
    const Vector3& v1 = vertexArray[i1];

    // First see if the forward directed edge already exists
    const MeshDirectedEdgeKey forward (v0, v1);

    int e = edgeTable.get(forward);

    if (e != MeshEdgeTable::NO_EDGE) {
        // The edge already exists as a forward edge.  Update
        // the edge pointers if the new face has more area.

        if (area > faceAreas[e].area[0]) {
            faceAreas[e].area[0]      = area;
            edgeArray[e].faceIndex[0] = f;
        }

        return e;
    }
    
    // Second see if the backward directed edge already exists
    const MeshDirectedEdgeKey backward(v1, v0);
    e = edgeTable.get(backward);

    if (e != MeshEdgeTable::NO_EDGE) {
        // The edge already exists as a backward edge.  Update
        // the edge pointers if the new face has more area.
        
        if (area > faceAreas[e].area[1]) {
            faceAreas[e].area[1]      = area;
            edgeArray[e].faceIndex[1] = f;
        }
        return ~e;
    }

    // Third, the edge must not exist so add it as a forward edge
    e = edgeArray.size();
    MeshAlg::Edge& edge = edgeArray.next();

    edge.vertexIndex[0] = i0;
    edge.vertexIndex[1] = i1;
    edge.faceIndex[0]   = f;
    edge.faceIndex[1]   = MeshAlg::Face::NONE;

    faceAreas.next().area[0] = area;

    edgeTable.insert(forward, e);

    return e;
}

namespace _internal {

class Welder {
public:
    /** Indices of newVertexArray elements in <B>or near</B> a grid cell. */
    typedef Array<int> List;

    enum {GRID_RES = 32};

    List grid[GRID_RES][GRID_RES][GRID_RES];

    const Array<Vector3>& oldVertexArray;
    Array<Vector3>&       newVertexArray;
    Array<int>&           toNew;
    Array<int>&           toOld;

    /** Must be less than one grid cell, not checked */
    const double          radius;

    /** (oldVertexArray[i] - offset) * scale is on the range [0, 1] */
    Vector3               offset;
    Vector3               scale;

    Welder(    
        const Array<Vector3>& _oldVertexArray,
        Array<Vector3>&       _newVertexArray,
        Array<int>&           _toNew,
        Array<int>&           _toOld,
        double                _radius);
        
    /**
     Computes the grid index from an ordinate.
     */
    void toGridCoords(Vector3 v, int& x, int& y, int& z) const;

    /** Gets the index of a vertex, adding it to 
        newVertexArray if necessary. */
    int getIndex(const Vector3& vertex);
    
    void weld();
};

}}


G3D::uint32 hashCode(const G3D::_internal::Welder::List* x) {
    return (G3D::uint32)x;
}

namespace G3D {
namespace _internal {

Welder::Welder(    
    const Array<Vector3>& _oldVertexArray,
    Array<Vector3>&       _newVertexArray,
    Array<int>&           _toNew,
    Array<int>&           _toOld,
    double                _radius) :
    oldVertexArray(_oldVertexArray),
    newVertexArray(_newVertexArray),
    toNew(_toNew),
    toOld(_toOld),
    radius(_radius) {

    // Compute a scale factor that moves the range
    // of all ordinates to [0, 1]
    Vector3 minBound = Vector3::INF3;
    Vector3 maxBound = -minBound;

    for (int i = 0; i < oldVertexArray.size(); ++i) {
        minBound.min(oldVertexArray[i]);
        maxBound.max(oldVertexArray[i]);
    }

    offset = minBound;
    scale  = maxBound - minBound;
    for (int i = 0; i < 3; ++i) {
        // The model might have zero extent along some axis
        if (fuzzyEq(scale[i], 0.0)) {
            scale[i] = 1.0;
        } else {
            scale[i] = 1.0 / scale[i];
        }
    }
}


void Welder::toGridCoords(Vector3 v, int& x, int& y, int& z) const {
    v = (v - offset) * scale;
    x = iClamp(v.x * GRID_RES, 0, GRID_RES - 1);
    y = iClamp(v.y * GRID_RES, 0, GRID_RES - 1);
    z = iClamp(v.z * GRID_RES, 0, GRID_RES - 1);
}


int Welder::getIndex(const Vector3& vertex) {

    int closestIndex = -1;
    double distanceSquared = inf;

    int ix, iy, iz;
    toGridCoords(vertex, ix, iy, iz);

    // Check against all vertices within radius of this grid cube
    const List& list = grid[ix][iy][iz];

    for (int i = 0; i < list.size(); ++i) {
        double d = (newVertexArray[list[i]] - vertex).squaredLength();

        if (d < distanceSquared) {
            distanceSquared = d;
            closestIndex = list[i];
        }
    }

    if (distanceSquared <= radius * radius) {

        return closestIndex;

    } else {

        // This is a new vertex
        int newIndex = newVertexArray.size();
        newVertexArray.append(vertex);

        // Create a new vertex and store its index in the
        // neighboring grid cells (usually, only 1 neighbor)

        Set<List*> neighbors;

        for (int dx = -1; dx <= +1; ++dx) {
            for (int dy = -1; dy <= +1; ++dy) {
                for (int dz = -1; dz <= +1; ++dz) {
                    int ix, iy, iz;
                    toGridCoords(vertex + Vector3(dx, dy, dz) * radius, ix, iy, iz);
                    neighbors.insert(&(grid[ix][iy][iz]));
                }
            }
        }

        Set<List*>::Iterator neighbor(neighbors.begin());
        Set<List*>::Iterator none(neighbors.end());

        while (neighbor != none) {
            (*neighbor)->append(newIndex);
            ++neighbor;
        }

        return newIndex;
    }
}


void Welder::weld() {
    newVertexArray.resize(0);

    // Prime the vertex positions
    for (int i = 0; i < oldVertexArray.size(); ++i) {
        getIndex(oldVertexArray[i]);
    }

    // Now create the official remapping by snapping to 
    // nearby vertices.
    toNew.resize(oldVertexArray.size());
    toOld.resize(newVertexArray.size());

    for (int oi = 0; oi < oldVertexArray.size(); ++oi) {
        toNew[oi] = getIndex(oldVertexArray[oi]);
        toOld[toNew[oi]] = oi;
    }
}

} // internal namespace


void MeshAlg::computeWeld(
    const Array<Vector3>& oldVertexArray,
    Array<Vector3>&       newVertexArray,
    Array<int>&           toNew,
    Array<int>&           toOld,
    double                radius) {

    _internal::Welder welder(oldVertexArray, newVertexArray, toNew, toOld, radius);
    welder.weld();
}

} // G3D namespace
