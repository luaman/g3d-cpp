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


class VideoSerializer {
private:

    void serializeRaw(BinaryOutput& b) const;
    void deserializeRaw(BinaryInput& b);

    /** Called after the width, height, and format have been read. */
    void serializeTimeDiff1(BinaryOutput& b) const;
    void deserializeTimeDiff1(BinaryInput& b);

public:

    /**
     ENCODING_RAW       
       Frames are serialized as:
                  
            uint32      encoding (will be ENCODING_RAW)
            uint32      format
            uint32      width
            uint32      height
            uint32      numBytes (will be width*height*bytesPerPixel(format))
            uint8[numBytes] uint8's that are the frame data

     <P>
     ENCODING_TIME_DIFF1 
            The maximum compression is 8:3, the minimum is 8:11.  Compression is
            best when frames resemble the previous one.

            uint32      encoding (will be ENCODING_TIME_DIFF1)
            uint32      format
            uint32      width
            uint32      height
            uint32      numBytes
            uint8[numBytes] Frame data as deltas from previous frame.
                        Data is packed into uint3's, with values as follows,
                        where 'P' is the corresponding byte position
                        in the previous frame:

                          0   P - 3
                          1   P - 2
                          2   P - 1
                          3   P + 0
                          4   P + 1
                          5   P + 2
                          6   P + 3
                          7   ESC

                        If the value is 7 (ESC), then the following 8 bits
                        are the actual value of the byte.  To end on a byte
                        boundary the final byte will be padded with unspecified
                        values.
     */ 
    enum Encoding {
        ENCODING_RAW,
        ENCODING_TIME_DIFF1,
        ENCODING_SPACE_DIFF1
    };

    /**
     R8G8B8             Every pixel contains 8 bits of red, 8 bits of green, and 8 bits of blue.
     <P>
     BAYER_R8G8_G8B8    Pixels are interpreted based on their position.  Even rows, starting with row 0,
                        alternate between red and green values.  Odd rows alternate between green and blue.
                        The width and height of the frame describes the total number of pixels, regardless of 
                        their color.
    <P>
     L8                 8-bit monochrome.
     */
    enum Format {
        FORMAT_R8G8B8,
        FORMAT_L8,
        FORMAT_BAYER_R8G8_G8B8,
    };

    /** Number of bytes in a pixel.  Note that Bayer pixels are monochrome pixels that
        alternate in interpretation.*/
    static int bytesPerPixel(Format f) {
        switch (f) {
        case FORMAT_R8G8B8:
            return 3;
        default:
            return 1;
        }
    }

    void serialize(BinaryOutput& b) const;
    void deserialize(BinaryInput& b);

    uint8*              currentFrame;
    uint8*              previousFrame;

    /** Number of pixel columns, not necessarily the number of bytes in that row.*/
    int                 width;
    int                 height;

    /** The encoding that the system will attempt to use first when sending data.
        Not guaranteed to be followed. */
    Encoding            preferredEncoding;

    /** Format of currentFrame and previousFrame */
    Format              frameFormat;
};


void VideoSerializer::serialize(BinaryOutput& b) const {
    Encoding encoding = preferredEncoding;

    if ((encoding == ENCODING_TIME_DIFF1) && (previousFrame == NULL)) {
        // There was no previous frame, so switch to a different encoding.
        encoding = ENCODING_RAW;
    }
    b.writeUInt32(encoding);
    b.writeUInt32(frameFormat);
    b.writeUInt32(width);
    b.writeUInt32(height);

    switch (encoding) {
    case ENCODING_RAW:
        serializeRaw(b);
        break;

    case ENCODING_TIME_DIFF1:
        serializeTimeDiff1(b);
        break;
    }
}




void VideoSerializer::deserialize(BinaryInput& b)  {
    Encoding encoding = (Encoding)b.readUInt32();
    frameFormat = (Format)b.readUInt32();
    width = b.readUInt32();
    height = b.readUInt32();

    switch (encoding) {
    case ENCODING_RAW:
        deserializeRaw(b);
        break;

    case ENCODING_TIME_DIFF1:
        deserializeTimeDiff1(b);
        break;
    }
}


void VideoSerializer::serializeRaw(BinaryOutput& b) const {
    uint32 n = width * height * bytesPerPixel(frameFormat);
    b.writeUInt32(n);
    b.writeBytes(currentFrame, n);
}


void VideoSerializer::deserializeRaw(BinaryInput& b) {
    uint32 n = width * height * bytesPerPixel(frameFormat);
    int x = b.readUInt32();
    debugAssert(x == n);
    b.readBytes(currentFrame, n);
}


void VideoSerializer::serializeTimeDiff1(BinaryOutput& b) const {
    uint32 n = width * height * bytesPerPixel(frameFormat);
    
    // Reserve space for the size header
    int sizePos = b.getPosition();
    b.writeUInt32(0);

    b.beginBits();

    // Iterate over the bytes in the frames
    for (int i = 0; i < n; ++i) {
        int P = previousFrame[i];
        int C = currentFrame[i];

        int delta = C - P;

        if (iAbs(delta) > 3) {
            // Escape
            b.writeBits(7, 3);
            b.writeBits(C, 8);
        } else {
            // Send delta
            b.writeBits(3 + delta, 3);
        }
    }

    b.endBits();

    int endPos = b.getPosition();
    b.setPosition(sizePos);
    b.writeUInt32(endPos - sizePos - 4);
    b.setPosition(endPos);
}


void VideoSerializer::deserializeTimeDiff1(BinaryInput& b) {
    uint32 n = width * height * bytesPerPixel(frameFormat);

    debugAssert(currentFrame != NULL);
    debugAssert(previousFrame != NULL);

    // Read the byte size (although we won't use it).
    int size = b.readUInt32();

    b.beginBits();

    for (int i = 0; i < n; ++i) {
        int a = b.readBits(3);

        if (a < 7) {
            int delta = a - 3;
            currentFrame[i] = previousFrame[i] + delta;
        } else {
            // Escape; the following 8 bits are the real value.
            // We can't read a uint8 because the 8 bits will
            // rarely be aligned on a byte boundary.
            currentFrame[i] = b.readBits(8);
        }
    }

    b.endBits();
}







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

    TextureRef          im;

    App(const GAppSettings& settings);

    ~App();
};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));


    GImage frame0("c:/tmp/brunette-walk-I_P-009.png");
    GImage frame1("c:/tmp/brunette-walk-I_P-010.png");

    debugAssert(frame0.channels == 3);
    debugAssert(frame1.channels == 3);

    VideoSerializer s;
    s.frameFormat = VideoSerializer::FORMAT_R8G8B8;
    s.width = frame0.width;
    s.height = frame0.height;
    s.preferredEncoding = VideoSerializer::ENCODING_TIME_DIFF1;
    s.previousFrame = frame0.byte();
    s.currentFrame = frame1.byte();

    int n = frame0.width * frame0.height * 3;
    int num0 = 0, num1 = 0, num2 = 0, num3 = 0;

    for (int i = 0; i < n; ++i) {
        switch (iAbs(frame1.byte()[i] - frame0.byte()[i])) {
        case 0:
            ++num0;
            break;
        case 1:
            ++num1;
            break;
        case 2:
            ++num2;
            break;
        case 3:
            ++num3;
            break;
        }
    }

    uint8* x = (uint8*)malloc(1024 * 1024 * 10);
    int compressedSize = 0;
    {
        BinaryOutput b("<memory>", G3D_LITTLE_ENDIAN);
        s.serialize(b);
        compressedSize = b.length();
        b.commit(x);
    }

    int rawSize = frame0.width * frame0.height * 3;

    GImage frame1decompressed(frame1.width, frame1.height, 3);
    s.currentFrame = frame1decompressed.byte();
    {
        BinaryInput b(x, compressedSize, G3D_LITTLE_ENDIAN);
        s.deserialize(b);
    }

    for (int i = 0; i < 8000; ++i) {
        debugAssert(frame1decompressed.byte()[i] == frame1.byte()[i]);
    }

    app->im = Texture::fromFile("c:/tmp/brunette-walk-I_P-009.png");
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
        app->sky->render(lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

       
    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(lighting);
    }

    app->renderDevice->push2D();
        app->renderDevice->setTexture(0, app->im);
        Draw::rect2D(Rect2D::xywh(0,0,800,600), app->renderDevice);
    app->renderDevice->pop2D();
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    // Load objects here
    sky = Sky::create(renderDevice, dataDir + "sky/");
    
    applet->run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}


int main(int argc, char** argv) {

    GAppSettings settings;
    settings.window.width = 800;
    settings.window.height = 700;
    App(settings).run();
    return 0;
}
