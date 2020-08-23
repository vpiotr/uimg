#ifndef __UIMG_TESTS_CHART_Z_FXY_3DC_H__
#define __UIMG_TESTS_CHART_Z_FXY_3DC_H__

#include <algorithm> 
#include <cmath>
#include <vector>
#include <utility>

#include "samples/test_painter_base.h"
#include "samples/chart3d/chart_z_fxy_3d.h"

class test_chart_z_fxy_3d_c : public test_painter_base {
public:
    test_chart_z_fxy_3d_c(const std::string outFileName) : test_painter_base(outFileName) {}

protected:
    virtual void paint() {

        class chart_z_fxy_3d_c: public chart_z_fxy_3d {
        public:
            chart_z_fxy_3d_c(const Point &canvasSize, PixelPainter &painter): chart_z_fxy_3d(canvasSize, painter) {}
        protected:
            virtual RgbColor getPlotColor(double x, double y, double z) {
                RgbColor color;
                color.red = std::max(70, std::min(255, 200+(int)floor(z * 200)));
                color.green = std::min(255, 50+(int)math_utils::iabs(200.0*x/2520));
                color.blue = std::min(255, 50+(int)math_utils::iabs(200.0*y/2520));
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

        };

        chart_z_fxy_3d_c chart1(getImage().getSize(), getPainter());
        chart1.paint();
    }

    virtual Point getImageSize() {
        return{ 512, 512};
    }

    virtual Point getScreenOffset() {
        return{ 10, 100 };
    }

};

#endif
    
