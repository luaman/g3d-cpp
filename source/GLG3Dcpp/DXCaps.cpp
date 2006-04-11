/**
 @file DXCaps.cpp

 @created 2006-04-06
 @edited  2006-04-06

 Copyright 2000-2006, Morgan McGuire.
 All rights reserved.
*/

#include "GLG3D/DXCaps.h"
#include "G3D/RegistryUtil.h"

// This file is only used on Windows
#ifdef G3D_WIN32

namespace G3D {


uint32 DXCaps::getVersion() {
    uint32 dxVersion = 0;

    if ( RegistryUtil::keyExists("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\DirectX") ) {
        std::string versionString;
        if ( RegistryUtil::readString("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\DirectX\\Version", versionString) ) {
            char major[3], minor[3];
            if ( versionString.size() >= 7 ) {
                major[0] = versionString[2];
                major[1] = versionString[3];
                major[2] = '\0';

                minor[0] = versionString[5];
                minor[1] = versionString[6];
                minor[2] = '\0';

                dxVersion = (atoi(major) * 100) + atoi(minor);
            }
        }
    }

    return dxVersion;
}


} // namespace G3D

#endif // G3D_WIN32
