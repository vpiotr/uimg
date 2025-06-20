#include "samples/test_painter_base.h"
#include "samples/voronoi/test_voronoi.h"

int main(int argc, const char *argv[]) {
    auto testInfo = test_painter_base::get_test_args(argc, argv, "voronoi");

    test_voronoi test(testInfo.outFileName);
    test.run();

    return 0;
}

