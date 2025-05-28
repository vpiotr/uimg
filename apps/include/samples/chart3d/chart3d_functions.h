#ifndef __UIMG_CHART3D_FUNCTIONS_H__
#define __UIMG_CHART3D_FUNCTIONS_H__

#include <cmath>
#include "uimg/utils/math_utils.h"

namespace chart3d_functions {
    
    /**
     * @brief Sinc function: sin(r)/r where r = sqrt(x² + y²)
     */
    inline double sinc(double x, double y) {
        static const double degreesToRadiansFactor = math_utils::pi_const_d() / 180;
        double r = sqrt(x * x + y * y) * degreesToRadiansFactor;
        if (r == 0.0)
            return 1.0;
        else
            return sin(r) / r;
    }
    
    /**
     * @brief Gaussian function: exp(-(x² + y²)/σ²)
     */
    inline double gaussian(double x, double y) {
        double sigma = 1500.0; // Adjust for better visualization
        double r_squared = (x * x + y * y) / (sigma * sigma);
        return exp(-r_squared);
    }
    
    /**
     * @brief Ripple function: cos(sqrt(x² + y²))
     */
    inline double ripple(double x, double y) {
        static const double scaleFactor = math_utils::pi_const_d() / 1000.0;
        double r = sqrt(x * x + y * y) * scaleFactor;
        return cos(r) * exp(-r * 0.2); // Add decay for better visualization
    }
    
    /**
     * @brief Saddle function: x² - y²
     */
    inline double saddle(double x, double y) {
        static const double scaleFactor = 1.0 / (2520.0 * 2520.0);
        return (x * x - y * y) * scaleFactor;
    }
    
} // namespace chart3d_functions

#endif // __UIMG_CHART3D_FUNCTIONS_H__
