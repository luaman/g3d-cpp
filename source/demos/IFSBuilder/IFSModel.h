/**
  @file IFSBuilder/XIFSModel.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2002-10-16
 */ 

#ifndef XIFSModel_H
#define XIFSModel_H

#include <G3DAll.h>

class XIFSModel {
private:
    friend class IFSModelBuilder;

    class Triangle {
    public:
        int                 index[3];
    };

    MeshAlg::Geometry       geometry;
    Array<Triangle>         triangleArray;

    class Edge {
    public:
        int vertexIndex[2];
    };

    /**
     An edge is broken if it appears in
     only one face or if its faces do not
     contain the vertices (happens with colocated vertices)
     */
    Array<MeshAlg::Edge>     brokenEdgeArray;

    Array<MeshAlg::Edge>     edgeArray;

    /** G3D Indexed Face Set */
    void loadIFS(const std::string& filename);

    /** Quake II Model */
    void loadMD2(const std::string& filename);

    /** 3D Studio */
    void load3DS(const std::string& filename);

    /** Wavefront Object */
    void loadOBJ(const std::string& filename);

    /** Brown University Sketch Model */
    void loadSM(const std::string& filename);

    /** Algorithmically generate a twisted ring model */
    void createRing();

public:

    std::string             name;

    XIFSModel(const std::string& filename);
    XIFSModel() {}

    /**
     Render the model.
     */
    void render();

    int numVertices() const {
        return geometry.vertexArray.size();
    }

    int numFaces() const {
        return triangleArray.size();
    }

    int numBrokenEdges() const {
        return brokenEdgeArray.size();
    }

    /**
     Write the IFS file to disk.
     */
    void save(const std::string& filename);
};

#endif
