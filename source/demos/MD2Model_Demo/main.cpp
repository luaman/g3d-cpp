/**
  @file main.cpp

  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2002-02-27
  @edited  2006-01-29
 */ 

#include <G3DAll.h>


class App : public GApp {
protected:
    void main();
public:

    GApplet* applet;

    App(const GApp::Settings& settings);

    ~App() {
        delete applet;
    }
};


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
    void loadModels(const std::string& dir);

    void loadSkins(const std::string& dir, double brighten);

    void load(const std::string& name);

    void drawCharWithShadow(CoordinateFrame cframe, MD2Model::Pose& pose);
public:

    class App*              app;
    SkyRef                  sky;
    GFontRef                font;

    GameTime                gameTime;

    MD2ModelRef             model;
    Array<TextureRef>       modelTexture;
    MD2ModelRef             weapon;
    TextureRef              weaponTexture;
    MD2Model::Pose          pose;

    /** Names of all of the models available to load */
    Array<std::string>      modelNameArray;

    /** Index into modelNameArray*/
    int                     currentModel;

    Demo(App* _app);

    virtual ~Demo() {}

    virtual void onInit();

    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt);

    virtual void onGraphics(RenderDevice* rd);

    virtual void onUserInput(UserInput* ui);

};


Demo::Demo(App* _app): GApplet(_app),
    app(_app),
    pose(MD2Model::STAND, 0),
    currentModel(0),
    gameTime(0) {

}


void Demo::onInit() {

    app->renderDevice->setCaption("G3D::MD2Model Demo");

    app->debugCamera.setPosition(Vector3(0, 1, -13));
    app->debugCamera.lookAt(Vector3(0,1.6f,-8));

    app->renderDevice->setColorClearValue(Color3(1, 1, 1));

    getDirs(app->dataDir + "quake2/players/*", modelNameArray);

    // Make sure these files all exist
    for (int i = modelNameArray.size() - 1; i >= 0; --i) {
        std::string s = modelNameArray[i];
        if (! fileExists(app->dataDir + "quake2/players/" + modelNameArray[i] + "/tris.md2")) {
            modelNameArray.fastRemove(i);
        }
    }

    if (modelNameArray.size() == 0) {
        const char* choice[] = {"Ok"};
        prompt("MD2 Demo Error", "No MD2 models found in data/players.  "
            "(Download some from polycount.com and expand them into the data directory)", choice, true);
        app->endProgram = true;
        return;
    }

    currentModel = 0;

    load(modelNameArray[currentModel]);
}


void Demo::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
    RealTime timeStep = sdt;

    app->debugController->onSimulation(rdt, clamp(timeStep, 0.0, 0.1), idt);
    Vector3 v = app->debugController->position();

    // Keep the camera above the ground plane
    if (v.y < .1f) {
        app->debugController->setPosition(Vector3(v.x, .1f, v.z));
    }

	app->debugCamera.setCoordinateFrame(app->debugController->frame());

    if (MD2Model::animationDeath(pose.animation)) {
        if (pose.time > 2) {
            // Bring back to life.
            pose.animation = MD2Model::STAND;
        }

    }

    pose.doSimulation(timeStep,
        app->userInput->keyDown(SDLK_LCTRL) || app->userInput->keyDown(SDLK_RCTRL) ||
        app->userInput->keyDown(SDLK_LSHIFT) || app->userInput->keyDown(SDLK_RSHIFT),
        app->userInput->keyDown('r'),
        app->userInput->keyDown('t'),
        app->userInput->keyPressed(SDL_LEFT_MOUSE_KEY),
        app->userInput->keyPressed(' ') || app->userInput->keyPressed(SDLK_BACKSPACE),
        app->userInput->keyPressed('1'),
        app->userInput->keyPressed('2'),
        app->userInput->keyPressed('3'),
        app->userInput->keyPressed('4'),
        app->userInput->keyPressed('5'),
        app->userInput->keyPressed('6'),
        app->userInput->keyPressed('7'),
        app->userInput->keyPressed('8'),
        app->userInput->keyPressed('9'),
        app->userInput->keyPressed('0'),
        app->userInput->keyPressed('-'));
}


void Demo::drawCharWithShadow(CoordinateFrame cframe, MD2Model::Pose& pose) {
    
    app->renderDevice->setColor(Color3::white());
    model->pose(cframe, pose)->render(app->renderDevice);

    //Draw::box(model->pose(cframe, pose)->worldSpaceBoundingBox(), renderDevice);

    if (! MD2Model::animationDeath(pose.animation)) {
        // Weapons have no death animations
        app->renderDevice->setTexture(0, weaponTexture);
        weapon->pose(cframe, pose)->render(app->renderDevice);
    }


    // Shadow
    app->renderDevice->setTexture(0, NULL);
    cframe.translation.y = 0;
    glDisable(GL_LIGHTING);
    cframe.rotation.setColumn(1, Vector3::zero());
    app->renderDevice->setColor(Color3::gray() * .5);
    model->pose(cframe, pose)->render(app->renderDevice);
    glEnable(GL_LIGHTING);
}


void Demo::onGraphics(RenderDevice* rd) {

    LightingParameters lighting(G3D::toSeconds(10, 00, 00, AM));

    // Some models have part of their geometry stored in the "weapon" file.
    // Darth Maul, for example, has his lower half in the weapon.
    const double my = model->pose(CoordinateFrame(), MD2Model::Pose(MD2Model::STAND))->objectSpaceBoundingBox().corner(0).y;
    const double wy = weapon->pose(CoordinateFrame(), MD2Model::Pose(MD2Model::STAND))->objectSpaceBoundingBox().corner(0).y;
    const double footy = 0.98 * min(my, wy);

    rd->clear(true, true, true);
    rd->pushState();
			
		rd->setProjectionAndCameraMatrix(app->debugCamera);
        
        rd->enableLighting();
        rd->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
        rd->setLight(1, GLight::directional(-lighting.lightDirection, Color3::white() * .25));
        rd->setAmbientLightColor(lighting.ambient);

        int n = 1;

        static const RealTime t0 = System::time();
    
        // Draw a bunch of characters
        for (int z = 0; z < 5; ++z) {
            for (int x = -2; x <= 2; ++x) {
                MD2Model::Pose pose(MD2Model::STAND, n + (System::time() - t0));
            
                CoordinateFrame cframe(Vector3(x * 6 + (z % 2) * 2, -footy, z * 6));
                cframe.rotation = Matrix3::fromAxisAngle(Vector3::unitY(), n * .5 + 4);
        
                if (modelTexture.size() > 0) {
                    rd->setTexture(0, modelTexture[(n + 1 + z * 2) % modelTexture.size()]);
                }

                drawCharWithShadow(cframe, pose);

                ++n;
            }
        }

        // Draw the main character
        CoordinateFrame cframe(Vector3(0, -footy, -8));
    
        if (modelTexture.size() > 0) {
            rd->setTexture(0, modelTexture.last());
        }

        drawCharWithShadow(cframe, pose);

        rd->disableLighting();

        rd->setObjectToWorldMatrix(CoordinateFrame());
    
        // Ground plane (to hide parts of characters that stick through ground)
        rd->setColor(Color3::white());
        rd->beginPrimitive(RenderDevice::QUADS);
            rd->sendVertex(Vector3(-50, -.01f, 50));
            rd->sendVertex(Vector3(50, -.01f, 50));
            rd->sendVertex(Vector3(50, -.01f, -50));
            rd->sendVertex(Vector3(-50, -.01f, -50));
        rd->endPrimitive();
    rd->popState();

    rd->push2D();
        double x = 10;
        double y = 10;
        double f = 16;
        app->debugFont->draw2D(rd, format("%d fps", iRound(rd->getFrameRate())), Vector2(x, y), 20, Color3::yellow(), Color3::black()); y += 30;
        app->debugFont->draw2D(rd, format("%d characters", n), Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;
        app->debugFont->draw2D(rd, format("%1.1f MB", model->mainMemorySize() / 1e6), Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;
        app->debugFont->draw2D(rd, format("%1.0f Mtris/sec", rd->getTriangleRate() / 1e6), Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;

        app->debugFont->draw2D(rd, model->name(), Vector2(rd->width()/2, rd->height() - 45), 30, Color3::black(), Color3::white(), GFont::XALIGN_CENTER);

        x = rd->width() - 130;
        y = 10;
        f = 12;
        app->debugFont->draw2D(rd, "CLICK   attack", Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;
        app->debugFont->draw2D(rd, "SPACE  jump", Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;
        app->debugFont->draw2D(rd, "CTRL     crouch", Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;
        app->debugFont->draw2D(rd, "1 . . 5    taunt", Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;
        app->debugFont->draw2D(rd, "6 . . 8    die", Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;
        app->debugFont->draw2D(rd, "9 . . -    pain", Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;
        app->debugFont->draw2D(rd, "R/T       run/back", Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;
        app->debugFont->draw2D(rd, "e           new character", Vector2(x, y), f, Color3::cyan(), Color3::black()); y += f * 1.5;

    rd->pop2D();	   

}


void Demo::loadModels(const std::string& dir) {
    model = MD2Model::create(dir + "tris.md2");

    Array<std::string> weaponFilename;
    getFiles(dir + "w_*.md2", weaponFilename);
    getFiles(dir + "weapon.md2", weaponFilename);

    weapon = MD2Model::create(dir + weaponFilename.last());
}


void Demo::loadSkins(const std::string& dir, double brighten) {
    Array<std::string> previewName;
    Array<std::string> textureName;

    // Find all of the skins that have preview files
    getFiles(dir + "*_i.*", previewName);
    for (int i = 0; i < previewName.size(); ++i) {
        std::string ext      = filenameExt(previewName[i]);
        // Strip off the extension and "_i"
        std::string base     = previewName[i].substr(0, previewName[i].length() - ext.length() - 3);

        getFiles(dir + base + ".*", textureName);
    }

    // Now load the skins themselves
	Texture::PreProcess preProcess;
	preProcess.brighten = brighten;
    for (int i = 0; i < textureName.size(); ++i) {
        std::string ext      = filenameExt(textureName[i]);

        if (GImage::supportedFormat(ext)) {
            std::string filename = dir + textureName[i];
            modelTexture.append(Texture::fromFile(filename, TextureFormat::AUTO, Texture::DIM_2D, Texture::Settings(), preProcess));
        }
    }

    if (weapon->textureFilenames().size() > 0) {
        std::string filename = "data/" + weapon->textureFilenames()[0];
        if (fileExists(filename)) {
            weaponTexture = Texture::fromFile(filename, TextureFormat::AUTO, Texture::DIM_2D, Texture::Settings(), preProcess);
        } else {
            filename = dir + "weapon.pcx";
            if (fileExists(filename)) {
                weaponTexture = Texture::fromFile(filename, TextureFormat::AUTO, Texture::DIM_2D, Texture::Settings(), preProcess);
            }
        }
    } else {
        weaponTexture = NULL;
    }
}


void Demo::load(const std::string& name) {
    double brighten = 2.0;

    std::string dir = std::string(app->dataDir + "quake2/players/") + name + "/";

    loadModels(dir);
    model->setName(toUpper(name.substr(0, 1)) + name.substr(1, name.length() - 1));

    modelTexture.clear();
    weaponTexture = NULL;
    loadSkins(dir, brighten);
}


void Demo::onUserInput(UserInput* ui) {
    GApplet::onUserInput(ui);

    if (ui->keyPressed(SDLK_e)) {
        currentModel = (currentModel + 1) % modelNameArray.size();
        load(modelNameArray[currentModel]);
        return;
    }
}


App::App(const GApp::Settings& settings): GApp(settings) {
}


void App::main() {
    setDebugMode(true);
    debugController->setActive(false);
    debugShowRenderingStats = false;
    debugQuitOnEscape = true;

    applet = new Demo(this);

    applet->run();    
}


int main(int argc, char** argv) {

	GApp::Settings settings;
    settings.useNetwork = false;
    settings.debugFontName = "dominant.fnt";

    App(settings).run();

    return 0;
}
