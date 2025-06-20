#ifndef __UIMG_DEMOS_PAINTER_BASE_H__
#define __UIMG_DEMOS_PAINTER_BASE_H__

#include <string>
#include <memory>
#include <ostream>
#include <iostream> // For std::cout
#include <cstdlib> // For exit

#include "uimg/base/structs.h"
#include "uimg/images/rgb_image.h"
#include "uimg/painters/painter_for_rgb_image.h"
#include "uimg/images/ppm_image.h"
#include "uimg/utils/cast.h"
#include "dlog/dlog.h"

struct demo_args {
    const std::string demoName;
    const std::string outFileName;
    bool useAntiAliasing;
    int numCharts;
    std::string layout;
    bool drawBorders;
    bool traceEnabled;
    dlog::LogLevel logLevel;

    std::string fontPath;
    
    demo_args(std::string demo_name, std::string out_file_name, bool use_anti_aliasing = false, int num_charts = 1, std::string chart_layout = "auto", bool draw_borders = false, bool trace_enabled = false, dlog::LogLevel log_level = dlog::LogLevel::INFO) 
        : demoName(demo_name), outFileName(out_file_name), useAntiAliasing(use_anti_aliasing), numCharts(num_charts), layout(chart_layout), drawBorders(draw_borders), traceEnabled(trace_enabled), logLevel(log_level) {}
};

class demo_painter_base {
public:
    demo_painter_base(const std::string &fname) : output_fname_(fname) {}

    virtual ~demo_painter_base() {}

    virtual void run() {
        init();
        paint();
        write();
    }

    static demo_args get_demo_args(int argc, const char *argv[]) {
        return get_demo_args(argc, argv, "");
    }

    static demo_args get_demo_args(int argc, const char *argv[], std::string defaultDemoName) {
        using namespace std;

        std::string demoName(defaultDemoName);
        bool useAntiAliasing = false;
        int numCharts = 1;
        std::string chartLayout = "auto";
        bool drawBorders = false;
        bool traceEnabled = false;
        dlog::LogLevel logLevel = dlog::LogLevel::INFO;
        std::string fontPath = "";

        std::string outFileName("output.ppm");

        if (argc > 1) {
            for (int i = 1; i < argc; ++i) {
                std::string current_arg(argv[i]);
                if (current_arg == "--aa" || current_arg == "-a") {
                    useAntiAliasing = true;
                    continue;
                }

                if (current_arg == "--trace" || current_arg == "-t") {
                    traceEnabled = true;
                    continue;
                }

                if (current_arg == "--debug" || current_arg == "-d") {
                    logLevel = dlog::LogLevel::DEBUG;
                    continue;
                }

                if (current_arg == "--verbose" || current_arg == "-v") {
                    logLevel = dlog::LogLevel::DEBUG; // Changed from VERBOSE to DEBUG since VERBOSE doesn't exist
                    continue;
                }

                if (current_arg == "--borders" || current_arg == "-b") {
                    drawBorders = true;
                    continue;
                }

                if ((current_arg == "--charts" || current_arg == "-c") && i < argc - 1) {
                    numCharts = atoi(argv[++i]);
                    continue;
                }

                if ((current_arg == "--layout" || current_arg == "-l") && i < argc - 1) {
                    chartLayout = argv[++i];
                    continue;
                }

                if ((current_arg == "--font" || current_arg == "-f") && i < argc - 1) {
                    fontPath = argv[++i];
                    continue;
                }

                if (current_arg == "--name" && i < argc - 1) {
                    demoName = argv[++i];
                    continue;
                }

                if (current_arg.find("--") == 0) {
                    // Ignore unknown args
                    continue;
                }

                // This is considered to be the output file name
                outFileName = current_arg;
            }
        }

        demo_args args(demoName, outFileName, useAntiAliasing, numCharts, chartLayout, drawBorders, traceEnabled, logLevel);
        args.fontPath = fontPath;
        return args;
    }

protected:
    virtual void init() {
        const unsigned width = 640;
        const unsigned height = 480;

        img_ = std::make_shared<RgbImage>(width, height);
    }

    virtual void paint() = 0;

    virtual void write() {
        std::ofstream outFile(output_fname_, std::ios::binary);
        PpmWriterForRgbImage writer(outFile);
        writer.writeImage(*img_);
    }

    RgbImage &getImage() {
        return *img_.get();
    }

    const std::string &getOutputFileName() const {
        return output_fname_;
    }

private:
    std::string output_fname_;
    std::shared_ptr<RgbImage> img_;
};

#endif
