#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>

#include "uimg/charts/chart.h"
#include "uimg/charts/chart_renderer.h"

using namespace uimg::charts;

struct enhanced_chart_demo_args {
    std::string fontPath;
    std::string outFileName = "enhanced_chart_demo_output.ppm";
    float lineThickness = 2.0f;
    int numCharts = 2;         // Number of charts to display
    bool useDarkTheme = false; // Use dark theme for charts
    
    static enhanced_chart_demo_args parse(int argc, const char* argv[]) {
        enhanced_chart_demo_args args;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-font" && i + 1 < argc) {
                args.fontPath = argv[++i];
            } else if (arg == "-out" && i + 1 < argc) {
                args.outFileName = argv[++i];
            } else if (arg == "-thickness" && i + 1 < argc) {
                try {
                    args.lineThickness = std::stof(argv[++i]);
                    if (args.lineThickness < 1.0f) args.lineThickness = 1.0f;
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing line thickness, using default: " << e.what() << std::endl;
                }
            } else if (arg == "-charts" && i + 1 < argc) {
                try {
                    args.numCharts = std::stoi(argv[++i]);
                    if (args.numCharts < 1) args.numCharts = 1;
                    if (args.numCharts > 4) args.numCharts = 4;
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing number of charts, using default: " << e.what() << std::endl;
                }
            } else if (arg == "-dark") {
                args.useDarkTheme = true;
            }
        }
        return args;
    }
};

int main(int argc, char *argv[]) {
    // Print usage information
    if (argc < 3) {
        std::cout << "Usage: enhanced_chart_demo -font <path/to/font.bdf> [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -font <path>       : Path to the BDF font file (required)" << std::endl;
        std::cout << "  -out <file.ppm>    : Output file path (default: enhanced_chart_demo_output.ppm)" << std::endl; 
        std::cout << "  -thickness <value> : Line thickness (default: 2.0)" << std::endl;
        std::cout << "  -charts <num>      : Number of charts to display (1-4, default: 2)" << std::endl;
        std::cout << "  -dark              : Use dark theme for charts" << std::endl;
        std::cout << "Example: enhanced_chart_demo -font ../fonts/courR12.bdf -charts 4 -dark" << std::endl;
        return 1;
    }

    // Parse command line arguments
    auto args = enhanced_chart_demo_args::parse(argc, const_cast<const char**>(argv));

    if (args.fontPath.empty()) {
        std::cerr << "Error: Font path is mandatory." << std::endl;
        std::cout << "Usage: enhanced_chart_demo -font <path/to/font.bdf> [options]" << std::endl;
        return 1;
    }

    try {
        // Create renderer with 800x600 canvas
        ChartRenderer renderer(800, 600, args.fontPath);
        
        // Choose style based on theme option
        ChartStyle chartStyle = args.useDarkTheme ? 
            ChartStyle::createDarkTheme() : ChartStyle::createDefault();
            
        // If using dark theme, set the background color of the entire canvas
        if (args.useDarkTheme) {
            renderer.setBackgroundColor(chartStyle.backgroundColor);
        }
        
        // Create different charts based on the user's selection
        
        // Chart 1: Quadratic function (y = x^2)
        Chart quadraticChart("Quadratic Function", 
                            AxisConfig::create(-10.0f, 10.0f, "X-axis"),
                            AxisConfig::create(0.0f, 100.0f, "Y-axis"),
                            chartStyle);
        
        // Create and configure a series
        Series& quadSeries = quadraticChart.createSeries("y = x^2", RgbColor{255, 0, 0}, args.lineThickness);
        quadSeries.generatePoints(-10.0f, 10.0f, 0.5f, [](float x) { return x * x; });
        
        // Chart 2: Trigonometric functions
        Chart trigChart("Trigonometric Functions",
                      AxisConfig::create(0.0f, 6.28f, "Angle (radians)"),
                      AxisConfig::create(-1.0f, 1.0f, "Value"),
                      chartStyle);
        
        // Create and configure series for sine and cosine
        Series& sinSeries = trigChart.createSeries("y = sin(x)", RgbColor{0, 0, 255}, args.lineThickness);
        sinSeries.generatePoints(0.0f, 6.28f, 0.1f, [](float x) { return std::sin(x); });
        
        Series& cosSeries = trigChart.createSeries("y = cos(x)", RgbColor{0, 128, 0}, args.lineThickness);
        cosSeries.generatePoints(0.0f, 6.28f, 0.1f, [](float x) { return std::cos(x); });
        
        // Chart 3: Polynomial (y = x^3)
        Chart polyChart("Cubic Function",
                       AxisConfig::create(-5.0f, 5.0f, "X-axis"),
                       AxisConfig::create(-125.0f, 125.0f, "Y-axis"),
                       chartStyle);
        
        Series& cubicSeries = polyChart.createSeries("y = x^3", RgbColor{255, 128, 0}, args.lineThickness);
        cubicSeries.generatePoints(-5.0f, 5.0f, 0.2f, [](float x) { return x * x * x; });
        
        // Chart 4: Exponential function (y = e^x)
        Chart expChart("Exponential Function",
                      AxisConfig::create(-2.0f, 2.0f, "X-axis"),
                      AxisConfig::create(0.0f, 8.0f, "Y-axis"),
                      chartStyle);
        
        Series& expSeries = expChart.createSeries("y = e^x", RgbColor{128, 0, 128}, args.lineThickness);
        expSeries.generatePoints(-2.0f, 2.0f, 0.1f, [](float x) { return std::exp(x); });
        
        // Add charts to the renderer based on user selection
        switch(args.numCharts) {
            case 1:
                renderer.addChart(quadraticChart, ChartLayout::createAuto());
                break;
            case 2:
                renderer.addChart(quadraticChart, ChartLayout::createAuto());
                renderer.addChart(trigChart, ChartLayout::createAuto());
                break;
            case 3:
                renderer.addChart(quadraticChart, ChartLayout::createAuto());
                renderer.addChart(trigChart, ChartLayout::createAuto());
                renderer.addChart(polyChart, ChartLayout::createAuto());
                break;
            case 4:
                renderer.addChart(quadraticChart, ChartLayout::createAuto());
                renderer.addChart(trigChart, ChartLayout::createAuto());
                renderer.addChart(polyChart, ChartLayout::createAuto());
                renderer.addChart(expChart, ChartLayout::createAuto());
                break;
        }
        
        // Render all charts and save to file
        renderer.renderToFile(args.outFileName);
        
        std::cout << "Enhanced chart demo image created: " << args.outFileName << std::endl;
        std::cout << "Number of charts: " << args.numCharts << std::endl;
        std::cout << "Line thickness: " << args.lineThickness << std::endl;
        std::cout << "Theme: " << (args.useDarkTheme ? "Dark" : "Light") << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
