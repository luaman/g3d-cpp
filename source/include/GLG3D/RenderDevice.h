/**
  @file RenderDevice.h

  Graphics hardware abstraction layer (wrapper for OpenGL).

  You can freely mix OpenGL calls with RenderDevice, just make sure you put
  the state back the way you found it or you will confuse RenderDevice.

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2001-05-29
  @edited  2004-05-15
*/

#ifndef GLG3D_RENDERDEVICE_H
#define GLG3D_RENDERDEVICE_H

#include "graphics3D.h"
#include "GLG3D/Texture.h"
#include "GLG3D/Milestone.h"
#include "GLG3D/VertexProgram.h"
#include "GLG3D/PixelProgram.h"
#include "GLG3D/Shader.h"
#include "GLG3D/VARArea.h"
#include "GLG3D/GWindowSettings.h"
#include "GLG3D/GWindow.h"

typedef unsigned int uint;

namespace G3D {

/**
 Number of hardware texture units to track state for.
 */
#define MAX_TEXTURE_UNITS 8

class VAR;


/**
 You must call RenderDevice::init() before using the RenderDevice.
  
 Rendering interface that abstracts OpenGL.  OpenGL is a basically
 good API with some rough spots.  Three of these are addressed by
 RenderDevice.  First, OpenGL state management is both tricky and
 potentially slow.  Second, OpenGL functions are difficult to use
 because many extensions have led to an evolutionary rather than
 designed API.  For type safety, new enums are introduced for values
 instead of the traditional OpenGL GLenum's, which are just ints.
 Third, OpenGL intialization is complicated.  This interface
 simplifies it significantly.

 <P> On Windows (G3D_WIN32) RenderDevice supports a getHDC() method that
 returns the HDC for the window.

 <P> NICEST line and point smoothing is enabled by default (however,
 you need to set your alpha blending mode to see it).

 <P> glEnable(GL_NORMALIZE) is set by default.  glEnable(GL_COLOR_MATERIAL) 
     is enabled by default.

 <P> For stereo rendering, set <CODE>GWindowSettings::stereo = true</CODE>
     and use RenderDevice::setDrawBuffer to switch which eye is being rendered.  Only
     use RenderDevice::beginFrame/RenderDevice::endFrame once per frame,
     but do clear both buffers separately.

 <P> The only OpenGL calls <B>NOT</B> abstracted by RenderDevice are
     fog and texture coordinate generation.  For everything else, use
     RenderDevice.

 <P>
 Example
  <PRE>
   RenderDevice renderDevice = new RenderDevice();
   renderDevice->init(GWindowSettings());
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
    renderDevice->setColor(Color3::white());
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

  <P>

  G3D::RenderDevice supports "X_CURRENT" as an option for most settings.

  <P>
 <B>Stereo Rendering</B>
  You can render in stereo (on a stereo capable card) by rendering twice,
  once for each eye's buffer:

  <pre>
    void doGraphics() {
        glDrawBuffer(GL_BACK_LEFT); 
        for (int count = 0; count < 2; ++count) {
           ... (put your normal rendering code here)
           glDrawBuffer(GL_BACK_RIGHT);
        }
    }
  </pre>

  Only flip the buffers once; that is, call renderDevice->swapBuffers
  once per frame (not once per eye).  If you use G3D::GApp, it takes
  care of this for you.

  <B>Multiple displays</B>
  If you are using multiple synchronized displays (e.g. the CAVE),
  see:
  http://www.nvidia.com/object/IO_10753.html
  and
  http://www.cs.unc.edu/Research/stc/FAQs/nVidia/FrameLock-V1.0C.pdf

  GLCaps loads the relevant extensions for you, but you must make
  the synchronizing calls yourself (typically, immediately before
  you call swap buffers).
}



 */
class RenderDevice {
public:
    enum Primitive {LINES, LINE_STRIP, TRIANGLES, TRIANGLE_STRIP,
                    TRIANGLE_FAN, QUADS, QUAD_STRIP, POINTS};

	/** RENDER_CURRENT = preserve whatever the render mode is currently set to.  */
    enum RenderMode {RENDER_SOLID, RENDER_WIREFRAME, RENDER_POINTS, RENDER_CURRENT};

    
    enum {MAX_LIGHTS = 8};

private:

    friend class VAR;
    friend class VARArea;
    friend class Milestone;
    friend class UserInput;

    /** Allows the UserInput to find the RenderDevice 
        @deprecated */
    static RenderDevice*        lastRenderDeviceCreated;

    GWindow*                    _window;

    /** Should the destructor delete _window?*/
    bool                        deleteWindow;

    /**
     Status and debug statements are written to this log.
     */
    class Log*                  debugLog;

    /**
     The current GLGeom generation.  Used for vertex arrays.
     */
    uint32                      generation;

    void setGamma(
        double                  brightness,
        double                  gamma);

	int                         stencilBits;

    /**
     Actual number of depth bits.
     */
    int                         depthBits;

    /**
     The intensity at which lights saturate.
     */
    double                      lightSaturation;

	void setVideoMode();

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
   
    /** Helper for setXXXArray.  Sets the currentVARArea and
        makes some consistency checks.*/
    void setVARAreaFromVAR(const class VAR& v);

    /** The area used inside of an indexedPrimitives call. */
    VARAreaRef                  currentVARArea;

    /** Number of triangles since last beginFrame() */
    int                         triangleCount;

    double                      emwaTriangleCount;
    double                      emwaTriangleRate;

	/** Updates the polygon count based on the primitive */
	void countPrimitive(RenderDevice::Primitive primitive, int numVertices);

    std::string                 cardDescription;

    /**
     Sets the milestones on the currentVARArea.
     */
    void setVARAreaMilestone();

    /** Called by sendIndices. */
    void internalSendIndices(
        RenderDevice::Primitive primitive,
        size_t                  indexSize, 
        int                     numIndices, 
        const void*             index);

    ////////////////////////////////////////////////////////////////////
public:

    RenderDevice();

    ~RenderDevice();

    /**
     Checkmarks all rendering state (<B>including</B> OpenGL fog and texture
     coordinate generation).
     */
    void pushState();

    /**
     Sets all state to a clean rendering environment.
     */
    void resetState();

    /**
     Restores all state to whatever was pushed previously.  Push and 
     pop must be used in matching pairs.
     */
    void popState();

    /** To clear the alpha portion of the color buffer, remember to
        enable alpha write */
    void clear(bool clearColor, bool clearDepth, bool clearStencil);

    /** Clears color, depth, and stencil. */
    void clear() {
        clear(true, true, true);
    }

    enum DepthTest   {DEPTH_GREATER,     DEPTH_LESS,       DEPTH_GEQUAL,  
                      DEPTH_LEQUAL,      DEPTH_NOTEQUAL,   DEPTH_EQUAL,   
                      DEPTH_ALWAYS_PASS, DEPTH_NEVER_PASS, DEPTH_CURRENT};

    enum AlphaTest   {ALPHA_GREATER,     ALPHA_LESS,       ALPHA_GEQUAL,  
                      ALPHA_LEQUAL,      ALPHA_NOTEQUAL,   ALPHA_EQUAL,  
                      ALPHA_ALWAYS_PASS, ALPHA_NEVER_PASS, ALPHA_CURRENT};

    enum StencilTest {STENCIL_GREATER,   STENCIL_LESS,     STENCIL_GEQUAL,
                      STENCIL_LEQUAL,    STENCIL_NOTEQUAL, STENCIL_EQUAL, 
                      STENCIL_ALWAYS_PASS, STENCIL_NEVER_PASS, STENCIL_CURRENT};

    enum BlendFunc   {BLEND_SRC_ALPHA,   BLEND_ONE_MINUS_SRC_ALPHA, BLEND_ONE,
                      BLEND_ZERO, BLEND_SRC_COLOR,  BLEND_DST_COLOR,  
                      BLEND_ONE_MINUS_SRC_COLOR, BLEND_CURRENT};

    enum StencilOp   {STENCIL_INCR_WRAP, STENCIL_DECR_WRAP,
                      STENCIL_KEEP,      STENCIL_INCR,     STENCIL_DECR,
                      STENCIL_REPLACE,   STENCIL_ZERO,     STENCIL_INVERT, STENCILOP_CURRENT};

    enum CullFace    {CULL_FRONT,        CULL_BACK,           CULL_NONE, CULL_CURRENT};

    enum ShadeMode   {SHADE_FLAT,        SHADE_SMOOTH, SHADE_CURRENT};

    enum CombineMode {TEX_REPLACE, TEX_INTERPOLATE, TEX_ADD, TEX_MODULATE, 
                      TEX_BLEND, TEX_CURRENT};

    enum Buffer      {BUFFER_BACK,       BUFFER_FRONT,
                      BUFFER_BACK_LEFT,  BUFFER_FRONT_LEFT,
                      BUFFER_BACK_RIGHT, BUFFER_FRONT_RIGHT, BUFFER_CURRENT};

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
     Use ALWAYS_PASS to shut off testing.
     */
    void setDepthTest(DepthTest test);
    void setStencilTest(StencilTest test);

    void setRenderMode(RenderMode mode);

    /**
     Sets the constant used in the stencil test and operation (if op == STENCIL_REPLACE)
     */
    void setStencilConstant(int reference);

	/** If the alpha test is ALPHA_CURRENT, the reference is ignored */
    void setAlphaTest(AlphaTest test, double reference);

    /**
     Sets the frame buffer that is written to.  Used to intentionally
     draw to the front buffer and for stereo rendering.
     */
    void setDrawBuffer(Buffer drawBuffer);

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
     independent. You do not need to enable the stencil test to use
     the stencil op.

     Use KEEP, KEEP, KEEP to disable stencil writing.  Equivalent to a
     combination of glStencilTest, glStencilFunc, and glStencilOp.


     If there is no depth buffer, the depth test always passes.  If there
     is no stencil buffer, the stencil test always passes.
     */
    void setStencilOp(
        StencilOp                       fail,
        StencilOp                       zfail,
        StencilOp                       zpass);

    /**
     When GLCaps::GL_ARB_stencil_two_side is true, separate
     stencil operations can be used for front and back faces.  This
     is useful for rendering shadow volumes.
     */
    void setStencilOp(
        StencilOp                       frontStencilFail,
        StencilOp                       frontZFail,
        StencilOp                       frontZPass,
        StencilOp                       backStencilFail,
        StencilOp                       backZFail,
        StencilOp                       backZPass);

    /**
     Use BLEND_ZERO, BLEND_ONE to shut off blending.
     Equivalent to glBlendFunc.
     */
    void setBlendFunc(
        BlendFunc                       src,
        BlendFunc                       dst);

    /**
     Equivalent to glLineWidth.
     */
    void setLineWidth(
        double                          width);

    /**
     Equivalent to glPointSize.
     */
    void setPointSize(
        double                          diameter);

    /**
     This is not the OpenGL MODELVIEW matrix: it is a matrix that maps
     object space to world space.  The actual MODELVIEW matrix
     is cameraToWorld.inverse() * objectToWorld.  You can retrieve it
     with getModelViewMatrix.
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

    Matrix4 getProjectionMatrix() const;

    /**
     cameraToWorld.inverse() * objectToWorld
     */
    CoordinateFrame getModelViewMatrix() const;

    /**
     projection() * cameraToWorld.inverse() * objectToWorld
     */
    Matrix4 getModelViewProjectionMatrix() const;


    /**
    To set a typical 3D perspective matrix, use either
     <CODE>renderDevice->setProjectionMatrix(Matrix4::perspectiveProjection(...)) </CODE>
     or call setProjectionAndCameraMatrix.
     */
    void setProjectionMatrix(const Matrix4& P);

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
     The matrix returned may not be the same as the
     underlying hardware matrix-- the y-axis is flipped
     in hardware when a texture with invertY = true is specified.
     */
    Matrix4 getTextureMatrix(uint textureUnit);

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
    void setTexCoord(uint textureUnit, const Vector3int16& texCoord);
    void setTexCoord(uint textureUnit, const Vector2& texCoord);
    void setTexCoord(uint textureUnit, const Vector2int16& texCoord);
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

    /**
     (0, 0) is the <B>upper</B>-left corner of the screen.
     */
    void setViewport(const Rect2D& v);
    Rect2D getViewport() const;

    /**
     Vertices are "sent" rather than "set" because they
     cause action.
     */
    void sendVertex(const Vector2& vertex);
    void sendVertex(const Vector3& vertex);
    void sendVertex(const Vector4& vertex);

    void setProjectionAndCameraMatrix(const class GCamera& camera);

    /**
     Analogous to glBegin.  See the example in the detailed description
     section of this page.
     */
    void beginPrimitive(Primitive p);

    /**
     Analogous to glEnd.  See the example in the detailed description
     section of this page.
     */
    void endPrimitive();

	void beginIndexedPrimitives();
	void endIndexedPrimitives();

    /** The vertex, normal, color, and tex coord arrays need not come from
        the same VARArea. 

        The format of a VAR array is restricted depending on its use.  The
        following table (from http://oss.sgi.com/projects/ogl-sample/registry/ARB/vertex_program.txt)
        shows the underlying OpenGL restrictions:

     <PRE>

                                       Normal    
      Command                 Sizes    ized?   Types
      ----------------------  -------  ------  --------------------------------
      VertexPointer           2,3,4     no     short, int, float, double
      NormalPointer           3         yes    byte, short, int, float, double
      ColorPointer            3,4       yes    byte, ubyte, short, ushort,
                                               int, uint, float, double
      IndexPointer            1         no     ubyte, short, int, float, double
      TexCoordPointer         1,2,3,4   no     short, int, float, double
      EdgeFlagPointer         1         no     boolean
      VertexAttribPointerARB  1,2,3,4   flag   byte, ubyte, short, ushort,
                                               int, uint, float, double
      WeightPointerARB        >=1       yes    byte, ubyte, short, ushort,
                                               int, uint, float, double
      VertexWeightPointerEXT  1         n/a    float
      SecondaryColor-         3         yes    byte, ubyte, short, ushort,
        PointerEXT                             int, uint, float, double
      FogCoordPointerEXT      1         n/a    float, double
      MatrixIndexPointerARB   >=1       no     ubyte, ushort, uint

      Table 2.4: Vertex array sizes (values per vertex) and data types.  The
      "normalized" column indicates whether fixed-point types are accepted
      directly or normalized to [0,1] (for unsigned types) or [-1,1] (for
      singed types). For generic vertex attributes, fixed-point data are
      normalized if and only if the <normalized> flag is set.

  </PRE>
    
    */
	void setVertexArray(const class VAR& v);
	void setNormalArray(const class VAR& v);
	void setColorArray(const class VAR& v);
	void setTexCoordArray(unsigned int unit, const class VAR& v);


    /** Returns the GWindow used by this RenderDevice */
    GWindow* window() const;

    /**
     Vertex attributes are a generalization of the various per-vertex
     attributes that relaxes the format restrictions.  There are at least
     16 attributes on any card (some allow more).  These attributes have
     special meaning under the fixed function pipeline, as follows:

    <PRE>
    Generic
    Attribute   Conventional Attribute       Conventional Attribute Command
    ---------   ------------------------     ------------------------------
         0      vertex position              Vertex
         1      vertex weights 0-3           WeightARB, VertexWeightEXT
         2      normal                       Normal
         3      primary color                Color
         4      secondary color              SecondaryColorEXT
         5      fog coordinate               FogCoordEXT
         6      -                            -
         7      -                            -
         8      texture coordinate set 0     MultiTexCoord(TEXTURE0, ...)
         9      texture coordinate set 1     MultiTexCoord(TEXTURE1, ...)
        10      texture coordinate set 2     MultiTexCoord(TEXTURE2, ...)
        11      texture coordinate set 3     MultiTexCoord(TEXTURE3, ...)
        12      texture coordinate set 4     MultiTexCoord(TEXTURE4, ...)
        13      texture coordinate set 5     MultiTexCoord(TEXTURE5, ...)
        14      texture coordinate set 6     MultiTexCoord(TEXTURE6, ...)
        15      texture coordinate set 7     MultiTexCoord(TEXTURE7, ...)
       8+n      texture coordinate set n     MultiTexCoord(TEXTURE0+n, ...)
    </PRE>

      @param normalize If true, the coordinates are forced to a [0, 1] scale
    */
    void setVertexAttribArray(unsigned int attribNum, const class VAR& v, bool normalize);

    /**
     Draws the specified kind of primitive from the current vertex array.
     */
	template<class T>
	void sendIndices(RenderDevice::Primitive primitive, int numIndices, 
                     const T* index) {
		
        internalSendIndices(primitive, sizeof(T), numIndices, index);

        // Mark all active arrays as busy.
        setVARAreaMilestone();

		countPrimitive(primitive, numIndices);
	}

    /**
     Renders sequential vertices from the current vertex array.
     (Equivalent to glDrawArrays)
     */
    void sendSequentialIndices(RenderDevice::Primitive primitive, int numVertices);

    /**
     Draws the specified kind of primitive from the current vertex array.
     */
	template<class T>
	void sendIndices(RenderDevice::Primitive primitive, 
                     const Array<T>& index) {
		sendIndices(primitive, index.size(), index.getCArray());
	}

    void setStencilClearValue(int s);
    void setDepthClearValue(double d);
    void setColorClearValue(const Color4& c);

    /**
     Devices with more textures than texture units (e.g. GeForceFX)
     
     @param textureUnit >= 0
     @param texture Set to NULL to disable the unit
     */
    void setTexture(
        uint                textureUnit,
        TextureRef          texture);

    /** Returns the number of textures available.  May be higher
        than the number of texture units if the programmable
        pipeline provides more textures than the fixed function
        pipeline.*/
    uint numTextures() const;

    /** Returns the number of texture units 
        (texture + reg combiner + matrix) available.
        This only applies to the fixed function pipeline.
    */
    uint numTextureUnits() const;

    /** Returns the number of texture coordinates allowed.
        This may be greater than the number of texture matrices.*/
    uint numTextureCoords() const;

    /**
     Set the current shader.  You may call VertexAndPixelShader::bindArgList either
     before or after this, or use the variation of this call that
     includes an arg list.  Typically called by your ObjectShader subclass.
     */
    void setVertexAndPixelShader(const VertexAndPixelShaderRef& s);

    /** Throws VertexAndPixelShader::ArgumentError if the arguments provided
      do not match the arguments declared */
    void setVertexAndPixelShader(const VertexAndPixelShaderRef& s,
            const VertexAndPixelShader::ArgList& args);

	void setObjectShader(const ObjectShaderRef& s);

    /**
     Automatically enables vertex programs when they are set. 
     Assumes GLCaps::supports_GL_ARB_vertex_program() is true.

     Don't mix VertexPrograms (old API) with VertexShaders (new API).

     @param vp Set to NULL to use the fixed function pipeline.
     @deprecated Use RenderDevice::setShader
     */
    void setVertexProgram(const VertexProgramRef& vp);

    /**
     Sets vertex program arguments for vertex programs outputted by Cg.
     The names of arguments are read from comments.

     <PRE>
        ArgList args;
        args.set("MVP", renderDevice->getModelViewProjection());
        args.set("height", 3);
        args.set("viewer", Vector3(1, 2, 3));
        renderDevice->setVertexProgram(toonShadeVP, args);
     </PRE>


     Don't mix VertexPrograms (old API) with VertexShaders (new API).
     @param args must include *all* arguments or an assertion will fail
     @deprecated Use RenderDevice::setShader
     */
    void setVertexProgram(const VertexProgramRef& vp,
                          const GPUProgram::ArgList& args);

    /**
     (Automatically enables pixel programs when they are set.) 
     Assumes GPUProgram() is true.
     Don't mix PixelPrograms (old API) with PixelShaders (new API).
     @param pp Set to NULL to use the fixed function pipeline.
     @deprecated Use RenderDevice::setShader
     */
    void setPixelProgram(const PixelProgramRef& pp);

    /**
     It is recommended to call RenderDevice::pushState immediately before
     setting the pixel program, since the arguments can affect texture
     state that will only be restored with RenderDevice::popState.
     Don't mix PixelPrograms (old API) with PixelShaders (new API).
     @deprecated Use RenderDevice::setShader
     */
    void setPixelProgram(const PixelProgramRef& pp,
                         const GPUProgram::ArgList& args);
    
    /**
      Reads a depth buffer value (1 @ far plane, 0 @ near plane) from
      the given screen coordinates (x, y) where (0,0) is the top left
      corner of the width x height screen.  Result is undefined for x, y not
      on screen.

      The result is sensitive to the projection and camera to world matrices.

      If you need to read back the entire depth buffer, use OpenGL glReadPixels
      calls instead of many calls to getDepthBufferValue.
     */
    double getDepthBufferValue(
        int                 x,
        int                 y) const;


    /**
     Description of the graphics card and driver version.
     */
    std::string getCardDescription() const;


private:

	/** Called immediately before a primitive group */
	void runObjectShader();

    /**
     For performance, we don't actually unbind a texture when
     turning off a texture unit, we just disable it.  If it 
     is enabled with the same texture, we've saved a swap.
    */
    uint32               currentlyBoundTexture[MAX_TEXTURE_UNITS];

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

        GLight                      light[MAX_LIGHTS];
        bool                        lightEnabled[MAX_LIGHTS];
        bool                        depthWrite;
        bool                        colorWrite;
        bool                        alphaWrite;

        Buffer                      drawBuffer;

        DepthTest                   depthTest;
        StencilTest                 stencilTest;
        int                         stencilReference;
        AlphaTest                   alphaTest;
        double                      alphaReference;

        int                         stencilClear;
        double                      depthClear;
        Color4                      colorClear;               

        CullFace                    cullFace;

        StencilOp                   frontStencilFail;
        StencilOp                   frontStencilZFail;
        StencilOp                   frontStencilZPass;
        StencilOp                   backStencilFail;
        StencilOp                   backStencilZFail;
        StencilOp                   backStencilZPass;
        
        BlendFunc                   srcBlendFunc;
        BlendFunc                   dstBlendFunc;
        
        ShadeMode                   shadeMode;
    
        double                      polygonOffset;

        RenderMode                  renderMode;

        double                      specular;
        double                      shininess;

        double                      lowDepthRange;
        double                      highDepthRange;

        VertexAndPixelShaderRef     vertexAndPixelShader;
		ObjectShaderRef				objectShader;

        /** @deprecated */
        VertexProgramRef            vertexProgram;
        /** @deprecated */
        PixelProgramRef             pixelProgram;

        // Ambient light level
        Color4                      ambient;

        double                      lineWidth;
        double                      pointSize;

        bool                        lighting;
        Color4                      color;
        Vector3                     normal;
        TextureUnit                 textureUnit[MAX_TEXTURE_UNITS];
    
        CoordinateFrame             objectToWorldMatrix;
        CoordinateFrame             cameraToWorldMatrix;

        Matrix4                     projectionMatrix;

        RenderState(int width = 1, int height = 1);

    };

    GLint toGLStencilOp(RenderDevice::StencilOp op) const;

    /**
     True between beginPrimitive and endPrimitive
     */
    bool                            inPrimitive;

	bool						    inIndexedPrimitive;

    int                             _numTextureUnits;

    int                             _numTextures;

    int                             _numTextureCoords;

    /**
     Called from the various setXLight functions.
     @param force When true, OpenGL state is changed
     regardless of whether RenderDevice thinks it is up to date.
     */
    void setLight(int i, const GLight* light, bool force);

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
    double                          brightScale;

    bool                            _initialized;

    /** Cache of values supplied to supportsTextureFormat.
        Works on pointers since there is no way for users
        to construct their own TextureFormats.
     */
    Table<const TextureFormat*, bool>      _supportedTextureFormat;

public:

    /** @deprecated 
      Use GLCaps::supports instead.
     */
    bool supportsOpenGLExtension(const std::string& extension) const;

    /** Returns true if the given texture format is supported on this device.
        Cannot be called between beginPrimitive and endPrimitive.
        @deprecated
        Use GLCaps::supports.*/
    bool supportsTextureFormat(const TextureFormat* fmt);

    /**
      When true, the 6-argument version of RenderDevice::setStencilOp
      can set the front and back operations to different values.

      @deprecated 
      Use GLCaps::supports_GL_ARB_stencil_two_side instead.
     
    */
    bool supportsTwoSidedStencil() const;

    /**
     When true, Texture::DIM_2D_RECT textures can be created.
     @deprecated
     Use GLCaps::supports_GL_EXT_texture_rectangle instead.
     */
    bool supportsTextureRectangle() const;

    /**
     @deprecated
     Use VertexAndPixelShader
     */
    bool supportsVertexProgram() const;

    /**
     When true, NVIDIA Vertex Program 2.0 vertex programs can
     be loaded by VertexProgram.
     @deprecated
     Use VertexAndPixelShader
     */
    bool supportsVertexProgramNV2() const;

    /**
     @deprecated
     Use VertexAndPixelShader
     */
    bool supportsPixelProgram() const;

    /**
     When true, VAR arrays will be in video, not main memory,
     and much faster.
     @deprecated
     Use GLCaps::supports_GL_ARB_vertex_buffer_object instead.
     */
    bool supportsVertexBufferObject() const;

    /**
     Returns a value that you should DIVIDE light intensities by
     based on the gamma.  This is automatically handled if you
     use setLight()
     */
    inline double getLightSaturation() const {
        return lightSaturation;
    }

    void push2D();

    /**
     Set up for traditional 2D rendering (origin = upper left, y increases downwards).
     */
    void push2D(const Rect2D& viewport);
    void pop2D();

    /**
     It is often useful to keep track of the number of polygons
     rendered in a scene for profiling purposes.
     */
    int                 polygonCount;
 
    /**
     Automatically constructs an SDLWindow then calls the other init
     routine (provided for backwards compatibility).  The constructed
     window is deleted on shutdown.
     */
    bool init(const GWindowSettings& settings, class Log* log = NULL);

    /**
     The renderDevice will <B>not</B> delete the window on cleanup.
     */
    bool init(GWindow* window, class Log* log = NULL);

    /** Returns true after RenderDevice::init has been called. */
    bool initialized() const;

	/**
	 Shuts down the system.  This should be the last call you make.
	 */
    void cleanup();

    /**
     Set the titlebar caption
     */
    void setCaption(const std::string& caption);

    /**
     Takes a JPG screenshot of the front buffer and saves it to a file.
     Returns the name of the file that was written.
     Example: renderDevice->screenshot("screens/"); 
     */
    std::string screenshot(const std::string& filepath) const;

    /**
     Notify RenderDevice that the window size has changed.  
     Called in response to a user resize event.  An example
     using SDL:
     <PRE>
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_VIDEORESIZE:
                {
                    renderDevice->notifyResize(event.resize.w, event.resize.h);
                    Rect2D full(0, 0, renderDevice->getWidth(), renderDevice->getHeight());
                    renderDevice->setViewport(full);
                }
                break;
            }
        }

     </PRE>
     */
    void notifyResize(int w, int h);

    /**
     Takes a screenshot and puts the data into the G3D::GImage dest variable.
     */
    void screenshotPic(GImage& dest, bool useBackBuffer = false) const;

	/**
     @deprecated
	 */
    int getWidth() const;

	/**
	 Pixel dimensions of the OpenGL window interior
	 */
    int width() const;

    /**
     @deprecated
     */
    int getHeight() const;

	/**
	 Pixel dimensions of the OpenGL window interior
	 */
    int height() const;

	inline int getStencilBitDepth() const {
		return stencilBits;
	}


	inline int getZBufferBitDepth() const {
		return depthBits;
	}

    /**
     You must also enableLighting.  Ambient light is handled separately.
     Lighting is automatically adjusted to the lightSaturation value.

     Lights are specified in <B>world space</B>-- they are not affected
     by the camera or object matrix.  Unlike OpenGL, you do not need to
     reset lights after you change the camera matrix.

     setLight(i, NULL) disables a light.
     */
    void setLight(int num, const GLight& light);
    void setLight(int num, void*);

    /**
     Sets the current specular coefficient used in the lighting equation.
     Should be on the range 0 (perfectly diffuse) to 1 (bright specular
     highlight).
     */
    void setSpecularCoefficient(double s);

    /**
     Sets the current shininess exponent used in the lighting equation.
     On the range 0 (large highlight) to 255 (tiny, focussed highlight).
     */
    void setShininess(double s);

    /**
     You must also RenderDevice::enableLighting.
     */
    void setAmbientLightColor(
        const Color3&        color);

    void setAmbientLightColor(
        const Color4&        color);

    /**
     Equivalent to glEnable(GL_LIGHTING).

     On initialization, RenderDevice configures the color material as follows
     (it will be this way unless you change it):
     <PRE>
      float spec[] = {1.0f, 1.0f, 1.0f, 1.0f};
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10);
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
     </PRE>
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
    Vector4 project(const Vector4& v) const;
    Vector4 project(const Vector3& v) const;


    /**
     Returns a new Milestone that can be passed to setMilestone and waitForMilestone.
     Milestones are garbage collected.
     */
    MilestoneRef createMilestone(const std::string& name);

    /**
     Inserts a milestone into the GPU processing list.  You can later call
     waitForMilestone to force the CPU to stall until the GPU has reached
     this milestone.
     <P>
     A milestone may be set multiple times, even without waiting for it in between.
     There is no requirement that a milestone be waited for once set.  Milestone
     setting transcends and is not affected by pushState()/popState() or beginFrame()/endFrame().
     */
    void setMilestone(const MilestoneRef& m);

    /**
     Stalls the CPU until the GPU has finished the milestone.  It is an error
     to wait for a milestone that was not set since it was last waited for.
     */
    void waitForMilestone(const MilestoneRef& m);

    /**
     Call within RenderDevice::pushState()...popState() so that you can
     restore the texture coordinate generation

     @param lightMVP The modelview projection matrix that
            was used to render the shadow map originally
            (you can get this from RenderDevice::getModelViewProjectionMatrix() 
            while rendering the shadow map).
     @param textureUnit The texture unit to use for shadowing. 0...RenderDevice::numTextureUnits()
            That unit cannot be used for texturing at the same time.
     */
    void configureShadowMap(
        uint                textureUnit,
        const Matrix4&      lightMVP,
        const TextureRef&   shadowMap);

    /**
     Call within RenderDevice::pushState()...popState() so that you can
     restore the texture coordinate generation.  Note that you can 
     obtain the reflection texture (aka environment map) from G3D::Sky
     or by loading it with G3D:Texture::fromFile.

     @param textureUnit The texture unit to use for shadowing. 0...RenderDevice::numTextureUnits()
            That unit cannot be used for texturing at the same time.
     */

    void configureReflectionMap(
        uint                textureUnit,
        TextureRef          reflectionTexture);

    #ifdef G3D_WIN32
        HDC getWindowHDC() const;
    #endif

};

} // namespace

#endif
