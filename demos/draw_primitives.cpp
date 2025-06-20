#include "samples/test_painter_base.h"
#include "samples/draw_primitives//test_draw_primitives.h"

int main(int argc, const char *argv[]) {
    auto testInfo = test_painter_base::get_test_args(argc, argv, "primitives");

    test_draw_primitives test(testInfo.outFileName);
    test.run();

    return 0;
}

