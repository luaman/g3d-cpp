/**
 @file MeshAlg.h

 Indexed Mesh algorithms.

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2003-09-14
 @edited  2004-02-17
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

    /** Adjacency information for a vertex.
        Does not contain the vertex position or normal,
        which are stored in the MeshAlg::Geometry object.
        */
    class Vertex {
    public:
        Vertex() {}

        /**
         Array of edges adjacent to this vertex.
         Let e = edgeIndex[i].  
         edge[(e >= 0) ? e : ~e].vertexIndex[0] == this
         vertex index.
         
         Edges may be listed multiple times if they are
         degenerate.
         */
        Array<int>              edgeIndex;

        /**
         Array of faces containing this vertex.  Faces
         may be listed multiple times if they are degenerate.
        */
        Array<int>              faceIndex;
    };


    /**
     Oriented, indexed triangle.
     */
    class Face {
    public:
        Face();

        /**
         Used by Edge::faceIndex to indicate a missing face.
         This is a large negative value.
         */
        static const int        NONE;


        /**
         Vertices in the face in counter-clockwise order.
         Degenerate faces may include the same vertex multiple times.
         */
        int                     vertexIndex[3];

        inline bool containsVertex(int v) const {
            return contains(vertexIndex, 3, v);
        }

        /**
         Edge indices in counter-clockwise order.  Edges are
         undirected, so it is important to know which way 
         each edge is pointing in a face.  This is encoded
         using negative indices.
         
         If <CODE>edgeIndex[i] >= 0</CODE> then this face
         contains the directed edge
         between vertex indices  
         <CODE>edgeArray[face.edgeIndex[i]].vertexIndex[0]</CODE>
         and
         <CODE>edgeArray[face.edgeIndex[i]].vertexIndex[1]</CODE>.
         
         If <CODE>edgeIndex[i] < 0</CODE> then 
         <CODE>~edgeIndex[i]</CODE> (i.e. the two's
         complement of) is used and this face contains the directed
         edge between vertex indices
         <CODE>edgeArray[~face.edgeIndex[i]].vertexIndex[0]</CODE>
         and
         <CODE>edgeArray[~face.edgeIndex[i]].vertexIndex[1]</CODE>.

         Degenerate faces may include the same edge multiple times.
         */
        // Temporarily takes on the value Face::NONE during adjacency
        // computation to indicate an edge that has not yet been assigned.
        int                     edgeIndex[3];

        inline bool containsEdge(int e) const {
            if (e < 0) {
                e = ~e;
            }
            return contains(edgeIndex, 3, e) || contains(edgeIndex, 3, ~e);
        }

        /** Contains the forward edge e if e >= 0 and the backward edge 
            ~e otherwise. */
        inline bool containsDirectedEdge(int e) const {
            return contains(edgeIndex, 3, e);
        }
    };


    /** Oriented, indexed edge */
    class Edge {
    public:
        Edge();

        /** Degenerate edges may include the same vertex times. */
        int                     vertexIndex[2];
        
        inline bool containsVertex(int v) const {
            return contains(vertexIndex, 3, v);
        }

        /**
         The edge is directed <B>forward</B> in face 0
         <B>backward</B> in face 1. Face index of MeshAlg::Face::NONE
         indicates a boundary (a.k.a. crack, broken) edge.
         */
        int                     faceIndex[2];
        
        /** Returns true if f is contained in the faceIndex array in either slot.
            To see if it is forward in that face, just check edge.faceIndex[0] == f.*/
        inline bool inFace(int f) const {
            return contains(faceIndex, 2, f);
        }

        /**
         Returns true if either faceIndex is NONE.
         */
        inline bool boundary() const {
            return (faceIndex[0] == Face::NONE) ||
                   (faceIndex[1] == Face::NONE);
        }

        /** @deprecated Use Edge::boundary */
        inline bool broken() const {
            return boundary();
        }

        /**
         Returns the reversed edge.
         */
        inline Edge reverse() const {
            Edge e;
            e.vertexIndex[0] = vertexIndex[1];
            e.vertexIndex[1] = vertexIndex[0];
            e.faceIndex[0]   = faceIndex[1];
            e.faceIndex[1]   = faceIndex[0];
            return e;
        }
    };
    

    /**
     Convenient for passing around the data that changes under
     animation. The faces and edges are needed to interpret 
     these values.
     */
    class Geometry {
    public:        
        Array<Vector3>          vertexArray;
        Array<Vector3>          normalArray;

        /**
         Assignment is optimized using SSE.
         */
        Geometry& operator=(const Geometry& src);

        void clear() {
            vertexArray.clear();
            normalArray.clear();
        }
    };

    /**
     Given a set of vertices and a set of indices for traversing them
     to create triangles, computes other mesh properties.  

     <B>Colocated vertices are treated as separate.</B>  To have
     colocated vertices collapsed (necessary for many algorithms,
     like shadowing), weld the mesh before computing adjacency.
     
     <I>Recent change: In version 6.00, colocated vertices were automatically
     welded by this routine and degenerate faces and edges were removed.  That
     is no longer the case.</I>

     Where two faces meet, there are two opposite directed edges.  These
     are collapsed into a single bidirectional edge in the edgeArray.
     If four faces meet exactly at the same edge, that edge will appear
     twice in the array, and so on.  If an edge is a boundary of the mesh 
     (i.e. if the edge has only one adjacent face) it will appear in the 
     array with one  face index set to MeshAlg::Face::NONE.

     @param vertexArray Vertex positions to use when deciding colocation.
     @param indexArray  Order to traverse vertices to make triangles
     @param faceArray   <I>Output</I>
     @param edgeArray   <I>Output</I>  
     @param facesAdjacentToVertex <I>Output</I> adjacentFaceArray[v] is an array of
                        indices for faces touching vertex index v
     */
    static void computeAdjacency(
        const Array<Vector3>&   vertexArray,
        const Array<int>&       indexArray,
        Array<Face>&            faceArray,
        Array<Edge>&            edgeArray,
        Array< Array<int> >&    facesAdjacentToVertex);


    static void computeAreaStatistics(
        const Array<Vector3>&   vertexArray,
        const Array<int>&       indexArray,
        double&                 minEdgeLength,
        double&                 meanEdgeLength,
        double&                 medianEdgeLength,
        double&                 maxEdgeLength,
        double&                 minFaceArea,
        double&                 meanFaceArea,
        double&                 medianFaceArea,
        double&                 maxFaceArea);

private:
    /**
     Computes the tangent space basis vectors for
     a counter-clockwise oriented face.

     @cite Max McGuire
     */
    static void computeTangentVectors(
        const Vector3&  normal,
        const Vector3   position[3],
        const Vector2   texCoord[3],
        Vector3&        tangent, 
        Vector3&        binormal);

public:

    /**
     Computes tangent and binormal vectors,
     which provide a (mostly) consistent
     parameterization over the surface for
     effects like bump mapping.

     The vertex, texCoord, tangent, and binormal
     arrays are parallel arrays.

     The resulting tangent and binormal might not be exactly
     perpendicular to each other.  They are guaranteed to
     be perpendicular to the normal.

     @cite Max McGuire
    */
    static void computeTangentSpaceBasis(
        const Array<Vector3>&       vertexArray,
        const Array<Vector2>&       texCoordArray,
        const Array<Vector3>&       vertexNormalArray,
        const Array<Face>&          faceArray,
        Array<Vector3>&             tangent,
        Array<Vector3>&             binormal);

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
     Computes face normals only.  Significantly faster (especially if
     normalize is true) than computeNormals.
     */
    static void computeFaceNormals(
        const Array<Vector3>&           vertexArray,
        const Array<Face>&              faceArray,
        Array<Vector3>&                 faceNormals,
        bool                            normalize = true);

    /**
     Classifies each face as a backface or a front face relative
     to the observer point P (which is at infinity when P.w = 0).
     A face with normal exactly perpendicular to the observer vector
     may be classified as either a front or a back face arbitrarily.
     */
    static void identifyBackfaces(
        const Array<Vector3>&           vertexArray,
        const Array<Face>&              faceArray,
        const Vector4&                  P,
        Array<bool>&                    backface);

    /** A faster version of identifyBackfaces for the case where
        face normals have already been computed */
    static void identifyBackfaces(
        const Array<Vector3>&           vertexArray,
        const Array<Face>&              faceArray,
        const Vector4&                  P,
        Array<bool>&                    backface,
        const Array<Vector3>&           faceNormals);

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
     Making Radiosity Usable: Automatic Preprocessing and Meshing Techniques for
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
    static int countBoundaryEdges(const Array<Edge>& edgeArray);

    /** @deprecated Use countBoundaryEdges */
    inline static int countBrokenEdges(const Array<Edge>& edgeArray) {
        return countBoundaryEdges(edgeArray);
    }


    /**
     Generates an array of integers from start to start + n - 1 that have run numbers
     in series then omit the next skip before the next run.  Useful for turning
     a triangle list into an indexed face set.

     Example: 
     <PRE>
       createIndexArray(10, x);
       // x = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9] 

       createIndexArray(5, x, 2);
       // x = [2, 3, 4, 5, 6, 7] 

       createIndexArray(6, x, 0, 2, 1);
       // x = [0, 1, 3, 4, 6, 7]
     </PRE>
     */
    static void createIndexArray(
        int                 n, 
        Array<int>&         array,
        int                 start   = 0,
        int                 run     = 1,
        int                 skip    = 0);

    /**
     Computes a conservative, near-optimal axis aligned bounding box and sphere. 

      @cite The bounding sphere uses the method from J. Ritter. An effcient bounding sphere. In Andrew S. Glassner, editor, Graphics Gems. Academic Press, Boston, MA, 1990.

    */
    static void computeBounds(const Array<Vector3>& vertex, class Box& box, class Sphere& sphere);

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

