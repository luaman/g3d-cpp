/** 
  @file Sky.cpp

  @author Morgan McGuire, matrix@graphics3d.com

  @created 2002-10-04
  @edited  2003-11-24
  */

#include "GLG3D/glcalls.h"
#include "GLG3D/Sky.h"
#include "GLG3D/LightingParameters.h"
#include "G3D/BinaryInput.h"
#include "G3D/g3dmath.h"
#include "GLG3D/TextureFormat.h"
#include "GLG3D/getOpenGLState.h"

namespace G3D {

#define SHORT_TO_FLOAT(x) ((2.0f*x+1.0f)*(1.0f/65535.0f))

    
SkyRef Sky::create(
    RenderDevice*                       rd,
    const std::string&                  directory,
    const std::string&                  filename,
    bool                                _drawCelestialBodies,
    double                              quality) {
    return new Sky(rd, directory, filename, _drawCelestialBodies, quality);
}


Sky::Sky(
    RenderDevice*                       rd,
    const std::string&                  directory,
    const std::string&                  filename,
    bool                                _drawCelestialBodies,
    double                              quality) : renderDevice(rd),
        drawCelestialBodies(_drawCelestialBodies) {

    debugAssertM((directory == "") || (directory[directory.size() - 1] == '/') 
        || (directory[directory.size() - 1] == '\\'), "Directory must end in a slash");

    const TextureFormat* format;
    const TextureFormat* alphaFormat;

    if (quality > .66) {
        format      = TextureFormat::RGB8;
        alphaFormat = TextureFormat::RGBA8;
    } else if (quality > .33) {
        format      = TextureFormat::RGB_DXT1;
        alphaFormat = TextureFormat::RGBA_DXT1;
    } else {
        format      = TextureFormat::RGBA_DXT5;
        alphaFormat = TextureFormat::RGBA_DXT5;
    }

    // Look for the filename
    // Parse the filename into a base name and extension
    std::string filenameBase;
    std::string filenameExt;
    std::string fullFilename = filename;

    Texture::splitFilename(fullFilename, filenameBase, filenameExt);

    if (! fileExists(filenameBase + "up" + filenameExt)) {
        fullFilename = directory + filename;
        Texture::splitFilename(fullFilename, filenameBase, filenameExt);
    }


    if (renderDevice->supportsOpenGLExtension("GL_ARB_texture_cube_map")) {
   
        cubeMap = Texture::fromFile(directory + filename, format, Texture::CLAMP, Texture::TRILINEAR_MIPMAP, Texture::DIM_CUBE_MAP);

        for (int t = 0; t < 6; ++t) {
            texture[t] = NULL;
        }

    } else {    
        static const char* ext[] = {"up", "lf", "rt", "bk", "ft", "dn"};
    

        for (int t = 0; t < 6; ++t) {
            texture[t] = Texture::fromFile(filenameBase + ext[t] + filenameExt, 
                format, Texture::CLAMP, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D);
        }
    }

    if (drawCelestialBodies) {
        moon     = Texture::fromTwoFiles(directory + "moon.jpg", directory + "moon-alpha.jpg",
            alphaFormat, Texture::TRANSPARENT_BORDER, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D);

        sun      = Texture::fromFile(directory + "sun.jpg", format, Texture::TRANSPARENT_BORDER, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D);
        disk     = Texture::fromFile(directory + "lensflare.jpg", format, Texture::TRANSPARENT_BORDER, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D);
        sunRays  = Texture::fromFile(directory + "sun-rays.jpg", format, Texture::TRANSPARENT_BORDER, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D);
    
        int i = 0;
        // Try to read actual star data
        BinaryInput in = BinaryInput(directory + "real.str", G3D_LITTLE_ENDIAN, true);
        if (in.getLength() > 0) {
	        // If file exists, load the real starfield
            int16 numStars;
		    float32 x, y, z;

	        std::string header = in.readString(5);
	        debugAssert(header == "STARS"); (void)header;

	        numStars = in.readInt16();
	        star.resize(numStars);
	        starIntensity.resize(numStars);

		    // Read X, Y, Z, and intensity
		    for(i = 0; i < numStars; i++) {
			    x = SHORT_TO_FLOAT(in.readInt16());
			    y = SHORT_TO_FLOAT(in.readInt16());
			    z = SHORT_TO_FLOAT(in.readInt16());

			    star[i] = Vector4(x, y, z, 0);

			    starIntensity[i] = square(SHORT_TO_FLOAT(in.readInt16())) + .7;
	        }
        } else {
		    // Create a random starfield
   		    star.resize(3000);
    	    starIntensity.resize(star.size());
   		    for (i = star.size() - 1; i >= 0; --i) {
   			    star[i] = Vector4(Vector3::random(), 0);
   			    starIntensity[i] = square(unitRandom()) + .3;
   		    }
 	    }
    }

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


static void hackProjectionMatrix(RenderDevice* renderDevice) {

    Matrix4 P = renderDevice->getProjectionMatrix();

    // Set the 3rd row (2nd index) so the depth always is in the middle of the depth range.

    P[2][0] = 0;
    P[2][1] = 0;
    P[2][2] = -0.5;
    P[2][3] = 0;

    renderDevice->setProjectionMatrix(P);
}


void Sky::renderBox() const {
    renderDevice->pushState();
    double s = 50;

    bool cube = (cubeMap != NULL);

    if (cube) {
        renderDevice->setTexture(0, cubeMap);

        glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_GEN_S | GL_TEXTURE_GEN_T | GL_TEXTURE_GEN_R | GL_TEXTURE_GEN_Q);

	    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
        for (int i = 0; i < 3; ++i) {
    	    glTexGeni(GL_S + i, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP_ARB);
	        glEnable(GL_TEXTURE_GEN_S + i);
        }
 
        // Texture generation will be in object space
        renderDevice->setTextureMatrix(0, renderDevice->getCameraToWorldMatrix());

    } else {
        CoordinateFrame cframe;
        cframe.rotation.fromAxisAngle(Vector3::UNIT_Y, toRadians(-90));
        renderDevice->setObjectToWorldMatrix(cframe);

        renderDevice->setTexture(0, texture[BK]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->setNormal(Vector3(-s, +s, -s));
		renderDevice->sendVertex(Vector3(-s, +s, -s));

		renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->setNormal(Vector3(-s, -s, -s));
		renderDevice->sendVertex(Vector3(-s, -s, -s));

		renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->setNormal(Vector3(+s, -s, -s));
		renderDevice->sendVertex(Vector3(+s, -s, -s));

		renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->setNormal(Vector3(+s, +s, -s));
		renderDevice->sendVertex(Vector3(+s, +s, -s));
	renderDevice->endPrimitive();

    if (! cube) {
        renderDevice->setTexture(0, texture[LT]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->setNormal(Vector3(-s, +s, +s));
		renderDevice->sendVertex(Vector3(-s, +s, +s));

		renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->setNormal(Vector3(-s, -s, +s));
		renderDevice->sendVertex(Vector3(-s, -s, +s));

        renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->setNormal(Vector3(-s, -s, -s));
		renderDevice->sendVertex(Vector3(-s, -s, -s));
		
        renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->setNormal(Vector3(-s, +s, -s));
		renderDevice->sendVertex(Vector3(-s, +s, -s));
	renderDevice->endPrimitive();

    
    if (! cube) {
        renderDevice->setTexture(0, texture[FT]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->setNormal(Vector3(+s, +s, +s));
		renderDevice->sendVertex(Vector3(+s, +s, +s));

        renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->setNormal(Vector3(+s, -s, +s));
		renderDevice->sendVertex(Vector3(+s, -s, +s));
		
        renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->setNormal(Vector3(-s, -s, +s));
		renderDevice->sendVertex(Vector3(-s, -s, +s));
		
        renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->setNormal(Vector3(-s, +s, +s));
		renderDevice->sendVertex(Vector3(-s, +s, +s));
	renderDevice->endPrimitive();

    if (! cube) {
        renderDevice->setTexture(0, texture[RT]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->setNormal(Vector3(+s, +s, +s));
		renderDevice->sendVertex(Vector3(+s, +s, +s));

		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->setNormal(Vector3(+s, +s, -s));
		renderDevice->sendVertex(Vector3(+s, +s, -s));

        renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->setNormal(Vector3(+s, -s, -s));
		renderDevice->sendVertex(Vector3(+s, -s, -s));
		
        renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->setNormal(Vector3(+s, -s, +s));
		renderDevice->sendVertex(Vector3(+s, -s, +s));
	renderDevice->endPrimitive();

    if (! cube) {
        renderDevice->setTexture(0, texture[UP]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->setNormal(Vector3(+s, +s, +s));
		renderDevice->sendVertex(Vector3(+s, +s, +s));

        renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->setNormal(Vector3(-s, +s, +s));
		renderDevice->sendVertex(Vector3(-s, +s, +s));
		
        renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->setNormal(Vector3(-s, +s, -s));
		renderDevice->sendVertex(Vector3(-s, +s, -s));
		
        renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->setNormal(Vector3(+s, +s, -s));
		renderDevice->sendVertex(Vector3(+s, +s, -s));
	renderDevice->endPrimitive();

    if (! cube) {
        renderDevice->setTexture(0, texture[DN]);
    }

    renderDevice->beginPrimitive(RenderDevice::QUADS);
		renderDevice->setTexCoord(0, Vector2(0, 0));
		renderDevice->setNormal(Vector3(+s, -s, -s));
		renderDevice->sendVertex(Vector3(+s, -s, -s));

		renderDevice->setTexCoord(0, Vector2(0, 1));
		renderDevice->setNormal(Vector3(-s, -s, -s));
		renderDevice->sendVertex(Vector3(-s, -s, -s));

		renderDevice->setTexCoord(0, Vector2(1, 1));
		renderDevice->setNormal(Vector3(-s, -s, +s));
		renderDevice->sendVertex(Vector3(-s, -s, +s));

		renderDevice->setTexCoord(0, Vector2(1, 0));
		renderDevice->setNormal(Vector3(+s, -s, +s));
		renderDevice->sendVertex(Vector3(+s, -s, +s));
	renderDevice->endPrimitive();

    if (cube) {
        for (int i = 0; i < 3; ++i) {
	        glDisable(GL_TEXTURE_GEN_S + i);
        }

	    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
        glPopAttrib();
    }

    renderDevice->popState();
}


void Sky::render(
    const LightingParameters&           lighting) {

    renderDevice->pushState();

	    CoordinateFrame matrix;
	    matrix.rotation = renderDevice->getCameraToWorldMatrix().rotation;
        renderDevice->setCameraToWorldMatrix(matrix);

        renderDevice->setColor(lighting.skyAmbient * renderDevice->getBrightScale());
        renderDevice->setCullFace(RenderDevice::CULL_BACK);
        renderDevice->disableDepthWrite();
        renderDevice->setDepthTest(RenderDevice::DEPTH_ALWAYS_PASS);

	    // Draw the sky box
        renderDevice->resetTextureUnit(0);
        renderBox();

        if (drawCelestialBodies) {
            // Ignore depth, make sure we're not clipped by the far plane
            hackProjectionMatrix(renderDevice);
   
            drawMoonAndStars(lighting);

            drawSun(lighting);
        }

    renderDevice->popState();
}


void Sky::drawMoonAndStars(const LightingParameters& lighting) {
    Vector4 L(lighting.moonPosition,0);
    Vector4 X(lighting.moonPosition.cross(Vector3::UNIT_Z).direction(), 0);
    Vector4 Y(Vector3::UNIT_Z, 0);

    // Draw stars
    if (lighting.moonPosition.y > -.3) {

        double k = (1 - square(lighting.skyAmbient.length())) * renderDevice->getBrightScale();
        renderDevice->pushState();
            // Rotate stars
            CoordinateFrame m;
            float aX, aY, aZ;
			// Use the east-west revolutions of the moon to rotate
			//   the starfield correctly
			Vector3 moon(-L.x, 0, L.y);
			Vector3 top(Vector3::UNIT_Y);
			m.lookAt(moon, top);

			// Correct for geographical location; currently Providence, RI
			m.rotation.toEulerAnglesXYZ(aX, aY, aZ);
			aX -= lighting.geoLatitude; // Latitude of 41 degrees, 44 minutes N
			m.rotation.fromEulerAnglesXYZ(aX, aY, aZ);
			renderDevice->setObjectToWorldMatrix(m);
            
			renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE);

            renderDevice->beginPrimitive(RenderDevice::POINTS);
                for (int i = star.size() - 1; i >= 0; --i) {
                    const double b = starIntensity[i] * k;
                    // We use raw GL calls here for performance
                    glColor3f(b, b, b);
                    glVertex3fv(star[i]);
                }
            renderDevice->endPrimitive();

            // Get RenderDevice back in sync with real GL state
            renderDevice->setColor(Color3::WHITE);
            glColor(Color3::WHITE);
        renderDevice->popState();
    }

    renderDevice->setTexture(0, moon);
    renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
    renderDevice->setAlphaTest(RenderDevice::ALPHA_GEQUAL, 0.05);
    drawCelestialSphere(renderDevice, L, X, Y, .06, Color4(1,1,1, min(1, max(0, lighting.moonPosition.y * 4))));
}


void Sky::drawSun(const LightingParameters& lighting) {
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


void Sky::renderLensFlare(
    const LightingParameters&           lighting) {

    if (! drawCelestialBodies) {
        return;
    }

    if (lighting.sunPosition.y < -.1) {
        return;
    }

    renderDevice->pushState();

        CoordinateFrame camera = renderDevice->getCameraToWorldMatrix();
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
        Vector4 pos = renderDevice->project(Vector4(lighting.sunPosition, 0));

        if (lighting.sunPosition.dot(camera.getLookVector()) > 0) {

            // Number of visible points on the sun
            int visible = 0;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    double distanceToSun = 
                        renderDevice->getDepthBufferValue(
                         iRound(pos.x + dx * 15.0),
                         iRound(pos.y + dy * 15.0));
                    visible += (distanceToSun >= .99999) ? 1 : 0;
                }
            }

            double fractionOfSunVisible = visible / 9.0;

            if (fractionOfSunVisible > 0.0) {

                // We need to switch to an infinite projection matrix
                // to draw the flares.  Note that we must make this
                // change *after* the depth buffer values have been
                // read back.
                hackProjectionMatrix(renderDevice);

                renderDevice->setBlendFunc(RenderDevice::BLEND_ONE,
                                           RenderDevice::BLEND_ONE);

                // Make flares fade out near sunset and sunrise
                double flareBrightness = 
                    max(0, sqrt(lighting.sunPosition.y * 4));

                // Sun position
                Vector4 L(lighting.sunPosition,0);
                Vector4 X(lighting.sunPosition.cross(Vector3::UNIT_Z).direction(), 0);
                Vector4 Y(Vector3::UNIT_Z, 0);

                // Sun rays at dawn
                if ((lighting.sunPosition.x > 0) && 
                    (lighting.sunPosition.y >= -.1)) {

                    renderDevice->setTexture(0, sunRays);
                    double occlusionAttenuation = 
                        (1 - square(2*fractionOfSunVisible - 1));

                    drawCelestialSphere(renderDevice, L, X , Y, .6,
                                        occlusionAttenuation * Color4(1,1,1,1) * .4 * max(0, min(1, 1 - lighting.sunPosition.y * 2 / sqrt(2.0))));
                }

                renderDevice->setTexture(0, sun);
                drawCelestialSphere(renderDevice, L, X, Y, .13,
                                    Color3::WHITE * fractionOfSunVisible * .5);

                // Lens flare
                Vector4 C(camera.getLookVector(), 0);
                double position[] = { .5,                    .5,                    -.25,                     -.75,                .45,                      .6,                    -.5,                   -.1,                   .55,                     -1.5,                       -2,                         1};
                double size[]     = { .12,                   .05,                    .02,                      .02,                .02,                      .02,                    .01,                  .01,                   .01,                     .01,                        .01,                        0.05}; 
                Color3 color[]    = {Color3(6, 4, 0) / 255, Color3(6, 4, 0) / 255, Color3(0, 12, 0) / 255, Color3(0, 12, 0) / 255, Color3(0, 12, 0) / 255, Color3(0, 12, 0) / 255, Color3(10, 0, 0) /255,  Color3(0, 12, 0) / 255, Color3(10,0,0) / 255, Color3::fromARGB(0x192125)/10,   Color3::fromARGB(0x1F2B1D)/10, Color3::fromARGB(0x1F2B1D)/10};
                int numFlares     = 12;

                renderDevice->setTexture(0, disk);
                for (int i = 0; i < numFlares; ++i) {
                    drawCelestialSphere(renderDevice, 
                         C + (C - L) * position[i], X, Y, size[i], 
                         Color4(color[i] * flareBrightness, 1));
                }
            }
        }

    renderDevice->popState();
}

} // namespace
