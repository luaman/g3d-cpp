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
  */