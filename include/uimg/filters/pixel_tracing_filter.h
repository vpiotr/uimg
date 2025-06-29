//
// Created for pixel tracing functionality
//

#ifndef UIMG_PIXEL_TRACING_FILTER_H
#define UIMG_PIXEL_TRACING_FILTER_H

#include "uimg/pixels/pixel_painter.h"
#include "dlog/dlog.h"
#include <limits>
#include <memory>

/**
 * @brief Pixel tracing filter that tracks min/max x,y coordinates for pixel drawing requests
 * 
 * This filter wraps another PixelPainter and forwards all requests to it while tracking
 * the bounding box of all pixel operations. It provides DEBUG level logging of the
 * pixel range when requested.
 */
class PixelTracingFilter : public PixelPainter {
public:
    /**
     * @brief Constructor
     * @param basePainter The pixel painter to forward requests to
     * @param chartName Optional name for this chart (used in logging)
     */
    PixelTracingFilter(PixelPainter& basePainter, const std::string& chartName = "Chart")
        : basePainter_(basePainter), chartName_(chartName), 
          minX_(std::numeric_limits<unsigned int>::max()),
          minY_(std::numeric_limits<unsigned int>::max()),
          maxX_(std::numeric_limits<unsigned int>::min()),
          maxY_(std::numeric_limits<unsigned int>::min()),
          hasPixels_(false) {}

    /**
     * @brief Get pixel color at coordinates
     * @param x X coordinate
     * @param y Y coordinate
     * @param output Output color
     */
    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) override {
        basePainter_.getPixel(x, y, output);
    }

    /**
     * @brief Put pixel with color at coordinates (main tracking method)
     * @param x X coordinate
     * @param y Y coordinate
     * @param color Pixel color
     */
    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) override {
        trackPixel(x, y);
        basePainter_.putPixel(x, y, color);
    }

    /**
     * @brief Put white pixel at coordinates
     * @param x X coordinate
     * @param y Y coordinate
     */
    virtual void putPixel(unsigned int x, unsigned int y) override {
        trackPixel(x, y);
        basePainter_.putPixel(x, y);
    }

    /**
     * @brief Put pixel with alpha blending
     * @param x X coordinate
     * @param y Y coordinate
     * @param color Pixel color
     * @param alpha Alpha value
     */
    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color, float alpha) override {
        trackPixel(x, y);
        basePainter_.putPixel(x, y, color, alpha);
    }

    /**
     * @brief Put pixel with RGBA color
     * @param x X coordinate
     * @param y Y coordinate
     * @param color RGBA pixel color
     */
    virtual void putPixel(unsigned int x, unsigned int y, const RgbaColor &color) override {
        trackPixel(x, y);
        basePainter_.putPixel(x, y, color);
    }

    /**
     * @brief Reset tracking to initial state
     */
    void resetTracking() {
        minX_ = std::numeric_limits<unsigned int>::max();
        minY_ = std::numeric_limits<unsigned int>::max();
        maxX_ = std::numeric_limits<unsigned int>::min();
        maxY_ = std::numeric_limits<unsigned int>::min();
        hasPixels_ = false;
    }

    /**
     * @brief Log the current pixel range with DEBUG level
     */
    void logPixelRange() const {
        auto logger = dlog::Logger::getInstance();
        if (hasPixels_) {
            logger->debug("%s chart pixel range: x=[%d, %d], y=[%d, %d]  [%ux%u pixels]", 
                        chartName_.c_str(), 
                        minX_, maxX_,
                        minY_, maxY_,
                        maxX_ - minX_ + 1, maxY_ - minY_ + 1);
        } else {
            logger->debug("%s: No pixels drawn", chartName_.c_str());
        }
    }

    /**
     * @brief Check if any pixels have been drawn
     * @return true if pixels have been tracked
     */
    bool hasPixels() const {
        return hasPixels_;
    }

    /**
     * @brief Get the bounding box of tracked pixels
     * @param minX Minimum X coordinate (output)
     * @param minY Minimum Y coordinate (output)
     * @param maxX Maximum X coordinate (output)
     * @param maxY Maximum Y coordinate (output)
     * @return true if valid range available
     */
    bool getPixelRange(unsigned int& minX, unsigned int& minY, unsigned int& maxX, unsigned int& maxY) const {
        if (hasPixels_) {
            minX = minX_;
            minY = minY_;
            maxX = maxX_;
            maxY = maxY_;
            return true;
        }
        return false;
    }

    /**
     * @brief Get the base painter (for operations that should bypass tracing)
     * @return Reference to the base painter
     */
    PixelPainter& getBasePainter() {
        return basePainter_;
    }

private:
    /**
     * @brief Track a pixel coordinate and update bounding box
     * @param x X coordinate
     * @param y Y coordinate
     */
    void trackPixel(unsigned int x, unsigned int y) {
        if (!hasPixels_) {
            minX_ = maxX_ = x;
            minY_ = maxY_ = y;
            hasPixels_ = true;
        } else {
            if (x < minX_) minX_ = x;
            if (x > maxX_) maxX_ = x;
            if (y < minY_) minY_ = y;
            if (y > maxY_) maxY_ = y;
        }
    }

    PixelPainter& basePainter_;       ///< The base painter to forward requests to
    std::string chartName_;           ///< Name of the chart for logging
    unsigned int minX_, minY_;        ///< Minimum coordinates
    unsigned int maxX_, maxY_;        ///< Maximum coordinates
    bool hasPixels_;                  ///< Whether any pixels have been drawn
};

#endif //UIMG_PIXEL_TRACING_FILTER_H
