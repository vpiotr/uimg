#ifndef __UIMG_TESTS_PAINTER_BASE_H__
#define __UIMG_TESTS_PAINTER_BASE_H__

#include <string>
#include <memory>
#include <ostream>

#include "uimg/base/structs.h"
#include "uimg/images/rgb_image.h"
#include "uimg/painters/painter_for_rgb_image.h"
#include "uimg/images/ppm_image.h"

struct test_args {
    const std::string testName;
    const std::string outFileName;

    test_args(std::string testName, std::string outFileName) : testName(testName), outFileName(outFileName) {}
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

        for (int i = 1; i < argc; ++i) {
            if ((strcmp(argv[i], "-run") == 0) && (i + 1 < argc)) {
                testName = std::string(argv[i + 1]);
            } else if ((strcmp(argv[i], "-out") == 0) && (i + 1 < argc)) {
                specOutFileName = std::string(argv[i + 1]);
            }
        }

        std::string outFileName(testName + ".ppm");
        if (!specOutFileName.empty()) {
            outFileName = specOutFileName;
        }

        return {testName, outFileName};
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
