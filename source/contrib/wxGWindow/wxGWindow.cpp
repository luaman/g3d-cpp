#include "wxGWindow.h"

void wxGWindow::makeAttribList(
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

wxGWindow::wxGWindow(
        const GWindowSettings&  _settings,
        wxWindow*               parent,
        wxWindowID              id)  : invisible(wxCURSOR_BLANK), arrow(wxCURSOR_ARROW) {

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

wxGWindow::wxGWindow(wxGLCanvas* canvas) : 
        invisible(wxCURSOR_BLANK), 
        arrow(wxCURSOR_ARROW), 
        window(canvas) {
    
        window->GetPosition(&settings.x, &settings.y);
        window->GetClientSize(&settings.width, &settings.height);
        settings.visible = window->IsShown();
}


std::string wxGWindow::getAPIVersion () const {
    return wxVERSION_STRING;
}

std::string wxGWindow::getAPIName () const {
    return "wxWindows";
}

/** The wxWindow represented by this object */
wxGLCanvas* wxGWindow::wxHandle() const {
    return window;
}

void wxGWindow::swapGLBuffers() {
    window->SwapBuffers();
}

void wxGWindow::getSettings (GWindowSettings& _settings) const {
    wxGWindow* t = const_cast<wxGWindow*>(this);
    window->GetPosition(&t->settings.x, &t->settings.y);
    _settings = settings;
}

int	wxGWindow::width() const {
    return settings.width;
}

int wxGWindow::height() const {
    return settings.height;
}

Rect2D wxGWindow::dimensions () const {
    wxGWindow* t = const_cast<wxGWindow*>(this);
    window->GetPosition(&t->settings.x, &t->settings.y);
    window->GetClientSize(&t->settings.width, &t->settings.height);
    return Rect2D::xywh(settings.x, settings.y, settings.width, settings.height);
}

void wxGWindow::setDimensions (const Rect2D &dims) {
    window->SetSize(dims.x0(), dims.y0(), dims.width(), dims.height());
}

void wxGWindow::setPosition (int x, int y) {
    window->Move(x, y);
}

bool wxGWindow::hasFocus () const {
    return window->IsEnabled() && (wxWindow::FindFocus() == window);
}

void wxGWindow::setGammaRamp (const Array<uint16>& gammaRamp) {
    // Ignore
}

void wxGWindow::setCaption (const std::string& caption) {
    window->SetTitle(caption.c_str());
}

int wxGWindow::numJoysticks () const {
    return 0;
}

std::string wxGWindow::joystickName (unsigned int sticknum) {
    return "";
}

std::string wxGWindow::caption() {
    return std::string(window->GetTitle());
}

void wxGWindow::notifyResize (int w, int h) {
    window->GetPosition(&settings.x, &settings.y);
    window->GetClientSize(&settings.width, &settings.height);
}

void wxGWindow::setRelativeMousePosition(double x, double y) {
    window->WarpPointer(x, y);
}

void wxGWindow::setRelativeMousePosition (const Vector2 &p) {
    window->WarpPointer(p.x, p.y);
}

void wxGWindow::getRelativeMouseState (Vector2 &position, uint8 &mouseButtons) const {
    // TODO
}

void wxGWindow::getRelativeMouseState (int &x, int &y, uint8 &mouseButtons) const {
    // TODO
}

void wxGWindow::getRelativeMouseState (double &x, double &y, uint8 &mouseButtons) const {
    // TODO
}

void wxGWindow::getJoystickState (unsigned int stickNum, Array< float > &axis, Array< bool > &button) {
    // Ignore
}

void wxGWindow::setInputCapture (bool c) {
    if (c) {
        window->CaptureMouse();
    } else {
        window->ReleaseMouse(); 
    }
}

bool wxGWindow::inputCapture () const {
    return window->HasCapture();
}

void wxGWindow::setMouseVisible (bool b) {
    _mouseVisible = b;
    if (b) {
        window->SetCursor(arrow);
    } else {
        window->SetCursor(invisible);
    }
}

bool wxGWindow::mouseVisible () const {
    return _mouseVisible;
}
