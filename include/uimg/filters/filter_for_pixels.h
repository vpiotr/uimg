#ifndef __UIMG_FILTER_4_PIX_H__
#define __UIMG_FILTER_4_PIX_H__

#include <algorithm>

#include "uimg/filters/filter_base.h"
#include "uimg/painters/painter_base.h"
#include "uimg/utils/math_utils.h"

// Utility class for flip operations
class FlipUtils {
public:
    // Calculates flip position based on offset and flip mode
    static int flipPos(unsigned int value, unsigned int offset, bool evenFlip) {
        int result;
        if (evenFlip) {
            if (value <= offset) {
                int dx = static_cast<int>(offset) - static_cast<int>(value);
                result = static_cast<int>(offset) + dx - 2;
            } else {
                int dx = static_cast<int>(value) - static_cast<int>(offset) - 1;
                result = static_cast<int>(offset) - dx - 2;
            }
        } else {
            int dx = static_cast<int>(offset) - static_cast<int>(value);
            result = static_cast<int>(offset) + dx;
        }
        return result;
    }
};

// just copy pixels to/from target
class CopyingFilter : public PixelFilter {
public:
    CopyingFilter(PixelPainter &target) : PixelFilter(target) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        getTarget().putPixel(x, y, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        getTarget().getPixel(x, y, output);
    }
};

// position rotation filter - rotation given by discrete values (-180,-90,0,+90,180)
class DiscreteRotationFilter : public PixelFilter {
public:
    DiscreteRotationFilter(PixelPainter &target, const Point &offset, int rotationValue) :
    	PixelFilter(target), offset_(offset), rotationValue_(rotationValue) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        recalculatePos(x, y);
        getTarget().putPixel(x, y, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        recalculatePos(x, y);
        getTarget().getPixel(x, y, output);
    }
protected:
    void recalculatePos(unsigned int &x, unsigned int &y) {
        switch (rotationValue_)
        {            case -90: {
                int xv = static_cast<int>(x) - offset_.x;
                int yv = static_cast<int>(y) - offset_.y;
                x = static_cast<unsigned int>(offset_.x + yv);
                y = static_cast<unsigned int>(offset_.y - xv);
                break;
            }            case 90: {
                int xv = static_cast<int>(x) - offset_.x;
                int yv = static_cast<int>(y) - offset_.y;
                x = static_cast<unsigned int>(offset_.x - yv);
                y = static_cast<unsigned int>(offset_.y + xv);
                break;
            }            case -180:
            case 180:
            {
                int xv = static_cast<int>(x) - offset_.x;
                int yv = static_cast<int>(y) - offset_.y;
                x = static_cast<unsigned int>(offset_.x - xv);
                y = static_cast<unsigned int>(offset_.y - yv);
                break;
            }
            default: {
                // does nothing
            }
        }
    }
private:
    Point offset_;
    int rotationValue_;
};

// horizontal flip filter
class HorizontalFlipFilter : public PixelFilter {
public:
    HorizontalFlipFilter(PixelPainter &target, const Point &offset, bool evenFlip) : PixelFilter(target), offset_(offset), evenFlip_(evenFlip) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        recalculatePos(x, y);
        getTarget().putPixel(x, y, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        recalculatePos(x, y);
        getTarget().getPixel(x, y, output);
    }

    Point getPixelPos(unsigned int x, unsigned int y) {
        recalculatePos(x, y);
        return Point(static_cast<int>(x), static_cast<int>(y));
    }

protected:    
    void recalculatePos(unsigned int &x, [[maybe_unused]] unsigned int &y) {
        x = static_cast<unsigned int>(FlipUtils::flipPos(x, static_cast<unsigned int>(offset_.x), evenFlip_));
    }

private:
    Point offset_;
    bool evenFlip_;
};

// vertical flip filter
class VerticalFlipFilter : public PixelFilter {
public:
    VerticalFlipFilter(PixelPainter &target, const Point &offset, bool evenFlip) : PixelFilter(target), offset_(offset), evenFlip_(evenFlip) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        recalculatePos(x, y);
        getTarget().putPixel(x, y, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        recalculatePos(x, y);
        getTarget().getPixel(x, y, output);
    }

protected:    void recalculatePos([[maybe_unused]] unsigned int &x, unsigned int &y) {
        y = static_cast<unsigned int>(FlipUtils::flipPos(y, static_cast<unsigned int>(offset_.y), evenFlip_));
    }
private:
    Point offset_;
    bool evenFlip_;
};

// transparency filter
class TransparencyFilter : public PixelFilter {
public:
    TransparencyFilter(PixelPainter &target, const RgbColor &transparencyColor) : PixelFilter(target), transparencyColor_(transparencyColor) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        if (color != transparencyColor_)
            getTarget().putPixel(x, y, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        getTarget().getPixel(x, y, output);
    }

private:
    RgbColor transparencyColor_;
};

// mask different filter
class MaskDiffFilter : public PixelFilter {
public:
    MaskDiffFilter(PixelPainter &target, const PixelSource &maskSource, const RgbColor &ignoreColor) :
    	PixelFilter(target), ignoreColor_(ignoreColor), maskSource_(maskSource) {}    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        RgbColor maskColor;
        maskColor = maskSource_.getPixel(Point(static_cast<int>(x), static_cast<int>(y)));
        if (maskColor != ignoreColor_)
            getTarget().putPixel(x, y, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        getTarget().getPixel(x, y, output);
    }

private:
    RgbColor ignoreColor_;
    const PixelSource &maskSource_;
};

// mask equal filter
class MaskEqFilter : public PixelFilter {
public:
    MaskEqFilter(PixelPainter &target, const PixelSource &maskSource, const RgbColor &maskColor) :
    	PixelFilter(target), maskColor_(maskColor), maskSource_(maskSource) {}    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        RgbColor maskColor;
        maskColor = maskSource_.getPixel(Point(static_cast<int>(x), static_cast<int>(y)));
        if (maskColor == maskColor_)
            getTarget().putPixel(x, y, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        getTarget().getPixel(x, y, output);
    }

private:
    RgbColor maskColor_;
    const PixelSource &maskSource_;
};

// alpha blend (transparency) filter
class AlphaFilter : public PixelFilter {
public:
    AlphaFilter(PixelPainter &target, float alpha) : PixelFilter(target), alpha_(std::min(1.0f, std::max(0.0f, alpha))) {}    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        RgbColor calcColor;
        getTarget().getPixel(x, y, calcColor);
        calcColor.red = static_cast<unsigned char>(std::min(255, static_cast<int>(round(alpha_ * color.red + (1.0f - alpha_) * calcColor.red))));
        calcColor.green = static_cast<unsigned char>(std::min(255, static_cast<int>(round(alpha_ * color.green + (1.0f - alpha_) * calcColor.green))));
        calcColor.blue = static_cast<unsigned char>(std::min(255, static_cast<int>(round(alpha_ * color.blue + (1.0f - alpha_) * calcColor.blue))));
        getTarget().putPixel(x, y, calcColor);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        getTarget().getPixel(x, y, output);
    }

private:
    float alpha_;
};

// add (a,b) pixels to calculate new position
class OffsetFilter : public PixelFilter {
public:
    OffsetFilter(PixelPainter &target, const Point &offset) : PixelFilter(target), offset_(offset) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        if (recalculatePos(x, y))
            getTarget().putPixel(x, y, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        if (recalculatePos(x, y))
            getTarget().getPixel(x, y, output);
        else
            output.clear();
    }

protected:    bool recalculatePos(unsigned int &x, unsigned int &y) {
        // handle shift below zero
        if (offset_.x < 0) {
            if (static_cast<unsigned int>(-offset_.x) > x) {
                x = 0;
                return false;
            }
        }

        x = static_cast<unsigned int>(static_cast<int>(x) + offset_.x);

        // handle shift below zero
        if (offset_.y < 0) {
            if (static_cast<unsigned int>(-offset_.y) > y) {
                y = 0;
                return false;
            }
        }

        y = static_cast<unsigned int>(static_cast<int>(y) + offset_.y);
        return true;
    }

private:
    Point offset_;
};

// clip operations so they are not performed if position is not available
class ClipFilter : public PixelFilter {
public:
    ClipFilter(PixelPainter &target, const RectInclusive &clipWindow) : PixelFilter(target), clipWindow_(clipWindow) {}    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        if (checkPos(static_cast<int>(x), static_cast<int>(y)))
            getTarget().putPixel(x, y, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        if (checkPos(static_cast<int>(x), static_cast<int>(y)))
            getTarget().getPixel(x, y, output);
        else
            output.clear();
    }

protected:
    bool checkPos(int x, int y) {
        if (x < clipWindow_.x1 || x > clipWindow_.x2)
            return false;
        if (y < clipWindow_.y1 || y > clipWindow_.y2)
            return false;

        return true;
    }

private:
    RectInclusive clipWindow_;
};

// pixel spread filter
class PixelSpreadFilter : public PixelFilter {
public:
    PixelSpreadFilter(PixelPainter &target, const Point &offset, const Point &zoom) : PixelFilter(target), offset_(offset), zoom_(zoom) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        recalculatePos(x, y);
        getTarget().putPixel(x, y, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        recalculatePos(x, y);
        getTarget().getPixel(x, y, output);
    }

protected:
    void recalculatePos(unsigned int &x, unsigned int &y) {
        int dx = static_cast<int>(x) - offset_.x;
        int dy = static_cast<int>(y) - offset_.y;
        x = static_cast<unsigned int>(offset_.x + dx / zoom_.x);
        y = static_cast<unsigned int>(offset_.y + dy / zoom_.y);
    }
private:
    Point offset_;
    Point zoom_;
};

// pixel zoom filter
class PixelZoomFilter : public PixelFilter {
public:
    PixelZoomFilter(PixelPainter &target, const Point &offset, const Point &zoom) : PixelFilter(target), offset_(offset), zoom_(zoom) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        unsigned int x2 = x + 1;
        unsigned int y2 = y + 1;
        recalculatePos(x, y);
        recalculatePos(x2, y2);
        for (unsigned int yi = y; yi <= y2; ++yi)
            for (unsigned int xi = x; xi <= x2; ++xi)
                getTarget().putPixel(xi, yi, color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        recalculatePos(x, y);
        getTarget().getPixel(x, y, output);
    }

protected:
    void recalculatePos(unsigned int &x, unsigned int &y) {
        int dx = static_cast<int>(x) - offset_.x;
        int dy = static_cast<int>(y) - offset_.y;
        x = static_cast<unsigned int>(offset_.x + zoom_.x * dx);
        y = static_cast<unsigned int>(offset_.y + zoom_.y * dy);
    }
private:
    Point offset_;
    Point zoom_;
};

// Linear gradient filter, 2 colors
class GradientFilter2C : public PixelFilter {
public:    GradientFilter2C(PixelPainter &target, const Point &startPoint, const Point &endPoint, const RgbColor &startColor, const RgbColor &endColor) :
        PixelFilter(target),
        startPoint_(startPoint),
        endPoint_(endPoint),
        startColor_(startColor),
        endColor_(endColor),
        gradDist_(static_cast<float>(std::max(1, calcPointDistanceSqr(startPoint, endPoint)))),
        gradDistRoot_(static_cast<float>(sqrt(gradDist_))),
        dx_(endPoint_.x - startPoint_.x),
        dy_(endPoint_.y - startPoint_.y)
    {    }

    virtual void putPixel(unsigned int x, unsigned int y, [[maybe_unused]] const RgbColor &color) {
        Point p1 = calcPointOnLine(x, y);
        float distance1 = static_cast<float>(calcPointDistanceSqr(p1, startPoint_));
        float distance2 = static_cast<float>(calcPointDistanceSqr(p1, endPoint_));

        if (distance1 > gradDist_) {
            if (distance1 > distance2) {
                distance1 = gradDist_;
                distance2 = 0.0f;
            }
            else {
                distance1 = 0.0f;
                distance2 = gradDist_;
            }
        }

        if (distance2 > gradDist_) {
            if (distance1 < distance2) {
                distance1 = 0.0f;
                distance2 = gradDist_;
            }
            else {
                distance1 = gradDist_;
                distance2 = 0.0f;
            }
        }

        double c1 = sqrt(distance1) / gradDistRoot_;
        double c2 = 1.0 - c1;

        RgbColor rcolor = mixColors(startColor_, c1, endColor_, c2);

        getTarget().putPixel(x, y, rcolor);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        getTarget().getPixel(x, y, output);
    }
protected:    RgbColor mixColors(const RgbColor &color1, double r1, const RgbColor &color2, double r2) {
        RgbColor result;
        result.red = static_cast<unsigned char>(std::min(255, static_cast<int>(round(static_cast<float>(color1.red) * r1 + static_cast<float>(color2.red) * r2))));
        result.green = static_cast<unsigned char>(std::min(255, static_cast<int>(round(static_cast<float>(color1.green) * r1 + static_cast<float>(color2.green) * r2))));
        result.blue = static_cast<unsigned char>(std::min(255, static_cast<int>(round(static_cast<float>(color1.blue) * r1 + static_cast<float>(color2.blue) * r2))));
        return result;
    }

    // returns squared distance between points
    int calcPointDistanceSqr(const Point &p1, const Point &p2) {
        return math_utils::square(p2.x - p1.x) + math_utils::square(p2.y - p1.y);
    }    // returns point casted onto line between gradient points
    Point calcPointOnLine(unsigned int x, unsigned int y) {
        int p3x = static_cast<int>(x);
        int p3y = static_cast<int>(y);
        int ua = (p3x - startPoint_.x) * (dx_)+(p3y - startPoint_.y) * (dy_);
        int ub = math_utils::square(dx_) + math_utils::square(dy_);
        if (ub == 0)
            return startPoint_;
        float u = static_cast<float>(ua) / static_cast<float>(ub);        
        Point result;
        result.x = static_cast<int>(round(static_cast<float>(startPoint_.x) + static_cast<float>(dx_)*u));
        result.y = static_cast<int>(round(static_cast<float>(startPoint_.y) + static_cast<float>(dy_)*u));
        return result;
    }
private:
    Point startPoint_;
    Point endPoint_;
    RgbColor startColor_;
    RgbColor endColor_;
    float gradDist_;
    float gradDistRoot_;
    int dx_;
    int dy_;
};


#endif
