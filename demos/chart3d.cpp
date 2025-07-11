#include <iostream>
#include "samples/demo_painter_base.h"
#include "samples/chart3d/chart3d_demo.h"
#include "samples/multi_chart3d/multi_chart_3d_demo.h"
#include "chart3d/chart_z_fxy_3d.h"
#include "chart3d/chart3d_renderer.h"
#include "dlog/dlog.h"

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
        // Multi-chart support
        multi_chart_3d_demo demo(demoInfo.outFileName, demoInfo.numCharts, 
                               demoInfo.useAntiAliasing, demoInfo.drawBorders, demoInfo.layout);
        demo.run();
    }
    
    return 0;
}
