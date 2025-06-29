#ifndef __UIMG_ANTI_ALIASING_FILTER_H__
#define __UIMG_ANTI_ALIASING_FILTER_H__

#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>

#include "uimg/base/structs.h"
#include "uimg/images/rgb_image.h"
#include "uimg/painters/painter_for_rgb_image.h"
#include "uimg/filters/filter_base.h"
#include "uimg/utils/cast.h"
#include "uimg/utils/color_utils.h"

/**
 * @brief Anti-aliasing filter that provides smooth rendering by using super-sampling
 * 
 * This filter creates a higher resolution internal buffer and downsamples it to 
 * the target image to achieve anti-aliasing effects. It uses a 2x2 super-sampling
 * approach for good quality with reasonable performance.
 */
class AntiAliasingFilter {
public:
    /**
     * @brief Constructor
     * @param targetImage The target image to render to
     * @param superSampleFactor Super-sampling factor (default 2 for 2x2 sampling)
     */
    AntiAliasingFilter(RgbImage& targetImage, unsigned int superSampleFactor = 2) 
        : targetImage_(targetImage), 
          superSampleFactor_(superSampleFactor),
          superSampleImage_(targetImage.width() * superSampleFactor, targetImage.height() * superSampleFactor),
          superSamplePainter_(superSampleImage_),
          filteredPainter_(*this) {
        
        // Clear the super-sample buffer to white initially
        unsigned char* data = static_cast<unsigned char*>(superSampleImage_.data());
        size_t dataSize = superSampleImage_.dataSize();
        std::fill(data, data + dataSize, 255);
    }

    /**
     * @brief Get the filtered painter that applies anti-aliasing
     * @return Reference to the anti-aliasing pixel painter
     */
    PixelPainter& getFilteredPainter() {
        return filteredPainter_;
    }

    /**
     * @brief Flush the super-sampled buffer to the target image
     * This should be called after all drawing operations are complete
     */
    void flush() {
        downsampleToTarget();
    }

private:
    /**
     * @brief Internal pixel painter that writes to the super-sampled buffer
     */
    class AntiAliasingPixelPainter : public PixelPainter {
    public:
        AntiAliasingPixelPainter(AntiAliasingFilter& filter) : filter_(filter) {}

        virtual void putPixel(unsigned int x, unsigned int y, const RgbColor& color) override {
            // Map target coordinates to super-sampled coordinates
            unsigned int ssX = x * filter_.superSampleFactor_;
            unsigned int ssY = y * filter_.superSampleFactor_;
            
            // Fill the super-sampled region
            for (unsigned int dy = 0; dy < filter_.superSampleFactor_; ++dy) {
                for (unsigned int dx = 0; dx < filter_.superSampleFactor_; ++dx) {
                    unsigned int pixelX = ssX + dx;
                    unsigned int pixelY = ssY + dy;
                    
                    if (pixelX < filter_.superSampleImage_.width() && 
                        pixelY < filter_.superSampleImage_.height()) {
                        filter_.superSamplePainter_.putPixel(pixelX, pixelY, color);
                    }
                }
            }
            
            // Immediately update the target pixel with the downsampled result
            filter_.updateTargetPixel(x, y);
        }

        virtual void getPixel(unsigned int x, unsigned int y, RgbColor& output) override {
            // Get the pixel from the target image
            if (x < filter_.targetImage_.width() && y < filter_.targetImage_.height()) {
                output = filter_.targetImage_.getPixel(Point(static_cast<int>(x), static_cast<int>(y)));
            } else {
                output = {0, 0, 0};
            }
        }

        virtual void putPixel(unsigned int x, unsigned int y, const RgbColor& color, float alpha) override {
            if (alpha >= 1.0f) {
                putPixel(x, y, color);
                return;
            }
            
            // Get existing color and blend
            RgbColor existing;
            getPixel(x, y, existing);
            RgbColor blended = color_utils::mix_colors(color, existing, alpha);
            putPixel(x, y, blended);
        }

    private:
        AntiAliasingFilter& filter_;
    };

    /**
     * @brief Update a single target pixel by downsampling from the super-sampled buffer
     * @param x Target pixel x coordinate
     * @param y Target pixel y coordinate
     */
    void updateTargetPixel(unsigned int x, unsigned int y) {
        if (x >= targetImage_.width() || y >= targetImage_.height()) {
            return;
        }

        unsigned int ssX = x * superSampleFactor_;
        unsigned int ssY = y * superSampleFactor_;
        
        unsigned int totalRed = 0, totalGreen = 0, totalBlue = 0;
        unsigned int sampleCount = 0;
        
        // Sample from the super-sampled buffer
        for (unsigned int dy = 0; dy < superSampleFactor_; ++dy) {
            for (unsigned int dx = 0; dx < superSampleFactor_; ++dx) {
                unsigned int sampleX = ssX + dx;
                unsigned int sampleY = ssY + dy;
                
                if (sampleX < superSampleImage_.width() && 
                    sampleY < superSampleImage_.height()) {
                    
                    RgbColor sampleColor = superSampleImage_.getPixel(Point(
                        static_cast<int>(sampleX), static_cast<int>(sampleY)));
                    
                    totalRed += sampleColor.red;
                    totalGreen += sampleColor.green;
                    totalBlue += sampleColor.blue;
                    sampleCount++;
                }
            }
        }
        
        if (sampleCount > 0) {
            RgbColor avgColor;
            avgColor.red = UNSIGNED_CAST(unsigned char, totalRed / sampleCount);
            avgColor.green = UNSIGNED_CAST(unsigned char, totalGreen / sampleCount);
            avgColor.blue = UNSIGNED_CAST(unsigned char, totalBlue / sampleCount);
            
            targetImage_.setPixel(Point(static_cast<int>(x), static_cast<int>(y)), avgColor);
        }
    }

    /**
     * @brief Downsample the entire super-sampled buffer to the target image
     */
    void downsampleToTarget() {
        for (unsigned int y = 0; y < targetImage_.height(); ++y) {
            for (unsigned int x = 0; x < targetImage_.width(); ++x) {
                updateTargetPixel(x, y);
            }
        }
    }

    RgbImage& targetImage_;                              ///< Target image to render to
    unsigned int superSampleFactor_;                     ///< Super-sampling factor
    RgbImage superSampleImage_;                          ///< High-resolution internal buffer
    PixelPainterForRgbImage superSamplePainter_;        ///< Painter for the super-sampled buffer
    AntiAliasingPixelPainter filteredPainter_;          ///< The filtered painter interface
};

#endif // __UIMG_ANTI_ALIASING_FILTER_H__
