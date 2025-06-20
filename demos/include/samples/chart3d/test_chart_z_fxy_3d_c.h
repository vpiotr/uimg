#ifndef __UIMG_TESTS_CHART_Z_FXY_3DC_H__
#define __UIMG_TESTS_CHART_Z_FXY_3DC_H__

#include <algorithm> 
#include <cmath>
#include <vector>
#include <utility>
#include <iostream>

#include "samples/test_painter_base.h"
#include "samples/chart3d/chart_z_fxy_3d.h"
#include "dlog/dlog.h"
#include "uimg/filters/pixel_tracing_filter.h"

class test_chart_z_fxy_3d_c : public test_painter_base {
public:
    test_chart_z_fxy_3d_c(const std::string outFileName) : test_painter_base(outFileName) {}
    
    bool useAntiAliasing() const { return useAntiAliasing_; }
    void setUseAntiAliasing(bool value) { useAntiAliasing_ = value; }
    
    bool drawBorders() const { return drawBorders_; }
    void setDrawBorders(bool value) { drawBorders_ = value; }

    void setTraceEnabled(bool value) {
       Chart3DTracer::getInstance()->setEnabled(value); 
    }

protected:
    virtual void paint() {

        class chart_z_fxy_3d_c: public chart_z_fxy_3d {
        public:
            chart_z_fxy_3d_c(const Point &canvasSize, PixelPainter &painter, bool useAntiAliasing, bool drawBorders = false): 
                chart_z_fxy_3d(canvasSize, painter, useAntiAliasing, drawBorders) {}
        protected:
            virtual RgbColor getPlotColor(double x, double y, double z) {
                RgbColor color;
                color.red = static_cast<unsigned char>(std::max(70, std::min(255, 200+(int)floor(z * 200))));
                color.green = static_cast<unsigned char>(std::min(255, 50+(int)math_utils::iabs(200.0*x/2520)));
                color.blue = static_cast<unsigned char>(std::min(255, 50+(int)math_utils::iabs(200.0*y/2520)));
                return color;
            }

            // returns function value
            virtual double getFunValue(double x, double y) {
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

        // Create pixel tracing filter for the single chart
        PixelTracingFilter tracingFilter(getPainter(), "Sinc Chart (Single)");
        
        chart_z_fxy_3d_c chart1(getImage().getSize(), tracingFilter, useAntiAliasing_, drawBorders_);
        
        auto logger = dlog::Logger::getInstance();
        logger->debug("=== Single Chart 3D Test Debug Information ===");
        logger->debug("Test image size: {0}x{1}", getImageSize().x, getImageSize().y);
        logger->debug("Test screen offset: ({0}, {1})", getScreenOffset().x, getScreenOffset().y);

        chart1.paint();
        
        // Log pixel range after chart drawing is complete
        tracingFilter.logPixelRange();
        
        // Print status message about anti-aliasing and borders
        if (useAntiAliasing_) {
            std::cout << "Rendering chart with anti-aliasing enabled." << std::endl;
        } else {
            std::cout << "Rendering chart with anti-aliasing disabled." << std::endl;
        }
        
        if (drawBorders_) {
            std::cout << "Rendering chart with borders enabled." << std::endl;
        } else {
            std::cout << "Rendering chart with borders disabled." << std::endl;
        }
    }

    virtual Point getImageSize() {
        return{ 512, 512};
    }

    virtual Point getScreenOffset() {
        return{ 10, 100 };
    }

private:
    bool useAntiAliasing_ = false;
    bool drawBorders_ = false;
};

#endif
    
