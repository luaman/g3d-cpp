/**
  @file load.cpp
  
  Code for loading graphics.

  @maintainer Morgan McGuire, matrix@graphics3d.com

  @created 2003-08-11
  @edited  2003-08-13
 */

#include <G3DAll.h>

// Defined in main.cpp
extern RenderDevice*           renderDevice;
extern CFontRef                font;
extern MD2ModelRef             model;
extern Array<TextureRef>       modelTexture;
extern MD2ModelRef             weapon;
extern TextureRef              weaponTexture;
extern std::string             DATA_DIR;

void loadModels(const std::string& dir) {
    model = MD2Model::create(dir + "tris.md2");

    Array<std::string> weaponFilename;
    getFiles(dir + "w_*.md2", weaponFilename);
    getFiles(dir + "weapon.md2", weaponFilename);

    weapon = MD2Model::create(dir + weaponFilename.last());
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
            modelTexture.append(Texture::fromFile(filename, TextureFormat::AUTO, Texture::TILE, Texture::TRILINEAR_MIPMAP, Texture::DIM_2D, brighten));
        }
    }

    if (weapon->textureFilenames().size() > 0) {
        std::string filename = "data/" + weapon->textureFilenames()[0];
        if (fileExists(filename)) {
            weaponTexture = Texture::fromFile(filename, TextureFormat::AUTO, Texture::TILE, Texture::TRILINEAR_MIPMAP, Texture::DIM_2D, brighten);
        } else {
            filename = dir + "weapon.pcx";
            if (fileExists(filename)) {
                weaponTexture = Texture::fromFile(filename, TextureFormat::AUTO, Texture::TILE, Texture::TRILINEAR_MIPMAP, Texture::DIM_2D, brighten);
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
    model->setName(toUpper(name.substr(0, 1)) + name.substr(1, name.length() - 1));

    modelTexture.clear();
    weaponTexture = NULL;
    loadSkins(dir, brighten);
}
