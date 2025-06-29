#pragma once

#include "uimg/charts/chart.h"
#include "uimg/images/rgb_image.h"
#include "uimg/painters/painter_for_rgb_image.h"
#include "uimg/painters/painter_for_pixels.h"
#include "uimg/painters/antialiased_painter_for_pixels.h"
#include "uimg/fonts/bdf_font.h"
#include "uimg/fonts/painter_for_bdf_font.h"
#include "uimg/images/ppm_image.h"
#include "uimg/utils/cast.h"

#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <stdexcept>

namespace uimg {
namespace charts {

/**
 * Helper class for drawing thick lines
 */
class ThickLinePainter {
public:
    ThickLinePainter(RgbImage& image, float thickness) 
        : image_(image), 
          pixelPainter_(image), 
          thickLinePainter_(pixelPainter_, thickness) {}
    
    void drawLine(int x1, int y1, int x2, int y2, const RgbColor& color) {
        thickLinePainter_.drawLine(UNSIGNED_CAST(unsigned int, x1), UNSIGNED_CAST(unsigned int, y1), UNSIGNED_CAST(unsigned int, x2), UNSIGNED_CAST(unsigned int, y2), color);
    }
    
private:
    RgbImage& image_;
    PixelPainterForRgbImage pixelPainter_;
    ThickLinePainterForPixels thickLinePainter_;
};

/**
 * Helper class for drawing anti-aliased thick lines
 */
class AntiAliasedThickLinePainter {
public:
    AntiAliasedThickLinePainter(RgbImage& image, float thickness) 
        : image_(image), 
          pixelPainter_(image), 
          antiAliasedThickLinePainter_(pixelPainter_, thickness) {}
    
    void drawLine(int x1, int y1, int x2, int y2, const RgbColor& color) {
        antiAliasedThickLinePainter_.drawLine(UNSIGNED_CAST(unsigned int, x1), UNSIGNED_CAST(unsigned int, y1), UNSIGNED_CAST(unsigned int, x2), UNSIGNED_CAST(unsigned int, y2), color);
    }
    
private:
    RgbImage& image_;
    PixelPainterForRgbImage pixelPainter_;
    AntiAliasedThickLinePainterForPixels antiAliasedThickLinePainter_;
};

/**
 * @brief Layout configuration for chart placement
 */
struct ChartLayout {
    Rect rect;              // Rectangle defining the chart area
    bool autoLayout = false; // Whether to auto-layout this chart
    
    // Create a layout with an explicit rectangle
    static ChartLayout create(const Rect& rect) {
        ChartLayout layout;
        layout.rect = rect;
        return layout;
    }
    
    // Create a layout with explicit coordinates
    static ChartLayout create(int x1, int y1, int x2, int y2) {
        ChartLayout layout;
        layout.rect = Rect::make_rect(x1, y1, x2, y2);
        return layout;
    }
    
    // Create an auto-layout that will be positioned by the renderer
    static ChartLayout createAuto() {
        ChartLayout layout;
        layout.autoLayout = true;
        return layout;
    }
};

/**
 * @brief The ChartRenderer renders multiple charts onto a single canvas
 */
class ChartRenderer {
public:
    /**
     * @brief Creates a chart renderer with specified image dimensions
     * @param width Width of the output image
     * @param height Height of the output image
     * @param fontPath Path to the BDF font file
     * @param useAntiAliasing Enable anti-aliased line rendering
     */
    ChartRenderer(int width, int height, const std::string& fontPath, bool useAntiAliasing = false) 
        : image_(UNSIGNED_CAST(unsigned int, width), UNSIGNED_CAST(unsigned int, height)),
          pixelPainter_(image_),
          linePainter_(image_),
          rectPainter_(image_),
          font_(),
          textPainter_(pixelPainter_, Point(width, height)),
          imageWidth_(width),
          imageHeight_(height),
          useAntiAliasing_(useAntiAliasing) {
        
        // Load the font
        std::ifstream fontFile(fontPath, std::ios::binary);
        if (!fontFile.is_open()) {
            throw std::runtime_error("Failed to open font file: " + fontPath);
        }
        
        BdfFontLoader loader;
        loader.load(fontFile, font_);
        textPainter_.setFont(&font_);
        
        // Initialize image with white background
        rectPainter_.drawFull(0, 0, UNSIGNED_CAST(unsigned int, imageWidth_ - 1), UNSIGNED_CAST(unsigned int, imageHeight_ - 1), RgbColor{255, 255, 255});
    }
    
    /**
     * @brief Add a chart to be rendered with a specific layout
     * @param chart The chart to render
     * @param layout The layout configuration for the chart
     */
    void addChart(const Chart& chart, const ChartLayout& layout) {
        charts_.push_back(chart);
        layouts_.push_back(layout);
    }
    
    /**
     * @brief Render all charts and save to a file
     * @param outputPath Path to the output image file
     */
    void renderToFile(const std::string& outputPath) {
        // Process auto-layouts
        processAutoLayouts();
        
        // Render each chart
        for (size_t i = 0; i < charts_.size(); ++i) {
            renderChart(charts_[i], layouts_[i].rect);
        }
        
        // Save image to file
        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Failed to open output file: " + outputPath);
        }
        
        PpmWriterForRgbImage writer(outFile);
        writer.writeImage(image_);
    }
    
    /**
     * @brief Set the background color for the entire image
     */
    void setBackgroundColor(const RgbColor& color) {
        rectPainter_.drawFull(0, 0, UNSIGNED_CAST(unsigned int, imageWidth_ - 1), UNSIGNED_CAST(unsigned int, imageHeight_ - 1), color);
    }
    
    /**
     * @brief Get the image being rendered
     * @return Reference to the image
     */
    RgbImage& getImage() {
        return image_;
    }

private:
    // Process auto-layouts to position charts automatically
    void processAutoLayouts() {
        int autoCount = 0;
        for (const auto& layout : layouts_) {
            if (layout.autoLayout) autoCount++;
        }
        
        if (autoCount == 0) return;
        
        // Determine layout based on number of auto charts
        if (autoCount == 1) {
            // Single chart takes full screen with margins
            for (auto& layout : layouts_) {
                if (layout.autoLayout) {
                    layout.rect = Rect::make_rect(20, 20, imageWidth_ - 20, imageHeight_ - 20);
                }
            }
        } else if (autoCount == 2) {
            // Two charts side by side
            int i = 0;
            for (auto& layout : layouts_) {
                if (layout.autoLayout) {
                    if (i == 0) {
                        layout.rect = Rect::make_rect(10, 10, imageWidth_ / 2 - 15, imageHeight_ - 20);
                    } else {
                        layout.rect = Rect::make_rect(imageWidth_ / 2 + 5, 10, imageWidth_ - 15, imageHeight_ - 20);
                    }
                    i++;
                }
            }
        } else if (autoCount == 4) {
            // Four charts in a 2x2 grid
            int i = 0;
            for (auto& layout : layouts_) {
                if (layout.autoLayout) {
                    int col = i % 2;
                    int row = i / 2;
                    int x1 = 10 + col * (imageWidth_ / 2);
                    int y1 = 10 + row * (imageHeight_ / 2);
                    int x2 = (col + 1) * (imageWidth_ / 2) - 10;
                    int y2 = (row + 1) * (imageHeight_ / 2) - 10;
                    layout.rect = Rect::make_rect(x1, y1, x2, y2);
                    i++;
                }
            }
        } else {
            // Default to a simple grid layout
            int cols = static_cast<int>(std::sqrt(autoCount));
            int rows = (autoCount + cols - 1) / cols; // Ceiling division
            
            int i = 0;
            for (auto& layout : layouts_) {
                if (layout.autoLayout) {
                    int col = i % cols;
                    int row = i / cols;
                    int x1 = 10 + col * (imageWidth_ / cols);
                    int y1 = 10 + row * (imageHeight_ / rows);
                    int x2 = (col + 1) * (imageWidth_ / cols) - 10;
                    int y2 = (row + 1) * (imageHeight_ / rows) - 10;
                    layout.rect = Rect::make_rect(x1, y1, x2, y2);
                    i++;
                }
            }
        }
    }
    
    // Convert world coordinates to screen coordinates
    PointF worldToScreen(float x, float y, const Rect& plotArea, float xMin, float xMax, float yMin, float yMax) {
        if (xMax <= xMin || yMax <= yMin) {
            return {static_cast<float>(plotArea.x1), static_cast<float>(plotArea.y1)};
        }

        float screenX = static_cast<float>(plotArea.x1) + (x - xMin) / (xMax - xMin) * static_cast<float>(plotArea.width());
        float screenY = static_cast<float>(plotArea.y2) - (y - yMin) / (yMax - yMin) * static_cast<float>(plotArea.height());

        return {screenX, screenY};
    }
    
    // Draw the chart legend
    void drawLegend(const Chart& chart, [[maybe_unused]] const Rect& chartRect, const Rect& legendArea) {
        const ChartStyle& style = chart.getStyle();
        const std::vector<Series>& seriesSet = chart.getSeries();
        
        int itemHeight = 25;
        int colorBoxSize = 10;
        int textPadding = 8;
        
        int fontHeight = 12;
        int fontBaseline = 9;
        
        for (size_t i = 0; i < seriesSet.size(); ++i) {
            const auto& series = seriesSet[i];
            int currentY = legendArea.y1 + static_cast<int>(i) * itemHeight;
            
            // Calculate vertical positions for perfect alignment
            int colorBoxY = currentY + (itemHeight - colorBoxSize) / 2;
            int textY = colorBoxY + (colorBoxSize / 2) - (fontHeight / 2) + fontBaseline;
            
            // Draw color box
            rectPainter_.drawFull(UNSIGNED_CAST(unsigned int, legendArea.x1), UNSIGNED_CAST(unsigned int, colorBoxY),
                                UNSIGNED_CAST(unsigned int, legendArea.x1 + colorBoxSize), UNSIGNED_CAST(unsigned int, colorBoxY + colorBoxSize),
                                series.getStyle().color);
            // Draw series name
            textPainter_.drawText(UNSIGNED_CAST(unsigned int, legendArea.x1 + colorBoxSize + textPadding), UNSIGNED_CAST(unsigned int, textY),
                                series.getStyle().name, style.textColor);
        }
    }
    
    // Draw chart axes
    void drawAxes(const Chart& chart, const Rect& plotArea) {
        const ChartStyle& style = chart.getStyle();
        const AxisConfig& xAxis = chart.getXAxis();
        const AxisConfig& yAxis = chart.getYAxis();
        
        float xMin = xAxis.min;
        float xMax = xAxis.max;
        float yMin = yAxis.min;
        float yMax = yAxis.max;
        
        // Draw axes lines
        linePainter_.drawLine(UNSIGNED_CAST(unsigned int, plotArea.x1), UNSIGNED_CAST(unsigned int, plotArea.y2), UNSIGNED_CAST(unsigned int, plotArea.x2), UNSIGNED_CAST(unsigned int, plotArea.y2), style.axisColor); // X-axis
        linePainter_.drawLine(UNSIGNED_CAST(unsigned int, plotArea.x1), UNSIGNED_CAST(unsigned int, plotArea.y1), UNSIGNED_CAST(unsigned int, plotArea.x1), UNSIGNED_CAST(unsigned int, plotArea.y2), style.axisColor); // Y-axis

        // Draw X-axis ticks and grid
        for (int i = 0; i <= style.numXTicks; ++i) {
            float value = xMin + (xMax - xMin) * static_cast<float>(i) / static_cast<float>(style.numXTicks);
            float xPos = static_cast<float>(plotArea.x1) + static_cast<float>(plotArea.width()) * static_cast<float>(i) / static_cast<float>(style.numXTicks);
            
            // Draw tick
            linePainter_.drawLine(UNSIGNED_CAST(unsigned int, xPos), UNSIGNED_CAST(unsigned int, plotArea.y2), UNSIGNED_CAST(unsigned int, xPos), UNSIGNED_CAST(unsigned int, plotArea.y2 + 5), style.axisColor);
            
            // Draw grid line
            if (style.showGrid) {
                linePainter_.drawLine(UNSIGNED_CAST(unsigned int, xPos), UNSIGNED_CAST(unsigned int, plotArea.y1), UNSIGNED_CAST(unsigned int, xPos), UNSIGNED_CAST(unsigned int, plotArea.y2), style.gridColor);
            }
            
            // Draw tick label
            char buffer[20];
            sprintf(buffer, "%.1f", value);
            std::string label = buffer;
            
            unsigned int labelWidth = textPainter_.textWidth(label);
            textPainter_.drawText(UNSIGNED_CAST(unsigned int, xPos) - labelWidth / 2, UNSIGNED_CAST(unsigned int, plotArea.y2 + 15), label, style.textColor);
        }

        // Draw Y-axis ticks and grid
        for (int i = 0; i <= style.numYTicks; ++i) {
            float value = yMin + (yMax - yMin) * static_cast<float>(style.numYTicks - i) / static_cast<float>(style.numYTicks);
            float yPos = static_cast<float>(plotArea.y1) + static_cast<float>(plotArea.height()) * static_cast<float>(i) / static_cast<float>(style.numYTicks);
            
            // Draw tick
            linePainter_.drawLine(UNSIGNED_CAST(unsigned int, plotArea.x1 - 5), UNSIGNED_CAST(unsigned int, yPos), UNSIGNED_CAST(unsigned int, plotArea.x1), UNSIGNED_CAST(unsigned int, yPos), style.axisColor);
            
            // Draw grid line
            if (style.showGrid) {
                linePainter_.drawLine(UNSIGNED_CAST(unsigned int, plotArea.x1), UNSIGNED_CAST(unsigned int, yPos), UNSIGNED_CAST(unsigned int, plotArea.x2), UNSIGNED_CAST(unsigned int, yPos), style.gridColor);
            }
            
            // Draw tick label
            char buffer[20];
            sprintf(buffer, "%.1f", value);
            std::string label = buffer;
            
            unsigned int labelWidth = textPainter_.textWidth(label);
            textPainter_.drawText(UNSIGNED_CAST(unsigned int, plotArea.x1) - labelWidth - 25, UNSIGNED_CAST(unsigned int, yPos), label, style.textColor);
        }
    }
    
    // Draw chart series data
    void drawSeries(const Chart& chart, const Rect& plotArea) {
        const std::vector<Series>& seriesSet = chart.getSeries();
        const AxisConfig& xAxis = chart.getXAxis();
        const AxisConfig& yAxis = chart.getYAxis();
        
        float xMin = xAxis.min;
        float xMax = xAxis.max;
        float yMin = yAxis.min;
        float yMax = yAxis.max;
        
        for (const auto& series : seriesSet) {
            const auto& points = series.getPoints();
            if (points.size() < 2) continue;

            float lineThickness = series.getStyle().lineThickness;
            
            // Use appropriate line painter based on thickness
            if (lineThickness <= 1.0f) {
                if (useAntiAliasing_) {
                    // Anti-aliased line painter for thin lines
                    AntiAliasedLinePainterForPixels antiAliasedPainter(pixelPainter_);
                    
                    for (size_t i = 0; i < points.size() - 1; ++i) {
                        const auto& p1 = points[i];
                        const auto& p2 = points[i+1];
                        
                        PointF p1_screen = worldToScreen(p1.x, p1.y, plotArea, xMin, xMax, yMin, yMax);
                        PointF p2_screen = worldToScreen(p2.x, p2.y, plotArea, xMin, xMax, yMin, yMax);
                        
                        antiAliasedPainter.drawLine(UNSIGNED_CAST(unsigned int, p1_screen.x), UNSIGNED_CAST(unsigned int, p1_screen.y),
                                                    UNSIGNED_CAST(unsigned int, p2_screen.x), UNSIGNED_CAST(unsigned int, p2_screen.y), 
                                                    series.getStyle().color);
                    }
                } else {
                    // Standard line painter for thin lines
                    for (size_t i = 0; i < points.size() - 1; ++i) {
                        const auto& p1 = points[i];
                        const auto& p2 = points[i+1];
                        
                        PointF p1_screen = worldToScreen(p1.x, p1.y, plotArea, xMin, xMax, yMin, yMax);
                        PointF p2_screen = worldToScreen(p2.x, p2.y, plotArea, xMin, xMax, yMin, yMax);
                        
                        linePainter_.drawLine(UNSIGNED_CAST(unsigned int, p1_screen.x), UNSIGNED_CAST(unsigned int, p1_screen.y),
                                            UNSIGNED_CAST(unsigned int, p2_screen.x), UNSIGNED_CAST(unsigned int, p2_screen.y), 
                                            series.getStyle().color);
                    }
                }
            } else {
                // Choose between standard or anti-aliased thick line painter
                if (useAntiAliasing_) {
                    // Anti-aliased thick line painter
                    AntiAliasedThickLinePainter thickPainter(image_, lineThickness);
                    
                    for (size_t i = 0; i < points.size() - 1; ++i) {
                        const auto& p1 = points[i];
                        const auto& p2 = points[i+1];
                        
                        PointF p1_screen = worldToScreen(p1.x, p1.y, plotArea, xMin, xMax, yMin, yMax);
                        PointF p2_screen = worldToScreen(p2.x, p2.y, plotArea, xMin, xMax, yMin, yMax);
                        
                        thickPainter.drawLine(static_cast<int>(p1_screen.x), static_cast<int>(p1_screen.y),
                                            static_cast<int>(p2_screen.x), static_cast<int>(p2_screen.y),
                                            series.getStyle().color);
                    }
                } else {
                    // Standard thick line painter
                    ThickLinePainter thickPainter(image_, lineThickness);
                    
                    for (size_t i = 0; i < points.size() - 1; ++i) {
                        const auto& p1 = points[i];
                        const auto& p2 = points[i+1];
                        
                        PointF p1_screen = worldToScreen(p1.x, p1.y, plotArea, xMin, xMax, yMin, yMax);
                        PointF p2_screen = worldToScreen(p2.x, p2.y, plotArea, xMin, xMax, yMin, yMax);
                        
                        thickPainter.drawLine(static_cast<int>(p1_screen.x), static_cast<int>(p1_screen.y),
                                            static_cast<int>(p2_screen.x), static_cast<int>(p2_screen.y),
                                            series.getStyle().color);
                    }
                }
            }
        }
    }
    
    // Render a single chart
    void renderChart(const Chart& chart, const Rect& chartRect) {
        const ChartStyle& style = chart.getStyle();
        
        // Apply chart background color
        rectPainter_.drawFull(UNSIGNED_CAST(unsigned int, chartRect.x1), UNSIGNED_CAST(unsigned int, chartRect.y1), UNSIGNED_CAST(unsigned int, chartRect.x2), UNSIGNED_CAST(unsigned int, chartRect.y2), style.backgroundColor);
        
        // Calculate plot area
        Rect plotArea = Rect::make_rect(
            chartRect.x1 + style.marginLeft,
            chartRect.y1 + style.marginTop,
            chartRect.x2 - style.marginRight,
            chartRect.y2 - style.marginBottom
        );
        
        // Draw chart title
        unsigned int titleWidth = textPainter_.textWidth(chart.getTitle());
        int titleX = chartRect.x1 + static_cast<int>((UNSIGNED_CAST(unsigned int, chartRect.width()) - titleWidth) / 2);
        int titleY = chartRect.y1 + 20;
        textPainter_.drawText(UNSIGNED_CAST(unsigned int, titleX), UNSIGNED_CAST(unsigned int, titleY), chart.getTitle(), style.textColor);
        
        // Draw axes
        drawAxes(chart, plotArea);
        
        // Draw data series
        drawSeries(chart, plotArea);
        
        // Draw X-axis label
        unsigned int xLabelWidth = textPainter_.textWidth(chart.getXAxis().label);
        int xLabelX = plotArea.x1 + static_cast<int>((UNSIGNED_CAST(unsigned int, plotArea.width()) - xLabelWidth) / 2);
        int xLabelY = plotArea.y2 + 30;
        textPainter_.drawText(UNSIGNED_CAST(unsigned int, xLabelX), UNSIGNED_CAST(unsigned int, xLabelY), chart.getXAxis().label, style.textColor);
        
        // Draw Y-axis label
        const std::string& yLabel = chart.getYAxis().label;
        int yLabelX = chartRect.x1 + 15;
        
        // Calculate position to center the label vertically on the y-axis
        int totalYLabelHeight = static_cast<int>(yLabel.length()) * 12; // 12 pixels per character
        int yLabelY = plotArea.y1 + (plotArea.height() - totalYLabelHeight) / 2;
        
        // Draw Y-axis label vertically
        for (size_t i = 0; i < yLabel.length(); ++i) {
            textPainter_.drawText(UNSIGNED_CAST(unsigned int, yLabelX), UNSIGNED_CAST(unsigned int, yLabelY + static_cast<int>(i) * 12), std::string(1, yLabel[i]), style.textColor);
        }
        
        // Draw legend
        Rect legendArea = Rect::make_rect(
            plotArea.x2 - style.legendWidth,
            plotArea.y1,
            plotArea.x2,
            plotArea.y1 + static_cast<int>(chart.getSeries().size()) * 25
        );
        drawLegend(chart, chartRect, legendArea);
    }
    
    RgbImage image_;
    PixelPainterForRgbImage pixelPainter_;
    LinePainterForRgbImage linePainter_;
    RectPainterForRgbImage rectPainter_;
    
    BdfFont font_;
    TextPainterForBdfFont textPainter_;
    
    int imageWidth_;
    int imageHeight_;
    bool useAntiAliasing_;
    
    std::vector<Chart> charts_;
    std::vector<ChartLayout> layouts_;
};

} // namespace charts
} // namespace uimg
