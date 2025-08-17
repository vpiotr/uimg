#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <memory>
#include <cmath>
#include "../common/include/demo_painter_base.h"
#include "../common/include/chart3d/chart_z_fxy_3d.h"
#include "../common/include/chart3d/chart3d_z_fxy.h"
#include "uimg/filters/anti_aliasing_filter.h"
// Text rendering includes
#include "uimg/fonts/bdf_font.h"
#include "uimg/fonts/font_utils.h"
#include "uimg/fonts/painter_for_bdf_font_ex.h"
#include "dlog/dlog.h"

/**
 * @brief Pixel painter that applies an offset to all drawing operations
 */
class OffsetPixelPainter : public PixelPainter {
public:
    OffsetPixelPainter(PixelPainter& basePainter, Point offset) 
        : basePainter_(basePainter), offset_(offset) {}
    
    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor& color) override {
        basePainter_.putPixel(x + static_cast<unsigned int>(offset_.x), y + static_cast<unsigned int>(offset_.y), color);
    }
    
    virtual void getPixel(unsigned int x, unsigned int y, RgbColor& output) override {
        basePainter_.getPixel(x + static_cast<unsigned int>(offset_.x), y + static_cast<unsigned int>(offset_.y), output);
    }
    
private:
    PixelPainter& basePainter_;
    Point offset_;
};

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
    multi_chart_3d_demo(const std::string &fname, int numCharts, bool useAntiAliasing, bool drawBorders, const std::string &layout = "auto", bool darkMode = false, int imageWidth = 800, int imageHeight = 600, const std::string &fontPath = "fonts/courR12.bdf") 
        : demo_painter_base(fname), numCharts_(numCharts), useAntiAliasing_(useAntiAliasing), drawBorders_(drawBorders), layout_(layout),
          margin_(10), drawDebugBorders_(false), darkMode_(darkMode), titleVerticalPositionMultiplier_(4.0f), titleFontScale_(1.0f), 
          imageWidth_(imageWidth), imageHeight_(imageHeight), fontPath_(fontPath) {} // Default margin of 10 pixels on all sides, debug borders off by default

    // Method to enable debug borders for line windows
    void setDrawDebugBorders(bool enable) { drawDebugBorders_ = enable; }
    
    // Method to configure title vertical position (multiplier for font height margin from top border)
    void setTitleVerticalPosition(float multiplier) { titleVerticalPositionMultiplier_ = multiplier; }
    
    // Method to configure title font scale (1.0 = normal size, 0.8 = smaller, 1.2 = larger)
    void setTitleFontScale(float scale) { titleFontScale_ = scale; }

protected:
    /**
     * @brief Override getImageSize to provide configurable size for text painter initialization
     */
    virtual Point getImageSize() override {
        return {imageWidth_, imageHeight_}; // Configurable size for multi-chart demo
    }
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
        RgbColor backgroundColor = darkMode_ ? RGB_BLACK : RGB_WHITE;
        backPainter.paint(backgroundColor);
        
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
        
        // Initialize text rendering for chart titles (after image is set up)
        initializeTextRenderer();
        
        // Render each chart
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
            chart.setBackColor(backgroundColor);
            chart.setGridColor(darkMode_ ? RgbColor{100, 100, 100} : RgbColor{200, 200, 200});
            chart.setAxisColor(darkMode_ ? RgbColor{200, 200, 200} : RgbColor{0, 0, 0});
            
            // Set color scheme configuration
            chart.setChartIndex(i);
            chart.setDarkMode(darkMode_);
            
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
            
            // Draw chart title
            drawChartTitle(i, layout);

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
            
            // Create offset painter for chart position (like Chart3DRenderer does)
            Point offset = chart.getOffset();
            OffsetPixelPainter offsetPainter(*pixelPainter, offset);
            
            // Create and render the internal chart with proper offset
            auto internalChart = chart.createInternalChart(offsetPainter, useAntiAliasing_);
            internalChart->paint();
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
        
        // Special case: For 3 charts, make the bottom chart (row=1, col=0) span the full width
        if (numCharts_ == 3 && row == 1 && col == 0) {
            layout.chartWindowWidth = layout.imageWidth - 2 * chartMargin;
            layout.chartWindowHeight = quarterHeight - 2 * chartMargin;
            layout.chartWindowX = chartMargin;
            layout.chartWindowY = row * quarterHeight + chartMargin;
        } else {
            layout.chartWindowWidth = quarterWidth - 2 * chartMargin;
            layout.chartWindowHeight = quarterHeight - 2 * chartMargin;
            layout.chartWindowX = col * quarterWidth + chartMargin;
            layout.chartWindowY = row * quarterHeight + chartMargin;
        }
        
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
    
    /**
     * @brief Initialize text rendering for chart titles
     */
    void initializeTextRenderer() {
        if (font_ && textPainter_) {
            return; // Already initialized
        }
        
        // Load font for chart titles
        font_ = std::make_unique<uimg::BdfFont>();
        if (!uimg::FontUtils::loadFontFromFile(*font_, fontPath_)) {
            std::cerr << "Warning: Failed to load font from: " << fontPath_ << std::endl;
            std::cerr << "Chart titles will not be displayed." << std::endl;
            return;
        }
        
        // Initialize text painter using the correct constructor pattern from text_demo
        textPainter_ = std::make_unique<uimg::TextPainterForBdfFontEx>(getPainter(), getImageSize());
        textPainter_->setFont(font_.get());
        // Note: scale and alignment will be set in drawChartTitle() using configurable values
    }
    
    /**
     * @brief Helper function to get pixel painter for text rendering
     */
    PixelPainterForImageBase& getPainter() {
        static PixelPainterForImageBase painter(getImage());
        return painter;
    }
    
    /**
     * @brief Get chart title with mathematical formula
     */
    std::string getChartTitle(int chartIndex) {
        switch(chartIndex) {
            case 0:
                return "Ripple: z = 0.4*sin(2r)/(0.5r+0.1) - 0.3";
            case 1:
                return "Mexican Hat: z = 0.6*(2-0.3r^2)*exp(-0.2r^2) - 0.4";
            case 2:
                return "Gaussian: z = 0.8*exp(-(x^2+y^2)) - 0.1";
            case 3:
                return "Peaks: z = multi-peak function";
            case 4:
                return "Sinc: z = sin(3r)/(3r)";
            case 5:
                return "Paraboloid: z = x*y";
            case 6:
                return "Sphere: z = sqrt(4-r^2)";
            case 7:
                return "Waves: z = sin(x)*sin(y) + ...";
            case 8:
                return "Twisted: z = 0.4*sin(x+y)*cos(x-y)";
            case 9:
                return "Volcano: z = piecewise function";
            default:
                return "Function " + std::to_string(chartIndex);
        }
    }
    
    /**
     * @brief Calculate title position inside the chart window
     * @param title The title text to calculate position for
     * @param layout The chart layout information
     * @return Point containing the calculated title position
     */
    Point calculateTitlePosition(const std::string& title, const ChartLayout& layout) {
        if (!textPainter_ || !font_) {
            return {0, 0}; // Return default if text rendering not available
        }
        
        // Calculate font height and use configurable multiplier as margin from border
        Point textSize = textPainter_->textSize(title);
        int fontHeight = textSize.y;
        int marginFromBorder = static_cast<int>(fontHeight * titleVerticalPositionMultiplier_);
        
        // Position title inside the chart window, centered horizontally
        int titleX = layout.chartWindowX + layout.chartWindowWidth / 2;
        int titleY = layout.chartWindowY + marginFromBorder;
        
        // Ensure coordinates are within chart window bounds
        titleX = std::max(titleX, layout.chartWindowX + 10);
        titleX = std::min(titleX, layout.chartWindowX + layout.chartWindowWidth - 10);
        titleY = std::max(titleY, layout.chartWindowY + marginFromBorder);
        titleY = std::min(titleY, layout.chartWindowY + layout.chartWindowHeight - fontHeight - 10);
        
        return {titleX, titleY};
    }
    
    /**
     * @brief Draw chart title inside the chart window
     */
    void drawChartTitle(int chartIndex, const ChartLayout& layout) {
        if (!textPainter_ || !font_) {
            return; // Text rendering not available
        }
        
        std::string title = getChartTitle(chartIndex);
        RgbColor titleColor = darkMode_ ? RgbColor{220, 220, 220} : RgbColor{40, 40, 40};
        textPainter_->setTextColor(titleColor);
        
        // Use configurable font scale
        textPainter_->setScale(titleFontScale_);
        textPainter_->setAlignment(uimg::TextAlignment::CENTER);
        
        // Calculate title position using the new helper function
        Point titlePos = calculateTitlePosition(title, layout);
        
        // Draw text using standard method like in text demo
        textPainter_->drawText(titlePos, title);
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
    bool darkMode_; // Controls whether to use dark mode (black background)
    float titleVerticalPositionMultiplier_; // Multiplier for font height to determine title vertical position from top border
    float titleFontScale_; // Scale factor for title font size (1.0 = normal size)
    int imageWidth_; // Configurable image width
    int imageHeight_; // Configurable image height
    std::string fontPath_; // Path to BDF font file
    std::unique_ptr<PixelPainterForImageBase> pixelPainter_;
    std::unique_ptr<AntiAliasingFilter> aaFilter_;
    
    // Text rendering for chart titles
    std::unique_ptr<uimg::BdfFont> font_;
    std::unique_ptr<uimg::TextPainterForBdfFontEx> textPainter_;
};

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " [options] <output_file>" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -charts <num>    Number of charts to display (1-4, default: 4)" << std::endl;
    std::cerr << "  -size <WxH>      Output image size in pixels (e.g., 1024x768, default: 800x600)" << std::endl;
    std::cerr << "  -font <path>     Path to BDF font file (default: fonts/courR12.bdf)" << std::endl;
    std::cerr << "  -aa              Enable anti-aliasing" << std::endl;
    std::cerr << "  -borders         Enable chart borders" << std::endl;
    std::cerr << "  -debug           Enable debug borders (blue borders around line windows)" << std::endl;
    std::cerr << "  -dark            Enable dark mode (black background)" << std::endl;
    std::cerr << "  -layout <type>   Layout type: auto, horizontal, vertical, or NxM (e.g., 2x2)" << std::endl;
    std::cerr << "  -trace           Enable chart tracing for debug output" << std::endl;
    std::cerr << "  -log <level>     Set log level (TRACE, DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cerr << "  -help            Show this help message" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Examples:" << std::endl;
    std::cerr << "  " << programName << " output.ppm" << std::endl;
    std::cerr << "  " << programName << " -charts 2 -aa output.ppm" << std::endl;
    std::cerr << "  " << programName << " -charts 4 -borders -layout 2x2 output.ppm" << std::endl;
    std::cerr << "  " << programName << " -charts 4 -borders -debug output.ppm" << std::endl;
    std::cerr << "  " << programName << " -charts 4 -dark -aa output.ppm" << std::endl;
    std::cerr << "  " << programName << " -size 1200x900 -charts 3 output.ppm" << std::endl;
    std::cerr << "  " << programName << " -font ../fonts/courR12.bdf -charts 3 output.ppm" << std::endl;
}

int main(int argc, const char *argv[]) {
    // Default values
    int numCharts = 4;
    int imageWidth = 800;
    int imageHeight = 600;
    std::string fontPath = "fonts/courR12.bdf";
    bool useAntiAliasing = false;
    bool drawBorders = false;
    bool drawDebugBorders = false;
    bool darkMode = false;
    bool traceEnabled = false;
    std::string layout = "auto";
    std::string outFileName;
    dlog::LogLevel logLevel = dlog::LogLevel::INFO;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-help" || arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-charts" && i + 1 < argc) {
            numCharts = std::atoi(argv[++i]);
            if (numCharts < 1 || numCharts > 4) {
                std::cerr << "Error: Number of charts must be between 1 and 4" << std::endl;
                return 1;
            }
        } else if (arg == "-size" && i + 1 < argc) {
            std::string sizeStr = argv[++i];
            size_t xPos = sizeStr.find('x');
            if (xPos == std::string::npos) {
                xPos = sizeStr.find('X');
            }
            if (xPos != std::string::npos) {
                try {
                    imageWidth = std::stoi(sizeStr.substr(0, xPos));
                    imageHeight = std::stoi(sizeStr.substr(xPos + 1));
                    if (imageWidth < 100 || imageHeight < 100 || imageWidth > 4096 || imageHeight > 4096) {
                        std::cerr << "Error: Image size must be between 100x100 and 4096x4096 pixels" << std::endl;
                        return 1;
                    }
                } catch (...) {
                    std::cerr << "Error: Invalid size format. Use WIDTHxHEIGHT (e.g., 1024x768)" << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: Invalid size format. Use WIDTHxHEIGHT (e.g., 1024x768)" << std::endl;
                return 1;
            }
        } else if (arg == "-font" && i + 1 < argc) {
            fontPath = argv[++i];
        } else if (arg == "-aa") {
            useAntiAliasing = true;
        } else if (arg == "-borders") {
            drawBorders = true;
        } else if (arg == "-debug") {
            drawDebugBorders = true;
        } else if (arg == "-dark") {
            darkMode = true;
        } else if (arg == "-layout" && i + 1 < argc) {
            layout = argv[++i];
        } else if (arg == "-trace") {
            traceEnabled = true;
        } else if (arg == "-log" && i + 1 < argc) {
            std::string levelStr = argv[++i];
            if (levelStr == "TRACE") logLevel = dlog::LogLevel::TRACE;
            else if (levelStr == "DEBUG") logLevel = dlog::LogLevel::DEBUG;
            else if (levelStr == "INFO") logLevel = dlog::LogLevel::INFO;
            else if (levelStr == "WARN") logLevel = dlog::LogLevel::WARN;
            else if (levelStr == "ERROR") logLevel = dlog::LogLevel::ERROR;
            else {
                std::cerr << "Error: Invalid log level: " << levelStr << std::endl;
                return 1;
            }
        } else if (arg[0] != '-') {
            // Output filename
            outFileName = arg;
        } else {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Check if output filename was provided
    if (outFileName.empty()) {
        std::cerr << "Error: Output filename is required" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    // Initialize logger
    dlog::Logger::getInstance()->setLevel(logLevel);
    
    // Display configuration
    std::cerr << "Multi-Chart 3D Demo Configuration:" << std::endl;
    std::cerr << "  Output file: " << outFileName << std::endl;
    std::cerr << "  Image size: " << imageWidth << "x" << imageHeight << std::endl;
    std::cerr << "  Font path: " << fontPath << std::endl;
    std::cerr << "  Number of charts: " << numCharts << std::endl;
    std::cerr << "  Layout: " << layout << std::endl;
    std::cerr << "  Anti-aliasing: " << (useAntiAliasing ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "  Borders: " << (drawBorders ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "  Debug borders: " << (drawDebugBorders ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "  Dark mode: " << (darkMode ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "  Tracing: " << (traceEnabled ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "  Log Level: " << dlog::Logger::levelToString(logLevel) << std::endl;
    std::cerr << std::endl;

    // Enable tracing if requested
    Chart3DTracer::getInstance()->setEnabled(traceEnabled);

    try {
        // Create and run the multi-chart demo
        multi_chart_3d_demo demo(outFileName, numCharts, useAntiAliasing, drawBorders, layout, darkMode, imageWidth, imageHeight, fontPath);
        
        // Enable debug borders if requested
        if (drawDebugBorders) {
            demo.setDrawDebugBorders(true);
        }
        
        demo.run();
        
        std::cerr << "Multi-chart 3D demo completed successfully!" << std::endl;
        std::cerr << "Generated: " << outFileName << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
