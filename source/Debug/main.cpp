
/**
  @file demos/main.cpp

  This is a sample main.cpp to get you started with G3D.  It is
  designed to make writing an application easy.  Although the
  GApp/GApplet infrastructure is helpful for most projects,
  you are not restricted to using it-- choose the level of
  support that is best for your project (see the G3D Map in the
  documentation).

  @author Morgan McGuire, matrix@graphics3d.com
 */

#include <G3DAll.h>

#if G3D_VER < 60500
    #error Requires G3D 6.05
#endif

#include "Load3DS.h"

GApp* app = NULL; 

typedef ReferenceCountedPointer<class ArticulatedModel> ArticulatedModelRef;

/**
 A model composed of a heirarchy of rigid, textured sub-models.
 */
class ArticulatedModel : public ReferenceCountedObject {
public:

    class Pose {
    public:
        /** Mapping from names to coordinate frames (relative to parent).
            If a name is not present, its coordinate frame is assumed to
            be the identity.
         */
        Table<std::string, CoordinateFrame>     cframe;
    };

    static const Pose DEFAULT_POSE;

private:

    Load3DS                     load;

    class Part {
    public:

        /** Each part must have a unique name */
        std::string             name;

        /** Root position, relative to parent. Has already been applied to vertices!*/
        CoordinateFrame         cframe;

        CoordinateFrame         keyframe;

        Array<Vector3>          vertexArray;
        Array<Vector2>          texCoordArray;
        Array<int>              indexArray;

        /** Indices into part array of sub-parts */
        Array<int>              subPartArray;

        TextureRef              texture1;

        /**
         Does not restore rendering state when done.
         @param Net frame of parent.
         */
        void render(RenderDevice* rd, const CoordinateFrame& parent, const Pose& pose) const;
    };

    /** Returns the index in partArray of the part with this name */
    Table<std::string, int>     partNameToIndex;
    Array<Part>                 partArray;

    /** Called from the constructor */
    void init(const std::string& filename);

public:

    PosedModelRef pose();

    void render(RenderDevice* rd, const Pose& pose = DEFAULT_POSE);

    /**  Supports 3DS, IFS, PLY file formats, detected by the extension. */
    static ArticulatedModelRef fromFile(const std::string& filename);
};

const ArticulatedModel::Pose ArticulatedModel::DEFAULT_POSE;


/////////////////////////////////////////////////////////

ArticulatedModelRef ArticulatedModel::fromFile(const std::string& filename) {
    ArticulatedModel* model = new ArticulatedModel();
    model->init(filename);
    return model;
}


void ArticulatedModel::init(const std::string& filename) {
    TextureManager textureManager;

    std::string path = filenamePath(filename);
    load.load(filename);

    partArray.resize(load.objectArray.size());
    for (int p = 0; p < load.objectArray.size(); ++p) {
        const Load3DS::Object& object = load.objectArray[p];
        Part& part = partArray[p];
        part.vertexArray = object.vertexArray;
        part.indexArray = object.indexArray;
        part.texCoordArray = object.texCoordArray;

        std::string name = object.name;
        int count = 0;
        while (partNameToIndex.containsKey(name)) {
            ++count;
            name = object.name + format("_#%d", count);
        }
        part.keyframe = object.keyframe.approxCoordinateFrame();
        part.name = name;
        partNameToIndex.set(part.name, p);

        if (object.faceMatArray.size() > 0) {
            const std::string& materialName = object.faceMatArray[0].materialName;

            if (load.materialNameToIndex.containsKey(materialName)) {
                int i = load.materialNameToIndex[materialName];
                const Load3DS::Material& material = load.materialArray[i];

                const Load3DS::Map& texture1 = material.texture1;

                std::string textureFile = texture1.filename;

                // Assume the first material applies to the entire part
                if (! fileExists(textureFile) && fileExists(path + textureFile)) {
                    textureFile = path + textureFile;
                }

                if (fileExists(textureFile)) {
                    part.texture1 = textureManager.loadTexture(textureFile);
                } else {
                    Log::common()->printf("Could not load texture '%s'\n", textureFile.c_str());
                }
            } else {
                Log::common()->printf("Referenced unknown material '%s'\n", materialName.c_str());
            }
        }
    }
}


void ArticulatedModel::render(RenderDevice* rd, const Pose& pose) {
    for (int p = 0; p < partArray.size(); ++p) {
        const Part& part = partArray[p];
        part.render(rd, CoordinateFrame(), pose);
    }
}


void ArticulatedModel::Part::render(
    RenderDevice*           rd,
    const CoordinateFrame&  parent, 
    const Pose&             pose) const {

    app->debugPrintf("%s", name.c_str());

    CoordinateFrame frame;

    if (pose.cframe.containsKey(name)) {
        frame = parent * keyframe * pose.cframe[name] * keyframe.inverse();
    } else {
        frame = parent;
    }

    rd->setObjectToWorldMatrix(frame);

    if (name == "m_rotor") {
        Draw::axes(keyframe, rd);
    }

    rd->setTexture(0, texture1);

    rd->beginPrimitive(RenderDevice::TRIANGLES);
    for (int i = 0; i < indexArray.size(); ++i) {
        int v = indexArray[i];
        if (texCoordArray.size() > 0) {
            rd->setTexCoord(0, texCoordArray[v]);
        }
        rd->sendVertex(vertexArray[v]);
    }
    rd->endPrimitive();

    // TODO: sub-parts
}


class App : public GApp {
protected:
    void main();
public:
    SkyRef              sky;
    ArticulatedModelRef model;
    TextureRef          texture;

    App(const GAppSettings& settings);
};

/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*					app;

    Demo(App* app);    

    virtual void init();

    virtual void doSimulation(RealTime dt);

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {

}


void Demo::init()  {
	// Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 0, 10));
    app->debugCamera.lookAt(Vector3(0, 0, 0));
}


void Demo::doSimulation(RealTime dt) {
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }
}


void Demo::doGraphics() {

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);
    app->renderDevice->setObjectToWorldMatrix(CoordinateFrame());

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(true, true, true);
    app->renderDevice->enableLighting();
    app->renderDevice->setLight(0, lighting.directionalLight());
    app->renderDevice->setLight(1, GLight::directional(-Vector3::unitY(), Color3::brown() * .5, false));
    app->renderDevice->setAmbientLightColor(lighting.ambient);

    ArticulatedModel::Pose pose;
    static RealTime t0 = System::time();

    RealTime t = (System::time() - t0) * 10;
    pose.cframe.set("m_rotor", 
        CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitY(), t),
                        Vector3::zero()));
    pose.cframe.set("t_rotor",
        CoordinateFrame(Matrix3::fromAxisAngle(Vector3::unitX(), t*2),
                        Vector3::zero()));

    app->renderDevice->setTexture(0, app->texture);
    app->model->render(app->renderDevice, pose);
    app->renderDevice->pushState();
        app->renderDevice->setTexture(0, NULL);
        app->renderDevice->setColor(Color3::black());
        app->renderDevice->setRenderMode(RenderDevice::RENDER_WIREFRAME);
        app->model->render(app->renderDevice, pose);
    app->renderDevice->popState();

    Draw::axes(app->renderDevice);
}



void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    model = ArticulatedModel::fromFile("c:/tmp/3ds/fs.3ds");
    texture = Texture::fromFile("d:/games/cpp/source/data/image/checkerboard.jpg");

    Demo(this).run();
}

App::App(const GAppSettings& settings) : GApp(settings) {
    ::app = this;
}


int main(int argc, char** argv) {
    GAppSettings settings;
    settings.window.depthBits = 24;
    settings.window.stencilBits = 8;
    settings.window.alphaBits = 0;
    settings.window.rgbBits = 8;
    settings.window.fsaaSamples = 0;
    settings.window.width = 400;
    settings.window.height = 400;
    App(settings).run();
    return 0;
}
