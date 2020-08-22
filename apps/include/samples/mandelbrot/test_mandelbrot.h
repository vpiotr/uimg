#ifndef __UIMG_TEST_MANDELBR_H__
#define __UIMG_TEST_MANDELBR_H__

#include <ctime>

#include "samples/mandelbrot/mandelbrot.h"
#include "samples/test_painter_base.h"

class test_mandelbrot : public test_painter_base {
public:
    test_mandelbrot(const std::string outFileName) : test_painter_base(outFileName) {}

protected:
    virtual void paint() {
        srand(static_cast<unsigned int>(time(NULL)));
        Mandelbrot mb;
        mb.Make(getImage().getSize(), getPainter(), 20);
    }
};

#endif
