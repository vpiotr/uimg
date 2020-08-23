#ifndef __UIMG_PAINTER_BASE_H__
#define __UIMG_PAINTER_BASE_H__

#include <vector>
#include "uimg/base/structs.h"
#include "uimg/utils/color_utils.h"

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

#endif
