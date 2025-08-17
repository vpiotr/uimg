#ifndef __UIMG_SAMPLES_VORONOI_H__
#define __UIMG_SAMPLES_VORONOI_H__

#include "uimg/images/rgb_image.h"
#include "uimg/painters/painter_base.h"
#include "uimg/utils/cast.h"
#include <climits>

// Voronoi diagram generator
class Voronoi {
public:
    void Make(const Point &canvasSize, PixelPainter &painter, int count) {
        canvasSize_ = canvasSize;
        painter_ = &painter;

        CreatePoints(count);
        CreateColors();
        CreateSites();
        SetSitesPoints();
    }

private:
    static int DistanceSqrd(const Point &point, int x, int y) {
        int xd = x - static_cast<int>(point.x);
        int yd = y - static_cast<int>(point.y);
        return (xd * xd) + (yd * yd);
    }

    void CreateSites() {
        int w = canvasSize_.x, h = canvasSize_.y, d;
        for (int hh = 0; hh < h; hh++) {
            for (int ww = 0; ww < w; ww++) {
                int ind = -1, dist = INT_MAX;
                for (size_t it = 0; it < points_.size(); it++) {
                    const Point &p = points_[it];
                    d = DistanceSqrd(p, ww, hh);
                    if (d < dist) {
                        dist = d;
                        ind = static_cast<int>(it);
                    }
                }

                if (ind > -1) {
                    painter_->putPixel(UNSIGNED_CAST(unsigned int, ww), UNSIGNED_CAST(unsigned int, hh), colors_[static_cast<size_t>(ind)]);
                }
            }
        }
    }

    void SetSitesPoints() {
        RgbColor pixel{0, 0, 0};

        for (const auto &point : points_) {
            int x = point.x, y = point.y;
            for (int i = -1; i < 2; i++)
                for (int j = -1; j < 2; j++) {
                    painter_->putPixel(UNSIGNED_CAST(unsigned int, x + i), UNSIGNED_CAST(unsigned int, y + j), pixel);
                }
        }
    }

    void CreatePoints(int count) {
        const int w = canvasSize_.x - 20, h = canvasSize_.y - 20;
        for (int i = 0; i < count; i++) {
            points_.push_back({(rand() % w + 10), (rand() % h + 10)});
        }
    }

    void CreateColors() {
        for (size_t i = 0; i < points_.size(); i++) {
            RgbColor c = {
                    UNSIGNED_CAST(unsigned char, rand() % 200 + 50),
                    UNSIGNED_CAST(unsigned char, rand() % 200 + 55),
                    UNSIGNED_CAST(unsigned char, rand() % 200 + 50)
            };
            colors_.push_back(c);
        }
    }

    std::vector<Point> points_;
    std::vector<RgbColor> colors_;
    PixelPainter *painter_;
    Point canvasSize_;
};

#endif
