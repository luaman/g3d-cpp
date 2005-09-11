/**
  @file gfxmeter/App.h

  @author Morgan McGuire, matrix@graphics3d.com
 */
#ifndef APP_H
#define APP_H


class App : public GApp {
protected:
    void main();
public:

    SkyRef                  sky;

    TextureRef              cardLogo;
    TextureRef              chipLogo;
    std::string             chipSpeed;
    TextureRef              osLogo;

    GFontRef                titleFont;
    GFontRef                reportFont;

    std::string             combineShader;
    std::string             asmShader;
    std::string             glslShader;

    int                     featureRating;
    float                   performanceRating;
    int                     bugCount;

    struct VertexPerformance {
        int                 numTris;
        float               beginEndFPS;
        float               drawElementsRAMFPS; 
        float               drawElementsVBOFPS; 
        float               drawElementsVBO16FPS; 
        float               drawElementsVBOIFPS;
        /* Turn everything off and just slam vertices through */
        float               drawElementsVBOPeakFPS;
    };

    VertexPerformance       vertexPerformance;

    Report*                 applet;

    void computeFeatureRating();
    void countBugs();

    /** Also loads reportFont and gfxMeterTexture */
    void showSplashScreen();

    App(const GAppSettings& settings);

    ~App();
};

#endif
