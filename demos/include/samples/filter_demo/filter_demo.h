#ifndef __UIMG_FILTER_DEMO_H__
#define __UIMG_FILTER_DEMO_H__

#include <string>
#include <memory>
#include <vector>
#include <iostream>

#include "uimg/base/structs.h"
#include "uimg/filters/filter_base.h"
#include "uimg/filters/filter_for_pixels.h"
#include "uimg/utils/cast.h"
#include "samples/demo_painter_base.h"

namespace uimg {

/**
 * @brief Demo application to showcase all available pixel filters
 */
class FilterDemo : public demo_painter_base {
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
    
    FilterDemo(const std::string &outFileName)
        : demo_painter_base(outFileName) {}

    virtual ~FilterDemo() {}

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
        
        // Draw section titles
        drawSectionTitles();
        
        // Apply various filters in different sections
        applyHorizontalFlip();
        applyVerticalFlip();
        applyRotation();
        applyTransparency();
        applyAlphaBlending();
        applyZoomOut();
        applyZoomIn();
        applyLinearGradient();
        applyRadialGradient();
        
        // Draw borders to visually separate filter sections
        drawAllBorders();
    }
    
private:
    void createSourceImage() {
        RgbImage &img = getImage();
        BackgroundPainterForRgbImage bgPainter(img);
        bgPainter.paint(RgbColor{240, 240, 240}); // Light gray background
        
        // Create a colorful source image in the original window
        drawSourceImage(ORIGINAL_WINDOW);
    }
    
    void drawSourceImage(const RectInclusive &rect) {
        RgbImage &img = getImage();
        PixelPainterForImageBase pixelPainter(img);
        
        // Draw a gradient background
        for (int y = rect.top; y <= rect.bottom; y++) {
            for (int x = rect.left; x <= rect.right; x++) {
                int r = (x - rect.left) * 255 / (rect.right - rect.left);
                int g = (y - rect.top) * 255 / (rect.bottom - rect.top);
                int b = 128;
                pixelPainter.paint(RgbColor{static_cast<unsigned char>(r), static_cast<unsigned char>(g), static_cast<unsigned char>(b)}, x, y);
            }
        }
        
        // Draw geometric shapes
        CirclePainterForPixels circlePainter(pixelPainter);
        int centerX = (rect.left + rect.right) / 2;
        int centerY = (rect.top + rect.bottom) / 2;
        int radius = std::min(rect.right - rect.left, rect.bottom - rect.top) / 4;
        
        circlePainter.drawFull(centerX, centerY, radius, RgbColor{255, 255, 255});
        
        RectPainterForRgbImage rectPainter(img);
        rectPainter.drawEmpty(centerX - radius/2, centerY - radius/2, centerX + radius/2, centerY + radius/2, RgbColor{0, 0, 0});
    }
    
    void drawSectionTitles() {
        // Implementation for drawing titles on each filter section
    }
    
    void applyHorizontalFlip() {
        // Implementation for horizontal flip filter
    }
    
    void applyVerticalFlip() {
        // Implementation for vertical flip filter
    }
    
    void applyRotation() {
        // Implementation for rotation filter
    }
    
    void applyTransparency() {
        // Implementation for transparency filter
    }
    
    void applyAlphaBlending() {
        // Implementation for alpha blending filter
    }
    
    void applyZoomOut() {
        // Implementation for zoom out filter
    }
    
    void applyZoomIn() {
        // Implementation for zoom in filter
    }
    
    void applyLinearGradient() {
        // Implementation for linear gradient filter
    }
    
    void applyRadialGradient() {
        // Implementation for radial gradient filter
    }
    
    void drawAllBorders() {
        // Implementation for drawing borders around all filter sections
    }
};

// Definition of the section window constants
const RectInclusive FilterDemo::ORIGINAL_WINDOW = {50, 50, 250, 250};
const RectInclusive FilterDemo::HFLIP_WINDOW = {300, 50, 500, 250};
const RectInclusive FilterDemo::VFLIP_WINDOW = {550, 50, 750, 250};
const RectInclusive FilterDemo::ROTATION_WINDOW = {50, 300, 250, 500};
const RectInclusive FilterDemo::TRANSPARENCY_WINDOW = {300, 300, 500, 500};
const RectInclusive FilterDemo::ALPHA_WINDOW = {550, 300, 750, 500};
const RectInclusive FilterDemo::ZOOM_OUT_WINDOW = {50, 550, 250, 750};
const RectInclusive FilterDemo::ZOOM_IN_WINDOW = {300, 550, 500, 750};
const RectInclusive FilterDemo::LINEAR_GRADIENT_WINDOW = {550, 550, 750, 750};
const RectInclusive FilterDemo::RADIAL_GRADIENT_WINDOW = {300, 50, 500, 250}; // Reusing HFLIP window for simplicity

} // namespace uimg

#endif
