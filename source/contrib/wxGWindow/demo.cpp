//
// Demo of wxGWindow
//
#include "wxGWindow.h"
#include "wx/chkconf.h"


class App : public GApp {

protected:

    void main();

public:

    App(GAppSettings& settings, GWindow* window) : GApp(settings, window) {}
};

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

    Draw::axes(app->renderDevice);

//    PosedModelRef posed = model->pose(frame.toCoordinateFrame());
//    posed->render(app->renderDevice);

//    app->renderDevice->pushState();
//        app->renderDevice->setColor(Color4(0,.5, 1,0.5));
//        app->renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
//        posed = model->pose(current.toCoordinateFrame());
//        posed->render(app->renderDevice);
//    app->renderDevice->popState();

    app->renderDevice->push2D();
        Draw::rect2D(Rect2D::xyxy(10,210,20,220), app->renderDevice);
        Draw::rect2D(Rect2D::xyxy(30,210,40,220), app->renderDevice);
        Draw::rect2D(Rect2D::xyxy(41,210,50,220), app->renderDevice);
        Draw::rect2D(Rect2D::xyxy(50,210,60,220), app->renderDevice);
        Draw::rect2DBorder(Rect2D::xyxy(10,210,20,220), app->renderDevice, Color3::black());
    app->renderDevice->pop2D();
}


// Define a new frame type
class MyFrame: public wxFrame {

public:
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
            const wxSize& size, long style = wxDEFAULT_FRAME_STYLE);
    
public:
    wxGWindow*      gWindow;

    void OnClose(wxCloseEvent& event);

    DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE( MyFrame::OnClose )
END_EVENT_TABLE()

/* -----------------------------------------------------------------------
  Main Window
-------------------------------------------------------------------------*/

MyFrame::MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
                 const wxSize& size, long style) :
         wxFrame(frame, -1, title, pos, size, style),
         gWindow(NULL) {
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
    frame = new MyFrame(NULL, "G3D wxWidgets Demo", wxPoint(50, 50),
                               wxSize(800, 600));

    frame->gWindow = new wxGWindow(GWindowSettings(), frame, -1);

//    demo = new Demo(gApp);

    // Show the frame
    frame->Show(true);

//    demo->app->setDebugMode(true);
//    demo->app->debugController.setActive(true);
//    demo->

    App* gApp = new App(GAppSettings(), frame->gWindow);
    gApp->run();

    return true;
}
