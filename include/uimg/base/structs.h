//
// Created by piotr on 8/23/20.
//

#ifndef UIMG_STRUCTS_H
#define UIMG_STRUCTS_H

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


#endif //UIMG_STRUCTS_H
