/** 
  @file Sky.cpp

  @author Morgan McGuire, matrix@graphics3d.com

  @created 2002-10-04
  @edited  2003-04-13
  */

#include "GLG3D/glcalls.h"
#include "GLG3D/Sky.h"
#include "GLG3D/LightingParameters.h"

namespace G3D {

Sky::Sky(
    const std::string&                  name,
    const std::string&                  directory,
    const std::string&                  filename,
    double                              quality) {

    // Parse the filename into a base name and extension
    std::string filenameBase("");
    std::string filenameExt("");

    {
        int i = filename.rfind("ft");
        if (i != -1) {
            filenameBase = directory + filename.substr(0, i);
            filenameExt  = filename.substr(i + 2, filename.size() - i - 2); 
        }
    }
    
    static const char* ext[] = {"up", "lf", "rt", "bk", "ft", "dn"};
    
    bool compress   = false;
    int cBits       = 8;

    if (quality > .66) {
        compress = false;
        cBits    = 8;
    } else if (quality > .33) {
        compress = true;
        cBits    = 8;
    } else {
        compress = true;
        cBits    = 5;
    }

    for (int t = 0; t < 6; ++t) {
        texture[t] = new Texture(ext[t], filenameBase + ext[t] + filenameExt, "", Texture::BILINEAR_NO_MIPMAP, Texture::CLAMP, Texture::DIM_2D, cBits, 0, compress);
    }

    moon     = new Texture("Moon",      directory + "moon.jpg",         directory + "moon-alpha.jpg", Texture::BILINEAR_NO_MIPMAP, Texture::TRANSPARENT_BORDER, Texture::DIM_2D, 5, 1, true);
    sun      = new Texture("Sun",       directory + "sun.jpg",          "",                           Texture::BILINEAR_NO_MIPMAP, Texture::TRANSPARENT_BORDER, Texture::DIM_2D, 8, 0, true);
    disk     = new Texture("Flare",     directory + "lensflare.jpg",    "",                           Texture::BILINEAR_NO_MIPMAP, Texture::TRANSPARENT_BORDER, Texture::DIM_2D, 5, 0, true);
    sunRays  = new Texture("Sun rays",  directory + "sun-rays.jpg",     "",                           Texture::BILINEAR_NO_MIPMAP, Texture::TRANSPARENT_BORDER, Texture::DIM_2D, 5, 0, false);

    
    int i = 0;
    star.resize(3000);
    starIntensity.resize(star.size());
    for (i = star.size() - 1; i >= 0; --i) {
        star[i] = Vector4(Vector3::random(), 0);
        starIntensity[i] = square(unitRandom()) + .3;
    }

	this->name = name;
}


Sky::~Sky() {
}


/**
 Used to draw the sky, moon, and lens flare

 @param C center
 @param X a unit axis
 @param Y a unit axis perpendicular to X
 @param r radius of the sphere
 */
static void drawCelestialSphere(
    RenderDevice*                       renderDevice,
    const Vector4&                      C,
    const Vector4&                      X,
    const Vector4&                      Y,
    const double                        r,
    const Color4                        color) {

    renderDevice->setColor(color);
    renderDevice->beginPrimitive(RenderDevice::QUADS);
        renderDevice->setTexCoord(0, Vector2(0, 0));
        renderDevice->sendVertex(C + (-X + Y) * r);
        renderDevice->setTexCoord(0, Vector2(0, 1));
        renderDevice->sendVertex(C + (-X - Y) * r);
        renderDevice->setTexCoord(0, Vector2(1, 1));
        renderDevice->sendVertex(C + ( X - Y) * r);
        renderDevice->setTexCoord(0, Vector2(1, 0));
        renderDevice->sendVertex(C + ( X + Y) * r);
        renderDevice->setTexCoord(0, Vector2(0, 0));
        renderDevice->sendVertex(C + (-X + Y) * r);
    renderDevice->endPrimitive();
}


static void infiniteProjectionMatrix(RenderDevice* renderDevice) {
    double l,r,t,b,n,f;
    bool is3D;
    renderDevice->getProjectionMatrixParams(l,r,t,b,n,f,is3D);
    if (is3D) {
        renderDevice->setProjectionMatrix3D(l,r,t,b,n,inf);
    } else {
        renderDevice->setProjectionMatrix2D(l,r,t,b,n,inf);
    }
}


void Sky::render(
    RenderDevice*                       renderDevice,
    const CoordinateFrame&              camera,
    const LightingParameters&           lighting) {

    renderDevice->pushState();

	CoordinateFrame matrix;
	matrix.rotation = camera.rotation;
    renderDevice->setCameraToWorldMatrix(matrix);

    renderDevice->setColor(lighting.skyAmbient * renderDevice->getBrightScale());
    renderDevice->setCullFace(RenderDevice::CULL_BACK);
    renderDevice->disableDepthWrite();
    renderDevice->setDepthTest(RenderDevice::DEPTH_ALWAYS_PASS);

	// Draw the sky box
	double s = 50;

    renderDevice->resetTextureUnit(0);
    renderDevice->setTexture(0, texture[BK]);
    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->sendVertex(Vector3(-s, +s, -s));
		renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->sendVertex(Vector3(-s, -s, -s));
		renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->sendVertex(Vector3(+s, -s, -s));
		renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->sendVertex(Vector3(+s, +s, -s));
	renderDevice->endPrimitive();

    renderDevice->setTexture(0, texture[LT]);
    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->sendVertex(Vector3(-s, +s, +s));
		renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->sendVertex(Vector3(-s, -s, +s));
		renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->sendVertex(Vector3(-s, -s, -s));
		renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->sendVertex(Vector3(-s, +s, -s));
	renderDevice->endPrimitive();

    renderDevice->setTexture(0, texture[FT]);
    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->sendVertex(Vector3(+s, +s, +s));
		renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->sendVertex(Vector3(+s, -s, +s));
		renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->sendVertex(Vector3(-s, -s, +s));
		renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->sendVertex(Vector3(-s, +s, +s));
	renderDevice->endPrimitive();

    renderDevice->setTexture(0, texture[RT]);
    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->sendVertex(Vector3(+s, +s, +s));
		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->sendVertex(Vector3(+s, +s, -s));
		renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->sendVertex(Vector3(+s, -s, -s));
		renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->sendVertex(Vector3(+s, -s, +s));
	renderDevice->endPrimitive();

    renderDevice->setTexture(0, texture[UP]);
    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->sendVertex(Vector3(+s, +s, +s));
		renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->sendVertex(Vector3(-s, +s, +s));
		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->sendVertex(Vector3(-s, +s, -s));
		renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->sendVertex(Vector3(+s, +s, -s));
	renderDevice->endPrimitive();

    renderDevice->setTexture(0, texture[DN]);
    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->sendVertex(Vector3(+s, -s, -s));
		renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->sendVertex(Vector3(-s, -s, -s));
		renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->sendVertex(Vector3(-s, -s, +s));
		renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->sendVertex(Vector3(+s, -s, +s));
	renderDevice->endPrimitive();

    infiniteProjectionMatrix(renderDevice);

   
    // Draw the moon
    {
        Vector4 L(lighting.moonPosition,0);
        Vector4 X(lighting.moonPosition.cross(Vector3::UNIT_Z).direction(), 0);
        Vector4 Y(Vector3::UNIT_Z, 0);
        // Draw stars
        if (lighting.moonPosition.y > -.3) {
            /*
            glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
            glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(4, GL_FLOAT, 0, star.getCArray());
                glDrawArrays(GL_POINTS, 0, star.size());
            glPopClientAttrib();
            glPopAttrib();
            */

            double k = square((1 - lighting.skyAmbient.length())) * renderDevice->getBrightScale();
            renderDevice->pushState();
                // rotate stars
                CoordinateFrame m;
                m.rotation.setColumn(0, Vector3(L.x, L.y, L.z));
                m.rotation.setColumn(1, Vector3(-X.x, -X.y, -X.z));
                renderDevice->setObjectToWorldMatrix(m);

                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE);
                renderDevice->beginPrimitive(RenderDevice::POINTS);
                for (int i = star.size() - 1; i >= 0; --i) {
                    double b = starIntensity[i] * k;
                    renderDevice->setColor(Color3(b,b,b));
                    renderDevice->sendVertex(star[i]);
                }
                renderDevice->endPrimitive();
            renderDevice->popState();
        }

        renderDevice->setTexture(0, moon);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        renderDevice->setAlphaTest(RenderDevice::ALPHA_GEQUAL, 0.05);
        drawCelestialSphere(renderDevice, L, X, Y, .06, Color4(1,1,1, min(1, max(0, lighting.moonPosition.y * 4))));

    }

    // Draw the sun
    {
        // Sun vector
        Vector4 L(lighting.sunPosition,0);
        Vector4 X(lighting.sunPosition.cross(Vector3::UNIT_Z).direction(), 0);
        Vector4 Y(Vector3::UNIT_Z, 0);

        renderDevice->setTexture(0, sun);
        renderDevice->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);
        Color3 c(Color3::WHITE * .8);

        if (lighting.sunPosition.y < 0) {
            // Fade out the sun as it goes below the horizon
            c *= max(0, (lighting.sunPosition.y + .1) * 10);
        }

        drawCelestialSphere(renderDevice, L, X, Y, .12, c);
    }

    renderDevice->popState();

}



void Sky::renderLensFlare(
    RenderDevice*                       renderDevice,
    const CoordinateFrame&              camera,
    const LightingParameters&           lighting) {

    if (lighting.sunPosition.y < -.1) {
        return;
    }

    renderDevice->pushState();

	    CoordinateFrame matrix;
	    matrix.rotation = camera.rotation;
        renderDevice->setCameraToWorldMatrix(matrix);
	    renderDevice->setObjectToWorldMatrix(CoordinateFrame());

        renderDevice->setColor(lighting.skyAmbient);
        renderDevice->setCullFace(RenderDevice::CULL_BACK);
        renderDevice->disableDepthWrite();
        renderDevice->setDepthTest(RenderDevice::DEPTH_ALWAYS_PASS);
        renderDevice->resetTextureUnit(0);

        // Compute the sun's position using the 3D transformation
        Vector3 pos = renderDevice->project(Vector4(lighting.sunPosition, 0));

        if (lighting.sunPosition.dot(camera.getLookVector()) > 0) {

            // Number of visible points on the sun
            int visible = 0;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    double distanceToSun = renderDevice->getDepthBufferValue(iRound(pos.x + dx * 15.0), iRound(pos.y + dy * 15.0));
                    visible += (distanceToSun >= .99999) ? 1 : 0;
                }
            }

            double fractionOfSunVisible = visible / 9.0;

            if (fractionOfSunVisible > 0.0) {

                // We need to switch to an infinite projection matrix to draw the flares.
                // Note that we must make this change *after* the depth buffer values have
                // been read back.
                infiniteProjectionMatrix(renderDevice);

                renderDevice->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE);

                // Make flares fade out near sunset and sunrise
                double flareBrightness = max(0, sqrt(lighting.sunPosition.y * 4));

                // Sun position
                Vector4 L(lighting.sunPosition,0);
                Vector4 X(lighting.sunPosition.cross(Vector3::UNIT_Z).direction(), 0);
                Vector4 Y(Vector3::UNIT_Z, 0);

                // Sun rays at dawn
                if ((lighting.sunPosition.x > 0) && (lighting.sunPosition.y >= -.1)) {
                    renderDevice->setTexture(0, sunRays);
                    double occlusionAttenuation = (1 - square(2*fractionOfSunVisible - 1));
                    drawCelestialSphere(renderDevice, L, X , Y, .6, occlusionAttenuation * Color4(1,1,1,1) * .4 * max(0, min(1, 1 - lighting.sunPosition.y * 2 / sqrt(2.0))));
                }

                renderDevice->setTexture(0, sun);
                drawCelestialSphere(renderDevice, L, X, Y, .13, Color3::WHITE * fractionOfSunVisible * .5);

                // Lens flare
                Vector4 C(camera.getLookVector(), 0);
                double position[] = { .5,                    .5,                    -.25,                     -.75,                .45,                      .6,                    -.5,                   -.1,                   .55,                     -1.5,                       -2,                         1};
                double size[]     = { .12,                   .05,                    .02,                      .02,                .02,                      .02,                    .01,                  .01,                   .01,                     .01,                        .01,                        0.05}; 
                Color3 color[]    = {Color3(6, 4, 0) / 255, Color3(6, 4, 0) / 255, Color3(0, 12, 0) / 255, Color3(0, 12, 0) / 255, Color3(0, 12, 0) / 255, Color3(0, 12, 0) / 255, Color3(10, 0, 0) /255,  Color3(0, 12, 0) / 255, Color3(10,0,0) / 255, Color3::fromARGB(0x192125)/10,   Color3::fromARGB(0x1F2B1D)/10, Color3::fromARGB(0x1F2B1D)/10};
                int numFlares     = 12;

                renderDevice->setTexture(0, disk);
                for (int i = 0; i < numFlares; ++i) {
                    drawCelestialSphere(renderDevice, C + (C - L) * position[i], X, Y, size[i], Color4(color[i] * flareBrightness, 1));
                }
            }
        }

    renderDevice->popState();
}

} // namespace



