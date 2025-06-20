#ifndef __UIMG_CHART3D_LAYOUT_H__
#define __UIMG_CHART3D_LAYOUT_H__

#include "uimg/base/structs.h"
#include <vector>

/**
 * @brief Rectangle structure for chart positioning
 */
struct ChartRect {
    int x1, y1, x2, y2;
    
    static ChartRect make_rect(int x1, int y1, int x2, int y2) {
        return {x1, y1, x2, y2};
    }
    
    int width() const { return x2 - x1; }
    int height() const { return y2 - y1; }
    Point getSize() const { return {width(), height()}; }
    Point getTopLeft() const { return {x1, y1}; }
};

/**
 * @brief Layout manager for positioning multiple 3D charts
 */
class Chart3DLayoutManager {
public:
    Chart3DLayoutManager(int canvasWidth, int canvasHeight) 
        : canvasWidth_(canvasWidth), canvasHeight_(canvasHeight) {}
    
    /**
     * @brief Calculate layout rectangles for the given number of charts
     * @param numCharts Number of charts to layout (1-4)
     * @return Vector of chart rectangles
     */
    std::vector<ChartRect> calculateLayout(int numCharts) {
        std::vector<ChartRect> rects;
        
        switch(numCharts) {
            case 1:
                // Single chart takes full canvas with margins
                rects.push_back(ChartRect::make_rect(20, 20, canvasWidth_ - 20, canvasHeight_ - 20));
                break;
                
            case 2:
                // Two charts side by side
                rects.push_back(ChartRect::make_rect(10, 10, canvasWidth_ / 2 - 15, canvasHeight_ - 20));
                rects.push_back(ChartRect::make_rect(canvasWidth_ / 2 + 5, 10, canvasWidth_ - 15, canvasHeight_ - 20));
                break;
                
            case 3:
                // Three charts: one on top, two on bottom
                rects.push_back(ChartRect::make_rect(10, 10, canvasWidth_ - 10, canvasHeight_ / 2 - 10));
                rects.push_back(ChartRect::make_rect(10, canvasHeight_ / 2 + 10, canvasWidth_ / 2 - 15, canvasHeight_ - 10));
                rects.push_back(ChartRect::make_rect(canvasWidth_ / 2 + 5, canvasHeight_ / 2 + 10, canvasWidth_ - 10, canvasHeight_ - 10));
                break;
                
            case 4: {
                // Four charts in a 2x2 grid
                int halfWidth = canvasWidth_ / 2;
                int halfHeight = canvasHeight_ / 2;
                rects.push_back(ChartRect::make_rect(10, 10, halfWidth - 10, halfHeight - 10));
                rects.push_back(ChartRect::make_rect(halfWidth + 10, 10, canvasWidth_ - 10, halfHeight - 10));
                rects.push_back(ChartRect::make_rect(10, halfHeight + 10, halfWidth - 10, canvasHeight_ - 10));
                rects.push_back(ChartRect::make_rect(halfWidth + 10, halfHeight + 10, canvasWidth_ - 10, canvasHeight_ - 10));
                break;
            }
                
            default:
                // Default to single chart
                rects.push_back(ChartRect::make_rect(20, 20, canvasWidth_ - 20, canvasHeight_ - 20));
                break;
        }
        
        return rects;
    }

private:
    int canvasWidth_;
    int canvasHeight_;
};

#endif // __UIMG_CHART3D_LAYOUT_H__
