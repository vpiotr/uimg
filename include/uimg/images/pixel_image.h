//
// Created by piotr on 8/23/20.
//

#ifndef UIMG_PIXEL_IMAGE_H
#define UIMG_PIXEL_IMAGE_H

#include "uimg/base/structs.h"
#include "uimg/pixels/pixel_source.h"

/**
 * Abstract pixel container (R/W)
 */
class PixelImageBase : public PixelSource {
public:
    virtual ~PixelImageBase() {}

    virtual unsigned int width() const = 0;

    virtual unsigned int height() const = 0;

    virtual Point getSize() const = 0;

    virtual RgbColor getPixel(const Point &pos) const = 0;

    virtual void setPixel(const Point &pos, const RgbColor &color) = 0;
};

class PixelImageMetaInfo {
public:
    virtual ~PixelImageMetaInfo() {}

    virtual Point getSize() const = 0;
};

// abstract class which writes RGB image
class PixelImageWriter {
public:
    virtual ~PixelImageWriter() {}

    virtual void writeImage(PixelImageBase &image) = 0;
};

// abstract class which loads image
class PixelImageLoader {
public:
    virtual ~PixelImageLoader() {}

    virtual PixelImageMetaInfo *loadImageMeta() = 0;

    virtual PixelImageBase *loadImage() = 0;

    virtual PixelImageBase *loadImagePart(const Rect &srcPart, const Point &targetOffset) = 0;

    virtual bool loadImageInto(PixelImageBase &outputImage) = 0;

    virtual bool loadImagePartInto(PixelImageBase &outputImage, const Rect &srcPart, const Point &targetOffset) = 0;
};

class PixelImageMetaInfoBase : public PixelImageMetaInfo {
public:
    void setSize(const Point &value) { size_ = value; }

    virtual Point getSize() const {
        return size_;
    }

private:
    Point size_;
};


#endif //UIMG_PIXEL_IMAGE_H
