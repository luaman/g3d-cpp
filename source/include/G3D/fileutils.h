/**
 @file fileutils.h
 
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @author  2002-06-06
 @edited  2003-04-10

 Copyright 2000-2003, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_FILEUTILS_H
#define G3D_FILEUTILS_H

#include <string>
#include <stdio.h>
#include "G3D/Array.h"
#include "G3D/g3dmath.h"

namespace G3D {
    
std::string readFileAsString(
    const std::string&          filename);

void writeStringToFile(
    const std::string&          str,
    const std::string&          filename);

/**
 Creates the directory (which may optionally end in a /)
 and any parents needed to reach it.
 */
void createDirectory(
    const std::string&          dir);

/**
 Fully qualifies a filename.
 */
std::string resolveFilename(const std::string& filename);

/** Returns the length of the file, -1 if it does not exist */
int64 fileLength(const std::string& filename);

/**
 Copies the file
 */
void copyFile(
    const std::string&          source,
    const std::string&          dest);

/** Returns a temporary file that is open for read/write access.  This
    tries harder than the ANSI tmpfile, so it may succeed when that fails. */
FILE* createTempFile();

/**
 Returns true if the given file (or directory) exists.
 Must not end in a trailing slash.
 */
bool fileExists(
    const std::string&          filename);

/**
  Parses a filename into four useful pieces.

  Examples:

  c:\a\b\d.e   
    root  = "c:\"
    path  = "a" "b"
    base  = "d"
    ext   = "e"
 
  /a/b/d.e
    root = "/"
    path  = "a" "b"
    base  = "d"
    ext   = "e"

  /a/b
    root  = "/"
    path  = "a"
    base  = "b"
    ext   = "e"

 */
void parseFilename(
    const std::string&  filename,
    std::string&        drive,    
    Array<std::string>& path,
    std::string&        base,
    std::string&        ext);

} // namespace

#endif

