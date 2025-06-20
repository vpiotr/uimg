#include <iostream>
#include <string>

#include "samples/demo_painter_base.h"
#include "samples/filter_demo/filter_demo.h"
#include "samples/filter_demo/filter_demo_args.h"

int main(int argc, char *argv[]) {
    // Print usage information
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        std::cout << "Usage: filter_demo [-out <output_file.ppm>]" << std::endl;
        std::cout << "Example: filter_demo -out filter_output.ppm" << std::endl;
        return 0;
    }
    
    // Parse command line arguments
    auto args = uimg::filter_demo_args::parse(argc, const_cast<const char**>(argv));
    
    // Create and run the filter demo
    uimg::FilterDemo demo(args.outFileName);
    demo.run();
    
    std::cout << "Filter demo image created: " << args.outFileName << std::endl;
    
    return 0;
}
