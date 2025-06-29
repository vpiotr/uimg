#ifndef __UIMG_DEMO_VORONOI_H__
#define __UIMG_DEMO_VORONOI_H__

#include <cstdlib>
#include <ctime>

#include "samples/demo_painter_base.h"

class voronoi_demo : public demo_painter_base {
public:
    voronoi_demo(const std::string outFileName) : demo_painter_base(outFileName) {}

protected:
    virtual void paint() {
        srand(static_cast<unsigned int>(time(NULL)));

        RgbImage &image = getImage();

        BackgroundPainterForRgbImage backPainter(image);
        backPainter.paint({255, 255, 255});

        // Generate random points and colors
        const unsigned numPoints = 50;
        std::vector<Point> points;
        std::vector<RgbColor> colors;

        for (unsigned i = 0; i < numPoints; ++i) {
            Point p;
            p.x = rand() % static_cast<int>(image.width());
            p.y = rand() % static_cast<int>(image.height());
            points.push_back(p);

            RgbColor color;
            color.red = rand() % 256;
            color.green = rand() % 256;
            color.blue = rand() % 256;
            colors.push_back(color);
        }

        // For each pixel, find the closest point and color it accordingly
        for (unsigned y = 0; y < image.height(); ++y) {
            for (unsigned x = 0; x < image.width(); ++x) {
                Point pixel(static_cast<int>(x), static_cast<int>(y));
                unsigned closestIdx = 0;
                int minDistSq = std::numeric_limits<int>::max();

                for (unsigned i = 0; i < numPoints; ++i) {
                    int dx = pixel.x - points[i].x;
                    int dy = pixel.y - points[i].y;
                    int distSq = dx * dx + dy * dy;

                    if (distSq < minDistSq) {
                        minDistSq = distSq;
                        closestIdx = i;
                    }
                }

                image.setPixel(pixel, colors[closestIdx]);
            }
        }

        // Draw the points on top
        PixelPainterForImageBase pixelPainter(image);
        CirclePainterForPixels circlePainter(pixelPainter);

        for (unsigned i = 0; i < numPoints; ++i) {
            circlePainter.drawFull(points[i].x, points[i].y, 3, {0, 0, 0});
        }
    }
};

#endif
