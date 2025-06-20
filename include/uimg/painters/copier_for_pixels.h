#ifndef __UIMG_COPIER_4_PIX_H__
#define __UIMG_COPIER_4_PIX_H__

#include <cassert>
#include <cmath>
#include <algorithm>

#include "uimg/pixels/pixel_painter.h"
#include "uimg/pixels/pixel_source.h"
#include "uimg/pixels/pixel_copier.h"

// copier using pixel by pixel operations
class ImageCopierByPixels : public PixelCopier {
public:
    ImageCopierByPixels(PixelPainter &painter) : pixelPainter_(&painter) {}

    ImageCopierByPixels() : pixelPainter_(nullptr) {}

    virtual ~ImageCopierByPixels() {}

    virtual void copyFrom(const PixelSource &src, const Point &targetPos) {
        PixelPainter *painter = checkPixelPainter();
        int offsetx = targetPos.x;
        int offsety = targetPos.y;
        for (int y = 0, eposy = src.getSize().y; y < eposy; ++y) {
            for (int x = 0, eposx = src.getSize().x; x < eposx; ++x) {
                painter->putPixel(static_cast<unsigned int>(offsetx + x), static_cast<unsigned int>(offsety + y), src.getPixel(Point(x, y)));
            }
        }
    }

    virtual void copyFrom(const PixelSource &src, const Point &targetPos, const Rect &srcPart) {
        PixelPainter *painter = checkPixelPainter();
        int offsetx = targetPos.x;
        int offsety = targetPos.y;
        for (int y = std::max(0, srcPart.y1), eposy = std::min(srcPart.y2 + 1, src.getSize().y); y < eposy; ++y) {
            for (int x = std::max(0, srcPart.x1), eposx = std::min(srcPart.x2 + 1, src.getSize().x); x < eposx; ++x) {
                painter->putPixel(static_cast<unsigned int>(offsetx + x), static_cast<unsigned int>(offsety + y), src.getPixel(Point(x, y)));
            }
        }
    }

protected:
    virtual PixelPainter *checkPixelPainter() {
        assert(pixelPainter_ != nullptr);
        return pixelPainter_;
    }

private:
    PixelPainter *pixelPainter_;
};


// Bilinear resampling copier
// based on "EECE\CS 253 Image Processing. Resizing Images", Richard Alan Peters II (2011)
class BilinearSamplingCopier : public PixelCopier {
public:
    virtual void copyFrom(const PixelSource &src, const Point &targetPos) {
        Rect fullRect;
        fullRect.topLeft({0, 0});
        fullRect.size(src.getSize());

        copyFrom(src, targetPos, fullRect);
    }

    virtual void copyFrom(const PixelSource &src, const Point &targetPos, const Rect &srcPart) {
        Point targetSize = calcTargetSize(srcPart);

        RgbColor color;
        RgbColor color00, color01, color10, color11;

        double xf, yf;

        for (int x = 0; x < targetSize.x; x++) {
            for (int y = 0; y < targetSize.y; y++) {

                if (!calcSourcePos(xf, yf, x, y, srcPart)) {
                    putBackgroundPixel(static_cast<unsigned int>(targetPos.x + x), static_cast<unsigned int>(targetPos.y + y));
                    continue;
                }

                double xf0 = floor(xf);
                double yf0 = floor(yf);

                double dx = xf - xf0;
                double dy = yf - yf0;

                int xs0 = srcPart.x1 + static_cast<int>(round(xf0));
                int ys0 = srcPart.y1 + static_cast<int>(round(yf0));

                color00 = src.getPixel({xs0, ys0});
                color10 = src.getPixel({xs0 + 1, ys0});
                color01 = src.getPixel({xs0, ys0 + 1});
                color11 = src.getPixel({xs0 + 1, ys0 + 1});

                color = {0, 0, 0};

                addScaledColor(color, color00, (1.0f - dx) * (1.0f - dy));
                addScaledColor(color, color10, dx * (1.0f - dy));
                addScaledColor(color, color01, (1.0f - dx) * dy);
                addScaledColor(color, color11, dx * dy);

                putPixel(static_cast<unsigned int>(targetPos.x + x), static_cast<unsigned int>(targetPos.y + y), color);
            }
        }
    }

protected:
    virtual Point calcTargetSize(const Rect &srcPart) = 0;

    virtual bool
    calcSourcePos(double &sourcePosX, double &sourcePosY, int targetPosX, int targetPosY, const Rect &srcPart) = 0;

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) = 0;

    virtual void putBackgroundPixel(unsigned int x, unsigned int y) = 0;

    void addScaledColor(RgbColor &color, const RgbColor &newColor, double ratio) {
        color.red = static_cast<unsigned char>(std::min(255, static_cast<int>(round(
                static_cast<double>(color.red) + ratio * static_cast<double>(newColor.red)))));
        color.green = static_cast<unsigned char>(std::min(255, static_cast<int>(round(
                static_cast<double>(color.green) + ratio * static_cast<double>(newColor.green)))));
        color.blue = static_cast<unsigned char>(std::min(255, static_cast<int>(round(
                static_cast<double>(color.blue) + ratio * static_cast<double>(newColor.blue)))));
    }
};

class ScalingCopierBase : public BilinearSamplingCopier {
    using inherited = BilinearSamplingCopier;
public:
    ScalingCopierBase() : initialized_(false) {}

    virtual void copyFrom(const PixelSource &src, const Point &targetPos) {
        inherited::copyFrom(src, targetPos);
    }

    virtual void copyFrom(const PixelSource &src, const Point &targetPos, const Rect &srcPart) {
        checkInitialized();
        inherited::copyFrom(src, targetPos, srcPart);
    }

protected:
    virtual void putBackgroundPixel(unsigned int /*x*/, unsigned int /*y*/) {}

    virtual void getScalingRatios(double &rx, double &ry) = 0;

    virtual void getScalingRatiosInv(double &irx, double &iry) = 0;

    void checkInitialized() {
        if (!initialized_) {
            initialize();
            initialized_ = true;
        }
    }

    virtual void initialize() {}

    virtual Point calcTargetSize(const Rect &srcPart) {
        double ratioX, ratioY;
        getScalingRatios(ratioX, ratioY);
        Point targetSize;
        targetSize.x = static_cast<int>(round(srcPart.width() * ratioX));
        targetSize.y = static_cast<int>(round(srcPart.height() * ratioY));
        return targetSize;
    }

    virtual bool
    calcSourcePos(double &sourcePosX, double &sourcePosY, int targetPosX, int targetPosY, const Rect &srcPart) {
        double iratioX, iratioY;
        getScalingRatiosInv(iratioX, iratioY);
        sourcePosX = srcPart.x1 + static_cast<double>(targetPosX) * iratioX;
        sourcePosY = srcPart.y1 + static_cast<double>(targetPosY) * iratioY;
        return true;
    }

private:
    bool initialized_;
};

class ScalingCopierForPixels : public ScalingCopierBase {
public:
    ScalingCopierForPixels(PixelPainter &pixelPainter, double ratioX, double ratioY) : pixelPainter_(pixelPainter),
                                                                                       ratioX_(ratioX),
                                                                                       ratioY_(ratioY) {}

protected:
    virtual double getRatioX() {
        return ratioX_;
    }

    virtual double getRatioY() {
        return ratioY_;
    }

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        pixelPainter_.putPixel(x, y, color);
    }

protected:
    virtual void initialize() {
        ratioXInv_ = 1.0 / ratioX_;
        ratioYInv_ = 1.0 / ratioY_;
    }

    virtual void getScalingRatios(double &rx, double &ry) {
        rx = ratioX_;
        ry = ratioY_;
    }

    virtual void getScalingRatiosInv(double &irx, double &iry) {
        irx = ratioXInv_;
        iry = ratioYInv_;
    }

private:
    PixelPainter &pixelPainter_;
    double ratioX_;
    double ratioY_;
    double ratioXInv_;
    double ratioYInv_;
};

class RotationCopierBase : public BilinearSamplingCopier {
    using inherited = BilinearSamplingCopier;
public:
    RotationCopierBase(double angle, const Point &srcOffset) : initialized_(false), angle_(angle), srcOffset_(srcOffset), ratioX_(1.0),
                                                               ratioY_(1.0) {}

    RotationCopierBase(double angle, double ratioX, double ratioY, const Point &srcOffset) :
            initialized_(false),
            angle_(angle),
            srcOffset_(srcOffset),
            ratioX_(ratioX),
            ratioY_(ratioY) {}

    virtual void copyFrom(const PixelSource &src, const Point &targetPos) {
        inherited::copyFrom(src, targetPos);
    }

    virtual void copyFrom(const PixelSource &src, const Point &targetPos, const Rect &srcPart) {
        checkInitialized(srcPart);
        inherited::copyFrom(src, targetPos, srcPart);
    }

protected:
    void checkInitialized(const Rect &srcPart) {
        if (!initialized_) {
            initialize(srcPart);
            initialized_ = true;
        }
    }

    virtual void initialize(const Rect &srcPart) {
        rotationCentre_.x = srcPart.width() / 2;
        rotationCentre_.y = srcPart.height() / 2;

        double angle = getAngle();

        sinma_ = sin(-angle);
        cosma_ = cos(-angle);
    }

    virtual bool
    calcSourcePos(double &sourcePosX, double &sourcePosY, int targetPosX, int targetPosY, const Rect &srcPart) {
        bool result = false;

        int xt =
                static_cast<int>(round((static_cast<double>(targetPosX) / ratioX_))) - rotationCentre_.x + srcOffset_.x;
        int yt =
                static_cast<int>(round((static_cast<double>(targetPosY) / ratioY_))) - rotationCentre_.y + srcOffset_.y;

        sourcePosX = (cosma_ * xt - sinma_ * yt) + rotationCentre_.x;
        sourcePosY = (sinma_ * xt + cosma_ * yt) + rotationCentre_.y;

        int xs = static_cast<int>(floor(sourcePosX));
        int ys = static_cast<int>(floor(sourcePosY));

        if (xs >= srcPart.x1 && xs <= srcPart.x2 && ys >= srcPart.y1 && ys <= srcPart.y2) {
            result = true;
        }

        return result;
    }

    // aproximated
    virtual Point calcTargetSize(const Rect &srcPart) {
        Point result;
        result.x = srcPart.width() * 2;
        result.y = srcPart.height() * 2;
        return result;
    }

    virtual double getAngle() {
        return angle_;
    }

private:
    bool initialized_;
    double angle_;
    Point srcOffset_;
    Point rotationCentre_;
    double sinma_;
    double cosma_;
    double ratioX_;
    double ratioY_;
};

class RotationCopierForPixels : public RotationCopierBase {
public:
    RotationCopierForPixels(PixelPainter &pixelPainter, double angle, const Point &srcOffset) :
            RotationCopierBase(angle, srcOffset),
            pixelPainter_(pixelPainter) {}

    RotationCopierForPixels(PixelPainter &pixelPainter, double angle, double ratioX, double ratioY,
                            const Point &srcOffset) :
            RotationCopierBase(angle, ratioX, ratioY, srcOffset),
            pixelPainter_(pixelPainter) {}

protected:
    virtual void putBackgroundPixel(unsigned int /*x*/, unsigned int /*y*/) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        pixelPainter_.putPixel(x, y, color);
    }

private:
    PixelPainter &pixelPainter_;
};

#endif
