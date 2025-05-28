#ifndef _UIMG_RGB_IMAGE_H__
#define _UIMG_RGB_IMAGE_H__

#include <vector>
#include <cstddef> // For size_t

#include "uimg/base/structs.h"
#include "uimg/images/pixel_image.h"

// RGB in-memory image container.
// Each pixel is represented as three bytes (red, green, blue), from top left to bottom right, (0,0) = top, left
class RgbImage : public PixelImageBase {
public:
    RgbImage(unsigned int width, unsigned int height) : data_(3 * width * height), width_(width), height_(height) {
    }

    // returns image width
    virtual unsigned int width() const {
        return width_;
    }

    // returns image height
    virtual unsigned int height() const {
        return height_;
    }

    virtual Point getSize() const {
        return Point(static_cast<int>(width_), static_cast<int>(height_));
    }

    virtual RgbColor getPixel(const Point &pos) const {
        RgbColor r;
        if (pos.x < static_cast<int>(width_) && pos.y < static_cast<int>(height_)) {
            unsigned int offset = (pos.y * width_ + pos.x) * 3;
            r.red = data_[offset];
            r.green = data_[offset + 1];
            r.blue = data_[offset + 2];
        } else {
            r.red = r.blue = r.green = 0;
        }
        return r;
    }
    
    // For direct x,y coordinate access
    RgbColor getPixel(int x, int y) const {
        return getPixel(Point(x, y));
    }
    
    // Alias for width() for consistency with other libraries
    unsigned int getWidth() const {
        return width();
    }
    
    // Alias for height() for consistency with other libraries
    unsigned int getHeight() const {
        return height();
    }

    virtual void setPixel(const Point &pos, const RgbColor &color) {
        if (pos.x < static_cast<int>(width_) && pos.y < static_cast<int>(height_)) {
            unsigned int offset = (pos.y * width_ + pos.x) * 3;
            data_[offset] = color.red;
            data_[offset + 1] = color.green;
            data_[offset + 2] = color.blue;
        }
    }

    // returns raw pointer to internal data
    virtual void *data() {
        return &(data_[0]);
    }

    // returns size in bytes of data
    virtual size_t dataSize() {
        return data_.size();
    }

private:
    std::vector<unsigned char> data_;
    unsigned int width_;
    unsigned int height_;
};

#endif
