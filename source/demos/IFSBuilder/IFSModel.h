/**
  @file IFSBuilder/IFSModel.h

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2002-04-03
 */ 

#ifndef IFSMODEL_H
#define IFSMODEL_H

#include <G3DAll.h>

class IFSModel {
private:
    friend class IFSModelBuilder;

    class Triangle {
    public:
        int                 index[3];
    };

    Array<Vector3>          vertexArray;
    Array<Triangle>         triangleArray;

    void loadIFS(const std::string& filename);
    void loadMD2(const std::string& filename);
    void load3DS(const std::string& filename);
    void loadOBJ(const std::string& filename);

public:

    std::string             name;

    IFSModel(const std::string& filename);
    IFSModel() {}

    /**
     Render the model.
     */
    void render();

    /**
     Write the IFS file to disk.
     */
    void save(const std::string& filename);
};

#endif
