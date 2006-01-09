#ifndef TONEMAP_H
#define TONEMAP_H

#include <G3DAll.h>

/** Applies inverse gamma and bloom. 

  Example:

  <PRE>
  void doGraphics() {
    LightingRef        lighting      = toneMap.prepareLighting(app->lighting);
    LightingParameters skyParameters = toneMap.prepareLightingParameters(app->skyParameters);

       // rendering code ...
    toneMap.apply(app->renderDevice);
  }
  </PRE>

*/
class ToneMap {
private:

    /** PS14ATI shaders */
    static unsigned int                 gammaShaderPS14ATI; 

    /** PS20 shaders */
    static ShaderRef            bloomShader, bloomFilterShader;

    TextureRef                  screenImage;

    bool                        stereo;

    /** When in stereo mode, BloomMap 0 is the left eye, bloom map 1 is the right eye. */
    TextureRef                  stereoBloomMap[2];

    bool                        mEnabled;

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

    /** Returns the appropriate bloom map for the current draw buffer */
    TextureRef getBloomMap(RenderDevice* rd) const;

public:

    ToneMap();

    void setEnabled(bool e);

    inline bool enabled() const {
        return mEnabled;
    }

    /** Call before rendering the scene to create a tone-mapping compatible lighting environment.
        Guaranteed to return a new lighting environment that is safe to further mutate. 
    
        If you created the lighting from LightingParameters that was itself prepared, do not call
        this method or the lights will be too dark.
    */
    LightingRef prepareLighting(const LightingRef& L) const;

    /** Call before rendering the scene to create a tone-mapping compatible lighting environment. */
    LightingParameters prepareLightingParameters(const LightingParameters& L) const;

    /** Call after rendering the rest of the scene to apply tone mapping.*/
    void apply(RenderDevice* rd);

};

#endif
