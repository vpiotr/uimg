#ifndef __UIMG_DEMO_PRIMITIVES_H__
#define __UIMG_DEMO_PRIMITIVES_H__

#include <ctime>

#include "samples/demo_painter_base.h"
#include "uimg/painters/copier_for_pixels.h"
#include "uimg/utils/cast.h"

class draw_primitives_demo : public demo_painter_base {
public:
    draw_primitives_demo(const std::string outFileName) : demo_painter_base(outFileName) {}

protected:
    virtual void paint() {
        RgbImage &image = getImage();

        // Create a gradient background
        BackgroundPainterForRgbImage backPainter(image);
        backPainter.paint({240, 248, 255}); // Light blue background

        PixelPainterForImageBase pixelPainter(image);

        // Create a house scene with organized layout
        
        // 1. House foundation - Rectangle
        RectPainterForRgbImage rectPainter(image);
        rectPainter.drawEmpty(150, 250, 350, 400, {139, 69, 19}); // Brown house
        
        // 2. House roof - Triangle
        ThickLinePainterForPixels tlnPainter(pixelPainter, 2.0f);
        TrianglePainterForPixels triPainter(pixelPainter, tlnPainter);
        triPainter.drawEmpty({150, 250}, {250, 180}, {350, 250}, {178, 34, 34}); // Dark red roof
        
        // 3. Door - smaller rectangle
        RectPainterForRgbImage doorPainter(image);
        doorPainter.drawEmpty(220, 320, 280, 400, {101, 67, 33}); // Darker brown door
        
        // 4. Windows - Circles
        CirclePainterForPixels circlePainter(pixelPainter);
        circlePainter.drawFull(190, 290, 20, {135, 206, 235}); // Light blue window
        circlePainter.drawFull(310, 290, 20, {135, 206, 235}); // Light blue window
        
        // 5. Sun - filled circle
        circlePainter.drawFull(450, 80, 40, {255, 215, 0}); // Golden sun
        
        // 6. Sun rays - lines
        ThickLinePainterForPixels rayPainter(pixelPainter, 3.0f);
        rayPainter.drawLine(450, 20, 450, 40, {255, 215, 0});   // Top ray
        rayPainter.drawLine(450, 120, 450, 140, {255, 215, 0}); // Bottom ray
        rayPainter.drawLine(390, 80, 410, 80, {255, 215, 0});   // Left ray
        rayPainter.drawLine(490, 80, 510, 80, {255, 215, 0});   // Right ray
        rayPainter.drawLine(410, 40, 425, 55, {255, 215, 0});   // Top-left ray
        rayPainter.drawLine(475, 55, 490, 40, {255, 215, 0});   // Top-right ray
        rayPainter.drawLine(410, 120, 425, 105, {255, 215, 0}); // Bottom-left ray
        rayPainter.drawLine(475, 105, 490, 120, {255, 215, 0}); // Bottom-right ray
        
        // 7. Tree trunk - thick line
        ThickLinePainterForPixels trunkPainter(pixelPainter, 8.0f);
        trunkPainter.drawLine(80, 350, 80, 420, {101, 67, 33}); // Brown trunk
        
        // 8. Tree crown - ellipse
        ThickPixelPainter tpix(pixelPainter, 2);
        EllipsePainterForPixels elPainter(tpix);
        elPainter.drawEmpty(80, 320, 40, 30, {34, 139, 34}); // Green tree crown
        
        // 9. Cloud shape using B-spline (decorative element in the sky)
        std::vector<Point> cloudPoints;
        // Create fluffy cloud shape control points
        cloudPoints.push_back({120, 130}); // Left bottom
        cloudPoints.push_back({110, 120}); // Left bottom curve
        cloudPoints.push_back({105, 105}); // Left side
        cloudPoints.push_back({110, 90});  // Left bump start
        cloudPoints.push_back({125, 85});  // Left bump top
        cloudPoints.push_back({140, 90});  // Left bump end
        cloudPoints.push_back({150, 80});  // Left-center bump
        cloudPoints.push_back({165, 75});  // Center high point
        cloudPoints.push_back({180, 80});  // Right-center bump
        cloudPoints.push_back({195, 85});  // Right bump start
        cloudPoints.push_back({210, 80});  // Right bump top
        cloudPoints.push_back({225, 85});  // Right bump end
        cloudPoints.push_back({235, 95});  // Right side
        cloudPoints.push_back({240, 110}); // Right bottom curve
        cloudPoints.push_back({230, 125}); // Right bottom
        cloudPoints.push_back({180, 135}); // Bottom center
        cloudPoints.push_back({150, 132}); // Bottom left-center
        cloudPoints.push_back({120, 130}); // Back to start
        // Add extra points to ensure proper closure
        cloudPoints.push_back({110, 120}); // Repeat second point
        cloudPoints.push_back({105, 105}); // Repeat third point
        cloudPoints.push_back({110, 90});  // Repeat fourth point
        
        // First draw thick gray cloud for fill effect
        ThickPixelPainter cloudFillPainter(pixelPainter, 8);
        BSplinePainterForPixels bsplineFilledPainter(cloudFillPainter);
        bsplineFilledPainter.draw(cloudPoints, 250, {200, 200, 200}); // Gray fill
        
        // Then draw thin dark border
        BSplinePainterForPixels bsplineBorderPainter(pixelPainter);
        bsplineBorderPainter.draw(cloudPoints, 250, {80, 80, 80}); // Dark gray border
        
        // 10. Ground line
        ThickLinePainterForPixels groundPainter(pixelPainter, 4.0f);
        groundPainter.drawLine(0, 420, 500, 420, {34, 139, 34}); // Green ground line
    }

};

#endif
