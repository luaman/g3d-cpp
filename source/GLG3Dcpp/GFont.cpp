/**
 @file GFont.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com

 @created 2002-11-02
 @edited  2004-02-28
 */

#include "GLG3D/GFont.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/TextureFormat.h"

namespace G3D {

CFontRef GFont::fromFile(RenderDevice* _rd, const std::string& filename) {
    return new GFont(_rd, filename);
}


GFont::GFont(RenderDevice* _rd, const std::string& filename) : renderDevice(_rd) {

    debugAssert(renderDevice);
    debugAssertM(renderDevice->initialized(), 
        "You must call RenderDevice::init before constructing a GFont");
    debugAssertM(renderDevice->supportsTextureFormat(TextureFormat::A8),
        "This graphics card does not support the GL_ALPHA8 texture format used by GFont.");
    debugAssertGLOk();

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
    (void)ver;

    // Read the widths
    for (int c = 0; c < 128; ++c) {
        subWidth[c] = b.readUInt16();
    }

    baseline = b.readUInt16();
    int texWidth = b.readUInt16();
    charWidth  = texWidth / 16;
    charHeight = texWidth / 16;

    // The input may not be a power of 2
    int width  = ceilPow2(charWidth * 16);
    int height = ceilPow2(charHeight * 8);
 
    // Create a texture
    const uint8* ptr = ((uint8*)b.getCArray()) + b.getPosition();
    texture = 
        Texture::fromMemory(filename, &ptr,
            TextureFormat::A8, width, height, 1, TextureFormat::A8, 
            Texture::CLAMP, Texture::TRILINEAR_MIPMAP, Texture::DIM_2D);
}


Vector2 GFont::texelSize() const {
    return Vector2(charWidth, charHeight);
}


Vector2 GFont::drawString(
    const std::string&  s,
    double              x,
    double              y,
    double              w,
    double              h,
    Spacing             spacing) const {

    const double propW = w / charWidth;
    const int n = s.length();

    // Shrink the vertical texture coordinates by 1 texel to avoid
    // bilinear interpolation interactions with mipmapping.
    double sy = h / charHeight;

    double x0 = 0;
    for (int i = 0; i < n; ++i) {
        char c   = s[i] & 127;

        if (c != ' ') {
            int row   = c / 16;
            int col   = c % 16;

            // Fixed width
            double sx = 0;
            
            if (spacing == PROPORTIONAL_SPACING) {
                sx = (charWidth - subWidth[(int)c]) * propW / 2.0;
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
            x += propW * subWidth[(int)c];
        } else {
            x += propW * subWidth[(int)'M'] * 0.85;
        }
    }
    return Vector2(x - x0, h);
}


Vector2 GFont::draw2D(
    const std::string&          s,
    const Vector2&              pos2D,
    double                      size,
    const Color4&               color,
    const Color4&               border,
    XAlign                      xalign,
    YAlign                      yalign,
    Spacing                     spacing) const {

    double x = pos2D.x;
    double y = pos2D.y;

    double h = size * 1.5;
    double w = h * charWidth / charHeight;

    switch (xalign) {
    case XALIGN_RIGHT:
        x -= get2DStringBounds(s, size, spacing).x;
        break;

    case XALIGN_CENTER:
        x -= get2DStringBounds(s, size, spacing).x / 2;
        break;
    
    default:
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

    default:
        break;
    }


    double m[] = 
       {1.0 / texture->getTexelWidth(), 0, 0, 0,
        0, 1.0 / texture->getTexelHeight(), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
    renderDevice->pushState();
        renderDevice->setTextureMatrix(0, m);
        renderDevice->setTexture(0, texture);

        renderDevice->setTextureCombineMode(0, RenderDevice::TEX_MODULATE);

        // This is BLEND_SRC_ALPHA because the texture has no luminance, only alpha
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA,
				   RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        renderDevice->setAlphaTest(RenderDevice::ALPHA_GEQUAL, 0.05);

        renderDevice->beginPrimitive(RenderDevice::QUADS);
            if (border.a > 0.05) {
                renderDevice->setColor(border);
                for (int dy = -1; dy <= 1; dy += 2) {
                    for (int dx = -1; dx <= 1; dx += 2) {
                        drawString(s, x + dx, y + dy, w, h, spacing);
                    }
                }
            }
            renderDevice->setColor(
		        Color4(color.r * renderDevice->getBrightScale(),
			    color.g * renderDevice->getBrightScale(),
			    color.b * renderDevice->getBrightScale(), color.a));
          Vector2 bounds = drawString(s, x, y, w, h, spacing);
        renderDevice->endPrimitive();

    renderDevice->popState();

    return bounds;
}


Vector2 GFont::draw3D(
    const std::string&          s,
    const CoordinateFrame&      pos3D,
    double                      size,
    const Color4&               color,
    const Color4&               border,
    XAlign                      xalign,
    YAlign                      yalign,
    Spacing                     spacing) const {
    
    double x = 0;
    double y = 0;

    double h = size * 1.5;
    double w = h * charWidth / charHeight;

    switch (xalign) {
    case XALIGN_RIGHT:
        x -= get2DStringBounds(s, size, spacing).x;
        break;

    case XALIGN_CENTER:
        x -= get2DStringBounds(s, size, spacing).x / 2;
        break;
    
    default:
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

    default:
        break;
    }


    double m[] = 
       {1.0 / texture->getTexelWidth(), 0, 0, 0,
        0, 1.0 / texture->getTexelHeight(), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

    renderDevice->pushState();
        // We need to get out of Y=up coordinates.
        CoordinateFrame flipY;
        flipY.rotation[1][1] = -1;
        renderDevice->setObjectToWorldMatrix(pos3D * flipY);

        renderDevice->setCullFace(RenderDevice::CULL_NONE);
        renderDevice->setTextureMatrix(0, m);
        renderDevice->setTexture(0, texture);

        renderDevice->setTextureCombineMode(0, RenderDevice::TEX_MODULATE);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, 
				   RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        renderDevice->setAlphaTest(RenderDevice::ALPHA_GEQUAL, 0.05);

        renderDevice->beginPrimitive(RenderDevice::QUADS);

            if (border.a > 0.05) {

	        // Make the equivalent of a 3D "1 pixel" offset (the
	        // default 2D text size is 12-pt with a 1pix border)

 	        const double borderOffset = size / 12.0;
                renderDevice->setColor(border);
                for (int dy = -1; dy <= 1; dy += 2) {
                    for (int dx = -1; dx <= 1; dx += 2) {
                        drawString(s,
				   x + dx * borderOffset, 
				   y + dy * borderOffset,
				   w, h, spacing);
                    }
                }
            }

            renderDevice->setColor(
		    Color4(color.r * renderDevice->getBrightScale(),
			   color.g * renderDevice->getBrightScale(), 
			   color.b * renderDevice->getBrightScale(), 
			   color.a));
            Vector2 bounds = drawString(s, x, y, w, h, spacing);

        renderDevice->endPrimitive();

    renderDevice->popState();

    return bounds;
}


Vector2 GFont::get2DStringBounds(
    const std::string&  s,
    double              size,
    Spacing             spacing) const {

    int n = s.length();

    double h = size * 1.5;
    double w = h * charWidth / charHeight;
    double propW = w / charWidth;
    double x = 0;
    double y = h;

    if (spacing == PROPORTIONAL_SPACING) {
        for (int i = 0; i < n; ++i) {
            char c   = s[i] & 127;
            x += propW * subWidth[(int)c];
        }
    } else {
        x = subWidth[(int)'M'] * n * 0.85 * propW;
    }

    return Vector2(x, y);
}


void GFont::convertRAWINItoPWF(const std::string& infileBase, std::string outfile) {
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
        (void)n;
        debugAssert(n == i);
        ini.readSymbol("=");
        int cw = (int)ini.readNumber();
        out.writeInt16(cw);
    }

    int width = (int)sqrt((double)pixel.size());
    
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
    out.commit(false);
}

}
