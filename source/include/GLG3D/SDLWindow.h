/**
  @file SDLWindow.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2004-02-10
  @edited  2004-02-10
*/

#ifndef G3D_SDLWINDOW_H
#define G3D_SDLWINDOW_H

#include "graphics3d.h"
#include "GLG3D/WindowSettings.h"
#include "GLG3D/Window.h"

#if defined(G3D_OSX)
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#else
#include <SDL.h>
#include <SDL_syswm.h>
#endif


namespace G3D {

/**
 An implementation of G3D::Window that uses the Open Source SDL library.
 Works on Windows, Linux, and OS/X.

 <B>Beta API!  This interface is experimental and is subject to change.</B>
 */
class SDLWindow : public Window {
private:

#ifdef G3D_WIN32
    /** Computed by _win32HDC() */
    HDC                 hdc;
#endif

    /** Sets hdc on Win32, does nothing on other platforms.
        Called from the constructor.*/
    void _win32HDC();

    /** Window title */
    std::string         _caption;

    /** API version */
    std::string         _version;

    WindowSettings      _settings;

public:

    SDLWindow(const WindowSettings& settings);

    virtual ~SDLWindow();

    virtual void getSettings(WindowSettings& settings) const;

    virtual int width() const;

    virtual int height() const ;

    virtual Rect2D dimensions() const;

    virtual void setDimensions(const Rect2D& dims);

    virtual void setPosition(int x, int y) const;

    virtual bool hasFocus() const;

    virtual std::string getAPIVersion() const;

    virtual std::string getAPIName() const;

    virtual void setGammaRamp(const Array<uint16>& gammaRamp);

    virtual void setCaption(const std::string& caption);
    
    #ifdef G3D_WIN32
        virtual HDC getHDC() const;
    #endif

    virtual std::string caption();

    virtual void setIcon(const GImage& image);

    virtual void swapGLBuffers();

    virtual void notifyResize(int w, int h);
};

} // namespace

#endif
