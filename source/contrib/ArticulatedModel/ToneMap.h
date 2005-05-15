#ifndef TONEMAP_H
#define TONEMAP_H

#include <G3DAll.h>

/** Applies inverse gamma and bloom */
class ToneMap {
private:

    /** PS14ATI shaders */
    static uint                 gammaShaderPS14ATI; 

    /** PS20 shaders */
    static ShaderRef            bloomShader, bloomFilterShader;

    TextureRef                  screenImage, bloomMap;

    /** Inverse gamma ramps. */
    // For programmable we don't use B
    static TextureRef RG, B;

    static void makeGammaCorrectionTextures();

    static void makeShadersPS14ATI();

    static void makeShadersPS20();

    /** Resizes screenImage and bloomMap if needed to match the screen size.*/
    void resizeImages(RenderDevice* rd);

    enum Profile {NO_TONE, UNINITIALIZED, PS14ATI, PS14NVIDIA, PS20};

    static Profile profile;

    void applyPS20(RenderDevice* rd);
    void applyPS14ATI(RenderDevice* rd);
    void applyPS14NVIDIA(RenderDevice* rd);

public:

    ToneMap();

    /** Call after rendering the rest of the scene to apply tone mapping.*/
    void apply(RenderDevice* rd);

};

#endif
