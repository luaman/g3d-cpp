/**
 @file format.h
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @author  2000-09-09
 @edited  2002-06-06

 Copyright 2000-2003, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_FORMAT_H
#define G3D_FORMAT_H

#include <string>
#include <stdio.h>
#include <cstdarg>
#ifndef _WIN32
    // Don't include varargs.h for some random
    // gcc reason
    //#include <varargs.h>
    #include <stdarg.h>
#endif

#ifndef __cdecl
    #define __cdecl __attribute__((cdecl))
#endif

namespace G3D {

/**
  Produces a string from arguments of the style of printf.  This avoids
  problems with buffer overflows when using sprintf and makes it easy
  to use the result functionally.  This function is fast when the resulting
  string is under 160 characters (not including terminator) and slower
  when the string is longer.
 */
std::string   __cdecl format(
    const char*                 fmt
    ...);

/**
  Like format, but can be called with the argument list from a ... function.
 */
std::string vformat(
    const char*                 fmt,
    va_list                     argPtr);


}; // namespace

#endif
