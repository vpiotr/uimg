# pragma once
#include <string>
#include "uimg/base/structs.h"

// Helper class for filter demo arguments
namespace uimg {
    struct filter_demo_args {
        std::string outFileName;
        
        static filter_demo_args parse(int argc, const char **argv) {
            filter_demo_args args;
            args.outFileName = "filter_demo_output.ppm";  // Default output file
            
            for (int i = 1; i < argc; i++) {
                std::string arg = argv[i];
                if ((arg == "-out" || arg == "--out" || arg == "-o") && i + 1 < argc) {
                    args.outFileName = argv[++i];
                }
            }
            
            return args;
        }
    };
}
