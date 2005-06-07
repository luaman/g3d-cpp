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

#if G3D_VER < 60700
    #error Requires G3D 6.0
#endif


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*          app;

    Demo(App* app);

    virtual ~Demo() {}

    virtual void init();

    virtual void doLogic();

	virtual void doNetwork();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();

    virtual void cleanup();

};



class App : public GApp {
protected:
    void main();
public:
    SkyRef              sky;

    Demo*               applet;

    App(const GAppSettings& settings);

    ~App();
};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));
}


void Demo::cleanup() {
    // Called when Demo::run() exits
}


void Demo::doNetwork() {
	// Poll net messages here
}


void Demo::doSimulation(SimTime dt) {
	// Add physical simulation here
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

	// Add other key handling here
}


void Demo::doGraphics() {

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (app->sky.notNull()) {
        app->sky->render(app->renderDevice, lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);

    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(app->renderDevice, lighting);
    }
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    // Load objects here
    sky = Sky::create(NULL, dataDir + "sky/");
    
    applet->run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}


////////////////////////////

/** 
  Helper for writing files with extensible and discoverable formats.

  <B>Writing a Chunk:</B>

  <PRE>
    BinaryOutput b(...);
    enum MyFormat {HEIGHTS, POSITION};
    Array<int32> heights;
    ...

    Chunk c(b, HEIGHTS, Chunk::INT32, heights.size());
    for (int i = 0; i < heights.size(); ++i) {
        b.writeInt32(heights[i]);
    }
    c.finish();
   </PRE>

  <B>Reading a Chunk:</B>

  <HR NOSHADE>
  Chunks have the following binary format:

  <PRE>
  uint16            name (application defined code)
  uint16            format (Chunk::Format)
  uint32            size (not including these 8 header bytes)
  format[num]       ...data...
  </PRE>

  The number of data elements is <CODE>num = size / Chunk::byteSize(format)</CODE>.

 */
class Chunk {
private:

    enum {HEADER_SIZE = 8};

    /** Keeps track of whether this chunk is for reading or writing */
    enum {WRITE = 0, READ = 1} mode;
    
    bool                finished;

    int                 startPos;

public:

    enum Format {
        FIRST_FORMAT = 0,

        CUSTOM,

        BOOL, 
        UINT8, INT8, UINT16, INT16, UINT32, INT32, UINT64, INT64, 
        FLOAT32, FLOAT64,
        VECTOR2, VECTOR2INT16,
        VECTOR3, VECTOR3INT16,
        VECTOR4,
        COLOR3, COLOR3UINT8,
        COLOR4, COLOR4UINT8,
        STRING, STRING32,
    
        LAST_FORMAT
    };

    /** Returns -1 if the format is custom, otherwise the byte size
        of a single element in this format.*/ 
    static size_t byteSize(Format f) {
        switch (f) {
        case BOOL:
        case UINT8:
        case INT8:
            return 1;

        case UINT16:
        case INT16:
            return 2;

        case UINT32:
        case INT32:
        case FLOAT32:
            return 4;

        case FLOAT64:
        case UINT64:
        case INT64:
            return 8;

        case VECTOR2:
            return 2 * 4;

        case VECTOR2INT16:
            return 2 * 2;

        case VECTOR3:
            return 3 * 4;

        case VECTOR3INT16:
            return 3 * 2;

        case VECTOR4:
            return 4 * 4;

        case COLOR3:
            return 3 * 4;

        case COLOR3UINT8:
            return 3 * 1;

        default:
            return -1;
        }
    }

    /** Application defined code for this chunk. */
    uint16                      name;

    /** Format of the data payload. */
    Format                      format;

    /** Size of the data payload in bytes, excluding the 8 header bytes. */
    size_t                      size;

    /** Number of elements in the data payload. 1 if the format is CUSTOM.*/
    uint32                      count;

    /** Begin a read chunk.  After the constructor finishes
        you can access the member fields to discover the chunk
        type.
      */
    Chunk(G3D::BinaryInput& b) : mode(READ), finished(false) {

        name   = b.readUInt32();
        format = (Format)b.readUInt32();

        debugAssertM(format > FIRST_FORMAT && format < LAST_FORMAT, 
            "Chunk file is corrupted-- format tag is invalid.");

        size   = b.readUInt16();
        debugAssertM(size < b.getLength(), 
            "Chunk file is corrupted-- chunk is larger than file!");

        if (format != CUSTOM) {
            count  = size / byteSize(format);
        } else {
            count  = 1;
        }
    }

    /** Begin a write chunk.*/
    Chunk(uint16 n, Format f, uint32 cnt, G3D::BinaryOutput& b) : startPos(b.getPosition()), name(n), format(f), count(cnt), finished(false) {
        b.writeUInt16(name);
        b.writeUInt16(format);
        b.writeUInt32(cnt);

        // Leave space for the payload size
        b.skip(sizeof(uint32));
    }


    ~Chunk() {
        debugAssertM(finished, "Chunk went out of scope or destroyed without being finished or aborted.");
    }

    /** Stop writing to a chunk abruptly.  Avoids the assertion failure that will
        occur in the destructor if a chunk is destroyed without being finished. */
    void abort() {
        debugAssertM(! finished, "Aborted a chunk that was already finished.");
        finished = true;
    }

    /** End writing to the chunk and write the header. */
    void finish(G3D::BinaryOutput& b) {
        debugAssertM(mode == WRITE, "Called finish(BinaryOutput) on a read chunk");
        debugAssertM(! finished, "Called finish on the same chunk twice.");

        // Write the header
        uint32 currentPos = b.position();
        b.setPosition(startPos + HEADER_SIZE - 4);
        b.writeUInt32(currentPos - startPos - HEADER_SIZE);
        b.setPosition(currentPos);

        finished = true;
    }

    /** Convenience helper to write a single integer value chunk. 
        You can also construct a write chunk and write the data manually.*/
    static void write(G3D::BinaryOutput& b, uint16 name, Format fmt, int32 val) {
        debugAssert(fmt == INT32);

        uint32 currentPos = b.position();
        b.writeUInt16(name);
        b.writeUInt16(fmt);
        b.writeUInt32(byteSize(fmt));
        b.writeInt32(val);
    }


    static void write(G3D::BinaryOutput& b, uint16 name, Format fmt, uint32 val) {
        debugAssert(fmt == UINT32);

        uint32 currentPos = b.position();
        b.writeUInt16(name);
        b.writeUInt16(fmt);
        b.writeUInt32(byteSize(fmt));
        b.writeUInt32(val);
    }

    /** Convenience helper to write an array of values as a chunk. */
    template<class T> static void write(G3D::BinaryOutput& b, uint16 name, Format fmt, const Array<T>& val) {

    }
};

////////////////////////////



int main(int argc, char** argv) {
    GAppSettings settings;
    settings.useNetwork = false;
    App(settings).run();
    return 0;
}
