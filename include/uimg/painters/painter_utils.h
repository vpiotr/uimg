# pragma once
#include "uimg/painters/painter_base.h"

class LineClippingPainter: public LinePainter {
public:
    LineClippingPainter(LinePainter &basePainter, const RectInclusive &clippingWindow): basePainter_(basePainter), clippingWindow_(clippingWindow) {}
    virtual ~LineClippingPainter() {}

    virtual void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, const RgbColor &color) {
        if (!isValidCoordinates(x1, y1) || !isValidCoordinates(x2, y2)) {
            return; // Coordinates are outside the clipping window
        }

        // TODO: Implement actual clipping logic for cases outside of clipping window
        // Clipping algorithm (Cohen-Sutherland or Liang-Barsky) can be implemented here
        // to determine if the line segment is within the clipping window and adjust coordinates accordingly.    

        basePainter_.drawLine(x1, y1, x2, y2, color);
    }

private:
    bool isValidCoordinates(unsigned int x, unsigned int y) const {
        return (x >= clippingWindow_.x1 && x <= clippingWindow_.x2 &&
                y >= clippingWindow_.y1 && y <= clippingWindow_.y2);
    }

    LinePainter &basePainter_;
    RectInclusive clippingWindow_;
};
