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
#include "uimg/filters/pixel_tracing_filter.h"
#include "uimg/utils/math_utils.h"
#include "samples/chart3d/chart3d_tracer.h"
#include "samples/logger.h"

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
            
            // Adaptive scaling based on aspect ratio to handle very wide or tall charts
            double aspectRatio = static_cast<double>(availableWidth) / availableHeight;
            double widthFactor, heightFactor;
            
            if (aspectRatio > 2.5) {
                // Very wide layout (like triple chart top): be more conservative
                widthFactor = 0.50;
                heightFactor = 0.55;  // Further reduced to prevent any Y overflow
            } else if (aspectRatio < 0.8) {
                // Very tall layout: be more conservative on height
                widthFactor = 0.70;
                heightFactor = 0.40;  // Further reduced to prevent any Y overflow
            } else {
                // Normal aspect ratio: use better scaling to fill space
                widthFactor = 0.65;
                heightFactor = 0.55;  // Further reduced to prevent any Y overflow
            }
            
            return { static_cast<int>(round(widthFactor * availableWidth)), 
                     static_cast<int>(round(heightFactor * availableHeight)) };
        } else {
            // No borders mode - use the allocated canvas size directly but with conservative scaling
            // This accounts for the 3D rendering algorithm drawing a few extra pixels beyond the calculated size
            double widthFactor = 0.70;   // Slightly more conservative than before
            double heightFactor = 0.70;  // Reduced to prevent Y overflow in multi-chart layouts
            
            return { static_cast<int>(round(widthFactor * canvasSize_.x)), 
                     static_cast<int>(round(heightFactor * canvasSize_.y)) };
        }
    }

    virtual Point getScreenOffset() {
        Point chartSize = getChartSize();
        
        // Calculate offset to center the chart within the canvas
        int offsetX = (canvasSize_.x - chartSize.x) / 2;
        int offsetY = (canvasSize_.y - chartSize.y) / 2;
        
        if (drawBorders_) {
            // When borders are enabled, ensure minimum margin for border
            const int minMargin = 15;
            offsetX = std::max(offsetX, minMargin);
            offsetY = std::max(offsetY, minMargin);
        } else {
            // Ensure minimum offset even without borders
            const int minOffset = 10;
            offsetX = std::max(offsetX, minOffset);
            offsetY = std::max(offsetY, minOffset);
        }
        
        return { offsetX, offsetY };
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

    virtual double getCenterZ() const {
        return 0.0;  // Default center Z value for chart centering
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

    /**
     * @brief Get the base painter for operations that should bypass tracing (like borders)
     * @return Reference to the base painter
     */
    PixelPainter& getBasePainter() const {
        // Check if the current painter is a PixelTracingFilter
        PixelTracingFilter* tracingFilter = dynamic_cast<PixelTracingFilter*>(&pixelPainter_);
        if (tracingFilter != nullptr) {
            // Return the base painter to bypass tracing
            return tracingFilter->getBasePainter();
        }
        // If not a tracing filter, return the painter itself
        return pixelPainter_;
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


        
        // Calculate available space for chart drawing (declare at method scope)
        int availableLeft, availableRight, availableTop, availableBottom;
        if (drawBorders_) {
            const int borderMargin = 15;
            const int chartMargin = 10; // Additional margin inside borders for chart
            availableLeft = borderMargin + chartMargin;
            availableRight = canvasSize_.x - borderMargin - chartMargin - 1;
            availableTop = borderMargin + chartMargin;
            availableBottom = canvasSize_.y - borderMargin - chartMargin - 1;
            
            int borderLeft = borderMargin;
            int borderRight = canvasSize_.x - borderMargin - 1;
            int borderTop = borderMargin;
            int borderBottom = canvasSize_.y - borderMargin - 1;
        } else {
            const int windowMargin = 5; // Margin from window edge when no borders
            availableLeft = windowMargin;
            availableRight = canvasSize_.x - windowMargin - 1;
            availableTop = windowMargin;
            availableBottom = canvasSize_.y - windowMargin - 1;
        }


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

        // Debug logging
        auto logger = DemoLogger::getInstance();
        
        // Calculate the center of the available space for proper chart centering
        double availableCenterX = (availableLeft + availableRight) / 2.0;
        double availableCenterY = (availableTop + availableBottom) / 2.0;
        
        // Original centering calculation (for reference)
        double xe0_old = midSampleSpaceX + sampleScaleForX * midSampleSpaceY + screenOffsetX;
        double ye0_old = sampleScaleForY * midSampleSpaceY + screenOffsetY;
        
        // New centering calculation: position the middle of the chart (input x=0, y=0) 
        // at the center of the available space
        // 
        // The coordinate transformation is: xe = xe0 + m + sampleScaleForX * q
        // When input x=0, y=0, we have m=0, q=0 (middle of sample space)
        // We want xe = availableCenterX when m=0, q=0
        // So: availableCenterX = xe0 + 0 + sampleScaleForX * 0 = xe0
        // Therefore: xe0 = availableCenterX
        //
        // For Y coordinates, the final pixel coordinate is: maxY - ye
        // When input x=0, y=0, we have q=0 and z=getCenterZ()*resultScale  
        // We want maxY - ye = availableCenterY when q=0 and z=z_center
        // ye = ye0 + sampleScaleForY * 0 + z_center = ye0 + z_center
        // So: maxY - (ye0 + z_center) = availableCenterY
        // Therefore: ye0 = maxY - availableCenterY - z_center
        //
        // However, empirical testing shows the 3D rendering algorithm creates a systematic
        // Y-offset of approximately 8-10 pixels. Compensate for this to achieve better visual centering.
        const double empiricalYOffset = 10.0;  // Compensation for 3D rendering Y-bias
        double z_center = resultScale * getCenterZ();  // Use user-provided center Z value
        double xe0 = availableCenterX;
        double ye0 = maxY - availableCenterY - z_center + empiricalYOffset;  // Add offset compensation
        
        logger->debug("=== Chart Centering Debug ===");
        logger->debug("Available space: (%d,%d) to (%d,%d)", availableLeft, availableTop, availableRight, availableBottom);
        logger->debug("Available space center: (%.1f, %.1f)", availableCenterX, availableCenterY);
        logger->debug("Sample space: %dx%d (mid: %d,%d)", sampleSpaceX, sampleSpaceY, midSampleSpaceX, midSampleSpaceY);
        logger->debug("Center Z value: %.2f (scaled: %.2f)", getCenterZ(), z_center);
        logger->debug("maxY: %d", maxY);
        logger->debug("Chart origin: xe0=%.1f, ye0=%.1f", xe0, ye0);
        logger->debug("Expected pixel center: (%.1f, %.1f)", xe0, maxY - (ye0 + z_center));
        logger->debug("Y coordinate analysis: ye0=%.1f, z_center=%.1f, final_ye=%.1f", ye0, z_center, ye0 + z_center);
        logger->debug("Sample space Y range: q=[%d,%d], actual Y input range: [%.3f,%.3f]", 
                     -midSampleSpaceY, midSampleSpaceY, sampleToInputShiftY, sampleToInputShiftY + 2*midSampleSpaceY*sampleToInputRatioY);

        // Draw border if enabled (using base painter to bypass tracing)
        if (shouldDrawBorders()) {
            PixelPainter& basePainter = getBasePainter();
            std::unique_ptr<LinePainterForPixels> borderPainter;
            
            // Create line painter for borders using base painter (bypasses tracing)
            if (useAntiAliasing_) {
                borderPainter.reset(new AntiAliasedLinePainterForPixels(basePainter));
            } else {
                borderPainter.reset(new LinePainterForPixels(basePainter));
            }
            
            drawChartBorder(borderPainter.get(), screenOffsetX, screenOffsetY, screenSizeX, screenSizeY, maxY);
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

                // Debug: Check point closest to center (m and q closest to 0)
                if (abs(m) <= 3 && abs(q) <= 5) {
                    auto logger = DemoLogger::getInstance();
                    logger->debug("NEAR CENTER: m=%d,q=%d input(%.3f,%.3f) -> z=%.3f -> screen(%.1f,%.1f) -> final_pixel(%.1f,%.1f)", 
                                 m, q, x, y, z/resultScale, xe, ye, xe, maxY - ye);
                }
                
                // Special debug for exact mathematical center
                if (m == 0 && q == 0) {
                    auto logger = DemoLogger::getInstance();
                    logger->debug("=== EXACT MATHEMATICAL CENTER ===");
                    logger->debug("Input coordinates: x=%.6f, y=%.6f", x, y);
                    logger->debug("Function value: z=%.6f (scaled: %.2f)", z/resultScale, z);
                    logger->debug("Screen coordinates: xe=%.2f, ye=%.2f", xe, ye);
                    logger->debug("Final pixel: (%.1f, %.1f)", xe, maxY - ye);
                    logger->debug("Expected center: (%.1f, %.1f)", availableCenterX, availableCenterY);
                    logger->debug("Center offset: X=%.1f, Y=%.1f", xe - availableCenterX, (maxY - ye) - availableCenterY);
                }

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
        
        validatePixelRange(availableLeft, availableRight, availableTop, availableBottom);
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

    /**
     * @brief Validate that chart pixels were drawn within the available space
     * @param availableLeft Left boundary of available space
     * @param availableRight Right boundary of available space  
     * @param availableTop Top boundary of available space
     * @param availableBottom Bottom boundary of available space
     */
    virtual void validatePixelRange(int availableLeft, int availableRight, int availableTop, int availableBottom) {
        // After chart drawing is complete, validate pixel usage against available space
        PixelTracingFilter* tracingFilter = dynamic_cast<PixelTracingFilter*>(&pixelPainter_);
        if (tracingFilter != nullptr && tracingFilter->hasPixels()) {
            unsigned int usedMinX, usedMinY, usedMaxX, usedMaxY;
            if (tracingFilter->getPixelRange(usedMinX, usedMinY, usedMaxX, usedMaxY)) {
                // Check if pixels were drawn outside available space
                bool outOfBounds = false;
                if (usedMinX < availableLeft || usedMaxX > availableRight || 
                    usedMinY < availableTop || usedMaxY > availableBottom) {
                    outOfBounds = true;
                    
                    auto logger = DemoLogger::getInstance();
                    logger->warn("CHART BOUNDARY VIOLATION: Chart pixels drawn outside available space!");
                    if (usedMinX < availableLeft) {
                        logger->warn("  Left overflow: chart used x=%d, available starts at x=%d (-%d pixels)", 
                                   usedMinX, availableLeft, availableLeft - usedMinX);
                    }
                    if (usedMaxX > availableRight) {
                        logger->warn("  Right overflow: chart used x=%d, available ends at x=%d (+%d pixels)", 
                                   usedMaxX, availableRight, usedMaxX - availableRight);
                    }
                    if (usedMinY < availableTop) {
                        logger->warn("  Top overflow: chart used y=%d, available starts at y=%d (-%d pixels)", 
                                   usedMinY, availableTop, availableTop - usedMinY);
                    }
                    if (usedMaxY > availableBottom) {
                        logger->warn("  Bottom overflow: chart used y=%d, available ends at y=%d (+%d pixels)", 
                                   usedMaxY, availableBottom, usedMaxY - availableBottom);
                    }
                }
            }
        }
    }

private:
    PixelPainter &pixelPainter_;
    Point canvasSize_;
    bool useAntiAliasing_;
    bool drawBorders_;
};

#endif
