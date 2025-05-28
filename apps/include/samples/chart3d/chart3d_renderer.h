#ifndef __UIMG_CHART3D_RENDERER_H__
#define __UIMG_CHART3D_RENDERER_H__

#include "multi_chart_3d.h"
#include "chart3d_layout.h"
#include "uimg/fonts/bdf_font.h"
#include "uimg/fonts/painter_for_bdf_font.h"
#include <memory>
#include <vector>

/**
 * @brief Multi-chart renderer for 3D charts
 */
class Chart3DRenderer {
public:
    Chart3DRenderer(RgbImage& image, PixelPainter& painter, bool useAntiAliasing = false) 
        : image_(image), painter_(painter), useAntiAliasing_(useAntiAliasing),
          layoutManager_(image.getSize().x, image.getSize().y) {}
    
    /**
     * @brief Render multiple 3D charts
     * @param numCharts Number of charts to render (1-4)
     */
    void renderCharts(int numCharts) {
        // Limit to supported range
        numCharts = std::max(1, std::min(4, numCharts));
        
        // Calculate layout
        auto rects = layoutManager_.calculateLayout(numCharts);
        
        // Create and render charts based on the number requested
        for (int i = 0; i < numCharts; ++i) {
            Point chartSize = rects[i].getSize();
            Point chartOffset = rects[i].getTopLeft();
            
            // Create chart with adjusted size and offset
            renderSingleChart(i % 4, chartSize, chartOffset);
        }
    }

private:
    void renderSingleChart(int chartType, Point chartSize, Point offset) {
        // Create an offset painter
        OffsetPixelPainter offsetPainter(painter_, offset);
        
        // Create the appropriate chart type
        std::unique_ptr<chart_z_fxy_3d_with_title> chart;
        switch(chartType) {
            case 0:
                chart = std::make_unique<chart_z_fxy_3d_sinc>(chartSize, offsetPainter, useAntiAliasing_);
                break;
            case 1:
                chart = std::make_unique<chart_z_fxy_3d_gaussian>(chartSize, offsetPainter, useAntiAliasing_);
                break;
            case 2:
                chart = std::make_unique<chart_z_fxy_3d_ripple>(chartSize, offsetPainter, useAntiAliasing_);
                break;
            case 3:
                chart = std::make_unique<chart_z_fxy_3d_saddle>(chartSize, offsetPainter, useAntiAliasing_);
                break;
        }
        
        // Paint the chart
        if (chart) {
            chart->paint();
        }
    }
    
    /**
     * @brief Pixel painter that applies an offset to all drawing operations
     */
    class OffsetPixelPainter : public PixelPainter {
    public:
        OffsetPixelPainter(PixelPainter& basePainter, Point offset) 
            : basePainter_(basePainter), offset_(offset) {}
        
        virtual void putPixel(unsigned int x, unsigned int y, const RgbColor& color) override {
            basePainter_.putPixel(x + offset_.x, y + offset_.y, color);
        }
        
        virtual void getPixel(unsigned int x, unsigned int y, RgbColor& output) override {
            basePainter_.getPixel(x + offset_.x, y + offset_.y, output);
        }
        
    private:
        PixelPainter& basePainter_;
        Point offset_;
    };
    
    RgbImage& image_;
    PixelPainter& painter_;
    bool useAntiAliasing_;
    Chart3DLayoutManager layoutManager_;
};

#endif // __UIMG_CHART3D_RENDERER_H__
