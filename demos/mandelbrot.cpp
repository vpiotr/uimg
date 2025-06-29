#include "samples/demo_painter_base.h"
#include "samples/mandelbrot/mandelbrot_demo.h"

int main(int argc, const char *argv[]) {
    auto demoInfo = demo_painter_base::get_demo_args(argc, argv, "mandelbrot");

    mandelbrot_demo demo(demoInfo.outFileName);
    demo.run();

    return 0;
}

