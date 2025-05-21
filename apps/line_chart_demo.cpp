#include <iostream>
#include <string>
#include <stdexcept> // Required for std::runtime_error

#include "samples/line_chart_demo/test_line_chart_demo.h"

// Assuming line_chart_demo_args is within a namespace or defined elsewhere.
// For now, let's assume it's in the global namespace or included via another header in test_line_chart_demo.h
// If it's in a namespace like 'uimg', it would be uimg::line_chart_demo_args
struct line_chart_demo_args {
    std::string fontPath;
    std::string outFileName = "line_chart_demo_output.ppm"; // Default output file
    float lineThickness = 1.0f; // Default line thickness

    static line_chart_demo_args parse(int argc, const char* argv[]) {
        line_chart_demo_args args;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-font" && i + 1 < argc) {
                args.fontPath = argv[++i];
            } else if (arg == "-out" && i + 1 < argc) {
                args.outFileName = argv[++i];
            } else if (arg == "-thickness" && i + 1 < argc) {
                try {
                    args.lineThickness = std::stof(argv[++i]);
                    // Ensure the thickness is at least 1.0
                    if (args.lineThickness < 1.0f) args.lineThickness = 1.0f;
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing line thickness, using default: " << e.what() << std::endl;
                }
            }
        }
        return args;
    }
};


int main(int argc, char *argv[]) {
    // Print usage information
    if (argc < 3) {
        std::cout << "Usage: line_chart_demo -font <path/to/font.bdf> [-out <output_file.ppm>] [-thickness <line_thickness>]" << std::endl;
        std::cout << "Example: line_chart_demo -font ../fonts/courR12.bdf -out line_chart_output.ppm -thickness 2.5" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -font <path>       : Path to the BDF font file (required)" << std::endl;
        std::cout << "  -out <file.ppm>    : Output file path (default: line_chart_demo_output.ppm)" << std::endl; 
        std::cout << "  -thickness <value> : Line thickness (default: 1.0)" << std::endl;
        return 1;
    }

    // Parse command line arguments
    auto args = line_chart_demo_args::parse(argc, const_cast<const char**>(argv));

    if (args.fontPath.empty()) {
        std::cerr << "Error: Font path is mandatory." << std::endl;
        std::cout << "Usage: line_chart_demo -font <path/to/font.bdf> [-out <output_file.ppm>] [-thickness <line_thickness>]" << std::endl;
        return 1;
    }

    try {
        // Create and run the line chart demo with specified line thickness
        LineChartDemo demo(800, 600, args.fontPath, args.lineThickness);
        demo.generateChart(args.outFileName);
        std::cout << "Line chart demo image created: " << args.outFileName << std::endl;
        std::cout << "Line thickness: " << args.lineThickness << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
