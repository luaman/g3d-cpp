/**
  @file MeshAlgAdjacency.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2003-09-14
  @edited  2003-10-22

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.

 */

#include "G3D/MeshAlg.h"
#include "G3D/Table.h"
#include "G3D/Set.h"

namespace G3D {

/**
 A half [i.e. directed] edge.
 */
class MeshDirectedEdgeKey {
public:

    /** vertexIndex[0] <= vertexIndex[1] */
    int     vertexIndex[2];

    MeshDirectedEdgeKey() {}
    
    MeshDirectedEdgeKey(
        const int        i0,
        const int        i1) {

        if (i0 <= i1) {
            vertexIndex[0] = i0;
            vertexIndex[1] = i1;
        } else {
            vertexIndex[0] = i1;
            vertexIndex[1] = i0;
        }
    }


    bool operator==(const MeshDirectedEdgeKey& e2) const {
        for (int i = 0; i < 2; ++i) {
            if (vertexIndex[i] != e2.vertexIndex[i]) {
                return false;
            }
        }
        return true;
    }
};

}

unsigned int hashCode(const G3D::MeshDirectedEdgeKey& e) {
    return e.vertexIndex[0] + (e.vertexIndex[1] << 16);
}

namespace G3D {

/**
 A hashtable mapping edges to lists of indices for
 faces.  This is used instead of Table because of the
 special logic in insert.

 Used only for MeshAlg::computeAdjacency.

 In the face lists, index <I>f</I> >= 0 indicates that
 <I>f</I> contains the edge as a forward edge.  Index <I>f</I> < 0 
 indicates that ~<I>f</I> contains the edge as a backward edge.
 */
class MeshEdgeTable {
public:
    typedef Table<MeshDirectedEdgeKey, Array<int> > ET;

private:
    
    ET                   table;

public:
    
    /**
     Clears the table.
     */
    void clear() {
        table.clear();
    }
    
    /**
     Inserts the faceIndex into the edge's face list.
     The index may be a negative number indicating a backface.
     */
    void insert(const MeshDirectedEdgeKey& edge, int faceIndex) {
        
        // debugAssertM((table.size() > 20) && (table.debugGetLoad() < 0.5 || table.debugGetNumBuckets() < 20),
        //    "MeshEdgeTable is using a poor hash function.");

        if (! table.containsKey(edge)) {
            // First time
            Array<int> x(1);
            x[0] = faceIndex;
            table.set(edge, x);
        } else {
            table[edge].append(faceIndex);
        }
    }

    /**
     Returns the face list for a given edge
     */
    const Array<int>& get(const MeshDirectedEdgeKey& edge) {
        return table[edge];
    }

    ET::Iterator begin() {
        return table.begin();
    }

    const ET::Iterator end() const {
        return table.end();
    }

};


/**
 edgeTable[edgeKey] is a list of faces containing
 
 Used and cleared by MeshModel::computeAdjacency()
 */
static MeshEdgeTable            edgeTable;

/**
 Assigns the edge index into the next unassigned edge
 index.  The edge index may be negative, indicating
 a reverse edge.
 */
static void assignEdgeIndex(MeshAlg::Face& face, int e) {
    for (int i = 0; i < 3; ++i) {
        if (face.edgeIndex[i] == MeshAlg::Face::NONE) {
            face.edgeIndex[i] = e;
            return;
        }
    }

    debugAssertM(false, "Face has already been assigned 3 edges");
}


void MeshAlg::computeAdjacency(
    const Array<Vector3>&   vertexArray,
    const Array<int>&       indexArray,
    Array<Face>&            faceArray,
    Array<Edge>&            edgeArray,
    Array< Array<int> >&    adjacentFaceArray) {

    edgeArray.clear();
    adjacentFaceArray.clear();
    faceArray.clear();
    edgeTable.clear();
    
    // Face normals
    Array<Vector3> faceNormal;

    // This array has the same size as the vertex array
    adjacentFaceArray.resize(vertexArray.size());

    // Iterate through the triangle list
    for (int q = 0; q < indexArray.size(); q += 3) {

        Vector3 vertex[3];
        int f = faceArray.size();
        MeshAlg::Face& face = faceArray.next();

        // Construct the face
        for (int j = 0; j < 3; ++j) {
            int v = indexArray[q + j];
            face.vertexIndex[j] = v;
            face.edgeIndex[j]   = Face::NONE;

            // Every vertex is adjacent to 
            adjacentFaceArray[v].append(f);

            // We'll need these vertices to find the face normal
            vertex[j]           = vertexArray[v];
        }

        // Compute the face normal
        Vector3 N = (vertex[1] - vertex[0]).cross(vertex[2] - vertex[0]);
        faceNormal.append(N.directionOrZero());

        static const int nextIndex[] = {1, 2, 0};

        // Add each edge to the edge table.
        for (int j = 0; j < 3; ++j) {
            const int      i0 = indexArray[q + j];
            const int      i1 = indexArray[q + nextIndex[j]];

            const MeshDirectedEdgeKey edge(i0, i1);

            if (i0 == edge.vertexIndex[0]) {
                // The edge was directed in the same manner as in the face
                edgeTable.insert(edge, f);
            } else {
                // The edge was directed in the opposite manner as in the face
                edgeTable.insert(edge, ~f);
            }
        }
    }
    
    // For each edge in the edge table, create an edge in the edge array.
    // Collapse every 2 edges from adjacent faces.

    MeshEdgeTable::ET::Iterator cur = edgeTable.begin();
    MeshEdgeTable::ET::Iterator end = edgeTable.end();

    while (cur != end) {
        MeshDirectedEdgeKey&  edgeKey        = cur->key; 
        Array<int>&           faceIndexArray = cur->value;

        // Process this edge
        while (faceIndexArray.size() > 0) {

            // Remove the last index
            int f0 = faceIndexArray.pop();

            // Find the normal to that face
            const Vector3& n0 = faceNormal[(f0 >= 0) ? f0 : ~f0];

            bool found = false;

            // We try to find the matching face with the closest
            // normal.  This ensures that we don't introduce a lot
            // of artificial ridges into flat parts of a mesh.
            double ndotn = -2;
            int f1, i1;
            
            // Try to Find the face with the matching edge
            for (int i = faceIndexArray.size() - 1; i >= 0; --i) {
                int f = faceIndexArray[i];

                if ((f >= 0) != (f0 >= 0)) {
                    // This face contains the oppositely oriented edge
                    // and has not been assigned too many edges

                    const Vector3& n1 = faceNormal[(f >= 0) ? f : ~f];
                    double d = n1.dot(n0);

                    if (found) {
                        // We previously found a good face; see if this
                        // one is better.
                        if (d > ndotn) {
                            // This face is better.
                            ndotn = d;
                            f1    = f;
                            i1    = i;
                        }
                    } else {
                        // This is the first face we've found
                        found = true;
                        ndotn = d;
                        f1    = f;
                        i1    = i;
                    }
                }
            }

            // Create the new edge
            int e = edgeArray.size();
            Edge& edge = edgeArray.next();
            
            edge.vertexIndex[0] = edgeKey.vertexIndex[0];
            edge.vertexIndex[1] = edgeKey.vertexIndex[1];

            if (f0 >= 0) {
                edge.faceIndex[0] = f0;
                edge.faceIndex[1] = Face::NONE;
                assignEdgeIndex(faceArray[f0], e); 
            } else {
                // The face indices above are two's complemented.
                // this code restores them to regular indices.
                debugAssert((~f0) >= 0);
                edge.faceIndex[1] = ~f0;
                edge.faceIndex[0] = Face::NONE;

                // The edge index *does* need to be inverted, however.
                assignEdgeIndex(faceArray[~f0], ~e); 
            }

            if (found) {
                // We found a matching face; remove both
                // faces from the active list.
                faceIndexArray.fastRemove(i1);

                if (f1 >= 0) {
                    edge.faceIndex[0] = f1;
                    assignEdgeIndex(faceArray[f1], e); 
                } else {
                    edge.faceIndex[1] = ~f1;
                    assignEdgeIndex(faceArray[~f1], ~e); 
                }
            }
        }

        ++cur;
    }

    edgeTable.clear();

    // Now order the edge indices inside the faces correctly.
    for (int f = 0; f < faceArray.size(); ++f) {
        Face& face = faceArray[f];
        int e0 = face.edgeIndex[0];
        int e1 = face.edgeIndex[1];
        int e2 = face.edgeIndex[2];

        // e0 will always remain first.  The only 
        // question is whether e1 and e2 should be swapped.
    
        // See if e1 begins at the vertex where e1 ends.
        const Vector3& e0End = (e0 < 0) ? 
            vertexArray[edgeArray[~e0].vertexIndex[0]] :
            vertexArray[edgeArray[e0].vertexIndex[1]];

        const Vector3& e1Begin = (e1 < 0) ? 
            vertexArray[edgeArray[~e1].vertexIndex[1]] :
            vertexArray[edgeArray[e1].vertexIndex[0]];

        if (e0End != e1Begin) {
            // We must swap e1 and e2
            face.edgeIndex[1] = e2;
            face.edgeIndex[2] = e1;
        }
    }
}

} // G3D namespace
