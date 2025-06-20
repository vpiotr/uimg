#include "samples/test_painter_base.h"
#include "samples/mandelbrot/test_mandelbrot.h"

int main(int argc, const char *argv[]) {
    auto testInfo = test_painter_base::get_test_args(argc, argv, "mandelbrot");

    test_mandelbrot test(testInfo.outFileName);
    test.run();

    return 0;
}

