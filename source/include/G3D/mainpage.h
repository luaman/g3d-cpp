// Copyright 2000-2004, Morgan McGuire.
// All rights reserved.
//
//   This library is recommended for use with the textbook:
//   <BR>Morgan McGuire, <I>3D Computer Graphics Techniques</I>, Prentice Hall
//

/**
 @mainpage
   <P>
    <CENTER><A HREF="http://sourceforge.net/forum/?group_id=76879">Forums</A>
    | <A HREF="map.png">Namespace Map</A> 
	| <A HREF="error.html">Error FAQ</A> 
    |   \ref Introduction  |  \ref Installation  | <A HREF="gettingstarted.html">Getting Started</A> |
    \ref defines "\#defines"  |  \ref License  |  \ref Contributors   |
    <A HREF="changelog.html">Change Log</A></CENTER>

    \section Overview Overview
     This library contains the support code needed for every 3D
     project in C++.  It is designed to be easy to use, make it hard
     to write bugs, easy to find them, to compile to fast code, and to
     provide a single solution to graphics needs under many different
     compilers, operating systems, and processor architectures.

     <P>
     <CODE>\#include &lt;G3DAll.h&gt;</CODE> in your project (if you don't
     want the OpenGL "GLG3D" part use <CODE>&lt;graphics3D.h&gt;</CODE> instead).
     <UL>
     <LI><B>Geometry</B> &nbsp; &nbsp;
        G3D::AABox,
        G3D::Box,
		G3D::Capsule,
		G3D::CollisionDetection,
		G3D::Color3,
		G3D::Color3uint8,
        G3D::Color4,
		G3D::Color4uint8,
		G3D::Cone,
		G3D::ConvexPolygon,
		G3D::ConvexPolyhedron,
		G3D::CoordinateFrame,
        G3D::GCamera,
		G3D::PhysicsFrame,
		G3D::Line,
		G3D::LineSegment,
		G3D::Matrix3,
        G3D::Matrix4,
        G3D::MeshAlg,
        G3D::Quat,
		G3D::Plane,
		G3D::Ray,
        G3D::Rect2D,
        G3D::Sphere,
		G3D::Triangle,
		G3D::Vector2,
        G3D::Vector2int16,
		G3D::Vector3,
		G3D::Vector3int16,
		G3D::Vector4,
        G3D::GameTime,
        G3D::RealTime,
        G3D::tesselateComplexPolygon,
        G3D::toSeconds 
     
     <LI><B>Data structures</B> &nbsp; &nbsp;
        G3D::AABSPTree,
		G3D::Array,
		G3D::Queue,
 	    G3D::Set,
		G3D::Table

     <LI><B>Image formats</B> &nbsp; &nbsp;
		G3D::GImage,
		G3D::GImage::Error,	
		G3D::computeNormalMap,
        G3D::flipRGBVertical,
  	    G3D::RGBtoARGB,
		G3D::RGBtoBGR,
		G3D::RGBtoBGRA,
        G3D::RGBtoRGBA,
		G3D::RGBxRGBtoRGBA

     <LI><B>Integer math</B> &nbsp; &nbsp;
        G3D::ceilPow2, 
        G3D::highestBit,
	    G3D::iAbs,
		G3D::iCeil,
		G3D::iClamp,
		G3D::iWrap,
		G3D::iFloor,
		G3D::iMax, 
        G3D::iMin,
	    G3D::isPow2,
		G3D::isOdd,
		G3D::isEven, 
		G3D::iSign,
		G3D::iRound,
		G3D::iRandom,
		G3D::uint8,
		G3D::uint16,
		G3D::uint32,
		G3D::uint64,
        G3D::int8,
		G3D::int16,
		G3D::int32,
		G3D::int64
     
     <LI><B>Floating point</B> &nbsp; &nbsp;
	     G3D::abs,
		 G3D::aCos,
		 G3D::aSin,
		 G3D::aTan,
		 G3D::aTan2,
         G3D::clamp,
         G3D::cyclicCatmullRomSpline,
		 G3D::float32,
         G3D::float64
         G3D::fuzzyEq,
         G3D::fuzzyNe,
         G3D::fuzzyGt,
         G3D::fuzzyGe,
         G3D::fuzzyLt,
         G3D::fuzzyLe,
         G3D::fuzzyEpsilon,
         G3D::lerp,
		 G3D::linearSpline,
         G3D::sign,
		 G3D::rsq,
		 G3D::unitRandom,
		 G3D::random,
         G3D::symmetricRandom,
         G3D::min,
		 G3D::max,
		 G3D::square,
         G3D::sinc,
		 G3D::sumSquares,
		 G3D::distance,
         G3D::toRadians,
		 G3D::toDegrees,
		 G3D::isNaN,
		 G3D::isFinite,
         G3D::inf,
		 G3D::nan,
		 G3D::infReal,
		 G3D::nanReal,
         G3D::wrap,
		 G3D::PI, 
         G3D::HALF_PI,
		 G3D::TWO_PI,
         G3D::rsqrt
         
     <LI><B>String</B> &nbsp; &nbsp;
        G3D::beginsWith, G3D::endsWith, G3D::license, G3D::stringSplit,
        G3D::stringJoin, G3D::format, G3D::vformat, G3D::wordWrap, 
        G3D::stringCompare, G3D::stringPtrCompare, G3D::toUpper, G3D::toLower,
        G3D::NEWLINE, G3D::trimWhitespace

     <LI><B>Debug</B> &nbsp; &nbsp;
        <B>G3D::debugAssert</B>, G3D::isValidHeapPointer,  
        G3D::isValidPointer,
        <B>G3D::debugAssertM</B>, G3D::debugPrintf, G3D::error, 
        G3D::Log, G3D::alwaysAssertM

     <LI><B>I/O</B> &nbsp; &nbsp;
        G3D::BinaryInput, 
        G3D::BinaryOutput, 
        G3D::copyFile, 
        G3D::createDirectory,
        G3D::createTempFile, 
        G3D::fileExists, 
        G3D::fileLength,
        G3D::filenameBaseExt,
        G3D::filenameExt,
        G3D::G3DEndian,
        G3D::getFiles, 
        G3D::getDirs,
        G3D::msgBox,
        G3D::parseFilename, 
        G3D::prompt, 
        G3D::readFileAsString,
        G3D::System,
        G3D::resolveFilename,
        G3D::TextInput,
		G3D::TextOutput,
        G3D::Token,
        G3D::TextInput::WrongTokenType,
        G3D::writeStringToFile

    <LI><B>Network</B> &nbsp; &nbsp;
        G3D::DiscoveryAdvertisement,
        G3D::DiscoveryClient,
        G3D::DiscoveryServer,
        G3D::DiscoverySettings,
        G3D::LightweightConduit,
        G3D::NetAddress,
        G3D::NetListener,
        G3D::NetMessage,
        G3D::NetworkDevice, 
        G3D::ReliableConduit
     </UL>

     <UL>
      <LI><B>OpenGL Abstraction</B> &nbsp; &nbsp;
        G3D::Draw,
        G3D::GWindow,
        G3D::SDLWindow,
        G3D::Milestone,
        G3D::PixelProgram,
        G3D::RenderDevice,
        G3D::RenderDeviceSettings,
        G3D::Texture,
        G3D::TextureFormat,
        G3D::TextureManager,
        G3D::VAR,
        G3D::VARArea, 
        G3D::VertexProgram,
        G3D::VertexAndPixelShader,
        G3D::Shader,
        G3D::SimpleShader,
        G3D::ObjectShader

      <LI><B>OpenGL Extension</B> &nbsp; &nbsp;
        debugAssertGLOk,
        DECLARE_GLFORMATOF,
        G3D::glLoadMatrix, 
        G3D::glLoadInvMatrix, 
        G3D::GLCaps,
        G3D::glColor,
        G3D::glMultInvMatrix, 
        G3D::glMultMatrix, 
        G3D::glMultiTexCoord,
        G3D::glNormal, 
        G3D::glTexCoord, 
        G3D::glToScreen,
        G3D::glVertex,
        G3D::sizeOfGLFormat, 
        G3D::glFormatOf, 
        G3D::glGetProcAddress,
        G3D::getOpenGLState,
        G3D::glGetInteger, 
        G3D::glGetBoolean,
        G3D::glGetDouble, 
        G3D::glGetFloat,
        G3D::glGetMatrix,
        G3D::GLenumToString

      <LI><B>Game/Demo Infrastructure</B> &nbsp; &nbsp;
        G3D::beginMarkShadows, G3D::endMarkShadows, G3D::markShadows,
        G3D::drawFeatureEdges,
        G3D::GFont,
        G3D::GApp, G3D::GApplet, G3D::GAppSettings,
        G3D::GLight,
        G3D::GMaterial,
        G3D::Sky,
        G3D::LightingParameters,
        G3D::MD2Model,
        G3D::IFSModel,
        G3D::PosedModel,
        G3D::PosedModelWrapper,
        G3D::realWorldLocalTime, 
        G3D::UserInput, 
        G3D::ManualCameraController
     </UL>

    See the demo projects for a simple introduction to using SDL, OpenGL, 
    Graphics3D and MSVC++ to create and render 3D geometry.

   \section Introduction Introduction
   Online documentation and updates available at 
   <A HREF="http://www.graphics3d.com/cpp">http://www.graphics3d.com/cpp</A>.
   This C++ library is distributed through 
   <A HREF="http://www.graphics3d.com">graphics3d.com</A>. 

   <P>

   Graphics3D (G3D) is an open source library for game developers,
   students, and researchers.  The goal of the library is to provide a
   set of low level routines and data structures for getting projects
   rolling.  Its scope is limited to those routines and structures
   that are so common they are needed in almost every graphics
   program.  and has been optimized for high performance when building
   in release mode.  The straightforward and clear coding style of the
   source will help you see how routines are implemented.  Extensive
   debugging support and debug assertions in the codebase itself will
   make your C++ programs reliable and easy to debug as well as fast.
 
   Although the library provides most of the low level pieces for
   building a graphics engine, it is not a game/graphics engine by
   itself.  You have to assemble the pieces, provide structures for
   manipulating 3D models, and connect your program to a rendering
   engine.

   A bridge between G3D data types and the OpenGL graphics API is
   provided by the GLG3D part of the library.  If you aren't using
   OpenGL, don't link against this.  If you are using OpenGL, this
   library can help with debugging and make your code cleaner.


  
  <H3>Directories</H3>
   The distribution contains the following subdirectories:

    <UL>
      <LI><B>html</B> - HTML documentation for the library

      <LI><B>include</B> - Header files to include in your code.

      <LI><B>lib</B> - Static libraries to link against.

      <LI><B>data</B> - Standard 2D & 3D data to test against.

      <LI><B>demos</B> - Demonstrations of the library used in 
                         various configurations.  You should compile these
                         to verify your installation is complete and correct.

    </UL>
   

   <H3>Data</H3>
   <P>

   Standard (and mostly public domain) 2D and 3D data are provided with this
   library.  These may be useful as test data or for comparison with
   previously published results.
   
     <PRE>
       data           <I>Data root</I>
          image       <I>Test images</I>
          height      <I>height maps</I>
          ifs         <I>Models for use with G3D::IFSModel</I>
          sky         <I>Images and data for use with G3D::Sky</I>
          font        <I>Fonts for use with G3D::GFont</I>
          quake2      <I>Models for use with G3D::MD2Model</I>
     </PRE>

   <HR>
   \section defines defines
   <P>
    G3D pays attention to the following #defines.
   <P>
     <B>SSE</B> - Allows the use of Intel PIII SSE instructions for faster math
      routines.  On Win32, you will need the Visual Studio processor pack 
      (Free from http://msdn.microsoft.com/vstudio/downloads/ppack/default.asp)
      to build with this option.  Your code will only run on PIII or later
      machines.

    <P>
	 <B>_MSC_VER</B> - Use Microsoft x86 assembly when assembly code is needed.

    <P>
     <B>NO_SDL_MAIN</B> - Do not attempt to link against sdlmain.lib.

    <P>
     <B>_DEBUG</B> - Build in in debug mode.  This enables debugAssert, 
     debugBreak, debugAssertM, array bounds checks, etc.

    <P>
     <B>G3D_DEBUG_NOGUI</B> - When building in debug mode, this flag
      says to use stdout and not popups on Windows for assertion 
      failures and error messages.  On Linux the console is always used.

    <P>
     <B>_WIN32</B> - Build using Windows API calls 

   <P>
     The static libraries themselves are built <I>without</I> SSE.  The 
     -debug versions have _DEBUG, the regular versions do not.
   
     <P>
     G3D defines one of the following based on the platform:
     <B>G3D_WIN32, G3D_LINUX, G3D_OSX</B>
   <HR>

   
   <HR>

   <IMG SRC="G3D.gif">
  
*/
