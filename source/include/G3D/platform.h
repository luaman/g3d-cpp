/**
 @file platform.h

 #defines for platform specific issues.

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2003-06-09
 @edited  2003-08-13
 */

#ifndef G3D_PLATFORM_H
#define G3D_PLATFORM_H

#ifdef _MSC_VER 
    #define G3D_WIN32 
#elif __linux__ 
    #define G3D_LINUX
#elif __APPLE__ 
    #define G3D_OSX
#else
    #error Unknown platform 
#endif

#if defined(G3D_WIN32) || defined(G3D_LINUX)
    #define SSE
#endif


// Verify that the supported compilers are being used and that this is a known
// processor.

#ifdef G3D_LINUX
    #ifndef __GNUC__
        #error G3d only supports the gcc compiler on Linux.
    #endif

    #ifndef __i386__
        #error G3D only supports x86 machines on Linux.
    #endif

    #ifndef __cdecl
        #define __cdecl __attribute__((cdecl))
    #endif
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

#endif
