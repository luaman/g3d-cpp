/**
  @file WindowSettings.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2005-02-10
  @edited  2004-03-06
*/

#ifndef G3D_GWINDOWSETTINGS_H
#define G3D_GWINDOWSETTINGS_H

namespace G3D {

/**
 Used by RenderDevice::init
 */
class GWindowSettings {
public:

    int     width;

    int     height;

    /** Initial x position (can be overridden by fullScreen and center) */
    int     x;

    /** Initial y position (can be overridden by fullScreen and center) */
    int     y;

    /** If true, overrides the x and y settings */
    bool    center;

    /** The number of bits in <B>each</B> color channel of the frame buffer.
       5, <B>8</B>.*/
    int     rgbBits;

    /* The number of bits in the alpha channel of the frame buffer. <B>0</B>, 1, 8 */
    int     alphaBits;

    /** <B>0 (autodetect the best for this card)</B>, 16, <B>24</B>, 32 */
    int     depthBits;

    /** <B>8</B> */
    int     stencilBits;

    /** Number of samples per pixel for anti-aliasing purposes.  <B>1</B> (none), 4, 8 */
    int     fsaaSamples;

    /** Will you accept a software rendering pipeline? */
    bool    hardware;

    bool    fullScreen;

    /** Should buffer flips be un-hitched from refresh rate?  <B>true</B>, false.  True
        generally gives higher frame rates.*/
    bool    asychronous;

    /** Allocate a stereo display context. true, <B>false</B> */
    bool    stereo;

    /** Specify the value at which lighting saturates
     before it is applied to surfaces.  1.0 is the default OpenGL value,
     higher numbers increase the quality of bright lighting at the expense of
     color depth.Default is 1.0.  Set
        to 2.0 to make a Color3::white() light 50% of the maximum brightness. */
    double  lightSaturation;

    /** In cycles/sec */
    int     refreshRate;

    /**
     If true, you should set up your event loop as described in the 
     docs for RenderDevice::resize.
     */
    bool    resizable;

    /**
     When true, a window frame and title bar are present.
     */
    bool    framed;

    GWindowSettings() :
        width(800),
        height(600),
        x(0),
        y(0),
        center(true),
        rgbBits(8),
        alphaBits(0),
        depthBits(24),
        stencilBits(8),
        fsaaSamples(1),
        hardware(true),
        fullScreen(false),
        asychronous(true),
        stereo(false),
        lightSaturation(1.0),
        refreshRate(85),
        resizable(false),
        framed(true) {}
};

// For backwards compatibility to 6.00
typedef GWindowSettings RenderDeviceSettings;

} // namespace

#endif



