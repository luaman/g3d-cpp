/**
 @file GModel.h

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2003-11-12
 @edited  2003-11-12
 */

#ifndef GLG3D_GMODEL_H
#define GLG3D_GMODEL_H

#include "graphics3D.h"

namespace G3D {

/**
 Base class for IFSModel and MD2Model.  You aren't constrained to using
 this interface for your own models-- G3D does not impose a scene-graph
 structure on your program.
 */
class GModel {
protected:
    Array<MeshAlg::Face>        faceArray;

    Array< Array<int> >         adjacentFaceArray;

    Array<MeshAlg::Edge>        edgeArray;

    Sphere                      _boundingSphere;

    Box                         _boundingBox;

    int                         _numBrokenEdges;

    std::string                 _name;

    bool                        _initialized;

public:

    ~GModel() {}

    std::string name() const;
    virtual void setName(const std::string& n);

    const Array<MeshAlg::Face>&  faces() const;
    const Array<MeshAlg::Edge>&  geometricEdges() const;
    const Array< Array<int> >&   adjacentFaces() const;

    /**
     Returns true if this model has been loaded.
     */
    bool initialized() const;

    /**
     A bounding sphere on the model.  Covers all vertices in all animations.
     */
    const Sphere& boundingSphere() const;

    /**
     The number of edges for which there is only one adjacent face.
     */
    int numBrokenEdges() const;

    /**
     An oriented bounding box on the model.  Covers all vertices in all animations.
     */
    const Box& boundingBox() const;

    /**
     Returns the approximate amount of <B>main</B> memory occupied by this data structure.
     */
    virtual size_t mainMemorySize() const = 0;
};

}

#endif




