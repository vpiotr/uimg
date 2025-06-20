#include <iostream>
#include "samples/test_painter_base.h"
#include "chart3d/test_chart_z_fxy_3d_c.h"
#include "chart3d/chart_z_fxy_3d.h"
#include "chart3d/chart3d_renderer.h"
#include "dlog/dlog.h"

int main(int argc, const char *argv[]) {
    auto testInfo = test_painter_base::get_test_args(argc, argv, "chart3d");

    // Initialize logger
    dlog::Logger::getInstance()->setLevel(testInfo.logLevel);
    
    // Display configuration
    std::cerr << "Anti-aliasing: " << (testInfo.useAntiAliasing ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "Number of charts: " << testInfo.numCharts << std::endl;
    std::cerr << "Layout: " << testInfo.layout << std::endl;
    std::cerr << "Borders: " << (testInfo.drawBorders ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "Log Level: " << dlog::Logger::levelToString(testInfo.logLevel) << std::endl;

    Chart3DTracer::getInstance()->setEnabled(testInfo.traceEnabled);
    std::cerr << "main - Tracing: " << (testInfo.traceEnabled ? "Enabled" : "Disabled") << std::endl;

    if (testInfo.numCharts == 1) {
        // Single chart - use original implementation
        test_chart_z_fxy_3d_c test(testInfo.outFileName);
        test.setUseAntiAliasing(testInfo.useAntiAliasing);
        test.setDrawBorders(testInfo.drawBorders);
        test.run();
    } else {
        // Multi-chart implementation
        class multi_chart_3d_test : public test_painter_base {
        public:
            multi_chart_3d_test(const std::string &fname, int numCharts, bool useAntiAliasing, bool drawBorders) 
                : test_painter_base(fname), numCharts_(numCharts), useAntiAliasing_(useAntiAliasing), drawBorders_(drawBorders) {}

        protected:
            virtual Point getImageSize() override {
                // Use larger canvas for multiple charts
                switch(numCharts_) {
                    case 2: return {1024, 512};  // Wide layout for 2 charts
                    case 3: return {1024, 768};  // Larger for 3 charts
                    case 4: return {1024, 1024}; // Square for 2x2 grid
                    default: return {512, 512};  // Default for single chart
                }
            }

            virtual void paint() override {
                Chart3DRenderer renderer(getImage(), getPainter(), useAntiAliasing_, drawBorders_);
                renderer.renderCharts(numCharts_);
                
                std::cout << "Rendered " << numCharts_ << " 3D charts with anti-aliasing " 
                          << (useAntiAliasing_ ? "enabled" : "disabled") 
                          << " and borders " << (drawBorders_ ? "enabled" : "disabled") << std::endl;
            }

        private:
            int numCharts_;
            bool useAntiAliasing_;
            bool drawBorders_;
        };

        multi_chart_3d_test test(testInfo.outFileName, testInfo.numCharts, testInfo.useAntiAliasing, testInfo.drawBorders);
        test.run();
    }

    return 0;
}

