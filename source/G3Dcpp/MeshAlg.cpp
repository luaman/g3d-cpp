/**
  @file MeshAlg.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2003-09-14
  @edited  2003-09-14

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.

 */

#include "G3D/MeshAlg.h"
#include "G3D/Table.h"

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
    unsigned int h = 0;
    for (int i = 0; i < 2; ++i) {
        h = (h << 7) + e.vertex[i].hashCode();
    }
    return h;
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
}
