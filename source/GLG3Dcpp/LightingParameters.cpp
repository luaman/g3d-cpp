/**
 @file LightingParameters.cpp

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-10-05
 @edited  2003-04-11
 */

#include "GLG3D/LightingParameters.h"

namespace G3D {

static const double sunRiseAndSetTime  = HOUR;

LightingParameters::LightingParameters() {
    setTime(0);
}


LightingParameters::LightingParameters(
    const double                    _time) {
    setTime(_time);
}


void LightingParameters::setTime(const GameTime _time) {

    // wrap to a 1 day interval
    double time = _time - floor(_time / DAY) * DAY;

    // sunAngle = 0 at midnight
    double sourceAngle = 2 * PI * time / DAY;

    sunPosition.x = sin(sourceAngle);
    sunPosition.y = -cos(sourceAngle);
    sunPosition.z = 0;

    moonPosition.x = sin(sourceAngle + PI);
    moonPosition.y = -cos(sourceAngle + PI);
    moonPosition.z = 0;

    // Determine which light source we observe.
    if ((sourceAngle < PI / 2) || (sourceAngle > 3 * PI / 2)) {
        source = MOON;
        sourceAngle += PI;
    } else {
        source = SUN;
    }

    // The light vector changes linearly over time.  The
    // sun and moon travels in a purely east-west arc and
    // are opposite each other.
    lightDirection.x = sin(sourceAngle);
    lightDirection.y = -cos(sourceAngle);
    lightDirection.z = 0;

    const Color3 dayAmbient = Color3(1,1,1) * .30;//Color3::fromARGB(0x373F58);
    const Color3 dayDiffuse = Color3(1,1,1) * .75;

    {
        const double times[] = {MIDNIGHT,               SUNRISE - HOUR,         SUNRISE,              SUNRISE + sunRiseAndSetTime / 4,  SUNRISE + sunRiseAndSetTime,    SUNSET - sunRiseAndSetTime,                               SUNSET - sunRiseAndSetTime / 2, SUNSET,                SUNSET + HOUR/2,       DAY};
        const Color3 color[] = {Color3(.07, .07, .07),  Color3(.1, .1, .1),     Color3(0,0,0),        Color3(.6, .6, 0),                dayDiffuse,                     dayDiffuse,         Color3(.1, .1, .075),           Color3(.1, .05, .05),  Color3(.1, .1, .1), Color3(.07, .07, .07)};
        lightColor = linearSpline(time, times, color, 10);
    }

    {
        const double times[] = {MIDNIGHT,               SUNRISE - HOUR,         SUNRISE,              SUNRISE + sunRiseAndSetTime / 4, SUNRISE + sunRiseAndSetTime, SUNSET - sunRiseAndSetTime,   SUNSET - sunRiseAndSetTime / 2, SUNSET,               SUNSET + HOUR/2,     DAY};
        const Color3 color[] = {Color3(0, .0, .1),      Color3(0, .0, .1),      Color3(0,0,0),        Color3(0,0,0),                   dayAmbient,  dayAmbient,   Color3(.5, .2, .2),             Color3(.05, .05, .1),   Color3(0, .0, .1), Color3(0, .0, .1)};
        ambient = linearSpline(time, times, color, 10);
    }

    {
        const double times[] = {MIDNIGHT,               SUNRISE - HOUR,         SUNRISE,              SUNRISE + sunRiseAndSetTime / 2, SUNRISE + sunRiseAndSetTime, SUNSET - sunRiseAndSetTime, SUNSET - sunRiseAndSetTime / 2, SUNSET,               SUNSET + HOUR/2, DAY};
        const Color3 color[] = {Color3(.1, .1, .17),    Color3(.05, .06, .07),  Color3(.08, .08, .01),  Color3(1,1,1) *.75,              Color3(1,1,1) * .75,         Color3(1,1,1) * .35,        Color3(.5, .2, .2),             Color3(.05, .05, .1),   Color3(.06, .06, .07), Color3(.1, .1, .17)};
        diffuseAmbient = linearSpline(time, times, color, 10);
    }

    {
        const double times[] = {MIDNIGHT,               SUNRISE - HOUR,         SUNRISE - HOUR/2,      SUNRISE,                       SUNRISE + sunRiseAndSetTime,  SUNSET - sunRiseAndSetTime, SUNSET,                  SUNSET + HOUR/3,     DAY};
        const Color3 color[] = {Color3(0,0,0),          Color3(0,0,0),          Color3(.2, .15, .01),   Color3(.2, .15, .01),           Color3(1,1,1),                Color3(1,1,1),              Color3(.4, .2, .05),     Color3(0,0,0),       Color3(0,0,0)};
        skyAmbient = linearSpline(time, times, color, 8);
    }

}

}

