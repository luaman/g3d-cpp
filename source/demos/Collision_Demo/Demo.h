#ifndef DEMO_H
#define DEMO_H

#include <G3DAll.h>
#include "Scene.h"

/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
private:
    void buildScene();

    void insertRamps();
    void insertSpiralSlide();
    void insertRollingContactSpheres();
    void insertTray();
    void insertRestingContactSpheres();

public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*              app;

    Scene                   scene;

    /** World time */
    GameTime                gameTime;

    Demo(App* app);
    ~Demo();

    virtual void init();

    virtual void doLogic();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();
};

#endif
