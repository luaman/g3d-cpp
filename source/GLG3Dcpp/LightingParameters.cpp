/**
 @file LightingParameters.cpp

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-10-05
 @edited  2003-11-26
 */

#include "GLG3D/LightingParameters.h"
#include <sys/timeb.h>
#include <sys/types.h> 

#ifndef _MSC_VER
   #define _timeb timeb
   #define _ftime ftime
#endif

namespace G3D {

static const double sunRiseAndSetTime = HOUR / 2;

LightingParameters::LightingParameters() {
    setTime(0);
	setLatitude(PROVIDENCE_LATITUDE);
}

LightingParameters::LightingParameters(
    const GameTime                  _time) {
    setTime(_time);
	setLatitude(PROVIDENCE_LATITUDE);
}


LightingParameters::LightingParameters(
    const GameTime                  _time,
	float                       _latitude=PROVIDENCE_LATITUDE) {
    setTime(_time);
	setLatitude(_latitude);
}

void LightingParameters::setLatitude(float _latitude) {
	geoLatitude = _latitude;
}

void LightingParameters::setTime(const GameTime _time) {

    // wrap to a 1 day interval
    double time = _time - floor(_time / DAY) * DAY;

    // sunAngle = 0 at midnight
    double sourceAngle = 2 * G3D_PI * time / DAY;

    sunPosition.x = sin(sourceAngle);
    sunPosition.y = -cos(sourceAngle);
    sunPosition.z = 0;

    moonPosition.x = sin(sourceAngle + G3D_PI);
    moonPosition.y = -cos(sourceAngle + G3D_PI);
    moonPosition.z = 0;

    // Determine which light source we observe.
    if ((sourceAngle < G3D_PI / 2) || (sourceAngle > 3 * G3D_PI / 2)) {
        source = MOON;
        sourceAngle += G3D_PI;
    } else {
        source = SUN;
    }

    // The light vector changes linearly over time.  The
    // sun and moon travels in a purely east-west arc and
    // are opposite each other.
    lightDirection.x = sin(sourceAngle);
    lightDirection.y = -cos(sourceAngle);
    lightDirection.z = 0;

    const Color3 dayAmbient = Color3(1,1,1) * .40;
    const Color3 dayDiffuse = Color3(1,1,1) * .75;

    {
        const double times[] = {MIDNIGHT,               SUNRISE - HOUR,         SUNRISE,              SUNRISE + sunRiseAndSetTime / 4,  SUNRISE + sunRiseAndSetTime,    SUNSET - sunRiseAndSetTime,     SUNSET - sunRiseAndSetTime / 2, SUNSET,                SUNSET + HOUR/2,       DAY};
        const Color3 color[] = {Color3(.2, .2, .2),  Color3(.1, .1, .1),    Color3(0,0,0),        Color3(.6, .6, 0),                dayDiffuse,                     dayDiffuse,                   Color3(.1, .1, .075),           Color3(.1, .05, .05),  Color3(.1, .1, .1), Color3(.2, .2, .2)};
        lightColor = linearSpline(time, times, color, 10);
    }

    {
        const double times[] = {MIDNIGHT,               SUNRISE - HOUR,         SUNRISE,              SUNRISE + sunRiseAndSetTime / 4, SUNRISE + sunRiseAndSetTime, SUNSET - sunRiseAndSetTime,   SUNSET - sunRiseAndSetTime / 2, SUNSET,   SUNSET + HOUR/2,     DAY};
        const Color3 color[] = {Color3(0, .1, .3),      Color3(0, .0, .1),      Color3(0,0,0),        Color3(0,0,0),                   dayAmbient,  dayAmbient,   Color3(.5, .2, .2),             Color3(.05, .05, .1),                     Color3(0, .0, .1),   Color3(0, .1, .3)};
        ambient = linearSpline(time, times, color, 10);
    }

    {
        const double times[] = {MIDNIGHT,               SUNRISE - HOUR,         SUNRISE,              SUNRISE + sunRiseAndSetTime / 2, SUNRISE + sunRiseAndSetTime, SUNSET - sunRiseAndSetTime, SUNSET - sunRiseAndSetTime / 2, SUNSET,               SUNSET + HOUR/2, DAY};
        const Color3 color[] = {Color3(.2, .2, .3),    Color3(.05, .06, .07),  Color3(.08, .08, .01),  Color3(1,1,1) *.75,              Color3(1,1,1) * .75,         Color3(1,1,1) * .35,        Color3(.5, .2, .2),             Color3(.05, .05, .1),   Color3(.06, .06, .07), Color3(.1, .1, .17)};
        diffuseAmbient = linearSpline(time, times, color, 10);
    }

    {
        const double times[] = {MIDNIGHT,               SUNRISE - HOUR,         SUNRISE - HOUR/2,      SUNRISE,                       SUNRISE + sunRiseAndSetTime,  SUNSET - sunRiseAndSetTime, SUNSET,                  SUNSET + HOUR/3,     DAY};
        const Color3 color[] = {Color3(0,0,0),          Color3(0,0,0),          Color3(.2, .15, .01),   Color3(.2, .15, .01),           Color3(1,1,1),                Color3(1,1,1),              Color3(.4, .2, .05),     Color3(0,0,0),       Color3(0,0,0)};
        skyAmbient = linearSpline(time, times, color, 8);
    }
}


GLight LightingParameters::directionalLight() const {
    return GLight::directional(lightDirection, lightColor);
}

}

