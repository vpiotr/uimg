#ifndef __UIMG_PAINTER_4_RGB_IMG_H__
#define __UIMG_PAINTER_4_RGB_IMG_H__

#include <cstring>

#include "uimg/painters/painter_base.h"
#include "uimg/painters/painter_for_pixels.h"
#include "uimg/images/rgb_image.h"

// pixel painter using standard PixelImageBase as a target
class PixelPainterForImageBase : public PixelPainter {
public:
    PixelPainterForImageBase(PixelImageBase &target) : target_(target) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        target_.setPixel(Point(x, y), color);
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        output = target_.getPixel(Point(x, y));
    }

private:
    PixelImageBase &target_;
};

// class which paints pixels on RGB image
class PixelPainterForRgbImage : public PixelPainter {
public:
    PixelPainterForRgbImage(RgbImage &image) : image_(image) {}

    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) {
        size_t offset = 3 * (y * image_.width() + x);
        if (offset >= image_.dataSize())
            return;

        char *dataPtr = static_cast<char *>(image_.data()) + offset;
        *(dataPtr++) = color.red;
        *(dataPtr++) = color.green;
        *(dataPtr++) = color.blue;
    }

    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) {
        size_t offset = 3 * (y * image_.width() + x);

        if (offset >= image_.dataSize()) {
            output.red = output.green = output.blue = 0;
            return;
        }

        char *dataPtr = static_cast<char *>(image_.data()) + offset;
        output.red = *(dataPtr++);
        output.green = *(dataPtr++);
        output.blue = *(dataPtr++);
    }

private:
    RgbImage &image_;
};

class LinePainterForRgbImage : public LinePainterForPixels {
    using inherited = LinePainterForPixels;
public:
    LinePainterForRgbImage(RgbImage &image) : LinePainterForPixels(pixelPainter_), image_(image),
                                              pixelPainter_(image) {}

    virtual void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        return inherited::drawLine(x1, y1, x2, y2, color);
    }

protected:
    virtual void drawVerticalLine(unsigned int x, unsigned int y1, unsigned int y2, const RgbColor &color) {
        if (x >= image_.width())
            return;

        if (y1 > y2) {
            drawVerticalLine(x, y2, y1, color);
            return;
        }

        unsigned int y1c = std::min(std::min(y1, y2), image_.height() - 1);
        unsigned int y2c = std::min(std::max(y1, y2), image_.height() - 1);

        size_t offset = 3 * (y1c * image_.width() + x);
        char *dataPtr = static_cast<char *>(image_.data()) + offset;
        unsigned int step = 3 * image_.width() - 2;

        for (unsigned int y = y1c; y <= y2c; ++y) {
            *(dataPtr++) = color.red;
            *(dataPtr++) = color.green;
            *dataPtr = color.blue;
            dataPtr += step;
        }
    }

    virtual void drawHorizontalLine(unsigned int x1, unsigned int x2, unsigned int y, const RgbColor &color) {
        if (y >= image_.height())
            return;

        if (x1 > x2) {
            drawHorizontalLine(x2, x1, y, color);
            return;
        }

        int x1c = std::min(x1, image_.width() - 1);
        int x2c = std::min(x2, image_.width() - 1);

        size_t offset = 3 * (y * image_.width() + x1c);
        char *dataPtr = static_cast<char *>(image_.data()) + offset;

        for (int x = x1c; x <= x2c; ++x) {
            *(dataPtr++) = color.red;
            *(dataPtr++) = color.green;
            *(dataPtr++) = color.blue;
        }
    }

    virtual void clipBySize(int &x, int &y) {
        Point size = image_.getSize();
        x = std::max(0, std::min(x, size.x - 1));
        y = std::max(0, std::min(y, size.y - 1));
    }

private:
    RgbImage &image_;
    PixelPainterForRgbImage pixelPainter_;
};

class RectPainterForRgbImage : public RectPainter {
public:
    RectPainterForRgbImage(RgbImage &image) : image_(image), pixelPainter_(image), linePainter_(image) {}

    virtual void
    drawFull(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        if (x1 > x2) {
            drawFull(x2, y2, x1, y1, color);
            return;
        }

        unsigned int ymin = std::min(y1, y2);
        unsigned int ymax = std::max(y1, y2);

        int x1c = std::min(x1, image_.width() - 1);
        int x2c = std::min(x2, image_.width() - 1);

        size_t lnOffset = 3 * (ymin * image_.width() + x1c);
        size_t lnStep = 3 * image_.width();

        char *lnPtr = static_cast<char *>(image_.data()) + lnOffset;
        char *pixelPtr;

        for (unsigned int y = ymin; y <= ymax; ++y) {
            pixelPtr = lnPtr;

            for (int x = x1c; x <= x2c; ++x) {
                *(pixelPtr++) = color.red;
                *(pixelPtr++) = color.green;
                *(pixelPtr++) = color.blue;
            }

            lnPtr += lnStep;
        }
    }

    virtual void
    drawEmpty(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        linePainter_.drawLine(x1, y1, x2, y1, color);
        linePainter_.drawLine(x1, y2, x2, y2, color);
        linePainter_.drawLine(x1, y1, x1, y2, color);
        linePainter_.drawLine(x2, y1, x2, y2, color);
    }

private:
    RgbImage &image_;
    PixelPainterForRgbImage pixelPainter_;
    LinePainterForRgbImage linePainter_;
};


class BackgroundPainterForRgbImage : public BackgroundPainter {
public:
    BackgroundPainterForRgbImage(RgbImage &image) : image_(&image) {}

    virtual ~BackgroundPainterForRgbImage() {}

    virtual void paint(const RgbColor &color) {
        char colorBytes[3];
        colorBytes[0] = color.red;
        colorBytes[1] = color.green;
        colorBytes[2] = color.blue;
        char *data = static_cast<char *>(image_->data());
        size_t dataSize = image_->dataSize();
        unsigned int offset = 0;
        while (offset < dataSize) {
            memcpy(data, colorBytes, sizeof(colorBytes));
            data += sizeof(colorBytes);
            offset += sizeof(colorBytes);
        }
    }

private:
    RgbImage *image_;
};

#endif
