#ifndef __UIMG_DEMOS_MULTI_CHART_3D_H__
#define __UIMG_DEMOS_MULTI_CHART_3D_H__

#include <vector>
#include <functional>
#include <string>

#include "samples/demo_painter_base.h"
#include "chart3d/chart_z_fxy_3d.h"
#include "chart3d/chart3d_renderer.h"
#include "chart3d/chart3d_z_fxy.h"
#include "uimg/filters/anti_aliasing_filter.h"

/**
 * @brief Multi-chart 3D demo with proper layout calculation and validation
 * 
 * Layout hierarchy (in order of calculation):
 * 1. Image size - Total size of the output image being built
 * 2. Window size & position - Part of image where each chart section fits (chart + labels + margins)
 * 3. Border - Drawn around window perimeter (final edge of window)
 * 4. Margin - Space between available space and border (configurable per side)
 * 5. Available space - Space where chart lines are drawn (centered within window)
 * 6. Available space blue border - Debug feature to verify chart stays within bounds
 */
class multi_chart_3d_demo : public demo_painter_base {
public:
    multi_chart_3d_demo(const std::string &fname, int numCharts, bool useAntiAliasing, bool drawBorders, const std::string &layout = "auto") 
        : demo_painter_base(fname), numCharts_(numCharts), useAntiAliasing_(useAntiAliasing), drawBorders_(drawBorders), layout_(layout),
          margin_(10), drawDebugBorders_(false) {} // Default margin of 10 pixels on all sides, debug borders off by default

    // Method to enable debug borders for line windows
    void setDrawDebugBorders(bool enable) { drawDebugBorders_ = enable; }

protected:
    /**
     * @brief Structure to hold layout dimensions and positions
     */
    struct ChartLayout {
        // 1. Image size - Total output image dimensions
        int imageWidth;
        int imageHeight;
        
        // 2. Chart window - area where all chart parts (lines, labels, axes) must fit
        int chartWindowWidth;
        int chartWindowHeight;
        int chartWindowX;
        int chartWindowY;
        
        // 3. Line window - smaller area where only chart lines are drawn
        int lineWindowWidth;
        int lineWindowHeight;
        int lineWindowX;
        int lineWindowY;
        
        // 4. Border and margin settings
        int borderThickness;
        int marginTop;
        int marginBottom; 
        int marginLeft;
        int marginRight;
        
        ChartLayout() : borderThickness(1), marginTop(10), marginBottom(10), marginLeft(10), marginRight(10) {}
    };

    virtual void paint() {
        // 1. Calculate image size
        ChartLayout imageLayout;
        calculateImageSize(imageLayout);
        
        // Define chart layout based on the number of charts
        int rows, cols;
        determineLayout(rows, cols);
        
        // Create chart functions
        std::vector<std::function<float(float, float)>> functions = createFunctions();
        
        // Initialize painters
        BackgroundPainterForRgbImage backPainter(getImage());
        backPainter.paint(RGB_WHITE);
        
        // Initialize pixel painter
        if (!pixelPainter_) {
            pixelPainter_ = std::make_unique<PixelPainterForImageBase>(getImage());
        }
        
        PixelPainter *pixelPainter = nullptr;
        if (useAntiAliasing_) {
            aaFilter_ = std::make_unique<AntiAliasingFilter>(getImage());
            pixelPainter = &aaFilter_->getFilteredPainter();
        } else {
            pixelPainter = pixelPainter_.get();
        }
        
        // Render each chart
        Chart3DRenderer renderer(*pixelPainter);
        
        bool boundaryViolation = false;
        
        for (int i = 0; i < std::min(numCharts_, static_cast<int>(functions.size())); i++) {
            int row = i / cols;
            int col = i % cols;
            
            // Calculate layout for this chart
            ChartLayout layout;
            calculateChartLayout(imageLayout, row, col, rows, cols, layout);
            
            // Validate layout
            if (!validateChartLayout(layout)) {
                std::cerr << "ERROR: Chart " << i << " layout validation failed!" << std::endl;
                boundaryViolation = true;
                continue;
            }
            
            // Create and configure chart
            Chart3D_Z_FXY chart;
            chart.setSize(layout.lineWindowWidth, layout.lineWindowHeight);
            chart.setOffset(layout.lineWindowX, layout.lineWindowY);
            chart.setShowAxis(false);  // Disable axis to eliminate extra borders
            chart.setShowGrid(false);  // Disable grid to eliminate extra borders
            chart.setBackColor(RGB_WHITE);
            chart.setGridColor({200, 200, 200});
            chart.setAxisColor({0, 0, 0});
            
            // Always disable chart's internal borders to avoid duplication
            chart.setDrawBorders(false);
            
            if (drawBorders_) {
                // Draw gray border around chart window (where all chart elements must fit)
                drawChartWindowBorder(layout, *pixelPainter);
            }
            
            if (drawDebugBorders_) {
                // Draw blue border around line window (where chart lines are drawn) - debug only
                drawLineWindowBorder(layout, *pixelPainter);
            }
            
            // Set chart range and function - optimize ranges individually for each function
            switch(i) {
                case 0: // Ripple (concentric waves) - smaller range to show more detail and prevent cutting
                    chart.setRange(-4.0f, 4.0f, -4.0f, 4.0f);
                    break;
                case 1: // Mexican hat (sombrero) - larger range to show the full hat shape
                    chart.setRange(-8.0f, 8.0f, -8.0f, 8.0f);
                    break;
                case 2: // Gaussian bell - keep as is (perfect reference)
                    chart.setRange(-2.5f, 2.5f, -2.5f, 2.5f);
                    break;
                case 3: // Peaks function - medium range to show multiple peaks clearly
                    chart.setRange(-5.0f, 5.0f, -5.0f, 5.0f);
                    break;
                default:
                    chart.setRange(-6.0f, 6.0f, -6.0f, 6.0f);
                    break;
            }
            chart.setFunction(functions[i]);
            
            // Log chart layout for debugging
            logChartLayout(i, layout);
            
            // Render this chart
            renderer.render(chart);
        }
        
        // Exit with error status if boundary violations detected
        if (boundaryViolation) {
            std::cerr << "FATAL: Boundary violations detected - exiting with error status" << std::endl;
            std::exit(1);
        }
    }

private:
    /**
     * @brief 1. Calculate total image size from current image
     */
    void calculateImageSize(ChartLayout& layout) {
        layout.imageWidth = getImage().width();
        layout.imageHeight = getImage().height();
    }
    
    /**
     * @brief 2. Calculate chart and line window layout for a specific chart
     */
    void calculateChartLayout(const ChartLayout& imageLayout, int row, int col, int rows, int cols, ChartLayout& layout) {
        // Copy image dimensions
        layout.imageWidth = imageLayout.imageWidth;
        layout.imageHeight = imageLayout.imageHeight;
        
        // 2. Calculate chart window (area where all chart elements must fit)
        int quarterWidth = layout.imageWidth / cols;
        int quarterHeight = layout.imageHeight / rows;
        
        // Chart window is slightly smaller than quarter to create separation
        int chartMargin = 5; // Small margin to separate charts
        layout.chartWindowWidth = quarterWidth - 2 * chartMargin;
        layout.chartWindowHeight = quarterHeight - 2 * chartMargin;
        layout.chartWindowX = col * quarterWidth + chartMargin;
        layout.chartWindowY = row * quarterHeight + chartMargin;
        
        // 3. Calculate line window (smaller area where only chart lines are drawn)
        layout.lineWindowWidth = layout.chartWindowWidth - layout.marginLeft - layout.marginRight;
        layout.lineWindowHeight = layout.chartWindowHeight - layout.marginTop - layout.marginBottom;
        layout.lineWindowX = layout.chartWindowX + layout.marginLeft;
        layout.lineWindowY = layout.chartWindowY + layout.marginTop;
    }
    
    /**
     * @brief Validate that all layout calculations are within bounds
     */
    bool validateChartLayout(const ChartLayout& layout) {
        // Validate chart window fits within image
        if (layout.chartWindowX < 0 || layout.chartWindowY < 0 ||
            layout.chartWindowX + layout.chartWindowWidth > layout.imageWidth ||
            layout.chartWindowY + layout.chartWindowHeight > layout.imageHeight) {
            std::cerr << "ERROR: Chart window exceeds image bounds!" << std::endl;
            return false;
        }
        
        // Validate line window is positive
        if (layout.lineWindowWidth <= 0 || layout.lineWindowHeight <= 0) {
            std::cerr << "ERROR: Line window is too small or negative!" << std::endl;
            return false;
        }
        
        // Validate line window fits within chart window
        if (layout.lineWindowX < layout.chartWindowX || layout.lineWindowY < layout.chartWindowY ||
            layout.lineWindowX + layout.lineWindowWidth > layout.chartWindowX + layout.chartWindowWidth ||
            layout.lineWindowY + layout.lineWindowHeight > layout.chartWindowY + layout.chartWindowHeight) {
            std::cerr << "ERROR: Line window exceeds chart window bounds!" << std::endl;
            return false;
        }
        
        return true;
    }
    
    /**
     * @brief Draw gray border around chart window (where all chart elements must fit)
     */
    void drawChartWindowBorder(const ChartLayout& layout, PixelPainter& painter) {
        RgbColor borderColor = {128, 128, 128}; // Gray border for chart window
        
        // Draw gray border around chart window
        for (int x = layout.chartWindowX; x < layout.chartWindowX + layout.chartWindowWidth; x++) {
            if (x >= 0 && x < layout.imageWidth && layout.chartWindowY >= 0 && layout.chartWindowY < layout.imageHeight) {
                painter.putPixel(x, layout.chartWindowY, borderColor); // Top
            }
            if (x >= 0 && x < layout.imageWidth && layout.chartWindowY + layout.chartWindowHeight - 1 >= 0 && layout.chartWindowY + layout.chartWindowHeight - 1 < layout.imageHeight) {
                painter.putPixel(x, layout.chartWindowY + layout.chartWindowHeight - 1, borderColor); // Bottom
            }
        }
        for (int y = layout.chartWindowY; y < layout.chartWindowY + layout.chartWindowHeight; y++) {
            if (layout.chartWindowX >= 0 && layout.chartWindowX < layout.imageWidth && y >= 0 && y < layout.imageHeight) {
                painter.putPixel(layout.chartWindowX, y, borderColor); // Left
            }
            if (layout.chartWindowX + layout.chartWindowWidth - 1 >= 0 && layout.chartWindowX + layout.chartWindowWidth - 1 < layout.imageWidth && y >= 0 && y < layout.imageHeight) {
                painter.putPixel(layout.chartWindowX + layout.chartWindowWidth - 1, y, borderColor); // Right
            }
        }
    }
    
    /**
     * @brief Draw blue border around line window (where chart lines are drawn) - debug only
     */
    void drawLineWindowBorder(const ChartLayout& layout, PixelPainter& painter) {
        RgbColor debugColor = {0, 0, 255}; // Blue for debugging
        
        // Draw line window border
        for (int x = layout.lineWindowX; x < layout.lineWindowX + layout.lineWindowWidth; x++) {
            if (x >= 0 && x < layout.imageWidth && layout.lineWindowY >= 0 && layout.lineWindowY < layout.imageHeight) {
                painter.putPixel(x, layout.lineWindowY, debugColor); // Top
            }
            if (x >= 0 && x < layout.imageWidth && layout.lineWindowY + layout.lineWindowHeight - 1 >= 0 && layout.lineWindowY + layout.lineWindowHeight - 1 < layout.imageHeight) {
                painter.putPixel(x, layout.lineWindowY + layout.lineWindowHeight - 1, debugColor); // Bottom
            }
        }
        for (int y = layout.lineWindowY; y < layout.lineWindowY + layout.lineWindowHeight; y++) {
            if (layout.lineWindowX >= 0 && layout.lineWindowX < layout.imageWidth && y >= 0 && y < layout.imageHeight) {
                painter.putPixel(layout.lineWindowX, y, debugColor); // Left
            }
            if (layout.lineWindowX + layout.lineWindowWidth - 1 >= 0 && layout.lineWindowX + layout.lineWindowWidth - 1 < layout.imageWidth && y >= 0 && y < layout.imageHeight) {
                painter.putPixel(layout.lineWindowX + layout.lineWindowWidth - 1, y, debugColor); // Right
            }
        }
    }
    
    /**
     * @brief Log chart layout for debugging
     */
    void logChartLayout(int chartIndex, const ChartLayout& layout) {
        std::cerr << "Chart " << chartIndex << " Layout:" << std::endl;
        std::cerr << "  Image: " << layout.imageWidth << "x" << layout.imageHeight << std::endl;
        std::cerr << "  Chart Window: pos=(" << layout.chartWindowX << "," << layout.chartWindowY 
                  << "), size=(" << layout.chartWindowWidth << "x" << layout.chartWindowHeight << ")" << std::endl;
        std::cerr << "  Margins: T=" << layout.marginTop << ", B=" << layout.marginBottom 
                  << ", L=" << layout.marginLeft << ", R=" << layout.marginRight << std::endl;
        std::cerr << "  Line Window: pos=(" << layout.lineWindowX << "," << layout.lineWindowY 
                  << "), size=(" << layout.lineWindowWidth << "x" << layout.lineWindowHeight << ")" << std::endl;
    }
    void determineLayout(int &rows, int &cols) {
        if (layout_ == "auto") {
            // Auto-determine layout based on number of charts
            if (numCharts_ <= 2) {
                rows = 1;
                cols = numCharts_;
            } else if (numCharts_ <= 4) {
                rows = 2;
                cols = 2;
            } else if (numCharts_ <= 6) {
                rows = 2;
                cols = 3;
            } else if (numCharts_ <= 9) {
                rows = 3;
                cols = 3;
            } else {
                rows = 4;
                cols = 3;
            }
        } else if (layout_ == "horizontal") {
            rows = 1;
            cols = numCharts_;
        } else if (layout_ == "vertical") {
            rows = numCharts_;
            cols = 1;
        } else {
            // Try to parse custom layout like "2x3"
            size_t pos = layout_.find('x');
            if (pos != std::string::npos) {
                try {
                    rows = std::stoi(layout_.substr(0, pos));
                    cols = std::stoi(layout_.substr(pos + 1));
                } catch (...) {
                    // Default to auto layout if parsing fails
                    determineLayout(rows, cols);
                }
            } else {
                // Default to auto layout
                determineLayout(rows, cols);
            }
        }
    }
    
    std::vector<std::function<float(float, float)>> createFunctions() {
        std::vector<std::function<float(float, float)>> functions;
        
        // Ripple function - visually striking with concentric waves (shifted down to use more bottom space)
        functions.push_back([](float x, float y) {
            float r = std::sqrt(x*x + y*y);
            return r == 0 ? 0.2f : 0.4f * std::sin(r * 2.0f) / (r * 0.5f + 0.1f) - 0.3f;
        });
        
        // Mexican hat (sombrero) - classic 3D visualization with central peak and ring (shifted down to use more bottom space)
        functions.push_back([](float x, float y) {
            float r2 = x*x + y*y;
            return 0.6f * (2.0f - r2 * 0.3f) * std::exp(-r2 * 0.2f) - 0.4f;
        });
        
        // Gaussian bell - keep as perfect reference (reduce amplitude to prevent clipping, add offset)
        functions.push_back([](float x, float y) {
            return 0.8f * std::exp(-(x*x + y*y)) - 0.1f;
        });
        
        // Peaks function - multiple peaks and valleys, reduced amplitude to prevent cutting
        functions.push_back([](float x, float y) {
            return 0.3f * (3.0f * std::exp(-(x-1)*(x-1) - (y+1)*(y+1)) - 
                          2.0f * std::exp(-x*x - y*y) - 
                          1.5f * std::exp(-(x+1)*(x+1) - (y-1)*(y-1)) +
                          std::exp(-(x-2)*(x-2) - y*y)) - 0.2f;
        });
        
        // Additional functions for variety
        
        // Sinc function
        functions.push_back([](float x, float y) {
            float r = std::sqrt(x*x + y*y) * 3.0f;
            return r == 0 ? 1.0f : std::sin(r) / r;
        });
        
        // Hyperbolic paraboloid
        functions.push_back([](float x, float y) {
            return x*y;
        });
        
        // Sphere
        functions.push_back([](float x, float y) {
            float r2 = x*x + y*y;
            return r2 <= 4.0f ? std::sqrt(4.0f - r2) : 0.0f;
        });
        
        // Wave interference pattern
        functions.push_back([](float x, float y) {
            return 0.5f * (std::sin(x) * std::sin(y) + 0.3f * std::sin(x*2 + y) + 0.2f * std::cos(x - y*1.5f));
        });
        
        // Twisted surface
        functions.push_back([](float x, float y) {
            return 0.4f * std::sin(x + y) * std::cos(x - y) + 0.2f * std::sin(x*y*0.5f);
        });
        
        // Volcano-like function
        functions.push_back([](float x, float y) {
            float r2 = x*x + y*y;
            return r2 < 1.0f ? 2.0f * (1.0f - r2) : 0.3f * std::exp(-(r2 - 1.0f));
        });
        
        return functions;
    }
    
    int numCharts_;
    bool useAntiAliasing_;
    bool drawBorders_;
    std::string layout_;
    int margin_; // Default margin in pixels
    bool drawDebugBorders_; // Controls whether to draw blue debug borders around line windows
    std::unique_ptr<PixelPainterForImageBase> pixelPainter_;
    std::unique_ptr<AntiAliasingFilter> aaFilter_;
};

#endif
