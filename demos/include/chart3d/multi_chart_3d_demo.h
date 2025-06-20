#ifndef __UIMG_DEMOS_MULTI_CHART_3D_H__
#define __UIMG_DEMOS_MULTI_CHART_3D_H__

#include <vector>
#include <functional>
#include <string>

#include "samples/demo_painter_base.h"
#include "chart_z_fxy_3d.h"
#include "chart3d_renderer.h"
#include "chart3d_z_fxy.h"
#include "uimg/filters/anti_aliasing_filter.h"

class multi_chart_3d_demo : public demo_painter_base {
public:
    multi_chart_3d_demo(const std::string &fname, int numCharts, bool useAntiAliasing, bool drawBorders, const std::string &layout = "auto") 
        : demo_painter_base(fname), numCharts_(numCharts), useAntiAliasing_(useAntiAliasing), drawBorders_(drawBorders), layout_(layout) {}

protected:
    virtual void paint() {
        // Define chart layout based on the number of charts
        int rows, cols;
        determineLayout(rows, cols);
        
        // Calculate chart dimensions
        int chartWidth = getImage().width() / cols;
        int chartHeight = getImage().height() / rows;
        
        // Create chart functions
        std::vector<std::function<float(float, float)>> functions = createFunctions();
        
        // Initialize painters
        BackgroundPainterForRgbImage backPainter(getImage());
        backPainter.paint(RGB_WHITE);
        
        PixelPainter *pixelPainter = nullptr;
        if (useAntiAliasing_) {
            aaFilter_ = std::make_unique<AntiAliasingFilter>(getImage());
            pixelPainter = &aaFilter_->getFilteredPainter();
        } else {
            pixelPainter = &pixelPainter_;
        }
        
        // Render each chart
        Chart3DRenderer renderer(*pixelPainter);
        
        for (int i = 0; i < std::min(numCharts_, static_cast<int>(functions.size())); i++) {
            int row = i / cols;
            int col = i % cols;
            
            // Calculate chart position
            int x = col * chartWidth;
            int y = row * chartHeight;
            
            // Create and configure chart
            Chart3D_Z_FXY chart;
            chart.setSize(chartWidth, chartHeight);
            chart.setOffset(x, y);
            chart.setShowAxis(true);
            chart.setShowGrid(true);
            chart.setBackColor(RGB_WHITE);
            chart.setGridColor({200, 200, 200});
            chart.setAxisColor({0, 0, 0});
            
            if (drawBorders_) {
                chart.setDrawBorders(true);
                chart.setBorderColor({180, 180, 180});
            }
            
            // Set chart range and function
            chart.setRange(-2.0f, 2.0f, -2.0f, 2.0f);
            chart.setFunction(functions[i]);
            
            // Render this chart
            renderer.render(chart);
        }
    }

private:
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
        
        // Basic sin(x*y)
        functions.push_back([](float x, float y) {
            return std::sin(x * y) * 2.0f;
        });
        
        // Sin waves
        functions.push_back([](float x, float y) {
            return std::sin(x) * std::cos(y);
        });
        
        // Gaussian bell
        functions.push_back([](float x, float y) {
            return 2.0f * std::exp(-(x*x + y*y));
        });
        
        // Saddle
        functions.push_back([](float x, float y) {
            return x*x - y*y;
        });
        
        // Ripple
        functions.push_back([](float x, float y) {
            float r = std::sqrt(x*x + y*y);
            return r == 0 ? 2.0f : 2.0f * std::sin(r*3.0f) / r;
        });
        
        // Mexican hat
        functions.push_back([](float x, float y) {
            float r2 = x*x + y*y;
            return 2.0f * (1.0f - r2) * std::exp(-r2/2.0f);
        });
        
        // Peaks
        functions.push_back([](float x, float y) {
            return 3.0f * std::exp(-x*x - (y+1)*(y+1)) - 
                   2.0f * std::exp(-x*x - y*y) - 
                   1.0f * std::exp(-(x+1)*(x+1) - y*y);
        });
        
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
        
        return functions;
    }
    
    int numCharts_;
    bool useAntiAliasing_;
    bool drawBorders_;
    std::string layout_;
    PixelPainterForImageBase pixelPainter_{getImage()};
    std::unique_ptr<AntiAliasingFilter> aaFilter_;
};

#endif
