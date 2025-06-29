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
            
            // Set chart range and function - optimized ranges for better sampling and detail
            switch(i) {
                case 0: // sin(x*y) - moderate range for good sampling with multiple periods
                    chart.setRange(-3.0f, 3.0f, -3.0f, 3.0f);
                    break;
                case 1: // sin(x)*cos(y) - range that shows multiple wave periods
                    chart.setRange(-4.0f, 4.0f, -4.0f, 4.0f);
                    break;
                case 2: // Gaussian bell - standard range with proper amplitude
                    chart.setRange(-2.5f, 2.5f, -2.5f, 2.5f);
                    break;
                case 3: // Saddle - wider range to show multiple extremes
                    chart.setRange(-3.0f, 3.0f, -3.0f, 3.0f);
                    break;
                default:
                    chart.setRange(-2.0f, 2.0f, -2.0f, 2.0f);
                    break;
            }
            chart.setFunction(functions[i]);
            
            // Log chart boundaries for debugging
            std::cerr << "Chart " << i << ": position=(" << x << "," << y 
                      << "), size=(" << chartWidth << "x" << chartHeight << ")" << std::endl;
            
            // Render this chart
            renderer.render(chart);
            
            // Check for boundary violations (simplified check)
            if (x < 0 || y < 0 || x + chartWidth > getImage().width() || y + chartHeight > getImage().height()) {
                std::cerr << "ERROR: Chart " << i << " boundary violation detected!" << std::endl;
                boundaryViolation = true;
            }
        }
        
        // Exit with error status if boundary violations detected
        if (boundaryViolation) {
            std::cerr << "FATAL: Boundary violations detected - exiting with error status" << std::endl;
            std::exit(1);
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
        
        // Basic sin(x*y) - moderate amplitude to show function pattern
        functions.push_back([](float x, float y) {
            return std::sin(x * y) * 1.0f;
        });
        
        // Sin waves - good amplitude for clear wave patterns
        functions.push_back([](float x, float y) {
            return std::sin(x) * std::cos(y) * 1.0f;
        });
        
        // Gaussian bell - reduce amplitude to prevent clipping, add offset
        functions.push_back([](float x, float y) {
            return 0.8f * std::exp(-(x*x + y*y)) - 0.1f;
        });
        
        // Saddle - increase amplitude to show better contrast
        functions.push_back([](float x, float y) {
            return 0.6f * (x*x - y*y);
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
    std::unique_ptr<PixelPainterForImageBase> pixelPainter_;
    std::unique_ptr<AntiAliasingFilter> aaFilter_;
};

#endif
