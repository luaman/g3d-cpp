/**
  @file IFSModel.h
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @cite Original IFS code by Nate Robbins

  @created 2003-11-12
  @edited  2004-03-28
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
        bool                    useMaterial;
        GMaterial               material;

        PosedIFSModel(IFSModelRef _model, const CoordinateFrame& _cframe, bool _pvn,
            const GMaterial& _mat, bool _useMat);
        virtual ~PosedIFSModel() {}
        virtual std::string name() const;
        virtual void getCoordinateFrame(CoordinateFrame&) const;
        virtual const MeshAlg::Geometry& objectSpaceGeometry() const;
        virtual const Array<MeshAlg::Face>& faces() const;
        virtual const Array<MeshAlg::Edge>& edges() const;
        virtual const Array<MeshAlg::Vertex>& vertices() const;
        virtual const Array<MeshAlg::Face>& weldedFaces() const;
        virtual const Array<MeshAlg::Edge>& weldedEdges() const;
        virtual const Array<MeshAlg::Vertex>& weldedVertices() const;
        virtual const Array<int>& triangleIndices() const;
        virtual void getObjectSpaceBoundingSphere(Sphere&) const;
        virtual void getObjectSpaceBoundingBox(Box&) const;
        virtual void render(RenderDevice* renderDevice) const;
        virtual int numBoundaryEdges() const;
        virtual int numWeldedBoundaryEdges() const;
        virtual const Array<Vector3>& objectSpaceFaceNormals(bool normalize = true) const;
    };

    friend class PosedIFSModel;

    std::string                 filename;
    MeshAlg::Geometry           geometry;
    Array<int>                  indexArray;
    Array<Vector3>              faceNormalArray;
    Array<MeshAlg::Face>        weldedFaceArray;
    Array<MeshAlg::Edge>        weldedEdgeArray;
    Array<MeshAlg::Vertex>      weldedVertexArray;
    Array<MeshAlg::Face>        faceArray;
    Array<MeshAlg::Edge>        edgeArray;
    Array<MeshAlg::Vertex>      vertexArray;
    Sphere                      boundingSphere;
    Box                         boundingBox;
    int                         numBoundaryEdges;
    int                         numWeldedBoundaryEdges;
    std::string                 name;

    /** Shared by all models */
    static VARAreaRef           varArea;

    /** Only called from create */
    IFSModel();
    
    /** Only called from create */
    void load(const std::string& filename, const Vector3& scale, const CoordinateFrame& cframe);

    /** Only called from create */
    void reset();

public:

    virtual ~IFSModel();

    /**
     Throws an std::string describing the error if anything
     goes wrong.
     @param scale 3D scale factors to apply to vertices while loading (*after* cframe)
     @param cframe Coordinate transform to apply to vertices while loading.  
                   This is not part of the object to world transformation
                   for the model when posed; it really modifies the object
                   space geometry.
     */
    static IFSModelRef create(const std::string& filename, const Vector3& scale = Vector3(1,1,1), const CoordinateFrame& cframe = CoordinateFrame());
    static IFSModelRef create(const std::string& filename, const double scale, const CoordinateFrame& cframe = CoordinateFrame());

    /**
     If perVertexNormals is false, the model is rendered with per-face normals,
     which are slower.  No color or texture is set when rendered (that is,
     the existing RenderDevice values are used.)
     */
    virtual PosedModelRef pose(const CoordinateFrame& cframe, bool perVertexNormals = true);

    /**
     If perVertexNormals is false, the model is rendered with per-face normals,
     which are slower.
     */
    virtual PosedModelRef pose(const CoordinateFrame& cframe, const GMaterial& material, bool perVertexNormals = true);

    virtual size_t mainMemorySize() const;
};

}

#endif
