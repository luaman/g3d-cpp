/**
  @file RenderDevice.h

  Graphics hardware abstraction layer (wrapper for OpenGL).

  Because per-pixel lighting interacts with texture units, this does
  not abstract lighting.  If you want OpenGL lights, you have to
  enable/disable and configure them yourself (render device will track
  the glEnable(GL_LIGHTING) for you, however).

  In general, you can't use OpenGL or GLU calls in combination with
  RenderDevice or you will corrupt the state.  However, you *can* use:

  <PRE>
    glEnable(GL_LIGHTn)
    glDisable(GL_LIGHTn)
    glHint
    glPolyMode
    glLight
    glTexGen
    glEnable(GL_FOG)
    glFog
    glPixelZoom
    glRasterPos2d
    glDrawPixels
  </PRE>

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2001-05-29
  @edited  2003-04-07
*/

#ifndef GLG3D_RENDERDEVICE_H
#define GLG3D_RENDERDEVICE_H

#include "graphics3D.h"
#include "GLG3D/Texture.h"

typedef unsigned int uint;

namespace G3D {

/**
 Number of hardware texture units to track state for.
 */
#define MAX_TEXTURE_UNITS 8

/**
 Parameters controlling the viewport and scissor region for
 rendering.
 */
class Rect2D {
public:
    int                                 x;
    int                                 y;
    int                                 width;
    int                                 height;

    Rect2D() : x(0), y(0), width(0), height(0) {}
    Rect2D(
        double x,
        double y,
        double width,
        double height) : 
        x((int)x),
        y((int)y),
        width((int)width),
        height((int)height) {}
};



/**
 A memory chunk of VAR space.  


 <P> A large buffer is allocated in video memory when the VAR system
 is initialized.  This buffer can be partitioned into multiple
 VARAreas.  Vertex arrays are uploaded to these areas by creating VAR
 objects.  Once used, those vertex arrays are dropped from memory by
 calling reset() on the corresponding VARArea.

 <P> Typically, two VARAreas are created.  One is a dynamic area that
 is reset every frame, the other is a static area that is never reset.
 */
class VARArea {
private:
	friend class VAR;

	/** Pointer to the memory. */
	void*				basePointer;

	/** Total  number of bytes in this area. */
	size_t				size;
	
	/** Number of bytes allocated. */
	size_t				allocated;

	/**
	 These prevent vertex arrays that have been freed from
	 accidentally being used.
	 */
	uint64				generation;

	/** The maximum size of this area that was ever used. */
	size_t				peakAllocated;

public:

	VARArea(void* _basePointer, size_t _size);

	size_t totalSize() const;

	size_t freeSize() const;

	size_t allocatedSize() const;

	size_t peakAllocatedSize() const;

	/** */ 
	void reset();
};


/**
 Rendering interface that abstracts OpenGL.  OpenGL is a basically
 good API with some rough spots.  Three of these are addressed by
 RenderDevice.  First, OpenGL state management is both tricky and
 potentially slow.  Second, OpenGL functions are difficult to use
 because many extensions have led to an evolutionary rather than
 designed API.  For type safety, new enums are introduced for values
 instead of the traditional OpenGL GLenum's, which are just ints.
 Third, OpenGL intialization is complicated.  This interface
 simplifies it significantly.

 <P> On Windows (_WIN32) RenderDevice supports a getHDC() method that
 returns the HDC for the window.

 <P>
 Example
  <PRE>
   RenderDevice renderDevice = new RenderDevice();
   renderDevice->init(width, height);
  </PRE>

 RenderDevice requires SDL and OpenGL.

  <P>
  Example 2 (textured quad)
  <PRE>
    RenderDevice* renderDevice = new RenderDevice();
    renderDevice->init(640, 480);

    TextureRef sprite = new Texture("Grass Texture", "image.jpg");

    renderDevice->beginFrame();
    renderDevice->pushState();
    renderDevice->clear(true, true, true);
    renderDevice->setCullFace(RenderDevice::CULL_NONE);
    renderDevice->setProjectionMatrix3D(-.2, .2, -.15, .15, .2, 200);
    renderDevice->setTexture(0, sprite);
    renderDevice->setColor(Color3::WHITE);
    renderDevice->beginPrimitive(RenderDevice::QUADS);
        renderDevice->setTexCoord(0,  Vector2(0, 1));
        renderDevice->sendVertex(Vector3(-3, -3, -5));
        
        renderDevice->setTexCoord(0,  Vector2(1, 1));
        renderDevice->sendVertex(Vector3( 3, -3, -5));
        
        renderDevice->setTexCoord(0,  Vector2(1, 0));
        renderDevice->sendVertex(Vector3( 3,  3, -5));

        renderDevice->setTexCoord(0,  Vector2(0, 0));
        renderDevice->sendVertex(Vector3(-3,  3, -5));
    renderDevice->endPrimitive();
    renderDevice->popState();

    renderDevice->endFrame();

    while (true);

    renderDevice->cleanup();
    </PRE>
 */
class VAR;

class RenderDevice {
public:
    enum Primitive {LINES, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP,
                    TRIANGLE_FAN, QUADS, QUAD_STRIP, POINTS};

private:

    friend class VAR;

	class VARSystem {
	private:


		/**
		 Vertex Array method
		 */
		enum VARMethod {VAR_NONE, VAR_NVIDIA, VAR_MALLOC};

		/** The base pointer for the entire VAR system */
		void*					basePointer;

		/** Size of the memory block referenced by varPtr */
		size_t					size;

		/** Number of bytes that have been allocated to VARAreas. */
		size_t					allocated;

		/**
		 We allocate a dynamic buffer that is uploaded every frame and
		 a static buffer that is changed much less frequently.
		 */
		VARMethod				method;

		Log*					debugLog;

		/**
		 All of the areas that have been allocated; stored so they can
		 be deleted when the var system shuts down.
		 */
		Array<class VARArea*>	areaList;

	public:

		VARSystem(
			RenderDevice*		rd,
			size_t				_size, 
			Log*				debugLog);


		~VARSystem();


		/**
		 Once allocated, VARAreas cannot be deallocated.  They are
		 automatically deleted when you shut down the VAR system.
		 */
		class VARArea* createArea(size_t areaSize);

		void beginIndexedPrimitives() const;

		void sendIndices(Primitive primitive, size_t indexSize,
                         int numIndices, const void* index) const;

		void setVertexArray(const class VAR& v) const;

		void setNormalArray(const class VAR& v) const;

		void setColorArray(const class VAR& v) const;

		void setTexCoordArray(unsigned int unit, const class VAR& v) const;

		void endIndexedPrimitives() const;
	};


	VARSystem*					varSystem;

    /**
     Status and debug statements are written to this log.
     */
    class Log*                  debugLog;

    /**
     The current GLGeom generation.  Used for vertex arrays.
     */
    uint32                      generation;

    /**
     The set of supported OpenGL extensions.
     */
    Set<std::string>            extensionSet;


    void setGamma(
        double                  brightness,
        double                  gamma);

    // Window parameters
	int                         screenWidth;
	int                         screenHeight;
	int                         stencilBits;

    /**
     Actual number of depth bits.
     */
    int                         depthBits;

    /**
     The intensity at which lights saturate.
     */
    double                      lightSaturation;

	void setVideoMode(  
        int                     width,
        int                     height, 
		int                     minimumDepthBits,
        int                     desiredDepthBits, 
		int                     minimumStencilBits,
        int                     desiredStencilBits,
        int                     colorBits,
        int                     alphaBits,
        bool                    fullscreen);

    /**
     Initialize the OpenGL extensions.
     */
    void initGLExtensions();

    /**
     True if EXT_stencil_wrap is in the extension list.
     */
    bool                        stencilWrapSupported;

    /**
     True if GL_NV_texture_rectangle is in the extension list.
     */
    bool                        textureRectangleSupported;

    /**
     For counting the number of beginFrame/endFrames.
     */
    int                         beginEndFrame;

    /** Sets the texture matrix without checking to see if it needs to
        be changed.*/
    void forceSetTextureMatrix(int unit, const double* m);

    /** Time at which the previous endFrame() was called */
    double                      lastTime;

    /** Exponentially weighted moving average frame rate */
    double                      emwaFrameRate;

    /** Argument to last beginPrimitive() */
    Primitive                   currentPrimitive;

    /** Number of vertices since last beginPrimitive() */
    int                         currentPrimitiveVertexCount;
    
    /** Number of triangles since last beginFrame() */
    int                         triangleCount;

    double                      emwaTriangleCount;
    double                      emwaTriangleRate;

	/** Updates the polygon count based on the primitive */
	void countPrimitive(RenderDevice::Primitive primitive, int numVertices);


    ////////////////////////////////////////////////////////////////////
public:
	/**
	 Initializes OpenGL via SDL.
	 */
    RenderDevice();

    /**
     Checkmarks all rendering state.  
     */
    void pushState();

    /**
     Sets all state to a clean rendering environment.
     (Doesn't affect Geometry objects)
     */
    void resetState();

    /**
     Restores all state to whatever was pushed previously.
     */
    void popState();

    void clear(bool clearColor, bool clearDepth, bool clearStencil);

    enum DepthTest   {DEPTH_GREATER,     DEPTH_LESS,       DEPTH_GEQUAL,  
                      DEPTH_LEQUAL,      DEPTH_NOTEQUAL,   DEPTH_EQUAL,   
                      DEPTH_ALWAYS_PASS, DEPTH_NEVER_PASS};

    enum AlphaTest   {ALPHA_GREATER,     ALPHA_LESS,       ALPHA_GEQUAL,  
                      ALPHA_LEQUAL,      ALPHA_NOTEQUAL,   ALPHA_EQUAL,  
                      ALPHA_ALWAYS_PASS, ALPHA_NEVER_PASS};

    enum StencilTest {STENCIL_GREATER,   STENCIL_LESS,     STENCIL_GEQUAL,
                      STENCIL_LEQUAL,    STENCIL_NOTEQUAL, STENCIL_EQUAL, 
                      STENCIL_ALWAYS_PASS, STENCIL_NEVER_PASS};

    enum BlendFunc   {BLEND_SRC_ALPHA,   BLEND_ONE_MINUS_SRC_ALPHA, BLEND_ONE,
                      BLEND_ZERO, BLEND_SRC_COLOR,  BLEND_DST_COLOR,  
                      BLEND_ONE_MINUS_SRC_COLOR};

    enum StencilOp   {STENCIL_INCR_WRAP, STENCIL_DECR_WRAP,         
                      STENCIL_KEEP,          STENCIL_INCR,     STENCIL_DECR};

    enum CullFace    {CULL_FRONT,        CULL_BACK,           CULL_NONE};

    enum ShadeMode   {SHADE_FLAT,        SHADE_SMOOTH};

    enum CombineMode {TEX_REPLACE, TEX_INTERPOLATE, TEX_ADD, TEX_MODULATE, 
                      TEX_BLEND};

    /**
     Call to begin the rendering frame.
     */
    void beginFrame();

    /**
     Call to end the current frame and swap buffers.
     */
    void endFrame();

    /**
     Returns an estimate of the number of frames rendered per second.
     The result is smoothed using an exponentially weighted moving
     average filter so it is robust to unequal frame rendering times.
     */
    double getFrameRate() const;

    /**
     Returns an estimate of the triangles rendered per second.  The
     result is smoothed using an exponentially weighted moving average
     filter.
     */
    double getTriangleRate() const;

    /**
     Returns an estimate of the triangles rendered per frame.  The
     result is smoothed using an exponentially weighted moving average
     filter.
     */
    double getTrianglesPerFrame() const;

    /**
     Use ALWAYS_PASS to shut off testing
     */
    void setDepthTest(DepthTest test);
    void setStencilTest(StencilTest test, int reference);
    void setAlphaTest(AlphaTest test, double reference);

    void setDepthRange(double low, double high);

    void enableColorWrite();
    void disableColorWrite();

    void enableAlphaWrite();
    void disableAlphaWrite();

    void enableDepthWrite();
    void disableDepthWrite();

    /**
     Equivalent to glShadeModel
     */
    void setShadeMode(ShadeMode s);

    /**
     If wrapping is not supported on the device, the nearest mode is
     selected.  Unlike OpenGL, stencil writing and testing are
     completely independent in this API.

     Use KEEP, KEEP, KEEP to disable stencil writing.  Equivalent to a
      combination of glStencilTest, glStencilFunc, and glStencilOp
     */
    void setStencilOp(
        StencilOp                       fail,
        StencilOp                       zfail,
        StencilOp                       zpass);

    /**
     Use BLEND_ZERO, BLEND_ONE to shut off blending.
     Equivalent to glBlendFunc
     */
    void setBlendFunc(
        BlendFunc                       src,
        BlendFunc                       dst);

    /**
     Equivalent to glLineWidth
     */
    void setLineWidth(
        double                          width);

    /**
     This is not the OpenGL MODELVIEW matrix: it is a matrix that maps
     object space to world space.  The actual MODELVIEW matrix
     is cameraToWorld.inverse() * objectToWorld.
     */
    void setObjectToWorldMatrix(
        const CoordinateFrame&          cFrame);

    CoordinateFrame getObjectToWorldMatrix() const;

    /**
     See RenderDevice::setObjectToWorldMatrix.
     */
    void setCameraToWorldMatrix(
        const CoordinateFrame&          cFrame);

    CoordinateFrame getCameraToWorldMatrix() const;

    /**
     Equivalent to glFrustum. A reasonable way to compute values is:

     <PRE>
        double pixelAspect = device->getWidth() / (double)device->getHeight();
        double y = nearPlane * tan(fieldOfView / 2);
        double x = y * pixelAspect;
        device->setProjectionMatrix3D(-x, x, -y, y, nearPlane, farPlane);
     </PRE>

     Some reasonable values are:
     <PRE>
      device->setProjectionMatrix3D(-.2, .2, -.15, .15, .2, 200);
     </PRE>

     The f value may be infinite (inf)
     */
    void setProjectionMatrix3D(
        double l,
        double r,
        double b,
        double t,
        double n,
        double f);

    /** Returns the current projection matrix parameters.  Is3D is true
        if this is a perspective projection matrix (i.e. setProjectionMatrix3D was called 
        to create it). */
    void getProjectionMatrixParams(
        double& l,
        double& r,
        double& b,
        double& t,
        double& n,
        double& f,
        bool&   is3D);

    void setProjectionMatrix2D(
        double l,
        double r,
        double b,
        double t,
        double n,
        double f);

    /**
     m is a 16-element matrix in row major order for multiplying
     texture coordinates:

     v' = M v

     All texture operations check textureUnit against the number of
     available texture units when in debug mode.

     Equivalen to glMatrixMode(GL_TEXTURE); glLoadMatrix(...);
     */
    void setTextureMatrix(
        uint                    textureUnit,
        const double*           m);

    void setTextureMatrix(
        uint                    textureUnit,
        const CoordinateFrame&  c);

    /**
     The combine mode specifies how to combine the result of a texture
     lookup with the accumulated fragment value (e.g. the output of
     the previous combine or the constant color for the first texture
     unit).

     The initial combine op is TEX_MODULATE 
     Equivalent to glTexEnvn.
     */
    void setTextureCombineMode(
        uint                      textureUnit,
        const CombineMode         texCombine);


    /**
     Resets the matrix, texture, combine op, and constant for a texture unit.
     */
    void resetTextureUnit(
        uint                      textureUnit);

    /**
     Equivalent to glPolygonOffset
     */
    void setPolygonOffset(
        double                  offset);

    /**
     Set the vertex color (equivalent to glColor).
     */
    void setColor(const Color4& color);
    void setColor(const Color3& color);

    /**
     Equivalent to glNormal
     */
    void setNormal(const Vector3& normal);

    /**
     Equivalent to glTexCoord
     */
    void setTexCoord(uint textureUnit, const Vector4& texCoord);
    void setTexCoord(uint textureUnit, const Vector3& texCoord);
    void setTexCoord(uint textureUnit, const Vector2& texCoord);
    void setTexCoord(uint textureUnit, double texCoord);

    /**
     Equivalent to glCullFace
     */
    void setCullFace(CullFace f);

    /**
     Number that the use must multiply all light intensities by 
     to account for the device's brightness.
     */
    inline double getBrightScale() const {
        return brightScale;
    }

    void setViewport(const Rect2D& v);

    /**
     Vertices are "sent" rather than "set" because they
     cause action.
     */
    void sendVertex(const Vector2& vertex);
    void sendVertex(const Vector3& vertex);
    void sendVertex(const Vector4& vertex);

    void beginPrimitive(Primitive p);
    void endPrimitive();

	class VARArea* createVARArea(size_t areaSize);
	void beginIndexedPrimitives();
	void endIndexedPrimitives();

    /** The vertex, normal, color, and tex coord arrays must all come from
        the same VARArea. */
	void setVertexArray(const class VAR& v);
	void setNormalArray(const class VAR& v);
	void setColorArray(const class VAR& v);
	void setTexCoordArray(unsigned int unit, const class VAR& v);

	template<class T>
	void sendIndices(RenderDevice::Primitive primitive, int numIndices, 
                     const T* index) {
		varSystem->sendIndices(primitive, sizeof(T), numIndices, index);
		countPrimitive(primitive, numIndices);
	}

	template<class T>
	void sendIndices(RenderDevice::Primitive primitive, 
                     const Array<T>& index) {
		sendIndices(primitive, index.size(), index.getCArray());
	}

    void setStencilClearValue(int s);
    void setDepthClearValue(double d);
    void setColorClearValue(const Color4& c);

    /**
     @param textureUnit >= 0
     @param texture Set to NULL to disable the unit
     */
    void setTexture(
        uint                textureUnit,
        TextureRef          texture);

    /**
      Reads a depth buffer value (1 @ far plane, 0 @ near plane) from
      the given screen coordinates (x, y) where (0,0) is the top left
      corner of the width x height screen.  Result is undefined for x, y not
      on screen.

      The result is sensitive to the projection and camera to world matrices.
     */
    double getDepthBufferValue(
        int                 x,
        int                 y) const;

    /**
     Render a set of x, y, z axes for the current object-to-world
     matrix.  

     @param scale Thickness and length of the axes.  You
     might want to use the radius of an object's bounding sphere.
     */
    void debugDrawAxes(double scale = 1);

    void debugDrawRay(const Ray& ray, const Color3& color = Color3::RED, double scale = 1);

    /**
     Render a box with wireframe edges and translucent faces.
     Convenient for debugging bounding boxes.  
     
     @param edgeScale A
     scaling factor on the thickness of the edges.  Use the same value
     you would with drawAxes
     */
    void debugDrawBox(const Box& box, const Color3& color = Color3::RED, 
                      double edgeScale = 1);

    /**
     Convenient for debugging bounding spheres.
     */
    void debugDrawSphere(const Sphere& sphere,
                         const Color3& color = Color3::RED, 
                         double edgeScale = 1);

    void debugDrawCapsule(const Capsule& capsule,
                         const Color3& color = Color3::RED, 
                         double edgeScale = 1);

private:
    /**
     Snapshot of the state maintained by the render device.
     */
    // WARNING: if you add state, you must initialize it in
    // the constructor and RenderDevice::init and set it in
    // setState().
    class RenderState {
    public:
        class TextureUnit {
        public:
            Vector4             texCoord;
            /** NULL if not bound */
            TextureRef          texture;
            double              textureMatrix[16];
            CombineMode         combineMode;

            TextureUnit();
        };

        Rect2D                      viewport;

        bool                        depthWrite;
        bool                        colorWrite;
        bool                        alphaWrite;

        DepthTest                   depthTest;
        StencilTest                 stencilTest;
        int                         stencilReference;
        AlphaTest                   alphaTest;
        double                      alphaReference;

        int                         stencilClear;
        double                      depthClear;
        Color4                      colorClear;               

        CullFace                    cullFace;

        StencilOp                   stencilFail;
        StencilOp                   stencilZFail;
        StencilOp                   stencilZPass;
        
        BlendFunc                   srcBlendFunc;
        BlendFunc                   dstBlendFunc;
        
        ShadeMode                   shadeMode;
    
        double                      polygonOffset;

        double                      lowDepthRange;
        double                      highDepthRange;

        // Ambient light level
        Color4                      ambient;

        double                      lineWidth;

        bool                        lighting;
        Color4                      color;
        Vector3                     normal;
        TextureUnit                 textureUnit[MAX_TEXTURE_UNITS];
    
        CoordinateFrame             objectToWorldMatrix;
        CoordinateFrame             cameraToWorldMatrix;

        /**
         LRBTNF
         */
        double                      projectionMatrixParams[6];

        /**
         When true, the projection matrix is 3D
         */
        bool                        proj3D;
        RenderState(int width = 1, int height = 1);

    };

    GLint toGLStencilOp(RenderDevice::StencilOp op) const;

    /**
     True between beginPrimitive and endPrimitive
     */
    bool                            inPrimitive;

	bool						    inIndexedPrimitive;


    int                             numTextureUnits;

    /**
     Current render state.
     */
    RenderState                     state;

    /**
     Old render states
     */
    Array<RenderState>              stateStack;

    void setState(
        const RenderState&          newState);

    /**
     Amount to multiply colors by due to gamma.
     */
    double brightScale;

    // Helpers for debug drawing routines
    void drawWireSphereSection(const Sphere& sphere, double cylRadius, const Color3& color, bool top, bool bottom);
    void drawFaceSphereSection(const Sphere& sphere, const Color3& color, bool top, bool bottom);
    
public:

    bool supportsOpenGLExtension(const std::string& extension) const;

    bool supportsTextureRectangle() const {
        return textureRectangleSupported;
    }

    /**
     Returns a value that you should DIVIDE light intensities by
     based on the gamma.  This is automatically handled if you
     use configureXXXLight() or bindLights()
     */
    inline double getLightSaturation() const {
        return lightSaturation;
    }

    /**
     Set up for 2D rendering.
     */
    void push2D();
    void push2D(const Rect2D& viewport);
    void pop2D();

    /**
     It is often useful to keep track of the number of polygons
     rendered in a scene for profiling purposes.
     */
    int                 polygonCount;
    
    /**
     Call this exactly once to initialize the system.

     @param lightSaturation Specify the value at which lighting saturates
     before it is applied to surfaces.  1.0 is the default OpenGL value,
     higher numbers increase the quality of bright lighting at the expense of
     color depth.

     @param varVideoMemory  The number of bytes of video memory to allocate
     for vertex arrays.  If 0, the VAR system is not initialized.  If nonzero,
	 you cannot use glDrawArrays or any other vertex array call.  Use the
	  corresponding RenderDevice methods instead.  If you prefer to use the raw
	  OpenGL calls, do not initialize the VAR system.

     @param colorBits The number of desired bits in each color channel of the frame buffer
     @param alphaBits The number of desired bits in the alpha channel of the frame buffer
     */
    bool init(
        int             width,
        int             height,
        class Log*      log               = NULL,
        double          lightSaturation   = 1.0,
        bool            fullscreen        = false,
        size_t          varVideoMemory    = 0,
        bool            asyncVideoRefresh = false,
        int             colorBits         = 8,
        int             alphaBits         = 8,
        int             depthBits         = 24,
        int             stencilBits       = 8);

	/**
	 Shuts down the system.  This should be the last call you make.
	 */
    void cleanup();

    /**
     Set the titlebar caption
     */
    void setCaption(const std::string& caption);

    virtual ~RenderDevice() {}

    /**
     Takes a JPG screenshot of the front buffer and saves it to a file.
     Example: renderDevice->screenshot("screens/");
     */
    void screenshot(const std::string& filepath) const;

	/**
	 Pixel dimensions of the OpenGL window interior
	 */
	inline int getWidth() const {
		return screenWidth;
	}

	/**
	 Pixel dimensions of the OpenGL window interior
	 */
	inline int getHeight() const {
		return screenHeight;
	}


	inline int getStencilBitDepth() const {
		return stencilBits;
	}


	inline int getZBufferBitDepth() const {
		return depthBits;
	}

    /**
     You must enable lighting and color materials yourself.  This just aids in
     configuring an OpenGL light.

     @param toLightVector The vector <B>towards</B> the light, in world space.  You
     must have already set the cameraToWorld matrix, and cannot change it without
     reconfiguring the light.  The current objectToWorld matrix is ignored.
     @param lightNum between 0 and 8
	 @param color Light color
     */
    void configureDirectionalLight(
        int                 lightNum,
        const Vector3&      toLightVector,
        const Color3&       color);

    void configurePointLight(
        int                 lightNum,
        const Vector3&      position,
        const Color3&       color,
        double              constantAttenuationCoef,
        double              linearAttenuationCoef,
        double              quadraticAttenuationCoef);

    /**
     You must also turn on lighting.
     */
    void setAmbientLightColor(
        const Color3&        color);
    void setAmbientLightColor(
        const Color4&        color);

    /**
     Equivalent to glEnable(GL_LIGHTING)
     */
    void enableLighting();
    void disableLighting();

    /**
     Multiplies v by the current object to world and world to camera matrices,
     then by the projection matrix to obtain a 2D point and z-value.  
     
     The result is the 2D position to which the 3D point v corresponds.  You
     can use this to make results rendered with push2D() line up with those
     rendered with a 3D transformation.
     */
    Vector3 project(const Vector4& v) const;
    Vector3 project(const Vector3& v) const;

    void RenderDevice::setAmbientLightLevel(
        const Color3&       color);


    #ifdef _WIN32
        HDC RenderDevice::getWindowHDC() const;
    #endif
};



/**
 A pointer to an array of vertices, colors, or normals in video memory.
 It is safe to copy these.  There is no destructor; it doesn't matter
 when you throw these objects away.  
 */
class VAR {
private:

	class VARArea*		area;

	/** Pointer to the block of uploaded memory */
	void*				pointer;

	/** Size of one element */
	size_t				elementSize;

	/** Pointer to the block of uploaded memory */
	int					numElements;

	uint64				generation;

    GLenum              underlyingRepresentation;

	bool ok() const;

	template<class T>
	void init(const T* sourcePtr, int _numElements, VARArea* _area) {

		numElements = _numElements;
		area		= _area;
        underlyingRepresentation = glFormatOf(T);

		debugAssert(area);
		debugAssert(area->basePointer);

		elementSize = sizeof(T);

        debugAssertM((elementSize % sizeOfGLFormat(format)) == 0,
            "Sanity check failed on OpenGL data format; you may be using an unsupported type in a vertex array.");

		pointer = (uint8*)area->basePointer + area->allocated;

		// Ensure that the next memory address is 8-byte aligned
		pointer = ((uint8*)pointer +      
   			       ((8 - (size_t)pointer % 8) % 8));

		generation = area->generation;
		
		size_t size = elementSize * numElements;
		debugAssert(size + area->allocated <= area->size);
		area->allocated = (size_t)pointer + size - (size_t)area->basePointer;

		// Upload the data
		memcpy(pointer, sourcePtr, size);
	}


	friend class RenderDevice::VARSystem;

	// The following are called by the VARSystem.
	void vertexPointer() const;


	void normalPointer() const;


	void colorPointer() const;


	void texCoordPointer(uint unit) const;

public:

	VAR();

	/**
	 Uploads the memory.  The element type is inferred from the pointer type by the
	 preprocessor.
    */
	template<class T>
	VAR(const T* sourcePtr, int _numElements, VARArea* _area) {
		init(sourcePtr, _numElements, _area);
	}		


	template<class T>
	VAR(const Array<T>& source, VARArea* _area) {
		init(source.getCArray(), source.size(), _area);
	}		
};


} // namespace

#endif
