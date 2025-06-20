#ifndef __UIMG_DEMO_MANDELBR_H__
#define __UIMG_DEMO_MANDELBR_H__

#include <cmath>
#include <complex>

#include "samples/demo_painter_base.h"

class mandelbrot_demo : public demo_painter_base {
public:
    mandelbrot_demo(const std::string outFileName) : demo_painter_base(outFileName) {}

protected:
    virtual void paint() {
        RgbImage &image = getImage();

        BackgroundPainterForRgbImage backPainter(image);
        backPainter.paint({0, 0, 0});

        PixelPainterForImageBase pixelPainter(image);

        const int maxIter = 100;
        const float scaleFactor = 5.0f;

        // Draw Mandelbrot set
        for (unsigned y = 0; y < image.height(); ++y) {
            for (unsigned x = 0; x < image.width(); ++x) {
                float fx = (static_cast<float>(x) / static_cast<float>(image.width()) - 0.5f) * scaleFactor - 0.7f;
                float fy = (static_cast<float>(y) / static_cast<float>(image.height()) - 0.5f) * scaleFactor;

                std::complex<float> z(0.0f, 0.0f);
                std::complex<float> c(fx, fy);

                int iter = 0;
                for (; iter < maxIter && std::abs(z) < 2.0f; ++iter) {
                    z = z * z + c;
                }

                if (iter == maxIter) {
                    pixelPainter.putPixel(x, y, {0, 0, 0});
                } else {
                    float col = (static_cast<float>(iter) / static_cast<float>(maxIter)) * 255.0f;
                    unsigned char r = static_cast<unsigned char>(col);
                    unsigned char g = static_cast<unsigned char>((col * 3) > 255 ? 255 : (col * 3));
                    unsigned char b = static_cast<unsigned char>((col * 8) > 255 ? 255 : (col * 8));
                    pixelPainter.putPixel(x, y, {r, g, b});
                }
            }
        }
    }
};

#endif
