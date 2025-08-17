#include <iostream>
#include <algorithm> 
#include <cmath>
#include <vector>
#include <utility>
#include "../common/include/demo_painter_base.h"
#include "../common/include/chart3d/chart_z_fxy_3d.h"
#include "include/chart_z_fxy_3d_sqrt.h"
#include "dlog/dlog.h"
#include "uimg/filters/pixel_tracing_filter.h"
#include "uimg/utils/cast.h"

class chart3d_demo : public demo_painter_base {
public:
    chart3d_demo(const std::string outFileName) : demo_painter_base(outFileName) {}
    
    bool useAntiAliasing() const { return useAntiAliasing_; }
    void setUseAntiAliasing(bool value) { useAntiAliasing_ = value; }
    
    bool drawBorders() const { return drawBorders_; }
    void setDrawBorders(bool value) { drawBorders_ = value; }

    void setTraceEnabled(bool value) {
       Chart3DTracer::getInstance()->setEnabled(value); 
    }

protected:
    virtual void paint() {
        // Create pixel tracing filter for the single chart
        PixelTracingFilter tracingFilter(getPainter(), "Sinc Chart (Single)");
        
        chart_z_fxy_3d_sqrt chart1(getImage().getSize(), tracingFilter, useAntiAliasing_, drawBorders_);
        
        auto logger = dlog::Logger::getInstance();
        logger->debug("=== Single Chart 3D Demo Debug Information ===");
        logger->debug("Demo image size: {0}x{1}", getImageSize().x, getImageSize().y);
        logger->debug("Demo screen offset: ({0}, {1})", getScreenOffset().x, getScreenOffset().y);

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

int main(int argc, const char *argv[]) {
    auto demoInfo = demo_painter_base::get_demo_args(argc, argv, "chart3d");

    // Initialize logger
    dlog::Logger::getInstance()->setLevel(demoInfo.logLevel);
    
    // Display configuration
    std::cerr << "Anti-aliasing: " << (demoInfo.useAntiAliasing ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "Number of charts: " << demoInfo.numCharts << std::endl;
    std::cerr << "Layout: " << demoInfo.layout << std::endl;
    std::cerr << "Borders: " << (demoInfo.drawBorders ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "Log Level: " << dlog::Logger::levelToString(demoInfo.logLevel) << std::endl;

    Chart3DTracer::getInstance()->setEnabled(demoInfo.traceEnabled);
    std::cerr << "main - Tracing: " << (demoInfo.traceEnabled ? "Enabled" : "Disabled") << std::endl;

    if (demoInfo.numCharts == 1) {
        // Single chart - use demo implementation
        chart3d_demo demo(demoInfo.outFileName);
        demo.setUseAntiAliasing(demoInfo.useAntiAliasing);
        demo.setDrawBorders(demoInfo.drawBorders);
        demo.run();
    } else {
        // chart3d demo only supports single charts - use multi_chart3d_demo for multiple charts
        std::cerr << "Error: chart3d_demo only supports single charts (numCharts=1)." << std::endl;
        std::cerr << "For multiple charts, use multi_chart3d_demo instead." << std::endl;
        return 1;
    }
    
    return 0;
}
