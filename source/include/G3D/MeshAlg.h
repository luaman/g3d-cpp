/**
 @file MeshAlg.h

 Indexed Mesh algorithms.

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2003-09-14
 @edited  2003-10-18
*/

#ifndef G3D_MESHALG_H
#define G3D_MESHALG_H

#include "G3D/platform.h"
#include "G3D/Array.h"
#include "G3D/Vector3.h"

namespace G3D {

/**
  Indexed mesh algorithms.  You have to build your own mesh class.
  <P>
  (No mesh class is provided with G3D because there isn't an "ideal" 
  mesh format-- you may want keyframed animation, skeletal animation,
  texture coordinates, etc.)
 */
class MeshAlg {
public:

    /**
     Oriented, indexed triangle.
     */
    class Face {
    public:
        Face();

        /**
         Used by Edge::faceIndex to indicate a missing face.
         */
        static const int        NONE;


        /**
         Vertices in the face in counter-clockwise order
         */
        int                     vertexIndex[3];

        /** If the edge index is negative, ~index is in this face
            but is directed oppositely.
         */
        int                     edgeIndex[3];
    };

    /** Oriented, indexed edge */
    class Edge {
    public:
        Edge();

        int                     vertexIndex[2];

        /** The edge is directed forward in the first face and
            backward in the second face.  Face index of MD2Model::Face::NONE
            indicates a dangling edge.
          */
        int                     faceIndex[2];
    };
    

    /**
     Convenient for passing around the data that changes under
     animation.
     The faces and edges are needed to interpret these values.
     */
    class Geometry {
    public:        
        Array<Vector3>          vertexArray;
        Array<Vector3>          normalArray;

        void clear() {
            vertexArray.clear();
            normalArray.clear();
        }
    };

    /**
     Given a set of vertices and a set of indices for traversing them
     to create triangles, computes other mesh properties.  
     
     Faces consisting of fewer than 3 discrete vertex indices will not
     appear in the face array and their edges will not appear in
     the edge array.

     @param vertexArray Vertex positions to use when deciding colocation.
     @param indexArray  Order to traverse vertices to make triangles
     @param faceArray   Output
     @param geometricEdgeArray Output.  These edges weld colocated vertices
                        and remove degenerate edges.
     @param adjacentFaceArray Output. adjacentFaceArray[v] is an array of
                        indices for faces touching vertex index v
     */
    static void computeAdjacency(
        const Array<Vector3>&   vertexArray,
        const Array<int>&       indexArray,
        Array<Face>&            faceArray,
        Array<Edge>&            geometricEdgeArray,
        Array< Array<int> >&    adjacentFaceArray);

   /**
     @param vertexNormalArray Output. Computed by averaging adjacent face normals
     @param faceNormalArray Output. 
    */
    static void computeNormals(
        const Array<Vector3>&   vertexArray,
        const Array<Face>&      faceArray,
        const Array< Array<int> >& adjacentFaceArray,
        Array<Vector3>&         vertexNormalArray,
        Array<Vector3>&         faceNormalArray);

    /**
     Welds nearby and colocated elements of the <I>oldVertexArray</I> together so that
     <I>newVertexArray</I> contains no vertices within <I>radius</I> of one another.
     This is useful for downsampling meshes and welding cracks created by artist errors
     or numerical imprecision.  It is not guaranteed to fix cracks but the probability of
     not fixing them (with a suitablly large radius) approaches zero.

     The two integer arrays map indices back and forth between the arrays according to:
     <PRE>
     oldVertexArray[toOld[ni]] == newVertexArray[ni]
     oldVertexArray[oi] == newVertexArray[toNew[ni]]
     </PRE>

     Note that newVertexArray is never longer than oldVertexArray and is shorter when
     vertices are welded.

     Welding with a large radius will effectively compute a lower level of detail for
     the mesh.

     The welding method runs in roughly linear time in the length of oldVertexArray--
     a uniform spatial grid is used to achieve nearly constant time vertex collapses
     for uniformly distributed vertices.

     @cite The method is that described as the 'Grouper' in Baum, Mann, Smith, and Winget, 
     Making Radiosity Usable: Automatic PReprocessing and Meshing Techniques for
     the Generation of Accurate Radiosity Solutions, Computer Graphics vol 25, no 4, July 1991.
     */
    static void computeWeld(
        const Array<Vector3>& oldVertexArray,
        Array<Vector3>&       newVertexArray,
        Array<int>&           toNew,
        Array<int>&           toOld,
        double                radius);


    /**
     Counts the number of edges (in an edge array returned from 
     MeshAlg::computeAdjacency) that have only one adjacent face.
     */
    static int countBrokenEdges(const Array<Edge>& edgeArray);

protected:

    /**
     Helper for computeAdjacency.  If a directed edge with index e already
     exists from i0 to i1 then e is returned.  If a directed edge with index e
     already exists from i1 to i0, ~e is returned (the complement) and
     edgeArray[e] is set to f.  Otherwise, a new edge is created from i0 to i1
     with first face index f and its index is returned.
    
     @param vertexArray Vertex positions to use when deciding colocation.
     
     @param area Area of face f.  When multiple edges of the same direction 
       are found between the same vertices (usually because of degenerate edges)
       the face with larger area is kept in the edge table.
     */
    static int findEdgeIndex(
        const Array<Vector3>&   vertexArray,
        Array<Edge>&            geometricEdgeArray,
        int i0, int i1, int f, double area);
};
}
#endif

