/**
 @file DXCaps.h

 @created 2006-04-06
 @edited  2006-04-06

 Copyright 2000-2006, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_DXCAPS_H
#define G3D_DXCAPS_H

#include "G3D/platform.h"
#include "G3D/g3dmath.h"


namespace G3D {

/** 
    Provides very basic DirectX detection and information support
*/
class DXCaps {

public:
    /**
        Returns 0 if not installed otherwise returns the major and minor number
        in the form (major * 100) + minor.  eg. 900 is 9.0 and 901 is 9.1
    */
    static uint32 getVersion();

    /**
        Returns the amount of video memory detected by Direct3D in bytes.
    */
    static uint64 getVideoMemorySize();
};

} // namespace G3D

#endif // G3D_DXCAPS_H