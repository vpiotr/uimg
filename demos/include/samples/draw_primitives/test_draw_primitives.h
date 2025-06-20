#ifndef __UIMG_TEST_PRIMITIVES_H__
#define __UIMG_TEST_PRIMITIVES_H__

#include <ctime>

#include "samples/test_painter_base.h"
#include "uimg/painters/copier_for_pixels.h"
#include "uimg/utils/cast.h"

class test_draw_primitives : public test_painter_base {
public:
    test_draw_primitives(const std::string outFileName) : test_painter_base(outFileName) {}

protected:
    virtual void paint() {
        srand(UNSIGNED_CAST(unsigned int, time(NULL)));

        RgbImage &image = getImage();

        BackgroundPainterForRgbImage backPainter(image);
        backPainter.paint({255, 255, 255});

        PixelPainterForImageBase pixelPainter(image);

        // Line
        ThickLinePainterForPixels tlnPainter(pixelPainter, 3.0f);
        tlnPainter.drawLine(250, 200, 350, 350, {0, 0, 255});

        // Circle
        CirclePainterForPixels circlePainter(pixelPainter);
        circlePainter.drawFull(140, 150, 80, {0, 250, 100});

        // Rectangle
        RectPainterForRgbImage rectPainter(image);
        rectPainter.drawEmpty(200, 100, 400, 200, {250, 0, 0});

        // Triangle
        TrianglePainterForPixels triPainter2(pixelPainter, tlnPainter);
        triPainter2.drawEmpty({100, 300}, {180, 400}, {50, 380}, {128, 34, 168});

        // Ellipse
        ThickPixelPainter tpix(pixelPainter, 4);
        EllipsePainterForPixels elPainter2(tpix);

        elPainter2.drawEmpty(450, 300, 50, 20, {0, 180, 255});

        // B-spline
        BSplinePainterForPixels bsplinePainter(pixelPainter);
        const unsigned splineDx = 300;
        const unsigned splineDy = 350;

        std::vector<Point> points;
        Point p;
        for (int i = 0; i < 10; ++i) {
            p.x = UNSIGNED_CAST(int, UNSIGNED_CAST(unsigned int, rand()) % UNSIGNED_CAST(unsigned int, splineDx)) + 40;
            p.y = UNSIGNED_CAST(int, UNSIGNED_CAST(unsigned int, rand()) % UNSIGNED_CAST(unsigned int, splineDy)) + 200;
            points.push_back(p);
        }

        int divCnt;
        divCnt = std::max(splineDx, splineDy);
        divCnt = 300;
        bsplinePainter.draw(points, divCnt, {255, 55, 100});
    }

};

#endif
