/**
 @file Font.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com

 @created 2002-11-02
 @edited  2003-05-24
 */

#include "GLG3D/Font.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/TextureFormat.h"

namespace G3D {

CFont::CFont(RenderDevice* _rd, const std::string& filename) : renderDevice(_rd) {

    debugAssert(renderDevice);
    debugAssertM(renderDevice->initialized(), "You must call RenderDevice::init before constructing a CFont");

    if (! fileExists(filename)) {
        debugAssertM(false, format("ERROR: Could not load font: %s", filename.c_str()));
        charWidth  = 0;
        charHeight = 0;
        memset(subWidth, 0, 128);
        return;
    }

    BinaryInput b(filename, G3D_LITTLE_ENDIAN, true);

    int ver = b.readInt32();
    debugAssertM(ver == 1, "Can't read font files other than version 1");

    // Read the widths
    for (int c = 0; c < 128; ++c) {
        subWidth[c] = b.readUInt16();
    }

    baseline = b.readUInt16();
    int texWidth = b.readUInt16();
    charWidth  = texWidth / 16;
    charHeight = texWidth / 16;

    // The input may not be a power of 2
    int width  = nextPowerOf2(charWidth * 16);
    int height = nextPowerOf2(charHeight * 8);
 
    // Create a texture
    texture = 
        Texture::fromMemory(filename, ((uint8*)b.getCArray()) + b.getPosition(),
        TextureFormat::A8, width, height, 1, TextureFormat::A8, Texture::CLAMP,
        Texture::TRILINEAR_MIPMAP, Texture::DIM_2D);
}


Vector2 CFont::texelSize() const {
    return Vector2(charWidth, charHeight);
}


void CFont::drawString(
    const std::string&  s,
    double              x,
    double              y,
    double              w,
    double              h,
    Spacing             spacing) const {

    const double propW = w / charWidth;
    const int n = s.length();

    // Shrink the vertical texture coordinates by 1 texel to avoid bilinear interpolation
    // interactions with mipmapping.
    double sy = h / charHeight;

    for (int i = 0; i < n; ++i) {
        char c   = s[i] & 127;

        if (c != ' ') {
            int row   = c / 16;
            int col   = c % 16;

            // Fixed width
            double sx = 0;
            
            if (spacing == PROPORTIONAL_SPACING) {
                sx = (charWidth - subWidth[c]) * propW / 2.0;
            }

            renderDevice->setTexCoord(0, Vector2(col * charWidth, row * charHeight + 1));
            renderDevice->sendVertex(Vector2(x - sx,     y + sy));

            renderDevice->setTexCoord(0, Vector2(col * charWidth, (row + 1) * charHeight - 2));
            renderDevice->sendVertex(Vector2(x- sx,      y + h - sy)); 

            renderDevice->setTexCoord(0, Vector2((col + 1) * charWidth - 1, (row + 1) * charHeight - 2));
            renderDevice->sendVertex(Vector2(x + w - sx, y + h - sy)); 

            renderDevice->setTexCoord(0, Vector2((col + 1) * charWidth - 1, row * charHeight + 1));
            renderDevice->sendVertex(Vector2(x + w - sx, y + sy));
        }

        if (spacing == PROPORTIONAL_SPACING) {
            x += propW * subWidth[c];
        } else {
            x += propW * subWidth['M'] * 0.85;
        }
    }
}


void CFont::draw2DString(
    const std::string&          s,
    double                      x,
    double                      y,
    double                      size,
    const Color4&               color,
    const Color4&               border,
    XAlign                      xalign,
    YAlign                      yalign,
    Spacing                     spacing) const {

    renderDevice->pushState();

    renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
    renderDevice->setAlphaTest(RenderDevice::ALPHA_GEQUAL, 0.05);

    double h = size * 1.5;
    double w = h * charWidth / charHeight;
    double fw = 1.0 / charWidth;
    double fh = 1.0 / charHeight;

    switch (xalign) {
    case XALIGN_RIGHT:
        x -= get2DStringBounds(s, size, spacing).x;
        break;

    case XALIGN_CENTER:
        x -= get2DStringBounds(s, size, spacing).x / 2;
        break;
    }

    switch (yalign) {
    case YALIGN_CENTER:
        y -= h / 2.0;
        break;

    case YALIGN_BASELINE:
        y -= baseline * h / (double)charHeight;
        break;

    case YALIGN_BOTTOM:
        y -= h;
        break;
    }


    double m[] = 
       {1.0 / texture->getTexelWidth(), 0, 0, 0,
        0, 1.0 / texture->getTexelHeight(), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
    renderDevice->setTexture(0, texture);
    renderDevice->setTextureCombineMode(0, RenderDevice::TEX_MODULATE);
    renderDevice->setTextureMatrix(0, m);

    renderDevice->beginPrimitive(RenderDevice::QUADS);

    if (border.a > 0.05) {
        renderDevice->setColor(border);
        for (int dy = -1; dy <= 1; dy += 2) {
            for (int dx = -1; dx <= 1; dx += 2) {
                drawString(s, x + dx, y + dy, w, h, spacing);
            }
        }
    }
    renderDevice->setColor(Color4(color.r * renderDevice->getBrightScale(), color.g * renderDevice->getBrightScale(), color.b * renderDevice->getBrightScale(), color.a));
    drawString(s, x, y, w, h, spacing);
    renderDevice->endPrimitive();

    renderDevice->popState();
}


Vector2 CFont::get2DStringBounds(
    const std::string&  s,
    double              size,
    Spacing             spacing) const {

    int n = s.length();

    double h = size * 1.5;
    double w = h * charWidth / charHeight;
    double propW = w / charWidth;
    double x = 0;
    double y = charHeight;

    if (spacing == PROPORTIONAL_SPACING) {
        for (int i = 0; i < n; ++i) {
            char c   = s[i] & 127;
            x += propW * subWidth[c];
        }
    } else {
        x = subWidth['M'] * n * 0.85;
    }

    return Vector2(x, y);
}


void CFont::convertRAWINItoPWF(const std::string& infileBase, std::string outfile) {
    debugAssert(fileExists(infileBase + ".raw"));
    debugAssert(fileExists(infileBase + ".ini"));

    if (outfile == "") {
        outfile = infileBase + ".fnt";
    }

    BinaryInput  pixel(infileBase + ".raw", G3D_LITTLE_ENDIAN);
    TextInput    ini(infileBase + ".ini");
    BinaryOutput out(outfile, G3D_LITTLE_ENDIAN);

    ini.readSymbol("[");
    ini.readSymbol("Char");
    ini.readSymbol("Widths");
    ini.readSymbol("]");

    // Version
    out.writeInt32(1);

    // Character widths
    for (int i = 0; i < 128; ++i) {
        int n = (int)ini.readNumber();
        debugAssert(n == i);
        ini.readSymbol("=");
        int cw = (int)ini.readNumber();
        out.writeInt16(cw);
    }

    int width = (int)sqrt(pixel.size());
    
    // Autodetect baseline from capital E
    {
        // Size of a character, in texels
        int          w        = width / 16;

        int          x0       = ('E' % 16) * w;
        int          y0       = ('E' / 16) * w;
        
        const uint8* p        = pixel.getCArray();
        bool         done     = false;
        int          baseline = w * 2 / 3;
    
        // Search up from the bottom for the first pixel
        for (int y = y0 + w - 1; (y >= y0) && ! done; --y) {
            for (int x = x0; (x < x0 + w) && ! done; ++x) {
                if (p[x + y * w * 16] != 0) {
                    baseline = y - y0 + 1;
                    done = true;
                }
            }
        }
        out.writeUInt16(baseline);
    }

    // Texture width
    out.writeUInt16(width);

    // Texture
    int num = width * (width / 2);
    out.writeBytes(pixel.getCArray(), num);

    out.compress();
    out.commit();
}

}
