/**
  @file SDL_GL_Demo/glheaders.h

  Includes the headers needed for writing simple Win32 OpenGL apps.

  <H1>MSVC++ Configuration</H1>

  You must set your INCLUDE path to include:
    <PRE>

    SDL-1.2.5\include
    graphics3d-5.00\include
    </PRE>

  You must set your LIBRARY path to include:
    <PRE>
    SDL-1.2.5\lib
    graphics3d-5.00\lib
    </PRE>

  In MSVC++, you can reach these options from Tools::Options::Directories
  
  Notes:
  <UL>
  <LI>Library version numbers aren't strict; it is likely that this process
  will work for other versions.
  </UL>


  <H1>Per-project Configuration</H1>
 
   When creating a new project you need to set the project's
   <I>include</I> and <I>lib</I> paths as described above (unless you set the
   default for all projects), and you need to set up the static link libraries.
   These instructions superceed instructions that are provided with the individual
   libraries.

   <P>
   <OL>
    <LI>Go to Project::Settings, Link Tab, Category Input. 
    <LI>Replace the contents Object/Library modules text box with:
        <PRE>
          graphics3D.lib SDLmain.lib SDL.lib nvparse.lib opengl32.lib glu32.lib MSVCRT.LIB libcp.lib SDL.lib msvcrt.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib uuid.lib
        </PRE>
        and Check the Ignore Default Libraries box.
     <LI><I>if you use GLUT, you must add GLUT32.lib</I>

    <LI>Repeat the above for both Debug and Release configurations if you plan to use more than just the Debug configuration.

     <LI>Place the following files in your distribution directory.
   <PRE>
     SDL.dll
   </PRE>
   </OL>

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-01-01
  @edited  2002-01-25
 */

#ifndef GLHEADERS_H

// Includes G3D and GLG3D OpenGL support
#include <G3DAll.h>

// SDL for easy window management
#include "SDL.h"

/** For loading GL Extensions */
#define LOAD_GL_EXTENSION(name) \
    OutputDebugString("Loading " #name " extension\n"); \
    *((void**)&name) = wglGetProcAddress(#name);


void initGL();

#ifdef _WIN32
HDC getWindowHDC();
#endif

#endif
