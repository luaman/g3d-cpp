//
// Demo of wxGWindow
//
#include <../contrib/wxGWindow/wxGWindow.h>
#include <../contrib/wxGWindow/wxGWindow.cpp>


class App : public GApp {

protected:

    void main();

public:

    static App* app;

    App(GAppSettings& settings, GWindow* window) : GApp(settings, window) {
        app = this;
    }
};

App* App::app = NULL;

// G3D wrapper objects
class Demo : public GApplet {

public:

    App*            app;

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

    virtual void doGraphics();
};


void App::main() {
	setDebugMode(true);
	debugController.setActive(false);
    Demo* demo = new Demo(this);
    demo->run();
}


Demo::Demo(App* _app) : GApplet(_app), app(_app) {

//    model = IFSModel::create("D:/libraries/g3d-6_05-b01/data/ifs/p51-mustang.ifs", 5);
}


void Demo::init()  {
    app->debugCamera.setPosition(Vector3(0, 0, 10));
    app->debugCamera.lookAt(Vector3(0, 0, 0));

}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }
}


void Demo::doGraphics() {

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);
    app->renderDevice->setObjectToWorldMatrix(CoordinateFrame());

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(true, true, true);
    app->renderDevice->enableLighting();
    app->renderDevice->setLight(0, lighting.directionalLight());
    app->renderDevice->setLight(1, GLight::directional(-Vector3::unitY(), Color3::brown() * .5, false));
    app->renderDevice->setAmbientLightColor(lighting.ambient);

    /*
    app->renderDevice->push2D();
        app->renderDevice->setColor(Color3::red());
        app->renderDevice->beginPrimitive(RenderDevice::QUADS);
            app->renderDevice->sendVertex(Vector2(1,1));
            app->renderDevice->sendVertex(Vector2(1,599));
            app->renderDevice->sendVertex(Vector2(799,599));
            app->renderDevice->sendVertex(Vector2(799,0));
        app->renderDevice->endPrimitive();
    app->renderDevice->pop2D();
    */

    Draw::axes(app->renderDevice);
}


// Define a new frame type
class MyFrame: public wxFrame {
public:
    MyFrame(wxFrame* frame, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

    wxToolBar*      toolBar;
    wxStatusBar*    statusBar;
    wxSlider*       timeSlider;

    Table<std::string, wxBitmap*> bitmapTable;

public:
    wxGWindow*      gwindow;

    // Event handlers
    void OnClose(wxCloseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnFileMenuExit(wxCommandEvent& event);

    DECLARE_EVENT_TABLE();
};

enum {
    // menu items
    FILEMENU_SAVE = 100,
    FILEMENU_EXIT,
    PLAY_BUTTON,
    PAUSE_BUTTON,
    STOP_BUTTON,
    BEGIN_BUTTON,
    RECORD_BUTTON,
    END_BUTTON
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE( MyFrame::OnClose )
    EVT_SIZE( MyFrame::OnSize )
    EVT_MENU(FILEMENU_EXIT,  MyFrame::OnFileMenuExit)
END_EVENT_TABLE()

/* -----------------------------------------------------------------------
  Main Window
-------------------------------------------------------------------------*/


MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style) :
         wxFrame(frame, -1, title, pos, size, style),
         gwindow(NULL),
         toolBar(NULL),
         statusBar(NULL),
         timeSlider(NULL) {

    // Menus            
    wxMenu* fileMenu = new wxMenu;

    fileMenu->Append(FILEMENU_SAVE, _T("&Save...\tCtrl-S"), _T("Save current clip"));
    fileMenu->AppendSeparator();
    fileMenu->Append(FILEMENU_EXIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, _T("&File"));
    SetMenuBar(menuBar);

    // Status bar
    statusBar = new wxStatusBar(this, wxID_ANY);
    SetStatusBar(statusBar);

    // Tool bar
    toolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxSize(100,32), wxTB_HORIZONTAL);
    toolBar->SetToolBitmapSize(wxSize(16,16));

    {
        char* resources[] = {"play", "record", "pause", "begin", "end", "stop", NULL};
        
        for (char** r = resources; *r; ++r) {
            bitmapTable.set(*r,
                new wxBitmap((std::string(*r) + ".bmp").c_str(), 
                             wxBITMAP_TYPE_BMP));
        }
    }

    toolBar->AddTool(PAUSE_BUTTON,      "Pause",    *bitmapTable["pause"],   "Pause playback or recording.", wxITEM_CHECK);
    toolBar->AddTool(RECORD_BUTTON,     "&Record",  *bitmapTable["record"],  "Record over the current clip from the current position.", wxITEM_CHECK);
    toolBar->AddTool(PLAY_BUTTON,       "&Play",    *bitmapTable["play"],    "Play the clip from the current position.", wxITEM_CHECK);
    toolBar->AddTool(STOP_BUTTON,       "Stop",     *bitmapTable["stop"],    "Stop playback or recording.");

    toolBar->AddSeparator();

    toolBar->AddTool(BEGIN_BUTTON,      "Reset",    *bitmapTable["begin"],   "Return to the beginning of the clip.");
    timeSlider = new wxSlider(toolBar, wxID_ANY, 0, 0, 100);
    toolBar->AddControl(timeSlider);
    toolBar->AddTool(END_BUTTON,        "End",      *bitmapTable["end"],     "Skip to the end of the clip.");

    toolBar->Realize();

    // 3D Window
    gwindow = new wxGWindow(GWindowSettings(), this, -1);
}


void MyFrame::OnSize(wxSizeEvent& event) {

    // Layout code
    wxSize size = GetClientSize();

    if (toolBar) {
        toolBar->SetSize(size.x, wxDefaultSize.y);
        toolBar->Move(0, 0);
    }


    if (gwindow) {
        double y0 = 0;
        double y1 = size.y;
        if (toolBar) {
            y0 = toolBar->GetSize().y + toolBar->GetPosition().y;
        }

        if (statusBar) {
            y1 = statusBar->GetPosition().y;
        }

        gwindow->setDimensions(Rect2D::xyxy(0, y0, size.x, y1));

        if (App::app) {
            int h = y1 - y0 + 1;
            App::app->renderDevice->notifyResize(size.x, h);
            App::app->renderDevice->setViewport(Rect2D::xywh(0, 0, size.x, h));
        }
    }


    event.Skip();
}


void MyFrame::OnFileMenuExit(wxCommandEvent& WXUNUSED(event)) {
    App::app->endProgram = true;
}


void MyFrame::OnClose(wxCloseEvent& event) {
    Destroy();
    bitmapTable.deleteValues();
}

// wxWidgets required app object
class MyApp: public wxApp {

private:

    Demo*           demo;

    MyFrame*        frame;

public:

    bool OnInit(void);
};


/*------------------------------------------------------------------
  Application object ( equivalent to main() )
------------------------------------------------------------------ */

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(void) {

    // Create the main frame window
    frame = new MyFrame(NULL,
        "MSVCapture",
        wxPoint(50, 50),
        wxSize(820, 640));

    

    // Show the frame
    frame->Show(true);

    App* gApp = new App(GAppSettings(), frame->gwindow);
    gApp->run();

    return true;
}
