#ifndef __UIMG_MATH_UTILS_H__
#define __UIMG_MATH_UTILS_H__

#include <cmath>

class math_utils {
public:
    template<typename T>
    static T square(T a) {
        return a * a;
    }

    template<typename T>
    static bool value_between(T a, T minVal, T maxVal) {
        return (a >= minVal) && (a <= maxVal);
    }

    template<typename T>
    static T iabs(T a) {
        return (a < 0) ? -a : a;
    }

    static double pi_const_d() {
        return 3.1415926;
    }

    template<typename T>
    static T pi_const() {
        return 3.1415926;
    }

    template<typename T>
    static T frac(T a) {
        double fracPart;
        double intPart;
        fracPart = modf(static_cast<double>(a), &intPart);
        return static_cast<T>(fracPart);
    }

    template<typename T>
    static int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }
};


#endif
