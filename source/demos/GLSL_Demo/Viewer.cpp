/**
 @file GLSL_Demo/Viewer.cpp
 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2004-06-25
 @edited  2004-09-25
 */


#include "header.h"

Viewer::Viewer(App* _app) : GApplet(_app), app(_app) {
}


void Viewer::init()  {
    app->debugCamera.setPosition(Vector3(0, 0, 4));
    app->debugCamera.lookAt(Vector3(0, 0, 0));

	entityArray.append(Entity::create(Mesh::quad(), CoordinateFrame()));
    bumpScale = 0.04;
    
    app->debugLog->println("Done Viewer::init");
}


void Viewer::cleanup() {
    // Called when Viewer::run() exits
}


void Viewer::doSimulation(SimTime dt) {
	double t = System::getTick();
	for (int e = 0; e < entityArray.size(); ++e) {
		EntityRef& entity = entityArray[e];

		double a = t + e;

		Vector3 target =
			entity->cframe.translation +
			Vector3(cos(a) * .5, -1, -1 + sin(a) * .5);

		entity->cframe.lookAt(target);
	}
}


void Viewer::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

	if (app->userInput->keyPressed('p')) {
		// Toggle parallax
		if (bumpScale > 0) {
			bumpScale = 0.0;
		} else {
			bumpScale = 0.05;
		}
	}
}


void Viewer::doGraphics() {
    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (app->sky.notNull()) {
        app->sky->render(lighting);
    }

    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);


        CoordinateFrame camera = app->renderDevice->getCameraToWorldMatrix();
        app->bumpShader->args.set("wsLightPos",      Vector4(lighting.lightDirection, 0));
        app->bumpShader->args.set("wsEyePos",        camera.translation);
        app->bumpShader->args.set("texture",         app->textureMap);
        app->bumpShader->args.set("normalBumpMap",   app->normalBumpMap);
        app->bumpShader->args.set("reflectivity",    0);//0.35);
	    app->bumpShader->args.set("specularity",     0);//0.4);
	    app->bumpShader->args.set("bumpScale",       bumpScale);
        app->bumpShader->args.set("environmentMap",  app->sky->getEnvironmentMap());

        app->renderDevice->setShader(app->bumpShader);
		for (int e = 0; e < entityArray.size(); ++e) {
			entityArray[e]->render(app->renderDevice);
		}
        app->renderDevice->setShader(NULL);

    app->renderDevice->disableLighting();

    if (app->sky.notNull()) {
        app->sky->renderLensFlare(lighting);
    }
}


//////////////////////////////////////////

EntityRef Entity::create(const MeshRef& m, const CoordinateFrame& c) {
	return new Entity(m, c);
}


Entity::Entity(const MeshRef& m, const CoordinateFrame& c) : mesh(m), cframe(c) {
}


void Entity::render(RenderDevice* rd) {
	rd->pushState();
		rd->setObjectToWorldMatrix(cframe);
		mesh->render(rd);
	rd->popState();
}


