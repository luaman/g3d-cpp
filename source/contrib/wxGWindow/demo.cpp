//
// Demo of wxGWindow
//
#include "wxGWindow.h"

// Define a new application type
class MyApp: public wxApp {
public:
    bool OnInit(void);
};

// Define a new frame type
class MyFrame: public wxFrame {

public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
    
public:
    wxGWindow*    gWindow;

};


/* -----------------------------------------------------------------------
  Main Window
-------------------------------------------------------------------------*/

MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style) :
         wxFrame(frame, -1, title, pos, size, style),
         gWindow(NULL) {
}


/*------------------------------------------------------------------
  Application object ( equivalent to main() )
------------------------------------------------------------------ */

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void) {

  // Create the main frame window
  MyFrame *frame = new MyFrame(NULL, "G3D wxWidgets Demo", wxPoint(50, 50),
                               wxSize(800, 600));

  frame->gWindow = new wxGWindow(GWindowSettings(), frame, -1);

  // Show the frame
  frame->Show(true);

  return true;
}


