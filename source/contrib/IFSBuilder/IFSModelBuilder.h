/**
  @file IFSBuilder.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2004-09-09
 */ 
#ifndef IFSBUILDER_H
#define IFSBUILDER_H

#include <G3DAll.h>

/**
 Allows creation of optimized watertight meshes from unoptimized polygon soups.
 See also G3D::MeshAlg for algorithms that operate on the output.
 */
class IFSBuilder {
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

	bool _twoSided;

public:

	inline IFSBuilder(bool twoSided = false) : _twoSided(twoSided) {}

    /** Writes the model to the arrays, which can then be used with
        G3D::IFSModel::save and G3D::MeshAlg */
    void commit(std::string& name, Array<int>& indexArray, Array<Vector3>& vertexArray);

    /**
     Adds a new triangle to the model. (Counter clockwise)
     */
    void addTriangle(const Vector3& a, const Vector3& b, const Vector3& c);

    /**
     Adds two new triangles to the model. (Counter clockwise)
     */
    void addQuad(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d);

    void addTriangle(const Triangle& t);

    void setName(const std::string& n);

};


#endif
