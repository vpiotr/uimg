#ifndef __UIMG_PAINTER_BASE_H__
#define __UIMG_PAINTER_BASE_H__

#include "uimg/painters/base.h"
#include "uimg/utils/color_utils.h"

// class which paints RGB pixels
class PixelPainter {
public:
    virtual ~PixelPainter() {}

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) = 0;

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) = 0;

    virtual void putPixel(unsigned int x, unsigned int y) {
        static const RgbColor color = {255, 255, 255};
        putPixel(x, y, color);
    }

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color, float alpha) {
        RgbColor prevColor;
        getPixel(x, y, prevColor);
        putPixel(x, y, color_utils::mix_colors(color, prevColor, alpha));
    }

    virtual void putPixel(unsigned int x, unsigned int y, const RgbaColor &color) {
        RgbColor c1;
        c1.red = color.red;
        c1.green = color.green;
        c1.blue = color.blue;
        putPixel(x, y, c1, static_cast<float>(color.alpha) / 255.0f);
    }
};

class LinePainter {
public:
    virtual ~LinePainter() {}

    virtual void
    drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) = 0;
};

class RectPainter {
public:
    virtual ~RectPainter() {}

    virtual void
    drawFull(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) = 0;

    virtual void
    drawEmpty(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) = 0;
};

class TextPainter {
public:
    virtual ~TextPainter() {}

    virtual void drawText(unsigned int x, unsigned int y, const std::string &text, const RgbColor &color) = 0;

    virtual unsigned int textWidth(const std::string &text) = 0;

    virtual Point textSize(const std::string &text) = 0;

    virtual void drawGlyph(unsigned int x, unsigned int y, text_char_code_t char_code, const RgbColor &color) = 0;

    virtual unsigned int glyphWidth(text_char_code_t char_code) = 0;

    virtual unsigned int glyphHeight(text_char_code_t char_code) = 0;

    virtual Point glyphSize(text_char_code_t charCode) = 0;

    virtual void drawText(unsigned int x, unsigned int y, text_source &src, const RgbColor &color) = 0;

    virtual unsigned int textWidth(text_source &src) = 0;

    virtual Point textSize(text_source &src) = 0;
};

class CirclePainter {
public:
    virtual ~CirclePainter() {}

    virtual void drawFull(unsigned int x, unsigned int y, unsigned int r, const RgbColor &color) = 0;

    virtual void drawEmpty(unsigned int x, unsigned int y, unsigned int r, const RgbColor &color) = 0;

    virtual void drawFullWithBorder(unsigned int x, unsigned int y, unsigned int r, unsigned int borderWidth,
                                    const RgbColor &fillColor, const RgbColor &borderColor) = 0;
};

class EllipsePainter {
public:
    virtual ~EllipsePainter() {}

    virtual void drawFull(unsigned int x, unsigned int y, unsigned int rx, unsigned int ry, const RgbColor &color) = 0;

    virtual void drawEmpty(unsigned int x, unsigned int y, unsigned int rx, unsigned int ry, const RgbColor &color) = 0;
};

class BSplinePainter {
public:
    virtual ~BSplinePainter() {}

    virtual void draw(const std::vector<Point> &points, int divisions, const RgbColor &color) = 0;
};

class BackgroundPainter {
public:
    virtual ~BackgroundPainter() {}

    virtual void paint(const RgbColor &color) = 0;
};

class TrianglePainter {
public:
    virtual ~TrianglePainter() {}

    virtual void drawFull(const Point &p1, const Point &p2, const Point &p3, const RgbColor &color) = 0;

    virtual void drawEmpty(const Point &p1, const Point &p2, const Point &p3, const RgbColor &color) = 0;
};

class FloodFillPainter {
public:
    virtual ~FloodFillPainter() {}

    virtual void fill(const Point &p, const RgbColor &color) = 0;
};

// copy image to image
class ImageCopier {
public:
    virtual ~ImageCopier() {}

    virtual void copyFrom(const PixelSource &src, const Point &targetPos) = 0;

    virtual void copyFrom(const PixelSource &src, const Point &targetPos, const Rect &srcPart) = 0;
};

#endif
