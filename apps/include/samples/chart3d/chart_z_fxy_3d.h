#ifndef __UIMG_SAMPLES_CHART_Z_FXY_3D_H__
#define __UIMG_SAMPLES_CHART_Z_FXY_3D_H__

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <utility>
#include <climits>
#include <memory>
#include <algorithm>
#include <cassert>
#include <iostream>

#include "uimg/images/rgb_image.h"
#include "uimg/painters/painter_for_pixels.h"
#include "uimg/painters/antialiased_painter_for_pixels.h"
#include "uimg/utils/math_utils.h"
#include "samples/chart3d/chart3d_tracer.h"

class chart_z_fxy_3d {
public:
    chart_z_fxy_3d(const Point &canvasSize, PixelPainter &pixelPainter, bool useAntiAliasing = false, bool drawBorders = false) 
        : canvasSize_(canvasSize), pixelPainter_(pixelPainter), useAntiAliasing_(useAntiAliasing), drawBorders_(drawBorders) {}

    virtual Point getChartSize() {
        if (drawBorders_) {
            // When borders are enabled, reserve space for border and margin
            const int borderAndMargin = 30; // 15 for margin on each side
            int availableWidth = canvasSize_.x - borderAndMargin;
            int availableHeight = canvasSize_.y - borderAndMargin;
            return { static_cast<int>(round(0.8 * availableWidth)), 
                     static_cast<int>(round(0.8 * availableHeight)) };
        } else {
            // Normal chart size when borders are disabled
            return { static_cast<int>(round(0.8 * canvasSize_.x)), 
                     static_cast<int>(round(0.8 * canvasSize_.y)) };
        }
    }

    virtual Point getScreenOffset() {
        if (drawBorders_) {
            // When borders are enabled, add extra offset for border and margin
            return{ 25, 115 }; // 15 margin + 10 original offset, 15 margin + 100 original offset
        } else {
            // Normal offset when borders are disabled
            return{ 10, 100 };
        }
    }

    virtual Point getCanvasSize() {
        return canvasSize_;
    }

    virtual Point getScreenStepSize() {
        return{ 3, 5 };
    }

    virtual std::pair<double, double> getInputRangeX() {
        return std::pair<double, double>( -2520.0, 2520.0 );
    }

    virtual std::pair<double, double> getInputRangeY() {
        return std::pair<double, double>( -2520.0, 2520.0 );
    }

    virtual double getSkewAngle() {
        return 75.0;
    }

    virtual double getSkewLength() {
        return .75;
    }

    virtual double getResultScale() {
        return 80.0;
    }

    virtual bool shouldDrawBorders() const {
        return drawBorders_;
    }

    virtual void setDrawBorders(bool enable) {
        drawBorders_ = enable;
    }

    virtual RgbColor getBorderColor() const {
        return {128, 128, 128}; // Default to gray borders
    }

    bool isTraceEnabled() const {
        return Chart3DTracer::getInstance()->isEnabled();
    }

    void paint() {
        PixelPainter *pixelPainter = &pixelPainter_;
        std::unique_ptr<LinePainterForPixels> lnPainter;
        
        // Create appropriate line painter based on anti-aliasing setting
        if (useAntiAliasing_) {
            lnPainter.reset(new AntiAliasedLinePainterForPixels(*pixelPainter));
            std::cerr << "Using anti-aliased line painter" << std::endl;
        } else {
            lnPainter.reset(new LinePainterForPixels(*pixelPainter));
            std::cerr << "Using standard line painter" << std::endl;
        }
        if (isTraceEnabled()) {
            std::cerr << "Tracing is enabled" << std::endl;
        } else {
            std::cerr << "Tracing is disabled" << std::endl;
        }
        RgbColor color;

        // screen size
        Point canvasSize = getCanvasSize();
        Point chartSize = getChartSize();
        Point screenOffset = getScreenOffset();

        int maxY = canvasSize_.y - 1;
        int sizeX = canvasSize_.x;

        int screenOffsetX = screenOffset.x;
        int screenOffsetY = screenOffset.y;
        int screenSizeX = chartSize.x;
        int screenSizeY = chartSize.y;

        // sample space
        std::pair<double, double> inputRangeX = getInputRangeX();
        std::pair<double, double> inputRangeY = getInputRangeY();

        Point stepSize = getScreenStepSize();
        int sampleSpaceX = screenSizeX; 
        int sampleSpaceY = screenSizeY; 
        int sampleStepX = stepSize.x;
        int sampleStepY = stepSize.y;

        double sampleScale = getSkewLength();
        const double resultScale = getResultScale(); 

        // other parameters
        double skewAngle = getSkewAngle();

        // pre-calculations
        int midSampleSpaceX = sampleSpaceX / 2;
        int midSampleSpaceY = sampleSpaceY / 2;

        double sampleToInputRatioX = (inputRangeX.second - inputRangeX.first) / (2 * midSampleSpaceX);
        double sampleToInputRatioY = (inputRangeY.second - inputRangeY.first) / (2 * midSampleSpaceY);

        static const double sampleToInputShiftX = inputRangeX.first;
        static const double sampleToInputShiftY = inputRangeY.first;

        double degreesToRadiansFactor = math_utils::pi_const_d() / 180;
        double sampleScaleForX = sampleScale * cos(skewAngle * degreesToRadiansFactor);
        double sampleScaleForY = sampleScale * sin(skewAngle * degreesToRadiansFactor);

        double xe0 = midSampleSpaceX + sampleScaleForX * midSampleSpaceY + screenOffsetX;
        double ye0 = sampleScaleForY * midSampleSpaceY + screenOffsetY;

        // Draw border if enabled
        if (shouldDrawBorders()) {
            drawChartBorder(lnPainter.get(), screenOffsetX, screenOffsetY, screenSizeX, screenSizeY, maxY);
        }

        // working variables
        double x,y,z;
        double xe, ye, x1, y1, x2, y2;
        int f1, f2;

        unsigned int e;
        std::vector<int> h_up(sizeX), h_down(sizeX);

        // Assert: sizeX must be positive and finite for the loop to terminate
        assert(sizeX > 0 && sizeX < 1000000 && "sizeX must be positive and reasonable for finite loop");
        // Assert: sampleStepY must be positive and sampleSpaceY must be positive
        assert(sampleStepY > 0 && sampleSpaceY > 0 && "sampleStepY and sampleSpaceY must be positive for finite loop");
        // Assert: sampleStepX must be positive and sampleSpaceX must be positive
        assert(sampleStepX > 0 && sampleSpaceX > 0 && "sampleStepX and sampleSpaceX must be positive for finite loop");

        for (int i = 0; i < sizeX; ++i) {
            h_up[i] = INT_MIN;
            h_down[i] = INT_MAX;
        }

        // Assert: outer loop q will terminate
        assert(midSampleSpaceY >= 0 && sampleStepY > 0 && "midSampleSpaceY and sampleStepY must be non-negative and positive");
        auto tracer = Chart3DTracer::getInstance();
        tracer->trace("Starting outer loop over q (Y axis) in paint()");
        for (int q = -midSampleSpaceY; q <= midSampleSpaceY; q += sampleStepY) {
            y = sampleToInputShiftY + (q + midSampleSpaceY) * sampleToInputRatioY;
            tracer->trace("q=%d, y=%f", q, y);
            tracer->trace(" Starting inner loop over m (X axis) for q=%d", q);
            for (int m = -midSampleSpaceX; m <= midSampleSpaceX; m += sampleStepX) {
                x = sampleToInputShiftX + (m + midSampleSpaceX) * sampleToInputRatioX;
                
                // Calculate z value based on x,y function
                z = resultScale * getFunValue(x, y);

                // Calculate screen coordinates with proper rounding
                xe = round(xe0 + m + sampleScaleForX * q);
                ye = round(ye0 + sampleScaleForY * q + z);
                tracer->trace("  m=%d, x=%f, z=%f, xe=%f, ye=%f", m, x, z, xe, ye);

                if (m <= -midSampleSpaceX) {
                    f1 = 0;
                    e = round(xe / sampleStepX);

                    if ((e < sizeX) && (ye >= h_up[e])) {
                        f1 = 1;
                        h_up[e] = ye;
                    } 
                    
                    if ((e < sizeX) && (ye <= h_down[e])) {
                        f1 = 1;
                        h_down[e] = ye;
                    }

                    x1 = xe;
                    y1 = ye;
                }
                else {
                    f2 = 0;
                    e = round(xe / sampleStepX);

                    if ((e < sizeX) && (ye >= h_up[e])) {
                        f2 = 1;
                        h_up[e] = ye;
                    }

                    if ((e < sizeX) && (ye <= h_down[e])) {
                        f2 = 1;
                        h_down[e] = ye;
                    }

                    x2 = xe; y2 = ye;

                    if (f1 * f2 == 1) {
                        color = getPlotColor(x, y, z);

                        // For non-anti-aliased version, just draw as before
                        pixelPainter->putPixel(x1, maxY - y1, color);
                        lnPainter->drawLine(x1, maxY - y1, x2, maxY - y2, color);
                    }

                    x1 = x2; 
                    y1 = y2;
                    f1 = f2;
                }
            }
            tracer->trace(" Finished inner loop for q=%d", q);
        }
        tracer->trace("Finished outer loop in paint()");
    }

protected:
    virtual RgbColor getPlotColor(double x, double y, double z) {
        return{ 255, 255, 255 };
    }

    // returns function value
    virtual double getFunValue(double x, double y) = 0;

    // Draw border around the chart area
    virtual void drawChartBorder(LinePainterForPixels* linePainter, int offsetX, int offsetY, int sizeX, int sizeY, int maxY) {
        if (!linePainter) return;
        
        RgbColor borderColor = getBorderColor();
        
        // Calculate border coordinates around the entire chart area with a bit of margin
        const int borderMargin = 15;
        
        int left = borderMargin;
        int right = canvasSize_.x - borderMargin - 1;
        int top = borderMargin;
        int bottom = canvasSize_.y - borderMargin - 1;
        
        // Draw rectangle border
        linePainter->drawLine(left, top, right, top, borderColor);       // top edge
        linePainter->drawLine(right, top, right, bottom, borderColor);   // right edge  
        linePainter->drawLine(right, bottom, left, bottom, borderColor); // bottom edge
        linePainter->drawLine(left, bottom, left, top, borderColor);     // left edge
    }

private:
    PixelPainter &pixelPainter_;
    Point canvasSize_;
    bool useAntiAliasing_;
    bool drawBorders_;
};

#endif
