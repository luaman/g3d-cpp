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
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);

public:
    wxGWindow*      gwindow;

    /** Close event */
    void OnClose(wxCloseEvent& event);

    void OnFileMenuExit(wxCommandEvent& event);

    DECLARE_EVENT_TABLE();
};

enum {
    // menu items
    FILEMENU_SAVE = 100,
    FILEMENU_EXIT,

    STATUSBAR,
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE( MyFrame::OnClose )
    EVT_MENU(FILEMENU_EXIT,  MyFrame::OnFileMenuExit)
END_EVENT_TABLE()

/* -----------------------------------------------------------------------
  Main Window
-------------------------------------------------------------------------*/


MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style) :
         wxFrame(frame, -1, title, pos, size, style),
         gwindow(NULL) {

    // Menus            
    wxMenu* fileMenu = new wxMenu;

    fileMenu->Append(FILEMENU_SAVE, _T("&Save...\tCtrl-S"), _T("Save"));
    fileMenu->AppendSeparator();
    fileMenu->Append(FILEMENU_EXIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, _T("&File"));


    SetMenuBar(menuBar);

    wxStatusBar* statusBar = new wxStatusBar(this, STATUSBAR);
    
    SetStatusBar(statusBar);
}


void MyFrame::OnFileMenuExit(wxCommandEvent& WXUNUSED(event)) {
    App::app->endProgram = true;
}


void MyFrame::OnClose(wxCloseEvent& event) {
    Destroy();
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
        "wxWidgets + G3D",
        wxPoint(50, 50),
        wxSize(820, 640));

    frame->gwindow = new wxGWindow(GWindowSettings(), frame, -1);

    // Show the frame
    frame->Show(true);

    App* gApp = new App(GAppSettings(), frame->gwindow);
    gApp->run();

    return true;
}
