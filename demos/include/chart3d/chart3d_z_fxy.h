#ifndef __UIMG_CHART3D_Z_FXY_H__
#define __UIMG_CHART3D_Z_FXY_H__

#include <functional>
#include <cmath>
#include <limits>
#include "uimg/base/structs.h"
#include "uimg/painters/painter_for_pixels.h"
#include "chart_z_fxy_3d.h"

/**
 * @brief 3D Chart class with configurable parameters and function
 * This class provides a high-level interface for creating 3D function charts
 * with customizable appearance and mathematical functions.
 */
class Chart3D_Z_FXY {
public:
    Chart3D_Z_FXY() 
        : size_(800, 600), offset_(0, 0), function_(nullptr),
          rangeX_(-2.0f, 2.0f), rangeY_(-2.0f, 2.0f),
          showAxis_(true), showGrid_(true),
          backColor_(RGB_WHITE), gridColor_({200, 200, 200}), 
          axisColor_({0, 0, 0}), borderColor_({128, 128, 128}),
          drawBorders_(false), chartIndex_(0), darkMode_(false) {}

    // Size and position
    void setSize(int width, int height) {
        size_ = Point(width, height);
    }
    
    void setOffset(int x, int y) {
        offset_ = Point(x, y);
    }
    
    Point getSize() const { return size_; }
    Point getOffset() const { return offset_; }

    // Display options
    void setShowAxis(bool show) { showAxis_ = show; }
    void setShowGrid(bool show) { showGrid_ = show; }
    bool getShowAxis() const { return showAxis_; }
    bool getShowGrid() const { return showGrid_; }

    // Colors
    void setBackColor(const RgbColor& color) { backColor_ = color; }
    void setGridColor(const RgbColor& color) { gridColor_ = color; }
    void setAxisColor(const RgbColor& color) { axisColor_ = color; }
    void setBorderColor(const RgbColor& color) { borderColor_ = color; }
    
    RgbColor getBackColor() const { return backColor_; }
    RgbColor getGridColor() const { return gridColor_; }
    RgbColor getAxisColor() const { return axisColor_; }
    RgbColor getBorderColor() const { return borderColor_; }

    // Borders
    void setDrawBorders(bool draw) { drawBorders_ = draw; }
    bool getDrawBorders() const { return drawBorders_; }

    // Color scheme configuration
    void setChartIndex(int index) { chartIndex_ = index; }
    void setDarkMode(bool dark) { darkMode_ = dark; }
    int getChartIndex() const { return chartIndex_; }
    bool getDarkMode() const { return darkMode_; }

    // Range
    void setRange(float minX, float maxX, float minY, float maxY) {
        rangeX_ = std::make_pair(minX, maxX);
        rangeY_ = std::make_pair(minY, maxY);
    }
    
    std::pair<float, float> getRangeX() const { return rangeX_; }
    std::pair<float, float> getRangeY() const { return rangeY_; }

    // Function
    void setFunction(const std::function<float(float, float)>& func) {
        function_ = func;
    }
    
    std::function<float(float, float)> getFunction() const {
        return function_;
    }

    /**
     * @brief Create an internal chart_z_fxy_3d implementation for rendering
     * @param painter The pixel painter to use for rendering
     * @param useAntiAliasing Whether to enable anti-aliasing
     * @return Unique pointer to the internal chart implementation
     */
    std::unique_ptr<chart_z_fxy_3d> createInternalChart(PixelPainter& painter, bool useAntiAliasing = false) const {
        // Create internal chart class that implements the specific function
        class Chart3D_Z_FXY_Impl : public chart_z_fxy_3d {
        public:
            Chart3D_Z_FXY_Impl(const Point& canvasSize, PixelPainter& pixelPainter, 
                             bool useAntiAliasing, bool drawBorders,
                             const Chart3D_Z_FXY& config)
                : chart_z_fxy_3d(canvasSize, pixelPainter, useAntiAliasing, drawBorders),
                  config_(config), zMin_(0.0), zMax_(1.0), rangeSampled_(false) {}

        protected:
            virtual RgbColor getPlotColor(double x, double y, double z) override {
                // Sample Z range on first call if not done yet
                if (!rangeSampled_) {
                    sampleZRange();
                    rangeSampled_ = true;
                }
                
                // Update the Z range dynamically as we see new values
                if (z < zMin_) zMin_ = z;
                if (z > zMax_) zMax_ = z;
                
                // Normalize z to 0-1 range using actual sampled range
                double normalizedZ = 0.5; // Default to middle if range is invalid
                if (zMax_ > zMin_) {
                    normalizedZ = (z - zMin_) / (zMax_ - zMin_);
                    normalizedZ = std::max(0.0, std::min(1.0, normalizedZ));
                }
                
                // Simple linear color interpolation based on chart index
                RgbColor color = getChartColorScheme(normalizedZ);
                
                // Adjust brightness for dark mode
                if (config_.getDarkMode()) {
                    color = brightenForDarkMode(color);
                }
                
                return color;
            }

        private:
            mutable double zMin_, zMax_;
            mutable bool rangeSampled_;
            
            // Sample the actual Z range of the function to get proper color mapping
            void sampleZRange() const {
                if (!config_.getFunction()) {
                    zMin_ = -1.0;
                    zMax_ = 1.0;
                    return;
                }
                
                auto rangeX = config_.getRangeX();
                auto rangeY = config_.getRangeY();
                
                // Sample the function at multiple points to find min/max Z values
                const int samples = 50; // 50x50 = 2500 sample points
                double minZ = std::numeric_limits<double>::max();
                double maxZ = std::numeric_limits<double>::lowest();
                
                for (int i = 0; i < samples; i++) {
                    for (int j = 0; j < samples; j++) {
                        float x = rangeX.first + (rangeX.second - rangeX.first) * i / (samples - 1);
                        float y = rangeY.first + (rangeY.second - rangeY.first) * j / (samples - 1);
                        float z = config_.getFunction()(x, y);
                        
                        minZ = std::min(minZ, static_cast<double>(z));
                        maxZ = std::max(maxZ, static_cast<double>(z));
                    }
                }
                
                zMin_ = minZ;
                zMax_ = maxZ;
                
                // Add small margin to avoid edge cases
                double margin = (zMax_ - zMin_) * 0.05;
                zMin_ -= margin;
                zMax_ += margin;
            }
            
            // Simple linear color interpolation between two colors
            RgbColor linearColorInterpolation(const RgbColor& color1, const RgbColor& color2, double t) const {
                double red = color1.red + (color2.red - color1.red) * t;
                double green = color1.green + (color2.green - color1.green) * t;
                double blue = color1.blue + (color2.blue - color1.blue) * t;
                
                return {
                    static_cast<unsigned char>(std::max(0.0, std::min(255.0, red))),
                    static_cast<unsigned char>(std::max(0.0, std::min(255.0, green))),
                    static_cast<unsigned char>(std::max(0.0, std::min(255.0, blue)))
                };
            }
            
            // Get color scheme for each chart with simple linear gradients
            RgbColor getChartColorScheme(double t) const {
                switch (config_.getChartIndex() % 4) {
                    case 0: // Deep Blue to Bright Red (cold to hot) - very vibrant
                        return linearColorInterpolation({20, 20, 200}, {220, 20, 20}, t);
                    case 1: // Dark Purple to Bright Orange (mystical to warm) - very vibrant
                        return linearColorInterpolation({120, 20, 160}, {220, 140, 20}, t);
                    case 2: // Deep Navy to Bright Cyan (ocean depths to sky) - very vibrant, no green
                        return linearColorInterpolation({20, 20, 140}, {20, 200, 220}, t);
                    case 3: // Dark Maroon to Bright Pink (wine to rose) - very vibrant, no green
                        return linearColorInterpolation({140, 20, 40}, {220, 20, 140}, t);
                    default:
                        return linearColorInterpolation({60, 60, 60}, {180, 180, 180}, t);
                }
            }
            
            RgbColor brightenForDarkMode(const RgbColor& color) {
                // Significantly increase brightness and saturation for better visibility on dark background
                const double brightnessFactor = 1.8;  // Much brighter for dark mode
                const double saturationBoost = 50;    // Add saturation boost
                
                return {
                    static_cast<unsigned char>(std::min(255.0, static_cast<double>(color.red) * brightnessFactor + saturationBoost)),
                    static_cast<unsigned char>(std::min(255.0, static_cast<double>(color.green) * brightnessFactor + saturationBoost)),
                    static_cast<unsigned char>(std::min(255.0, static_cast<double>(color.blue) * brightnessFactor + saturationBoost))
                };
            }

        protected:

            virtual double getFunValue(double x, double y) override {
                if (config_.getFunction()) {
                    // Scale input coordinates from chart range to function range
                    auto rangeX = config_.getRangeX();
                    auto rangeY = config_.getRangeY();
                    
                    // Convert from chart coordinates to function domain
                    double scaledX = rangeX.first + (rangeX.second - rangeX.first) * (x + 2520.0) / 5040.0;
                    double scaledY = rangeY.first + (rangeY.second - rangeY.first) * (y + 2520.0) / 5040.0;
                    
                    return static_cast<double>(config_.getFunction()(static_cast<float>(scaledX), static_cast<float>(scaledY)));
                }
                
                // Default function: sinc
                double r = sqrt(x * x + y * y) * 0.001;
                return r == 0 ? 1.0 : sin(r) / r;
            }

            virtual RgbColor getBackgroundColor() const override {
                return config_.getBackColor();
            }

            virtual RgbColor getBorderColor() const override {
                return config_.getBorderColor();
            }

        private:
            const Chart3D_Z_FXY& config_;
        };

        return std::make_unique<Chart3D_Z_FXY_Impl>(size_, painter, useAntiAliasing, drawBorders_, *this);
    }

private:
    Point size_;
    Point offset_;
    std::function<float(float, float)> function_;
    std::pair<float, float> rangeX_;
    std::pair<float, float> rangeY_;
    
    bool showAxis_;
    bool showGrid_;
    
    RgbColor backColor_;
    RgbColor gridColor_;
    RgbColor axisColor_;
    RgbColor borderColor_;
    
    bool drawBorders_;
    int chartIndex_;
    bool darkMode_;
};

#endif // __UIMG_CHART3D_Z_FXY_H__
