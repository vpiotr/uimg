#ifndef __UIMG_PAINTERS_RECT_PAINTER_H__
#define __UIMG_PAINTERS_RECT_PAINTER_H__

#include "uimg/base/structs.h"
#include "uimg/painters/painter_for_pixels.h"

namespace uimg {

/**
 * @brief Class for drawing rectangles using a pixel painter
 */
class RectPainterForPixels {
public:
    RectPainterForPixels(PixelPainter& pixelPainter)
        : pixelPainter_(pixelPainter) {}

    /**
     * Draw an outline rectangle
     * 
     * @param x1 Top-left x coordinate
     * @param y1 Top-left y coordinate
     * @param x2 Bottom-right x coordinate
     * @param y2 Bottom-right y coordinate
     * @param color Color to use for the rectangle outline
     */
    void drawRect(int x1, int y1, int x2, int y2, const RgbColor& color) {
        // Draw horizontal lines
        for (int x = x1; x <= x2; ++x) {
            pixelPainter_.putPixel(x, y1, color);
            pixelPainter_.putPixel(x, y2, color);
        }
        
        // Draw vertical lines
        for (int y = y1 + 1; y < y2; ++y) {
            pixelPainter_.putPixel(x1, y, color);
            pixelPainter_.putPixel(x2, y, color);
        }
    }
    
    /**
     * Fill a rectangle with a solid color
     * 
     * @param x1 Top-left x coordinate
     * @param y1 Top-left y coordinate
     * @param x2 Bottom-right x coordinate
     * @param y2 Bottom-right y coordinate
     * @param color Color to fill the rectangle with
     */
    void fillRect(int x1, int y1, int x2, int y2, const RgbColor& color) {
        for (int y = y1; y <= y2; ++y) {
            for (int x = x1; x <= x2; ++x) {
                pixelPainter_.putPixel(x, y, color);
            }
        }
    }

private:
    PixelPainter& pixelPainter_;
};

} // namespace uimg

#endif // __UIMG_PAINTERS_RECT_PAINTER_H__
