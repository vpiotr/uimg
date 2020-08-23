//
// Created by piotr on 8/23/20.
//

#ifndef UIMG_PIXEL_COPIER_H
#define UIMG_PIXEL_COPIER_H

#include "uimg/base/structs.h"
#include "uimg/pixels/pixel_source.h"

// copy image to image
class PixelCopier {
public:
    virtual ~PixelCopier() {}

    virtual void copyFrom(const PixelSource &src, const Point &targetPos) = 0;

    virtual void copyFrom(const PixelSource &src, const Point &targetPos, const Rect &srcPart) = 0;
};


#endif //UIMG_PIXEL_COPIER_H
