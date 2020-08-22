#pragma once

#include <algorithm>
#include <cmath>

#include "uimg/utils/math_utils.h"

class algorithm {
public:
    // calculates minimum distance to vector's start, end and connecting line
    // if point outside of vector, one of it's ends will be selected for distance calc
    template<typename U, typename T>
    static U distance_to_vector(T x, T y, T x1, T y1, T x2, T y2) {
        U dp1, dp2;
        U dp;

        T dx = x2 - x1;
        T dy = y2 - y1;

        T dxy2 = math_utils::square(dx) + math_utils::square(dy);
        U u;

        bool singlePoint = (dxy2 == static_cast<T>(0));

        dp1 = static_cast<U>(sqrt(math_utils::square(x - x1) + (math_utils::square(y - y1))));

        if (!singlePoint) {
            dp2 = static_cast<U>(sqrt(math_utils::square(x - x2) + (math_utils::square(y - y2))));
            u = static_cast<U>((dx * (x - x1) + dy * (y - y1))) / dxy2;
            dp = std::min(dp1, dp2);
        } else {
            u = 2.0;   // cast outside of vector
            dp = dp1;  // distance = distance to first point
        }

        if ((u >= 0.0) && (u <= 1.0) && (!singlePoint)) {
            U xv = x1 + u * dx;
            U yv = y1 + u * dy;
            U dp12 = sqrt(math_utils::square(x - xv) + (math_utils::square(y - yv)));
            dp = std::min(dp, dp12);
        }

        return dp;
    }

    // calculates where point lays on vector, if outside of vector, one of ends will be selected
    template<typename T>
    static void cast_point_on_vector(T &out_x, T &out_y, T x, T y, T x1, T y1, T x2, T y2) {
        T dx = x2 - x1;
        T dy = y2 - y1;

        T p3x = x;
        T p3y = y;

        T ua = (p3x - x1) * (dx) + (p3y - y1) * dy;

        int ub = math_utils::square(dx) + math_utils::square(dy);

        if (ub == 0) {
            out_x = x1;
            out_y = y1;
            return;
        }

        double u = static_cast<double>(ua) / static_cast<double>(ub);

        if (u < 0.0) {
            out_x = x1;
            out_y = y1;
        } else if (u > 1.0) {
            out_x = x2;
            out_y = y2;
        } else {
            out_x = static_cast<T>(x1 + dx * u);
            out_y = static_cast<T>(y1 + dy * u);
        }
    }

    // calculates normalized distance of point to vector start (0..1)
    template<typename T>
    static T distance_to_vector_start(T x, T y, T x1, T y1, T x2, T y2) {
        T dx = x2 - x1;
        T dy = y2 - y1;

        T p3x = x;
        T p3y = y;

        T ua = (p3x - x1) * (dx) + (p3y - y1) * dy;

        int ub = math_utils::square(dx) + math_utils::square(dy);

        if (ub == 0) {
            return 0;
        }

        double u = static_cast<double>(ua) / static_cast<double>(ub);

        T result;

        if (u < 0.0) {
            result = 0;
        } else if (u > 1.0) {
            result = 1;
        } else {
            result = u;
        }

        return result;
    }

    template<typename T>
    static T point_distance(T x1, T y1, T x2, T y2) {
        return sqrt(math_utils::square(x2 - x1) + math_utils::square(y2 - y1));
    }

    template<typename T>
    static T point_distance_sqr(T x1, T y1, T x2, T y2) {
        return math_utils::square(x2 - x1) + math_utils::square(y2 - y1);
    }

};
