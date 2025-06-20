#ifndef __UIMG_CHART3D_Z_FXY_H__
#define __UIMG_CHART3D_Z_FXY_H__

#include <functional>
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
          drawBorders_(false) {}

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
                  config_(config) {}

        protected:
            virtual RgbColor getPlotColor(double x, double y, double z) override {
                // Create a colorful gradient based on z-value
                RgbColor color;
                double normalizedZ = std::max(-1.0, std::min(1.0, z / 3.0)); // Normalize to [-1, 1]
                
                if (normalizedZ >= 0) {
                    // Positive values: blue to red gradient
                    color.red = static_cast<unsigned char>(255 * normalizedZ);
                    color.green = static_cast<unsigned char>(100 * (1.0 - normalizedZ));
                    color.blue = static_cast<unsigned char>(255 * (1.0 - normalizedZ));
                } else {
                    // Negative values: green to blue gradient
                    double absZ = -normalizedZ;
                    color.red = static_cast<unsigned char>(50 * (1.0 - absZ));
                    color.green = static_cast<unsigned char>(255 * absZ);
                    color.blue = static_cast<unsigned char>(150 + 105 * absZ);
                }
                
                return color;
            }

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
};

#endif // __UIMG_CHART3D_Z_FXY_H__
