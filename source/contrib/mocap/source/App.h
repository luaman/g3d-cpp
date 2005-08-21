#ifndef APP_H
#define APP_H

#include "World.h"

class App : public GApp {
protected:

    void main();

    void loadScene();

public:

    World               world;

    class Demo*               applet;

    App(const GAppSettings& settings);

    ~App();
};

extern App* app;

#endif
