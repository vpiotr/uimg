#include <iostream>
#include "samples/test_painter_base.h"
#include "chart3d/test_chart_z_fxy_3d_c.h"
#include "chart3d/chart_z_fxy_3d.h"
#include "chart3d/chart3d_renderer.h"
#include "dlog/dlog.h"

int main(int argc, const char *argv[]) {
    auto demoInfo = test_painter_base::get_test_args(argc, argv, "chart3d");

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
        // Single chart - use original implementation
        test_chart_z_fxy_3d_c test(demoInfo.outFileName);
        test.setUseAntiAliasing(demoInfo.useAntiAliasing);
        test.setDrawBorders(demoInfo.drawBorders);
        test.run();
    } else {
        // For multi-chart, use the same single chart implementation but indicate it's not supported yet
        std::cerr << "Warning: Multi-chart support is not fully implemented. Using single chart." << std::endl;
        test_chart_z_fxy_3d_c test(demoInfo.outFileName);
        test.setUseAntiAliasing(demoInfo.useAntiAliasing);
        test.setDrawBorders(demoInfo.drawBorders);
        test.run();
    }
    
    return 0;
}
