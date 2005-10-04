/**
 @file platform.h

 #defines for platform specific issues.

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2003-06-09
 @edited  2005-09-09
 */

#ifndef G3D_PLATFORM_H
#define G3D_PLATFORM_H

/**
 The version number of G3D in the form: MmmBB -> 
 version M.mm [beta BB]
 */
#define G3D_VER 60800


#ifdef _MSC_VER 
    #define G3D_WIN32 
#elif __MINGW32__
    #define G3D_WIN32 
    #define G3D_MINGW32 
#elif __linux__ 
    #define G3D_LINUX
#elif __APPLE__ 
    #define G3D_OSX
#else
    #error Unknown platform 
#endif


// Default to compiling with SSE, but if you want to compile 
// without installing SP5.0 and the Processor Pack on Windows, compile with NO_SSE
// defined (can be passed to the compiler command line with /D "NO_SSE")
#if !defined(NO_SSE)
   #define SSE
#endif

#ifdef G3D_WIN32
// Turn off warnings about deprecated C routines
#	pragma warning (disable : 4996)
#endif

// On g++, recognize cases where the -msse2 flag was not specified
#if defined(SSE) && defined(__GNUC__) && ! defined (__SSE__)
#   undef SSE
#endif

#if defined(__GNUC__)
#    if __STDC_VERSION__ < 199901
#        define restrict __restrict__
#    endif
#endif

// Verify that the supported compilers are being used and that this is a known
// processor.

#ifdef G3D_LINUX
#   ifndef __GNUC__
#       error G3d only supports the gcc compiler on Linux.
#   endif

#   ifndef __i386__
#       error G3D only supports x86 machines on Linux.
#   endif

#   ifndef __cdecl
#       define __cdecl __attribute__((cdecl))
#   endif

#   ifndef __stdcall
#       define __stdcall __attribute__((stdcall))
#   endif
#endif

#ifdef G3D_OSX
    #ifndef __GNUC__
        #error G3D only supports the gcc compiler on OS X.
    #endif

    #ifndef __POWERPC__
        #error G3D only supports PowerPC processors on OS X.
    #endif

    #ifndef __cdecl
        #define __cdecl __attribute__((cdecl))
    #endif
#endif


#ifdef G3D_WIN32
    // Old versions of MSVC (6.0 and previous) don't
    // support C99 for loop scoping rules.  This fixes them.
#   if (_MSC_VER <= 1200)
        // This trick will generate a warning; disable the warning
#       pragma warning (disable : 4127)
#       define for if (false) {} else for

#    endif


// Disable 'name too long for browse information' warning
#   pragma warning (disable : 4786)

#   define restrict


    // On MSVC, we need to link against the multithreaded DLL version of
    // the C++ runtime because that is what SDL and ZLIB are compiled
    // against.  This is not the default for MSVC, so we set the following
    // defines to force correct linking.  
    //
    // For documentation on compiler options, see:
    //  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccore/html/_core_.2f.md.2c_2f.ml.2c_2f.mt.2c_2f.ld.asp
    //  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccore98/HTML/_core_Compiler_Reference.asp
    //

    // DLL runtime
    #ifndef _DLL
	    #define _DLL
    #endif

    // Multithreaded runtime
    #ifndef _MT
	    #define _MT 1
    #endif

    // Ensure that we aren't forced into the static lib
    #ifdef _STATIC_CPPLIB
	    #undef _STATIC_CPPLIB
    #endif

    #ifdef _DEBUG
        #pragma comment (linker, "/NODEFAULTLIB:LIBCMTD.LIB")
        #pragma comment (linker, "/NODEFAULTLIB:LIBCPMTD.LIB")
        #pragma comment (linker, "/NODEFAULTLIB:LIBCPD.LIB")
        #pragma comment (linker, "/DEFAULTLIB:MSVCPRTD.LIB")
        #pragma comment(linker, "/NODEFAULTLIB:LIBCD.LIB")
        #pragma comment(linker, "/DEFAULTLIB:MSVCRTD.LIB")
    #else
        #pragma comment(linker, "/NODEFAULTLIB:LIBC.LIB")
        #pragma comment(linker, "/DEFAULTLIB:MSVCRT.LIB")
        #pragma comment (linker, "/NODEFAULTLIB:LIBCMT.LIB")
        #pragma comment (linker, "/NODEFAULTLIB:LIBCPMT.LIB")
        #pragma comment(linker, "/NODEFAULTLIB:LIBCP.LIB")
        #pragma comment (linker, "/DEFAULTLIB:MSVCPRT.LIB")
    #endif

    // Now set up external linking
    #define ZLIB_DLL

    #pragma comment(lib, "zdll.lib")
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "winmm.lib")
    #pragma comment(lib, "imagehlp.lib")
    #pragma comment(lib, "gdi32.lib")
    #pragma comment(lib, "user32.lib")
    #pragma comment(lib, "kernel32.lib")
    #pragma comment(lib, "version.lib")

    #ifdef _DEBUG
        // zlib and SDL were linked against the release MSVCRT; force
        // the debug version.
        #pragma comment(linker, "/NODEFAULTLIB:MSVCRT.LIB")

        // Don't link against G3D when building G3D itself.
        #ifndef G3D_BUILDING_LIBRARY_DLL
           #pragma comment(lib, "G3D-debug.lib")
        #endif
    #else
        // Don't link against G3D when building G3D itself.
        #ifndef G3D_BUILDING_LIBRARY_DLL
            #pragma comment(lib, "G3D.lib")
        #endif
    #endif

#endif

/** 
  @def STR(expression)

  Creates a string from the expression.  Frequently used with G3D::Shader
  to express shading programs inline.  

  <CODE>STR(this becomes a string)<PRE> evaluates the same as <CODE>"this becomes a string"</CODE>
 */
#define STR(x) #x

// Header guard
#endif
