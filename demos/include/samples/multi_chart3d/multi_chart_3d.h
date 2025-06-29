#ifndef __UIMG_MULTI_CHART_3D_H__
#define __UIMG_MULTI_CHART_3D_H__

#include "chart3d/chart_z_fxy_3d.h"
#include "chart3d/chart3d_functions.h"
#include "uimg/utils/cast.h"
#include <string>

/**
 * @brief Base class for different 3D chart types with titles
 */
class chart_z_fxy_3d_with_title : public chart_z_fxy_3d {
public:
    chart_z_fxy_3d_with_title(const Point &canvasSize, PixelPainter &pixelPainter, bool useAntiAliasing, const std::string& title, bool drawBorders = false) 
        : chart_z_fxy_3d(canvasSize, pixelPainter, useAntiAliasing, drawBorders), title_(title) {}
    
    const std::string& getTitle() const { return title_; }

protected:
    std::string title_;
};

/**
 * @brief Sinc function 3D chart
 */
class chart_z_fxy_3d_sinc : public chart_z_fxy_3d_with_title {
public:
    chart_z_fxy_3d_sinc(const Point &canvasSize, PixelPainter &painter, bool useAntiAliasing, bool drawBorders = false): 
        chart_z_fxy_3d_with_title(canvasSize, painter, useAntiAliasing, "Sinc Function: sin(r)/r", drawBorders) {}

protected:
    virtual RgbColor getPlotColor(double x, double y, double z) override {
        RgbColor color;
        color.red = UNSIGNED_CAST(unsigned char, std::max(70, std::min(255, 200+(int)floor(z * 200))));
        color.green = UNSIGNED_CAST(unsigned char, std::min(255, 50+(int)math_utils::iabs(200.0*x/2520)));
        color.blue = UNSIGNED_CAST(unsigned char, std::min(255, 50+(int)math_utils::iabs(200.0*y/2520)));
        return color;
    }

    virtual double getFunValue(double x, double y) override {
        return chart3d_functions::sinc(x, y);
    }

    virtual double getCenterZ() const override {
        return 1.0;  // Sinc function value at origin: sin(0)/0 = 1.0
    }
};

/**
 * @brief Gaussian function 3D chart
 */
class chart_z_fxy_3d_gaussian : public chart_z_fxy_3d_with_title {
public:
    chart_z_fxy_3d_gaussian(const Point &canvasSize, PixelPainter &painter, bool useAntiAliasing, bool drawBorders = false): 
        chart_z_fxy_3d_with_title(canvasSize, painter, useAntiAliasing, "Gaussian Function: e^(-r²)", drawBorders) {}

protected:
    virtual RgbColor getPlotColor(double /*x*/, double /*y*/, double z) override {
        RgbColor color;
        // Use a blue-to-red gradient for Gaussian
        int intensity = std::max(0, std::min(255, (int)(z * 255)));
        color.red = UNSIGNED_CAST(unsigned char, intensity);
        color.green = UNSIGNED_CAST(unsigned char, intensity / 2);
        color.blue = UNSIGNED_CAST(unsigned char, 255 - intensity);
        return color;
    }

    virtual double getFunValue(double x, double y) override {
        return chart3d_functions::gaussian(x, y);
    }

    virtual double getCenterZ() const override {
        return 1.0;  // Gaussian function value at origin: e^(-0²) = 1.0
    }
};

/**
 * @brief Ripple function 3D chart
 */
class chart_z_fxy_3d_ripple : public chart_z_fxy_3d_with_title {
public:
    chart_z_fxy_3d_ripple(const Point &canvasSize, PixelPainter &painter, bool useAntiAliasing, bool drawBorders = false): 
        chart_z_fxy_3d_with_title(canvasSize, painter, useAntiAliasing, "Ripple Function: cos(r)·e^(-0.2r)", drawBorders) {}

protected:
    virtual RgbColor getPlotColor(double /*x*/, double /*y*/, double z) override {
        RgbColor color;
        // Use a green-based color scheme for ripples
        int intensity = std::max(0, std::min(255, (int)((z + 1) * 127.5))); // z ranges from -1 to 1
        color.red = UNSIGNED_CAST(unsigned char, intensity / 3);
        color.green = UNSIGNED_CAST(unsigned char, intensity);
        color.blue = UNSIGNED_CAST(unsigned char, intensity / 2);
        return color;
    }

    virtual double getFunValue(double x, double y) override {
        return chart3d_functions::ripple(x, y);
    }

    virtual double getCenterZ() const override {
        return 1.0;  // Ripple function value at origin: cos(0)·e^(-0.2·0) = 1.0
    }
};

/**
 * @brief Saddle function 3D chart
 */
class chart_z_fxy_3d_saddle : public chart_z_fxy_3d_with_title {
public:
    chart_z_fxy_3d_saddle(const Point &canvasSize, PixelPainter &painter, bool useAntiAliasing, bool drawBorders = false): 
        chart_z_fxy_3d_with_title(canvasSize, painter, useAntiAliasing, "Saddle Function: x² - y²", drawBorders) {}

protected:
    virtual RgbColor getPlotColor(double /*x*/, double /*y*/, double z) override {
        RgbColor color;
        // Use a purple-orange gradient for saddle
        if (z >= 0) {
            int intensity = std::max(0, std::min(255, (int)(z * 255 * 10))); // Scale for visibility
            color.red = 255;
            color.green = UNSIGNED_CAST(unsigned char, 165 - intensity/2);
            color.blue = 0;
        } else {
            int intensity = std::max(0, std::min(255, (int)(-z * 255 * 10)));
            color.red = UNSIGNED_CAST(unsigned char, 128 - intensity/2);
            color.green = 0;
            color.blue = UNSIGNED_CAST(unsigned char, 128 + intensity/2);
        }
        return color;
    }

    virtual double getFunValue(double x, double y) override {
        return chart3d_functions::saddle(x, y);
    }

    virtual double getCenterZ() const override {
        return 0.0;  // Saddle function value at origin: 0² - 0² = 0.0
    }
};

#endif // __UIMG_MULTI_CHART_3D_H__
