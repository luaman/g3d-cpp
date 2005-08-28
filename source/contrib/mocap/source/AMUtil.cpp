/**
  Helper methods for ArticulatedModels.
 */

#include "AMUtil.h"


ArticulatedModelRef createIFSModel(const std::string& filename, Color3 color, const CoordinateFrame& cframe) {
    ArticulatedModelRef model = ArticulatedModel::fromFile(filename, cframe);

    SuperShader::Material& material = model->partArray[0].triListArray[0].material;
    material.diffuse = color;
    material.transmit = Color3::black();
    material.reflect = Color3::black();
    material.specular = Color3::white() * .05;
    material.specularExponent = Color3::white() * 10;
    model->updateAll();

    return model;
}



ArticulatedModelRef createHelicopter(Color3 color) {
    // Animated, hierarchical part
    ArticulatedModelRef body  = ArticulatedModel::fromFile("D:/games/cpp/source/data/ifs/ah64-body.ifs", 2);

    // Rotate the rotor to the correct orientation on load
    ArticulatedModelRef rotor = ArticulatedModel::fromFile("D:/games/cpp/source/data/ifs/ah64-rotor.ifs", 
        CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitX(), toRadians(-90)) * 2, Vector3::zero()));

    ArticulatedModelRef model = ArticulatedModel::createEmpty();
    model->name = "Apache";

    SuperShader::Material material;
    material.diffuse = color;
    material.transmit = Color3::black();
    material.reflect = Color3::black();
    material.specular = Color3::white() * .3;
    material.specularExponent = Color3::white() * 30;

    {
        ArticulatedModel::Part& part = model->partArray.next();
        part = body->partArray.last();
        part.name = "Root";
        // Make a reference to the child we're going to add
        part.subPartArray.append(1);
        part.triListArray[0].material = material;
    }

    {
        ArticulatedModel::Part& part = model->partArray.next();
        part = rotor->partArray.last();
        part.parent = 0;
        part.name = "Rotor";
        part.cframe = CoordinateFrame(Vector3(0.0, 0.3,- 0.5));
        part.triListArray[0].material = material;
    }


    model->updateAll();

    return model;
}


ArticulatedModelRef createPlaneModel(const std::string& textureFile, float side, float tilePeriod) {
    Log::common()->printf("Creating plane...");
    ArticulatedModelRef model = ArticulatedModel::createEmpty();

    model->name = "Ground Plane";
    ArticulatedModel::Part& part = model->partArray.next();
    part.cframe = CoordinateFrame();
    part.name = "root";

    Log::common()->printf("1,\n");
    // There are N^2 blocks in this ground plane
    const int N = ceil(side);
    {
        const Vector3 scale   = Vector3(1, 1, 1) * side / N;
        const Vector3 center  = Vector3(1, 0, 1) * side / 2;
        const double texScale = 1 / tilePeriod;

        for (int z = 0; z <= N; ++z) {
            for (int x = 0; x <= N; ++x) {
                part.geometry.vertexArray.append(Vector3(x, 0, z) * scale - center);
                part.texCoordArray.append(Vector2(x - N/2.0, z - N/2.0) * texScale);
            }
        }
    }

    Log::common()->printf("2,\n");
    // Build triangle list
    ArticulatedModel::Part::TriList& triList = part.triListArray.next();
    triList.indexArray.clear();

    const int M = N + 1;
    for (int z = 0; z < N; ++z) {
        for (int x = 0; x < N; ++x) {
            triList.indexArray.append(x + z * M, x + (z + 1) * M, (x + 1) + (z + 1) * M);
            triList.indexArray.append((x + 1) + (z + 1) * M, (x + 1) + z * M, x + z * M);
        }
    }

    triList.twoSided = true;
    triList.material.emit.constant = Color3::black();

    triList.material.specular.constant = Color3::black();
    triList.material.specularExponent.constant = Color3::white() * 60;
    triList.material.reflect.constant = Color3::black();

    triList.material.diffuse.constant = Color3::white();
    triList.material.diffuse.map = Texture::fromFile(textureFile, TextureFormat::AUTO, Texture::TILE,
        Texture::TRILINEAR_MIPMAP, Texture::DIM_2D, 1.0, Texture::DEPTH_NORMAL, 8.0);

    triList.computeBounds(part);
    Log::common()->printf("3,\n");

    part.indexArray = triList.indexArray;

    part.computeIndexArray();
    part.computeNormalsAndTangentSpace();
    part.updateVAR();
    part.updateShaders();

    Log::common()->printf("done\n");

    return model;
}


void conoid(
    const Vector3&              c1,
    const Vector2&              r1,
    const Vector3&              c2,
    const Vector2&              r2,
    const Vector3&              direction,
    const SuperShader::Material& material,
    ArticulatedModel::Part&     part) {

    // Index of the first new vertex
    int i0 = part.geometry.vertexArray.size();

    // Faces per side
    const int N = 16;

    // Sides
    {
        Array<Vector3>& vertex   = part.geometry.vertexArray;
        Array<Vector2>& texCoord = part.texCoordArray;

        // Go around a circle creating the side vertices. We intentionally double the 
        // first vertex to provide correct texture coordinates.
        for (int a = 0; a <= N; ++a) {
            float angle = ((float)a / N) * G3D_TWO_PI - G3D_PI;

            const Vector2 p(cos(angle), sin(angle));

            // Top
            const Vector3 v1 = Vector3(p * r1, 0) + c1;

            // Bottom
            const Vector3 v2 = Vector3(p * r2, 0) + c2;

            const Vector2 x1((float)a / N, 0.5);
            const Vector2 x2((float)a / N, 1.0);

            vertex.append(v1, v2);
            texCoord.append(x1, x2);
        }
    }

    // Top
    {
        Array<Vector3>& vertex   = part.geometry.vertexArray;
        Array<Vector2>& texCoord = part.texCoordArray;

        // Go around a circle creating the side vertices. We intentionally double the 
        // first vertex to provide correct texture coordinates.
        for (int a = 0; a <= N; ++a) {
            float angle = ((float)a / N) * G3D_TWO_PI - G3D_PI;

            const Vector2 p(cos(angle), sin(angle));
    
            vertex.append(Vector3(p * r1, 0) + c1);
            texCoord.append((Vector2(-p.x, p.y) / 4 + Vector2(0.25, 0.25)) + Vector2(0.5, 0.0));
        }
    }


    // Bottom
    {
        Array<Vector3>& vertex   = part.geometry.vertexArray;
        Array<Vector2>& texCoord = part.texCoordArray;

        // Go around a circle creating the side vertices. We intentionally double the 
        // first vertex to provide correct texture coordinates.
        for (int a = 0; a <= N; ++a) {
            float angle = ((float)a / N) * G3D_TWO_PI - G3D_PI;

            const Vector2 p(cos(angle), sin(angle));
    
            vertex.append(Vector3(p * r2, 0) + c2);
            texCoord.append((p / 4 + Vector2(0.25, 0.25)) + Vector2(0.0, 0.0));
        }
    }

    {
        // Build triangle list for sides
        ArticulatedModel::Part::TriList& triList = part.triListArray.next();
        triList.indexArray.clear();

        for (int a = 0; a < N; ++a) {
            int i = 2 * a + i0;
            triList.indexArray.append(i, i + 1, i + 3);
            triList.indexArray.append(i, i + 3, i + 2);
        }

        triList.twoSided = false;
        triList.material = material;

        triList.computeBounds(part);
        part.indexArray.append(triList.indexArray);
    }


    {
        // Build triangle list for top
        ArticulatedModel::Part::TriList& triList = part.triListArray.next();
        triList.indexArray.clear();

        int i = 2 * (N + 1) + i0;
        for (int a = 1; a < N; ++a) {
            triList.indexArray.append(i, i + a, i + a + 1);
        }

        triList.twoSided = false;
        triList.material = material;

        triList.computeBounds(part);
        part.indexArray.append(triList.indexArray);
    }

    {
        // Build triangle list for bottom
        ArticulatedModel::Part::TriList& triList = part.triListArray.next();
        triList.indexArray.clear();

        int i = i0 + 3 * (N + 1);
        for (int a = 1; a < N; ++a) {
            triList.indexArray.append(i, i + a + 1, i + a);
        }

        triList.twoSided = false;
        triList.material = material;

        triList.computeBounds(part);
        part.indexArray.append(triList.indexArray);
    }

    // Make a local reference frame
    CoordinateFrame cframe;
    {
        Vector3 Z = direction;
        Vector3 X = Vector3::unitX();
        if (abs(Z.dot(X)) < .1) {
            X = -Vector3::unitY();
        }
        X = (X - Z * (X.dot(Z))).direction();
        Vector3 Y = Z.cross(X);
        cframe.rotation.setColumn(0, X);
        cframe.rotation.setColumn(1, Y);
        cframe.rotation.setColumn(2, Z);
    }

//    cframe.rotation = Matrix3::fromAxisAngle(Vector3::unitY(), toRadians(90));

    for (int i = i0; i < part.geometry.vertexArray.size(); ++i) {
        part.geometry.vertexArray[i] = cframe.pointToWorldSpace(part.geometry.vertexArray[i]);
    }

    /*
    part.computeIndexArray();
    part.computeNormalsAndTangentSpace();
    part.updateVAR();
    part.updateShaders();
    */
}


SuperShader::Material brass() {
    SuperShader::Material material;
    material.specular.constant = Color3::white();
    material.specularExponent.constant = Color3::white() * 60;
    material.reflect.constant = Color3::yellow() * 0.3;
    material.diffuse.constant = Color3::orange() * .5;
    return material;
}