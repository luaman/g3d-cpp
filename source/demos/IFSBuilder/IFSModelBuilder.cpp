/**
  @file IFSBuilder/IFSModelBuilder.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2003-09-18
 */

#include "IFSModelBuilder.h"
#include "IFSModel.h"

const double IFSModelBuilder::CLOSE = 0.02;

void IFSModelBuilder::setName(const std::string& n) {
    name = n;
}


void IFSModelBuilder::commit(IFSModel* model) {
    model->name = name;

    // Make the data fit in a unit cube
    centerTriList();

    // Prime the vertex list
    for (int i = 0; i < triList.size(); ++i) {
        getIndex(triList[i], model);
    }

    // Construct triangles
    for (int t = 0; t < triList.size(); t += 3) {
        IFSModel::Triangle tri;

        for (int i = 0; i < 3; ++i) {
           tri.index[i] = getIndex(triList[t + i], model);
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

    // TODO: computeNormals doesn't need face array, it should take index array

    // Find broken edges
    model->brokenEdgeArray.resize(0);
    for (int e = 0; e < model->edgeArray.size(); ++e) {
        const MeshAlg::Edge& edge = model->edgeArray[e];

        debugAssert(edge.vertexIndex[0] != edge.vertexIndex[1]);

        if ((edge.faceIndex[1] == MeshAlg::Face::NONE) || 
            (edge.faceIndex[0] == edge.faceIndex[1])) {
            // Dangling edge
            model->brokenEdgeArray.append(edge);
        } else {
            // Each vertex must appear in each adjacent face
            int numFound = 0;
            // Check each vertex
            for (int i = 0; i < 2; ++i) {
                // Check each face
                for (int j = 0; j < 2; ++j) {
                    const int f = edge.faceIndex[j];
                    const MeshAlg::Face& face = faceArray[f];
                    for (int j = 0; j < 3; ++j) {
                        if (face.vertexIndex[j] == edge.vertexIndex[i]) {
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


/**
 Computes the grid index from an ordinate on the range [-1, 1]
 */
static int gridCoord(double x) {
    return iClamp(iFloor((x + 1) * 0.5 * (GRID_RES - 1)), 0, GRID_RES - 1);
}


uint32 hashCode(const IFSModelBuilder::List* x) {
    return (uint32)x;
}


int IFSModelBuilder::getIndex(const Vector3& v, IFSModel* model) {

    int closestIndex = -1;
    double distanceSquared = inf;

    int ix = gridCoord(v.x);
    int iy = gridCoord(v.y);
    int iz = gridCoord(v.z);

    // Check against all vertices within CLOSE of this grid cube
    const List& list = grid[ix][iy][iz];

    for (int i = 0; i < list.size(); ++i) {
        double d = (model->geometry.vertexArray[list[i]] - v).squaredLength();

        if (d < distanceSquared) {
            distanceSquared = d;
            closestIndex = list[i];
        }
    }

    if (distanceSquared <= CLOSE * CLOSE) {

        return closestIndex;

    } else {

        // This is a new vertex
        int newIndex = model->geometry.vertexArray.size();
        model->geometry.vertexArray.append(v);

        // Create a new vertex and store its index in the
        // neighboring grid cells (usually, only 1 neighbor)

        Set<List*> neighbors;

        debugAssertM(CLOSE < (2.0 / GRID_RES), "CL must be less than one grid cell's width");

        int ix = gridCoord(v.x);
        int iy = gridCoord(v.y);
        int iz = gridCoord(v.z);
        neighbors.insert(&(grid[ix][iy][iz]));

        for (int dx = -1; dx <= +1; ++dx) { 
            int ix = gridCoord(v.x + dx * CLOSE);
            for (int dy = -1; dy <= +1; ++dy) {
                int iy = gridCoord(v.y + dy * CLOSE);
                for (int dz = -1; dz <= +1; ++dz) { 
                    int iz = gridCoord(v.z + dz * CLOSE);
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
