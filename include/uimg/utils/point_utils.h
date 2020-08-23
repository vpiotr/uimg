//
// Created by piotr on 8/22/20.
//

#ifndef UIMG_MIN_POINT_UTILS_H
#define UIMG_MIN_POINT_UTILS_H

#include <cmath>
#include "uimg/base/structs.h"

class point_utils {
    point_utils() {}

public:
    static Point &multiply(Point &p, int a) {
        p.x *= a;
        p.y *= a;
        return p;
    }

    static Point multiply_copy(const Point &p, int a) {
        Point result;
        result.x = p.x * a;
        result.y = p.y * a;
        return result;
    }

    static Point &multiply(Point &p, float a) {
        p.x = round(p.x * a);
        p.y = round(p.y * a);
        return p;
    }

    static Point multiply_copy(const Point &p, float a) {
        Point result;
        result.x = round(p.x * a);
        result.y = round(p.y * a);
        return result;
    }

    static Point &multiply(Point &p, int ax, int ay) {
        p.x *= ax;
        p.y *= ay;
        return p;
    }

    static Point multiply_copy(const Point &p, int ax, int ay) {
        Point result;
        result.x = p.x * ax;
        result.y = p.y * ay;
        return result;
    }

    static Point &multiply(Point &p, float ax, float ay) {
        p.x = round(p.x * ax);
        p.y = round(p.y * ay);
        return p;
    }

    static Point multiply_copy(const Point &p, float ax, float ay) {
        Point result;
        result.x = round(p.x * ax);
        result.y = round(p.y * ay);
        return result;
    }

    static Point &shift(Point &p, int ax, int ay) {
        p.x += ax;
        p.y += ay;
        return p;
    }

    static Point shift_copy(const Point &p, int ax, int ay) {
        Point result;
        result.x = p.x + ax;
        result.y = p.y + ay;
        return result;
    }

    static Point &shift(Point &p, float ax, float ay) {
        p.x = round(p.x + ax);
        p.y = round(p.y + ay);
        return p;
    }

    static Point shift_copy(const Point &p, float ax, float ay) {
        Point result;
        result.x = round(p.x + ax);
        result.y = round(p.y + ay);
        return result;
    }
};

#endif //UIMG_MIN_POINT_UTILS_H
