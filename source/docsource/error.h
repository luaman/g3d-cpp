/** @page errorfaq Error FAQ


There are a few confusing error messages you can run into when compiling,
linking, and debugging programs that use G3D.  Here are the common fixes.

<P><B>MSVCPRT.LIB(MSVCP60.dll) : error LNK2005: "public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > & __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::assign(class s
td::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &,unsigned int,unsigned int)" (?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ABV12@II@Z) already defined in xxx(yyy)</B>
<P>
G3D changes the linker settings for files, so you should generally #include <G3DAll.h> or #include<graphics3d.h> before any other includes and use it in every source (or header) file.  Also, include all of G3D and not just the specific class you are using. 

<P><B><CODE>Error C2440: 'delete' : cannot convert from 'class G3D::ReferenceCountedPointer<class G3D::GFont>' to '' No user-defined-conversion operator available that can perform this conversion, or the operator cannot be called</CODE></B>
<P>
You called <CODE>delete</CODE> on a reference counted class.  You do not need to delete G3D::GFontRef, G3D::TextureRef or any other Ref class; just 
set them to NULL when you don't need them and the underlying memory will be recovered.

<P><B>../include\G3D/ReferenceCount.h(159) : error C2664: 'setPointer' : cannot convert parameter 1 from 'class G3D::Shader *' to 'class SphereMap *'
        Types pointed to are unrelated; conversion requires reinterpret_cast, C-style cast or function-style cast
        D:\games\cpp\source\Debug\main.cpp(167) : see reference to function template instantiation '__thiscall G3D::ReferenceCountedPointer<class SphereMap>::G3D::ReferenceCountedPointer<class SphereMap>(const class G3D::ReferenceCountedPointer<clas
s G3D::Shader> &)' being compiled</B><P>

You tried to assign a reference counted pointer from the wrong type 
(e.g. TextureRef a = GFontRef::create().

<P><B>error C2440: 'return' : cannot convert from 'class A *' to 'class B *'
Types pointed to are unrelated; conversion requires reinterpret_cast, C-style cast or function-style cast
d:\games\cpp\source\include\g3d\referencecount.h(194) : see reference to function template instantiation 'class B *__cdecl G3D::ReferenceCountedPointer<class B>::safecast(class A *)' being compiled</B>
<P>
You tried to assign a reference counted pointer from the wrong type 
(e.g. TextureRef a = GFontRef::create().

<P><B>Program "randomly" crashes AMD systems</B><P>
See the 
<A HREF="http://support.microsoft.com/default.aspx?scid=http://support.microsoft.com:80/support/kb/articles/Q270/7/15.ASP&NoWebContent=1">Microsoft Knowledge Base article</A>.

<P><B>undefined symbol SDL_main</B><P>
The linking error: <CODE>undefined symbol SDL_main </CODE> can result from a main() function that doesn't have argc
and argv arguments.  For example:
<CODE> int main() {..}</CODE>

is no good...  make sure you have

<CODE> int main(int argc, char **argv) {..} </CODE>

<P><B>Application Error: The application failed to initialize properly (0xc0000022). Click on OK to terminate the application.</B><P>

   At runtime, your program can't load the SDL.dll, zlib.dll, or glut32.dll dynamic libraries.

   This is probably occurring because the current working directory is invalid or because
   the files and directory exist but cannot be accessed.  This frequently occurs when a Windows
   program is running from a Linux file system via Samba.  Make sure you have set the e<B>X</B>ecute

   bit on the DLL's from Linux.
   <P>

<B><CODE>../include/G3D/g3dmath.h:27: limits: No such file or directory</CODE></B>

<BR>You are using an old version of GCC on Linux. Add these lines to your .cshrc:<br>
<PRE>
   setenv CC gcc-3.2
   setenv CXX g++-3.2
</PRE>

<P><B><CODE>
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "public: __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::~basic_string<char,struct std::char_traits<char>,class std::allocator<char> >(void)" (??1?$basic_strin
g@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAE@XZ) already defined in main.obj
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "public: __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >(char const *,class basic_
string<char,struct std::char_traits<char>,class std::allocator<char> >::allocator<char> const &)" (??0?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAE@PBDABV?$allocator@D@1@@Z) already defined in main.obj
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "public: char const * __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::c_str(void)const " (?c_str@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@Q
BEPBDXZ) already defined in main.obj
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > & __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::operator=(cl
ass std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &)" (??4?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV01@ABV01@@Z) already defined in main.obj
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "public: __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >(class basic_string<char,s
truct std::char_traits<char>,class std::allocator<char> >::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &)" (??0?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAE@ABV01@@Z) already defined in main.
obj
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "public: static double __cdecl std::numeric_limits<double>::infinity(void)" (?infinity@?$numeric_limits@N@std@@SANXZ) already defined in main.obj
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "public: static double __cdecl std::numeric_limits<double>::quiet_NaN(void)" (?quiet_NaN@?$numeric_limits@N@std@@SANXZ) already defined in main.obj
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "public: unsigned int __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::size(void)const " (?size@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QBE
IXZ) already defined in main.obj
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > & __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::operator+=(c
lass std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &)" (??Y?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV01@ABV01@@Z) already defined in main.obj
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "bool __cdecl std::operator==(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &,char const *)" (??8std@@YA_NABV?$basic_string@DU?$char_traits@D@std@@V?$allocat
or@D@2@@0@PBD@Z) already defined in main.obj
msvcprtd.lib(MSVCP60D.dll) : error LNK2005: "public: class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > & __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::operator+=(c
har const *)" (??Y?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV01@PBD@Z) already defined in main.obj
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _fflush already defined in LIBCD.lib(fflush.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _fclose already defined in LIBCD.lib(fclose.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _free already defined in LIBCD.lib(dbgheap.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _realloc already defined in LIBCD.lib(dbgheap.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: __vsnprintf already defined in LIBCD.lib(vsnprint.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _exit already defined in LIBCD.lib(crt0dat.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _malloc already defined in LIBCD.lib(dbgheap.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: __CrtIsValidHeapPointer already defined in LIBCD.lib(dbgheap.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: __CrtIsValidPointer already defined in LIBCD.lib(dbgheap.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _sprintf already defined in LIBCD.lib(sprintf.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _toupper already defined in LIBCD.lib(toupper.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _tolower already defined in LIBCD.lib(tolower.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _calloc already defined in LIBCD.lib(dbgheap.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: _strncpy already defined in LIBCD.lib(strncpy.obj)
MSVCRTD.lib(MSVCRTD.dll) : error LNK2005: __isctype already defined in LIBCD.lib(isctype.obj)
LINK : warning LNK4098: defaultlib "MSVCRTD" conflicts with use of other libs; use /NODEFAULTLIB:library
Debug/GLG3D_Demo.exe : fatal error LNK1169: one or more multiply defined symbols found
</CODE></B>

   <P>
   You are linking against the wrong standard library.  Navigate the MSVC menus to
   Project:Settings:C++:Code Generation and select 
   "Debug Multithreaded DLL" for your debug build and "Multithreaded DLL" for
   your release build.
   <P>

<P>
<B><CODE>array.h(233) : error C2512: : no appropriate default constructor available 
   array.h(195) : while compiling class-template member function 'void __thiscall G3D::Array<class X>::resize(int,bool)'</CODE></B>

<P> You created a G3D::Array of a class that does not provide a default 
constructor (constructor with no arguments).  Define a default constructor 
for your class-- G3D::Array needs it to initialize new elements when you 
invoke G3D::Array::resize().

<P><B><CODE>sdlmain.lib(SDL_main.obj) : error LNK2005: _main already defined in main.obj</CODE></B> 
<BR>You need to <CODE>\#include &lt;SDL.h&gt;</CODE> in your main.cpp file.

<P>
<B><CODE>error LNK2001: unresolved external symbol _glActiveTextureARB</CODE></B>
<BR>Some other (e.g. MSVC .NET) program's include directory is listed before the 
G3D directory and contains conflicting OpenGL drivers.  Reorder the include directories
under Tools/Options/Directories

<P>

<B><CODE>fatal error C1083: Cannot open include file: 'G3DAll.h': No such file or directory</CODE></B>
<BR> You need to add the g3d/include directory to your include path.

<B>LINK : fatal error LNK1181: cannot open input file
   "glg3d.lib"</B> <BR> You need to add the graphics3d/lib directory
   to your library path (instructions above) 

<P><B>"warning LNK4229: invalid directive" in MSVC build</B><P>
When linking against G3D's libraries the following linker errors are produced
<CODE>G3D-debug.lib(TextInput.obj) : warning LNK4229: invalid directive '/alternatename:__imp_??$?9DU?$char_traits@D@std@@V?$allocator@D@1@@std@@YA_NABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@0@PBD@Z=__imp_??9std@@YA_NABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@0@PBD@Z' encountered; ignored 
G3D-debug.lib(TextInput.obj) : warning LNK4229: invalid directive '/alternatename:__imp_??$?8DU?$char_traits@D@std@@V?$allocator@D@1@@std@@YA_NABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@0@PBD@Z=__imp_??8std@@YA_NABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@0@PBD@Z' encountered; ignored 
G3D-debug.lib(TextInput.obj) : warning LNK4229: invalid directive '/alternatename:__imp_??$?8DU?$char_traits@D@std@@V?$allocator@D@1@@std@@YA_NABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@0@0@Z=__imp_??8std@@YA_NABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@0@0@Z' encountered; ignored 
</CODE>
etc...

Odds are you're linking against the wrong version of the libraries. Current G3D distributions include two versions of windows libraries. For MSVC version 6, the libraries reside in win32-lib, and  for version 7.0 (MSVC dot net) win32-lib7. Note there are TWO MSVC "dot net" versions, 7.0 (2002) and 7.1 (2003). They are incompatible with each other. If you have the latest 7.1 version, or any more recent version (such as the free Visual C++ 2005 Express Beta) you will need to build the libraries on your own.

<P><B>Building with Dev-C++</B><P>
Dev-C++ is currently and unsupported build environment for G3D. That is, Windows MSVC++ 6, Windows Visual Studio .NET (MSVC++ 7.0), Linux x86 gcc 3.3, and OS X Xcode are the only supported build environments. Though there is no official effort to get it to work, there has been community interest in supporting Dev-C++.

Committed developers can feel free to submit their progress to the G3D forums to contribute to the cause, but there are some considerable hurdles. Specifically, if you define an MSVC build, you will get CRT debug routines that don't exist in Dev C++. Your best bet might be to define a proper MSVC version in the build. Then, create a header which fills in missing data and defines which this build requires. Examples are CRT function calls and perhaps some other library calls. 

<P><B>Building with Visual C++ 2005 Express Beta</B><P>
To build G3D with Microsoft's free compiler, you will need to link against Windows platform libraries not included with the software. Specifically, user32.lib and gdi32.lib are included in the Windows Core SDK.

<P><B>Linux build versions</B><P>
G3D is only formally support Linux build environments with Linux x86 gcc 3.3, and Automake version 1.7. Note these are not the latest version of the software, so you may need to downgrade to get things to work.

*/