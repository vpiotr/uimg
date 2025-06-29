#include <iostream>
#include <string>
#include "samples/demo_painter_base.h"
#include "samples/multi_chart3d/multi_chart_3d_demo.h"
#include "dlog/dlog.h"

void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " [options] <output_file>" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -charts <num>    Number of charts to display (1-4, default: 4)" << std::endl;
    std::cerr << "  -aa              Enable anti-aliasing" << std::endl;
    std::cerr << "  -borders         Enable chart borders" << std::endl;
    std::cerr << "  -debug           Enable debug borders (blue borders around line windows)" << std::endl;
    std::cerr << "  -dark            Enable dark mode (black background)" << std::endl;
    std::cerr << "  -layout <type>   Layout type: auto, horizontal, vertical, or NxM (e.g., 2x2)" << std::endl;
    std::cerr << "  -trace           Enable chart tracing for debug output" << std::endl;
    std::cerr << "  -log <level>     Set log level (TRACE, DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cerr << "  -help            Show this help message" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Examples:" << std::endl;
    std::cerr << "  " << programName << " output.ppm" << std::endl;
    std::cerr << "  " << programName << " -charts 2 -aa output.ppm" << std::endl;
    std::cerr << "  " << programName << " -charts 4 -borders -layout 2x2 output.ppm" << std::endl;
    std::cerr << "  " << programName << " -charts 4 -borders -debug output.ppm" << std::endl;
    std::cerr << "  " << programName << " -charts 4 -dark -aa output.ppm" << std::endl;
}

int main(int argc, const char *argv[]) {
    // Default values
    int numCharts = 4;
    bool useAntiAliasing = false;
    bool drawBorders = false;
    bool drawDebugBorders = false;
    bool darkMode = false;
    bool traceEnabled = false;
    std::string layout = "auto";
    std::string outFileName;
    dlog::LogLevel logLevel = dlog::LogLevel::INFO;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-help" || arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-charts" && i + 1 < argc) {
            numCharts = std::atoi(argv[++i]);
            if (numCharts < 1 || numCharts > 4) {
                std::cerr << "Error: Number of charts must be between 1 and 4" << std::endl;
                return 1;
            }
        } else if (arg == "-aa") {
            useAntiAliasing = true;
        } else if (arg == "-borders") {
            drawBorders = true;
        } else if (arg == "-debug") {
            drawDebugBorders = true;
        } else if (arg == "-dark") {
            darkMode = true;
        } else if (arg == "-layout" && i + 1 < argc) {
            layout = argv[++i];
        } else if (arg == "-trace") {
            traceEnabled = true;
        } else if (arg == "-log" && i + 1 < argc) {
            std::string levelStr = argv[++i];
            if (levelStr == "TRACE") logLevel = dlog::LogLevel::TRACE;
            else if (levelStr == "DEBUG") logLevel = dlog::LogLevel::DEBUG;
            else if (levelStr == "INFO") logLevel = dlog::LogLevel::INFO;
            else if (levelStr == "WARN") logLevel = dlog::LogLevel::WARN;
            else if (levelStr == "ERROR") logLevel = dlog::LogLevel::ERROR;
            else {
                std::cerr << "Error: Invalid log level: " << levelStr << std::endl;
                return 1;
            }
        } else if (arg[0] != '-') {
            // Output filename
            outFileName = arg;
        } else {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Check if output filename was provided
    if (outFileName.empty()) {
        std::cerr << "Error: Output filename is required" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    // Initialize logger
    dlog::Logger::getInstance()->setLevel(logLevel);
    
    // Display configuration
    std::cerr << "Multi-Chart 3D Demo Configuration:" << std::endl;
    std::cerr << "  Output file: " << outFileName << std::endl;
    std::cerr << "  Number of charts: " << numCharts << std::endl;
    std::cerr << "  Layout: " << layout << std::endl;
    std::cerr << "  Anti-aliasing: " << (useAntiAliasing ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "  Borders: " << (drawBorders ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "  Debug borders: " << (drawDebugBorders ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "  Dark mode: " << (darkMode ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "  Tracing: " << (traceEnabled ? "Enabled" : "Disabled") << std::endl;
    std::cerr << "  Log Level: " << dlog::Logger::levelToString(logLevel) << std::endl;
    std::cerr << std::endl;

    // Enable tracing if requested
    Chart3DTracer::getInstance()->setEnabled(traceEnabled);

    try {
        // Create and run the multi-chart demo
        multi_chart_3d_demo demo(outFileName, numCharts, useAntiAliasing, drawBorders, layout, darkMode);
        
        // Enable debug borders if requested
        if (drawDebugBorders) {
            demo.setDrawDebugBorders(true);
        }
        
        demo.run();
        
        std::cerr << "Multi-chart 3D demo completed successfully!" << std::endl;
        std::cerr << "Generated: " << outFileName << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
