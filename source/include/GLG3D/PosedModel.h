/**
  @file PosedModel.h
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-11-15
  @edited  2003-11-15
 */ 


#ifndef GLG3D_POSEDMODEL_H
#define GLG3D_POSEDMODEL_H

#include "graphics3D.h"

namespace G3D {

typedef ReferenceCountedPointer<class PosedModel> PosedModelRef;

/**
 Base class for posed models.

 G3D does not provide a scene graph structure because the visible
 surface determination, rendering, and caching behavior of a scene
 graph are different for each application.  There is currently no 
 one-size-fits-all scene graph design pattern for us to implement.
 
 G3D <B>does</B> provide some basic infrastructure for making your
 own scene graph, however.  Model classes (e.g. G3D::IFSModel, G3D::MD2Model),
 which have no common base class, should all implement a <CODE>pose</CODE> 
 method.  The arguments to this method are all of the parameters (e.g. 
 animation frame, limb position) needed for reducing the general purpose,
 poseable model to a specific world space mesh for rendering.  This
 instance specific mesh is a G3D::PosedModel.  Because all PosedModels
 have the same interface, they can be used interchangably.

 Use G3D::PosedModelWrapper to encapsulate an existing posed model
 with your own.
 */
class PosedModel : public ReferenceCountedObject {
protected:

    PosedModel() {}

public:

    virtual ~PosedModel() {}

    virtual std::string name() const = 0;

    /** Object to world space coordinate frame.*/
    virtual void getCoordinateFrame(CoordinateFrame& c) const = 0;

    virtual CoordinateFrame coordinateFrame() const;

    /** Get the <B>object space</B> geometry (faster than getWorldSpaceGeometry). */
    virtual void getObjectSpaceGeometry(MeshAlg::Geometry& geometry) const = 0;

    /** Get the <B>world space</B> geometry. */
    virtual void getWorldSpaceGeometry(MeshAlg::Geometry& geometry) const;

    virtual void getFaces(Array<MeshAlg::Face>& faces) const = 0;

    virtual void getEdges(Array<MeshAlg::Edge>& edges) const = 0;

    virtual void getAdjacentFaces(Array< Array<int> >& adjacentFaces) const = 0;

    virtual void getTriangleIndices(Array<int>& indices) const = 0;

    virtual void getObjectSpaceBoundingSphere(Sphere&) const = 0;

    virtual Sphere objectSpaceBoundingSphere() const;

    virtual void getWorldSpaceBoundingSphere(Sphere& s) const;

    virtual Sphere worldSpaceBoundingSphere() const;

    virtual void getObjectSpaceBoundingBox(Box&) const = 0;

    virtual Box objectSpaceBoundingBox() const;

    virtual void getWorldSpaceBoundingBox(Box& box) const;

    virtual Box worldSpaceBoundingBox() const;

    virtual void render(class RenderDevice* renderDevice) const = 0;
    /**
     Number of edges that have only one adjacent face.
     */
    virtual int numBrokenEdges() const = 0;
};

}

#endif
