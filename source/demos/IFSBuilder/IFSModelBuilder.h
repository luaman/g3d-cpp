/**
  @file IFSBuilder/IFSModelBuilder.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2003-10-18
 */ 


#ifndef IFSMODELBUILDER_H
#define IFSMODELBUILDER_H

#include <G3DAll.h>

/**
 Used by IFSModel for loading.
 */
class IFSModelBuilder {
public:
    /** Indices of vertices in <B>or near</B>  a grid cell. */
    typedef Array<int> List;

    static const double CLOSE;

    /**
     Set close to AUTO_WELD to weld vertices closer than 1/2
     the smalled edge length in a model.
     */
    enum {AUTO_WELD = -100};

private:
   
    std::string                 name;
    
    /**
     All of the triangles, as a long triangle list.
     */
    Array<Vector3>              triList;

    void centerTriList();
    void computeBounds(Vector3& min, Vector3& max);

public:

    /** Writes out the model data into the passed in object. */
    void commit(class XIFSModel* model);

    /**
     Adds a new triangle to the model.
     */
    void addTriangle(const Vector3& a, const Vector3& b, const Vector3& c);
    void addTriangle(const Triangle& t);

    void setName(const std::string& n);

};


#endif
