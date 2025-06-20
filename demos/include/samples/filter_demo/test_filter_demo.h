#ifndef __UIMG_FILTER_DEMO_H__
#define __UIMG_FILTER_DEMO_H__

#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include "uimg/base/structs.h"
#include "uimg/filters/filter_base.h"
#include "uimg/filters/filter_for_pixels.h"
#include "samples/test_painter_base.h"

namespace uimg {

/**
 * @brief Demo application to showcase all available pixel filters
 */
class TestFilterDemo : public test_painter_base {
public:
    // Define section window constants for each filter demo section
    static const RectInclusive ORIGINAL_WINDOW;
    static const RectInclusive HFLIP_WINDOW;
    static const RectInclusive VFLIP_WINDOW;
    static const RectInclusive ROTATION_WINDOW;
    static const RectInclusive TRANSPARENCY_WINDOW;
    static const RectInclusive ALPHA_WINDOW;
    static const RectInclusive ZOOM_OUT_WINDOW;
    static const RectInclusive ZOOM_IN_WINDOW;
    static const RectInclusive LINEAR_GRADIENT_WINDOW;
    static const RectInclusive RADIAL_GRADIENT_WINDOW;
    
    TestFilterDemo(const std::string &outFileName)
        : test_painter_base(outFileName) {}

    virtual ~TestFilterDemo() {}

protected:
    virtual Point getImageSize() override {
        return {800, 800}; // Larger image for filter demonstration
    }

    // This method is replaced by drawSectionTitles() that handles all section titles
    void drawSimpleTitles() {
        // Method kept for backward compatibility but not used
    }
    
    virtual void paint() override {
        // Create a nice source image with geometric shapes and gradients
        createSourceImage();
        
        // Draw all filter demos
        drawTransformationFilters();
        drawTransparencyFilters();
        drawZoomFilters();
        drawGradientFilters();
        
        // Add section titles and borders
        drawSections();
        drawMainTitle();
    }

private:
    void createSourceImage() {
        // Set background color
        RgbImage &img = getImage();

        // fill image with background painter
        BackgroundPainterForRgbImage bgPainter(img);
        bgPainter.paint(RgbColor{240, 240, 240}); // Light gray background
        
        // Draw some basic shapes that will be used for filter demonstrations
        drawSourceShapes();
    }

    void drawSourceShapes() {
        [[maybe_unused]] RgbImage &img = getImage();
        PixelPainter &painter = getPainter();

        int x0 = ORIGINAL_WINDOW.x1, y0 = ORIGINAL_WINDOW.y1;
        int x1 = ORIGINAL_WINDOW.x2, y1 = ORIGINAL_WINDOW.y2;
        int width = x1 - x0 + 1, heigth = y1 - y0 + 1;

        drawSourceCheckboard(y0, heigth, x0, width, painter);

        drawSourceGradient(y0, heigth, x0, width, painter);

        // Draw geometric shapes on top of the patterns

        drawSourceRectangle(y0, x0, painter);

        drawSourceCircle(x0, x1, y0, y1, painter);

        drawSourceTriangle(y0, heigth, x0, width, x1, y1, painter);
    }

    void drawSourceTriangle(int y0, int heigth, int x0, int width, int x1, int y1, PixelPainter &painter)
    {
        // Draw a green triangle in the checkerboard area
        // Draw a green triangle inside the window x0, y0, x1, y1
        // The triangle will have its base at the bottom of the window and its apex at the top center
        // Move the triangle to the right so it doesn't overlap the circle
        int tri_base_y = y0 + heigth - 10; // Slightly above the bottom
        int tri_apex_y = y0 + 10;          // Slightly below the top
        // Shift the triangle center to the right by 60 pixels
        int tri_center_x = x0 + width / 2 + 60;
        int tri_half_base = width / 4; // Base width is half of window width

        for (int y = tri_apex_y; y <= tri_base_y; ++y)
        {
            float progress = float(y - tri_apex_y) / float(tri_base_y - tri_apex_y); // 0.0 at apex, 1.0 at base
            int half_width = static_cast<int>(progress * static_cast<float>(tri_half_base));
            int x_start = tri_center_x - half_width;
            int x_end = tri_center_x + half_width;
            for (int x = x_start; x <= x_end; ++x)
            {
                if (x >= x0 && x < x1 && y >= y0 && y < y1)
                {
                    painter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{0, 200, 0});
                }
            }
        }
    }

    void drawSourceCircle(int x0, int x1, int y0, int y1, PixelPainter &painter)
    {
        // Draw a blue circle inside window x0, y0, x1, y1
        int centerX = 200;
        int centerY = 150;
        int radius = 30; // Slightly smaller to avoid overlap issues
        for (int y = centerY - radius; y <= centerY + radius; ++y)
        {
            for (int x = centerX - radius; x <= centerX + radius; ++x)
            {
                if ((x - centerX) * (x - centerX) + (y - centerY) * (y - centerY) <= radius * radius)
                {
                    if (x >= x0 && x < x1 && y >= y0 && y < y1)
                    {                                                // Bounds check
                        painter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{0, 0, 255}); // Blue
                    }
                }
            }
        }
    }

    void drawSourceRectangle(int y0, int x0, PixelPainter &painter)
    {
        // Draw a red rectangle
        for (int y = y0 + 20; y < y0 + 80; ++y)
        {
            for (int x = x0 + 20; x < x0 + 80; ++x)
            {
                painter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{255, 0, 0}); // Red
            }
        }
    }

    void drawSourceGradient(int y0, int heigth, int x0, int width, PixelPainter &painter)
    {
        // Draw a colorful test pattern in the bottom half of the source region
        for (int y = y0; y < y0 + heigth / 2; ++y)
        {
            for (int x = x0; x < x0 + width; ++x)
            {
                // Ensure color components stay within valid range
                int r = std::min(255, std::max(0, 255 - (x - x0) / 2));
                int g = std::min(255, std::max(0, 100 + (y - y0 - heigth / 2) / 2));
                int b = std::min(255, std::max(0, 100 + (x - x0) / 2));
                painter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b)});
            }
        }
    }

    void drawSourceCheckboard(int y0, int heigth, int x0, int width, PixelPainter &painter)
    {
        // Clear the source region first with a light gray background
        for (int y = y0; y < y0 + heigth; ++y)
        {
            for (int x = x0; x < x0 + width; ++x)
            {
                painter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{240, 240, 240});
            }
        }

        // Draw a checkerboard pattern in the top half
        for (int y = y0; y < y0 + heigth; ++y)
        {
            for (int x = x0; x < x0 + width; ++x)
            {
                if (((x / 20) + (y / 20)) % 2 == 0)
                {
                    painter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{220, 220, 220});
                }
                else
                {
                    painter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{80, 80, 80});
                }
            }
        }
    }

    void drawTransformationFilters() {
        RgbImage &img = getImage();
        PixelPainter &basePainter = getPainter();
        
        // 1. Horizontal flip filter
        // Create clip filters for each section
        ClipFilter hFlipClip(basePainter, HFLIP_WINDOW);
        int windowWidth = HFLIP_WINDOW.x2 - HFLIP_WINDOW.x1 + 1;
        Point flipCenter = {HFLIP_WINDOW.x1 + windowWidth / 2, 0}; // x-center for flipping
        bool evenFlip = windowWidth % 2 == 0; // Check if the width is even
        HorizontalFlipFilter hFlipFilter(hFlipClip, flipCenter, evenFlip);
        Point leftTopTarget = {HFLIP_WINDOW.x1, HFLIP_WINDOW.y1};
        Point rightTopTarget = {HFLIP_WINDOW.x2, HFLIP_WINDOW.y1};
        [[maybe_unused]] Point leftTopSource = hFlipFilter.getPixelPos(static_cast<unsigned int>(leftTopTarget.x), static_cast<unsigned int>(leftTopTarget.y));
        [[maybe_unused]] Point rightTopSource = hFlipFilter.getPixelPos(static_cast<unsigned int>(rightTopTarget.x), static_cast<unsigned int>(rightTopTarget.y));

        
        for (int y = HFLIP_WINDOW.y1; y <= HFLIP_WINDOW.y2; ++y) {
            for (int x = HFLIP_WINDOW.x1; x <= HFLIP_WINDOW.x2; ++x) {
                int sourceX = x - HFLIP_WINDOW.x1 + ORIGINAL_WINDOW.x1; // Adjust source x-coordinate
                int sourceY = y - HFLIP_WINDOW.y1 + ORIGINAL_WINDOW.y1; // Adjust source y-coordinate
                RgbColor color = img.getPixel(Point(sourceX, sourceY)); // Copy from source area
                hFlipFilter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), color);
            }
        }
        
        // 2. Vertical flip filter
        ClipFilter vFlipClip(basePainter, VFLIP_WINDOW);
        int windowHeight = VFLIP_WINDOW.y2 - VFLIP_WINDOW.y1 + 1;
        bool evenVFlip = windowHeight % 2 == 0; // Check if the height is even
        Point vFlipCenter = {0, VFLIP_WINDOW.y1 + windowHeight / 2}; // y-center for flipping
        VerticalFlipFilter vFlipFilter(vFlipClip, vFlipCenter, evenVFlip);
        
        for (int y = VFLIP_WINDOW.y1; y <= VFLIP_WINDOW.y2; ++y) {
            for (int x = VFLIP_WINDOW.x1; x <= VFLIP_WINDOW.x2; ++x) {
                int sourceX = x - VFLIP_WINDOW.x1 + ORIGINAL_WINDOW.x1; // Adjust source x-coordinate
                int sourceY = y - VFLIP_WINDOW.y1 + ORIGINAL_WINDOW.y1; // Adjust source y-coordinate
                RgbColor color = img.getPixel(Point(sourceX, sourceY)); // Copy from source area
                vFlipFilter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), color);
            }
        }
        
        // 3. Rotation filter (90 degrees)
        ClipFilter rotClip(basePainter, ROTATION_WINDOW);
        Point rotCenter = {ROTATION_WINDOW.x1 + (ROTATION_WINDOW.x2 - ROTATION_WINDOW.x1 + 1) / 2, 
                           ROTATION_WINDOW.y1 + (ROTATION_WINDOW.y2 - ROTATION_WINDOW.y1 + 1) / 2};
        DiscreteRotationFilter rotFilter(rotClip, rotCenter, 90);
        
        for (int y = ROTATION_WINDOW.y1; y <= ROTATION_WINDOW.y2; ++y) {
            for (int x = ROTATION_WINDOW.x1; x <= ROTATION_WINDOW.x2; ++x) {
                int sourceX = x - ROTATION_WINDOW.x1 + ORIGINAL_WINDOW.x1; // Adjust source x-coordinate
                int sourceY = y - ROTATION_WINDOW.y1 + ORIGINAL_WINDOW.y1; // Adjust source y-coordinate
                RgbColor color = img.getPixel(Point(sourceX, sourceY)); // Copy from source area
                rotFilter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), color);
            }
        }
    }

    void drawTransparencyFilters() {
        RgbImage &img = getImage();
        PixelPainter &basePainter = getPainter();
        
        // Create clip filters for each section
        ClipFilter transClip(basePainter, TRANSPARENCY_WINDOW);
        ClipFilter alphaClip(basePainter, ALPHA_WINDOW);
        
        // Create a background pattern for transparency filter
        for (int y = TRANSPARENCY_WINDOW.y1; y <= TRANSPARENCY_WINDOW.y2; ++y) {
            for (int x = TRANSPARENCY_WINDOW.x1; x <= TRANSPARENCY_WINDOW.x2; ++x) {
                // Checkered pattern
                if (((x / 20) + (y / 20)) % 2 == 0) {
                    transClip.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{200, 200, 200});
                } else {
                    transClip.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{150, 150, 150});
                }
            }
        }
        
        // 1. Transparency filter
        TransparencyFilter transFilter(transClip, RgbColor{0, 0, 255}); // Blue is transparent
        
        for (int y = TRANSPARENCY_WINDOW.y1; y <= TRANSPARENCY_WINDOW.y2; ++y) {
            for (int x = TRANSPARENCY_WINDOW.x1; x <= TRANSPARENCY_WINDOW.x2; ++x) {
                int sourceX = x - TRANSPARENCY_WINDOW.x1 + ORIGINAL_WINDOW.x1; // Adjust source x-coordinate
                int sourceY = y - TRANSPARENCY_WINDOW.y1 + ORIGINAL_WINDOW.y1; // Adjust source y-coordinate
                RgbColor color = img.getPixel(Point(sourceX, sourceY)); // Copy from source area
                transFilter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), color);
            }
        }
        
        // 2. Alpha filter
        
        // Create the same background pattern for alpha filter
        for (int y = ALPHA_WINDOW.y1; y <= ALPHA_WINDOW.y2; ++y) {
            for (int x = ALPHA_WINDOW.x1; x <= ALPHA_WINDOW.x2; ++x) {
                // Checkered pattern
                if (((x / 20) + (y / 20)) % 2 == 0) {
                    alphaClip.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{200, 200, 200});
                } else {
                    alphaClip.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), RgbColor{150, 150, 150});
                }
            }
        }
        
        AlphaFilter alphaFilter(alphaClip, 0.5f); // 50% transparency
        
        for (int y = ALPHA_WINDOW.y1; y <= ALPHA_WINDOW.y2; ++y) {
            for (int x = ALPHA_WINDOW.x1; x <= ALPHA_WINDOW.x2; ++x) {
                int sourceX = x - ALPHA_WINDOW.x1 + ORIGINAL_WINDOW.x1; // Adjust source x-coordinate
                int sourceY = y - ALPHA_WINDOW.y1 + ORIGINAL_WINDOW.y1; // Adjust source y-coordinate
                RgbColor color = img.getPixel(Point(sourceX, sourceY)); // Copy from source area
                alphaFilter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), color);
            }
        }
    }

    void drawZoomFilters() {
        RgbImage &img = getImage();
        PixelPainter &basePainter = getPainter();
        
        // Create clip filters for each section
        ClipFilter zoomOutClip(basePainter, ZOOM_OUT_WINDOW);
        ClipFilter zoomInClip(basePainter, ZOOM_IN_WINDOW);
        
        // 1. Pixel Spread Filter (zoom out)
        Point spreadCenter = {ZOOM_OUT_WINDOW.x1 + (ZOOM_OUT_WINDOW.x2 - ZOOM_OUT_WINDOW.x1) / 2,
                             ZOOM_OUT_WINDOW.y1 + (ZOOM_OUT_WINDOW.y2 - ZOOM_OUT_WINDOW.y1) / 2};
        Point spreadZoom = {2, 2}; // Zoom out by factor of 2
        PixelSpreadFilter spreadFilter(zoomOutClip, spreadCenter, spreadZoom);
        
        for (int y = ZOOM_OUT_WINDOW.y1; y <= ZOOM_OUT_WINDOW.y2; ++y) {
            for (int x = ZOOM_OUT_WINDOW.x1; x <= ZOOM_OUT_WINDOW.x2; ++x) {
                int sourceX = x - ZOOM_OUT_WINDOW.x1 + ORIGINAL_WINDOW.x1; // Adjust source x-coordinate
                int sourceY = y - ZOOM_OUT_WINDOW.y1 + ORIGINAL_WINDOW.y1; // Adjust source y-coordinate
                RgbColor color = img.getPixel(Point(sourceX, sourceY)); // Copy from source area
                spreadFilter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), color);
            }
        }
        
        // 2. Pixel Zoom Filter (zoom in)
        Point zoomCenter = {ZOOM_IN_WINDOW.x1 + (ZOOM_IN_WINDOW.x2 - ZOOM_IN_WINDOW.x1) / 2,
                           ZOOM_IN_WINDOW.y1 + (ZOOM_IN_WINDOW.y2 - ZOOM_IN_WINDOW.y1) / 2};
        Point zoomFactor = {2, 2}; // Zoom in by factor of 2
        PixelZoomFilter zoomFilter(zoomInClip, zoomCenter, zoomFactor);
        
        for (int y = ZOOM_IN_WINDOW.y1; y < ZOOM_IN_WINDOW.y2; ++y) {
            for (int x = ZOOM_IN_WINDOW.x1; x < ZOOM_IN_WINDOW.x2; ++x) {
                int sourceX = x - ZOOM_IN_WINDOW.x1 + ORIGINAL_WINDOW.x1; // Adjust source x-coordinate
                int sourceY = y - ZOOM_IN_WINDOW.y1 + ORIGINAL_WINDOW.y1; // Adjust source y-coordinate
                RgbColor color = img.getPixel(Point(sourceX, sourceY)); // Copy from source area
                zoomFilter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(y), color);
            }
        }
    }

    void drawGradientFilters() {
        PixelPainter &basePainter = getPainter();
        
        // Create clip filters for each section
        ClipFilter linearGradClip(basePainter, LINEAR_GRADIENT_WINDOW);
        ClipFilter radialGradClip(basePainter, RADIAL_GRADIENT_WINDOW);
        
        // 1. Linear gradient filter
        Point gradStart = {LINEAR_GRADIENT_WINDOW.x1 + 50, LINEAR_GRADIENT_WINDOW.y1};
        Point gradEnd = {LINEAR_GRADIENT_WINDOW.x1 + 150, LINEAR_GRADIENT_WINDOW.y2};
        RgbColor startColor = {255, 0, 0}; // Red
        RgbColor endColor = {0, 0, 255}; // Blue
        GradientFilter2C gradFilter(linearGradClip, gradStart, gradEnd, startColor, endColor);
        
        for (int y = LINEAR_GRADIENT_WINDOW.y1; y <= LINEAR_GRADIENT_WINDOW.y2; ++y) {
            for (int x = LINEAR_GRADIENT_WINDOW.x1; x <= LINEAR_GRADIENT_WINDOW.x2; ++x) {
                gradFilter.putPixel(x, y, RgbColor{0, 0, 0}); // Color is determined by gradient
            }
        }
        
        // 2. Radial gradient
        Point center = {RADIAL_GRADIENT_WINDOW.x1 + (RADIAL_GRADIENT_WINDOW.x2 - RADIAL_GRADIENT_WINDOW.x1) / 2,
                       RADIAL_GRADIENT_WINDOW.y1 + (RADIAL_GRADIENT_WINDOW.y2 - RADIAL_GRADIENT_WINDOW.y1) / 2};
        int radius = 100;
        
        for (int y = RADIAL_GRADIENT_WINDOW.y1; y <= RADIAL_GRADIENT_WINDOW.y2; ++y) {
            for (int x = RADIAL_GRADIENT_WINDOW.x1; x <= RADIAL_GRADIENT_WINDOW.x2; ++x) {
                float distance = sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y));
                float ratio = std::min(1.0f, distance / radius);
                
                RgbColor color = {
                    static_cast<unsigned char>((1 - ratio) * 255), // Red decreases with distance
                    static_cast<unsigned char>((1 - ratio) * 100), // Some green
                    static_cast<unsigned char>(ratio * 255) // Blue increases with distance
                };
                
                radialGradClip.putPixel(x, y, color);
            }
        }
    }

    void drawSections() {
        drawSectionTitles();
        drawSectionBorders();
    }

    void drawSectionBorders()
    {
        // Draw borders around sections
        drawSectionBorder(ORIGINAL_WINDOW);
        drawSectionBorder(HFLIP_WINDOW);
        drawSectionBorder(VFLIP_WINDOW);
        drawSectionBorder(ROTATION_WINDOW);

        drawSectionBorder(TRANSPARENCY_WINDOW);
        drawSectionBorder(ALPHA_WINDOW);

        drawSectionBorder(ZOOM_OUT_WINDOW);
        drawSectionBorder(ZOOM_IN_WINDOW);

        drawSectionBorder(LINEAR_GRADIENT_WINDOW);
        drawSectionBorder(RADIAL_GRADIENT_WINDOW);
    }

    void drawSectionBorder(const RectInclusive &section)
    {
        // Draw border around the specified section
        int sectionWidth = section.x2 - section.x1 + 1;
        int sectionHeight = section.y2 - section.y1 + 1;
        drawBorder(section.x1 - 1, section.y1 - 1, sectionWidth + 2, sectionHeight + 2, RgbColor{0, 0, 0});
    }

    void drawSectionTitles()
    {
        RgbImage &img = getImage();
        PixelPainter &painter = getPainter();

        // Section titles
        drawText(painter, ORIGINAL_WINDOW.x1, ORIGINAL_WINDOW.y1 - 20, "Original", RgbColor{0, 0, 0});
        drawText(painter, HFLIP_WINDOW.x1, HFLIP_WINDOW.y1 - 20, "Horizontal Flip", RgbColor{0, 0, 0});
        drawText(painter, VFLIP_WINDOW.x1, VFLIP_WINDOW.y1 - 20, "Vertical Flip", RgbColor{0, 0, 0});
        drawText(painter, ROTATION_WINDOW.x1, ROTATION_WINDOW.y1 - 20, "90 Degree Rotation", RgbColor{0, 0, 0});

        drawText(painter, TRANSPARENCY_WINDOW.x1, TRANSPARENCY_WINDOW.y1 - 20, "Transparency Filter", RgbColor{0, 0, 0});
        drawText(painter, ALPHA_WINDOW.x1, ALPHA_WINDOW.y1 - 20, "Alpha Blend (0.5)", RgbColor{0, 0, 0});

        drawText(painter, ZOOM_OUT_WINDOW.x1, ZOOM_OUT_WINDOW.y1 - 20, "Zoom Out (x0.5)", RgbColor{0, 0, 0});
        drawText(painter, ZOOM_IN_WINDOW.x1, ZOOM_IN_WINDOW.y1 - 20, "Zoom In (x2)", RgbColor{0, 0, 0});

        drawText(painter, LINEAR_GRADIENT_WINDOW.x1, LINEAR_GRADIENT_WINDOW.y1 - 20, "Linear Gradient", RgbColor{0, 0, 0});
        drawText(painter, RADIAL_GRADIENT_WINDOW.x1, RADIAL_GRADIENT_WINDOW.y1 - 20, "Radial Gradient", RgbColor{0, 0, 0});
    }

    void drawMainTitle() {
        PixelPainter &painter = getPainter();
        
        // Draw main title at the top
        drawText(painter, 290, 25, "uImg Filter Demo", RgbColor{0, 0, 150}, 2);
        
        // Draw border around the entire image
        Point size = getImageSize();
        drawBorder(0, 0, size.x, size.y, RgbColor{100, 100, 100});
    }
    
    void drawBorder(int x, int y, int width, int height, const RgbColor &color) {
        PixelPainter &painter = getPainter();
        
        // Draw horizontal lines
        for (int i = x; i < x + width; ++i) {
            painter.putPixel(i, y, color);
            painter.putPixel(i, y + height - 1, color);
        }
        
        // Draw vertical lines
        for (int i = y; i < y + height; ++i) {
            painter.putPixel(x, i, color);
            painter.putPixel(x + width - 1, i, color);
        }
    }
    
    void drawText(PixelPainter &painter, int x, int y, const std::string &text, const RgbColor &color, int size = 1) {
        // Simple bitmap font rendering (very basic)
        static const int charWidth = 6;
        static const int charHeight = 8;
        
        for (size_t i = 0; i < text.length(); ++i) {
            drawChar(painter, x + i * charWidth * size, y, text[i], color, size);
        }
    }
    
    void drawChar(PixelPainter &painter, int x, int y, char c, const RgbColor &color, int size = 1) {
        // Very simple bitmap font - just for demonstration purposes
        // Define patterns for some basic characters
        static const unsigned char patterns[][8] = {
            // A
            {0b01100000,
             0b10010000,
             0b10010000,
             0b11110000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b00000000},
            // B
            {0b11100000,
             0b10010000,
             0b10010000,
             0b11100000,
             0b10010000,
             0b10010000,
             0b11100000,
             0b00000000},
            // C
            {0b01110000,
             0b10000000,
             0b10000000,
             0b10000000,
             0b10000000,
             0b10000000,
             0b01110000,
             0b00000000},
            // D
            {0b11100000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b11100000,
             0b00000000},
            // E
            {0b11110000,
             0b10000000,
             0b10000000,
             0b11100000,
             0b10000000,
             0b10000000,
             0b11110000,
             0b00000000},
            // F
            {0b11110000,
             0b10000000,
             0b10000000,
             0b11100000,
             0b10000000,
             0b10000000,
             0b10000000,
             0b00000000},
            // G
            {0b01110000,
             0b10000000,
             0b10000000,
             0b10110000,
             0b10010000,
             0b10010000,
             0b01110000,
             0b00000000},
            // H
            {0b10010000,
             0b10010000,
             0b10010000,
             0b11110000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b00000000},
            // I
            {0b11100000,
             0b01000000,
             0b01000000,
             0b01000000,
             0b01000000,
             0b01000000,
             0b11100000,
             0b00000000},
            // J
            {0b00110000,
             0b00010000,
             0b00010000,
             0b00010000,
             0b00010000,
             0b10010000,
             0b01100000,
             0b00000000},
            // K
            {0b10010000,
             0b10100000,
             0b11000000,
             0b10000000,
             0b11000000,
             0b10100000,
             0b10010000,
             0b00000000},
            // L
            {0b10000000,
             0b10000000,
             0b10000000,
             0b10000000,
             0b10000000,
             0b10000000,
             0b11110000,
             0b00000000},
            // M
            {0b10001000,
             0b11011000,
             0b10101000,
             0b10101000,
             0b10001000,
             0b10001000,
             0b10001000,
             0b00000000},
            // N
            {0b10010000,
             0b11010000,
             0b11010000,
             0b10110000,
             0b10110000,
             0b10010000,
             0b10010000,
             0b00000000},
            // O
            {0b01100000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b01100000,
             0b00000000},
            // P
            {0b11100000,
             0b10010000,
             0b10010000,
             0b11100000,
             0b10000000,
             0b10000000,
             0b10000000,
             0b00000000},
            // Q
            {0b01100000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b10110000,
             0b10010000,
             0b01110000,
             0b00000000},
            // R
            {0b11100000,
             0b10010000,
             0b10010000,
             0b11100000,
             0b11000000,
             0b10100000,
             0b10010000,
             0b00000000},
            // S
            {0b01110000,
             0b10000000,
             0b10000000,
             0b01100000,
             0b00010000,
             0b00010000,
             0b11100000,
             0b00000000},
            // T
            {0b11111000,
             0b00100000,
             0b00100000,
             0b00100000,
             0b00100000,
             0b00100000,
             0b00100000,
             0b00000000},
            // U
            {0b10010000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b01100000,
             0b00000000},
            // V
            {0b10001000,
             0b10001000,
             0b10001000,
             0b10001000,
             0b10001000,
             0b01010000,
             0b00100000,
             0b00000000},
            // W
            {0b10001000,
             0b10001000,
             0b10001000,
             0b10101000,
             0b10101000,
             0b11011000,
             0b10001000,
             0b00000000},
            // X
            {0b10010000,
             0b10010000,
             0b01100000,
             0b01100000,
             0b01100000,
             0b10010000,
             0b10010000,
             0b00000000},
            // Y
            {0b10001000,
             0b10001000,
             0b01010000,
             0b00100000,
             0b00100000,
             0b00100000,
             0b00100000,
             0b00000000},
            // Z
            {0b11110000,
             0b00010000,
             0b00100000,
             0b01000000,
             0b10000000,
             0b10000000,
             0b11110000,
             0b00000000},
            // Space
            {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000},
            // 0
            {0b01100000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b10010000,
             0b01100000,
             0b00000000},
            // 1
            {0b00100000,
             0b01100000,
             0b00100000,
             0b00100000,
             0b00100000,
             0b00100000,
             0b01110000,
             0b00000000},
            // 2
            {0b01100000,
             0b10010000,
             0b00010000,
             0b00100000,
             0b01000000,
             0b10000000,
             0b11110000,
             0b00000000},
            // 3
            {0b11110000,
             0b00010000,
             0b00100000,
             0b00110000,
             0b00010000,
             0b10010000,
             0b01100000,
             0b00000000},
            // 4
            {0b00010000,
             0b00110000,
             0b01010000,
             0b10010000,
             0b11110000,
             0b00010000,
             0b00010000,
             0b00000000},
            // 5
            {0b11110000,
             0b10000000,
             0b11100000,
             0b00010000,
             0b00010000,
             0b10010000,
             0b01100000,
             0b00000000},
            // 6
            {0b00100000,
             0b01000000,
             0b10000000,
             0b11100000,
             0b10010000,
             0b10010000,
             0b01100000,
             0b00000000},
            // 7
            {0b11110000,
             0b00010000,
             0b00100000,
             0b01000000,
             0b10000000,
             0b10000000,
             0b10000000,
             0b00000000},
            // 8
            {0b01100000,
             0b10010000,
             0b10010000,
             0b01100000,
             0b10010000,
             0b10010000,
             0b01100000,
             0b00000000},
            // 9
            {0b01100000,
             0b10010000,
             0b10010000,
             0b01110000,
             0b00010000,
             0b00100000,
             0b01000000,
             0b00000000},
            // . (period)
            {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b01000000,
             0b00000000},
            // , (comma)
            {0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b01000000,
             0b01000000,
             0b10000000},
            // : (colon)
            {0b00000000,
             0b01000000,
             0b01000000,
             0b00000000,
             0b00000000,
             0b01000000,
             0b01000000,
             0b00000000},
            // ; (semicolon)
            {0b00000000,
             0b01000000,
             0b01000000,
             0b00000000,
             0b00000000,
             0b01000000,
             0b01000000,
             0b10000000},
            // ( (left parenthesis)
            {0b00100000,
             0b01000000,
             0b10000000,
             0b10000000,
             0b10000000,
             0b01000000,
             0b00100000,
             0b00000000},
            // ) (right parenthesis)
            {0b10000000,
             0b01000000,
             0b00100000,
             0b00100000,
             0b00100000,
             0b01000000,
             0b10000000,
             0b00000000},
            // - (hyphen)
            {0b00000000,
             0b00000000,
             0b00000000,
             0b11110000,
             0b00000000,
             0b00000000,
             0b00000000,
             0b00000000}
        };
        
        // Map characters to pattern indices
        int index = -1;
        if (c >= 'A' && c <= 'Z') {
            index = c - 'A';
        } else if (c >= 'a' && c <= 'z') {
            index = c - 'a'; // Use same pattern as uppercase
        } else if (c == ' ') {
            index = 26; // Space
        } else if (c >= '0' && c <= '9') {
            index = 27 + (c - '0'); // Numbers
        } else if (c == '.') {
            index = 37;
        } else if (c == ',') {
            index = 38;
        } else if (c == ':') {
            index = 39;
        } else if (c == ';') {
            index = 40;
        } else if (c == '(') {
            index = 41;
        } else if (c == ')') {
            index = 42;
        } else if (c == '-') {
            index = 43;
        }
        
        if (index >= 0) {
            // Draw the character
            for (int dy = 0; dy < 8; ++dy) {
                for (int dx = 0; dx < 5; ++dx) {
                    if ((patterns[index][dy] >> (7 - dx)) & 0x01) {
                        // Draw pixel at scaled size
                        for (int sy = 0; sy < size; ++sy) {
                            for (int sx = 0; sx < size; ++sx) {
                                painter.putPixel(x + dx * size + sx, y + dy * size + sy, color);
                            }
                        }
                    }
                }
            }
        }
    }
};

struct filter_demo_args {
    std::string outFileName = "filter_demo.ppm";
    
    static filter_demo_args parse(int argc, const char* const* argv) {
        filter_demo_args args;
        
        for (int i = 1; i < argc; ++i) {
            if ((strcmp(argv[i], "-out") == 0) && (i + 1 < argc)) {
                args.outFileName = std::string(argv[i + 1]);
            }
        }
        
        return args;
    }
};

const int LEFT_OFFSET = 120;
const int TOP_OFFSET = 70;
const int WINDOW_WIDTH = 200;
const int WINDOW_HEIGHT = 100;
const int HORIZONTAL_SPACING = 50;
const int VERTICAL_SPACING = 50;

// Define the constants outside of the class
// All windows use the same width/height (200x100) with equal spacing (50px horizontally, 50px vertically)
const RectInclusive TestFilterDemo::ORIGINAL_WINDOW = RectInclusive::make_rect(
    LEFT_OFFSET + HORIZONTAL_SPACING, 
    TOP_OFFSET + 10, 
    LEFT_OFFSET + HORIZONTAL_SPACING + WINDOW_WIDTH, 
    TOP_OFFSET + 10 + WINDOW_HEIGHT);
const RectInclusive TestFilterDemo::HFLIP_WINDOW = RectInclusive::make_rect(
    LEFT_OFFSET + HORIZONTAL_SPACING*2 + WINDOW_WIDTH, 
    TOP_OFFSET + 10, 
    LEFT_OFFSET + HORIZONTAL_SPACING*2 + WINDOW_WIDTH*2, 
    TOP_OFFSET + 10 + WINDOW_HEIGHT);
const RectInclusive TestFilterDemo::VFLIP_WINDOW = RectInclusive::make_rect(
    LEFT_OFFSET + HORIZONTAL_SPACING, 
    TOP_OFFSET + 10 + WINDOW_HEIGHT + VERTICAL_SPACING, 
    LEFT_OFFSET + HORIZONTAL_SPACING + WINDOW_WIDTH, 
    TOP_OFFSET + 10 + WINDOW_HEIGHT*2 + VERTICAL_SPACING);
const RectInclusive TestFilterDemo::ROTATION_WINDOW = RectInclusive::make_rect(
    LEFT_OFFSET + HORIZONTAL_SPACING*2 + WINDOW_WIDTH, 
    TOP_OFFSET + 10 + WINDOW_HEIGHT + VERTICAL_SPACING, 
    LEFT_OFFSET + HORIZONTAL_SPACING*2 + WINDOW_WIDTH*2, 
    TOP_OFFSET + 10 + WINDOW_HEIGHT*2 + VERTICAL_SPACING);
const RectInclusive TestFilterDemo::TRANSPARENCY_WINDOW = RectInclusive::make_rect(
    LEFT_OFFSET + HORIZONTAL_SPACING, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*2, 
    LEFT_OFFSET + HORIZONTAL_SPACING + WINDOW_WIDTH, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*2 + WINDOW_HEIGHT);
const RectInclusive TestFilterDemo::ALPHA_WINDOW = RectInclusive::make_rect(
    LEFT_OFFSET + HORIZONTAL_SPACING*2 + WINDOW_WIDTH, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*2, 
    LEFT_OFFSET + HORIZONTAL_SPACING*2 + WINDOW_WIDTH*2, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*2 + WINDOW_HEIGHT);
const RectInclusive TestFilterDemo::ZOOM_OUT_WINDOW = RectInclusive::make_rect(
    LEFT_OFFSET + HORIZONTAL_SPACING, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*3, 
    LEFT_OFFSET + HORIZONTAL_SPACING + WINDOW_WIDTH, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*3 + WINDOW_HEIGHT);
const RectInclusive TestFilterDemo::ZOOM_IN_WINDOW = RectInclusive::make_rect(
    LEFT_OFFSET + HORIZONTAL_SPACING*2 + WINDOW_WIDTH, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*3, 
    LEFT_OFFSET + HORIZONTAL_SPACING*2 + WINDOW_WIDTH*2, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*3 + WINDOW_HEIGHT);
const RectInclusive TestFilterDemo::LINEAR_GRADIENT_WINDOW = RectInclusive::make_rect(
    LEFT_OFFSET + HORIZONTAL_SPACING, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*4, 
    LEFT_OFFSET + HORIZONTAL_SPACING + WINDOW_WIDTH, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*4 + WINDOW_HEIGHT);
const RectInclusive TestFilterDemo::RADIAL_GRADIENT_WINDOW = RectInclusive::make_rect(
    LEFT_OFFSET + HORIZONTAL_SPACING*2 + WINDOW_WIDTH, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*4, 
    LEFT_OFFSET + HORIZONTAL_SPACING*2 + WINDOW_WIDTH*2, 
    TOP_OFFSET + 10 + (WINDOW_HEIGHT + VERTICAL_SPACING)*4 + WINDOW_HEIGHT);

} // namespace uimg

#endif // __UIMG_FILTER_DEMO_H__
