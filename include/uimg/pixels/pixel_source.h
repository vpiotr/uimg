//
// Created by piotr on 8/23/20.
//

#ifndef UIMG_PIXEL_SOURCE_H
#define UIMG_PIXEL_SOURCE_H

#include "uimg/base/structs.h"

// interface to classes which can be used as pixel source
class PixelSource {
public:
    virtual ~PixelSource() {}

    virtual Point getSize() const = 0;

    virtual RgbColor getPixel(const Point &pos) const = 0;
};

#endif //UIMG_PIXEL_SOURCE_H
