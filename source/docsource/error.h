/** @page errorfaq Error FAQ


There are a few confusing error messages you can run into when compiling,
linking, and debugging programs that use G3D.  Here are the common fixes.

<P>  
<B><CODE>Error C2440: 'delete' : cannot convert from 'class G3D::ReferenceCountedPointer<class G3D::GFont>' to '' No user-defined-conversion operator available that can perform this conversion, or the operator cannot be called</CODE></B>
<P>
You called <CODE>delete</CODE> on a reference counted class.  You do not need to delete G3D::GFontRef, G3D::TextureRef or any other Ref class; just 
set them to NULL when you don't need them and the underlying memory will be recovered.


<P>
<B>error C2440: 'return' : cannot convert from 'class A *' to 'class B *'
Types pointed to are unrelated; conversion requires reinterpret_cast, C-style cast or function-style cast
d:\games\cpp\source\include\g3d\referencecount.h(194) : see reference to function template instantiation 'class B *__cdecl G3D::ReferenceCountedPointer<class B>::safecast(class A *)' being compiled</B>
<P>
You tried to assign a reference counted pointer from the wrong type 
(e.g. TextureRef a = GFontRef::create().
<P>


<P>
<B>Program "randomly" crashes AMD systems</B>
<P> See the 
<A HREF="http://support.microsoft.com/default.aspx?scid=http://support.microsoft.com:80/support/kb/articles/Q270/7/15.ASP&NoWebContent=1">Microsoft Knowledge Base article</A>.

<P>

<B>undefined
symbol SDL_main</B>
<BR>
The linking error: undefined
symbol SDL_main can result from a main() function that doesn't have argc
and argv arguments.  For example:
int main() {..}  is no good...  make sure you have  int main(int argc,
char **argv) {..}
<P>


  <B>Application Error: The application failed to initialize properly (0xc0000022). Click on OK to terminate the application.</B>
  <BR>
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

   <PRE><B>
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
</PRE></B>
   <BR>
   You are linking against the wrong standard library.  Navigate the MSVC menus to
   Project:Settings:C++:Code Generation and select 
   "Debug Multithreaded DLL" for your debug build and "Multithreaded DLL" for
   your release build.
   <P>

<P>
<B><CODE>array.h(233) : error C2512: : no appropriate default constructor available 
   <BR>array.h(195) : while compiling
   class-template member function 'void __thiscall G3D::Array<class
   X>::resize(int,bool)'</CODE></B>

<BR> You created a G3D::Array of a class that does not provide a default 
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

<!
/link /PDB:G3D-debug.pdb
>
 
*/