/**
  @file IFSModel.h
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite Original IFS code by Nate Robbins

  @created 2003-11-12
  @edited  2003-12-07
 */ 


#ifndef GLG3D_IFSMODEL_H
#define GLG3D_IFSMODEL_H

#include "graphics3D.h"
#include "GLG3D/PosedModel.h"

namespace G3D {

typedef ReferenceCountedPointer<class IFSModel> IFSModelRef;

/**
 Loads the IFS file format.  Note that you can convert 
 many other formats (e.g. 3DS, SM, OBJ, MD2) to IFS format
 using the IFSBuilder sample code provided with G3D.

 IFS models are geometric meshes; they don't have texture
 coordinates, animation, or other data and are primarily
 useful for scientific research.
 */
class IFSModel : public ReferenceCountedObject {
private:
    class PosedIFSModel : public PosedModel {
    public:
        IFSModelRef             model;
        CoordinateFrame         cframe;
        bool                    perVertexNormals;

        PosedIFSModel(IFSModelRef _model, const CoordinateFrame& _cframe, bool _pvn);
        virtual ~PosedIFSModel() {}
        virtual std::string name() const;
        virtual void getCoordinateFrame(CoordinateFrame&) const;
        virtual void getObjectSpaceGeometry(MeshAlg::Geometry& geometry) const;
        virtual void getWorldSpaceGeometry(MeshAlg::Geometry& geometry) const;
        virtual void getFaces(Array<MeshAlg::Face>& faces) const;
        virtual void getEdges(Array<MeshAlg::Edge>& edges) const;
        virtual void getAdjacentFaces(Array< Array<int> >& adjacentFaces) const;
        virtual void getTriangleIndices(Array<int>& indices) const;
        virtual void getObjectSpaceBoundingSphere(Sphere&) const;
        virtual void getObjectSpaceBoundingBox(Box&) const;
        virtual void render(RenderDevice* renderDevice) const;
        virtual int numBrokenEdges() const;
    };

    friend class PosedIFSModel;

    std::string                 filename;
    MeshAlg::Geometry           geometry;
    Array<int>                  indexArray;
    Array<Vector3>              faceNormalArray;
    Array<MeshAlg::Face>        faceArray;
    Array< Array<int> >         adjacentFaceArray;
    Array<MeshAlg::Edge>        edgeArray;
    Sphere                      boundingSphere;
    Box                         boundingBox;
    int                         numBrokenEdges;
    std::string                 name;

    /** Shared by all models */
    static VARAreaRef           varArea;

    /** Only called from create */
    IFSModel();
    
    /** Only called from create */
    void load(const std::string& filename, double scale);

    /** Only called from create */
    void reset();

public:

    virtual ~IFSModel();

    /**
     Throws an std::string describing the error if anything
     goes wrong.
     */
    static IFSModelRef create(const std::string& filename, double scale = 1);

    /**
     If perVertexNormals is false, the model is rendered with per-face normals,
     which are slower.
     */
    virtual PosedModelRef pose(const CoordinateFrame& cframe, bool perVertexNormals = true);

    virtual size_t mainMemorySize() const;
};

}

#endif
