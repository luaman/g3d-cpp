/**
  @file IFSModel.h
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite Original IFS code by Nate Robbins

  @created 2003-11-12
  @edited  2003-11-12
 */ 


#ifndef GLG3D_IFSMODEL_H
#define GLG3D_IFSMODEL_H

#include "graphics3D.h"
#include "GLG3D/GModel.h"

namespace G3D {

/**
 Loads the IFS file format.  Note that you can convert 
 many other formats (e.g. 3DS, SM, OBJ, MD2) to IFS format
 using the IFSBuilder sample code provided with G3D.

 IFS models are geometric meshes; they don't have texture
 coordinates, animation, or other data and are primarily
 useful for scientific research.
 */
class IFSModel : public GModel {
private:

    std::string                 filename;
    MeshAlg::Geometry           geometry;
    Array<int>                  indexArray;
    Array<Vector3>              faceNormalArray;

    /** Shared by all models */
    static VARArea*             varArea;

    void reset();

public:

    IFSModel();
    virtual ~IFSModel();

    void getGeometry(MeshAlg::Geometry& geometry) const;

    /**
     Throws an std::string describing the error if anything
     goes wrong.
     */
    void load(const std::string& filename);

    /**
     If perVertexNormals is false, the model is rendered with per-face normals,
     which are slower.
     */
    void render(RenderDevice* renderDevice, bool perVertexNormals = true);

    virtual size_t mainMemorySize() const;
};

}

#endif
