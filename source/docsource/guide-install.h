/** @page guideinstall Installing G3D
   <P>
   @section windows WindowsXP/2000</B>

   <OL>
   <LI>
   Download the latest g3d-xxx.zip from
   http://g3d-cpp.sf.net. 
   Unzip all of the files in the distribution to your
   library directory (e.g. <CODE>c:\\libraries</CODE>).

   <LI>Download and install SDL <B>version 1.2.7</B> from
   <A HREF="http://www.libsdl.org">http://www.libsdl.org</A>

   <LI>Ensure that you are using MSVC++ 6.0 with <A HREF="http://download.microsoft.com/download/vstudio60ent/SP5/Wideband-Full/WIN98Me/EN-US/vs6sp5.exe">Service Pack 5</A> (Service pack 6 does not 
   support the processor pack) and
   the <A HREF="http://download.microsoft.com/download/vb60ent/Update/6/W9X2KXP/EN-US/vcpp5.exe">Processor Pack</A>.  If you are using
   VS .NET you must recompile the library because Microsoft changed the standard library (you'll get memory leaks if you don't!).

   <LI>(<I>Optional</I>) Add the following lines to your Autoexp.dat file (usually in <CODE>C:\\Program Files\\Microsoft
   Visual Studio\\Common\\MSDev98\\Bin</CODE>): 

<PRE> ;; graphics3D
   G3D::Quat=Quat(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;,&lt;w&gt;)
   G3D::Vector4=Vector4(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;,&lt;w&gt;)
   G3D::Vector3=Vector3(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;)
   G3D::Vector2=Vector2(&lt;x&gt;,&lt;y&gt;)
   G3D::Vector4int16=Vector4(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;,&lt;w&gt;)
   G3D::Vector3int16=Vector3(&lt;x&gt;,&lt;y&gt;,&lt;z&gt;)
   G3D::Vector2int16=Vector2(&lt;x&gt;,&lt;y&gt;)
   G3D::Color4=Color4(&lt;r&gt;,&lt;g&gt;,&lt;b&gt;,&lt;a&gt;)
   G3D::Color3=Color3(&lt;r&gt;,&lt;g&gt;,&lt;b&gt;)
   G3D::Color4uint8=Color4uint8(&lt;r&gt;,&lt;g&gt;,&lt;b&gt;,&lt;a&gt;)
   G3D::Color3uint8=Color3uint8(&lt;r&gt;,&lt;g&gt;,&lt;b&gt;)
   G3D::NetAddress=NetAddress(&lt;addr.sin_addr.S_un.S_un_b.s_b1,u&rt;.&lt;addr.sin_addr.S_un.S_un_b.s_b2,u&rt;.&lt;addr.sin_addr.S_un.S_un_b.s_b3,u&rt;.&lt;addr.sin_addr.S_un.S_un_b.s_b4,u&rt;)

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
   <B>For each project you create</B>, copy <CODE>SDL.dll</CODE>, <CODE>glut32.dll</CODE>, 
      and <CODE>zlib1.dll</CODE>
      to your program directory (or put them in Windows/System)
    </OL>
   </OL>

  <P>

  @section linux Linux

  @section osx OS X

  @section build Building G3D
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