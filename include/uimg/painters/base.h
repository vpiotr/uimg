#ifndef __GUARD_UIMG_BASE_H__
#define __GUARD_UIMG_BASE_H__


// -----------------------------------------
// uImg - minimal image generation library
// -----------------------------------------

#include <vector>
#include <cstdint>
#include <sstream>

#include "uimg/utils/point_utils.h"

// pixel position
struct Point {
    int x, y;

    Point(int ax, int ay) : x(ax), y(ay) {}

    Point() : x(0), y(0) {}

    Point(const Point &src) {
        x = src.x;
        y = src.y;
    }

    Point &operator=(const Point &src) {
        x = src.x;
        y = src.y;
        return *this;
    }

    static Point make_point(int x, int y) {
        return {x, y};
    }
};

class point_utils : public point_utils_base<Point> {
};

struct PointF {
    float x, y;

    PointF(float ax, float ay) : x(ax), y(ay) {}

    PointF() : x(0), y(0) {}

    PointF(const PointF &src) {
        x = src.x;
        y = src.y;
    }

    PointF &operator=(const PointF &src) {
        x = src.x;
        y = src.y;
        return *this;
    }
};

struct Rect {
    int x1, y1, x2, y2;

    Point topLeft() const { return Point(x1, y1); }

    Point bottomRight() const { return Point(x2, y2); }

    Rect &topLeft(const Point &newValue) {
        x1 = newValue.x;
        y1 = newValue.y;
        return *this;
    }

    Rect &bottomRight(const Point &newValue) {
        x2 = newValue.x;
        y2 = newValue.y;
        return *this;
    }

    Rect &size(const Point &newValue) {
        x2 = x1 + newValue.x - 1;
        y2 = y1 + newValue.y - 1;
        return *this;
    }

    Point size() {
        Point result;
        result.x = width();
        result.y = height();
        return result;
    }

    int width() const { return std::abs(x2 - x1); }

    int height() const { return std::abs(y2 - y1); }

    static Rect make_rect(int x1, int y1, int x2, int y2) {
        return {x1, y1, x2, y2};
    }
};

template<typename S, typename D>
D rect_cast(S src) {
    D result;
    result.x1 = src.x1;
    result.y1 = src.y1;
    result.x2 = src.x2;
    result.y2 = src.y2;
    return result;
}

// pixel color specification for RGB image
struct RgbColor {
    unsigned char red;
    unsigned char green;
    unsigned char blue;

    bool operator!=(const RgbColor &rhs) const {
        return (red != rhs.red) ||
               (green != rhs.green) ||
               (blue != rhs.blue);
    }

    bool operator==(const RgbColor &rhs) const {
        return (red == rhs.red) &&
               (green == rhs.green) &&
               (blue == rhs.blue);
    }

    void clear() {
        red = green = blue = 0;
    }
};

// pixel color specification for RGBA image
struct RgbaColor {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;

    bool operator!=(const RgbaColor &rhs) const {
        return (red != rhs.red) ||
               (green != rhs.green) ||
               (blue != rhs.blue) ||
               (alpha != rhs.alpha);
    }

    bool operator==(const RgbaColor &rhs) const {
        return (red == rhs.red) &&
               (green == rhs.green) &&
               (blue == rhs.blue) &&
               (alpha == rhs.alpha);
    }

    void clear() {
        red = green = blue = alpha = 0;
    }
};

class PixelSource {
public:
    virtual ~PixelSource() {}

    virtual Point getSize() const = 0;

    virtual RgbColor getPixel(const Point &pos) const = 0;
};

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

typedef uint32_t text_char_code_t;

// Abstract text source class
class text_source {
public:
    virtual ~text_source() {}

    virtual bool has_next() {
        return false;
    }

    virtual text_char_code_t get_next() { return 0; }
};

/// Character source using vector of unicode code values as storage
class vector_char_source : public text_source {
private:
    vector_char_source() : data_(nullptr), pos_(0) {}

public:
    vector_char_source(const vector_char_source &src) : data_(src.data_), pos_(src.pos_) {}

    vector_char_source(const std::vector<text_char_code_t> &data) : data_(&data), pos_(0) {}

    virtual bool has_next() {
        return pos_ < data_->size();
    }

    virtual text_char_code_t get_next() { return (*data_)[pos_++]; }

private:
    const std::vector<text_char_code_t> *data_;
    size_t pos_;
};

namespace uimg_details {

    template<typename T>
    std::string to_string(const T &arg) {
        std::ostringstream out;
        out << arg;
        return (out.str());
    }

};


#endif
