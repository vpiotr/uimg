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
#include "dlog/dlog.h"
#include "uimg/painters/painter_utils.h"


class chart_z_fxy_3d {
public:
    chart_z_fxy_3d(const Point &canvasSize, PixelPainter &pixelPainter, bool useAntiAliasing = false, bool drawBorders = false) 
        : pixelPainter_(pixelPainter), canvasSize_(canvasSize), useAntiAliasing_(useAntiAliasing), drawBorders_(drawBorders) {}

    virtual Point getChartSize() {
        if (drawBorders_) {
            // When borders are enabled, reserve minimal space for border and margin
            // Use percentage-based margin instead of fixed pixels for better adaptability
            const double borderMarginPercent = 0.04; // 4% margin for borders
            int borderAndMargin = std::max(static_cast<int>(std::min(canvasSize_.x, canvasSize_.y) * borderMarginPercent), 16);
            int availableWidth = canvasSize_.x - borderAndMargin;
            int availableHeight = canvasSize_.y - borderAndMargin;
            
            // Use adaptive scaling factors based on aspect ratio
            // More generic approach that scales with available space
            double aspectRatio = static_cast<double>(availableWidth) / availableHeight;
            double widthFactor, heightFactor;
            
            // Define scaling factors based on layout characteristics
            const double conservativeFactor = 0.70;
            const double normalFactor = 0.80;
            const double aggressiveFactor = 0.85;
            
            if (aspectRatio > 2.5) {
                // Very wide layout: be more conservative with width
                widthFactor = conservativeFactor;
                heightFactor = normalFactor;
            } else if (aspectRatio < 0.8) {
                // Very tall layout: optimize for height usage
                widthFactor = aggressiveFactor;
                heightFactor = 0.65; // More conservative on height for tall layouts
            } else {
                // Normal aspect ratio: use most of the available space
                widthFactor = aggressiveFactor;
                heightFactor = conservativeFactor + 0.05; // Slightly more aggressive
            }
            
            return { static_cast<int>(round(widthFactor * availableWidth)), 
                     static_cast<int>(round(heightFactor * availableHeight)) };
        } else {
            // No borders mode - use more of the canvas with adaptive scaling
            const double noBordersFactor = 0.85; // Consistent factor for no-borders mode
            
            return { static_cast<int>(round(noBordersFactor * canvasSize_.x)), 
                     static_cast<int>(round(noBordersFactor * canvasSize_.y)) };
        }
    }

    virtual Point getScreenOffset() {
        Point chartSize = getChartSize();
        
        // Calculate offset to center the chart within the canvas
        int offsetX = (canvasSize_.x - chartSize.x) / 2;
        int offsetY = (canvasSize_.y - chartSize.y) / 2;
        
        if (drawBorders_) {
            // When borders are enabled, ensure minimum margin for border
            // Use percentage-based minimum to be more adaptive
            const double minMarginPercent = 0.02; // 2% of canvas size minimum
            int minMargin = std::max(static_cast<int>(std::min(canvasSize_.x, canvasSize_.y) * minMarginPercent), 8);
            offsetX = std::max(offsetX, minMargin);
            offsetY = std::max(offsetY, minMargin);
        } else {
            // Reduced minimum offset for better space utilization
            const double minOffsetPercent = 0.01; // 1% of canvas size minimum
            int minOffset = std::max(static_cast<int>(std::min(canvasSize_.x, canvasSize_.y) * minOffsetPercent), 4);
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

    virtual RectInclusive getAllowedDrawingArea() {
        Point chartSize = getChartSize();
        Point screenOffset = getScreenOffset();
        
        // Calculate the allowed drawing area based on chart size and screen offset
        return RectInclusive::make_rect(screenOffset.x, screenOffset.y, 
                                        screenOffset.x + chartSize.x - 1, 
                                        screenOffset.y + chartSize.y - 1);
    }

    virtual RgbColor getBackgroundColor() const {
        return {255, 255, 255}; // Default to white background
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

    virtual RgbColor getAllowedBorderColor() const {
        // return debug color for allowed drawing area
        return {0, 0, 255}; // Default to blue for allowed area
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
        RectInclusive allowedArea = getAllowedDrawingArea();
        std::unique_ptr<LinePainter> lnPainter;

        drawAllowedArea(allowedArea, getAllowedBorderColor(), pixelPainter);
        
        lnPainter = createLinePainter(pixelPainter, allowedArea);

        if (isTraceEnabled()) {
            std::cerr << "Tracing is enabled" << std::endl;
        } else {
            std::cerr << "Tracing is disabled" << std::endl;
        }
        RgbColor color;

        // screen size
        [[maybe_unused]] Point canvasSize = getCanvasSize();
        Point chartSize = getChartSize();
        Point screenOffset = getScreenOffset();

        int maxY = canvasSize_.y - 1;
        int sizeX = canvasSize_.x;

        int screenOffsetX = screenOffset.x;
        int screenOffsetY = screenOffset.y;
        int screenSizeX = chartSize.x;
        int screenSizeY = chartSize.y;


        
        // Use the getAllowedDrawingArea() result directly instead of calculating our own
        int availableLeft = allowedArea.x1;
        int availableRight = allowedArea.x2;
        int availableTop = allowedArea.y1;
        int availableBottom = allowedArea.y2;


        // sample space
        std::pair<double, double> inputRangeX = getInputRangeX();
        std::pair<double, double> inputRangeY = getInputRangeY();

        Point stepSize = getScreenStepSize();
        
        // Adjust sample space to fit within allowed area boundaries
        // Use allowed area dimensions instead of chart size to ensure we stay within bounds
        int availableWidth = availableRight - availableLeft + 1;
        int availableHeight = availableBottom - availableTop + 1;
        
        // Use an adaptive safety factor based on available space and 3D projection needs
        // Smaller spaces need more conservative factors due to higher relative impact of projection
        double baseSafetyFactor = 0.75; // Base conservative factor
        
        // Add extra safety margin for smaller charts (more sensitive to projection effects)
        double sizeFactor = std::min(1.0, std::min(availableWidth, availableHeight) / 500.0);
        double adaptiveSafetyFactor = baseSafetyFactor * (0.85 + 0.15 * sizeFactor);
        
        // Additional 3D projection margin to account for skew and depth effects
        double skewAngleRad = getSkewAngle() * math_utils::pi_const_d() / 180.0;
        double skewLength = getSkewLength();
        double projectionMarginX = std::abs(skewLength * cos(skewAngleRad)) * availableHeight * 0.1;
        double projectionMarginY = std::abs(skewLength * sin(skewAngleRad)) * availableWidth * 0.1;
        
        // Apply safety factor and subtract projection margins
        int sampleSpaceX = static_cast<int>((availableWidth - projectionMarginX) * adaptiveSafetyFactor);
        int sampleSpaceY = static_cast<int>((availableHeight - projectionMarginY) * adaptiveSafetyFactor);
        
        // Debug the adaptive safety factor calculation (reduced verbosity for production)
        auto safetyLogger = dlog::Logger::getInstance();
        safetyLogger->debug("=== Adaptive Safety Factor ===");
        safetyLogger->debug("Available: {0}x{1}, Safety: {2:.1f}%, Margins: {3:.1f}x{4:.1f}, Sample: {5}x{6}", 
                           availableWidth, availableHeight, adaptiveSafetyFactor * 100, 
                           projectionMarginX, projectionMarginY, sampleSpaceX, sampleSpaceY);
        
        // Ensure sample space is reasonable and aligned with step size
        int sampleSpaceX_beforeAlign = sampleSpaceX;
        int sampleSpaceY_beforeAlign = sampleSpaceY;
        sampleSpaceX = (sampleSpaceX / stepSize.x) * stepSize.x; // Align to step size
        sampleSpaceY = (sampleSpaceY / stepSize.y) * stepSize.y; // Align to step size
        safetyLogger->debug("After step alignment (step %dx%d): %dx%d", stepSize.x, stepSize.y, sampleSpaceX, sampleSpaceY);
        
        // Ensure minimum size for reasonable detail, but make it adaptive to available space
        // Use a percentage of available space rather than fixed values to be more generic
        int minSampleSpaceX = std::max(static_cast<int>(availableWidth * 0.15), stepSize.x * 10); // At least 15% or 10 steps
        int minSampleSpaceY = std::max(static_cast<int>(availableHeight * 0.12), stepSize.y * 8);  // At least 12% or 8 steps
        
        sampleSpaceX = std::max(sampleSpaceX, minSampleSpaceX);
        sampleSpaceY = std::max(sampleSpaceY, minSampleSpaceY);
        safetyLogger->debug("Final sample space: {0}x{1} (mins: {2}x{3})", 
                           sampleSpaceX, sampleSpaceY, minSampleSpaceX, minSampleSpaceY);
        
        if (sampleSpaceX != sampleSpaceX_beforeAlign || sampleSpaceY != sampleSpaceY_beforeAlign) {
            safetyLogger->debug("Step size alignment changed sample space from {0}x{1} to {2}x{3}", 
                               sampleSpaceX_beforeAlign, sampleSpaceY_beforeAlign, sampleSpaceX, sampleSpaceY);
        }
        
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
        auto logger = dlog::Logger::getInstance();
        
        logger->debug("=== Using getAllowedDrawingArea() for Chart Sizing ===");
        logger->debug("Canvas size: {0}x{1}", canvasSize_.x, canvasSize_.y);
        logger->debug("Chart size: {0}x{1}", chartSize.x, chartSize.y);
        logger->debug("Screen offset: ({0}, {1})", screenOffsetX, screenOffsetY);
        logger->debug("Allowed drawing area: ({0},{1}) to ({2},{3})", availableLeft, availableTop, availableRight, availableBottom);
        logger->debug("Allowed area dimensions: {0}x{1}", availableRight - availableLeft + 1, availableBottom - availableTop + 1);

        // Calculate the center of the allowed drawing area for proper chart centering
        // Add a small margin within the allowed area to prevent touching edges
        const double marginPercent = 0.05; // 5% margin within allowed area
        int marginX = static_cast<int>(availableWidth * marginPercent);
        int marginY = static_cast<int>(availableHeight * marginPercent);
        
        // Ensure minimum margins for very small areas
        marginX = std::max(marginX, 3);
        marginY = std::max(marginY, 3);
        
        // Create target area with margins for chart centering
        int targetLeft = availableLeft + marginX;
        int targetRight = availableRight - marginX;
        int targetTop = availableTop + marginY;
        int targetBottom = availableBottom - marginY;
        
        double availableCenterX = (targetLeft + targetRight) / 2.0;
        double availableCenterY = (targetTop + targetBottom) / 2.0;
        
        logger->debug("=== Chart Centering with Margins ===");
        logger->debug("Allowed area: ({0},{1}) to ({2},{3})", availableLeft, availableTop, availableRight, availableBottom);
        logger->debug("Margin: {0}x{1} pixels ({2:.1f}% of available area)", marginX, marginY, marginPercent * 100);
        logger->debug("Target area: ({0},{1}) to ({2},{3})", targetLeft, targetTop, targetRight, targetBottom);

        // Original centering calculation (for reference)
        [[maybe_unused]] double xe0_old = midSampleSpaceX + sampleScaleForX * midSampleSpaceY + screenOffsetX;
        [[maybe_unused]] double ye0_old = sampleScaleForY * midSampleSpaceY + screenOffsetY;
        
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
        // Direct centering using allowed area center without empirical offsets
        double z_center = resultScale * getCenterZ();  // Use user-provided center Z value
        double xe0 = availableCenterX;
        double ye0 = maxY - availableCenterY - z_center;  // Direct centering without empirical offset
        
        logger->debug("=== Chart Centering Debug ===");
        logger->debug("Available space: ({0},{1}) to ({2},{3})", availableLeft, availableTop, availableRight, availableBottom);
        logger->debug("Target space center: ({0:.1f}, {1:.1f})", availableCenterX, availableCenterY);
        logger->debug("Sample space: {0}x{1} (mid: {2},{3})", sampleSpaceX, sampleSpaceY, midSampleSpaceX, midSampleSpaceY);
        logger->debug("Center Z value: {0:.2f} (scaled: {1:.2f})", getCenterZ(), z_center);
        logger->debug("maxY: {0}", maxY);
        logger->debug("Chart origin: xe0={0:.1f}, ye0={1:.1f}", xe0, ye0);
        logger->debug("Expected pixel center: ({0:.1f}, {1:.1f})", xe0, maxY - (ye0 + z_center));
        logger->debug("Centering within target area with margins to prevent edge touching");
        logger->debug("Sample space Y range: q=[{0},{1}], actual Y input range: [{2:.3f},{3:.3f}]",
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

        int e;
        std::vector<int> h_up(static_cast<size_t>(sizeX)), h_down(static_cast<size_t>(sizeX));

        // Assert: sizeX must be positive and finite for the loop to terminate
        assert(sizeX > 0 && sizeX < 1000000 && "sizeX must be positive and reasonable for finite loop");
        // Assert: sampleStepY must be positive and sampleSpaceY must be positive
        assert(sampleStepY > 0 && sampleSpaceY > 0 && "sampleStepY and sampleSpaceY must be positive for finite loop");
        // Assert: sampleStepX must be positive and sampleSpaceX must be positive
        assert(sampleStepX > 0 && sampleSpaceX > 0 && "sampleStepX and sampleSpaceX must be positive for finite loop");

        for (int i = 0; i < sizeX; ++i) {
            h_up[static_cast<size_t>(i)] = INT_MIN;
            h_down[static_cast<size_t>(i)] = INT_MAX;
        }

        // Assert: outer loop q will terminate
        assert(midSampleSpaceY >= 0 && sampleStepY > 0 && "midSampleSpaceY and sampleStepY must be non-negative and positive");
        
        // Line debugging infrastructure for tracking pixel positions
        struct LineDebugInfo {
            double startX, startY;      // Start coordinates of the line
            double endX, endY;          // End coordinates of the line  
            double minY, maxY;          // Y range of the line
            bool hasData;               // Whether this line has any data
            
            LineDebugInfo() : startX(0), startY(0), endX(0), endY(0), 
                            minY(std::numeric_limits<double>::max()), 
                            maxY(std::numeric_limits<double>::lowest()), 
                            hasData(false) {}
        };
        
        // Identify which q values correspond to first, middle, and last lines
        // Use the actual q values that will be hit in the loop instead of exact target values
        int firstQ = -midSampleSpaceY;  // This will always be hit (loop starts here)
        int middleQ = INT_MAX;  // Will find the closest to 0
        int lastQ = INT_MIN;    // Will find the actual last q value processed
        
        // Find the q value closest to 0 and the actual last q value that will be hit in the loop
        int bestMiddleDistance = INT_MAX;
        for (int q = -midSampleSpaceY; q <= midSampleSpaceY; q += sampleStepY) {
            // Track closest to middle (0)
            int distance = abs(q);
            if (distance < bestMiddleDistance) {
                bestMiddleDistance = distance;
                middleQ = q;
            }
            // Track actual last q value
            lastQ = q;  // This will be the last value when the loop ends
        }
        
        LineDebugInfo firstLineInfo, middleLineInfo, lastLineInfo;
        LineDebugInfo* currentLineInfo = nullptr;
        
        auto tracer = Chart3DTracer::getInstance();
        tracer->trace("Starting outer loop over q (Y axis) in paint()");
        
        // Debug: Log the actual q values that will be processed
        logger->debug("=== Loop Range Analysis ===");
        logger->debug("Sample space Y: midSampleSpaceY=%d, sampleStepY=%d", midSampleSpaceY, sampleStepY);
        logger->debug("Target lines: first=%d, middle=%d, last=%d", firstQ, middleQ, lastQ);
        int qCount = 0;
        for (int q = -midSampleSpaceY; q <= midSampleSpaceY; q += sampleStepY) {
            qCount++;
            if (qCount <= 3 || q == middleQ || q >= midSampleSpaceY - sampleStepY) {
                logger->debug("  q=%d (iteration %d)", q, qCount);
            }
        }
        logger->debug("Total q iterations: %d", qCount);
        for (int q = -midSampleSpaceY; q <= midSampleSpaceY; q += sampleStepY) {
            // Identify which line we're tracking for debugging
            currentLineInfo = nullptr;
            if (q == firstQ) {
                currentLineInfo = &firstLineInfo;
            } else if (q == middleQ) {
                currentLineInfo = &middleLineInfo;
            } else if (q == lastQ) {
                currentLineInfo = &lastLineInfo;
            }
            
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
                
                // Clamp coordinates to stay within allowed boundaries
                // This is a safety net to prevent any boundary violations
                xe = std::max(static_cast<double>(availableLeft), std::min(static_cast<double>(availableRight), xe));
                ye = std::max(static_cast<double>(availableTop), std::min(static_cast<double>(availableBottom), ye));
                
                tracer->trace("  m=%d, x=%f, z=%f, xe=%f, ye=%f", m, x, z, xe, ye);

                // Debug: Check point closest to center (m and q closest to 0)
                if (abs(m) <= 3 && abs(q) <= 5) {
                    auto debugLogger = dlog::Logger::getInstance();
                    debugLogger->debug("NEAR CENTER: m=%d,q=%d input(%.3f,%.3f) -> z=%.3f -> screen(%.1f,%.1f) -> final_pixel(%.1f,%.1f)", 
                                 m, q, x, y, z/resultScale, xe, ye, xe, maxY - ye);
                }
                
                // Special debug for exact mathematical center
                if (m == 0 && q == 0) {
                    auto centerLogger = dlog::Logger::getInstance();
                    centerLogger->debug("=== EXACT MATHEMATICAL CENTER ===");
                    centerLogger->debug("Input coordinates: x=%.6f, y=%.6f", x, y);
                    centerLogger->debug("Function value: z=%.6f (scaled: %.2f)", z/resultScale, z);
                    centerLogger->debug("Screen coordinates: xe=%.2f, ye=%.2f", xe, ye);
                    centerLogger->debug("Final pixel: (%.1f, %.1f)", xe, maxY - ye);
                    centerLogger->debug("Expected center: (%.1f, %.1f)", availableCenterX, availableCenterY);
                    centerLogger->debug("Center offset: X=%.1f, Y=%.1f", xe - availableCenterX, (maxY - ye) - availableCenterY);
                }

                if (m <= -midSampleSpaceX) {
                    f1 = 0;
                    e = static_cast<int>(round(xe / sampleStepX));

                    if ((e < sizeX) && (ye >= h_up[static_cast<size_t>(e)])) {
                        f1 = 1;
                        h_up[static_cast<size_t>(e)] = static_cast<int>(round(ye));
                    } 
                    
                    if ((e < sizeX) && (ye <= h_down[static_cast<size_t>(e)])) {
                        f1 = 1;
                        h_down[static_cast<size_t>(e)] = static_cast<int>(round(ye));
                    }

                    x1 = xe;
                    y1 = ye;
                }
                else {
                    f2 = 0;
                    e = static_cast<int>(round(xe / sampleStepX));

                    if ((e < sizeX) && (ye >= h_up[static_cast<size_t>(e)])) {
                        f2 = 1;
                        h_up[static_cast<size_t>(e)] = static_cast<int>(round(ye));
                    }

                    if ((e < sizeX) && (ye <= h_down[static_cast<size_t>(e)])) {
                        f2 = 1;
                        h_down[static_cast<size_t>(e)] = static_cast<int>(round(ye));
                    }

                    x2 = xe; y2 = ye;

                    if (f1 * f2 == 1) {
                        color = getPlotColor(x, y, z);

                        // Ensure final pixel coordinates are within allowed bounds before drawing
                        double finalX1 = x1;
                        double finalY1 = maxY - y1;
                        double finalX2 = x2;
                        double finalY2 = maxY - y2;
                        
                        // Clamp final pixel coordinates to allowed area
                        finalX1 = std::max(static_cast<double>(availableLeft), std::min(static_cast<double>(availableRight), finalX1));
                        finalY1 = std::max(static_cast<double>(availableTop), std::min(static_cast<double>(availableBottom), finalY1));
                        finalX2 = std::max(static_cast<double>(availableLeft), std::min(static_cast<double>(availableRight), finalX2));
                        finalY2 = std::max(static_cast<double>(availableTop), std::min(static_cast<double>(availableBottom), finalY2));

                        // Draw pixel and line only if they're within bounds
                        pixelPainter->putPixel(static_cast<unsigned int>(finalX1), static_cast<unsigned int>(finalY1), color);
                        lnPainter->drawLine(static_cast<unsigned int>(finalX1), static_cast<unsigned int>(finalY1), static_cast<unsigned int>(finalX2), static_cast<unsigned int>(finalY2), color);
                        
                        // Track pixel positions for line debugging if this is a tracked line
                        if (currentLineInfo != nullptr) {
                            if (!currentLineInfo->hasData) {
                                // First pixel of this line
                                currentLineInfo->startX = finalX1;
                                currentLineInfo->startY = finalY1;
                                currentLineInfo->endX = finalX2;
                                currentLineInfo->endY = finalY2;
                                currentLineInfo->minY = std::min(finalY1, finalY2);
                                currentLineInfo->maxY = std::max(finalY1, finalY2);
                                currentLineInfo->hasData = true;
                            } else {
                                // Update end coordinates and Y range
                                currentLineInfo->endX = finalX2;
                                currentLineInfo->endY = finalY2;
                                currentLineInfo->minY = std::min(currentLineInfo->minY, std::min(finalY1, finalY2));
                                currentLineInfo->maxY = std::max(currentLineInfo->maxY, std::max(finalY1, finalY2));
                            }
                        }
                    } else if (currentLineInfo != nullptr) {
                        // Debug: Track why certain lines don't get data rendered
                        static int debugCount = 0;
                        if (debugCount < 3) { // Limit debug output to avoid spam
                            logger->debug("Line q=%d: f1*f2=%d (f1=%d, f2=%d) - no rendering", q, f1*f2, f1, f2);
                            debugCount++;
                        }
                    }

                    x1 = x2; 
                    y1 = y2;
                    f1 = f2;
                }
            }
            tracer->trace(" Finished inner loop for q=%d", q);
        }
        tracer->trace("Finished outer loop in paint()");
        
        // Output line position debugging information
        logger->debug("=== Line Position Debug Information ===");
        logger->debug("Rectangle range of available chart space: (%d,%d) to (%d,%d)", 
                     availableLeft, availableTop, availableRight, availableBottom);
        logger->debug("Chart space dimensions: %dx%d pixels", 
                     availableRight - availableLeft + 1, availableBottom - availableTop + 1);
        logger->debug("Chart space center: (%.1f, %.1f)", 
                     (availableLeft + availableRight) / 2.0, (availableTop + availableBottom) / 2.0);
        
        if (firstLineInfo.hasData) {
            logger->debug("FIRST line (q=%d): start(%.1f,%.1f) -> end(%.1f,%.1f), Y range[%.1f,%.1f]", 
                         firstQ, firstLineInfo.startX, firstLineInfo.startY, 
                         firstLineInfo.endX, firstLineInfo.endY,
                         firstLineInfo.minY, firstLineInfo.maxY);
        } else {
            logger->debug("FIRST line (q=%d): No data rendered", firstQ);
        }
        
        if (middleLineInfo.hasData) {
            logger->debug("MIDDLE line (q=%d): start(%.1f,%.1f) -> end(%.1f,%.1f), Y range[%.1f,%.1f]", 
                         middleQ, middleLineInfo.startX, middleLineInfo.startY,
                         middleLineInfo.endX, middleLineInfo.endY,
                         middleLineInfo.minY, middleLineInfo.maxY);
        } else {
            logger->debug("MIDDLE line (q=%d): No data rendered", middleQ);
        }
        
        if (lastLineInfo.hasData) {
            logger->debug("LAST line (q=%d): start(%.1f,%.1f) -> end(%.1f,%.1f), Y range[%.1f,%.1f]", 
                         lastQ, lastLineInfo.startX, lastLineInfo.startY,
                         lastLineInfo.endX, lastLineInfo.endY,
                         lastLineInfo.minY, lastLineInfo.maxY);
        } else {
            logger->debug("LAST line (q=%d): No data rendered", lastQ);
        }
        
        // Log chart sizing effectiveness before validation
        logger->debug("=== Chart Sizing Verification ===");
        logger->debug("Final allowed area: (%d,%d) to (%d,%d)", availableLeft, availableTop, availableRight, availableBottom);
        logger->debug("Allowed area size: %dx%d", availableRight - availableLeft + 1, availableBottom - availableTop + 1);
        logger->debug("Canvas utilization: %.1f%% x %.1f%% y", 
                     100.0 * (availableRight - availableLeft + 1) / canvasSize_.x,
                     100.0 * (availableBottom - availableTop + 1) / canvasSize_.y);
        
        validatePixelRange(availableLeft, availableRight, availableTop, availableBottom);
    }

protected:
    virtual RgbColor getPlotColor([[maybe_unused]] double x, [[maybe_unused]] double y, [[maybe_unused]] double z) {
        return{ 255, 255, 255 };
    }

    // returns function value
    virtual double getFunValue(double x, double y) = 0;

    // Draw border around the chart area
    virtual void drawChartBorder(LinePainterForPixels* linePainter, [[maybe_unused]] int offsetX, [[maybe_unused]] int offsetY, [[maybe_unused]] int sizeX, [[maybe_unused]] int sizeY, [[maybe_unused]] int maxY) {
        if (!linePainter) return;
        
        RgbColor borderColor = getBorderColor();
        
        // Calculate border coordinates with adaptive margin based on canvas size
        const double borderMarginPercent = 0.02; // 2% of canvas size for border margin
        int borderMargin = std::max(static_cast<int>(std::min(canvasSize_.x, canvasSize_.y) * borderMarginPercent), 8);
        
        int left = borderMargin;
        int right = canvasSize_.x - borderMargin - 1;
        int top = borderMargin;
        int bottom = canvasSize_.y - borderMargin - 1;
        
        // Draw rectangle border
        linePainter->drawLine(static_cast<unsigned int>(left), static_cast<unsigned int>(top), static_cast<unsigned int>(right), static_cast<unsigned int>(top), borderColor);       // top edge
        linePainter->drawLine(static_cast<unsigned int>(right), static_cast<unsigned int>(top), static_cast<unsigned int>(right), static_cast<unsigned int>(bottom), borderColor);   // right edge  
        linePainter->drawLine(static_cast<unsigned int>(right), static_cast<unsigned int>(bottom), static_cast<unsigned int>(left), static_cast<unsigned int>(bottom), borderColor); // bottom edge
        linePainter->drawLine(static_cast<unsigned int>(left), static_cast<unsigned int>(bottom), static_cast<unsigned int>(left), static_cast<unsigned int>(top), borderColor);     // left edge
    }

    /**
     * @brief Validate that chart pixels were drawn within the available space
     * @param availableLeft Left boundary of available space
     * @param availableRight Right boundary of available space  
     * @param availableTop Top boundary of available space
     * @param availableBottom Bottom boundary of available space
     */
    virtual void validatePixelRange(int availableLeft, int availableRight, int availableTop, int availableBottom) {
        auto logger = dlog::Logger::getInstance();
        
        // After chart drawing is complete, validate pixel usage against available space
        PixelTracingFilter* tracingFilter = dynamic_cast<PixelTracingFilter*>(&pixelPainter_);
        if (tracingFilter != nullptr && tracingFilter->hasPixels()) {
            unsigned int usedMinX, usedMinY, usedMaxX, usedMaxY;
            if (tracingFilter->getPixelRange(usedMinX, usedMinY, usedMaxX, usedMaxY)) {
                logger->debug("=== Pixel Usage Analysis ===");
                logger->debug("Chart actually used area: (%d,%d) to (%d,%d)", usedMinX, usedMinY, usedMaxX, usedMaxY);
                logger->debug("Chart used dimensions: %dx%d", usedMaxX - usedMinX + 1, usedMaxY - usedMinY + 1);
                logger->debug("Available area: (%d,%d) to (%d,%d)", availableLeft, availableTop, availableRight, availableBottom);
                
                // Calculate utilization
                int usedWidth = usedMaxX - usedMinX + 1;
                int usedHeight = usedMaxY - usedMinY + 1;
                int availableWidth = availableRight - availableLeft + 1;
                int availableHeight = availableBottom - availableTop + 1;
                
                logger->debug("Space utilization: %.1f%% width, %.1f%% height", 
                             100.0 * usedWidth / availableWidth, 
                             100.0 * usedHeight / availableHeight);
                
                // Check if pixels were drawn outside available space
                bool outOfBounds = false;
                if (usedMinX < availableLeft || usedMaxX > availableRight || 
                    usedMinY < availableTop || usedMaxY > availableBottom) {
                    outOfBounds = true;
                    
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
                } else {
                    logger->debug("SUCCESS: All chart pixels drawn within allowed area!");
                    
                    // Show margins to help understand space usage
                    int leftMargin = usedMinX - availableLeft;
                    int rightMargin = availableRight - usedMaxX;
                    int topMargin = usedMinY - availableTop;
                    int bottomMargin = availableBottom - usedMaxY;
                    
                    logger->debug("Margins: left=%d, right=%d, top=%d, bottom=%d", 
                                 leftMargin, rightMargin, topMargin, bottomMargin);
                    
                    // Final success summary
                    if (leftMargin == 0 && rightMargin == 0 && topMargin == 0 && bottomMargin == 0) {
                        logger->debug("PERFECT: Chart utilizes 100%% of available space with zero boundary violations!");
                    }
                }
            }
        } else {
            logger->debug("No pixel tracing available for validation");
        }
    }

private:
    /**
     * @brief Draw the allowed drawing area rectangle (for debugging or visualization)
     */
    void drawAllowedArea(const RectInclusive& area, const RgbColor& color, PixelPainter* painter) {
        if (!painter) return;
        // Draw rectangle for allowed area
        int left = area.x1;
        int right = area.x2;
        int top = area.y1;
        int bottom = area.y2;
        LinePainterForPixels linePainter(*painter);
        linePainter.drawLine(left, top, right, top, color);       // top edge
        linePainter.drawLine(right, top, right, bottom, color);   // right edge
        linePainter.drawLine(right, bottom, left, bottom, color); // bottom edge
        linePainter.drawLine(left, bottom, left, top, color);     // left edge
    }

    /**
     * @brief Ownership wrapper for LineClippingPainter to manage memory safely
     */
    class LineClippingPainterOwner : public LinePainter {
    public:
        LineClippingPainterOwner(std::unique_ptr<LinePainter> basePainter, const RectInclusive& clippingWindow)
            : basePainter_(std::move(basePainter)), clippingPainter_(*basePainter_, clippingWindow) {}
        
        virtual ~LineClippingPainterOwner() {}
        
        virtual void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor& color) override {
            clippingPainter_.drawLine(x1, y1, x2, y2, color);
        }
        
    private:
        std::unique_ptr<LinePainter> basePainter_; // Owns the base painter
        LineClippingPainter clippingPainter_;     // Uses the existing LineClippingPainter
    };

    /**
     * @brief Create and configure line painter based on anti-aliasing setting
     * @param pixelPainter The pixel painter to use as base
     * @param allowedArea The clipping area for the line painter
     * @return Configured line painter with clipping functionality
     */
    std::unique_ptr<LinePainter> createLinePainter(PixelPainter* pixelPainter, const RectInclusive& allowedArea) {
        std::unique_ptr<LinePainter> lnActualPainter;
        
        // Create appropriate line painter based on anti-aliasing setting
        if (useAntiAliasing_) {
            lnActualPainter.reset(
                    new AntiAliasedLinePainterForPixels(*pixelPainter)
            );
            std::cerr << "Using anti-aliased line painter with clipping" << std::endl;
        } else {
            lnActualPainter.reset(
                    new LinePainterForPixels(*pixelPainter)
            );
            std::cerr << "Using standard line painter with clipping" << std::endl;
        }

        // Wrap with clipping functionality using the existing LineClippingPainter
        std::unique_ptr<LinePainter> clippingPainter(
            new LineClippingPainterOwner(std::move(lnActualPainter), allowedArea)
        );
        
        return clippingPainter;
    }

    PixelPainter &pixelPainter_;
    Point canvasSize_;
    bool useAntiAliasing_;
    bool drawBorders_;
};

#endif
