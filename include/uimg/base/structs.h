//
// Created by piotr on 8/23/20.
//

#ifndef UIMG_STRUCTS_H
#define UIMG_STRUCTS_H

#include <cmath> // For std::abs

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

struct RectInclusive;

/*
    * Rectangle structure
    * x1, y1 - top left corner
    * x2, y2 - bottom right corner, exclusive
*/
struct RectExclusive {
    int x1, y1, x2, y2;

    Point topLeft() const { return Point(x1, y1); }

    Point bottomRight() const { return Point(x2, y2); }

    RectExclusive &topLeft(const Point &newValue) {
        x1 = newValue.x;
        y1 = newValue.y;
        return *this;
    }

    RectExclusive &bottomRight(const Point &newValue) {
        x2 = newValue.x;
        y2 = newValue.y;
        return *this;
    }

    RectExclusive &size(const Point &newValue) {
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

    static RectExclusive make_rect(int x1, int y1, int x2, int y2) {
        return {x1, y1, x2, y2};
    }

    // Convert to RectInclusive
    RectInclusive toInclusive() const;

    // Create from RectInclusive
    static RectExclusive fromInclusive(const RectInclusive &rect);
};

typedef RectExclusive Rect;

/*
    * Rectangle structure
    * x1, y1 - top left corner
    * x2, y2 - bottom right corner, inclusive
*/
struct RectInclusive {
    int x1, y1, x2, y2;

    Point topLeft() const { return Point(x1, y1); }

    Point bottomRight() const { return Point(x2, y2); }

    RectInclusive &topLeft(const Point &newValue) {
        x1 = newValue.x;
        y1 = newValue.y;
        return *this;
    }

    RectInclusive &bottomRight(const Point &newValue) {
        x2 = newValue.x;
        y2 = newValue.y;
        return *this;
    }

    RectInclusive &size(const Point &newValue) {
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

    int width() const { return std::abs(x2 - x1) + 1; }

    int height() const { return std::abs(y2 - y1) + 1; }

    static RectInclusive make_rect(int x1, int y1, int x2, int y2) {
        return {x1, y1, x2, y2};
    }

    // Convert to RectExclusive
    RectExclusive toExclusive() const;

    // Create from RectExclusive
    static RectInclusive fromExclusive(const RectExclusive &rect);
};


// pixel color specification for RGB image
struct RgbColor {
    unsigned char red;
    unsigned char green;
    unsigned char blue;

    static RgbColor make_rgb(int r, int g, int b) {
        return {(unsigned char)r, (unsigned char)g, (unsigned char)b};
    }

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

// Implement the conversion functions between RectExclusive and RectInclusive

// RectExclusive to RectInclusive conversion
inline RectInclusive RectExclusive::toInclusive() const {
    RectInclusive rect;
    rect.x1 = x1;
    rect.y1 = y1;
    rect.x2 = x2 - 1; // Convert exclusive to inclusive (subtract 1)
    rect.y2 = y2 - 1; // Convert exclusive to inclusive (subtract 1)
    return rect;
}

// RectInclusive to RectExclusive conversion
inline RectExclusive RectInclusive::toExclusive() const {
    RectExclusive rect;
    rect.x1 = x1;
    rect.y1 = y1;
    rect.x2 = x2 + 1; // Convert inclusive to exclusive (add 1)
    rect.y2 = y2 + 1; // Convert inclusive to exclusive (add 1)
    return rect;
}

// Create RectExclusive from RectInclusive
inline RectExclusive RectExclusive::fromInclusive(const RectInclusive& rect) {
    return rect.toExclusive();
}

// Create RectInclusive from RectExclusive
inline RectInclusive RectInclusive::fromExclusive(const RectExclusive& rect) {
    return rect.toInclusive();
}

#endif //UIMG_STRUCTS_H
