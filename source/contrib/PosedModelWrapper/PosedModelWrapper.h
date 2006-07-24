/**
 A wrapper that trampolines every method to an internal model.
 Use this as a base class when you have a PosedModelRef and 
 want to override its render() [or some other] method.
 @deprecated (Will be moved to contrib)
 */
class PosedModelWrapper : public PosedModel {
protected:

    PosedModelRef model;

    PosedModelWrapper(PosedModelRef _model) : model(_model) {}

public:

    virtual ~PosedModelWrapper() {}

    virtual std::string name() const;

    virtual void getCoordinateFrame(CoordinateFrame& c) const;

    virtual CoordinateFrame coordinateFrame() const;

    virtual const MeshAlg::Geometry& objectSpaceGeometry() const;

    virtual void getWorldSpaceGeometry(MeshAlg::Geometry& geometry) const;

    virtual const Array<MeshAlg::Face>& faces() const;

    virtual const Array<MeshAlg::Edge>& edges() const;

	virtual bool hasTexCoords() const;

    virtual const Array<Vector2>& texCoords() const;

    virtual const Array<MeshAlg::Vertex>& vertices() const;

    virtual const Array<MeshAlg::Face>& weldedFaces() const;

    virtual const Array<MeshAlg::Edge>& weldedEdges() const;

    virtual const Array<MeshAlg::Vertex>& weldedVertices() const;

    virtual void getObjectSpaceFaceNormals(Array<Vector3>& faceNormals, bool normalize = true) const;

    virtual void getWorldSpaceFaceNormals(Array<Vector3>& faceNormals, bool normalize = true) const;

	virtual const Array<Vector3>& objectSpaceFaceNormals(bool normalize = true) const;

    virtual const Array<int>& triangleIndices() const;

    virtual void getObjectSpaceBoundingSphere(Sphere&) const;

    virtual Sphere objectSpaceBoundingSphere() const;

    virtual void getWorldSpaceBoundingSphere(Sphere& s) const;

    virtual Sphere worldSpaceBoundingSphere() const;

    virtual void getObjectSpaceBoundingBox(Box&) const;

    virtual Box objectSpaceBoundingBox() const;

    virtual void getWorldSpaceBoundingBox(Box& box) const;

    virtual Box worldSpaceBoundingBox() const;

    virtual void render(class RenderDevice* renderDevice) const;

    virtual int numBoundaryEdges() const;

    virtual int numWeldedBoundaryEdges() const;

};


std::string PosedModelWrapper::name() const {
    return model->name();
}


void PosedModelWrapper::getCoordinateFrame(CoordinateFrame& c) const {
    model->getCoordinateFrame(c);
}


CoordinateFrame PosedModelWrapper::coordinateFrame() const {
    return model->coordinateFrame();
}


const MeshAlg::Geometry& PosedModelWrapper::objectSpaceGeometry() const {
    return model->objectSpaceGeometry();
}


void PosedModelWrapper::getWorldSpaceGeometry(MeshAlg::Geometry& geometry) const {
    model->getWorldSpaceGeometry(geometry);
}


void PosedModelWrapper::getObjectSpaceFaceNormals(Array<Vector3>& faceNormals, bool normalize) const {
    model->getObjectSpaceFaceNormals(faceNormals, normalize);
}


void PosedModelWrapper::getWorldSpaceFaceNormals(Array<Vector3>& faceNormals, bool normalize) const {
    model->getWorldSpaceFaceNormals(faceNormals, normalize);
}

const Array<Vector3>& PosedModelWrapper::objectSpaceFaceNormals(bool normalize) const {
	return model->objectSpaceFaceNormals(normalize);
}



const Array<MeshAlg::Face>& PosedModelWrapper::faces() const {
    return model->faces();
}


const Array<MeshAlg::Edge>& PosedModelWrapper::edges() const {
    return model->edges();
}


const Array<MeshAlg::Vertex>& PosedModelWrapper::vertices() const {
    return model->vertices();
}


const Array<Vector2>& PosedModelWrapper::texCoords() const {
    return model->texCoords();
}


bool PosedModelWrapper::hasTexCoords() const {
	return model->hasTexCoords();
}


const Array<MeshAlg::Face>& PosedModelWrapper::weldedFaces() const {
    return model->weldedFaces();
}


const Array<MeshAlg::Edge>& PosedModelWrapper::weldedEdges() const {
    return model->weldedEdges();
}


const Array<MeshAlg::Vertex>& PosedModelWrapper::weldedVertices() const {
    return model->weldedVertices();
}


const Array<int>& PosedModelWrapper::triangleIndices() const {
    return model->triangleIndices();
}


void PosedModelWrapper::getObjectSpaceBoundingSphere(Sphere& s) const {
    model->getObjectSpaceBoundingSphere(s);
}


Sphere PosedModelWrapper::objectSpaceBoundingSphere() const {
    return model->objectSpaceBoundingSphere();
}


void PosedModelWrapper::getWorldSpaceBoundingSphere(Sphere& s) const {
    model->getWorldSpaceBoundingSphere(s);
}


Sphere PosedModelWrapper::worldSpaceBoundingSphere() const {
    return model->worldSpaceBoundingSphere();
}


void PosedModelWrapper::getObjectSpaceBoundingBox(Box& box) const {
    model->getObjectSpaceBoundingBox(box);
}


Box PosedModelWrapper::objectSpaceBoundingBox() const {
    return model->objectSpaceBoundingBox();
}


void PosedModelWrapper::getWorldSpaceBoundingBox(Box& box) const {
    model->getWorldSpaceBoundingBox(box);
}


Box PosedModelWrapper::worldSpaceBoundingBox() const {
    return model->worldSpaceBoundingBox();
}


void PosedModelWrapper::render(class RenderDevice* renderDevice) const { 
    model->render(renderDevice);
}


int PosedModelWrapper::numBoundaryEdges() const {
    return model->numBoundaryEdges();
}


int PosedModelWrapper::numWeldedBoundaryEdges() const {
    return model->numWeldedBoundaryEdges();
}

////////////////////////////////////////////////////
