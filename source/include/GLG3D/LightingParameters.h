/**
 @file LightingParameters.h

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-10-05
 @edited  2003-11-26

 Copyright 2000-2003, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_LIGHTINGPARAMETERS_H
#define G3D_LIGHTINGPARAMETERS_H

#include "graphics3D.h"

namespace G3D {

#define BROWN_UNIVERSITY_LATITUDE 41.7333f
#define BROWN_UNIVERSITY_LONGITUDE 71.4333f

/* Initital star offset on Jan 1 1970 midnight */
/* Definition of a sidereal day */
#define SIDEREAL_DAY ((23*HOUR)+(56*MINUTE)+(4.071f*SECOND))
	
/** The actual time (measured in seconds since Jan 1 1970 midnight).
    Adjusted for local timezone and daylight savings time. */
//RealTime realWorldLocalTime();

/**
 Provides a reasonable (but not remotely physically correct!) set of lighting parameters
 based on the time of day.  The sun and moon travel in a perfectly east-west arc
 where +x = east and -x = west.
 */
class LightingParameters {
public:
    /** Modulate sky box color */
    Color3                  skyAmbient;

    /**
     Use this for objects that do not receive directional lighting
     (e.g. billboards).
     */
    Color3                  diffuseAmbient;

    /**
     Directional light color.
     */
    Color3                  lightColor;
    Color3                  ambient;

    /** Only one light source, the sun or moon, is active at a given time. */
    Vector3                 lightDirection;
    enum {SUN, MOON}        source;

    /** Using physically correct parameters. */
    bool                    physicallyCorrect;

    /** The vector <B>to</B> the sun */
    Vector3		    trueSunPosition;
    Vector3         sunPosition;

    /** The vector <B>to</B> the moon */
    Vector3		    trueMoonPosition;
    Vector3         moonPosition;
	double			moonPhase;	

    /** The coordinate frame and vector related to the starfield */
    CoordinateFrame	    starFrame;
	CoordinateFrame		trueStarFrame;
    Vector3		    starVec;

    /* Geographic position */
    float                   geoLatitude;
    //float		    geoLongitude;
	
    LightingParameters();

    /**
     Sets light parameters for the sun/moon based on the
     specified time since midnight, as well as geographic 
     latitude for starfield orientation (positive for north 
     of the equator and negative for south) and geographic 
     longitude for sun positioning (postive for east of 
     Greenwich, and negative for west). The latitude and 
     longitude is set by default to that of Providence, RI, 
     USA.
     */
     LightingParameters(
	 const GameTime     _time,
	 bool 	            _physicallyCorrect = true,
	 float              _latitude = BROWN_UNIVERSITY_LATITUDE);

    void setTime(const GameTime _time);
	void setLatitude(float _latitude);

    /**
     Returns a directional light composed from the light direction
     and color.
     */
    GLight directionalLight() const;
};

}

#endif


