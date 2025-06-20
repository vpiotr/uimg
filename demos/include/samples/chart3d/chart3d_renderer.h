#ifndef __UIMG_CHART3D_RENDERER_H__
#define __UIMG_CHART3D_RENDERER_H__

#include "multi_chart_3d.h"
#include "chart3d_layout.h"
#include "uimg/fonts/bdf_font.h"
#include "uimg/fonts/painter_for_bdf_font.h"
#include "chart_z_fxy_3d.h"
#include "dlog/dlog.h"
#include "uimg/filters/pixel_tracing_filter.h"
#include <memory>
#include <vector>

/**
 * @brief Multi-chart renderer for 3D charts
 */
class Chart3DRenderer {
public:
    Chart3DRenderer(RgbImage& image, PixelPainter& painter, bool useAntiAliasing = false, bool drawBorders = false) 
        : image_(image), painter_(painter), useAntiAliasing_(useAntiAliasing), drawBorders_(drawBorders),
          layoutManager_(image.getSize().x, image.getSize().y) {}
    
    /**
     * @brief Render multiple 3D charts
     * @param numCharts Number of charts to render (1-4)
     */
    void renderCharts(int numCharts) {
        // Limit to supported range
        numCharts = std::max(1, std::min(4, numCharts));
        
        auto logger = dlog::Logger::getInstance();
        logger->debug("=== Multi-Chart 3D Renderer Debug Information ===");
        logger->debug("Total image size: {0}x{1}", image_.getSize().x, image_.getSize().y);
        logger->debug("Number of charts to render: {0}", numCharts);
        
        // Calculate layout
        auto rects = layoutManager_.calculateLayout(numCharts);
        
        // Create and render charts based on the number requested
        for (int i = 0; i < numCharts; ++i) {
            Point chartSize = rects[i].getSize();
            Point chartOffset = rects[i].getTopLeft();

            logger->debug("Chart {0} layout: size={1}x{2}, offset=({3},{4})", 
                         i, chartSize.x, chartSize.y, chartOffset.x, chartOffset.y);
            
            // Create chart with adjusted size and offset
            renderSingleChart(i % 4, chartSize, chartOffset);
        }
    }

private:
    void renderSingleChart(int chartType, Point chartSize, Point offset) {
        auto logger = dlog::Logger::getInstance();
        logger->debug("--- Rendering single chart {0} ---", chartType);
        logger->debug("Chart type: {0}, Canvas size: {1}x{2}, Offset: ({3},{4})", 
                     chartType, chartSize.x, chartSize.y, offset.x, offset.y);
        
        // Create an offset painter
        OffsetPixelPainter offsetPainter(painter_, offset);
        
        // Create chart type name for tracing
        std::string chartTypeName;
        switch(chartType) {
            case 0: chartTypeName = "Sinc Chart"; break;
            case 1: chartTypeName = "Gaussian Chart"; break;
            case 2: chartTypeName = "Ripple Chart"; break;
            case 3: chartTypeName = "Saddle Chart"; break;
            default: chartTypeName = "Unknown Chart"; break;
        }
        
        // Create pixel tracing filter for this chart
        PixelTracingFilter tracingFilter(offsetPainter, chartTypeName);
        
        // Create the appropriate chart type using the tracing filter
        std::unique_ptr<chart_z_fxy_3d_with_title> chart;
        switch(chartType) {
            case 0:
                chart = std::make_unique<chart_z_fxy_3d_sinc>(chartSize, tracingFilter, useAntiAliasing_, drawBorders_);
                logger->debug("Created Sinc function chart");
                break;
            case 1:
                chart = std::make_unique<chart_z_fxy_3d_gaussian>(chartSize, tracingFilter, useAntiAliasing_, drawBorders_);
                logger->debug("Created Gaussian function chart");
                break;
            case 2:
                chart = std::make_unique<chart_z_fxy_3d_ripple>(chartSize, tracingFilter, useAntiAliasing_, drawBorders_);
                logger->debug("Created Ripple function chart");
                break;
            case 3:
                chart = std::make_unique<chart_z_fxy_3d_saddle>(chartSize, tracingFilter, useAntiAliasing_, drawBorders_);
                logger->debug("Created Saddle function chart");
                break;
        }
        
        // Paint the chart
        if (chart) {
            chart->paint();
            logger->debug("Chart {0} painted successfully", chartType);
            
            // Log pixel range after chart drawing is complete
            tracingFilter.logPixelRange();
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
    bool drawBorders_;
    Chart3DLayoutManager layoutManager_;
};

#endif // __UIMG_CHART3D_RENDERER_H__
