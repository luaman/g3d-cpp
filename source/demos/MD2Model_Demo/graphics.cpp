/**
  @file graphics.cpp
  
  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-08-11
  @edited  2003-08-13
 */

#include <G3DAll.h>

// Defined in main.cpp
extern RenderDevice*           renderDevice;
extern CFontRef                font;
extern GCamera*		           camera;
extern MD2Model                model;
extern Array<TextureRef>       modelTexture;
extern MD2Model                weapon;
extern TextureRef              weaponTexture;
extern MD2Model::Pose          pose;
extern SimTime                 gameTime;

void beginLighting(const LightingParameters& lighting) {

    // Setup lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    renderDevice->configureDirectionalLight
      (0, lighting.lightDirection, lighting.lightColor);

    renderDevice->configureDirectionalLight
      (1, -lighting.lightDirection, Color3::WHITE * .25);

    renderDevice->setAmbientLightColor(lighting.ambient);    
}


void endLighting() {
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
}


void drawCharWithShadow(CoordinateFrame cframe, const MD2Model::Pose& pose) {
    renderDevice->setObjectToWorldMatrix(cframe);
    renderDevice->setColor(Color3::WHITE);
    model.render(renderDevice, pose);

    if (! MD2Model::animationDeath(pose.animation)) {
        // Weapons have no death animations
        renderDevice->setTexture(0, weaponTexture);
        weapon.render(renderDevice, pose);
    }

    // Shadow
    renderDevice->setTexture(0, NULL);
    cframe.translation.y = 0;
    glDisable(GL_LIGHTING);
    cframe.rotation.setColumn(1, Vector3::ZERO);
    renderDevice->setObjectToWorldMatrix(cframe);
    renderDevice->setColor(Color3::GRAY * .5);
    model.render(renderDevice, pose);
    glEnable(GL_LIGHTING);

}


void doGraphics() {

    LightingParameters lighting(G3D::toSeconds(10, 00, 00, AM));

    // Some models have part of their geometry stored in the "weapon" file.
    // Darth Maul, for example, has his lower half in the weapon.
    const double footy = 0.98 * min(model.boundingBox(MD2Model::STAND).getCorner(0).y, weapon.boundingBox(MD2Model::STAND).getCorner(0).y);

    renderDevice->beginFrame();
        renderDevice->clear(true, true, true);
        renderDevice->pushState();
			    
		    camera->setProjectionAndCameraMatrix();
            
            beginLighting(lighting);

                int n = 1;
            
                // Draw a bunch of characters
                for (int z = 0; z < 6; ++z) {
                    for (int x = -2; x <= 2; ++x) {
                        MD2Model::Pose pose(MD2Model::STAND, n + gameTime);
                    
                        CoordinateFrame cframe(Vector3(x * 6 + (z % 2) * 2, -footy, z * 6));
                        cframe.rotation.fromAxisAngle(Vector3::UNIT_Y, n * .5 + 4);


                        if (modelTexture.size() > 0) {
                            renderDevice->setTexture(0, modelTexture[(n + 1 + z * 2) % modelTexture.size()]);
                        }

                        drawCharWithShadow(cframe, pose);

                        ++n;
                    }
                }

                // Draw the main character
                {
                    CoordinateFrame cframe(Vector3(0, -footy, -8));
                
                    if (modelTexture.size() > 0) {
                        renderDevice->setTexture(0, modelTexture.last());
                    }

                    drawCharWithShadow(cframe, pose);
                }

            endLighting();

            renderDevice->setObjectToWorldMatrix(CoordinateFrame());
        
            // Ground plane (to hide parts of characters that stick through ground)
            renderDevice->setColor(Color3::WHITE);
            renderDevice->beginPrimitive(RenderDevice::QUADS);
                renderDevice->sendVertex(Vector3(-50, -.01, 50));
                renderDevice->sendVertex(Vector3(50, -.01, 50));
                renderDevice->sendVertex(Vector3(50, -.01, -50));
                renderDevice->sendVertex(Vector3(-50, -.01, -50));
            renderDevice->endPrimitive();
        renderDevice->popState();

        renderDevice->push2D();
            double x = 10;
            double y = 10;
            double f = 16;
            font->draw2D(format("%d fps", iRound(renderDevice->getFrameRate())), Vector2(x, y), 20, Color3::YELLOW, Color3::BLACK); y += 30;
            font->draw2D(format("%d characters", n), Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;
            font->draw2D(format("%1.1f MB", model.mainMemorySize() / 1e6), Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;
            font->draw2D(format("%1.0f Mtris/sec", renderDevice->getTriangleRate() / 1e6), Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;

            font->draw2D(model.name, Vector2(renderDevice->getWidth()/2, renderDevice->getHeight() - 45), 30, Color3::BLACK, Color3::WHITE, CFont::XALIGN_CENTER);

            x = renderDevice->getWidth() - 130;
            y = 10;
            f = 12;
            font->draw2D("CLICK   attack", Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;
            font->draw2D("SPACE  jump", Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;
            font->draw2D("CTRL     crouch", Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;
            font->draw2D("1 . . 5    taunt", Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;
            font->draw2D("6 . . 8    die", Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;
            font->draw2D("9 . . -    pain", Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;
            font->draw2D("R/T       run/back", Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;
            font->draw2D("e           new character", Vector2(x, y), f, Color3::CYAN, Color3::BLACK); y += f * 1.5;
        renderDevice->pop2D();
	   
    renderDevice->endFrame();
}
