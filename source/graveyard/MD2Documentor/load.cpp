/**
  @file load.cpp
  
  Code for loading graphics.

  @maintainer Kevin Egan, ktegan@cs.rpi.edu
  @cite Original code by Morgan McGuire, matrix@graphics3d.com

  @created 2003-08-11
  @edited  2003-08-21
 */

#include <G3DAll.h>

// Defined in main.cpp
extern RenderDevice*           renderDevice;
extern CFontRef                font;
extern MD2Model                model;
extern Array<TextureRef>       modelTexture;
extern MD2Model                weapon;
extern TextureRef              weaponTexture;
extern std::string             DATA_DIR;


/**
 @param brighten Most Quake II textures are dark; this argument is a factor to make
 them brighter by.  Default = 1.0, normal intensity.  It is safe to call
 load multiple times-- the previously loaded model will be freed correctly.
*/
TextureRef loadBrightTexture(const std::string& filename, double brighten) {

    alwaysAssertM(fileExists(filename), std::string("Can't find \"") + filename + "\"");

    TextureRef texture;

    if (brighten == 1.0) {

        texture = Texture::fromFile(filename, TextureFormat::RGBA8, Texture::CLAMP);

    } else {

        // Brighten the image
        GImage im(filename);

        if (im.channels == 3) {
            for (int i = im.width * im.height - 1; i >= 0; --i) {
                Color3uint8& color = im.pixel3()[i];

                for (int c = 0; c < 3; ++c) {
                    color[c] = iClamp(color[c] * brighten, 0, 255);
                }
            }

            const uint8* array[1];
            array[0] = im.byte();

            texture = Texture::fromMemory(filename, array, TextureFormat::RGB8, im.width, im.height, 1, TextureFormat::RGB8, Texture::CLAMP);
        } else {
            for (int i = im.width * im.height - 1; i >= 0; --i) {
                Color4uint8& color = im.pixel4()[i];

                for (int c = 0; c < 3; ++c) {
                    color[c] = iClamp(color[c] * brighten, 0, 255);
                }
            }

            const uint8* array[1];
            array[0] = im.byte();

            texture = Texture::fromMemory(filename, array, TextureFormat::RGBA8, im.width, im.height, 1, TextureFormat::RGBA8, Texture::CLAMP);
        }
    }

    return texture;
}


void loadModels(const std::string& dir) {
    model.load(dir + "tris.md2");

    Array<std::string> weaponFilename;
    getFiles(dir + "w_*.md2", weaponFilename);
    getFiles(dir + "weapon.md2", weaponFilename);

    weapon.load(dir + weaponFilename.last());
}


void loadSkins(const std::string& dir, double brighten) {
    Array<std::string> previewName;
    Array<std::string> textureName;

    // Find all of the skins that have preview files
    getFiles(dir + "*_i.*", previewName);
    for (int i = 0; i < previewName.size(); ++i) {
        std::string ext      = filenameExt(previewName[i]);
        // Strip off the extension and "_i"
        std::string base     = previewName[i].substr(0, previewName[i].length() - ext.length() - 3);

        getFiles(dir + base + ".*", textureName);
    }

    // Now load the skins themselves
    for (int i = 0; i < textureName.size(); ++i) {
        std::string ext      = filenameExt(textureName[i]);

        if (GImage::supportedFormat(ext)) {
            std::string filename = dir + textureName[i];
            modelTexture.append(loadBrightTexture(filename, brighten));
        }
    }

    if (weapon.textureFilenames().size() > 0) {
        std::string filename = "data/" + weapon.textureFilenames()[0];
        if (fileExists(filename)) {
            weaponTexture = loadBrightTexture(filename, brighten);
        } else {
            filename = dir + "weapon.pcx";
            if (fileExists(filename)) {
                weaponTexture = loadBrightTexture(filename, brighten);
            }
        }
    } else {
        weaponTexture = NULL;
    }
}


void load(const std::string& name) {
    double brighten = 2.0;

    std::string dir = std::string(DATA_DIR + "quake2/players/") + name + "/";

    loadModels(dir);
    model.name = toUpper(name.substr(0, 1)) + name.substr(1, name.length() - 1);

    modelTexture.clear();
    weaponTexture = NULL;
    loadSkins(dir, brighten);
}