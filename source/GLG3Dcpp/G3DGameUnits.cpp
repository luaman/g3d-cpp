/**
 @file G3DGameUnits.cpp

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-10-05
 @edited  2003-02-15
 */

#include "../include/G3D/G3DGameUnits.h"

namespace G3D {

double toSeconds(int hour, int minute, double seconds, AMPM ap) {
    double t = ((hour % 12) * 60 + minute) * 60 + seconds;

    if (ap == PM) {
        t += 12 * 60 * 60;
    }

    return t;
}

}

