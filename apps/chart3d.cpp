#include <iostream>
#include "samples/test_painter_base.h"
#include "samples/chart3d/test_chart_z_fxy_3d_c.h"

int main(int argc, const char *argv[]) {
    auto testInfo = test_painter_base::get_test_args(argc, argv, "chart3d");

    // Display anti-aliasing status
    std::cerr << "Anti-aliasing: " << (testInfo.useAntiAliasing ? "Enabled" : "Disabled") << std::endl;

    test_chart_z_fxy_3d_c test(testInfo.outFileName);
    test.setUseAntiAliasing(testInfo.useAntiAliasing);
    test.run();

    return 0;
}

