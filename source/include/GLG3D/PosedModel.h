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
 */
class PosedModel : public ReferenceCountedObject {
protected:
    PosedModel() {}

public:

    virtual ~PosedModel() {}

    virtual std::string name() const = 0;

    /** Object to world space coordinate frame. */
    virtual CoordinateFrame coordinateFrame() const = 0;

    /** Get the <B>object space</B> geometry (faster than getWorldSpaceGeometry). */
    virtual void getObjectSpaceGeometry(MeshAlg::Geometry& geometry) const = 0;

    /** Get the <B>world space</B> geometry. */
    virtual void getWorldSpaceGeometry(MeshAlg::Geometry& geometry) const = 0;

    virtual void getFaces(Array<MeshAlg::Face>& faces) const = 0;

    virtual void getEdges(Array<MeshAlg::Edge>& edges) const = 0;

    virtual void getAdjacentFaces(Array< Array<int> >& adjacentFaces) const = 0;

    virtual Sphere objectSpaceBoundingSphere() const = 0;

    virtual Sphere worldSpaceBoundingSphere() const = 0;

    virtual Box objectSpaceBoundingBox() const = 0;

    virtual Box worldSpaceBoundingBox() const = 0;

    virtual void render(RenderDevice* renderDevice) const = 0;
    /**
     Number of edges that have only one adjacent face.
     */
    virtual int numBrokenEdges() const = 0;
};

}

#endif
