/**
  @file IFSBuilder/IFSModelBuilder.cpp

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2002-02-27
 */

#include "IFSModelBuilder.h"
#include "IFSModel.h"

void IFSModelBuilder::setName(const std::string& n) {
    name = n;
}


void IFSModelBuilder::commit(IFSModel* model) {
    model->name = name;

    // Make the data fit in a unit cube
    centerTriList();

    for (int t = 0; t < triList.size(); t += 3) {
        IFSModel::Triangle tri;

        for (int i = 0; i < 3; ++i) {
           tri.index[i] = getIndex(triList[t + i], model);
        }

        // Throw out zero size triangles, but keep sliver
        // triangles that may be fixing T-junctions.
        if ((tri.index[0] != tri.index[1]) || (tri.index[1] != tri.index[2]) || (tri.index[2] != tri.index[0])) {
            model->triangleArray.append(tri);
        }
    }
}


/**
 Computes the grid index from an ordinate.
 */
static int gridCoord(double x) {
    return iMax(0, iMin(GRID_RES - 1, iFloor((x + 1) * 0.5 * (GRID_RES - 1))));
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

    const List& list = grid[ix][iy][iz];

    for (int i = 0; i < list.size(); ++i) {
        double d = (model->vertexArray[list[i]] - v).squaredLength();

        if (d < distanceSquared) {
            distanceSquared = d;
            closestIndex = list[i];
        }
    }

    if (distanceSquared <= CLOSE) {

        return closestIndex;

    } else {

        // This is a new vertex
        int newIndex = model->vertexArray.size();
        model->vertexArray.append(v);

        // Create a new vertex and store its index in the
        // neighboring grid cells. 

        Set<List*> neighbors;

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
