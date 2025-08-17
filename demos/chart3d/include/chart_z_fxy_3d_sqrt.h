#ifndef __UIMG_CHART_Z_FXY_3D_SQRT_H__
#define __UIMG_CHART_Z_FXY_3D_SQRT_H__

#include <algorithm>
#include <cmath>
#include "chart3d/chart_z_fxy_3d.h"
#include "uimg/utils/cast.h"
#include "uimg/utils/math_utils.h"

/**
 * @brief Sinc function 3D chart implementation
 * Renders the sinc function: sin(r)/r where r = sqrt(x² + y²)
 */
class chart_z_fxy_3d_sqrt : public chart_z_fxy_3d {
public:
    chart_z_fxy_3d_sqrt(const Point &canvasSize, PixelPainter &painter, bool useAntiAliasing, bool drawBorders = false): 
        chart_z_fxy_3d(canvasSize, painter, useAntiAliasing, drawBorders) {}
        
protected:
    virtual RgbColor getPlotColor(double x, double y, double z) override {
        RgbColor color;
        color.red = UNSIGNED_CAST(unsigned char, std::max(70, std::min(255, 200+(int)floor(z * 200))));
        color.green = UNSIGNED_CAST(unsigned char, std::min(255, 50+(int)math_utils::iabs(200.0*x/2520)));
        color.blue = UNSIGNED_CAST(unsigned char, std::min(255, 50+(int)math_utils::iabs(200.0*y/2520)));
        return color;
    }

    // returns function value
    virtual double getFunValue(double x, double y) override {
        static const double degreesToRadiansFactor = math_utils::pi_const_d() / 180;
        double r = sqrt(x * x + y * y) * degreesToRadiansFactor;
        double result;
        
        if (r == 0.0)
            result = 1.0;
        else
            result = sin(r) / r;

        return result;
    }

    virtual double getCenterZ() const override {
        return 1.0;  // Sinc function value at origin: sin(0)/0 = 1.0
    }
};

#endif // __UIMG_CHART_Z_FXY_3D_SQRT_H__
