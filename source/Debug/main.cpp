
#if 0        
        
        /** Crappy demo (:P) showing GLSL bug on ATI using G3D 6.06 **/

#include <G3DAll.h>
#pragma once

#include <G3DAll.h>

#ifdef FREE
#	undef FREE
#endif
#define FREE(x) { if(x) { delete x; x = NULL; } }

class MainClass
{
public:

	MainClass(void) :	m_renderDevice(NULL),
						m_userInput(NULL)
	{
	}

	~MainClass(void)
	{
		FREE(m_userInput);
		m_renderDevice->cleanup();
		FREE(m_renderDevice);
	}

	bool init(void);
	int run(void);

	bool procesEvents(void);

	void renderPlane(CoordinateFrame cframe, ShaderRef shader);

private:

	RenderDevice*		m_renderDevice;
	UserInput*			m_userInput;

	GCamera				m_camera;

	TextureRef			m_stoneTexture;

	// Using no uniforms to give the fragment a color should work.
	ShaderRef			m_constantShader;

	// Using uniforms to give the fragment a color and a texture doesn't work, so the bug is in the texture arguments.
	ShaderRef			m_uniformShader;

};

int main(int argc, char *argv[])
{
	MainClass mainClass;
	if(!mainClass.init())	return 1;

	return mainClass.run();
}


bool MainClass::init(void)
{
	RenderDeviceSettings settings;

	if((m_renderDevice = new RenderDevice()) == NULL)	
	{
		return false;
	}
	if(!m_renderDevice->init(settings, Log::common()))	
	{
		return false;
	}

	m_renderDevice->setCaption("GLSL bug");
	m_renderDevice->setColorClearValue(Color3::white() * .5);

	if((m_userInput	= new UserInput()) == NULL)	
	{
		return false;		
	}

	m_camera.setPosition(Vector3(0.0f, 1.0f, -2.0f));
	m_camera.lookAt(Vector3(0.0f, 0.0f, 0.0f));


	m_stoneTexture = Texture::fromFile("D:/tmp/stones.jpg");


	m_constantShader = Shader::fromStrings("", STR(
		void main(void)
		{
			gl_FragColor.rgb = vec3(1.0, 0.0, 0.0);
		}
		), DO_NOT_DEFINE_G3D_UNIFORMS);
	debugAssert(m_constantShader.notNull());
	debugAssert(m_constantShader->ok());


	m_uniformShader = Shader::fromStrings("", STR(
		uniform sampler2D texture;

		void main(void)
		{
			gl_FragColor.rgb = texture2D(texture, gl_TexCoord[0].st).rgb;
		}
		), DO_NOT_DEFINE_G3D_UNIFORMS);
	debugAssert(m_uniformShader.notNull());
	debugAssert(m_uniformShader->ok());

	return true;
}

int MainClass::run(void)
{
	while(procesEvents())
	{
		m_renderDevice->beginFrame();
		m_renderDevice->clear(true, true, false);
		m_renderDevice->pushState();

		m_renderDevice->setProjectionAndCameraMatrix(m_camera);

		CoordinateFrame cframe;

        m_renderDevice->pushState();
		cframe.translation = Vector3(-1, 0.0f, -0.6f);
		renderPlane(cframe, m_constantShader);
        m_renderDevice->popState();

        m_renderDevice->pushState();
		m_uniformShader->args.set("texture", m_stoneTexture);
		cframe.translation = Vector3(0, 0.0f, -0.6f);
		renderPlane(cframe, m_uniformShader);
        m_renderDevice->popState();

        m_renderDevice->pushState();
        m_renderDevice->setTexture(0, NULL);
		m_uniformShader->args.set("texture", m_stoneTexture);
		cframe.translation = Vector3(1, 0.0f, -0.6f);
		renderPlane(cframe, m_uniformShader);
        m_renderDevice->popState();

		m_renderDevice->popState();
		m_renderDevice->endFrame();
	}

	return 1;
}

bool MainClass::procesEvents(void)
{
	m_userInput->beginEvents();

	GEvent event;
	while(m_renderDevice->window()->pollEvent(event))
	{
		if(event.type == SDL_QUIT) return false;
		else if(event.type == SDL_KEYDOWN)
		{
			if(event.key.keysym.sym == SDLK_ESCAPE)	return false;
		}

		m_userInput->processEvent(event);
	}

	m_userInput->endEvents();

	return true;
}

void MainClass::renderPlane(CoordinateFrame cframe, ShaderRef shader)
{
	Array<Vector3> vertexArray;
	vertexArray.append(Vector3(-0.5f, 0.0f, -0.5f));
	vertexArray.append(Vector3(-0.5f, 0.0f,  0.5f));
	vertexArray.append(Vector3( 0.5f, 0.0f,  0.5f));
	vertexArray.append(Vector3( 0.5f, 0.0f, -0.5f));

	Array<Vector2> texCoordArray;
	texCoordArray.append(Vector2(0.0f, 0.0f));
	texCoordArray.append(Vector2(0.0f, 1.0f));
	texCoordArray.append(Vector2(1.0f, 1.0f));
	texCoordArray.append(Vector2(1.0f, 0.0f));

	Array<int> indexArray;
	indexArray.append(0, 1, 2, 3);

	VARAreaRef area	= VARArea::create(vertexArray.size() * sizeof(Vector3) + 
										texCoordArray.size() * sizeof(Vector2) + 10, VARArea::WRITE_ONCE);
	VAR vertexVAR	= VAR(vertexArray, area);
	VAR texCoordVAR	= VAR(texCoordArray, area);

	m_renderDevice->pushState();
	m_renderDevice->setObjectToWorldMatrix(cframe);

	m_renderDevice->setShader(shader);
	m_renderDevice->beginIndexedPrimitives();

	m_renderDevice->setVertexArray(vertexVAR);
	m_renderDevice->setTexCoordArray(0, texCoordVAR);
	m_renderDevice->sendIndices(RenderDevice::QUADS, indexArray);

	m_renderDevice->endIndexedPrimitives();

	/*m_renderDevice->beginPrimitive(RenderDevice::QUADS);

	m_renderDevice->setTexCoord(0, Vector2(0.0f, 0.0f));
	m_renderDevice->sendVertex(Vector3(-0.5f, 0.0f, -0.5f));
	m_renderDevice->setTexCoord(0, Vector2(0.0f, 1.0f));
	m_renderDevice->sendVertex(Vector3(-0.5f, 0.0f,  0.5f));
	m_renderDevice->setTexCoord(0, Vector2(1.0f, 1.0f));
	m_renderDevice->sendVertex(Vector3( 0.5f, 0.0f,  0.5f));
	m_renderDevice->setTexCoord(0, Vector2(1.0f, 0.0f));
	m_renderDevice->sendVertex(Vector3( 0.5f, 0.0f, -0.5f));

	m_renderDevice->endPrimitive();*/

	m_renderDevice->popState();
}




#if 0

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

#if G3D_VER < 60700
    #error Requires G3D 6.0
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
        app->sky->render(app->renderDevice, lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);

		Draw::axes(CoordinateFrame(Vector3(0, 4, 0)), app->renderDevice);

    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(app->renderDevice, lighting);
    }
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(true);

    // Load objects here
    sky = Sky::create(NULL, dataDir + "sky/");
    
    applet->run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
    applet = new Demo(this);
}


App::~App() {
    delete applet;
}


int main(int argc, char** argv) {
    GAppSettings settings;
    settings.useNetwork = false;
    settings.window.fullScreen = true;
    settings.window.framed = false;
    settings.window.width = 800;
    settings.window.height = 600;
    App(settings).run();
    return 0;
}

#endif
#endif