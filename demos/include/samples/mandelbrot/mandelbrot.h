#ifndef __UIMG_SAMPLES_VORONOI_H__
#define __UIMG_SAMPLES_VORONOI_H__

#include <climits>
#include <vector>
#include "uimg/base/structs.h"
#include "uimg/pixels/pixel_painter.h"
#include "uimg/utils/cast.h"

class Mandelbrot {
public:
    void Make(const Point &canvasSize, PixelPainter &painter, int count) {
        canvasSize_ = canvasSize;
        painter_ = &painter;

        CreateColors(count);
        CreatePixels();
    }

private:
    void CreatePixels() {
        double windowReStart = -2;
        double windowReSize = 3;
        double windowImStart = -1;
        double windowImSize = 2;

        int maxIterations = static_cast<int>(colors_.size()) - 1;

        int w = canvasSize_.x, h = canvasSize_.y;
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                double c_re = windowReStart + (static_cast<double>(x) / w) * windowReSize;
                double c_im = windowImStart + (static_cast<double>(y) / h) * windowImSize;
                int m = 1 + countMandelbrot(c_re, c_im, maxIterations);
                if (m >= static_cast<int>(colors_.size())) {
                    m = 0;
                }

                painter_->putPixel(UNSIGNED_CAST(unsigned int, x), UNSIGNED_CAST(unsigned int, y), colors_[static_cast<size_t>(m)]);
            }
        }
    }

    int countMandelbrot(double c_re, double c_im, int maxIterations) {
        int result = 0;
        double x = 0, y = 0;
        while (x * x + y * y <= 4 && result < maxIterations) {
            double x_new = x * x - y * y + c_re;
            y = 2 * x * y + c_im;
            x = x_new;
            result++;
        }
        return result;
    }

    void CreateColors(int count) {
        RgbColor background = {0, 0, 0};
        colors_.push_back(background);

        for (size_t i = 0; i < static_cast<size_t>(count); i++) {
            RgbColor c = {
                    UNSIGNED_CAST(unsigned char, rand() % 200 + 50),
                    UNSIGNED_CAST(unsigned char, rand() % 200 + 55),
                    UNSIGNED_CAST(unsigned char, rand() % 200 + 50)
            };
            colors_.push_back(c);
        }
    }

    std::vector<RgbColor> colors_;
    PixelPainter *painter_;
    Point canvasSize_;
};

#endif
