/**
  @file MeshAlg.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2003-09-14
  @edited  2003-11-15

  Copyright 2000-2003, Morgan McGuire.
  All rights reserved.

 */

#include "G3D/MeshAlg.h"
#include "G3D/Table.h"
#include "G3D/Set.h"
#include "G3D/Box.h"
#include "G3D/Sphere.h"

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


int MeshAlg::countBrokenEdges(const Array<MeshAlg::Edge>& edgeArray) {
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

} // G3D namespace
