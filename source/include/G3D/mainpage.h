// Copyright 2000-2004, Morgan McGuire.
// All rights reserved.
//
//   This library is recommended for use with the textbook:
//   <BR>Morgan McGuire, <I>3D Computer Graphics Techniques</I>, Prentice Hall
//

/**
 @mainpage
   <P>

  Check out the new \link guidewelcome Manual \endlink
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

     
    See the demo projects for a simple introduction to using SDL, OpenGL, 
    Graphics3D and MSVC++ to create and render 3D geometry.

   \section Introduction Introduction
   Online documentation and updates available at 
   <A HREF="http://www.graphics3d.com/cpp">http://www.graphics3d.com/cpp</A>.
   This C++ library is distributed through 
   <A HREF="http://www.graphics3d.com">graphics3d.com</A>. 

   <P>

   Graphics3D (G3D) is an open source library for game developers,
   students, and researchers.  It provides a
   set of low level routines and data structures for getting projects
   rolling.  Its scope is limited to those routines and structures
   that are so common they are needed in almost every graphics
   program.  G3D manages OpenGL
   
   The straightforward and clear coding style of the
   source will help you see how routines are implemented.  Extensive
   debugging support and debug assertions in the codebase itself will
   make your C++ programs reliable and easy to debug while extensive
   optimizations will make your program fast when building
   in release mode. 
   
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
