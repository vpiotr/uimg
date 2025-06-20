#ifndef __UIMG_ANTIALIASED_PAINTER_FOR_PIXELS_H__
#define __UIMG_ANTIALIASED_PAINTER_FOR_PIXELS_H__

#include <algorithm>
#include <cmath>

#include "uimg/base/structs.h"
#include "uimg/pixels/pixel_painter.h"
#include "uimg/utils/math_utils.h"
#include "uimg/painters/painter_for_pixels.h"
#include "uimg/filters/filter_for_pixels.h"

// Anti-aliased line drawer using Wu's algorithm
class AntiAliasedLinePainterForPixels : public LinePainterForPixels {
public:
    AntiAliasedLinePainterForPixels(PixelPainter &pixelPainter) 
        : LinePainterForPixels(pixelPainter), pixelPainter_(&pixelPainter) {}

    // Enhanced implementation of Xiaolin Wu's line algorithm
    virtual void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        // Handle special cases - vertical and horizontal lines
        if (x1 == x2) {
            // Use regular vertical line drawing to avoid artifacts
            LinePainterForPixels::drawVerticalLine(x1, y1, y2, color);
            return;
        } else if (y1 == y2) {
            // Use regular horizontal line drawing to avoid artifacts
            LinePainterForPixels::drawHorizontalLine(x1, x2, y1, color);
            return;
        }

        // Convert to signed integers for safer calculations
        int ix1 = static_cast<int>(x1);
        int iy1 = static_cast<int>(y1);
        int ix2 = static_cast<int>(x2);
        int iy2 = static_cast<int>(y2);

        // Determine if the line is steep (|dy| > |dx|)
        // If steep, we transpose the line to make it less steep
        bool steep = std::abs(iy2 - iy1) > std::abs(ix2 - ix1);
        
        if (steep) {
            std::swap(ix1, iy1);
            std::swap(ix2, iy2);
        }
        
        // Ensure line is always drawn left to right for algorithm consistency
        if (ix1 > ix2) {
            std::swap(ix1, ix2);
            std::swap(iy1, iy2);
        }

        // Convert back to unsigned for drawing
        int dx = ix2 - ix1;
        int dy = iy2 - iy1;
        
        // Calculate the gradient (slope)
        float gradient = (dx == 0) ? 1.0f : static_cast<float>(dy) / static_cast<float>(dx);

        // Handle first endpoint
        int xend = static_cast<int>(std::round(ix1));
        float yend = static_cast<float>(iy1) + gradient * (static_cast<float>(xend) - static_cast<float>(ix1));
        float xgap = 1.0f - fractionalPart(static_cast<float>(ix1) + 0.5f);
        int xpxl1 = xend;
        int ypxl1 = static_cast<int>(yend); // Integer part of y

        // Calculate fractional parts for anti-aliasing
        float fpart_yend = fractionalPart(yend);
        float rfpart_yend = 1.0f - fpart_yend;
        
        // Draw first endpoint
        if (steep) {
            // If the line is steep, x and y are swapped
            plotPixel(static_cast<unsigned int>(ypxl1), static_cast<unsigned int>(xpxl1), color, rfpart_yend * xgap);
            plotPixel(static_cast<unsigned int>(ypxl1 + 1), static_cast<unsigned int>(xpxl1), color, fpart_yend * xgap);
        } else {
            plotPixel(static_cast<unsigned int>(xpxl1), static_cast<unsigned int>(ypxl1), color, rfpart_yend * xgap);
            plotPixel(static_cast<unsigned int>(xpxl1), static_cast<unsigned int>(ypxl1 + 1), color, fpart_yend * xgap);
        }

        // Initialize the y-coordinate for the main loop
        // This is the y-value of the first pixel in the main loop
        float intery = yend + gradient;

        // Handle second endpoint
        xend = std::round(ix2);
        yend = iy2 + gradient * (xend - ix2);
        xgap = fractionalPart(ix2 + 0.5f);
        int xpxl2 = xend;
        int ypxl2 = static_cast<int>(yend);

        // Calculate fractional parts for the second endpoint
        float fpart_yend2 = fractionalPart(yend);
        float rfpart_yend2 = 1.0f - fpart_yend2;
        
        // Second endpoint
        if (steep) {
            plotPixel(ypxl2, xpxl2, color, rfpart_yend2 * xgap);
            plotPixel(ypxl2 + 1, xpxl2, color, fpart_yend2 * xgap);
        } else {
            plotPixel(xpxl2, ypxl2, color, rfpart_yend2 * xgap);
            plotPixel(xpxl2, ypxl2 + 1, color, fpart_yend2 * xgap);
        }

        // Main loop for drawing the line
        if (steep) {
            // If steep, we traverse along the transposed x-axis (originally y)
            for (int x = xpxl1 + 1; x < xpxl2; x++) {
                int y_base = static_cast<int>(intery);
                float fpart = fractionalPart(intery);
                float rfpart = 1.0f - fpart;
                
                // Draw two neighboring pixels with appropriate weights
                plotPixel(y_base, x, color, rfpart);
                plotPixel(y_base + 1, x, color, fpart);
                
                // Increment y-coordinate by gradient
                intery += gradient;
            }
        } else {
            // If not steep, we traverse normally along the x-axis
            for (int x = xpxl1 + 1; x < xpxl2; x++) {
                int y_base = static_cast<int>(intery);
                float fpart = fractionalPart(intery);
                float rfpart = 1.0f - fpart;
                
                // Draw two neighboring pixels with appropriate weights
                plotPixel(x, y_base, color, rfpart);
                plotPixel(x, y_base + 1, color, fpart);
                
                // Increment y-coordinate by gradient
                intery += gradient;
            }
        }
    }

protected:
    // Enhanced helper method to draw a pixel with alpha blending and intensity boost
    void plotPixel(unsigned int x, unsigned int y, const RgbColor &color, float alpha) {
        // Skip if alpha is too small to be visible
        if (alpha <= 0.02f) return;
        
        // Don't boost alpha too much as it can cause artifacts
        // Keep alpha more natural for better anti-aliasing
        alpha = std::min(1.0f, alpha);

        // Full opacity, just use the direct pixel value
        if (alpha >= 0.99f) {
            pixelPainter_->putPixel(x, y, color);
            return;
        }

        // Get existing color for alpha blending
        RgbColor existingColor;
        pixelPainter_->getPixel(x, y, existingColor);
        
        // Blend the colors with enhanced contrast
        RgbColor blendedColor;
        blendedColor.red = std::min(255, static_cast<int>(round(alpha * color.red + (1.0f - alpha) * existingColor.red)));
        blendedColor.green = std::min(255, static_cast<int>(round(alpha * color.green + (1.0f - alpha) * existingColor.green)));
        blendedColor.blue = std::min(255, static_cast<int>(round(alpha * color.blue + (1.0f - alpha) * existingColor.blue)));
        
        // Put the blended pixel
        pixelPainter_->putPixel(x, y, blendedColor);
    }

    // Extract the fractional part of a float
    float fractionalPart(float x) {
        return x - std::floor(x);
    }

    // Special anti-aliased horizontal line drawing
    // This is used instead of the standard horizontal line drawing to avoid artifacts
    void drawAntiAliasedHorizontalLine(unsigned int x1, unsigned int x2, unsigned int y, const RgbColor &color) {
        if (x1 > x2) {
            std::swap(x1, x2);
        }
        
        // We need to use regular horizontal line drawing to avoid introducing artifacts
        // Skip the anti-aliasing effect for purely horizontal lines
        LinePainterForPixels::drawHorizontalLine(x1, x2, y, color);
    }

private:
    PixelPainter *pixelPainter_;
};

// Anti-aliased thick line painter
class AntiAliasedThickLinePainterForPixels : public ThickLinePainterForPixels {
public:
    AntiAliasedThickLinePainterForPixels(PixelPainter &pixelPainter, float lineWidth, int pointCount = -1)
        : ThickLinePainterForPixels(pixelPainter, lineWidth, pointCount) {}

    virtual void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        // For thick lines, we'll use the same approach as ThickLinePainterForPixels
        // but with anti-aliasing applied at the circle edges
        int dx = x2 - x1;
        int dy = y2 - y1;

        float lineWidthD2f = lineWidth_ / 2.0f;
        unsigned int lineWidthD2 = static_cast<unsigned>(round(lineWidthD2f));

        int cnt = calcPointCount(x1, y1, x2, y2);

        float fdx = static_cast<float>(dx) / cnt;
        float fdy = static_cast<float>(dy) / cnt;

        float x = static_cast<float>(x1);
        float y = static_cast<float>(y1);

        for (int i = 0; i < cnt; ++i) {
            // Draw anti-aliased circle at each point
            drawAntiAliasedCircle(
                static_cast<unsigned int>(round(x)), 
                static_cast<unsigned int>(round(y)),
                lineWidthD2, 
                color
            );
            
            x += fdx;
            y += fdy;
        }
    }

protected:
    // Draw an anti-aliased circle with improved edge smoothing
    void drawAntiAliasedCircle(unsigned int x, unsigned int y, unsigned int radius, const RgbColor &color) {
        // We'll use the base class's usedCirclePainter_ to draw the main circle
        // and then manually add anti-aliasing at the edges
        if (radius <= 1) {
            pixelPainter_.putPixel(x, y, color);
            return;
        }

        // Draw the inner circle (radius-1) with full color
        usedCirclePainter_.drawFull(x, y, radius-1, color);

        // Draw the edge pixels with distance-based alpha blending for better anti-aliasing
        float radiusF = static_cast<float>(radius);
        float radiusInnerF = radiusF - 1.0f;
        
        // Draw the edge pixels with distance-based alpha
        for (int yi = -static_cast<int>(radius); yi <= static_cast<int>(radius); yi++) {
            for (int xi = -static_cast<int>(radius); xi <= static_cast<int>(radius); xi++) {
                // Calculate the exact distance from center
                float distF = std::sqrt(xi*xi + yi*yi);
                
                // Skip pixels clearly inside or outside the circle
                if (distF < radiusInnerF || distF > radiusF + 0.5f) 
                    continue;
                
                // Calculate alpha based on distance from the edge
                // Pixels exactly at radius have alpha 0.5
                // Linear transition for smoother appearance
                float alpha;
                if (distF <= radiusF) {
                    // Inside edge - fade from solid to 0.5 at exact radius
                    alpha = 1.0f - 0.5f * ((distF - radiusInnerF) / (radiusF - radiusInnerF));
                } else {
                    // Outside edge - fade from 0.5 to 0 
                    alpha = 0.5f * (1.0f - (distF - radiusF) / 0.5f); 
                }
                
                // Ensure alpha is in valid range
                alpha = std::max(0.0f, std::min(1.0f, alpha));
                
                // Skip trivial cases
                if (alpha < 0.05f)
                    continue;
                    
                // Get existing color for blending
                unsigned int px = x + xi;
                unsigned int py = y + yi;
                
                RgbColor existingColor;
                pixelPainter_.getPixel(px, py, existingColor);
                
                RgbColor blendedColor;
                blendedColor.red = std::min(255, static_cast<int>(round(alpha * color.red + (1.0f - alpha) * existingColor.red)));
                blendedColor.green = std::min(255, static_cast<int>(round(alpha * color.green + (1.0f - alpha) * existingColor.green)));
                blendedColor.blue = std::min(255, static_cast<int>(round(alpha * color.blue + (1.0f - alpha) * existingColor.blue)));
                
                pixelPainter_.putPixel(px, py, blendedColor);
            }
        }
    }
};

#endif // __UIMG_ANTIALIASED_PAINTER_FOR_PIXELS_H__
