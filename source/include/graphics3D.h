/**
 @file graphics3D.h

 This header includes all of the graphics3D libraries in
 appropriate namespaces.

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2001-08-25
 @edited  2003-11-13

 Copyright 2001-2003, Morgan McGuire.
 All rights reserved.
*/


#ifndef G3D_GRAPHICS3D_H
#define G3D_GRAPHICS3D_H

#define NOMINMAX
#ifdef min
    #undef min
#endif
#ifdef max
    #undef max
#endif

/**
 The version number of G3D in the form: MmmBB -> 
 version M.mm [beta BB]
 */
#define G3D_VER 60008

#include "G3D/platform.h"


#ifdef G3D_WIN32
    // Set up the linker on Windows
    #define ZLIB_DLL

    #pragma comment(linker, "/VERSION:6.00") 

    #pragma comment(lib, "zlib.lib")
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "winmm.lib")
    #pragma comment(lib, "imagehlp.lib")
    #pragma comment(lib, "version.lib")

    #ifdef _DEBUG
        // zlib and SDL were linked against the release MSVCRT; force
        // the debug version.
        #pragma comment(linker, "/nodefaultlib:msvcrt.lib")

        #pragma comment(lib, "G3D-debug.lib")
    #else
        #pragma comment(lib, "G3D.lib")
    #endif

#endif


#include "G3D/Array.h"
#include "G3D/Queue.h"
#include "G3D/format.h"
#include "G3D/Vector2.h"
#include "G3D/Vector3.h"
#include "G3D/Vector4.h"
#include "G3D/Color3.h"
#include "G3D/Color4.h"
#include "G3D/Matrix3.h"
#include "G3D/CoordinateFrame.h"
#include "G3D/PhysicsFrame.h"
#include "G3D/Plane.h"
#include "G3D/Line.h"
#include "G3D/Ray.h"
#include "G3D/Sphere.h"
#include "G3D/Box.h"
#include "G3D/Cone.h"
#include "G3D/Quat.h"
#include "G3D/stringutils.h"
#include "G3D/prompt.h"
#include "G3D/g3derror.h"
#include "G3D/Table.h"
#include "G3D/Set.h"
#include "G3D/BinaryInput.h"
#include "G3D/BinaryOutput.h"
#include "G3D/debug.h"
#include "G3D/G3DGameUnits.h"
#include "G3D/g3dmath.h"
#include "G3D/fileutils.h"
#include "G3D/ReferenceCount.h"
#include "G3D/CImage.h"
#include "G3D/CollisionDetection.h"
#include "G3D/Log.h"
#include "G3D/TextInput.h"
#include "G3D/NetworkDevice.h"
#include "G3D/System.h"
#include "G3D/LineSegment.h"
#include "G3D/Capsule.h"
#include "G3D/Triangle.h"
#include "G3D/Color3uint8.h"
#include "G3D/Color4uint8.h"
#include "G3D/Vector2int16.h"
#include "G3D/Vector3int16.h"
#include "G3D/ConvexPolyhedron.h"
#include "G3D/Discovery.h"
#include "G3D/MeshAlg.h"
#include "G3D/vectorMath.h"
#include "G3D/Rect2D.h"

#endif

