/**
  @file IFSBuilder/IFSModelBuilder.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2003-09-22
 */ 


#ifndef IFSMODELBUILDER_H
#define IFSMODELBUILDER_H

#include <G3DAll.h>

// We construct a grid to find neighboring vertices in O(n/g^3) time
#define GRID_RES 1

/**
 Used by IFSModel for loading.
 */
class IFSModelBuilder {
public:
    /** Indices of vertices in <B>or near</B>  a grid cell. */
    typedef Array<int> List;

    /** Vertices that are within this distance of each other are considered
     close (colocated) */
    static const double CLOSE;

private:
   
    std::string                 name;


    List grid[GRID_RES][GRID_RES][GRID_RES];
    
    /**
     All of the triangles, as a long triangle list.
     */
    Array<Vector3>              triList;

    void centerTriList();
    void computeBounds(Vector3& min, Vector3& max);

    /** Gets the index of a vertex, adding it to the
        model's list if necessary. */
    int getIndex(const Vector3& v, class IFSModel* model);

public:

    /** Writes out the model data into the passed in object. */
    void commit(class IFSModel* model);

    /**
     Adds a new triangle to the model.
     */
    void addTriangle(const Vector3& a, const Vector3& b, const Vector3& c);
    void addTriangle(const Triangle& t);

    void setName(const std::string& n);

};


#endif
