/**
  @file MeshAlg.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2003-09-14
  @edited  2004-02-18

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.

 */

#include "G3D/MeshAlg.h"
#include "G3D/Table.h"
#include "G3D/Set.h"
#include "G3D/Box.h"
#include "G3D/Sphere.h"
#include "G3D/vectorMath.h"

namespace G3D {

const int MeshAlg::Face::NONE             = INT_MIN;


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


MeshAlg::Geometry& MeshAlg::Geometry::operator=(const MeshAlg::Geometry& src) {
    vertexArray.resize(src.vertexArray.size());
    normalArray.resize(src.vertexArray.size());

    System::memcpy(vertexArray.getCArray(), src.vertexArray.getCArray(), sizeof(Vector3)*vertexArray.size());
    System::memcpy(normalArray.getCArray(), src.normalArray.getCArray(), sizeof(Vector3)*normalArray.size());

    return *this;
}


void MeshAlg::computeNormals(
    const Array<Vector3>&   vertexGeometry,
    const Array<Face>&      faceArray,
    const Array< Array<int> >& adjacentFaceArray,
    Array<Vector3>&         vertexNormalArray,
    Array<Vector3>&         faceNormalArray) {

    // Construct a fake vertex array for backwards compatibility
    Array<Vertex> fakeVertexArray(adjacentFaceArray.size());

    for (int v = 0; v < adjacentFaceArray.size(); ++v) {
        fakeVertexArray[v].faceIndex = adjacentFaceArray[v];
        // We leave out the edges because they aren't used to compute normals
    }

    computeNormals(vertexGeometry, faceArray, fakeVertexArray, 
        vertexNormalArray, faceNormalArray);
}

    
void MeshAlg::computeNormals(
    const Array<Vector3>&   vertexGeometry,
    const Array<Face>&      faceArray,
    const Array<Vertex>&    vertexArray,
    Array<Vector3>&         vertexNormalArray,
    Array<Vector3>&         faceNormalArray) {

    // Face normals
    faceNormalArray.resize(faceArray.size());
    for (int f = 0; f < faceArray.size(); ++f) {
        const Face& face = faceArray[f];

        Vector3 vertex[3];
        for (int j = 0; j < 3; ++j) {
            vertex[j] = vertexGeometry[face.vertexIndex[j]];
        }

        faceNormalArray[f] = (vertex[1] - vertex[0]).cross(vertex[2] - vertex[0]);
    }

    // Per-vertex normals, computed by averaging
    vertexNormalArray.resize(vertexGeometry.size());
    for (int v = 0; v < vertexNormalArray.size(); ++v) {
        Vector3 sum = Vector3::ZERO;
        for (int k = 0; k < vertexArray[v].faceIndex.size(); ++k) {
            const int f = vertexArray[v].faceIndex[k];
            sum += faceNormalArray[f];
        }
        vertexNormalArray[v] = sum.direction();
    }


    for (int f = 0; f < faceArray.size(); ++f) {
        faceNormalArray[f] = faceNormalArray[f].direction();
    }

}


void MeshAlg::computeFaceNormals(
    const Array<Vector3>&           vertexArray,
    const Array<MeshAlg::Face>&     faceArray,
    Array<Vector3>&                 faceNormals,
    bool                            normalize) {

    faceNormals.resize(faceArray.size());

    for (int f = 0; f < faceArray.size(); ++f) {
        const MeshAlg::Face& face = faceArray[f];

        const Vector3& v0 = vertexArray[face.vertexIndex[0]];
        const Vector3& v1 = vertexArray[face.vertexIndex[1]];
        const Vector3& v2 = vertexArray[face.vertexIndex[2]];
        
        faceNormals[f] = (v1 - v0).cross(v2 - v0);
    }

    if (normalize) {
        for (int f = 0; f < faceArray.size(); ++f) {
            faceNormals[f] = faceNormals[f].direction();
        }
    }
}


void MeshAlg::identifyBackfaces(
    const Array<Vector3>&           vertexArray,
    const Array<MeshAlg::Face>&     faceArray,
    const Vector4&                  HP,
    Array<bool>&                    backface) {

    Vector3 P = HP.xyz();

    backface.resize(faceArray.size());

    if (fuzzyEq(HP.w, 0.0)) {
        // Infinite case
        for (int f = faceArray.size() - 1; f >= 0; --f) {
            const MeshAlg::Face& face = faceArray[f];

            const Vector3& v0 = vertexArray[face.vertexIndex[0]];
            const Vector3& v1 = vertexArray[face.vertexIndex[1]];
            const Vector3& v2 = vertexArray[face.vertexIndex[2]];
        
            const Vector3 N = (v1 - v0).cross(v2 - v0);

            backface[f] = N.dot(P) < 0;
        }
    } else {
        // Finite case
        for (int f = faceArray.size() - 1; f >= 0; --f) {
            const MeshAlg::Face& face = faceArray[f];

            const Vector3& v0 = vertexArray[face.vertexIndex[0]];
            const Vector3& v1 = vertexArray[face.vertexIndex[1]];
            const Vector3& v2 = vertexArray[face.vertexIndex[2]];
        
            const Vector3 N = (v1 - v0).cross(v2 - v0);

            backface[f] = N.dot(P - v0) < 0;
        }
    }
}


void MeshAlg::identifyBackfaces(
    const Array<Vector3>&           vertexArray,
    const Array<MeshAlg::Face>&     faceArray,
    const Vector4&                  HP,
    Array<bool>&                    backface,
    const Array<Vector3>&           faceNormals) {

    Vector3 P = HP.xyz();

    backface.resize(faceArray.size());

    if (fuzzyEq(HP.w, 0.0)) {
        // Infinite case
        for (int f = faceArray.size() - 1; f >= 0; --f) {
            const Vector3& N = faceNormals[f];
            backface[f] = N.dot(P) < 0;
        }
    } else {
        // Finite case
        for (int f = faceArray.size() - 1; f >= 0; --f) {
            const MeshAlg::Face& face = faceArray[f];        
            const Vector3& v0 = vertexArray[face.vertexIndex[0]];
            const Vector3& N = faceNormals[f];

            backface[f] = N.dot(P - v0) < 0;
        }
    }
}


void MeshAlg::createIndexArray(int n, Array<int>& array, int start, int run, int skip) {
    debugAssert(skip >= 0);
    debugAssert(run >= 0);

    array.resize(n);
    if (skip == 0) {
        for (int i = 0; i < n; ++i) {
            array[i] = start + i;
        }
    } else {
        int rcount = 0;
        int j = start;
        for (int i = 0; i < n; ++i) {
            array[i] = j;

            ++j;
            ++rcount;

            if (rcount == run) {
                rcount = 0;
                j += skip;
            }
        }
    }
}


void MeshAlg::computeAreaStatistics(
    const Array<Vector3>&   vertexArray,
    const Array<int>&       indexArray,
    double&                 minEdgeLength,
    double&                 meanEdgeLength,
    double&                 medianEdgeLength,
    double&                 maxEdgeLength,
    double&                 minFaceArea,
    double&                 meanFaceArea,
    double&                 medianFaceArea,
    double&                 maxFaceArea) {

    debugAssert(indexArray.size() % 3 == 0);

    Array<double> area(indexArray.size() / 3);
    Array<double> length(indexArray.size());

    for (int i = 0; i < indexArray.size(); i += 3) {
        const Vector3& v0 = vertexArray[indexArray[i]];
        const Vector3& v1 = vertexArray[indexArray[i + 1]];
        const Vector3& v2 = vertexArray[indexArray[i + 2]];

        area[i / 3] = (v1 - v0).cross(v2 - v0).length() / 2.0;
        length[i] = (v1 - v0).length();
        length[i + 1] = (v2 - v1).length();
        length[i + 2] = (v0 - v2).length();
    }

    area.sort();
    length.sort();

    minEdgeLength = max(0, length[0]);
    maxEdgeLength = max(0, length.last());
    medianEdgeLength = max(0, length[length.size() / 2]);
    meanEdgeLength = 0;
    for (int i = 0; i < length.size(); ++i) {
        meanEdgeLength += length[i];
    }
    meanEdgeLength /= length.size();

    minFaceArea = max(0, area[0]);
    maxFaceArea = max(0, area.last());
    medianFaceArea = max(0, area[area.size() / 2]);
    meanFaceArea = 0;
    for (int i = 0; i < area.size(); ++i) {
        meanFaceArea += area[i];
    }
    meanFaceArea /= area.size();


    // Make sure round-off hasn't pushed values less than zero
    meanFaceArea   = max(0, meanFaceArea);
    meanEdgeLength = max(0, meanEdgeLength);
}


int MeshAlg::countBoundaryEdges(const Array<MeshAlg::Edge>& edgeArray) {
    int b = 0;

    for (int i = 0; i < edgeArray.size(); ++i) {
        if ((edgeArray[i].faceIndex[0] == MeshAlg::Face::NONE) !=
            (edgeArray[i].faceIndex[1] == MeshAlg::Face::NONE)) {
            ++b;
        }
    }

    return b;
}


void MeshAlg::computeBounds(
    const Array<Vector3>&   vertexArray,
    Box&                    box, 
    Sphere&                 sphere) {

    Vector3 xmin, xmax, ymin, ymax, zmin, zmax;

    // FIRST PASS: find 6 minima/maxima points
    xmin.x = ymin.y = zmin.z = inf;
    xmax.x = ymax.y = zmax.z = -inf;

    for (int v = 0; v < vertexArray.size(); ++v) {
        const Vector3& vertex = vertexArray[v];

        if (vertex.x < xmin.x) {
    		xmin = vertex;
        }

        if (vertex.x > xmax.x) {
    		xmax = vertex;
        }

        if (vertex.y < ymin.y) {
    		ymin = vertex;
        }

        if (vertex.y > ymax.y) {
		    ymax = vertex;
        }

        if (vertex.z < zmin.z) {
		    zmin = vertex;
        }

        if (vertex.z > zmax.z) {
		    zmax = vertex;
        }
	}

    // Set points dia1 & dia2 to the maximally separated pair
    Vector3 dia1 = xmin; 
    Vector3 dia2 = xmax;
    {
        // Set xspan = distance between the 2 points xmin & xmax (squared)
        double xspan = (xmax - xmin).squaredLength();

        // Same for y & z spans
        double yspan = (ymax - ymin).squaredLength();
        double zspan = (zmax - zmin).squaredLength();
    
        double maxspan = xspan;

        if (yspan > maxspan) {
	        maxspan = yspan;
	        dia1    = ymin;
            dia2    = ymax;
	    }

        if (zspan > maxspan) {
            maxspan = zspan;
    	    dia1    = zmin;
            dia2    = zmax;
	    }
    }


    // dia1, dia2 is a diameter of initial sphere

    // calc initial center
    Vector3 center = (dia1 + dia2) / 2.0;

    // calculate initial radius^2 and radius 
    Vector3 d = dia2 - sphere.center;

    double radSq = d.squaredLength();
    double rad  = sqrt(radSq);

    // SECOND PASS: increment current sphere
    double old_to_p, old_to_new;

    for (int v = 0; v < vertexArray.size(); ++v) {
        const Vector3& vertex = vertexArray[v];

        d = vertex - center;

        double old_to_p_sq = d.squaredLength();

    	// do r^2 test first 
        if (old_to_p_sq > radSq) {
		 	// this point is outside of current sphere
    		old_to_p = sqrt(old_to_p_sq);

    		// calc radius of new sphere
		    rad = (rad + old_to_p) / 2.0;

            // for next r^2 compare
		    radSq = rad * rad; 	
		    old_to_new = old_to_p - rad;

		    // calc center of new sphere
            center = (rad * center + old_to_new * vertex) / old_to_p;
		}	
	}

    box = Box(Vector3(xmin.x, ymin.y, zmin.z), Vector3(xmax.x, ymax.y, zmax.z));
    sphere = Sphere(center, rad);
}


void MeshAlg::computeTangentVectors(
    const Vector3&  normal,
    const Vector3   position[3],
    const Vector2   texCoord[3],
    Vector3&        tangent, 
    Vector3&        binormal) {

    Vector3 v[3];
    Vector2 t[3];
    
    for (int i = 0; i < 3; ++i) {
        v[i] = position[i];
        t[i] = texCoord[i];
    }

    /////////////////////////////////////////////////
    // Begin by computing the tangent

    // Sort the vertices by texture coordinate y.
    if (t[0].y < t[1].y) {
        std::swap(v[0], v[1]);
        std::swap(t[0], t[1]);
    }

    if (t[0].y < t[2].y) {
        std::swap(v[0], v[2]);
        std::swap(t[0], t[2]);
    }

    if (t[1].y < t[2].y) {
        std::swap(v[1], v[2]);
        std::swap(t[1], t[2]);
    }

    double amount;

    // Compute the direction of constant y.
    if (fuzzyEq(t[2].y, t[0].y)) {
        amount = 1.0;
    } else {
        amount = (t[1].y - t[0].y) / (t[2].y - t[0].y);
    }

    tangent = lerp(v[0], v[2], amount) - v[1];

    // Make sure the tangent points in the right direction and is 
    // perpendicular to the normal.
    if (lerp(t[0].x, t[2].x, amount) < t[1].x) {
        tangent = -tangent;
    }

    // TODO: do we need this?  We take this component off
    // at the end anyway
    tangent -= tangent.dot(normal) * normal;

    // Normalize the tangent so it contributes
    // equally at the vertex (TODO: do we need this?)
    if (fuzzyEq(tangent.length(), 0.0)) {
        tangent = Vector3::UNIT_X;
    } else {
        tangent = tangent.direction();
    }

    //////////////////////////////////////////////////
    // Now compute the binormal

    // Sort the vertices by texture coordinate x.
    if (t[0].x < t[1].x) {
        std::swap(v[0], v[1]);
        std::swap(t[0], t[1]);
    }

    if (t[0].x < t[2].x) {
        std::swap(v[0], v[2]);
        std::swap(t[0], t[2]);
    }

    if (t[1].x < t[2].x) {
        std::swap(v[1], v[2]);
        std::swap(t[1], t[2]);
    }

    // Compute the direction of constant x.
    if (fuzzyEq(t[2].x, t[0].x)) {
        amount = 1.0;
    } else {
        amount = (t[1].x - t[0].x) / (t[2].x - t[0].x);
    }

    binormal = lerp(v[0], v[2], amount) - v[1];

    // Make sure the binormal points in the right direction and is 
    // perpendicular to the normal.
    if (lerp(t[0].y, t[2].y, amount) < t[1].y) {
        binormal = -binormal;
    }

    binormal -= binormal.dot(normal) * normal;

    // Normalize the binormal so that it contributes
    // an equal amount to the per-vertex value (TODO: do we need this? 
    // Nelson Max showed that we don't for computing per-vertex normals)
    if (fuzzyEq(binormal.length(), 0.0)) {
        binormal = Vector3::UNIT_Z;
    } else {
        binormal = binormal.direction();
    }

}


void MeshAlg::computeTangentSpaceBasis(
    const Array<Vector3>&       vertexArray,
    const Array<Vector2>&       texCoordArray,
    const Array<Vector3>&       vertexNormalArray,
    const Array<Face>&          faceArray,
    Array<Vector3>&             tangent,
    Array<Vector3>&             binormal) {

    Vector3 position[3];
    Vector2 texCoord[3];
    Vector3 normal;
    Vector3 t, b;

    tangent.resize(vertexArray.size());
    binormal.resize(vertexArray.size());
    // Zero the arrays.
    System::memset(tangent.getCArray(), 0, sizeof(Vector3) * tangent.size());
    System::memset(binormal.getCArray(), 0, sizeof(Vector3) * binormal.size());

    for (int f = 0; f < faceArray.size(); ++f) {
        const Face& face = faceArray[f];

        for (int v = 0; v < 3; ++v) {
            int i = face.vertexIndex[v];
            position[v] = vertexArray[i];
            texCoord[v] = texCoordArray[i];
        }

        normal = (position[1] - position[0]).cross(position[2] - position[0]).direction();
        computeTangentVectors(normal, position, texCoord, t, b);

        // We average the tangent and binormal vectors as if they were
        // normals.

        for (int v = 0; v < 3; ++v) {
            int i = face.vertexIndex[v];
            tangent[i] += t;
            binormal[i] += b;
        }
    }

    // Normalize the basis vectors
    for (int v = 0; v < vertexArray.size(); ++v) {
        // Remove the component parallel to the normal
        const Vector3& N = vertexNormalArray[v];
        tangent[v]  -= tangent[v].dot(N) * N;
        binormal[v] -= binormal[v].dot(N) * N;

        // Note that the tangent and binormal might not be perpendicular anymore
        tangent[v]  = tangent[v].direction();
        binormal[v] = binormal[v].direction();
    }
}



} // G3D namespace
