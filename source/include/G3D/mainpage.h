// Copyright 2000-2003, Morgan McGuire.
// All rights reserved.
//
//   This library is recommended for use with the textbook:
//   <BR>Morgan McGuire, <I>3D Computer Graphics Techniques</I>, Prentice Hall
//

/**
 @mainpage
   <P>
    <CENTER>\ref Overview  |   \ref Introduction  |  \ref Installation  |
    \ref defines "\#defines"  |  \ref License  |  \ref Contributors   |
    \ref Change "Change Log"</CENTER>

    \section Overview Overview
     This library contains the support code I need for every 3D
     project in C++.  It is designed to be easy to use, make it hard
     to write bugs, easy to find them, to compile to fast code, and to
     provide a single solution to graphics needs under many different
     compilers, operating systems, and processor architectures.

     <P>
     Use <CODE>\#include "graphics3D.h"</CODE> in your project and link
     against <CODE>graphics3d.lib</CODE>
     to gain access to the following members of the <CODE>G3D</CODE> namespace.
     
     <UL>
     <LI>Geometry
        <BR><BLOCKQUOTE>
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
		G3D::PhysicsFrame,
		G3D::Line,
		G3D::LineSegment,
		G3D::Matrix3,
        G3D::Quat,
		G3D::Plane,
		G3D::Ray,
        G3D::Sphere,
		G3D::Triangle,
		G3D::Vector2,
        G3D::Vector2int16,
		G3D::Vector3,
		G3D::Vector3int16,
		G3D::Vector4,
        G3D::GameTime,
        G3D::RealTime,
        G3D::toSeconds 
		</BLOCKQUOTE>
     
     <LI>Data structures
        <BR><BLOCKQUOTE>
		G3D::Array,
		G3D::Queue,
 	    G3D::Set,
		G3D::Table
		</BLOCKQUOTE>

     <LI>Image formats
        <BR><BLOCKQUOTE>
		G3D::CImage,
		G3D::CImage::Error,		
        G3D::flipRGBVertical,
  	    G3D::RGBtoARGB,
		G3D::RGBtoBGR,
		G3D::RGBtoBGRA,
        G3D::RGBtoRGBA,
		G3D::RGBxRGBtoRGBA
		</BLOCKQUOTE>

     <LI>Integer math
       <BR><BLOCKQUOTE>
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
		</BLOCKQUOTE>
     
     <LI>Floating point math
       <BR><BLOCKQUOTE>
	     G3D::abs,
		 G3D::aCos,
		 G3D::aSin,
		 G3D::aTan,
		 G3D::aTan2,
         G3D::clamp,
         G3D::sign,
		 G3D::rsq,
		 G3D::unitRandom,
		 G3D::random,
         G3D::symmetricRandom,
         G3D::min,
		 G3D::max,
		 G3D::square,
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
		 G3D::PI, 
         G3D::HALF_PI,
		 G3D::TWO_PI,
		 G3D::linearSpline,
		 G3D::float32,
         G3D::float64
		 </BLOCKQUOTE>

     <LI>Robust floating point math
       <BR><BLOCKQUOTE>G3D::fuzzyEq, G3D::fuzzyNe, G3D::fuzzyGt,
       G3D::fuzzyGe, G3D::fuzzyLt, G3D::fuzzyLe, G3D::fuzzyEpsilon</BLOCKQUOTE>
     
     <LI>String utilities
        <BR><BLOCKQUOTE>G3D::beginsWith, G3D::endsWith, G3D::stringSplit,
        G3D::stringJoin, G3D::format, G3D::vformat, G3D::wordWrap, 
        G3D::stringCompare, G3D::stringPtrCompare, G3D::toUpper, G3D::toLower,
        G3D::NEWLINE, G3D::trimWhitespace</BLOCKQUOTE>

     <LI>Debugging
        <BR><B>G3D::debugAssert</B>, G3D::isValidHeapPointer,  
        G3D::isValidPointer,
        <B>G3D::debugAssertM</B>, G3D::debugPrintf, G3D::error, 
        G3D::Log, G3D::alwaysAssertM</BLOCKQUOTE>

     <LI>I/O
        <BR><BLOCKQUOTE>
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
        G3D::parseFilename, 
        G3D::prompt, 
        G3D::readFileAsString,
        G3D::System,
        G3D::resolveFilename,
        G3D::TextInput,
        G3D::TextInput::Token,
        G3D::TextInput::WrongTokenType,
        G3D::writeStringToFile
        </BLOCKQUOTE>

    <LI>Networking
    <BR><BLOCKQUOTE>
        G3D::DiscoveryAdvertisement,
        G3D::DiscoveryClient,
        G3D::DiscoveryServer,
        G3D::DiscoverySettings,
        G3D::LightweightConduit,
        G3D::NetAddress,
        G3D::NetListener, 
        G3D::NetworkDevice, 
        G3D::ReliableConduit
        </BLOCKQUOTE>
     </UL>

     Link against glg3d.lib and \#include "glg3d.h" to gain access to:
     <UL>
      <LI>OpenGL Abstraction
        <BR><BLOCKQUOTE>
        G3D::Milestone,
        G3D::PixelProgram,
        G3D::reflectionMatrix,
        G3D::RenderDevice, 
        G3D::tesselateComplexPolygon,
        G3D::Texture, 
        G3D::TextureFormat,
        G3D::VAR,
        G3D::VARArea, 
        G3D::VertexProgram
        </BLOCKQUOTE>

      <LI>OpenGL 
        <BR><BLOCKQUOTE>
        G3D::glLoadMatrix, 
        G3D::glLoadInvMatrix, 
        G3D::glMultInvMatrix, 
        G3D::glMultMatrix, 
        G3D::glVertex, 
        G3D::glColor,
        G3D::glNormal, 
        G3D::glTexCoord, 
        G3D::glMultiTexCoord,
        G3D::sizeOfGLFormat, 
        G3D::glFormatOf, 
        DECLARE_GLFORMATOF,
        G3D::glGetProcAddress
        </BLOCKQUOTE>

      <LI>GL Debugging
        <BR><BLOCKQUOTE>
        G3D::getOpenGLState, 
        G3D::glGetInteger, 
        G3D::glGetBoolean,
        G3D::glGetDouble, 
        G3D::glGetFloat,
        G3D::GLenumToString
        </BLOCKQUOTE>

      <LI>Game/Demo Infrastructure</LI>
        <BR><BLOCKQUOTE>
        G3D::CFont,
        G3D::CFontRef,
        G3D::Sky,
        G3D::Camera, 
        G3D::LightingParameters,
        G3D::MD2Model,
        G3D::realWorldLocalTime, 
        G3D::UserInput, 
        G3D::ManualCameraController
        </BLOCKQUOTE>
     </UL>

    See the Demo project for a simple introduction to using SDL, OpenGL, 
    Graphics3D and MSVC to create and render 3D geometry.

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

   \section Installation Installation
   At the end of this section is a short FAQ regarding compiler error messages.

   <P>
   <B>Windows</B>

   <OL>
   <LI>
   Download the latest g3d-xxx.zip from
   http://g3d-cpp.sourceforge.net. 
   Unzip all of the files in the distribution to your
   library directory (e.g. <CODE>c:\\libraries</CODE>).

   <LI>Download and install the latest version of SDL from
   <A HREF="http://www.libsdl.org">http://www.libsdl.org</A>.

   <LI>Ensure that you are using MSVC++ 6.0 with Service Pack 5 and
   the Processor Pack.

  <LI> Autoexp.dat file (usually in <CODE>C:\\Program Files\\Microsoft
   Visual Studio\\Common\\MSDev98\\Bin</CODE>): 

<PRE> ;; graphics3D
   G3D::Quat=Quat(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;,&lt;w&gt;)
   G3D::Vector4=Vector4(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;,&lt;w&gt;)
   G3D::Vector3=Vector3(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;)
   G3D::Vector2=Vector2(&lt;x&gt;,&lt;y&gt;)
   G3D::Color4=Color4(&lt;r&gt;,&lt;g&gt;,&lt;b&gt;,&lt;a&gt;)
   G3D::Color3=Color3(&lt;r&gt;,&lt;g&gt;,&lt;b&gt;)
   G3D::NetAddress=NetAddress(&lt;addr.sin_addr.S_un.S_un_b.s_b1,u&rt;.&lt;addr.sin_addr.S_un.S_un_b.s_b2,u&rt;.&lt;addr.sin_addr.S_un.S_un_b.s_b3,u&rt;.&lt;addr.sin_addr.S_un.S_un_b.s_b4,u&rt;)


    ;; Prevent stepping into certain functions
    [ExecutionControl]
    std::*=NoStepInto
    G3D::Array&lt;*&gt;=NoStepInto
   </PRE>

   <LI>
   In Tools:Options:Directories, add the g3d include directory 
   (e.g. c:\\libraries\\g3d-6_00\\include) to the
   include list.  Make sure it preceeds all other include directories.
   <LI>
   In Tools:Options:Directories, add the g3d lib directory 
   (e.g. c:\\libraries\\g3d-6_00\\lib) to the library
   list.
   <LI>
   <B>For each project you create:
    <OL>
      <LI> Copy SDL.dll, glut32.dll, and zlib.dll to your program directory.
   
      <LI> Go to Projects:Settings:C++:Code Generation and select
      "Multithreaded DLL" for use run-time library.  Use
      "Debug Multithreaded DLL" in your debug build.
    </OL>
   </OL>

  <B>Linux</B> The current Linux build requires you to download
  g3d-src-6_00.zip and build the library yourself.  See readme.html in
  that distribution for build instructions.

  <P>

  Note that G3D uses zlib, which is distributed as part of the library
  (as headers and a windows binary).  If you need zlib for your own platform 
  or wish to build it yourself, go to http://www.gzip.org/zlib/.

  <H3>Compiler Error FAQ</H3>
   There are a few misleading error messages you can run into.  Here are the 
   common fixes.

   <P>

   <B>../include/G3D/g3dmath.h:27: limits: No such file or directory</B>
   <BR>You are using an old version of GCC on Linux. Add these lines to your .cshrc:<br>
   <ul>
   <li> setenv CC gcc-3.2
   <li> setenv CXX g++-3.2
   </ul>

   <P>
   <B>array.h(233) : error C2512: 'X' : no appropriate default
   constructor available <BR>array.h(195) : while compiling
   class-template member function 'void __thiscall G3D::Array<class
   X>::resize(int,bool)'</B> <BR> You created a G3D::Array of a class
   that does not provide a default constructor (constructor with no
   arguments).  Define a default constructor for your class--
   G3D::Array needs it to initialize new elements when you invoke
   G3D::Array::resize().

    <P> <B>sdlmain.lib(SDL_main.obj) : error LNK2005: _main already
   defined in main.obj</B> <BR>You need to \#include <SDL.h> in your
   main.cpp file.

   <P>
   <B>DemoSettings.obj : error LNK2001: unresolved external symbol _glActiveTextureARB</B>
   <BR>Some other (e.g. MSVC .NET) program's include directory is listed before the G3D directory
   and contains conflicting OpenGL drivers.
   <P>

   <B>main.cpp(9) : fatal error C1083: Cannot open include file:
   'G3DAll.h': No such file or directory</B> <BR> You need to add the
   graphics3d/include directory to your include path (instructions
   above)

   <B>LINK : fatal error LNK1181: cannot open input file
   "glg3d.lib"</B> <BR> You need to add the graphics3d/lib directory
   to your library path (instructions above)

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

   Standard (public domain) 2D and 3D data are provided with this
   library.  These may be useful as test data or for comparison with
   previously published results.
   
     <PRE>
       data           <I>Data root</I>
          image       <I>Test images</I>
          height      <I>height maps</I>
          ifs         <I>3D models (see IFS_Demo for loading code)</I>
          sky         <I>Images for use with G3D::Sky</I>
          font        <I>Fonts for use with G3D::Font</I>
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
   \section License License
   <A NAME=\#License></A>
   <H3>Intent of License</H3>
   (This section is informal and not legally binding.)

   <BR> This library is free code-- you can use it without charge and
   it is minimally legally encumbered.

   <P> I intend the license (below) to protect me and the other
   contributors from liability and allow you to use the source however
   you want.  You can make your own closed or open-source programs,
   sell them, give them away, whatever.

   <P>

   You have an obligation to say "this software is based in part on
   the work of the Independent JPEG Group" in your documentation or
   application help if you use the CImage class because it is based on
   the IJG library.  The OpenGL headers and ZLib headers included may
   be freely distributed provided their copyright notices remain
   intact.
   
   <P>

   All of the data resources have either entered the public domain and have
   been in several published papers or are data that I have explicitly 
   received permission to distribute with G3D.  The G3D fonts are actually font
   images, not TrueType font descriptions and may be freely
   distributed.  As a rule of thumb, you can freely use anything you find
   in the data directory, even in a commercial product.

   <P> 

   You are required by the BSD license to acknowledge G3D in your 
   documentation.  This can be as minimal as a note buried in the
   fine print at the end of a manual or a text file accompanying
   your program.  I appreciate it if you acknowledged the library
   more publicly but you aren't required to.

   <P>
   
   Likewise, you are encouraged but not required to submit patches to
   improve the library for the benefit of all.  E-mail with bugs,
   patches, and questions.  <P>

   -Morgan McGuire
   &lt;<I><A HREF="mailto:matrix@graphics3d.com">matrix@graphics3d.com</A></I>&gt;
    
   <HR>
   <H2>License</H2>

   <I>G3D is licensed under the <A HREF="http://www.opensource.org/licenses/bsd-license.php">BSD license</A>, 
   with portions controlled by the <A HREF="IJG-README.TXT">IJG license</A></I>

  <CODE>
   <IMG SRC="http://opensource.org/trademarks/osi-certified/web/osi-certified-120x100.gif">
   <DT>Copyright &copy; 2000-2003, Morgan McGuire
   <DT>All rights reserved.
   <P>
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   <P>
   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
   <P>
   Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
   <P>
   Neither the name of Morgan McGuire, Brown University, nor the names of
   its contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.
   <P>
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
   THE POSSIBILITY OF SUCH DAMAGE.

   <P>
      You agree to be bound by the terms of the Independent JPEG
      Group license for the portions of this library that are based
      on the work of the Independent JPEG Group, <B>if you use those
      portions</B>.  Note: if you do not use the CImage class,
      this clause does not apply to you because the linker will
      strip that code from your project.  The <A
      HREF="IJG-README.TXT">IJG-README.TXT</A> file contains the
      Independent JPEG Group license.

   <P> You agree to be bound by the terms of the Free Software
        License Agreement of Magic Software which is available at <A
        HREF="http://www.magic-software.com/License/free.pdf">http://www.magic-software.com/License/free.pdf</A>.

   </OL>
   <HR>

   \section Contributors Contributors

   This library contains code and resources contributed by the
   following people, or based open code and articles by them:

   <BLOCKQUOTE>
   <BR>Seth Block
   <BR>Nick Capens
   <BR>John Chisholm
   <BR>Jauhn Dabz
   <BR>Dave Eberly
   <BR>Kevin Egan
   <BR>Cass Everitt
   <BR>Jakob Fischer
   <BR>Harishabd Khalsa
   <BR>Nicolai Haehnle
   <BR>Michael Herf
   <BR>Peter Hunt
   <BR>Robert Hunter
   <BR>Ed Johnson
   <BR>Benjamin Jurke
   <BR>Chris Kern
   <BR>James O'Sullivan
   <BR>Aaron Orenstein
   <BR>Jordan Parker
   <BR>Independent JPEG Group
   <BR>Darius Jazayeri
   <BR>Ben Landon
   <BR>Thomas G. Lane
   <BR>Ray Larabie
   <BR>Jukka Liimatta
   <BR>Max McGuire
   <BR>Morgan McGuire
   <BR>Justin Miller
   <BR>Kurt Miller
   <BR>Nate Miller
   <BR>Edward Resnick
   <BR>Nate Robbins
   <BR>Sebastian Schuberth
   <BR>SGI
   <BR>Matthew Welch
   <BR>Simon Winkelbach
   <BR>Laura Wollstadt
   </BLOCKQUOTE>
   <HR>

   \section Change Change Log
   <P>

   Major version numbers introduce API changes that are not backwards
   compatible.  Minor versions are backwards compatible to the
   previous major release.

   Changes in 6.00

   <P>
   <UL>
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
     <LI> FIX: Windows with an aspect ratio of less than 1 now allowed.
     <LI> VARArea::reset() now waits for rendering calls using its vertex
          arrays to complete before wiping the memory.
     <LI> G3D::filenameBaseExt, G3D::filenameExt
     <LI> VARArea::finish()
     <LI> Milestone
     <LI> RenderDevice now loads glFlushVertexArrayRangeNV
     <LI> Vector2int16
     <LI> RenderDevice::freeVARSize()
     <LI> Array now allocates 16-byte aligned pointers.
     <LI> Decreased the default camera movement rate by 50% for better resolution.
     <LI> RenderDevice enables GL_NORMALIZE by default
     <LI> Improved the performance of Array::append/Array::push/Array::next
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
     <LI> G3D::reflectionMatrix
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
   <IMG SRC="G3D.gif">
  
*/
