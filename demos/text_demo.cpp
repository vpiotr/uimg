#include <iostream>
#include <string>

#include "samples/demo_painter_base.h"
#include "samples/text_demo/text_demo.h"
#include "samples/text_demo/text_demo_args.h"

int main(int argc, char *argv[]) {
    // Print usage information
    if (argc < 3) {
        std::cout << "Usage: text_demo -font <path/to/font.bdf> [-out <output_file.ppm>]" << std::endl;
        std::cout << "Example: text_demo -font ../fonts/courR08.bdf -out text_output.ppm" << std::endl;
        return 1;
    }
    
    // Parse command line arguments
    auto args = text_demo_args::parse(argc, const_cast<const char**>(argv));
    
    // Create and run the text demo
    text_demo demo(args.outFileName, args.fontPath);
    demo.run();
    
    std::cout << "Text demo image created: " << args.outFileName << std::endl;
    
    return 0;
}
