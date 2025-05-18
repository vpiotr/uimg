#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

#include "uimg/fonts/bdf_font.h"

namespace uimg {

/**
 * @brief Utility functions for font operations
 */
class FontUtils {
public:
    /**
     * @brief Load a BDF font from file
     * @param font Font to populate
     * @param filename Path to the font file
     * @return true if loaded successfully, false otherwise
     */
    static bool loadFontFromFile(BdfFont &font, const std::string &filename) {
        std::ifstream fontIn(filename, std::ios::in);

        if (!fontIn.is_open()) {
            std::cerr << "Error - file [" << filename << "] cannot be opened!" << std::endl;
            return false;
        }

        BdfFontLoader fontLoader;
        fontLoader.load(fontIn, font);
        fontIn.close();
        
        return true;
    }
};

} // namespace uimg
