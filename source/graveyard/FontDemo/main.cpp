/**
 This is the program used to generate a picture of all the fonts for the website.
 It is not really a demo and is not officially supported.
 */

#include <G3DAll.h>

RenderDevice*           device      = NULL;


int main(int argc, char** argv) {
    
    // Initialize
    device      = new RenderDevice();
    device->init(800, 600);

    Array<CFontRef>    font;

    Array<std::string> tname;

    
    std::string prefix = "d:/graphics3d/book/data/font/";
    /*
    {
        // Convert fonts
        std::string srcDir = "c:/tmp/font/";
        getFiles(srcDir + "*.ini", tname);
        for (int i = 0; i < tname.size(); ++i) {
            std::string file = tname[i].substr(0, tname[i].length() - 4);
            CFont::convertRAWINItoPWF(srcDir + file, prefix + file + ".fnt");
        }
        tname.clear();
    }
    */

    getFiles(prefix + "*.fnt", tname);

    Array<std::string> name;
    for (int n = 0; n < tname.size(); ++n) {
        // Don't show -small fonts.
        if (! endsWith(tname[n], "-small.fnt")) {
            font.append(CFont::fromFile(device, prefix + tname[n]));
            // Strip the extension
            name.append(tname[n].substr(0, tname[n].length() - 4));
        }
    }

    int k = iCeil(font.size() / 3.0);
    do {
        device->beginFrame();
        device->setColorClearValue(Color3(0, .2, .1));
        device->clear(true, true, true);
        device->push2D();
        for (int i = 0; i < k; ++i) {

            font[i]->draw2DString(name[i], 10, 10 + i * 30, 24, Color3::WHITE, Color3::BLACK);
            font[i + k]->draw2DString(name[i + k], 240, 10 + i * 30, 24, Color3::WHITE, Color3::BLACK);
            if (i + k * 2 < font.size()) {
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
