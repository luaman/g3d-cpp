/** @page guidewelcome Welcome to G3D

G3D is a commercial-grade 3D Engine available as Open Source (BSD License).  
It is used in games, tech demos, research papers, military simulators, and 
university courses.  

G3D provides all of the common support code needed to build any 3D application.  It 
makes low-level libraries like OpenGL and sockets easier to use without taking away 
any functionality or performance.  G3D is lower level than a Game Engine or Scene 
Graph system (e.g. Unreal Engine).  Your programs can be much more flexible, but the 
tradeoff is that you have to know more about 3D programming.  G3D is intended for 
users who are already familiar with C++ and DirectX or OpenGL.

@section chapters Table of Contents

<TABLE BORDER=0 WIDTH=75%>
 <TR><TD VALIGN=TOP WIDTH=33%><B>Chapter</B></TD><TD VALIGN=TOP><B>Topics</B></TD></TR>

 <TR><TD VALIGN=TOP>\link guidewelcome Welcome \endlink</TD><TD VALIGN=TOP>
  Documentation and Support,
  Platform and Functionality,
  Design Philosophy,
  Data Formats,
  Team,
  Working With Other Libraries,
  Release Schedule,
  Beta and Deprecated APIs,
  Notes for Expert Users</TD></TR>

  <TR><TD VALIGN=TOP>\link guideinstall Installing G3D \endlink</TD><TD VALIGN=TOP>
  WindowsXP/2000,
  Linux,
  OS X,
  Building the Library</TD></TR>


  <TR><TD VALIGN=TOP>\link guidecompiling Compiling and Linking with G3D \endlink</TD><TD></TD></TR>
  

  <TR><TD VALIGN=TOP>\link guideconcepts Concepts \endlink</TD><TD>
  Immediate Mode Graphics,
  Coordinate Systems,
  Reference Counting,
  STL</TD></TR>

  <TR><TD VALIGN=TOP>\link guideapp Application Framework \endlink</TD><TD VALIGN=TOP>
   Applets, Models, Selection, FPS Controls
  </TD></TR>

  <TR><TD VALIGN=TOP>\link guidedebugging Debugging \endlink</TD><TD VALIGN=TOP>
  </TD></TR>

  <TR><TD VALIGN=TOP>\link guidefiles Files \endlink</TD><TD VALIGN=TOP>
  File Management,
  Text Files,
  Binary Files,
  Network,
  Images, Models
  </TD></TR>

  <TR><TD VALIGN=TOP>\link guidephysics Physics and Ray Tracing \endlink</TD><TD VALIGN=TOP>
  Physics,
  Ray Tracing,
  Bounding Volumes,
  BSP Tree
  </TD></TR>

  <TR><TD VALIGN=TOP>\link guideeffects Effects \endlink</TD><TD VALIGN=TOP>
  Cartoon Rendering, Shadows, Reflections
  </TD></TR>

  
  <TR><TD VALIGN=TOP>\link guideshaders Shaders \endlink</TD><TD VALIGN=TOP>
  Supported Languages,
  Tools,
  Sample Shaders</TD></TR>

<TR><TD>\link license License (BSD/IJG) \endlink</TD><TD></TD></TR>
<TR><TD>\link changelog Change Log \endlink</TD><TD></TD></TR>
<TR><TD>\link errorfaq Error FAQ \endlink</TD><TD></TD></TR>
<TR><TD><A HREF="index.html">Index of Entry Points</A></TD><TD></TD></TR>
</TABLE>

@section documentation Documentation and Support
This guide gives a brief introduction to help you decide if G3D is right
for your project and introduce you to the major features.
However, it covers only a fraction of the functionality in G3D!  The individual 
entry points are extensively documented and can be accessed through the 
<A HREF="index.html">index</A>.

Post questions and feedback on the User Forum 
(http://sourceforge.net/forum/forum.php?forum_id=262426) and consider subscribing to 
it to receive all posts via e-mail.  Don't be afraid to post if you are a new or 
inexperienced user-- the User Forum is our primary support path.  You can also read
the Developer Forum (http://sourceforge.net/forum/forum.php?forum_id=262428) to keep 
abreast of changes coming in future releases and download Beta releases 
(http://sourceforge.net/project/showfiles.php?group_id=76879&package_id=100364) to 
provide feedback.

G3D provides several demos with source that show different ways of structuring your 3D program 
and how to use some of the most popular features.  The source code for the entire 
library is available (http://cvs.sourceforge.net/viewcvs.py/g3d-cpp/cpp/).

The official support team and the excellent user base respond to most user posts with
hours and patches for critical user-discovered bugs are given top priority.  Write 
to <A HREF="http://mailto:morgan3d@sf.net">Morgan McGuire</A> (morgan3d@sf.net) 
if your project has special commercial support needs.



@section platform Platform and Functionality

<IMG SRC="platforms.png" ALIGN=RIGHT>
G3D allows you to write code once that will compile and run on Windows, 
Linux, and OS X.  It supports for all OpenGL implementations, from software 
rasterization under Mesa to the latest GeForce and Radeon hardware accelerator 
cards. 

G3D is built on OpenGL because it is platform independent and allows access
to newer features than DirectX.  Many top games (including Doom 3) are written with 
OpenGL.  There are advantages to both APIs, but in the end they are equivalent except 
for the platform issue.  <B>You can implement any effect from any game in G3D</B>. Unlike
higher level engines it does not restrict access to low-level features-- every graphics
card function is available in OpenGL and exposed in G3D.

Releases contain precompiled libraries for MSVC 6.0, Linux gcc, and XCode.  
For other compilers you may need to build the library yourself.

@section philosophy Design Philosophy
Our philosophy is simple. The library should have the following properties:

  <UL>
    <LI> Easy to use!
    <LI> Only contains code needed for every 3D project.
    <LI> It is hard to write bugs...
    <LI> and easy to find them.
    <LI> You can use pieces of the library without the whole.
    <LI> Defaults are safe and fast.
    <LI> The license encourages both commercial and non-commercial use.
    <LI> It is easy to build the library and link against it.
    <LI> Provides access to popular data formats.
    <LI> The same source code runs on all operating systems and graphics cards.
    <LI> Manages resources for the user.
  </UL>


@section formats Data Formats
G3D provides direct load/save access to images in TGA, BMP, JPG, and PCX format.  
It has sample loaders for the IFS and MD2 3D mesh formats and comes with code 
(in tools/IFSBuilder) to help you load other formats like 3DS and OBJ.  G3D uses its own 
font format to avoid legal issues with TrueType.  There is no official support for audio,
however contrib/AudioDevice provides mappings for some audio libraries.

@section team Team
The library contains code, documentation, and demos from over 30 
\link contributors contributors \endlink.

Morgan McGuire has been the G3D project manager since 2000.  He is a 3D
software consultant with Masters degrees in electrical engineering and computer
science from MIT and Brown University.  He has been a senior architect at several 
companies in the graphics  industry and published award-winning research papers.

Gabe Taubman and Benjamin Landon provide OS X support.  Gabe has been a Mac user since 1998
and took first prize in the 2004 undergraduate ACM SIGGRAPH Student Research Competition 
with his G3D-based blast wave simulator.  He is currently a junior at Brown University.
Ben is a senior engineer at Sensable Technologies and has 20 years of experience in 
software development and scientific research.  

G3D is supported by donations from users (http://sourceforge.net/project/project_donations.php?group_id=76879), 
and by hardware and technical support from NVIDIA Corporation and ATI.


@section otherlibs Working With Other Libraries
G3D is compatible with most other libraries.  

Although G3D uses SDL 
(http://www.libsdl.org) for platform window management, you can use the G3D::GWindow API 
to replace SDL with a platform-specific or alternative library.  Users have 
contributed unsupported GWindow implementations for wxWindows, qtWindows, and the 
Win32 API that can be found in the contrib directory.

The contrib directory also contains AudioDevice implementations for FMOD 
(http://www.fmod.org) and SDL_Mixer (http://www.libsdl.com/sdl_mixer).

It is possible to use DirectX instead of OpenGL.  The easiest way is to link 
against G3D.lib only and use DirectX calls instead of the GLG3D equivalents.  A more 
full-featured solution is to replace the RenderDevice and Texture implementations with 
DirectX versions and rebuild the entire library.  We have not investigated the latter.  

To avoid memory leaks, ensure that all libraries you use have been compiled against 
the same version of the C++ standard library.  G3D expects to be compiled with run-time
type checking (RTTI), C++ exceptions, and a multithreaded, dynamically linked runtime.

G3D uses zlib (http://www.gzip.org/zlib/), which is distributed as part of G3D (as headers and a windows .lib and .dll).
GFont, BinaryInput, and BinaryOutput use zlib; you can also call zlib functions directly 
from your G3D program without additional headers or linking steps.

G3D::SDLWindow uses SDL and #including G3DAll.h G3D automatically 
causes your program to link SDL.  SDL is not included with the G3D installation and
must be downloaded separately. 

The G3D installation includes GLUT but does not use it by default.  You may #include glut.h if you like.

In addition to those mentioned above, you may be interested in using the following 
libraries to complement G3D:

<UL>
    <LI>Image Magick's Magick Wand (http://www.imagemagick.org/)
    <LI>Free Type2 (http://freetype.sourceforge.net/index2.html)
    <LI>STLport (http://www.stlport.org/download.html)
</UL>



  @section b1 Release Schedule
   G3D has a stable, backwards compatible API.  Most changes between versions add 
   functionality and fix bugs.  Point releases are backwards compatible
   to the last major release, except where critical bugs necessitate small API 
   changes.  Point releases occur about once every two months and are announced 
   on the <A HREF="http://sourceforge.net/forum/forum.php?forum_id=262426">User Forum</A>.
   
   Major releases may break source compatibility to fix critical bugs, match the 
   changing hardware standards, and streamline the API.  Incompatible changes are marked
   in the \link changelog Change Log \endlink and an upgrade path is recommended.
   Major releases occur about once a year and are announced well in advance.
   Migrating code across major releases generally takes a few hours of work for a 100,000 
   line program.  We try to design changes so they will trigger compiler errors for 
   older code instead of failing silently.

   Beta releases occur between point releases to allow users to access new functionality
   and provide feedback.  Beta releases are frequently Windows-only and are neither
   supported nor guaranteed to be stable.  Any APIs in a Beta release may change without
   notice.

  @section beta Beta and Deprecated APIs

   Even in official releases, some parts of G3D are marked as Beta in the documentation. 
   These are previews of new APIs that are still under construction but are stable enough
   to use.  They will likely change in small ways before becoming official APIs-- use at
   your own risk.

   Deprecated APIs have been replaced by some newer functionality but are supported and
   guaranteed to be present until the next major release, at which point they will be 
   removed. Avoid writing new code to deprecated APIs and consider migrating existing 
   code away from them.

@section experts Notes for Expert Users
G3D is designed so that you can use as much or as little as you want.  You are
welcome to rip out the source code for a single class like G3D::Texture and use 
it standalone in your project, link against the library and use our main G3D::RenderDevice
class, or let G3D::GApp provide the entire structure for your program.  

Most G3D programs need not access OpenGL directly.  However, you can always execute OpenGL
functions directly and mix them with RenderDevice calls <B>as long as you restore 
GL state afterwards.</B>  If you change the GL state and do not restore it, RenderDevice
will assume nothing has changed and may become corrupted.  Most classes, like Texture, expose
the relevant OpenGL handles so you can work with them directly as needed.

The routines are packaged as two static libraries, G3D and GLG3D, so that you can use 
the low-level vector math without bringing in any of the SDL/OpenGL code.

You may find it easier (particularly on Windows) to debug code if you build G3D on
your own machine so that you can step into the G3D routines and set breakpoints
and watch variables.  On Windows you should change the Debug Info level
to Program Database from Line Numbers Only.

*/

