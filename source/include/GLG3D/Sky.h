/**
  @file Sky.h

  @maintainer Morgan McGuire, morgan@cs.brown.edu

  @created 2002-10-04
  @edited  2003-07-07
*/

#ifndef G3D_SKY_H
#define G3D_SKY_H

#include "GLG3D/Texture.h"
#include "GLG3D/RenderDevice.h"
#include "graphics3D.h"

namespace G3D {

/**
 A background cube with an appropriately warped texture to make it 
 look like an infinite environment.  The sky also manages drawing
 the sun and moon, with lens flare effects for the sun.  
 
 Copy the images from data/sky to your project directory to use this
 class or provide your own.
   
 Example:

  <PRE>
    Sky sky("Gentle Clouds", "", "null_plainsky512_*.jpg");
    LightingParameters lighting(toSeconds(9, 00, 00, AM));

    ...

    // Rendering loop
    while (true) {
        sky.render(renderDevice, camera, lighting);
        
        // Draw the rest of the scene
        ...

        sky.renderLensFlare(renderDevice, camera, lighting);
        ...
    }
  </PRE>

 */
class Sky {
private:
    /** Indices into texture array */
    enum Direction {UP = 0, LT = 1, RT = 2, BK = 3, FT = 4, DN = 5};
	
    /**
     Only used if the render device does not support cube maps.
     */
    TextureRef                                  texture[6];

    /**
     Used unless the render device does not support cube maps.
     */
    TextureRef                                  cubeMap;

    TextureRef                                  sun;
    TextureRef                                  moon;
    TextureRef                                  disk;
    TextureRef                                  sunRays;

    std::string                                 name;

    Array<Vector4>                              star;
    Array<float>                                starIntensity;

    class RenderDevice*                         renderDevice;

    /**
     Renders the sky box.
     */
    void renderBox() const;

public:
    /**
     @param directory A directory containing images for:
      moon.jpg, moon-alpha.jpg, sun.jpg, lensflare.jpg, sun-rays.jpg
      as well as the skybox.  If directory is not "" it should
      end in a trailing slash.

     @param filename The name of the front face in series of image files
     that are a skybox.  Each must have the format ffffffXXgggg where
     XX is one of {up, lt, rt, bk, ft, dn}.  Filename should not include
     the directory specified as the previous parameter.

     @param quality Trade image quality for texture memory: 
       .5 -> 1/8  the texture memory of 1.0, 
        0 -> 1/16 the texture memory of 1.0.
        Color banding will occur at low quality settings.

     @param name Arbitrary name for this sky object (for debugging purposes)
     */
    Sky(
        class RenderDevice*                     renderDevice,
        const std::string&                      name,
        const std::string&                      directory,
        const std::string&                      filename = "null_plainsky512_*.jpg",
        double                                  quality = 1.0);

    virtual ~Sky();

    inline std::string getName() const {
		return name;
	}

    /**
     Call at the very beginning of your rendering routine.
     Will restore all state it changes.
     */
	void render(
        const class CoordinateFrame&            camera,
        const class LightingParameters&         lighting);

    /**
     Call at the very end of your rendering routine.
     Will restore all state it changes.
     */
    void renderLensFlare(
        const class CoordinateFrame&            camera,
        const class LightingParameters&         lighting);

    /**
     Returns an environment map (NULL if cube maps are not supported
     on this machine).
     */
    inline TextureRef getEnvironmentMap() const {
        return cubeMap;
    }

};

}

#endif
