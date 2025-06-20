// Helper class for text demo arguments
struct text_demo_args {
    std::string fontPath;
    std::string outFileName;
    
    static text_demo_args parse(int argc, const char **argv) {
        text_demo_args args;
        args.outFileName = "text_demo_output.ppm";  // Default output file
        
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if ((arg == "-font" || arg == "--font") && i + 1 < argc) {
                args.fontPath = argv[++i];
            } else if ((arg == "-out" || arg == "--out" || arg == "-o") && i + 1 < argc) {
                args.outFileName = argv[++i];
            }
        }
        
        return args;
    }
};
