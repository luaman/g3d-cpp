/**
 @page changelog Change Log

<IMG SRC="G3D.gif" ALIGN=RIGHT>
<P>

   Major version numbers introduce API changes that are not backwards
   compatible.  Minor versions are backwards compatible to the
   previous major release [except for bug fixes].  Deprecated functionality
   will be supported until (at least) the next major release.

   <P>
   Changes in 6.04:
   <UL>
     <LI> G3D Guide overview documentation
     <LI> Changelog and Error FAQ moved under Doxygen
     <LI> Build scripts and documentation now under the 'doc' .dsp on Windows
     <LI> Textures now support a DepthReadMode that can be used to perform hardware
	     shadow map comparisions.  <B>RenderDevice::configureShadowMap now requires 
		 an appropriately configured texture-- in previous releases it would
		 reconfigure the texture for you.</B>
     <LI> G3D::UserInput::keyReleased, G3D::UserInput::ReleasedKeys
     <LI> G3D::Array::randomElement
	 <LI> Optimized G3D::Array::randomize
     <LI> G3D::cyclicCatmullRomSpline
     <LI> G3D::wrap
     <LI> contrib/AudioDevice
     <LI> G3D::System::time();
	 <LI> More precise System::sleep
     <LI> G3D::AABSPTree::serializeStructure, deserializeStructure,
     <LI> serialize(Vector3::Axis, BinaryOutput), deserialize(Vector3::Axis, BinaryInput),
     <LI> "glslc" GLSL compiler in the tools directory for getting compile-time errors from shaders 
     <LI> GLCaps::init now takes optional debug log
	 <LI> G3D::VertexAndPixelShader static constructors take optional 'debug' argument
     <LI> GWindowSettings::visible; Win32Window can now start invisible
     <LI> [ 991147 ] glBlendEquationEXT, RenderDevice::BlendEq, min, max, subtract, reverse subtract alpha blending
     <LI> [ 989785 ] Draw::rect2D
	 <LI> GLCaps::numTextureCoords, GLCaps::numTextureUnits, GLCaps::numTextures
	 <LI> GLCaps::G3D_MAX_TEXTURE_UNITS
     <LI> Rect2D::corner
     <LI> GCamera::getFrustum, GCamera::frustum, GCamera::Frustum, GCamera::Frustum::Face
     <LI> Plane constructor that accepts Vector4s (possibly at infinity)
     <LI> AABox::inf, AABox::zero, AABox::maxFinite
     <LI> AABox::intersects(Sphere)
     <LI> Vector3::minFinite, Vector3::maxFinite
	 <LI> Plane::halfSpaceContainsFinite
	 <LI> Plane::halfSpaceContains(Vector4)
     <LI> AABSPTree::getIntersectingMembers(Array<Plane>) 
	 <LI> AABSPTree::getIntersectingMembers(GCamera::Frustum) for view-frustum culling
	 <LI> AABSPTree::getIntersectingMembers(Sphere)
     <LI> AABox::split
     <LI> Extended AABox::culledBy, Box::culledBy, and Sphere::culledBy with extra
	        information for bounding volume hierarchies
     <LI> G3D::computeNormalMap
     <LI> Matrix3::fuzzyEq(Matrix3)
     <LI> Removed System::sleep(0.02) from GLG3D demo to give more accurate performance measure
     <LI> [ 965824 ] changed link library defaults
     <LI> serialize/deserialize for int, bool, double, float, std::string
     <LI> G3D::TextOutput
     <LI> [ 976924 ] Texture::texelWidth
     <LI> [ 973413 ] VertexAndPixelShader::ArgList::set can be called more than once per variable
     <LI> GWindow::setIcon(std::string filename)
     <LI> Texture::fromMemory that takes a single image (instead of an array of images)
     <LI> [972604] RenderDevice::setTextureMatrix(uint, Matrix4)
     <LI> [972747] Rect2D::center
     <LI> GImage and Texture now load ICO files
	 <LI> GL_SAMPLER_1D_ARB, 2D, 3D, CUBE
	 <LI> Win32Window mouse events
	 <LI> Added normals to AABox collision results
     <LI> Fix: ManualCameraController is prevented from looking precisely along the Y-axis, which would cause 
	           a singularity.
	 <LI> Fix: Added '?' as a valid symbol Token
	 <LI> Fix: [ 946235 ] GFont::align right w/ fixed_spacing
	 <LI> Fix: [ 1001033 ] RenderDevice with 0 texture units
	 <LI> Fix: GLCaps:: ARB stencil two side -> EXT stencil two side (stencilled shadows were broken)
	 <LI> Fix: [ 993449 ] vsnprintf crashes MSVC 7
	 <LI> Fix: [ 991320 ] Pointer truncation Warnings
	 <LI> Fix: [ 981440 ] AUTO with Texture::fromMemory
	 <LI> Fix: Plane::halfSpaceContains now works for infinite and semi-infinite points
     <LI> Fix: [ 979032 ] Quat <-> Matrix3 roundtrip inverts
	 <LI> Fix: [ 976743 ] document GLCaps functions
	 <LI> Fix: [ 976746 ] #include GLCaps in g3dall
	 <LI> Fix: [ 973550 ] sampler2DRect now supported in GLSL shaders (NVIDIA only; ATI drivers are broken)
	 <LI> Fix: [ 973490 ] Win32Window width/height off by non-client amount
     <LI> Fix: [ 961827 ] In debug mode, RenderDevice tries to access 
	            GL_MAX_TEXTURE_IMAGE_UNITS_ARB and an assertion fails on cards that
				don't support it.
	 <LI> Fix: Texture binding for VertexAndPixelShader
   </UL>

   <P>
   Changes in 6.03:
   <UL>
     <LI> Matrix4::approxCoordinateFrame
     <LI> Vector2(const Vector2int16&) [Giulio]
     <LI> RenderDevice::setObjectShader
     <LI> RenderDevice::setVertexAndPixelShader
     <LI> G3D::RenderDevice supports "..._CURRENT" as an option for most settings
     <LI> inf -> inf(), nan -> nan(), NAN -> NAN()
	        <B>This is an incompatible change-- it was needed to fix a bug with the order
			   of initialization of globals</B>
     <LI> GImage::sizeInMemory
     <LI> Defined std::ostream << NetAddress, std::ostream << Vector3 
     <LI> 'build doc' copies the contrib directory to the install directory
     <LI> LightweightConduit::PacketSizeException
     <LI> Quat::unitRandom() [Giulio]
     <LI> Color3::wheelRandom
     <LI> GImage::save and encode now const [Thanks Arni Mar Jonsson]
     <LI> LightweightConduit::send that accepts multiple destinations
	 <LI> ReliableConduit::multisend
     <LI> Moved IFSBuilder from demos to contrib
     <LI> LightweightConduit and ReliableConduit send/receive can now take references as well as pointers
     <LI> RenderDevice::clear() that takes no arguments
     <LI> RenderDevice::setShader
     <LI> G3D::GApp now catches ShaderGroup::ArgumentError exceptions
     <LI> System::operatingSystem() now includes a version number on Linux
     <LI> SDLWindow no longer initializes the audio system; use SDL_InitSubsytem if you need audio.
     <LI> Extended GLenumToString with GL_SHADER_OBJECTS_ARB types.
     <LI> NVIDIA p-buffer: GLX_SAMPLE_BUFFERS_ARB, GLX_SAMPLES_ARB, GLX_FLOAT_COMPONENTS_NV,
	      glXDestroyGLXPbufferSGIX, glXChooseFBConfigSGIX, glXCreateGLXPbufferSGIX, 
	      glXCreateContextWithConfigSGIX, glXQueryGLXPbufferSGIX
     <LI> NVIDIA swap lock: glXJoinSwapGroupNV, glXBindSwapBarrierNV, glXQuerySwapGroupNV, 
          glXQueryMaxSwapGroupsNV, glXQueryFrameCountNV, glXResetFrameCountNV
	 <LI> GWindow::requiresMainLoop, GWindow::runMainLoop (Beta)
     <LI> GWindow::pollEvent, SDLWindow::pollEvent
     <LI> G3D::GApp accepts an optional GWindow on construction
     <LI> G3D::VertexAndPixelShader, G3D::ObjectShader (Beta)
     <LI> Deprecated GPUProgram, VertexProgram, and PixelProgram (the OpenGL 1.5 shaders
	      follow a different paradigm than the OpenGL 1.3 ones, so the G3D API must change
		  to match it).
     <LI> Support for GL_ARB_vertex_shader, GL_ARB_fragment_shader, and GL_ARB_shader_objects
     <LI> G3D::drawFeatureEdges
     <LI> const Array<Vector3>& G3D::MD2Model::PosedModel::objectSpaceFaceNormals();
	 <LI> G3D::RenderDevice::sendSequentialIndices
     <LI> Network_Demo
     <LI> contrib/Win32Window
	 <LI> contrib/pingtest
     <LI> contrib/GlutWindow [Morgan and Dan Keefe]
	 <LI> contrib/ObjModel [Corey Taylor]
     <LI> G3D::GLCaps
	 <LI> GAppSettings::logFilename
	 <LI> Deprecated RenderDevice::suportsOpenGLExtension, RenderDevice::supportsTextureFormat,
	      other supports shortcuts (use GLCaps instead).
	 <LI> DiscoveryClient::cleanup
	 <LI> Optimized BinaryInput::readUInt32, readUInt16
     <LI> Extended network documentation
     <LI> 'fastlib' build target for G3D library developers
	 <LI> glGetVector2, glGetVector3, glGetVector4
	 <LI> float * Quat (double * Quat already existed)
	 <LI> GApp automatically generates g3d-license.txt at runtime ([RFE#856338] CREDIT.TXT)
	 <LI> G3D::license
	 <LI> Removed several large files (tag, ppt, exe) from the source zipfile, bringing it down to 3 MB
     <LI> Improved CoordinateFrame:pointToObjectSpace() (RFE#715996) [Giulio]
     <LI> [RFE#945935] Make static constants into functions [Giulio]
     <LI> Fix: LightweightConduit::send verifies that the packet size is smaller than the UDP limit
	 <LI> Fix: Multitexture on ATI and Wildcat cards
	 <LI> Fix: Incorrect occlusion in GLG3D_Demo (was caused by global constant problem)
     <LI> Fix: [BUG#949377] Checks for stencil extensions [Giulio]
     <LI> Fix: [BUG#922725] Non-multitexture implementation for getTextureState() [Giulio]
	 <LI> Fix: Restore ambient light color after RenderDevice::popState
	 <LI> Fix: RenderDevice now initializes OpenGL extensions before testing for multitexture [Erik Cassel, Dan Keefe]
	 <LI> Fix: Bottom clipping plane of GCamera frustum now correct (was slanted incorrectly, making frustum too big)
     <LI> Fix: GFont::draw2D now returns correct y value (used to be too small)
	 <LI> Fix: NetworkDevice now returns useful hostname on Linux (used to be "localhost")
	 <LI> Fix: The conduit returned from NetworkDevice::createReliableConduit now has ok() == false when connect fails
	 <LI> Fix: Tangent space computation of constant u, v now correct (was missing a factor of 2, leading to slight errors) [Max McGuire]
	 <LI> Fix: [ 925456 ] select broken on Linux (Networking was broken on Linux)
	 <LI> Fix: getDepthBufferValue off by 1 [Andi Fein]
   </UL>

   <P>
   Changes in 6.02:
   <UL>
     <LI> Default constructor for Line.
     <LI> Various patches to make G3D work with the CAVE [Dan Keefe]
     <LI> AABox::set
     <LI> Made GWindow::setPosition non-const
     <LI> VARArea now tests for the presence of all VBO extensions, on the freak chance that
	      a driver has only partial support (due to a bug)
     <LI> Linux build statically links OpenGL 1.2.1 and loads extensions through OpenGL 1.5
   	      to work around Wildcat Linux driver bug (Windows and Mac statically link OpenGL 1.1 
		  and load extensions through OpenGL 1.5)
     <LI> Triangle stores precomputed edge lengths
     <LI> Ray-triangle with vertex weights
     <LI> Highly optimized ray-triangle intersection test [Tomas Moller & Ben Trumbore]
     <LI> Create a texture from 6 different cube-map filenames
     <LI> Added contrib directory built as part of the 'doc' target
	 <LI> contrib/CoreyGWindow: GWindow implementations for various platforms
     <LI> AABSPSet::beginRayIntersection [Pete Hopkins]
     <LI> AABSPTree::beginBoxIntersection
     <LI> CollisionDetection::intersectionTimeForMovingPointFixedAABox, Ray::intersectionTime(AABox)
	 [Pierre Terdiman and Andrew Woo]
     <LI> Triangle::center
     <LI> Renamed KDTreeSet to AABSPTree, old name is #defined 
     <LI> RenderDevice now works on cards without multitexture
     <LI> void glTexCoord(const G3D::Vector4& t); [Dan Keefe]
	 <LI> Overloaded float, double, and int * Matrix3
     <LI> Fix: [ 923944 ] Matrix/Quat ambiguity
     <LI> Fix: fuzzyEq(inf, inf) is true
	 <LI> Fix: Triangle::randomPoint returns values outside the triangle
     <LI> Fix: [ 913763 ] tokenTypeToString(Token::END)
	 <LI> Fix: Compute number of texture coordinates before RenderDevice::setVideoMode [Dan Keefe]
     <LI> Changed the default depth bits to '0' for wider compatibility
	      (Fix: Unable to create OpenGL screen: Couldn't find matching GLX visual)
     <LI> Fix: [912305] Table, Queue, and Set assignment operators do not free old values 
     <LI> Fix: Separate specular and Multisample on Tablet PC w/ Trident [Dan Keefe]
     <LI> Fix: Linux debug build now has line numbers
     <LI> Upgraded to SDL 1.2.7
	        Fix: [ 838030 ] SDL 1.2.6 blocks prompt
			Fix: FSAA does not work under SDL
			Fix: Default Win32 refresh rate
     <LI> Draw::vertexVectors
     <LI> New meshes from Brown University: hemisphere.ifs, curvy.ifs, head.ifs,
	       closed-low-poly-teapot.ifs, bump.ifs
     <LI> GLight::specular
     <LI> SDLWindow::setWindowDimensions and setWindowPosition now work on Win32
     <LI> GWindowSettings::x, GWindowSettings::y, GWindowSettings::center
     <LI> System::setEnv
     <LI> [ 909999 ] GWindow Joystick interface
     <LI> double * Quat ([ 909305 ] scalar * {quat, vector, matrix})
	 <LI> Increased the precision of several Vector2 and Vector3 methods
     <LI> MeshAlg::computeNormals now returns 0 instead of NaN for degenerate normals
     <LI> Updated main-no-GApp.cpp for 6.02
     <LI> RenderDevice::screenshotPic can copy from the back buffer
     <LI> Improved VAR documentation.
     <LI> If NO_SDL_MAIN is defined, G3D does not attempt to link against sdlmain.lib
     <LI> UserInput::setPureDeltaMouse
     <LI> UserInput::mouseXY, mouseX, mouseY
	 <LI> UserInput::mouseDXY
	 <LI> Deprecated UserInput keyMapping constructor argument
     <LI> RenderDevice::setDrawBuffer [Dan Keefe]
     <LI> GFont::draw3D [Dan Keefe]
     <LI> GImage::pixel3(x, y) and GImage::pixel4(x, y)
	 <LI> debugAssert, debugBreak, debugAssertM, etc. all release input grab 
          when an assertion fails (Win32 and Linux) and restore it when the 
          program continues (Win32).  This also fixes the DirectInput laggy 
          cursor that occurs after a break.
   </UL>

   <P>
   Changes in 6.01:
   <UL>
     <LI> Default constructor for G3D::LineSegment
     <LI> Rect2D::clipPoly (Pete & Morgan)
	 <LI> Draw::poly2D, Draw::poly2DOutline (Pete & Morgan)
	 <LI> Added instructions for rotated text to G3D::GFont::draw2D
	 <LI> Fix: iRandom now compiles correctly under gcc.
     <LI> Fix: [ 852076 ] Compute better/faster vertex normals in MeshAlg
	      MeshAlg::computeNormals now weighs adjacent faces by their area
     <LI> Fix: [ 896028 ] Textures broken on Trident TabletPC (Dan Keefe)
	 <LI> Fix: [ 860800 ] ManualCameraController cursor jumps
     <LI> Fix: G3D::UserInput no longer offsets the mouse position by 1/2 pixel
     <LI> Fix: Alt-Tab no longer toggles the GApp camera before switching windows
     <LI> Fix: [ 901248 ] Font bounds y-value incorrect
	 <LI> Fix: G3D::PhysicsFrame::toCoordinateFrame() was rotated by 90 degrees
     <LI> Fix: [ 895493 ] Radeon 7500 Cube Map
	 <LI> Fix: G3D::MeshAlg::computeWeld produces linker errors on Linux
     <LI> G3D::TextInput::peekLineNumber(), G3D::TextInput::peekCharacterNumber()
     <LI> G3D::GAppSettings::dataDir
     <LI> html/gettingstarted.html
     <LI> G3D::MeshAlg::debugCheckConsistency
     <LI> G3D::MD2Model and G3D::IFSModel now weld their adjacency information
     <LI> Renamed/retyped G3D::PosedModel::adjacentFaces to G3D::PosedModel::vertices 
	   (most programs can be fixed by changing the type from Array< Array<int> > to
	   Array<MeshAlg::Vertex> and adjacentVertexArray[v] to vertexArray[v].faceIndex)
	 <LI> Shadow volumes now use the welded adjacency information
	 <LI> G3D::PosedModel now offers both welded and non-welded adjacency information
     <LI> G3D::contains for C-Arrays
     <LI> Generate .tag files in the build
     <LI> G3D::MeshAlg::computeAdjacency does not merge colocated vertices
     <LI> G3D::MeshAlg::computeAdjacency does not remove degenerate faces and edges
     <LI> G3D::MeshAlg::Vertex
     <LI> G3D::Vector3::directionOrZero
     <LI> G3D::GMaterial
     <LI> ManualCameraController renamed to G3D::FPCameraController
     <LI> glGetCurrentContext (beta)
     <LI> G3D::RenderDevice::supportsTextureFormat
     <LI> G3D::Vector3::magnitude
     <LI> G3D::Vector3::cross() [returns Matrix3]
     <LI> G3D::Quat changes (API is still in beta)
	 <LI> G3D::Quat::norm now returns the 2-norm, not the function Dave Eberly uses.
     <LI> Matrix3 default constructor
     <LI> Switched UserInput to use SDLWindow internally
	 <LI> Switched RenderDevice to use SDLWindow internally
	 <LI> G3D::Window
	 <LI> G3D::SDLWindow
     <LI> Renamed G3D::RenderDeviceSettings to G3D::WindowSettings (with a typedef for the old name)
     <LI> IFSModel now loads models with up to 10 million polygons (like the buddha).
	 <LI> Internal G3D::KDTreeSet state now private.
   </UL>

   <P>
   Changes in 6.00:
   <UL>
     <LI> FIX: warning: passing `double' for argument 1 of `void G3D::Queue<T>::repackAndRealloc(int)'
     <LI> Optimized static Matrix3::transpose (36 cycle) and
	      Matrix3::mul (52 cycle) variations.
     <LI> Changed some lerp arguments from float to double
	 <LI> MeshAlg::computeTangentSpaceBasis
	 <LI> Draw::axes now uses scale to compute axis length
     <LI> New ParallaxBump demo
     <LI> Changed several Vector3 return values from float to double
     <LI> Real-world stars, sun, and moon path (Nick Musurca)
     <LI> Now compiles under MSVC++ 7.0 (David Baszucki)
	 <LI> Now compiles under g++ OS/X (Ben Landon)
     <LI> Changed the default RenderDeviceSettings::alphaBits to 0 in the hope that it
	      will work with more graphics cards.
     <LI> Matrix3::fromX methods became factory methods
     <LI> G3D::sinc
     <LI> Multi-platform lib directories
     <LI> Vector3::average(), Color3::average(), Vector3::sum(), Color3::sum()
     <LI> Ray::reflect, Ray::refract
     <LI> Physically correct sky model
     <LI> FIX: Older graphics cards can now initialize properly
     <LI> Increased fuzzyEpsilon to 0.000001
     <LI> Color3::max, Color3::min, Color4::max, Color4::min
     <LI> Array::sortSubArray
     <LI> GCamera::getClipPlanes now takes a G3D::Array
     <LI> G3D::AABox
     <LI> Box::randomInteriorPoint, Box::randomSurfacePoint
     <LI> Vector3::cosRandom, Vector3::hemiRandom, Vector3::reflectAbout, Vector3::reflectionDirection, Vector3::refractionDirection
     <LI> log(Color3)
     <LI> Upgraded to zlib 1.2.1
     <LI> VAR::valid (Peter)
     <LI> System::getLocalTime, System::getTicks
	 <LI> High-performance cycle count and time queries on Linux
     <LI> UserInput::anyKeyPressed
     <LI> G3D::Box now provides axes, center, and extent information
	        (serialization is backwards compatible to 5.xx)
     <LI> TextInput's exceptions now provide file, line, and character numbers
	      as well as preformatted error messages in the style of MSVC++.
     <LI> G3D::Texture::fromGImage
     <LI> G3D::TextInput now parses hex numbers of the form 0x#####
     <LI> G3D::CollisionDetection::penetrationDepthForFixedSphereFixedPlane
	 <LI> G3D::CollisionDetection::penetrationDepthForFixedSphereFixedBox
     <LI> G3D::beginMarkShadows, G3D::endMarkShadows, G3D::markShadows
     <LI> GFont::draw2D now returns the string bounds
     <LI> Sphere::surfaceArea, Sphere::volume, Box::surfaceArea, Box::volume
     <LI> Two-sided stencil operations
     <LI> Removed G3D::Real
     <LI> FIX: [ 855947 ] Fonts are broken on Radeon
     <LI> Switched vertex arrays to use the new ARB_vertex_buffer_object extension.
	      Compared to 5.xx rendering speed: NVIDIA/Win32 is the same (fast),
		  ATI and Linux rendering are about 10x faster.  The API has changed
		  slightly-- most significant, the vertex, normal, color, etc. arrays
		  must all come from the same VARArea now.
     <LI> Disabled the "conditional is constant" level 4 warning on Windows
	      that is triggered by the for-loop scoping fix.
     <LI> G3D::LightingParameters::directionalLight
     <LI> G3D::TextureManager (Peter S. & Morgan)
     <LI> Flipped skybox X-axis to match OpenGL cube map coordinates
     <LI> Texture now uses hardware MIP-map generation
     <LI> Texture::copyFromScreen for cube map faces
     <LI> RenderDevice::configureReflectionMap
     <LI> RenderDevice::configureShadowMap
     <LI> Renamed CFont to GFont
     <LI> Renamed CImage to GImage
	 <LI> G3D::Matrix3::getRow
	 <LI> Added optional argument drawCelestialBodies to Sky::create.
     <LI> RenderDevice::getTextureMatrix
     <LI> Depth Textures
	 <LI> Texture::createEmpty
	 <LI> RenderDevice::setViewport has flipped the y-axis since version 5.00
     <LI> ReferenceCountedPointer::isLastReference
     <LI> Support for textures beyond the number of texture units (which occurs on NVIDIA cards)
     <LI> G3D::PosedModel
	 <LI> G3D::IFSModel
     <LI> G3D::CoordinateFrame::normalToObjectSpace, G3D::CoordinateFrame::normalToWorldSpace
     <LI> Simplified arguments on Texture::copyFromScreen
     <LI> Moved Camera in GLG3D to GCamera in G3D
     <LI> Moved setProjectionAndCameraMatrix from Camera to RenderDevice
     <LI> Moved G3D::Rect2D to G3D from GLG3D, changed interface
     <LI> G3D::setRenderMode
     <LI> G3D::RenderDevice::setSpecularCoefficient, G3D::RenderDevice::setShininess
     <LI> G3D::GLight
	 <LI> Renamed G3D::RenderDevice::configureDirectionalLight, configurePointLight to G3D::RenderDevice::setLight
     <LI> Changed G3D::Rect2D to use doubles
     <LI> G3D::Camera::setPosition()
     <LI> G3D::Camera::lookAt()
	 <LI> G3D::ManualCameraController::setPosition()
     <LI> G3D::System::getTick, G3D::System::getLocalTime
     <LI> Fixed [ 839618 ] peak var only updated on reset()
     <LI> G3D::Array::findIndex (thanks to David Baszucki for the suggestion)
     <LI> Removed RenderDevice::setProjectionMatrix3D and RenderDevice::setProjectionMatrix2D
     <LI> RenderDevice::project 
     <LI> RenderDevice::push2D() now uses the current viewport instead of full screen by default
     <LI> RenderDevice::getViewport
     <LI> G3D::SimTime
     <LI> Sky::render no longer needs a camera matrix (it gets it from the render device)
     <LI> SkyRef, Sky::create()
     <LI> Removed Sky::getName
     <LI> Removed RenderDevice::setAmbientLightLevel (duplicated RenderDevice::setAmbientLightColor)
     <LI> G3D::GApp, G3D::GApplet, G3D::GAppSettings
     <LI> RenderDevice::getCardDescription
     <LI> GPUProgram interface for setting program constants [Peter, Morgan & Dan]
     <LI> RenderDevice::getModelViewMatrix
     <LI> RenderDevice::getModelViewProjectionMatrix
     <LI> RenderDevice::getProjectionMatrix
     <LI> Documented some more common compiler errors.
     <LI> Moved RenderDevice::debugDraw methods to the Draw class, changed rendering from
	      cylinders to lines for wireframe (for performance)
     <LI> Ray::direction no longer has unit length
     <LI> Line::point, Line::direction
	 <LI> LineSegment::endPoint
     <LI> IFSBuilder loads Brown University Sketch Model (sm) format
	 <LI> New IFS models: angel, distributor-cap, dragon2, duck, elephant, hippo, hub, mech-part, rotor, sandal, trumpet, venus-torso, woman
     <LI> RenderDevices are now optionally resizable
     <LI> MeshAlg::computeWeld
     <LI> Array::randomize
     <LI> Table now refuses to push the load factor above 19/20 and stops rehashing
	 <LI> Table always keeps an odd number of buckets
     <LI> Sphere::randomInteriorPoint, Sphere::randomSurfacePoint
	 <LI> LineSegment::randomPoint
	 <LI> Hardcoded some common paths into demoFindData
     <LI> Deprecated old RenderDevice::init method.	      
     <LI> Full screen anti-aliasing (FSAA)
     <LI> G3D::RenderDeviceSettings
     <LI> All 2, 3, and 4 character swizzles for Vector2, Vector3, Vector4 are defined.
     <LI> G3D::rsqrt
     <LI> Most vector methods are also defined as functions now
	 <LI> sign(Vector2), sign(Vector3), sign(Vector4)
	 <LI> G3D::Matrix4
     <LI> Changed G3D_VER from double to integer
     <LI> G3D::lerp
     <LI> Changed G3D::PI, G3D::HALF_PI, and G3D::TWO_PI to #defines
     <LI> Vector2::clamp, Vector3::clamp, Vector4::clamp
     <LI> Changed order of arguments to all lerp methods to match DirectX/Cg
	 <LI> Changed order of arguments to G3D::clamp and G3D::iClamp to match DirectX/Cg
     <LI> G3D::ManualCameraController::ManualCameraController now requires a G3D::UserInput
	 <LI> G3D::UserInput::appHasFocus
     <LI> G3D::ManualCameraController now stops tracking the mouse when the app loses focus
     <LI> G3D::ManualCameraController::setActive
	 <LI> G3D::ManualCameraController now manages the mouse cursor instead of G3D::RenderDevice
	 <LI> G3D::UserInput::getMouseXY, G3D::UserInput::getXY
     <LI> RenderDevice::debugDrawVertexNormals
     <LI> GPUProgram, VertexProgram, and PixelProgram now recognize the output of the
	      Cg compiler and automatically bind constants.
     <LI> RenderDevice now loads glActiveStencilFaceEXT
     <LI> RenderDevice::numTextureCoords
     <LI> Moved changelog to a separate page
	 <LI> Reformatted overview to be smaller
     <LI> Added model debugging info to the IFSBuilder display
     <LI> Welded some broken vertices in the teapot.ifs file
     <LI> Renamed Font.* to CFont.*
     <LI> CFont::draw2DString renamed to CFont::draw2D (use a #define to port old code)
     <LI> MeshAlg
     <LI> RenderDevice now enables GL_COLOR_MATERIAL by default
     <LI> msgBox
     <LI> MD2 model gallery in documentation (Kevin)
     <LI> MD2Documentor (Kevin)
     <LI> debugAssertGLOk macro
     <LI> VertexProgram now supports NVIDIA Vertex Program 2.0
     <LI> RenderDevice now loads glGenProgramsNV, glDeleteProgramsNV, glBindProgramNV, glLoadProgramNV, glTrackMatrixNV, glProgramParameter4fvNV, glGetProgramParameterfvNV, glGetProgramParameterdvNV extensions	 
     <LI> VertexProgram and PixelProgram static factory methods now return reference counted values.
     <LI> Split the reference value from RenderDevice::setStencilTest into setStencilConstant
     <LI> RenderDevice::STENCIL_INVERT, RenderDevice::STENCIL_REPLACE, RenderDevice::STENCIL_ZERO 
     <LI> Added brighten argument to Texture::fromFile
     <LI> Increased CImage JPEG save quality
     <LI> RenderDevice::screenshot now returns the name of the file that was written
     <LI> nextPowerOf2 renamed to ceilPow2
     <LI> System::alignedMalloc, System::alignedFree
     <LI> Carbon, Crackman, Edenmill, Futurist, Interplanetary,
          Iomanoid, Starlight, Lesser, and Wild fonts by Ray Larabie.
          Like all of our fonts, they are free, but please consider a
          donation to him if you like them.  http://www.larabiefonts.com/
     <LI> MD2Model_Demo
     <LI> G3D::MD2Model
     <LI> FIX: Fixed a bug in Array shrinking that could cause memory corruption
     <LI> FIX: RenderDevice windows with an aspect ratio of less than 1 now allowed.
     <LI> FIX: TextInput now parses '#', '~', '~=', '&', '&&', '|', '||' correctly
     <LI> VARArea::reset() now waits for rendering calls using its vertex
          arrays to complete before wiping the memory.
     <LI> G3D::filenameBaseExt, G3D::filenameExt
     <LI> VARArea::finish()
     <LI> Milestone
	 <LI> TextInput::Options::signedNumbers
     <LI> RenderDevice now loads glFlushVertexArrayRangeNV
     <LI> Vector2int16
     <LI> RenderDevice::freeVARSize()
     <LI> Array now allocates 16-byte aligned pointers.
     <LI> Decreased the default camera movement rate by 50% for better resolution.
     <LI> RenderDevice enables GL_NORMALIZE by default
     <LI> Improved the performance of Array::append/Array::push/Array::next
	 <LI> Fix: [ 875219 ] Array::sort must use std::sort
     <LI> Array::next
     <LI> Array::reverse
     <LI> PCX file loading
     <LI> Test images
     <LI> Color3uint8 as uint8[] addressing
     <LI> Color4uint8 as uint8[] addressing
     <LI> Removed const from VAR::pointer
     <LI> ReferenceCountedPointer::isNull
     <LI> alwaysAssertM
     <LI> Log::common, Log::getCommonLogFilename
     <LI> Switched from static to dynamic linking of zlib
     <LI> Upgraded to zlib 1.1.3
     <LI> On Win32 the lib list is automatically updated through pragmas
          (5.xx programs should revert to linking against default libraries)
     <LI> Increased default sky quality to 1.00
     <LI> G3D::CFontRef
     <LI> RenderDevice now loads all register combiner extensions (NVIDIA only)
     <LI> Sky::getEnvironmentMap
     <LI> Sky implementation now uses a cube map (when one is available)
     <LI> G3D::Sky constructor now takes a render device
     <LI> Rotated Sky box 90 degrees to match environment maps
     <LI> G3D::Sky now takes the environment filenames as "sky_*.jpg" instead of "sky_ft.jpg"
     <LI> Added default filename for Sky constructor
     <LI> Added caustics textures created with Kjell Andersson's generator http://www.lysator.liu.se/~kand/caustics/
     <LI> #defined "for" under MSVC so that it obeys C99 scoping rules
     <LI> System::consoleKeyPressed
     <LI> System::consoleClearScreen
     <LI> System::consoleReadKey
     <LI> NetMessage::type()
     <LI> Changed the Conduit message protocol to include a message type.
          The API is backwards compatible to 5.01 even though the protocol is not.
     <LI> Removed optional argument maxSize from LightweightConduit::receive.
     <LI> NetAddress::serialize
     <LI> NetAddress::deserialize
     <LI> NetAddress == NetAddress
     <LI> hashCode(NetAddress)
     <LI> RenderDevice::init now prints ATI or NVIDIA driver version to the log under Windows
     <LI> readme.html library build instructions now have downloads for required libraries
     <LI> Library list has changed for Win32 (added version.lib)
     <LI> System::cpuArchitecture
     <LI> System::operatingSystem
     <LI> double-precision Plane::getEquation
     <LI> Vector2::lerp
     <LI> Platform specific #defines G3D_WIN32, G3D_LINUX, G3D_OSX
     <LI> G3D::Array::contains
     <LI> G3D::Queue::contains
     <LI> G3D::TextureFormat
     <LI> G3D::Texture::DIM_CUBE_MAP
     <LI> G3D::Texture resizes non-power of two textures
     <LI> G3D::Texture constructors are completely changed from 5.01 (and hopefully easier to use)
     <LI> G3D::CImage now supports images with alpha
     <LI> Removed most of the width/height arguments from G3D::Camera methods
     <LI> BinaryInput::readBytes and BinaryOutput::writeBytes now take void* as an argument to avoid casting
     <LI> Plane::fromEquation
     <LI> Removed Plane::getNormal (use Plane::normal instead)
     <LI> Removed CDTriangle (use G3D::Triangle instead)
     <LI> Removed Font (use G3D::CFont instead)
     <LI> FIX: Camera::getClipPlanes now transforms infinite planes correctly.
     <LI> FIX: The last reference of an RGC pointer assigned to itself no
               longer tries to collect before re-assigning
   </UL>

   <P>
   Changes in 5.01
   <UL>
     <LI> G3D::tesselateComplexPolygon
	 <LI> G3D::ConvexPolygon
	 <LI> G3D::ConvexPolyhedron
     <LI> G3D::iClamp, G3D::clamp
	 <LI> G3D::iWrap
	 <LI> G3D::iRandom, G3D::random
     <LI> G3D::getFiles
     <LI> G3D::getDirs
     <LI> G3D::VAR::pointer
     <LI> G3D::realWorldLocalTime
     <LI> G3D::Texture::TRANSPARENT_BORDER
     <LI> DECLARE_GLFORMATOF
     <LI> G3D::System::machineEndian
     <LI> G3D::VertexProgram, G3D::VertexProgramRef, G3D::RenderDevice::setVertexProgram
     <LI> G3D::PixelProgram, G3D::PixelProgramRef, G3D::RenderDevice::setPixelProgram
     <LI> G3D::GPUProgram, G3D::GPUProgramRef
     <LI> G3D::sizeOfGLFormat
     <LI> G3D::RenderDevice::setVertexAttrib
     <LI> G3D::Vector2*=Vector2, /= Vector2, * Vector2, / Vector2
     <LI> glFormatOf
     <LI> G3D::Color4uint8
     <LI> G3D::Color3uint8
     <LI> G3D::Vector3int16
     <LI> G3D::System::currentProgramFilename
     <LI> CImage::insertRedAsAlpha
     <LI> CImage::stripAlpha
     <LI> Texture::hasAlpha
     <LI> Added support for TGA with alpha channel
     <LI> Re-implemented Texture to support a broader range of formats and cleaner implementation.
     <LI> Fix: Improved Texture::LUMINANCE support
     <LI> Added == and != overloads for TextureRef so that "a != NULL" is now legal and does not require a cast to TextureRef.
     <LI> G3D::CFont is a typedef for G3D::Font to avoid name conflicts with X11 Font under Linux.  In future releases, the name Font will be deprecated.
     <LI> RenderDevice::setPointSize
	 <LI> Added a new teapot (teapot.ifs) that is closed, with a properly fitting top.  The classic teapot is now called "utah-teapot.ifs" (Sebastian Schuberth and Simon Winkelbach)
     <LI> RenderDevice::init now loads glPointParameterfvARB, glPointParameterfARB,
        glMultiDrawArraysEXT, and glMultiDrawElementsEXT functions.
     <LI> GLenumToString(4) now returns "GL_TRIANGLES" instead of "GL_LINE_BIT" (both are correct)
     <LI> Added TextInput::Options to optionally allow C++ comments to
          be treated as two slashes instead of a comment
     <LI> Added data/image/meter.jpg, a meter stick texture convenient for testing
     <LI> Added sansserif, news, and terminal fonts based on Bitstream's <A HREF="http://www.gnome.org/fonts/">free fonts</A>
     <LI> RenderDevice::numTextureUnits
     <LI> Added stars to night Sky
     <LI> Added classic GL dinosaur model as data/ifs/dinosaur.ifs
     <LI> Documented G3D::glGetProcAddress
     <LI> Fix: Texture now restored GL_ENABLE bits properly after creation
     <LI> Fix: Texture::sizeInMemory now accounts for MIP-map levels
     <LI> Fix: Fonts and skies now adjust their brightness for the screen gamma level
     <LI> Fix: Strange compilation bug was causing Sky to be black for some programs
     <LI> resolveFilename
     <LI> GLProgram_Demo to show how to use vertex programs in G3D
     <LI> Support for GL_ARB_vertex_program 
     <LI> Modified ManualCameraController so that diagonal movement does not exceed
          maximum rate.
     <LI> Added support for non-GL_FLOAT vertex arrays to RenderDevice
     <LI> Added support for Wavefront OBJ files to IFSBuilder
     <LI> Removed duplicate copies of SDL.dll from the source tree
     <LI> Renamed G3D::CDTriangle to G3D::Triangle
     <LI> Added several G3D::Triangle methods
     <LI> Moved CollisionDetection::primaryAxis to Vector3::primaryAxis
     <LI> Fix: Texture::sizeInMemory now returns correct results for RGB8 textures.
     <LI> Changed texture constructors in ways that slightly break backwards compatibility
     <LI> Deprecated several arguments to the texture constructors.
   </UL>


   Changes in 5.00
   <UL>
     <LI> Color3::operator*=(const Color3&)
     <LI> Color3::operator*(const Color3&)
     <LI> Eliminated duplicate GL headers [James O'Sullivan]
     <LI> Linux Makefiles [James O'Sullivan, Jordan Parker]
     <LI> RenderDevice::getProjectionMatrixParams
     <LI> RenderDevice::debugDrawCylinder
     <LI> Added an option to not copy input memory for BinaryInput
     <LI> Added data/ifs/sphere.ifs
     <LI> Added data/ifs/spikeball.ifs
     <LI> Added a new (imperfect) demo/tool that converts 3DS and MD2 to IFS.
     <LI> Added RenderDevice to the Font constructor
     <LI> Removed RenderDevice from Font::drawString
     <LI> Included glut32.lib, .dll, and .h (Version 3.7.6) in the distribution. 
          The windows glut port is by Nate Robbins and is from 
          http://www.xmission.com/~nate/glut.html. 
          glut was originally written by Mark Kilgard.
     <LI> Modified OpenGL headers to work cross platform, with the latest NVIDIA extensions
     <LI> Changed library name from graphics3D.lib to G3D.lib, same for
          debug version.
     <LI> Changed directory structure and added readme.html to explain
          the new setup.
     <LI> Changed BinaryInput::readBytes to allow reading onto the stack
     <LI> Added Vector4::isFinite
     <LI> G3D::CDTriangle (for 35% faster collision detection)
     <LI> CollisionDetection::closestPointToRectangle
     <LI> CollisionDetection::movingSpherePassesThroughFixedBox
     <LI> CollisionDetection::movingSpherePassesThroughFixedSphere
     <LI> Changed CollisionDetection::movingXFixedTriangle arguments
     <LI> CollisionDetection::collisionTimeForMovingSphereFixedSphere
     <LI> Changed CollisionDetection::distanceToX methods to closestPointToX
     <LI> Vector3::NAN3
     <LI> Made Vector3::isUnit fuzzy
     <LI> Made Vector3::isZero fuzzy
     <LI> Fix: Texture(std::string, std::string) constructor now works for alpha-only textures. 
     <LI> FIX: Array now calls copy constructor when resizing
     <LI> FIX: Triangle-sphere and rectangle-sphere collision detection
              returned an incorrect collision location; now fixed.
     <LI> FIX: changed VectorX::isFinite to call isFinite (used to give bad result for NaNs)
     <LI> FIX: Used the normalized edge to compute intersection in
            CollisionDetection::distanceToTrianglePerimeter
     <LI> FIX: Changed the order of corners returned from Box::getFaceCorners so the
	      face is ccw, facing out
     <LI> FIX: ManualCameraController::lookAt now faces along the -z axis.
     <LI> FIX: data/ifs/icosa.ifs model is now an icosahedron
     <LI> Made Set::begin() and Set::end() const
     <LI> Added ifdef _WIN32 all over for typedefing types from Windows to Linux and vice versa.
     <LI> G3D::isNaN, G3D::isFinite
     <LI> Added a single triangle triangle.ifs file
     <LI> G3D::LineSegment
     <LI> RenderDevice::debugDrawRay
     <LI> CoordinateFrame::toObjectSpace(Ray&)
     <LI> CoordinateFrame::toObjectSpace(Box&)
     <LI> CoordinateFrame::toObjectSpace(Sphere&)
     <LI> Changed CollisionDetection routines to return the surface normal of the
          surface at the collision location.
	 <LI> CollisionDetection::collisionTimeForMovingPointFixedCapsule
	 <LI> CollisionDetection::collisionTimeForMovingSphereFixedCapsule
     <LI> G3D::Capsule class
     <LI> Removed e-mail addresses from contributor list to protect them from spammers
     <LI> Linux port [Hari Khalsa & Chris Kern]
     <LI> Added serialize and deserialize methods, deserializing constructor to
	      Vector2, Vector3, Vector4, Color3, Color4, Matrix3, CoordinateFrame, Box,
		  Sphere, Plane, Ray, Line, Capsule, LineSegment
	 <LI> Moved parts of Plane.h into Plane.cpp
     <LI> BinaryInput::readBool8 and BinaryOutput::writeBool8
     <LI> G3D::System [based on Michael Herf, Rob Wyatt, and Benjamin
           Jurke's work]
     <LI> Networking infrastructure: G3D::NetworkDevice, G3D::NetAddress,
          G3D::ReliableConduit, G3D::LightweightConduit, G3D::NetListener
     <LI> G3D::Camera
     <LI> Vector2::toString
     <LI> G3D::createTempFile
     <LI> G3D::fileLength
     <LI> UserInput::setKeyMapping
     <LI> UserInput::keyCodeToString, UserInput::stringToKeyCode
     <LI> JPEG library uses createTempFile
     <LI> JPEG library will allocate up to 6MB before resorting to temp 
          files-- faster and more reliable
     <LI> Moved SDL initialization to RenderDevice constructor from the init
          method so extension can be used earlier
     <LI> Support for up to 8 texture units, no longer crashes on machines 
          that have more than 4 units
     <LI> Made Arrays allocate at least 32 bytes when resized to improve
          performance of small char stacks
     <LI> Added UserInput key codes for mouse wheel buttons
     <LI> UserInput::keyPressed, UserInput::pressedKeys()
     <LI> UserInput::KeyCode
     <LI> Renamed UserInput::poll() to UserInput::endEvents(), added 
          UserInput::beginEvents()
     <LI> Moved custom UserInput key codes into an enum so they are 
          compile-time constants
     <LI> Changed all <io.h> to <stdio.h> for cross-platform [Rob & Chris]
     <LI> Moved LITTLE_ENDIAN and BIG_ENDIAN constants to an enum and renamed 
          them to G3D_LITTLE_ENDIAN and G3D_BIG_ENDIAN for cross-platform 
          [Rob & Chris]
     <LI> Permanently fixed the precision of Real to be 32-bit float.
     <LI> RenderDevice now loads the NVIDIA VAR fence extensions.
     <LI> Renamed RenderDevice::begin to RenderDevice::beginPrimitive, same 
     for end.
     <LI> Redesigned the vertex array system; see VAR and VARArea.
     <LI> Changed GLG3D demo to demonstrate the use of the new VAR and 
        VARArea classes
     <LI> CoordinateFrame(Vector3) constructor.
     <LI> Improved the performance of zero-radius sphere [aka point] 
          collision detection
   </UL>

   <P>
    Changes in 4.01
   <UL>
     <LI> trimWhitespace()
     <LI> Pointwise multiplication and division for Vector3
     <LI> Array::sort now uses > operator by default; two alternative sort methods allow qsort style sorting
     <LI> Texture::copyFromScreen
     <LI> Texture::invertY
     <LI> BinaryInput/BinaryOutput compression (via zlib)
     <LI> Alpha-only G3D::Texture mode
     <LI> G3D::Font and fonts in data/font
     <LI> Array::fastRemove
     <LI> TextInput [Morgan & Aaron]
     <LI> Color4::CLEAR
     <LI> Table [] operator now returns a non-const reference
     <LI> RenderDevice::getFrameRate, RenderDevice::getTriangleRate, RenderDevice::getTriangleCount
     <LI> ManualCameraController::setMoveRate, ManualCameraController::setTurnRate
     <LI> LightingParameters default constructor
     <LI> Vector2, Vector3, Vector4 isZero(), isUnit(), isFinite()
     <LI> Vector4::length(), Vector4::squaredLength()
     <LI> isValidPointer now returns false for 0xFEEEFEEE
     <LI> RenderDevice checks for texture compression extensions
     <LI> Restructured the directories for the CPP sources (only affects people who build G3D)
     <LI> Included NVIDIA and SGI OpenGL headers in the distribution, changed install notes
     <LI> Fixed a bug that previously prevented textures from being garbage collected
     <LI> Fixed Line::distance returning values too small
     <LI> Fixed Plane(normal, point) constructor to compute point from normalized direction [Kevin]
     <LI> LED font by Matthew Welch daffy-duck@worldnet.att.net
     <LI> VenusRising font by Ray Larabie <A HREF="mailto:drowsy@cheerful.com">drowsy@cheerful.com</A>
     <LI> VideoFreak font by Jakob Fischer pizzadude@pizzadude.dk
   </UL>

   <P>
   Changes in 4.00
   <UL>
     <LI> Moved texture combine modes from Textures onto RenderDevice texture units
     <LI> Documented RenderDevice::getHDC() (Windows only)
     <LI> Renamed RenderDevice::swapBuffers() to RenderDevice::endFrame(), added corresponding RenderDevice::beginFrame()
     <LI> Moved getNumJoySticks from RenderDevice to UserInput
     <LI> Added TEX_ADD combine mode
     <LI> Table::getKeys and Set::getMembers now have overloads that take an Array as input.
     <LI> BinaryOutput::getCArray
     <LI> RenderDevice::getObjectToWorldMatrix(), RenderDevice::getCameraToWorldMatrix()
     <LI> RenderDevice::debugDrawAxes(), RenderDevice::debugDrawBox(), RenderDevice::debugDrawSphere()
     <LI> Color3::Color3(const Vector3&) and Color4::Color4(const Vector4&)
     <LI> Moved hashCode(const Vector3&) and hashCode(const Vector4&) to the global namespace [Kevin]
     <LI> isValidPointer now returns false for 0xCCCCCCCC and 0xDEADBEEF
     <LI> Fix: RenderDevice::setPolygonOffset now affects polygons rendered in line and point mode
     <LI> Fix: Sun is now invisible after it goes below the horizon
     <LI> Fix: BinaryInput now supports endian-ness correctly in memory read mode
     <LI> Fix: Table.copyFrom and copy constructor now work
   </UL>

   <P>
   Changes in 3.02
   <UL>
     <LI> Built libraries using "Multithreaded DLL" [Kevin & Darius]
     <LI> Added depth, color, and stencil bit depth preferences to G3D::RenderDevice
     <LI> G3D::Sky (plus sky directory in the data distribution)
     <LI> Sky cube data [Jauhn Dabz, jauhn@yahoo.com, http://nullpoint.fragland.net]
     <LI> G3D::UserInput
     <LI> G3D::ManualCameraController
     <LI> G3D::LightingParameters
     <LI> G3D::toSeconds, G3D::AMPM, G3D::GameTime, G3D::RealTime
     <LI> G3D::RenderDevice::project
     <LI> G3D::linearSpline
     <LI> G3D::Color3::fromARGB and G3D::Color4::fromARGB
     <LI> Added non-const G3D::Array::last() [Kevin]
     <LI> Modified G3D::RenderDevice::configureDirectionalLight to operate in world space
     <LI> Fix: Flipped the y-axis of G3D::RenderDevice::getDepthBufferValue so it matches the documentation.
     <LI> Removed brief descriptions from documentation
     <LI> Removed sqrt, sin, cos, etc. that conflict with standard library names
     <LI> Removed TWO_PI constant
     <LI> Removed G3D::Matrix3 virtual destructor
     <LI> Removed G3D::Quat virtual destructor [Kevin]
   </UL>

  
   Changes in 3.01
   <UL>
     <LI> Changed an assert() to debugAssert() in Queue.h
     <LI> G3D::Table doesn't grow the number of buckets under bad hash codes [Morgan & Darius]
     <LI> G3D::Table allocates only 10 initial buckets
     <LI> G3D::Table::debugGetLoad()
     <LI> G3D::CollisionDetection::collisionTimeForMovingPointFixedRectangle
     <LI> G3D::CollisionDetection::collisionTimeForMovingPointFixedBox
     <LI> G3D::Ray::intersectionTime, G3D::Ray::unit()
     <LI> G3D::Log [Morgan & Aaron]
     <LI> G3D::RenderDevice (OpenGL state abstraction.  VertexBuffer support is beta only)
     <LI> G3D::Texture (includes texture compression, image loading, and texture rectangle)
     <LI> Added a comment to the vector classes noting that they can't be sublcassed [Kevin Egan]
   </UL>

   Changes in 3.00
   <UL>
     <LI> G3D::NEWLINE
     <LI> writeStringToFile
     <LI> Fixed empty stringJoin bug
     <LI> Fixed parseFilename with no path bug
     <LI> Vector3::INF3, Vector3::ZERO3
     <LI> G3D::PhysicsFrame (beta-- this interface is going to change in 4.00)
     <LI> G3D::Vector4
     <LI> G3D::Queue
     <LI> Default constructor for G3D::CImage
     <LI> G3D::isValidHeapPointer, G3D::isValidPointer
     <LI> G3D::Ray
     <LI> CImage copy constructor, CImage::load
     <LI> Removed \#pragma once for gcc compatibility
     <LI> Renamed several hashcode methods to hashCode
     <LI> Fixed fuzzy math to work with infinite numbers
     <LI> Fixed Table::remove(), Set::remove() bug [Darius Jazayeri]
     <LI> G3D::CoordinateFrame.toObjectSpace(Vector4), G3D::CoordinateFrame.toWorldSpace(Vector4)
     <LI> Added the data directory
     <LI> G3D::CollisionDetection
     <LI> G3D::Sphere::culledBy()
     <LI> Added the GLG3D library [Morgan McGuire & Seth Block]
     <LI> Changed SDL_GL_Demo to use GLG3D, rotate triangle, and use color blending
     <LI> Fixed debugPrintf to handle long strings on Win32
     <LI> Wrapped the MMX headers with \#ifdefs [Nate Miller]
     <LI> Moved OpenGL code out of CoordinateFrame.h/cpp
     <LI> Fixed BinaryInput readVector*, readColor* to read in correct order [Nate Miller]
     <LI> BinaryInput::readVector4, BinaryInput::readColor4, BinaryOutput::writeVector4, BinaryOutput::writeColor4
     <LI> IFS_Demo for loading IFS files, dealing with models in OpenGL [Nate Miller]
   </UL>

   <P>
   Changes in 2.00
   <UL>
     <LI> Vector2 members renamed to x,y from s,t
     <LI> Added SDL_GL_Demo and Win32_Demo
     <LI> Removed Group
   </UL>

   <P>
   Changes in 1.10
   <UL>
     <LI> CImage, color conversion routines [Morgan McGuire, John Chisholm, and Edward Resnick]
     <LI> Array dereference for BinaryInput
     <LI> BinaryInput from memory
     <LI> BinaryOutput to memory
     <LI> toUpper(std::string), toLower(std::string)
     <LI> Group::clear()
     <LI> inf, nan as global constants (double precision)
     <LI> Can iterate over const Tables
     <LI> Table::deleteValues()
     <LI> Fixed an off-by-one bug in BinaryInput::readString()
     <LI> beginsWith() and wordWrap() string utilities
     <LI> prompt dialogs have fixed width font [Kurt Miller]
     <LI> iMax(), iMin()
     <LI> Array::sort()
     <LI> stringCompare(), stringPtrCompare()
     <LI> readFileAsString()
     <LI> Fixed textPrompt() to wait for input
     <LI> BinaryInput.getFilename(), BinaryOutput.getFilename()
     <LI> ReferenceCount [Justin Miller]
     <LI> endsWith()
     <LI> stringSplit(), stringJoin()
     <LI> Renamed format.* to stringutils.*
     <LI> fileExists(), parseFilename(), createDirectory(), copyFile()
     <LI> highestBit() [Jukka Liimatta]
     <LI> flipRGBVertical()
     <LI> Changed all header guards to use G3D_ prefix
     <LI> ConvexPolyhedron
     <LI> Virtual destructors on almost all objects.
     <LI> RGBtoBGR()
     <LI> Color4
     <LI> Array::pop(bool shrinkArray=true)
     <LI> Vector2::isFinite, Vector2::fuzzyEq, Vector::fuzzyNe
   </UL>
   <P>

   Changes in 1.09
   <UL>
     <LI> Removed pointer hash [Aaron Orenstein]
     <LI> Changed some includes from quotes to pointy brackets [Aaron Orenstein]
     <LI> Sphere::toString() 
     <LI> Plane::toString()
     <LI> Added a change log
   </UL>

   */