/**
  @file demos/main.cpp

  This is a sample main.cpp to get you started with G3D.  It is
  designed to make writing an application easy.  Although the
  GApp/GApplet infrastructure is helpful for most projects,
  you are not restricted to using it-- choose the level of
  support that is best for your project (see the G3D Map in the
  documentation).

  @author Morgan McGuire, matrix@graphics3d.com
 */

#include <G3DAll.h>
#include "Mmsystem.h"

#if G3D_VER < 60500
    #error Requires G3D 6.05
#endif


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*          app;

    Demo(App* app);

    virtual ~Demo() {}

    virtual void init();

    virtual void doLogic();

	virtual void doNetwork();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();

    virtual void cleanup();

};



class App : public GApp {
protected:
    void main();
public:
    SkyRef              sky;

    Demo*               applet;

    App(const GAppSettings& settings);

    ~App();
};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 2, 10));
    app->debugCamera.lookAt(Vector3(0, 2, 0));
}


void Demo::cleanup() {
    // Called when Demo::run() exits
}


void Demo::doNetwork() {
	// Poll net messages here
}


void Demo::doSimulation(SimTime dt) {
	// Add physical simulation here
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

	// Add other key handling here
}


void Demo::doGraphics() {

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (app->sky.notNull()) {
        app->sky->render(lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);

    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(lighting);
    }
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    // Load objects here
    sky = Sky::create(renderDevice, dataDir + "sky/");
    
    applet->run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}


/** Handle of the timer */
MMRESULT m_idEvent;
double repeatTimeMillis = 33.333;  // milliseconds
int flag = 0;
Array<RealTime> times(100);
int currentTime = 1;

void OnStop() {
    // destroy the timer
    timeKillEvent(m_idEvent);
    
    // reset the timer
    timeEndPeriod((int)repeatTimeMillis);
}

void CALLBACK TimerFunction(UINT wTimerID, UINT msg, 
    DWORD dwUser, DWORD dw1, DWORD dw2)  {
    // This is used only to call MMTimerHandler
    // Typically, this function is static member of CTimersDlg
    //CTimersDlg* obj = (CTimersDlg*) dwUser;
    if (currentTime < 100) {
        times[currentTime] = System::time();
        ++currentTime;
    } else {
        OnStop();
    }
}


void OnBegin() {
    // Set resolution to the minimum supported by the system
    TIMECAPS tc;
    timeGetDevCaps(&tc, sizeof(TIMECAPS));
    DWORD resolution = min(max(tc.wPeriodMin, 0), tc.wPeriodMax);
    timeBeginPeriod(resolution);
	
    // create the timer
    m_idEvent = timeSetEvent(
        (int)repeatTimeMillis,          
        resolution,   
        TimerFunction,      
        0,//(DWORD)this,                  
        TIME_PERIODIC);                
}




int main(int argc, char** argv) {

    // Let the system start up.
    System::sleep(0.5);

    times[0] = System::time();

    // Sleep code
    
  
    for (int i = 1; i < 100; ++i) {
        System::sleep(repeatTimeMillis / 1000.0);
        times[i] = System::time();
    }
/*
    // Timer code
    OnBegin();
    while (currentTime < 100) {
        Sleep(1);
    }
    OnStop();
*/
    TextOutput to("C:/temp/times.txt");
    for (int i = 1; i < 100; ++i) {
        to.writeNumber(times[i] - times[i - 1]);
        to.writeNewline();
    }
    to.commit();
    
    return 0;

    GAppSettings settings;
    settings.window.width = 400;
    settings.window.height = 400;
    App(settings).run();
    return 0;
}

#if 0


 
    OnBegin();
	while(true)	{				
				if (controller_state == 1)
				{
					controller_state = 0;
					OnStop();
				}
				Sleep(200);
				short pattern = 255;
				Out32(0x378, pattern);
				Sleep(200);
				pattern = 0;
				Out32(0x378, pattern);

			}
			else if (c == 101) // recording -- issue 325 signals at the highest rate
			{
				if (controller_state == 1)
				{
					controller_state = 0;
					OnStop();
				}

				m_elTime = 43;
				frame_count = 0;
				recording = true;
				OnBegin();
				do
				{
					Sleep(5);
				} while (frame_count <325);
				OnStop();
				recording = false;

			}
			else if (c == 102) // extr calibration -- 301 triggers slow
			{
				if (controller_state == 1)
				{
					controller_state = 0;
					OnStop();
					Sleep(250);
				}

				for (int k = 0; k < 301; k++)
				{
					short pattern = 255;	
					Out32(0x378, pattern);
					Sleep(250);
					pattern = 0;
					Out32(0x378, pattern);
					Sleep(250);
				}
			}
		}
		Sleep(5);
	}

    OnStop();


	return 0;
}
#endif