#include <G3DAll.h>


RenderDevice*           device      = NULL;


int main(int argc, char** argv) {
    
    // Initialize
    device      = new RenderDevice();
    device->init(800, 600);

    Font*       font[100];

    char*       name[] = 
    {"adventure", "anglo", "arialblack", "arial", "arialround", "bahaus",
    "baskerville", "britannic", "broadway", "bubble", "burton", "century",
    "colonna", "comics", "cooper", "curly", "desmonda", "diner", "dominant",
    "elephant", "emperor", "eurostyle", "federation", "gradl", "led", 
    "lighttrek", "news", "note", 
    "onyx", "pricedown", "sansserif", "saxon",
    "shadowed", "stencil", "terminal", "times", "venusrising", "videophreak", "widelatin", ""};

    int n = 0;


//    Font::convertRAWINItoPWF("c:/tmp/g3dfont/sansserif", "d:/graphics3d/book/cpp/data/font/sansserif.fnt");


    std::string prefix = "d:/graphics3d/book/cpp/data/font/";
    while (std::string("") != name[n]) {
        font[n] = new Font(device, prefix + name[n] + ".fnt");
        ++n;
    }

    int k = iCeil(n / 3.0);
    do {
        device->beginFrame();
        device->setColorClearValue(Color3(0, .2, .1));
        device->clear(true, true, true);
        device->push2D();
        for (int i = 0; i < iCeil(n / 3.0); ++i) {
            font[i]->draw2DString(name[i], 10, 10+ i * 30, 24, Color3::WHITE, Color3::BLACK);
            font[i + k]->draw2DString(name[i + k], 240, 10+ i * 30, 24, Color3::WHITE, Color3::BLACK);
            if (i + k * 2 < n) {
                font[i + k * 2]->draw2DString(name[i + k * 2], 470, 10+ i * 30, 24, Color3::WHITE, Color3::BLACK);
            }
        }
        device->pop2D();
        device->endFrame();

	} while (true);

    // Cleanup
    delete device;

    return 0;
}
