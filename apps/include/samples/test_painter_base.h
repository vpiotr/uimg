#ifndef __UIMG_TESTS_PAINTER_BASE_H__
#define __UIMG_TESTS_PAINTER_BASE_H__

#include <string>
#include <memory>
#include <ostream>
#include <iostream> // For std::cout
#include <cstdlib> // For exit

#include "uimg/base/structs.h"
#include "uimg/images/rgb_image.h"
#include "uimg/painters/painter_for_rgb_image.h"
#include "uimg/images/ppm_image.h"

struct test_args {
    const std::string testName;
    const std::string outFileName;
    bool useAntiAliasing;
    int numCharts;
    std::string layout;

    test_args(std::string testName, std::string outFileName, bool useAntiAliasing = false, int numCharts = 1, std::string layout = "auto") 
        : testName(testName), outFileName(outFileName), useAntiAliasing(useAntiAliasing), numCharts(numCharts), layout(layout) {}
};

class test_painter_base {
public:
    test_painter_base(const std::string &fname) : output_fname_(fname) {}

    virtual ~test_painter_base() {}

    virtual void run() {
        init();
        paint();
        write();
    }

    static test_args get_test_args(int argc, const char *argv[]) {
        return get_test_args(argc, argv, "");
    }

    static test_args get_test_args(int argc, const char *argv[], std::string defaultTestName) {
        using namespace std;

        std::string testName(defaultTestName);
        std::string specOutFileName;
        bool useAntiAliasing = false;
        int numCharts = 1;
        std::string layout = "auto";

        for (int i = 1; i < argc; ++i) {
            if ((strcmp(argv[i], "-run") == 0) && (i + 1 < argc)) {
                testName = std::string(argv[i + 1]);
            } else if ((strcmp(argv[i], "-out") == 0) && (i + 1 < argc)) {
                specOutFileName = std::string(argv[i + 1]);
            } else if ((strcmp(argv[i], "-aa") == 0) || (strcmp(argv[i], "-antialiasing") == 0)) {
                useAntiAliasing = true;
            } else if ((strcmp(argv[i], "-charts") == 0) && (i + 1 < argc)) {
                try {
                    numCharts = std::stoi(argv[i + 1]);
                    if (numCharts < 1) numCharts = 1;
                    if (numCharts > 4) numCharts = 4;
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing number of charts, using default: " << e.what() << std::endl;
                }
            } else if ((strcmp(argv[i], "-layout") == 0) && (i + 1 < argc)) {
                layout = std::string(argv[i + 1]);
            } else if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "-help") == 0) || (strcmp(argv[i], "--help") == 0)) {
                // Display help information
                std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
                std::cout << "Options:" << std::endl;
                std::cout << "  -run <test>         : Run specific test" << std::endl;
                std::cout << "  -out <file>         : Set output file name" << std::endl;
                std::cout << "  -aa, -antialiasing  : Enable anti-aliased line rendering" << std::endl;
                std::cout << "  -charts <num>       : Number of charts to display (1-4, default: 1)" << std::endl;
                std::cout << "  -layout <type>      : Layout type (auto, grid, default: auto)" << std::endl;
                std::cout << "  -h, -help, --help   : Display this help information" << std::endl;
                exit(0);
            }
        }

        std::string outFileName(testName + ".ppm");
        if (!specOutFileName.empty()) {
            outFileName = specOutFileName;
        }

        return {testName, outFileName, useAntiAliasing, numCharts, layout};
    }

protected:
    virtual std::string get_output_fname() {
        if (!output_fname_.empty())
            return output_fname_;
        else
            return "output.ppm";
    }

    virtual Point getImageSize() {
        return {512, 512};
    }

    virtual void init() {
        Point size = getImageSize();
        image_.reset(new RgbImage(size.x, size.y));
        painter_.reset(new PixelPainterForRgbImage(*image_));
    }

    virtual void write() {
        using namespace std;
        std::ofstream output(get_output_fname(), ios::out | ios::binary);

        PpmWriterForRgbImage writer(output);

        writer.writeImage(*image_);

        output.close();
    }

    virtual void paint() = 0;

    RgbImage &getImage() {
        return *image_;
    }

    PixelPainter &getPainter() {
        return *painter_;
    }

private:
    std::unique_ptr<RgbImage> image_;
    std::unique_ptr<PixelPainter> painter_;
    std::string output_fname_;
};

#endif
