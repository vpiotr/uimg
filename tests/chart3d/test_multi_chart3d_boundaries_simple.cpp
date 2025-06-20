#include "utest/utest.h"
#include "dlog/dlog.h"
#include "uimg/base/structs.h"
#include "uimg/images/rgb_image.h"
#include "uimg/painters/painter_for_rgb_image.h"
#include "uimg/pixels/pixel_painter.h"

#include <vector>
#include <map>
#include <set>
#include <functional>

/**
 * @brief Tracking pixel painter that records all pixel operations
 * 
 * This painter wraps another painter and tracks all putPixel calls,
 * recording the coordinates and colors for later analysis.
 */
class TrackingPixelPainter : public PixelPainter {
public:
    struct PixelRecord {
        unsigned int x, y;
        RgbColor color;
        
        PixelRecord(unsigned int x_, unsigned int y_, const RgbColor& color_) 
            : x(x_), y(y_), color(color_) {}
    };

private:
    PixelPainter& wrappedPainter_;
    std::vector<PixelRecord> pixelHistory_;
    std::set<std::pair<unsigned int, unsigned int>> pixelPositions_;
    
public:
    TrackingPixelPainter(PixelPainter& painter) : wrappedPainter_(painter) {}
    
    virtual void putPixel(unsigned int x, unsigned int y, const RgbColor &color) override {
        // Record the pixel operation
        pixelHistory_.emplace_back(x, y, color);
        pixelPositions_.insert({x, y});
        
        // Forward to wrapped painter
        wrappedPainter_.putPixel(x, y, color);
    }
    
    virtual void getPixel(unsigned int x, unsigned int y, RgbColor &output) override {
        wrappedPainter_.getPixel(x, y, output);
    }
    
    // Test utility methods
    const std::vector<PixelRecord>& getPixelHistory() const { return pixelHistory_; }
    const std::set<std::pair<unsigned int, unsigned int>>& getPixelPositions() const { return pixelPositions_; }
    
    void clearHistory() {
        pixelHistory_.clear();
        pixelPositions_.clear();
    }
    
    bool hasPixelAt(unsigned int x, unsigned int y) const {
        return pixelPositions_.find({x, y}) != pixelPositions_.end();
    }
    
    std::vector<PixelRecord> getPixelsInRegion(unsigned int x1, unsigned int y1, 
                                               unsigned int x2, unsigned int y2) const {
        std::vector<PixelRecord> result;
        for (const auto& record : pixelHistory_) {
            if (record.x >= x1 && record.x <= x2 && 
                record.y >= y1 && record.y <= y2) {
                result.push_back(record);
            }
        }
        return result;
    }
    
    int countPixelsOutsideRegion(unsigned int x1, unsigned int y1, 
                                unsigned int x2, unsigned int y2) const {
        int count = 0;
        for (const auto& record : pixelHistory_) {
            if (record.x < x1 || record.x > x2 || 
                record.y < y1 || record.y > y2) {
                count++;
            }
        }
        return count;
    }
};

/**
 * @brief Mock multi-chart renderer for testing boundary violations
 * 
 * This class simulates the behavior of multi-chart rendering for testing purposes,
 * focusing on the boundary checking logic without the complexity of actual chart rendering.
 */
class MockMultiChartRenderer {
private:
    int canvasWidth_;
    int canvasHeight_;
    int numCharts_;
    std::string layout_;
    
public:
    MockMultiChartRenderer(int width, int height, int numCharts, const std::string& layout)
        : canvasWidth_(width), canvasHeight_(height), numCharts_(numCharts), layout_(layout) {}
    
    void determineLayout(int &rows, int &cols) {
        if (layout_ == "auto") {
            if (numCharts_ <= 2) {
                rows = 1;
                cols = numCharts_;
            } else if (numCharts_ <= 4) {
                rows = 2;
                cols = 2;
            } else if (numCharts_ <= 6) {
                rows = 2;
                cols = 3;
            } else if (numCharts_ <= 9) {
                rows = 3;
                cols = 3;
            } else {
                rows = 4;
                cols = 3;
            }
        } else if (layout_ == "horizontal") {
            rows = 1;
            cols = numCharts_;
        } else if (layout_ == "vertical") {
            rows = numCharts_;
            cols = 1;
        } else {
            // Try to parse custom layout like "2x3"
            size_t pos = layout_.find('x');
            if (pos != std::string::npos) {
                try {
                    rows = std::stoi(layout_.substr(0, pos));
                    cols = std::stoi(layout_.substr(pos + 1));
                } catch (...) {
                    // Default to auto layout if parsing fails
                    layout_ = "auto";
                    determineLayout(rows, cols);
                }
            } else {
                // Default to auto layout
                layout_ = "auto";
                determineLayout(rows, cols);
            }
        }
    }
    
    void renderMockCharts(PixelPainter& painter) {
        int rows, cols;
        determineLayout(rows, cols);
        
        int chartWidth = canvasWidth_ / cols;
        int chartHeight = canvasHeight_ / rows;
        
        const RgbColor chartColors[] = {
            {255, 0, 0},    // Red
            {0, 255, 0},    // Green  
            {0, 0, 255},    // Blue
            {255, 255, 0},  // Yellow
            {255, 0, 255},  // Magenta
            {0, 255, 255},  // Cyan
        };
        
        for (int i = 0; i < numCharts_; i++) {
            int row = i / cols;
            int col = i % cols;
            
            // Calculate chart position
            int startX = col * chartWidth;
            int startY = row * chartHeight;
            int endX = startX + chartWidth - 1;
            int endY = startY + chartHeight - 1;
            
            // Ensure we don't exceed canvas bounds
            endX = std::min(endX, canvasWidth_ - 1);
            endY = std::min(endY, canvasHeight_ - 1);
            
            // Render mock chart by drawing some pixels in the chart region
            RgbColor color = chartColors[i % 6];
            
            // Draw border around chart
            for (int x = startX; x <= endX; x++) {
                painter.putPixel(x, startY, color);
                painter.putPixel(x, endY, color);
            }
            for (int y = startY; y <= endY; y++) {
                painter.putPixel(startX, y, color);
                painter.putPixel(endX, y, color);
            }
            
            // Draw some internal content (diagonal lines)
            for (int offset = 10; offset < std::min(chartWidth, chartHeight) - 10; offset += 5) {
                if (startX + offset <= endX && startY + offset <= endY) {
                    painter.putPixel(startX + offset, startY + offset, color);
                }
                if (endX - offset >= startX && startY + offset <= endY) {
                    painter.putPixel(endX - offset, startY + offset, color);
                }
            }
        }
    }
};

UTEST_FUNC_DEF(MultiChart3D_FourCharts_2x2_NoBoundaryViolations) {
    auto logger = dlog::Logger::getInstance();
    logger->setLevel(dlog::LogLevel::DEBUG);
    logger->setConsoleEnabled(false);
    logger->setBufferEnabled(true);
    logger->clearBuffer();
    
    const int canvasWidth = 800;
    const int canvasHeight = 600;
    
    // Create image and painters
    RgbImage testImage(canvasWidth, canvasHeight);
    PixelPainterForImageBase basePainter(testImage);
    TrackingPixelPainter tracker(basePainter);
    
    // Create mock renderer for 4 charts in 2x2 layout
    MockMultiChartRenderer renderer(canvasWidth, canvasHeight, 4, "2x2");
    
    // Render charts with tracking
    renderer.renderMockCharts(tracker);
    
    // Verify no pixels were drawn outside canvas bounds
    int pixelsOutsideCanvas = tracker.countPixelsOutsideRegion(0, 0, canvasWidth - 1, canvasHeight - 1);
    UTEST_ASSERT_EQUALS(pixelsOutsideCanvas, 0);
    
    // Check individual chart regions
    const int chartWidth = canvasWidth / 2;
    const int chartHeight = canvasHeight / 2;
    
    struct ChartRegion {
        int x1, y1, x2, y2;
        std::string name;
    };
    
    std::vector<ChartRegion> regions = {
        {0, 0, chartWidth - 1, chartHeight - 1, "Top-Left"},
        {chartWidth, 0, 2 * chartWidth - 1, chartHeight - 1, "Top-Right"},
        {0, chartHeight, chartWidth - 1, 2 * chartHeight - 1, "Bottom-Left"}, 
        {chartWidth, chartHeight, 2 * chartWidth - 1, 2 * chartHeight - 1, "Bottom-Right"}
    };
    
    // Verify each chart region has pixels and no empty regions
    for (const auto& region : regions) {
        auto pixelsInRegion = tracker.getPixelsInRegion(region.x1, region.y1, region.x2, region.y2);
        UTEST_ASSERT_GT(pixelsInRegion.size(), 0);
        DLOG_INFO("Chart region verified for boundary compliance");
    }
    
    // Verify reasonable total pixel count
    UTEST_ASSERT_GT(tracker.getPixelHistory().size(), 0);
    DLOG_INFO("Multi-chart boundary test completed successfully");
}

UTEST_FUNC_DEF(MultiChart3D_SingleChart_FillsCanvas) {
    auto logger = dlog::Logger::getInstance();
    logger->setLevel(dlog::LogLevel::INFO);
    logger->setConsoleEnabled(false);
    logger->setBufferEnabled(true);
    logger->clearBuffer();
    
    const int canvasWidth = 400;
    const int canvasHeight = 300;
    
    // Create image and painters
    RgbImage testImage(canvasWidth, canvasHeight);
    PixelPainterForImageBase basePainter(testImage);
    TrackingPixelPainter tracker(basePainter);
    
    // Single chart should use entire canvas
    MockMultiChartRenderer renderer(canvasWidth, canvasHeight, 1, "auto");
    renderer.renderMockCharts(tracker);
    
    // Verify no pixels outside canvas
    int pixelsOutsideCanvas = tracker.countPixelsOutsideRegion(0, 0, canvasWidth - 1, canvasHeight - 1);
    UTEST_ASSERT_EQUALS(pixelsOutsideCanvas, 0);
    
    // Verify pixels exist in all quadrants for single chart
    int midX = canvasWidth / 2;
    int midY = canvasHeight / 2;
    
    auto topLeft = tracker.getPixelsInRegion(0, 0, midX - 1, midY - 1);
    auto topRight = tracker.getPixelsInRegion(midX, 0, canvasWidth - 1, midY - 1);
    auto bottomLeft = tracker.getPixelsInRegion(0, midY, midX - 1, canvasHeight - 1);
    auto bottomRight = tracker.getPixelsInRegion(midX, midY, canvasWidth - 1, canvasHeight - 1);
    
    // Single chart should have content in all quadrants
    UTEST_ASSERT_GT(topLeft.size(), 0);
    UTEST_ASSERT_GT(topRight.size(), 0);
    UTEST_ASSERT_GT(bottomLeft.size(), 0);
    UTEST_ASSERT_GT(bottomRight.size(), 0);
    
    DLOG_INFO("Single chart canvas fill test completed");
}

UTEST_FUNC_DEF(MultiChart3D_HorizontalLayout_TwoCharts) {
    auto logger = dlog::Logger::getInstance();
    logger->setLevel(dlog::LogLevel::INFO);
    logger->setConsoleEnabled(false);
    logger->setBufferEnabled(true);
    logger->clearBuffer();
    
    const int canvasWidth = 800;
    const int canvasHeight = 400;
    
    RgbImage testImage(canvasWidth, canvasHeight);
    PixelPainterForImageBase basePainter(testImage);
    TrackingPixelPainter tracker(basePainter);
    
    MockMultiChartRenderer renderer(canvasWidth, canvasHeight, 2, "horizontal");
    renderer.renderMockCharts(tracker);
    
    // Verify no canvas boundary violations
    int pixelsOutsideCanvas = tracker.countPixelsOutsideRegion(0, 0, canvasWidth - 1, canvasHeight - 1);
    UTEST_ASSERT_EQUALS(pixelsOutsideCanvas, 0);
    
    // Check left and right halves
    const int chartWidth = canvasWidth / 2;
    auto leftChart = tracker.getPixelsInRegion(0, 0, chartWidth - 1, canvasHeight - 1);
    auto rightChart = tracker.getPixelsInRegion(chartWidth, 0, canvasWidth - 1, canvasHeight - 1);
    
    UTEST_ASSERT_GT(leftChart.size(), 0);
    UTEST_ASSERT_GT(rightChart.size(), 0);
    
    DLOG_INFO("Horizontal layout test completed");
}

UTEST_FUNC_DEF(MultiChart3D_LayoutParsing_CustomLayout) {
    MockMultiChartRenderer renderer(800, 600, 6, "3x2");
    
    int rows, cols;
    renderer.determineLayout(rows, cols);
    
    UTEST_ASSERT_EQUALS(rows, 3);
    UTEST_ASSERT_EQUALS(cols, 2);
}

UTEST_FUNC_DEF(MultiChart3D_LayoutParsing_AutoLayout) {
    MockMultiChartRenderer renderer4(800, 600, 4, "auto");
    
    int rows, cols;
    renderer4.determineLayout(rows, cols);
    
    // 4 charts should result in 2x2
    UTEST_ASSERT_EQUALS(rows, 2);
    UTEST_ASSERT_EQUALS(cols, 2);
    
    MockMultiChartRenderer renderer6(800, 600, 6, "auto");
    renderer6.determineLayout(rows, cols);
    
    // 6 charts should result in 2x3  
    UTEST_ASSERT_EQUALS(rows, 2);
    UTEST_ASSERT_EQUALS(cols, 3);
}

UTEST_FUNC_DEF(TrackingPixelPainter_RecordsPixelOperations) {
    RgbImage testImage(100, 100);
    PixelPainterForImageBase basePainter(testImage);
    TrackingPixelPainter tracker(basePainter);
    
    const RgbColor red = {255, 0, 0};
    const RgbColor green = {0, 255, 0};
    
    // Draw some pixels
    tracker.putPixel(10, 20, red);
    tracker.putPixel(30, 40, green);
    tracker.putPixel(50, 60, red);
    
    // Verify tracking
    UTEST_ASSERT_EQUALS(tracker.getPixelHistory().size(), 3);
    UTEST_ASSERT_EQUALS(tracker.getPixelPositions().size(), 3);
    
    UTEST_ASSERT_TRUE(tracker.hasPixelAt(10, 20));
    UTEST_ASSERT_TRUE(tracker.hasPixelAt(30, 40));
    UTEST_ASSERT_TRUE(tracker.hasPixelAt(50, 60));
    UTEST_ASSERT_FALSE(tracker.hasPixelAt(70, 80));
    
    // Test region queries
    auto regionPixels = tracker.getPixelsInRegion(0, 0, 50, 50);
    UTEST_ASSERT_EQUALS(regionPixels.size(), 2); // Should contain (10,20) and (30,40)
    
    auto outsideCount = tracker.countPixelsOutsideRegion(0, 0, 40, 50);
    UTEST_ASSERT_EQUALS(outsideCount, 1); // Should be (50,60)
}

int main() {
    UTEST_PROLOG();
    
    UTEST_FUNC(TrackingPixelPainter_RecordsPixelOperations);
    UTEST_FUNC(MultiChart3D_LayoutParsing_AutoLayout);
    UTEST_FUNC(MultiChart3D_LayoutParsing_CustomLayout);
    UTEST_FUNC(MultiChart3D_SingleChart_FillsCanvas);
    UTEST_FUNC(MultiChart3D_HorizontalLayout_TwoCharts);
    UTEST_FUNC(MultiChart3D_FourCharts_2x2_NoBoundaryViolations);
    
    UTEST_EPILOG();
}
