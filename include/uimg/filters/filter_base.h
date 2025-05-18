#ifndef __UIMG_FILTER_BASE_H__
#define __UIMG_FILTER_BASE_H__

#include "uImg/painters/painter_base.h"

// pixel painting chaining operator - performs recalculations and executes chained painter
class PixelFilter : public PixelPainter {
public:
    PixelFilter(PixelPainter &target) : target_(target) {}
protected:
    PixelPainter &getTarget() { return target_; }
private:
    PixelPainter &target_;
};


#endif
