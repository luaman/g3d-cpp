/**
 @file fileutils.cpp
 
 @author Morgan McGuire, graphics3d.com
 
 @author  2002-06-06
 @edited  2003-04-14
 */

#include "G3D/fileutils.h"
#include "G3D/BinaryInput.h"
#include "G3D/g3dmath.h"
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _MSC_VER
   // Needed for _getcwd
   #include <direct.h>
#else
   #include <unistd.h>
   #define _getcwd getcwd
#endif
#include <stdio.h>
#include "G3D/BinaryOutput.h"

extern "C" {
    #include "../IJG/jpeglib.h"
}


#ifdef _WIN32
    // For CopyFile
    #include <windows.h>
    //for _mkdir and _stat
    #include <direct.h>
#else
    #define _stat stat
#endif

namespace G3D {

std::string resolveFilename(const std::string& filename) {
    if (filename.size() >= 1) {
        if ((filename[0] == '/') || (filename[0] == '\\')) {
            // Already resolved
            return filename;
        } else {

            #ifdef _WIN32
                if ((filename.size() >= 2) && (filename[1] == ':')) {
                    // There is a drive spec on the front.
                    if ((filename.size() >= 3) && ((filename[2] == '\\') || (filename[2] == '/'))) {
                        // Already fully qualified
                        return filename;
                    } else {
                        // The drive spec is relative to the working directory on that drive.
                        debugAssertM(false, "Files of the form d:path are not supported (use a fully qualified name).");
                        return filename;
                    }
                }
            #endif
        }
    }

    char buffer[1024];

    // Prepend the working directory.
    _getcwd(buffer, 1024);

    return format("%s/%s", buffer, filename.c_str());
}


std::string readFileAsString(
    const std::string& filename) {

    int64 length = fileLength(filename);

    if (length == -1) {
        return "";
    }

    char* buffer = (char*)malloc(length + 1);
    debugAssert(buffer);
    FILE* f = fopen(filename.c_str(), "rb");
    debugAssert(f);
    int ret = fread(buffer, 1, length, f);
	debugAssert(ret == length);
    fclose(f);

    buffer[length] = '\0';
    std::string s = std::string(buffer);
    free(buffer);

    return s;
}


int64 fileLength(const std::string& filename) {
    struct _stat st;
    int result = _stat(filename.c_str(), &st);
    
    if (result == -1) {
        return -1;
    }

    return st.st_size;
}


FILE* createTempFile() {
    // Added to the G3D version of jpeglib
#ifdef _WIN32
    return _robustTmpfile();
#else
    return tmpfile();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////

void writeStringToFile(
    const std::string&          str,
    const std::string&          filename) {

    // TODO: don't use BinaryOutput
    BinaryOutput b = BinaryOutput(filename, G3D_LITTLE_ENDIAN);
    b.writeString(str);
    b.commit();
}

//////////////////////////////////////////////////////////////////////////////////////

/**
 Creates the directory (which may optionally end in a /)
 and any parents needed to reach it.
 */
void createDirectory(
    const std::string&  dir) {
    
    std::string d;

    // Add a trailing / if there isn't one.
    switch (dir[dir.size() - 1]) {
    case '/':
    case '\\':
        d = dir;
        break;

    default:
        d = dir + "/";
    }

    // If it already exists, do nothing
    if (fileExists(d.substr(0, d.size() - 1))) {
        return;
    }

    // Parse the name apart
    std::string root, base, ext;
    Array<std::string> path;

    std::string lead;
    parseFilename(d, root, path, base, ext);
    debugAssert(base == "");
    debugAssert(ext == "");

    // Begin with an extra period so "c:\" becomes "c:\.\" after
    // appending a path and "c:" becomes "c:.\", not root: "c:\"
    std::string p = root + ".";

    // Create any intermediate that doesn't exist
    for (int i = 0; i < path.size(); ++i) {
        p += "/" + path[i];
        if (! fileExists(p)) {
	    // Windows only requires one argument to mkdir,
	    // where as unix also requires the permissions.
	    #ifndef _WIN32
	 	 mkdir(p.c_str(),0777);
 	    #else
                 _mkdir(p.c_str());
	    #endif
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////

bool fileExists(
    const std::string& filename) {

    struct _stat st;
    return _stat(filename.c_str(), &st) != -1;
}

//////////////////////////////////////////////////////////////////////////////////////

void copyFile(
    const std::string&          source,
    const std::string&          dest) {

    #ifdef _WIN32
        CopyFile(source.c_str(), dest.c_str(), TRUE);
    #else
        // TODO: don't use BinaryInput and BinaryOutput
        // Read it all in, then dump it out
        BinaryInput  in  = BinaryInput(source, G3D_LITTLE_ENDIAN);
        BinaryOutput out = BinaryOutput(dest, G3D_LITTLE_ENDIAN);
        out.writeBytes(in.getCArray(), in.size());
        out.commit();
    #endif
}

//////////////////////////////////////////////////////////////////////////////////////

static bool isSlash(char c) {
    return (c == '\\') || (c == '/');
}


void parseFilename(
    const std::string&  filename,
    std::string&        root,
    Array<std::string>& path,
    std::string&        base,
    std::string&        ext) {

    std::string f = filename;

    root = "";
    path.clear();
    base = "";
    ext = "";

    if (f == "") {
        // Empty filename
        return;
    }

    // See if there is a root/drive spec.
    if ((f.size() >= 2) && (f[1] == ':')) {
        
        if ((f.size() > 2) && isSlash(f[2])) {
        
            // e.g.  c:\foo
            root = f.substr(0, 3);
            f = f.substr(3, f.size() - 3);
        
        } else {
        
            // e.g.  c:foo
            root = f.substr(2);
            f = f.substr(2, f.size() - 2);

        }

    } else if ((f.size() >= 2) & isSlash(f[0]) && isSlash(f[1])) {
        
        // e.g. //foo
        root = f.substr(0, 2);
        f = f.substr(2, f.size() - 2);

    } else if (isSlash(f[0])) {
        
        root = f.substr(0, 1);
        f = f.substr(1, f.size() - 1);

    }

    // Pull the extension off
    {
        // Find the period
        size_t i = f.rfind('.');

        // Make sure it is before a slash!
        size_t j = iMax(f.rfind('/'), f.rfind('\\'));
        if ((i != std::string::npos) && (i > j)) {
            ext = f.substr(i + 1, f.size() - i - 1);
            f = f.substr(0, i);
        }
    }

    // Pull the basename off
    {
        // Find the last slash
        size_t i = iMax(f.rfind('/'), f.rfind('\\'));
        
        if (i == std::string::npos) {
            
            // There is no slash; the basename is the whole thing
            base = f;
            f    = "";

        } else if ((i != std::string::npos) && (i < f.size() - 1)) {
            
            base = f.substr(i + 1, f.size() - i - 1);
            f    = f.substr(0, i);

        }
    }

    // Parse what remains into path.
    size_t prev, cur = 0;

    while (cur < f.size()) {
        prev = cur;
        
        // Allow either slash
        size_t i = f.find('/', prev + 1);
        size_t j = f.find('\\', prev + 1);
        if (i == std::string::npos) {
            i = f.size();
        }

        if (j == std::string::npos) {
            j = f.size();
        }

        cur = iMin(i, j);

        if (cur == std::string::npos) {
            cur = f.size();
        }

        path.append(f.substr(prev, cur - prev));
        ++cur;
    }
}

}

#ifndef _WIN32
  #undef _stat
#endif
