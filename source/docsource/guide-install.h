/** @page guideinstall Installing G3D

  \htmlonly
<TABLE BORDER=0 WIDTH=80%><TR><TD><I><FONT FACE="Arial">
<A HREF="license.html"><IMG SRC="backarrow.gif" BORDER=0 ALIGN=MIDDLE>
License</A></I></FONT></TD><TD ALIGN=RIGHT><FONT FACE="Arial"><I>
<A HREF="guidecompiling.html">
Compiling and Linking <IMG SRC="forwardarrow.gif" BORDER=0 ALIGN=MIDDLE></A></I></FONT></TD></TR></TABLE>
\endhtmlonly

  @section windows WindowsXP/2000

   <OL>
   <LI>
   Download the latest g3d-xxx.zip from
   http://g3d-cpp.sf.net. 
   Unzip all of the files in the distribution to your
   library directory (e.g. <CODE>c:\\libraries</CODE>).

   <LI>Download and install SDL <B>version 1.2.7</B> from
   <A HREF="http://www.libsdl.org">http://www.libsdl.org</A>

   <LI>Microsft Visual C++ 6.0 and Microsoft Visual C++ 7.0 (which comes as part of Microsoft Visual Studio .NET 2002 and 2003) 
	require different versions of the library, and a few different installation steps. Precompiled binaries for MSVC 6 are located
	in win32-lib and precompiled binaries for MSVC 7 are in win32-lib7. Figure out which one you're using, and point to the right
	G3D binaries, or you will have strange trouble, including linker errors and memory leaks. 
   </LI>

   <LI>
   If you're using MSVC 6:
   <OL>
   <LI>Install 
   <A HREF="http://download.microsoft.com/download/vstudio60ent/SP5/Wideband-Full/WIN98Me/EN-US/vs6sp5.exe">Service Pack 5</A>. 
   (Service pack 6 does not support the processor pack) 
   </LI>
   <LI>Install the 
   <A HREF="http://download.microsoft.com/download/vb60ent/Update/6/W9X2KXP/EN-US/vcpp5.exe">Processor Pack</A>.
   </LI>
   <LI>(<I>Optional</I>) Add the following lines to your Autoexp.dat file (usually in <CODE>C:\\Program Files\\Microsoft
   Visual Studio\\Common\\MSDev98\\Bin</CODE>): 

<PRE> ;; graphics3D
   %G3D::Quat=Quat(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;,&lt;w&gt;)
   %G3D::Vector4=Vector4(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;,&lt;w&gt;)
   %G3D::Vector3=Vector3(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;)
   %G3D::Vector2=Vector2(&lt;x&gt;,&lt;y&gt;)
   %G3D::Vector4int16=Vector4(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;,&lt;w&gt;)
   %G3D::Vector3int16=Vector3(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;)
   %G3D::Vector2int16=Vector2(&lt;x&gt;,&lt;y&gt;)
   %G3D::Color4=Color4(&lt;r&gt;,&lt;g&gt;,&lt;b&gt;,&lt;a&gt;)
   %G3D::Color3=Color3(&lt;r&gt;,&lt;g&gt;,&lt;b&gt;)
   %G3D::Color4uint8=Color4uint8(&lt;r&gt;,&lt;g&gt;,&lt;b&gt;,&lt;a&gt;)
   %G3D::Color3uint8=Color3uint8(&lt;r&gt;,&lt;g&gt;,&lt;b&gt;)
   %G3D::NetAddress=NetAddress(&lt;addr.sin_addr.S_un.S_un_b.s_b1,u&rt;.&lt;addr.sin_addr.S_un.S_un_b.s_b2,u&rt;.&lt;addr.sin_addr.S_un.S_un_b.s_b3,u&rt;.&lt;addr.sin_addr.S_un.S_un_b.s_b4,u&rt;)

   ;; Prevent stepping into certain functions
    [ExecutionControl]
    std::*=NoStepInto
   </PRE>

   <LI>
   In Tools:Options:Directories, add the g3d include directory 
   (e.g. c:\\libraries\\g3d-6_00\\include) to the
   include list.  Make sure it preceeds all other include directories.
   <LI>
   In Tools:Options:Directories, add the g3d lib directory 
   (e.g. c:\\libraries\\g3d-6_00\\win32-lib for Windows.  On Linux
   the directory is linux-lib and on OS/X it is osx-lib.) to the library
   list.
   <LI>

   </OL>
   </LI>

   <LI>If you're using VC7:
   Go to Tools:Options. Select the Projects heading, and within that heading, Visual C++
   Directories. In that panel, select "Show Directories For: Include files." Add the %G3D
   include directory, G3D_ROOT/include, and the SDL include directory, probably C:/SDL-1.2.7/include
   In the same panel, select "Show Directories For: Library Files."
   Add the %G3D library directory, G3D_ROOT/win32-7-lib.
   </LI>

   <B>For each project you create</B>, copy <CODE>SDL.dll</CODE>, <CODE>glut32.dll</CODE>, 
      and <CODE>zlib1.dll</CODE>
      to your program directory (or put them in Windows/System)
    </OL>
   </OL>

  <P>

  @section linux Linux
  <OL>
   <LI>
   Download the latest g3d-xxx.zip from
   http://g3d-cpp.sf.net. 
   Unzip all of the files in the distribution to your
   library directory (e.g. <CODE>~/lib/</CODE>).
   </LI>

   <LI>Make sure that SDL <B>version 1.2.7</B> is installed on your machine. 
   Try running <CODE>sdl-config --version</CODE>. If it returns "1.2.7" then you're all set. 
   (<CODE>sdl-config</CODE> is a handy tool that tells you what flags to pass to your compiler
   and linker to use SDL. If you run into SDL trouble, <CODE>sdl-config</CODE> is your friend.)
   If <CODE>sdl-config</CODE> can't be found, or returns something other than "1.2.7", you'll 
   need to install or update SDL. Download and install SDL from
   <A HREF="http://www.libsdl.org">http://www.libsdl.org</A>. 
   </LI>

   <LI>We recommend using <A HREF="http://ice.sf.net">iCompile</A>, 
	which automatically compiles and links every C++ file in the directory it's run from, 
	so you won't need to edit it to refer to each source file (as you would with a Makefile). 
	It needs the following environment variables, which you can set in your .cshrc file using 
	setenv. If you're using iCompile,
	you will need to set up some environment variables to point to SDL and %G3D. 
	Modify the paths as appropriate for your installation:

	<CODE>
	INCLUDE ~me/libraries/g3d/include:/usr/include/SDL <BR>
	LIBRARY ~me/libraries/g3d/linux-lib<BR>
	LD_LIBRARY_PATH $LIBRARY <BR>
	CC gcc-3.2 <BR>
	CXX g++-3.2 <BR>
	</CODE>

	You are also welcome to use a makefile; set compiler and linker flags to point to SDL and %G3D.   
	</LI>

	<LI>
	Confirm that you've installed %G3D correctly by installing and running one of the demos:<BR>
	<CODE>cd g3d/demos/GLG3D_Demo</CODE><BR>
	<CODE>cp your_iCompile_script iCompile</CODE><BR>
	<CODE>iCompile</CODE><BR>
	<CODE>distrib/GLG3D_Demo-debug</CODE><BR>
	This should run the GLG3D demo; you should see many airplanes of various colors flying around in a beautiful blue sky. 
	</LI>

  @section osx OS X

  See \link guidemacosx Setting Up a Project in Mac OS X\endlink for complete instructions on installing %G3D on the mac. 

  @section build Building G3D

  You don't have to build %G3D to use %G3D; the instructions above walk you through 
  setting up your environment to use the precompiled %G3D binaries. You may <EM>want</em>
  to build %G3D, in which case, we salute you! Reasons for wanting to build %G3D include...
  <UL>
  <LI>Stepping into library functions while debugging
  </LI>
  <LI>Using an unsupported compiler
  </LI>
  <LI>Customize the library code.
  </LI>
  </UL>

  To build %G3D, see the <A HREF="build-instructions.html">build instructions</A>.
  
   \section defines \#defines
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

     <B>G3D \#defines</B> one of the following based on the platform:
     <B>G3D_WIN32, G3D_LINUX, G3D_OSX</B>
   <HR>
  */