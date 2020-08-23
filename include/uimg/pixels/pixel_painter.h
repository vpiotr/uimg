//
// Created by piotr on 8/23/20.
//

#ifndef UIMG_PIXEL_PAINTER_H
#define UIMG_PIXEL_PAINTER_H

#include "uimg/base/structs.h"
#include "uimg/utils/color_utils.h"

// abstract class which paints RGB pixels
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

#endif //UIMG_PIXEL_PAINTER_H
