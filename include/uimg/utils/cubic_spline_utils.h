#ifndef __UIMG_CUBIC_SPL_UTIL_H__
#define __UIMG_CUBIC_SPL_UTIL_H__

#include <vector>
#include "uimg/base/structs.h"

class cubic_spline_utils {
public:
    // Cubic B-spline
    static void bsp(const Point &p1, const Point &p2, const Point &p3, const Point &p4, int divisions,
                    std::vector<Point> &pixelPoints) {
        pixelPoints.clear();

        double a[5];
        double b[5];
        Point p;

        a[0] = (-p1.x + 3 * p2.x - 3 * p3.x + p4.x) / 6.0;
        a[1] = (3 * p1.x - 6 * p2.x + 3 * p3.x) / 6.0;
        a[2] = (-3 * p1.x + 3 * p3.x) / 6.0;
        a[3] = (p1.x + 4 * p2.x + p3.x) / 6.0;
        b[0] = (-p1.y + 3 * p2.y - 3 * p3.y + p4.y) / 6.0;
        b[1] = (3 * p1.y - 6 * p2.y + 3 * p3.y) / 6.0;
        b[2] = (-3 * p1.y + 3 * p3.y) / 6.0;
        b[3] = (p1.y + 4 * p2.y + p3.y) / 6.0;

        p.x = static_cast<int>(round(a[3]));
        p.y = static_cast<int>(round(b[3]));

        pixelPoints.push_back(p);
        int i;
        float fdiv = static_cast<float>(divisions);

        for (i = 1; i <= divisions - 1; i++) {
            float t = static_cast<float>(i) / fdiv;

            p.x = static_cast<int>(round((a[2] + t * (a[1] + t * a[0])) * t + a[3]));
            p.y = static_cast<int>(round((b[2] + t * (b[1] + t * b[0])) * t + b[3]));

            pixelPoints.push_back(p);
        }
    }

    // include end points in bspline
    static void include_ends(std::vector<Point> &pixelPoints) {
        int_include_ends(pixelPoints);
        int_include_ends(pixelPoints);
    }

protected:
    static void int_include_ends(std::vector<Point> &pixelPoints) {
        if (pixelPoints.size() < 2)
            return;
        Point p1 = pixelPoints[0];
        Point p2 = pixelPoints[1];
        Point p3 = find_mid_point_close_first(p1, p2);
        std::vector<Point>::iterator it = pixelPoints.begin();
        pixelPoints.insert(it + 1, p3);

        size_t n = pixelPoints.size();
        p1 = pixelPoints[n - 1];
        p2 = pixelPoints[n - 2];
        p3 = find_mid_point_close_first(p1, p2);

        it = pixelPoints.begin();
        pixelPoints.insert(it + static_cast<std::vector<Point>::difference_type>(n - 1), p3);
    }

private:
    static Point find_mid_point_close_first(const Point &p1, const Point &p2) {
        Point result;
        int dx = p2.x - p1.x;
        int dy = p2.y - p1.y;
        int np;

        if (dx == 0 && dy == 0) {
            return p1;
        } else if (dx == 0) {
            result.x = p1.x;
            np = static_cast<int>(round(1 / dy));
            result.y = p1.y + ((dy < 0) ? -np : np);
        } else if (dy == 0) {
            np = static_cast<int>(round(1 / dx));
            result.x = p1.x + ((dx < 0) ? -np : np);
            result.y = p1.y;
        } else {
            np = (dx < 0) ? -1 : 1;
            result.x = p1.x + np;
            result.y = p1.y + static_cast<int>(round(static_cast<float>(np * dy) / static_cast<float>(dx)));
        }

        return result;
    }
};


#endif
