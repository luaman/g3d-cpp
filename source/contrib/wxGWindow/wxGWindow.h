/**
  Binding to allow G3D within wxWindows.

  You must have already installed wxWindows for your platform and
  compiled it with OpenGL bindings:

    "To switch wxGLCanvas support on under Windows, edit setup.h 
     and set wxUSE_GLCANVAS to 1... on Unix, pass --with-opengl 
     to configure to compile using OpenGL or Mesa."

  Including this file automatically links against all necessary
  files on Windows.

  @maintainer Morgan McGuire, matrix@graphics3d.com
  @created 2004-10-02
  @edited  2004-10-12
 */
#ifndef G3D_wxGWindow_H
#define G3D_wxGWindow_H

#define NO_SDL_MAIN
#include <G3DAll.h>

// Link against wxWindows
#ifdef G3D_WIN32
    #ifdef _DEBUG
        #pragma comment(lib, "wxmswd.lib")
        #pragma comment(lib, "regexd.lib")
    #else
        #pragma comment(lib, "wxmsw.lib")
        #pragma comment(lib, "regex.lib")
    #endif

    #pragma comment(lib, "rpcrt4.lib")
    #pragma comment(lib, "comctl32.lib")
#endif

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation
#pragma interface
#endif

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/log.h"

#if !wxUSE_GLCANVAS
#error Please set wxUSE_GLCANVAS to 1 in setup.h.
#endif

#include "wx/glcanvas.h"



/** wxWindows implementation of G3D::GWindow.  Can either wrap an 
    existing wxGLCanvas so that it can be used with RenderDevice
    or create the wxGLCanvas for you from GWindowSettings.  */
class wxGWindow : public GWindow {
private:
    /**
     @param attribList  NULL terminated wxGLCanvas attribute list
     */
    static void makeAttribList(
        const GWindowSettings&  settings,
        Array<int>&             attribList) {

        attribList.clear();
        attribList.append(WX_GL_RGBA, 1);
        attribList.append(WX_GL_LEVEL, 0);
        attribList.append(WX_GL_DOUBLEBUFFER, 1);
        attribList.append(WX_GL_STEREO, settings.stereo ? 1 : 0);
        attribList.append(WX_GL_MIN_RED, settings.rgbBits);
        attribList.append(WX_GL_MIN_GREEN, settings.rgbBits);
        attribList.append(WX_GL_MIN_BLUE, settings.rgbBits);
        attribList.append(WX_GL_MIN_ALPHA, settings.alphaBits);
        attribList.append(WX_GL_DEPTH_SIZE, 
            (settings.depthBits == 24 || settings.depthBits == -1) ? 
            32 : settings.depthBits);
        attribList.append(WX_GL_STENCIL_SIZE, settings.stencilBits); 
    }

    wxGLCanvas*                 window;
    GWindowSettings             settings;
    bool                        _mouseVisible;

    wxCursor                    invisible;
    wxCursor                    arrow;

public:

    wxGWindow(
        const GWindowSettings&  _settings,
        wxWindow*               parent,
        wxWindowID              id = -1)  : invisible(wxCURSOR_BLANK), arrow(wxCURSOR_ARROW) {

        Array<int> attribList;
        makeAttribList(_settings, attribList);
        settings = _settings;

        wxPoint pos(_settings.x, _settings.y);
        wxSize size(_settings.width, _settings.height);

        window = new wxGLCanvas(
            parent, id, pos, size, 0, 
            "WxWindow", attribList.getCArray(), 
            wxNullPalette);

        if (settings.center) {
            window->Center();
        }

        if (! settings.visible) {
            window->Hide();
        }

        window->GetPosition(&settings.x, &settings.y);
        window->GetClientSize(&settings.width, &settings.height);
    }

    wxGWindow(wxGLCanvas* canvas) : 
        invisible(wxCURSOR_BLANK), 
        arrow(wxCURSOR_ARROW), 
        window(canvas) {
    
        window->GetPosition(&settings.x, &settings.y);
        window->GetClientSize(&settings.width, &settings.height);
        settings.visible = window->IsShown();
    }


    std::string getAPIVersion () const {
        return wxVERSION_STRING;
    }

    std::string getAPIName () const {
        return "wxWindows";
    }

    /** The wxWindow represented by this object */
    wxGLCanvas* wxHandle() const {
        return window;
    }

    virtual void swapGLBuffers() {
        window->SwapBuffers();
    }

    virtual void getSettings (GWindowSettings& _settings) const {
        wxG3DCanvas* t = const_cast<wxG3DCanvas*>(this);
        window->GetPosition(&t->settings.x, &t->settings.y);
        _settings = settings;
    }

    virtual int	width() const {
        return settings.width;
    }

    virtual int height() const {
        return settings.height;
    }

    virtual Rect2D dimensions () const {
        wxG3DCanvas* t = const_cast<wxG3DCanvas*>(this);
        window->GetPosition(&t->settings.x, &t->settings.y);
        window->GetClientSize(&t->settings.width, &t->settings.height);
        return Rect2D::xywh(settings.x, settings.y, settings.width, settings.height);
    }

    virtual void setDimensions (const Rect2D &dims) {
        window->SetSize(dims.x0(), dims.y0(), dims.width(), dims.height());
    }

    virtual void setPosition (int x, int y) {
        window->Move(x, y);
    }

    virtual bool hasFocus () const {
        return window->IsEnabled() && (wxWindow::FindFocus() == window);
    }

    virtual void setGammaRamp (const Array<uint16>& gammaRamp) {
        // Ignore
    }

    virtual void setCaption (const std::string& caption) {
        window->SetTitle(caption.c_str());
    }

    virtual int numJoysticks () const {
        return 0;
    }

    virtual std::string joystickName (unsigned int sticknum) {
        return "";
    }

    virtual std::string caption() {
        return window->GetTitle();
    }

    virtual void notifyResize (int w, int h) {
        window->GetPosition(&settings.x, &settings.y);
        window->GetClientSize(&settings.width, &settings.height);
    }

    virtual void setRelativeMousePosition(double x, double y) {
        window->WarpPointer(x, y);
    }

    virtual void setRelativeMousePosition (const Vector2 &p) {
        window->WarpPointer(p.x, p.y);
    }

    virtual void getRelativeMouseState (Vector2 &position, uint8 &mouseButtons) const {
        // TODO
    }

    virtual void getRelativeMouseState (int &x, int &y, uint8 &mouseButtons) const {
        // TODO
    }

    virtual void getRelativeMouseState (double &x, double &y, uint8 &mouseButtons) const {
        // TODO
    }

    virtual void getJoystickState (unsigned int stickNum, Array< float > &axis, Array< bool > &button) {
        // Ignore
    }

    virtual void setInputCapture (bool c) {
        if (c) {
            window->CaptureMouse();
        } else {
            window->ReleaseMouse(); 
        }
    }

    virtual bool inputCapture () const {
        return window->HasCapture();
    }

    virtual void setMouseVisible (bool b) {
        _mouseVisible = b;
        if (b) {
            window->SetCursor(arrow);
        } else {
            window->SetCursor(invisible);
        }
    }

    virtual bool mouseVisible () const {
        return _mouseVisible;
    }
};

#endif
