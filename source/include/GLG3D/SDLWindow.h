/**
  @file SDLWindow.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2004-02-10
  @edited  2004-04-25
*/

#ifndef G3D_SDLWINDOW_H
#define G3D_SDLWINDOW_H

#include "graphics3D.h"
#include "GLG3D/GWindowSettings.h"
#include "GLG3D/GWindow.h"

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
class SDLWindow : public GWindow {
private:

    /** Window title */
    std::string                 _caption;

    /** API version */
    std::string                 _version;

    /** The x, y fields are not updated when the window moves. */
    GWindowSettings             _settings;

    bool                        _inputCapture;

    Array< ::SDL_Joystick* >    joy;

    bool                        _mouseVisible;

    #if defined(G3D_LINUX)
        Display*                _X11Display;
        Window                  _X11Window;
        Window                  _X11WMWindow;
    #elif defined(G3D_WIN32)
        HDC                     _Win32HDC;
        HWND                    _Win32HWND;
    #endif

public:

    SDLWindow(const GWindowSettings& settings);

    virtual ~SDLWindow();

    virtual void getSettings(GWindowSettings& settings) const;

    virtual int width() const;

    virtual int height() const ;

    virtual Rect2D dimensions() const;

    virtual void setDimensions(const Rect2D& dims);

    virtual void setPosition(int x, int y);

    virtual bool hasFocus() const;

    virtual std::string getAPIVersion() const;

    virtual std::string getAPIName() const;

    virtual void setGammaRamp(const Array<uint16>& gammaRamp);

    virtual void setCaption(const std::string& caption);

    virtual std::string caption();

    virtual void setIcon(const GImage& image);

    virtual void swapGLBuffers();

    virtual void notifyResize(int w, int h);

    virtual int numJoysticks() const;

    virtual std::string joystickName(unsigned int sticknum);

    virtual void getJoystickState(unsigned int stickNum, Array<float>& axis, Array<bool>& button);

    virtual void setRelativeMousePosition(double x, double y);

    virtual void setRelativeMousePosition(const Vector2& p);

    virtual void getRelativeMouseState(Vector2& p, uint8& mouseButtons) const;
    virtual void getRelativeMouseState(int& x, int& y, uint8& mouseButtons) const;
    virtual void getRelativeMouseState(double& x, double& y, uint8& mouseButtons) const;

    virtual void setMouseVisible(bool b);

    virtual bool mouseVisible() const;

    virtual void setInputCapture(bool c);

    virtual bool inputCapture() const;

    virtual bool pollEvent(GEvent& e);

    /** Returns the underlying SDL joystick pointer */
    ::SDL_Joystick* getSDL_Joystick(unsigned int num) const;

    #if defined(G3D_LINUX)
        Window   x11Window() const;
        Display* x11Display() const;
    #elif defined(G3D_WIN32)
        HDC      win32HDC() const;
        HWND     win32HWND() const;
    #endif
};

} // namespace

#endif
