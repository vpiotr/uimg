#ifndef __UIMG_PAINTER_FOR_PIXELS_H__
#define __UIMG_PAINTER_FOR_PIXELS_H__

#include <cassert>
#include <algorithm>

#include "uimg/base/structs.h"
#include "uimg/pixels/pixel_painter.h"
#include "uimg/utils/math_utils.h"
#include "uimg/utils/cubic_spline_utils.h"

typedef std::vector<unsigned int> LineDashPattern;

class LinePainterForPixels : public LinePainter {
public:
    LinePainterForPixels(PixelPainter &pixelPainter) : pixelPainter_(&pixelPainter) {}

    // uses Bresenham's line algorithm
    virtual void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        if (x1 > x2) {
            drawLine(x2, y2, x1, y1, color);
            return;
        } else if (x1 == x2) {
            drawVerticalLine(x1, y1, y2, color);
            return;
        } else if (y1 == y2) {
            drawHorizontalLine(x1, x2, y1, color);
            return;
        }

        typedef float real;
        real deltax = static_cast<real>(x2) - static_cast<real>(x1);
        real deltay = static_cast<real>(y2) - static_cast<real>(y1);
        real error = 0.0;
        real deltaerr = fabs(deltay / deltax);

        int y = y1;
        int x = x1;

        int ye = y2;
        int xe = x2;

        clipBySize(xe, ye);

        int sign = (ye - static_cast<int>(y1) > 0) ? 1 : -1;

        for (; x <= xe; ++x) {
            pixelPainter_->putPixel(x, y, color);
            error += deltaerr;
            while (error >= 0.5) {
                pixelPainter_->putPixel(x, y, color);
                y += sign;
                error -= 1.0;
            }
        }
    }

protected:
    virtual void drawVerticalLine(unsigned int x, unsigned int y1, unsigned int y2, const RgbColor &color) {
        if (y1 > y2) {
            drawVerticalLine(x, y2, y1, color);
            return;
        }

        for (unsigned yi = y1; yi <= y2; ++yi)
            pixelPainter_->putPixel(x, yi, color);
    }

    virtual void drawHorizontalLine(unsigned int x1, unsigned int x2, unsigned int y, const RgbColor &color) {
        if (x1 > x2) {
            drawHorizontalLine(x2, x1, y, color);
            return;
        }

        for (unsigned xi = x1; xi <= x2; ++xi)
            pixelPainter_->putPixel(xi, y, color);
    }

    virtual void clipBySize(int &x, int &y) {
        x = std::max(0, x);
        y = std::max(0, y);
    }

private:
    PixelPainter *pixelPainter_;
};

// uses dash pattern specified in pixels
class DashedLinePainterForPixels : public LinePainter {
public:
    DashedLinePainterForPixels(PixelPainter &pixelPainter, const LineDashPattern &pattern) : pixelPainter_(
            &pixelPainter), pattern_(pattern) {
    }

    // uses Bresenham's line algorithm
    virtual void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        checkConfig();
        if (x1 > x2) {
            drawLine(x2, y2, x1, y1, color);
            return;
        } else if (x1 == x2) {
            drawVerticalLine(x1, y1, y2, color);
            return;
        } else if (y1 == y2) {
            drawHorizontalLine(x1, x2, y1, color);
            return;
        }

        typedef float real;
        real deltax = static_cast<real>(x2) - static_cast<real>(x1);
        real deltay = static_cast<real>(y2) - static_cast<real>(y1);
        real error = 0.0;
        real deltaerr = fabs(deltay / deltax);

        int y = y1;
        int x = x1;

        int ye = y2;
        int xe = x2;

        clipBySize(xe, ye);

        bool dash = true;
        int xp = x1, yp = y1;
        size_t patternPos = 0;
        int patternDist = math_utils::square(pattern_[patternPos]);
        int dashDistSqr;

        int sign = (ye - static_cast<int>(y1) > 0) ? 1 : -1;

        for (; x <= xe; ++x) {
            dashDistSqr = math_utils::square(x - xp) + math_utils::square(y - yp);
            if (dashDistSqr > patternDist) {
                nextDash(x, y, dash, patternPos, xp, yp, patternDist);
            }

            if (dash)
                pixelPainter_->putPixel(x, y, color);

            error += deltaerr;

            while (error >= 0.5) {
                if (dash)
                    pixelPainter_->putPixel(x, y, color);

                y += sign;

                error -= 1.0;

                dashDistSqr = math_utils::square(x - xp) + math_utils::square(y - yp);
                if (dashDistSqr > patternDist) {
                    nextDash(x, y, dash, patternPos, xp, yp, patternDist);
                }
            }
        }
    }

protected:
    void nextDash(int x, int y, bool &dash, size_t &patternPos, int &xp, int &yp, int &patternDist) {
        if (patternPos + 1 < pattern_.size()) {
            patternPos++;
            dash = !dash;
        } else {
            patternPos = 0;
            dash = true;
        }
        xp = x;
        yp = y;
        patternDist = math_utils::square(pattern_[patternPos]);
    }

    virtual void checkConfig() {
        if (pattern_.empty())
            pattern_.push_back(100);
    }

    virtual void drawVerticalLine(unsigned int x, unsigned int y1, unsigned int y2, const RgbColor &color) {
        if (y1 > y2) {
            drawVerticalLine(x, y2, y1, color);
            return;
        }

        bool dash = true;
        int xp = x, yp = y1;
        size_t patternPos = 0;
        int patternDist = math_utils::square(pattern_[patternPos]);
        int dashDistSqr;

        for (unsigned yi = y1; yi <= y2; ++yi) {
            if (dash) {
                pixelPainter_->putPixel(x, yi, color);
            }

            dashDistSqr = math_utils::square(x - xp) + math_utils::square(yi - yp);
            if (dashDistSqr > patternDist) {
                nextDash(x, yi, dash, patternPos, xp, yp, patternDist);
            }
        }
    }

    virtual void drawHorizontalLine(unsigned int x1, unsigned int x2, unsigned int y, const RgbColor &color) {
        if (x1 > x2) {
            drawHorizontalLine(x2, x1, y, color);
            return;
        }

        bool dash = true;
        int xp = x1, yp = y;
        size_t patternPos = 0;
        int patternDist = math_utils::square(pattern_[patternPos]);
        int dashDistSqr;

        for (unsigned xi = x1; xi <= x2; ++xi) {
            if (dash) {
                pixelPainter_->putPixel(xi, y, color);
            }

            dashDistSqr = math_utils::square(xi - xp) + math_utils::square(y - yp);
            if (dashDistSqr > patternDist) {
                nextDash(xi, y, dash, patternPos, xp, yp, patternDist);
            }
        }
    }

    virtual void clipBySize(int &x, int &y) {
        x = std::max(0, x);
        y = std::max(0, y);
    }

private:
    PixelPainter *pixelPainter_;
    LineDashPattern pattern_;
};

class RectPainterForPixels : public RectPainter {
public:
    RectPainterForPixels(PixelPainter &pixelPainter) : pixelPainter_(&pixelPainter), linePainter_(pixelPainter),
                                                       usedLinePainter_(linePainter_) {}

    RectPainterForPixels(PixelPainter &pixelPainter, LinePainter &linePainter) : pixelPainter_(&pixelPainter),
                                                                                 linePainter_(pixelPainter),
                                                                                 usedLinePainter_(linePainter) {}

    virtual ~RectPainterForPixels() {}

    virtual void drawFull(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        if (x2 < x1 || y2 < y1)
            drawFull(x2, y2, x1, y1, color);

        for (unsigned int yi = y1; yi <= y2; ++yi)
            for (unsigned int xi = x1; xi <= x2; ++xi)
                pixelPainter_->putPixel(xi, yi, color);
    }

    virtual void drawEmpty(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        usedLinePainter_.drawLine(x1, y1, x2, y1, color);
        usedLinePainter_.drawLine(x1, y2, x2, y2, color);
        usedLinePainter_.drawLine(x1, y1, x1, y2, color);
        usedLinePainter_.drawLine(x2, y1, x2, y2, color);
    }

private:
    PixelPainter *pixelPainter_;
    LinePainterForPixels linePainter_;
    LinePainter &usedLinePainter_;
};

class CirclePainterForPixels : public CirclePainter {
public:
    CirclePainterForPixels(PixelPainter &pixelPainter) : pixelPainter_(pixelPainter) {}

    // naive algorithm
    virtual void drawFull(unsigned int x, unsigned int y, unsigned int r, const RgbColor &color) {
        int ir = static_cast<int>(r);
        int r2 = r * r;
        for (int yi = -ir; yi <= ir; yi++) {
            int yi2 = yi * yi;
            int yc = r2 - yi2;
            for (int xi = -ir; xi <= ir; xi++)
                if (xi * xi <= yc)
                    pixelPainter_.putPixel(x + xi, y + yi, color);
        }
    }

    // Midpoint circle algorithm
    virtual void drawEmpty(unsigned int x0, unsigned int y0, unsigned int r, const RgbColor &color) {
        int x = r;
        int y = 0;
        int decisionOver2 = 1 - x;

        while (y <= x) {
            pixelPainter_.putPixel(x + x0, y + y0, color);
            pixelPainter_.putPixel(y + x0, x + y0, color);
            pixelPainter_.putPixel(-x + x0, y + y0, color);
            pixelPainter_.putPixel(-y + x0, x + y0, color);
            pixelPainter_.putPixel(-x + x0, -y + y0, color);
            pixelPainter_.putPixel(-y + x0, -x + y0, color);
            pixelPainter_.putPixel(x + x0, -y + y0, color);
            pixelPainter_.putPixel(y + x0, -x + y0, color);
            y++;

            if (decisionOver2 <= 0) {
                decisionOver2 += 2 * y + 1;
            } else {
                x--;
                decisionOver2 += 2 * (y - x) + 1;
            }
        }
    }

    virtual void drawFullWithBorder(unsigned int x, unsigned int y, unsigned int r, unsigned int borderWidth,
                                    const RgbColor &fillColor, const RgbColor &borderColor) {
        drawFull(x, y, r, borderColor);
        if (borderWidth < r)
            drawFull(x, y, r - borderWidth, fillColor);
    }

private:
    PixelPainter &pixelPainter_;
};

class ThickLinePainterForPixels : public LinePainter {
public:
    ThickLinePainterForPixels(PixelPainter &pixelPainter, float lineWidth, int pointCount = -1) : lineWidth_(lineWidth),
                                                                                                  pointCount_(
                                                                                                          pointCount),
                                                                                                  pixelPainter_(
                                                                                                          pixelPainter),
                                                                                                  circlePainter_(
                                                                                                          pixelPainter),
                                                                                                  usedCirclePainter_(
                                                                                                          circlePainter_) {}

    ThickLinePainterForPixels(PixelPainter &pixelPainter, CirclePainter &circlePainter, float lineWidth,
                              int pointCount = -1) : lineWidth_(lineWidth), pointCount_(pointCount),
                                                     pixelPainter_(pixelPainter), circlePainter_(pixelPainter),
                                                     usedCirclePainter_(circlePainter) {}

    virtual void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        int dx = x2 - x1;
        int dy = y2 - y1;

        unsigned int lineWidthD2 = static_cast<unsigned>(round(lineWidth_ / 2));

        int cnt = calcPointCount(x1, y1, x2, y2);

        float fdx = static_cast<float>(dx) / cnt;
        float fdy = static_cast<float>(dy) / cnt;

        float x = static_cast<float>(x1);
        float y = static_cast<float>(y1);

        for (int i = 0, epos = cnt; i < epos; ++i) {
            usedCirclePainter_.drawFull(static_cast<unsigned int>(round(x)), static_cast<unsigned int>(round(y)),
                                        lineWidthD2, color);
            x += fdx;
            y += fdy;
        }

    }

protected:
    virtual int calcPointCount(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
        int dx = x2 - x1;
        int dy = y2 - y1;

        unsigned int lineWidthD2 = static_cast<unsigned>(round(lineWidth_ / 2));

        int cnt;
        if (pointCount_ > 0)
            cnt = pointCount_;
        else {
            cnt = static_cast<int>(round(sqrt(std::max(1, math_utils::square(math_utils::iabs(dx))) *
                                              std::max(1, math_utils::square(math_utils::iabs(dy))))));
            cnt = cnt / lineWidthD2;
        }

        return cnt;
    }

protected:
    float lineWidth_;
    int pointCount_;
    PixelPainter &pixelPainter_;
    CirclePainterForPixels circlePainter_;
    CirclePainter &usedCirclePainter_;
};

class DashedThickLinePainterForPixels : public LinePainter {
public:
    DashedThickLinePainterForPixels(PixelPainter &pixelPainter, const LineDashPattern &pattern, float lineWidth,
                                    int pointCount = -1) : lineWidth_(lineWidth), pointCount_(pointCount),
                                                           pixelPainter_(pixelPainter), circlePainter_(pixelPainter),
                                                           usedCirclePainter_(circlePainter_), pattern_(pattern) {}

    DashedThickLinePainterForPixels(PixelPainter &pixelPainter, CirclePainter &circlePainter,
                                    const LineDashPattern &pattern, float lineWidth, int pointCount = -1) : lineWidth_(
            lineWidth), pointCount_(pointCount), pixelPainter_(pixelPainter), circlePainter_(pixelPainter),
                                                                                                            usedCirclePainter_(
                                                                                                                    circlePainter),
                                                                                                            pattern_(
                                                                                                                    pattern) {}

    virtual void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        checkConfig();

        int dx = x2 - x1;
        int dy = y2 - y1;

        float lineWidthD2f = lineWidth_ / 2.0f;
        unsigned int lineWidthD2 = static_cast<unsigned>(round(lineWidthD2f));

        int cnt = calcPointCount(x1, y1, x2, y2);

        float fdx = static_cast<float>(dx) / cnt;
        float fdy = static_cast<float>(dy) / cnt;

        float x = static_cast<float>(x1);
        float y = static_cast<float>(y1);

        bool dash = true;
        int xp = x1, yp = y1;
        size_t patternPos = 0;
        int patternDist = pattern_[patternPos];
        float dashDistSqr;

        int sx, sy;

        for (int i = 0, epos = cnt; i < epos; ++i) {
            sx = static_cast<unsigned int>(round(x));
            sy = static_cast<unsigned int>(round(y));

            if (dash) {
                usedCirclePainter_.drawFull(sx, sy, lineWidthD2, color);
            }

            dashDistSqr = sqrt(math_utils::square(sx - xp) + math_utils::square(sy - yp));
            if (
                    (dash && (round(dashDistSqr + lineWidthD2f) > patternDist))
                    ||
                    (!dash && (round(dashDistSqr - lineWidthD2f) > patternDist))
                    ) {
                nextDash(sx, sy, dash, patternPos, xp, yp, patternDist);
            }

            x += fdx;
            y += fdy;
        }

    }

protected:
    virtual int calcPointCount(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
        int dx = x2 - x1;
        int dy = y2 - y1;

        unsigned int lineWidthD2 = static_cast<unsigned>(round(lineWidth_ / 2));

        int cnt;
        if (pointCount_ > 0)
            cnt = pointCount_;
        else {
            cnt = static_cast<int>(round(sqrt(std::max(1, math_utils::square(math_utils::iabs(dx))) *
                                              std::max(1, math_utils::square(math_utils::iabs(dy))))));
            cnt = cnt / lineWidthD2;
        }

        return cnt;
    }

    virtual void checkConfig() {
        if (pattern_.empty())
            pattern_.push_back(100);
    }

    void nextDash(int x, int y, bool &dash, size_t &patternPos, int &xp, int &yp, int &patternDist) {
        if (patternPos + 1 < pattern_.size()) {
            patternPos++;
            dash = !dash;
        } else {
            patternPos = 0;
            dash = true;
        }
        xp = x;
        yp = y;
        patternDist = pattern_[patternPos];
    }

protected:
    float lineWidth_;
    int pointCount_;
    PixelPainter &pixelPainter_;
    CirclePainterForPixels circlePainter_;
    CirclePainter &usedCirclePainter_;
    LineDashPattern pattern_;
};

// draw pixels using circles
class ThickPixelPainter : public PixelPainter {
public:
    ThickPixelPainter(PixelPainter &pixelPainter, int pixelWidth) : pixelPainter_(pixelPainter),
                                                                    circlePainter_(pixelPainter),
                                                                    radius_(static_cast<int>(std::max(1.0f,
                                                                                                      roundf(static_cast<float>(pixelWidth) /
                                                                                                             2)))) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        circlePainter_.drawFull(x, y, radius_, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        pixelPainter_.getPixel(x, y, output);
    }

protected:
    PixelPainter &pixelPainter_;
    CirclePainterForPixels circlePainter_;
    int radius_;
};

class EllipsePainterForPixels : public EllipsePainter {
public:
    EllipsePainterForPixels(PixelPainter &pixelPainter) : pixelPainter_(pixelPainter) {}

    // naive algorithm
    virtual void drawFull(unsigned int x, unsigned int y, unsigned int rx, unsigned int ry, const RgbColor &color) {
        int irx = static_cast<int>(rx);
        int iry = static_cast<int>(ry);
        long int irx2 = irx * irx;
        long int iry2 = iry * iry;
        long int irxy2 = irx2 * iry2;

        for (int yi = -iry; yi <= iry; yi++) {
            int yi2 = yi * yi;
            long int yc = irxy2 - yi2 * irx2;
            for (int xi = -irx; xi <= irx; xi++)
                if (iry2 * xi * xi <= yc)
                    pixelPainter_.putPixel(x + xi, y + yi, color);
        }
    }

    // Bresenham's ellipse drawing algorithm
    virtual void drawEmpty(unsigned int x0, unsigned int y0, unsigned int rx, unsigned int ry, const RgbColor &color) {
        int a2 = rx * rx;
        int b2 = ry * ry;
        int fa2 = 4 * a2, fb2 = 4 * b2;
        int xi, yi, sigma;

        /* first half */
        for (xi = 0, yi = ry, sigma = 2 * b2 + a2 * (1 - 2 * ry); b2 * xi <= a2 * yi; xi++) {
            pixelPainter_.putPixel(x0 + xi, y0 + yi, color);
            pixelPainter_.putPixel(x0 - xi, y0 + yi, color);
            pixelPainter_.putPixel(x0 + xi, y0 - yi, color);
            pixelPainter_.putPixel(x0 - xi, y0 - yi, color);

            if (sigma >= 0) {
                sigma += fa2 * (1 - yi);
                yi--;
            }

            sigma += b2 * ((4 * xi) + 6);
        }

        /* second half */
        for (xi = rx, yi = 0, sigma = 2 * a2 + b2 * (1 - 2 * rx); a2 * yi <= b2 * xi; yi++) {
            pixelPainter_.putPixel(x0 + xi, y0 + yi, color);
            pixelPainter_.putPixel(x0 - xi, y0 + yi, color);
            pixelPainter_.putPixel(x0 + xi, y0 - yi, color);
            pixelPainter_.putPixel(x0 - xi, y0 - yi, color);

            if (sigma >= 0) {
                sigma += fb2 * (1 - xi);
                xi--;
            }

            sigma += a2 * ((4 * yi) + 6);
        }
    }

private:
    PixelPainter &pixelPainter_;
};

class BSplinePainterForPixels : public BSplinePainter {
public:
    BSplinePainterForPixels(PixelPainter &pixelPainter)
            : BSplinePainter(), pixelPainter_(pixelPainter), linePainter_(pixelPainter),
              usedLinePainter_(linePainter_) {}

    BSplinePainterForPixels(PixelPainter &pixelPainter, LinePainter &linePainter)
            : BSplinePainter(), pixelPainter_(pixelPainter), linePainter_(pixelPainter),
              usedLinePainter_(linePainter) {}

    virtual void draw(const std::vector<Point> &points, int divisions, const RgbColor &color) {
        size_t ps = points.size();
        size_t offset = 0;
        std::vector<Point> pixels;
        Point p;
        Point p1;
        bool pixelSaved = false;

        while (ps >= offset + 4) {
            cubic_spline_utils::bsp(points[offset], points[offset + 1], points[offset + 2], points[offset + 3],
                                    divisions, pixels);

            for (int i = 0, epos = pixels.size(); i < epos; ++i) {
                p = pixels[i];

                if (!pixelSaved) {
                    pixelSaved = true;
                    pixelPainter_.putPixel(p.x, p.y, color);
                } else {
                    usedLinePainter_.drawLine(p1.x, p1.y, p.x, p.y, color);
                }

                p1 = p;
            }

            offset++;
        }
    }

protected:

private:
    PixelPainter &pixelPainter_;
    LinePainterForPixels linePainter_;
    LinePainter &usedLinePainter_;
};

class DashedBSplinePainterForPixels : public BSplinePainter {
public:
    DashedBSplinePainterForPixels(PixelPainter &pixelPainter, const LineDashPattern &pattern)
            : BSplinePainter(), pixelPainter_(pixelPainter), linePainter_(pixelPainter), usedLinePainter_(linePainter_),
              pattern_(pattern) {}

    DashedBSplinePainterForPixels(PixelPainter &pixelPainter, LinePainter &linePainter, const LineDashPattern &pattern)
            : BSplinePainter(), pixelPainter_(pixelPainter), linePainter_(pixelPainter), usedLinePainter_(linePainter),
              pattern_(pattern) {}

    virtual void draw(const std::vector<Point> &points, int divisions, const RgbColor &color) {
        checkConfig();

        size_t ps = points.size();
        size_t offset = 0;
        std::vector<Point> pixels;
        Point p;
        Point p1;
        bool pixelSaved = false;

        bool dash = true;
        size_t patternPos = 0;
        int patternDist = pattern_[patternPos];
        float dashDist = 0.0f;

        while (ps >= offset + 4) {
            cubic_spline_utils::bsp(points[offset], points[offset + 1], points[offset + 2], points[offset + 3],
                                    divisions, pixels);

            for (int i = 0, epos = pixels.size(); i < epos; ++i) {
                p = pixels[i];

                if (!pixelSaved) {
                    pixelSaved = true;
                    pixelPainter_.putPixel(p.x, p.y, color);
                } else {
                    if (dash) {
                        usedLinePainter_.drawLine(p1.x, p1.y, p.x, p.y, color);
                    }
                    dashDist += sqrt(math_utils::square(p.x - p1.x) + math_utils::square(p.y - p1.y));
                    if (dashDist > patternDist) {
                        nextDash(dash, patternPos, patternDist);
                        dashDist = 0.0f;
                    }
                }

                p1 = p;
            }

            offset++;
        }
    }

protected:
    virtual void checkConfig() {
        if (pattern_.empty())
            pattern_.push_back(100);
    }

    void nextDash(bool &dash, size_t &patternPos, int &patternDist) {
        if (patternPos + 1 < pattern_.size()) {
            patternPos++;
            dash = !dash;
        } else {
            patternPos = 0;
            dash = true;
        }
        patternDist = pattern_[patternPos];
    }

private:
    PixelPainter &pixelPainter_;
    LinePainterForPixels linePainter_;
    LinePainter &usedLinePainter_;
    LineDashPattern pattern_;
};

class BackgroundPainterForPixelPainter : public BackgroundPainter {
public:
    BackgroundPainterForPixelPainter(PixelPainter &pixelPainter, const Point &canvasSize) : pixelPainter_(
            &pixelPainter), canvasSize_(canvasSize) {}

    virtual ~BackgroundPainterForPixelPainter() {}

    virtual void paint(const RgbColor &color) {
        for (unsigned int y = 0, eposy = canvasSize_.y; y < eposy; ++y)
            for (unsigned int x = 0, eposx = canvasSize_.x; x < eposx; ++x)
                pixelPainter_->putPixel(x, y, color);
    }

private:
    PixelPainter *pixelPainter_;
    Point canvasSize_;
};

class TrianglePainterForPixels : public TrianglePainter {
public:
    TrianglePainterForPixels(PixelPainter &pixelPainter) : pixelPainter_(&pixelPainter), linePainter_(pixelPainter),
                                                           usedLinePainter_(linePainter_) {}

    TrianglePainterForPixels(PixelPainter &pixelPainter, LinePainter &linePainter) : pixelPainter_(&pixelPainter),
                                                                                     linePainter_(pixelPainter),
                                                                                     usedLinePainter_(linePainter) {}

    // algorithm from: http://forum.devmaster.net/t/advanced-rasterization/6145
    // requires points to be sorted counter-clockwise order
    virtual void drawFull(const Point &p1, const Point &p2, const Point &p3, const RgbColor &color) {
        typedef int pixpos;
        pixpos x1 = p1.x;
        pixpos x2 = p2.x;
        pixpos x3 = p3.x;

        pixpos y1 = p1.y;
        pixpos y2 = p2.y;
        pixpos y3 = p3.y;

        int minx = static_cast<int>(std::min(x1, std::min(x2, x3)));
        int maxx = static_cast<int>(std::max(x1, std::max(x2, x3)));
        int miny = static_cast<int>(std::min(y1, std::min(y2, y3)));
        int maxy = static_cast<int>(std::max(y1, std::max(y2, y3)));

        pixpos dx12 = x1 - x2;
        pixpos dx23 = x2 - x3;
        pixpos dx31 = x3 - x1;

        pixpos dy12 = y1 - y2;
        pixpos dy23 = y2 - y3;
        pixpos dy31 = y3 - y1;

        pixpos dx12i = dx12 * (miny - y1);
        pixpos dx23i = dx23 * (miny - y2);
        pixpos dx31i = dx31 * (miny - y3);

        for (int y = miny; y <= maxy; y++) {
            pixpos da12i = -dy12 * (minx - x1) + dx12i;
            pixpos da23i = -dy23 * (minx - x2) + dx23i;
            pixpos da31i = -dy31 * (minx - x3) + dx31i;

            for (int x = minx; x <= maxx; x++) {
                if (da12i >= 0 &&
                    da23i >= 0 &&
                    da31i >= 0) {
                    pixelPainter_->putPixel(x, y, color);
                }

                da12i -= dy12;
                da23i -= dy23;
                da31i -= dy31;
            } // for x

            dx12i += dx12;
            dx23i += dx23;
            dx31i += dx31;
        } // for y

    }

    virtual void drawEmpty(const Point &p1, const Point &p2, const Point &p3, const RgbColor &color) {
        usedLinePainter_.drawLine(p1.x, p1.y, p2.x, p2.y, color);
        usedLinePainter_.drawLine(p2.x, p2.y, p3.x, p3.y, color);
        usedLinePainter_.drawLine(p3.x, p3.y, p1.x, p1.y, color);
    }

private:
    PixelPainter *pixelPainter_;
    LinePainterForPixels linePainter_;
    LinePainter &usedLinePainter_;
};

class FloodFillPainterForPixels : public FloodFillPainter {
    struct PixelStatus {
        int x;
        int y;
        char checkMask;
    };

    enum {
        CheckUp = 1,
        CheckDown = 2,
        CheckLeft = 4,
        CheckRight = 8,
        CheckAll = CheckUp + CheckDown + CheckLeft + CheckRight
    };
public:
    FloodFillPainterForPixels(PixelPainter &pixelPainter, const Point &canvasSize) : pixelPainter_(&pixelPainter),
                                                                                     canvasSize_(canvasSize) {}

    virtual void fill(const Point &p, const RgbColor &color) {
        RgbColor initialColor;
        pixelPainter_->getPixel(p.x, p.y, initialColor);

        fillFromPixel(p.x, p.y, color, initialColor);
    }

protected:
    // 4-directions non-recursive flood fill
    void fillFromPixel(int x, int y, const RgbColor &newColor, const RgbColor &initialColor) {
        std::vector<PixelStatus> pixels;
        RgbColor c;

        PixelStatus s;
        PixelStatus s1;
        s.x = x;
        s.y = y;
        s.checkMask = calcCheckMask(x, y);

        pixels.push_back(s);

        while (!pixels.empty()) {
            s = pixels.back();
            pixels.pop_back();

            pixelPainter_->getPixel(s.x, s.y, c);

            if (c == initialColor) {
                pixelPainter_->putPixel(s.x, s.y, newColor);

                if (((s.checkMask & CheckLeft) != 0) && (s.x >= 0)) {
                    s1.x = s.x - 1;
                    s1.y = s.y;
                    s1.checkMask = calcCheckMask(s1.x, s1.y) & s.checkMask;
                    pixels.push_back(s1);
                }

                if (((s.checkMask & CheckRight) != 0) && (s.x + 1 < canvasSize_.x)) {
                    s1.x = s.x + 1;
                    s1.y = s.y;
                    s1.checkMask = calcCheckMask(s1.x, s1.y) & s.checkMask;
                    pixels.push_back(s1);
                }

                if (((s.checkMask & CheckUp) != 0) && (s.y >= 0)) {
                    s1.x = s.x;
                    s1.y = s.y - 1;
                    s1.checkMask = calcCheckMask(s1.x, s1.y) & s.checkMask;
                    pixels.push_back(s1);
                }

                if (((s.checkMask & CheckDown) != 0) && (s.y + 1 < canvasSize_.y)) {
                    s1.x = s.x;
                    s1.y = s.y + 1;
                    s1.checkMask = calcCheckMask(s1.x, s1.y) & s.checkMask;
                    pixels.push_back(s1);
                }
            }
        }

    }

    char calcCheckMask(int x, int y) {
        char checkMask = 0;
        if (x > 0)
            checkMask += CheckLeft;
        if (x + 1 < canvasSize_.x)
            checkMask += CheckRight;
        if (y > 0)
            checkMask += CheckUp;
        if (y + 1 < canvasSize_.y)
            checkMask += CheckDown;
        return checkMask;
    }

private:
    PixelPainter *pixelPainter_;
    Point canvasSize_;
};

#endif
