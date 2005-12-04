#include "ToneMap.h"

// Has to divide exactly into screen width and height
static const double BLOOMSCALE = 8.0;

ToneMap::Profile ToneMap::profile = ToneMap::UNINITIALIZED;

ShaderRef  ToneMap::bloomShader;
ShaderRef  ToneMap::bloomFilterShader;
GLuint     ToneMap::gammaShaderPS14ATI;
TextureRef ToneMap::RG;
TextureRef ToneMap::B;


void ToneMap::apply(RenderDevice* rd) {
    if (! mEnabled) {
        return;
    }

    switch (profile) {
    case PS20:
        applyPS20(rd);
        break;
        
    case PS14ATI:
        applyPS14ATI(rd);
        break;
        
    default:
        // No tone mapping
        ;
    }
}


TextureRef ToneMap::getBloomMap(RenderDevice* rd) const {
    if (stereo && 
        (rd->drawBuffer() == RenderDevice::BUFFER_FRONT_RIGHT) ||
        (rd->drawBuffer() == RenderDevice::BUFFER_BACK_RIGHT)) {
        return stereoBloomMap[1];
    } else {
        return stereoBloomMap[0];
    }
}


void ToneMap::applyPS14ATI(RenderDevice* rd) {

    // TODO: obey viewport

    // TODO: gamma correct
    // TODO: bloom
    resizeImages(rd);
    TextureRef bloomMap = getBloomMap(rd);

    // We're going to use combiners, which G3D does not preserve
    glPushAttrib(GL_TEXTURE_BIT);
    rd->push2D();
        rd->disableLighting();

        // Undo renderdevice's 0.35 translation
        rd->setCameraToWorldMatrix(CoordinateFrame(Matrix3::identity(), Vector3(0, 0, 0.0)));
        
        Rect2D rect = Rect2D::xywh(0, 0, rd->width(), rd->height());
        Rect2D smallRect = bloomMap->rect2DBounds();
        screenImage->copyFromScreen(rect);
        
        glBindFragmentShaderATI(gammaShaderPS14ATI);
        glEnable(GL_FRAGMENT_SHADER_ATI);

            rd->setTexture(0, screenImage);
            rd->setTexture(1, RG);
            rd->setTexture(2, B);

            Draw::rect2D(rect, rd, Color3::white());
    
        glDisable(GL_FRAGMENT_SHADER_ATI);
    rd->pop2D();
    glPopAttrib();
}

/*
        // On NVIDIA, use GL_NV_texture_shader OFFSET_TEXTURE_2D_NV

        // Combiner setup
        //
        // Unit 0:
        // Mode = replace
        // arg0 = texture (screen) 
        // 
        // Unit 1:
        // Mode = dependent read off 0
        // arg0 = texture (RG gamma) 
        //
        // Unit 2:
        // Mode = dependent read off 0
        // arg0 = texture (B gamma) 
        //
        // Unit 3:
        // Mode = add
        // arg0 = unit 1
        // arg1 = unit 2 

        // Apply gamma correction
        rd->setTexture(0, screenImage);
        glActiveTextureARB(GL_TEXTURE0_ARB + 0);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_PRIMARY_COLOR_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);

        rd->setTexture(1, RG);
        glActiveTextureARB(GL_TEXTURE0_ARB + 1);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   BUMP_TARGET_ATI);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_PRIMARY_COLOR_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);
        
        rd->setTexture(2, B);
        glActiveTextureARB(GL_TEXTURE0_ARB + 2);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_MODULATE);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_PRIMARY_COLOR_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);

        rd->setTexture(3, B); // Bind B as a dummy texture
        glActiveTextureARB(GL_TEXTURE0_ARB + 3);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_COMBINE_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB,   GL_ADD);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB,   GL_TEXTURE1_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB,  GL_SRC_COLOR);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB,   GL_TEXTURE2_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB,  GL_SRC_COLOR);
        */


void ToneMap::applyPS20(RenderDevice* rd) {
    // TODO: obey viewport

    resizeImages(rd);
    TextureRef bloomMap = getBloomMap(rd);

    const Vector2 screenScale(1.0 / rd->width(), 1.0 / rd->height());
    bloomFilterShader->args.set("screenScale", screenScale);
    bloomFilterShader->args.set("screenImage", screenImage);

    bloomShader->args.set("screenScale", screenScale);
    bloomShader->args.set("screenImage", screenImage);
    bloomShader->args.set("bloomMap",    bloomMap);
    bloomShader->args.set("gamma",       RG);

    rd->push2D();
        // Undo renderdevice's 0.35 translation
        rd->setCameraToWorldMatrix(CoordinateFrame(Matrix3::identity(), Vector3(0, 0, 0.0)));
        Rect2D rect = Rect2D::xywh(0, 0, rd->width(), rd->height());
        Rect2D smallRect = bloomMap->rect2DBounds();
        screenImage->copyFromScreen(rect);
    
        // Shrink and filter
        rd->setShader(bloomFilterShader);
        Draw::rect2D(smallRect, rd, Color3::white());
    
        // Blend in the previous bloom map for temporal coherence and a nice motion blur.  
        // Due to a bug on NVIDIA cards, we have to do this with a separate pass; 
        // sampler2Ds with different sizes don't work correctly in the same shader. (TODO: verify that this is still a problem!)
        rd->setShader(NULL);
        rd->setTexture(0, bloomMap);
        rd->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        Draw::rect2D(smallRect, rd, Color4(1, 1, 1, 0.25));
        rd->setBlendFunc(RenderDevice::BLEND_ONE, RenderDevice::BLEND_ZERO);    
        bloomMap->copyFromScreen(smallRect);
    
        rd->setShader(bloomShader);

        Draw::rect2D(rect, rd, Color3::white());
    rd->pop2D();
}


void ToneMap::makeGammaCorrectionTextures() {
    if (RG.notNull() || (profile == NO_TONE)) {
        return;
    }
    
    // The inverse gamma ramp function
    G3D::uint8 ramp[256];
    
    for (int i = 0; i < 256; ++i) {
        // Linear
        //ramp[i] = i;

        // Inverse power
        const double A = 1.9;
        ramp[i] = iRound((1.0 - pow(1.0 - i/255.0, A)) * 255.0);

        // Log
        // const double A = 10, B = 1; 
        // ramp[i] = iRound(((log(A*i/255.0 + B) - log(B)) /
        //                  (log(A+B) - log(B))) * 255.0);
    }
    
    GImage data(256, 256, 3);
    
    for (int g = 0; g < 256; ++g) {
        for (int r = 0; r < 256; ++r) {
            Color3uint8& p = data.pixel3(r, g);
            p.r = ramp[r];
            p.g = ramp[g];
            p.b = 0;
        }
    }
    
    // MIP-mapping causes bad interpolation for some reason
    RG = Texture::fromGImage("RG Gamma", data, TextureFormat::RGB8, 
        Texture::CLAMP, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D, 
        Texture::DEPTH_NORMAL, 1.0);
    
    if (profile != PS20) {
        // On PS20 we can re-use the original RG texture
        data.resize(256, 1, 3);
        for (int b = 0; b < 256; ++b) {
            Color3uint8& p = data.pixel3(b, 0);
            p.r = 0;
            p.g = 0;
            p.b = ramp[b];
        }
    
        B = Texture::fromGImage("B Gamma", data, TextureFormat::RGB8, 
            Texture::CLAMP, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D, 
            Texture::DEPTH_NORMAL, 1.0);
    }
}


void ToneMap::makeShadersPS14ATI() {

    // Really slow--appears to be implemented in software!
    gammaShaderPS14ATI = glGenFragmentShadersATI(1);
    glBindFragmentShaderATI(gammaShaderPS14ATI);
    glBeginFragmentShaderATI();

        // Pass 1

        // R0 = texture0[texcoord0]
        glSampleMapATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);  

        // Move the blue component of R0 into the red component of R3
        glColorFragmentOp1ATI(GL_MOV_ATI, GL_REG_3_ATI, GL_RED_BIT_ATI, GL_NONE, 
                              GL_REG_0_ATI, GL_BLUE, GL_NONE);
        // R1 = texture1[R0]
        // R2 = texture2[R3]
        glSampleMapATI(GL_REG_1_ATI, GL_REG_0_ATI, GL_SWIZZLE_STR_ATI);  
        glSampleMapATI(GL_REG_2_ATI, GL_REG_3_ATI, GL_SWIZZLE_STR_ATI);

        // At this point, R1 contains the gamma corrected red and green channels
        // and R2 contains the corrected blue channel.  Combine them:

        // R0 = R1 + R2
        glColorFragmentOp2ATI(GL_ADD_ATI, GL_REG_0_ATI, GL_NONE, GL_NONE, 
                              GL_REG_1_ATI, GL_NONE, GL_NONE,
                              GL_REG_2_ATI, GL_NONE, GL_NONE);
        
        // R0 = texcoord 0
        //glPassTexCoordATI(GL_REG_0_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);
        // 
        //glPassTexCoordATI(GL_REG_1_ATI, GL_TEXTURE0_ARB, GL_SWIZZLE_STR_ATI);

    glEndFragmentShaderATI();
}


void ToneMap::makeShadersPS20() {

    // Create a filtered, thresholded low-resolution version of an image.
    bloomFilterShader = Shader::fromStrings("",         
        STR(
        /* (1/w, 1/h) */
        uniform vec2          screenScale;
        uniform sampler2D     screenImage;
    
        // Only allows bright pixels to pass
        vec4 threshold(vec4 v) {
            // Threshold cutoff
            const float T = 0.875;
        
            const float S = 1.0 / (1.0 - T);
        
            return 
                clamp((v - vec4(T,T,T,0)) * S, vec4(0,0,0,0), vec4(1,1,1,1));
        }
    
        void main(void) {
            // The center pixel on the full screen.  Shift by 1/2 texel to
            // sample two texels at once via bilinear interpolation.
            vec2 p = gl_TexCoord[0].xy + vec2(0.5, 0.5) * screenScale;
        
            vec4 color = vec4(0,0,0,0);
        
            for (int dx = -5; dx <= 5; dx += 2) {
                for (int dy = -5; dy <= 5; dy += 2) {
                    color += threshold(texture2D(screenImage, p + vec2(dx, dy) * screenScale));
                }
            }
        
            // Divide by the number of samples and 
            // rescale the entire range from the cutoff at 0 to max = 4.0
            // We'll apply another factor of 2.5 when the bloom is applied,
            // but we don't want to over saturate here.
        
            gl_FragColor = color * 4.0 / 36.0;
        }
    ));
    
    // Combine the bloom map with the screen image
    bloomShader = Shader::fromStrings("",
        STR(
        /* (1/w, 1/h) */
        uniform vec2          _screenScale;
        uniform sampler2D     screenImage;
        uniform sampler2D     bloomMap;
        uniform sampler2D     gamma; 
    
        void main(void) {
            // The center pixel
            vec2 p = gl_TexCoord[0].xy;
        
            // Brighten the screen image by 1/0.75, since we darkened the 
            // scene when rendering to avoid saturation.
            vec3 screenColor = texture2D(screenImage, gl_TexCoord[0].xy).rgb * 1.34;

            // Apply gamma correction
            screenColor.rg = texture2D(gamma, screenColor.rg).rg;
            screenColor.b  = texture2D(gamma, screenColor.rb).g; 

            // Bloom filter is 8x smaller
            vec2 screenScale = screenScale / 8;

            vec4 bloomColor = 
            
                // Add the bloom (brightened by a factor of 2.5)
                (texture2D(bloomMap, p) +
            
                (texture2D(bloomMap, p + vec2(-0.8,  0.8) * screenScale) + 
                 texture2D(bloomMap, p + vec2( 0.8,  0.8) * screenScale) + 
                 texture2D(bloomMap, p + vec2(-0.8, -0.8) * screenScale) + 
                 texture2D(bloomMap, p + vec2( 0.8, -0.8) * screenScale)) * 0.5 + 

                 texture2D(bloomMap, p + vec2( 0.0, -1.0) * screenScale) + 
                 texture2D(bloomMap, p + vec2(-1.0,  0.0) * screenScale) + 
                 texture2D(bloomMap, p + vec2( 1.0,  0.0) * screenScale) + 
                 texture2D(bloomMap, p + vec2( 0.0,  1.0) * screenScale)) * 2.5 / 7.0;

            // Apply bloom
            gl_FragColor.rgb = screenColor + bloomColor.rgb; 
        }        
        ));
    
}


ToneMap::ToneMap() : mEnabled(true) {
    if (profile == UNINITIALIZED) {
        profile = NO_TONE;
        
        if (GLCaps::supports_GL_ARB_texture_non_power_of_two()) {
            if (Shader::supportsPixelShaders()) {
                profile = PS20;
            } else if (GLCaps::supports("GL_ARB_texture_env_crossbar") &&
                GLCaps::supports("GL_ARB_texture_env_combine") &&
                GLCaps::supports("GL_EXT_texture_env_add") &&
                GLCaps::supports("GL_NV_texture_shader") &&
                (GLCaps::numTextureUnits() >= 4)) {

                profile = PS14NVIDIA;

                // TODO: remove (not currently supporting PS14NIVIDA)
                profile = NO_TONE;
            } else if (GLCaps::supports("GL_ATI_fragment_shader") &&
                (GLCaps::numTextureUnits() >= 4)) {

//              profile = PS14ATI;
                profile = NO_TONE;
            }
        }
    }

    if (profile != NO_TONE) {
        makeGammaCorrectionTextures();
        
        if (profile == PS20) {
            makeShadersPS20();
        } else if (profile == PS14ATI) {
            makeShadersPS14ATI();
        }
    }
}


void ToneMap::resizeImages(RenderDevice* rd) {
    
    const Rect2D viewport = rd->getViewport();
    
    if (screenImage.isNull() ||
        (viewport.wh() != screenImage->vector2Bounds())) {

        stereo = glGetBoolean(GL_STEREO) != 0;
        
        screenImage = Texture::createEmpty(viewport.width(), viewport.height(), 
            "Copied Screen Image", TextureFormat::RGB8,
            Texture::CLAMP, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D_NPOT,
            Texture::DEPTH_NORMAL, 1.0);

        for (int i = 0; i < (stereo ? 2 : 1); ++i) {
            stereoBloomMap[i] = Texture::createEmpty(viewport.width() / BLOOMSCALE, viewport.height() / BLOOMSCALE, 
                "Bloom map", TextureFormat::RGB8,
                Texture::CLAMP, Texture::BILINEAR_NO_MIPMAP, Texture::DIM_2D_NPOT, 
                Texture::DEPTH_NORMAL, 1.0);
        }
    }
}


LightingParameters ToneMap::prepareLightingParameters(const LightingParameters& L) const {

    bool on = mEnabled && (profile != NO_TONE);

    double lightScale = on ? 0.75 : 1.0;
    LightingParameters params = L;

    params.emissiveScale *= lightScale;
    params.skyAmbient   *= on ? 0.5 : 1.0;
    params.diffuseAmbient *= lightScale;
    params.lightColor   *= lightScale;
    params.ambient      *= lightScale;
    
    return params;
}


LightingRef ToneMap::prepareLighting(const LightingRef& L) const {

    double lightScale = (mEnabled && (profile != NO_TONE)) ? 0.75 : 1.0;

    LightingRef lighting = Lighting::create();
    *lighting = *L;

    lighting->environmentMapColor *= lightScale;
    lighting->emissiveScale *= lightScale;

    lighting->ambientTop *= lightScale;
    lighting->ambientBottom *= lightScale;

    for (int i = 0; i < lighting->lightArray.size(); ++i) {
        lighting->lightArray[i].color *= lightScale;
    }

    for (int i = 0; i < lighting->shadowedLightArray.size(); ++i) {
        lighting->shadowedLightArray[i].color *= lightScale;
    }
 
    return lighting;
}


void ToneMap::setEnabled(bool e) {
    mEnabled = e;
    if (! mEnabled) {
        // TODO: wipe old bloomMap to black
    }
}
