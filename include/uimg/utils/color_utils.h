#ifndef __UIMG_COLOR_UTILS_H__
#define __UIMG_COLOR_UTILS_H__

#include <cmath>

class color_utils {
public:

    static RgbColor mix_colors(const RgbColor &color1, const RgbColor &color2, float partOfColor1) {
        RgbColor result;
        float factor1 = std::min(1.0f, std::max(0.0f, partOfColor1));
        float factor2 = 1.0f - factor1;
        result.red = std::min(255, std::max(0, static_cast<int>(round(color1.red * factor1 + color2.red * factor2))));
        result.green = std::min(255,
                                std::max(0, static_cast<int>(round(color1.green * factor1 + color2.green * factor2))));
        result.blue = std::min(255,
                               std::max(0, static_cast<int>(round(color1.blue * factor1 + color2.blue * factor2))));
        return result;
    }

};

#endif
