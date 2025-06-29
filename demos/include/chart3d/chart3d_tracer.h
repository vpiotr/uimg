# pragma once
#include <cstdio>
#include <string>
#include <iostream>
#include <utility>
#include <algorithm>
#include <memory>

class Chart3DTracer {
public:
    Chart3DTracer(bool enabled = false) : enabled_(enabled) {}
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    template<typename... Args>
    void trace(const char* fmt, Args... args) const {
        if (enabled_) {
            printf("[CHART3D TRACE] ");
            if constexpr (sizeof...(args) == 0) {
                printf("%s", fmt);
            } else {
                printf(fmt, args...);
            }
            printf("\n");
        }
    }
    static std::shared_ptr<Chart3DTracer> getInstance() {
        static std::shared_ptr<Chart3DTracer> instance = std::make_shared<Chart3DTracer>();
        return instance;
    }
private:
    bool enabled_;
};
