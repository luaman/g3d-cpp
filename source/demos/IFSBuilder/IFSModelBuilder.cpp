/**
  @file IFSBuilder/IFSModelBuilder.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2003-10-06
 */

#include "IFSModelBuilder.h"
#include "IFSModel.h"

const double IFSModelBuilder::CLOSE = IFSModelBuilder::AUTO_WELD;


void IFSModelBuilder::setName(const std::string& n) {
    name = n;
}


double close;

void IFSModelBuilder::commit(IFSModel* model) {
    model->name = name;

    // Make the data fit in a unit cube
    centerTriList();

    Array<int> toNew, toOld;

    close = CLOSE;
    if (close == IFSModelBuilder::AUTO_WELD) {
        Array<int> index;
        MeshAlg::createIndexArray(triList.size(), index);
        double minEdgeLen, maxEdgeLen, meanEdgeLen, medianEdgeLen;
        double minFaceArea, maxFaceArea, meanFaceArea, medianFaceArea;
        MeshAlg::computeAreaStatistics(triList, index,
            minEdgeLen, meanEdgeLen, medianEdgeLen, maxEdgeLen,
            minFaceArea, meanFaceArea, medianFaceArea, maxFaceArea);
        close = minEdgeLen * 0.5;
    }

    MeshAlg::computeWeld(triList, model->geometry.vertexArray, toNew, toOld, close);

    // Construct triangles
    for (int t = 0; t < triList.size(); t += 3) {
        IFSModel::Triangle tri;

        for (int i = 0; i < 3; ++i) {
           tri.index[i] = toNew[t + i];
        }

        // Throw out zero size triangles
        if ((tri.index[0] != tri.index[1]) &&
            (tri.index[1] != tri.index[2]) &&
            (tri.index[2] != tri.index[0])) {
            model->triangleArray.append(tri);
        }
    }


    // Trilist reformatted as an index array
    Array<int> indexArray(model->triangleArray.size() * 3);
    for (int t = 0, i = 0; t < model->triangleArray.size(); ++t, i += 3) {
        for (int j = 0; j < 3; ++j) {
            indexArray[i + j] = model->triangleArray[t].index[j];
        }
    }

    Array<MeshAlg::Face> faceArray;
    Array<Array<int> >   adjacentFaceArray;
    Array<Vector3>       faceNormalArray;
    MeshAlg::computeAdjacency(model->geometry.vertexArray, indexArray, faceArray, model->edgeArray, adjacentFaceArray);
    MeshAlg::computeNormals(model->geometry.vertexArray, faceArray, adjacentFaceArray, model->geometry.normalArray, faceNormalArray);

    // Find broken edges
    model->brokenEdgeArray.resize(0);
    for (int e = 0; e < model->edgeArray.size(); ++e) {
        const MeshAlg::Edge& edge = model->edgeArray[e];

        debugAssert(edge.vertexIndex[0] != edge.vertexIndex[1]);

        if ((edge.faceIndex[1] == MeshAlg::Face::NONE) ||
            (edge.faceIndex[0] == MeshAlg::Face::NONE) ||
            (edge.faceIndex[0] == edge.faceIndex[1])) {
            // Dangling edge
            model->brokenEdgeArray.append(edge);
        } else {
            // Each vertex must appear in each adjacent face.  If it doesn't, something
            // has gone wrong.
            int numFound = 0;
            // Check each vertex
            for (int i = 0; i < 2; ++i) {
                // Check each face
                for (int j = 0; j < 2; ++j) {
                    const int f = edge.faceIndex[j];
                    const MeshAlg::Face& face = faceArray[f];
                    for (int j = 0; j < 3; ++j) {
                        if (model->geometry.vertexArray[face.vertexIndex[j]] == 
                            model->geometry.vertexArray[edge.vertexIndex[i]]) {
                            ++numFound;
                            break;
                        }
                    }
                }
            }

            if (numFound < 4) {
                model->brokenEdgeArray.append(edge);
            }
        }
    }
}


void IFSModelBuilder::centerTriList() {
    // Compute the range of the vertices
    Vector3 vmin, vmax;

    computeBounds(vmin, vmax);

    Vector3 diagonal = vmax - vmin;
    double scale = max(max(diagonal.x, diagonal.y), diagonal.z) / 2;
    debugAssert(scale > 0);

    Vector3 translation = vmin + diagonal / 2;

    // Center and scale all vertices in the input list
    int v;
    for (v = 0; v < triList.size(); ++v) {
        triList[v] = (triList[v] - translation) / scale;
    }
}


void IFSModelBuilder::computeBounds(Vector3& min, Vector3& max) {
    min = Vector3::INF3; 
    max = -min;

    int v;
    for (v = 0; v < triList.size(); ++v) {
        min = min.min(triList[v]);
        max = max.max(triList[v]);
    }
}


void IFSModelBuilder::addTriangle(const Vector3& a, const Vector3& b, const Vector3& c) {
    triList.append(a, b, c);
}


void IFSModelBuilder::addTriangle(const Triangle& t) {
    triList.append(t.vertex(0), t.vertex(1), t.vertex(2));
}
