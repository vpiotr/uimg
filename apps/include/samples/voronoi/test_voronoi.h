#ifndef __UIMG_TEST_VORONOI_H__
#define __UIMG_TEST_VORONOI_H__

#include <ctime>

#include "samples/voronoi/voronoi.h"
#include "samples/test_painter_base.h"

class test_voronoi : public test_painter_base {
public:
    test_voronoi(const std::string outFileName) : test_painter_base(outFileName) {}

protected:
    virtual void paint() {
        srand(static_cast<unsigned int>(time(NULL)));
        Voronoi v;
        v.Make(getImage().getSize(), getPainter(), 50);
    }
};

#endif
