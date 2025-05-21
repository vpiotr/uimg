#pragma once

#include "uimg/images/rgb_image.h"
#include "uimg/painters/painter_for_rgb_image.h"
#include "uimg/fonts/bdf_font.h"
#include "uimg/fonts/painter_for_bdf_font.h"
#include "uimg/utils/color_utils.h"
#include "uimg/base/structs.h" // For PointF, Rect

#include <vector>
#include <string>
#include <functional> // Not strictly needed with current design, but good for future extensions

// Structure to hold data for a single series on the chart
struct SeriesData {
    std::vector<PointF> points; // Data points (x, y)
    RgbColor color;             // Color of the line for this series
    std::string name;           // Name of the series for the legend
};

class LineChartDemo {
public:
    /**
     * @brief Constructor for the LineChartDemo.
     * @param width Width of the output image.
     * @param height Height of the output image.
     * @param fontPath Path to the BDF font file.
     * @param lineThickness The thickness of the lines in the chart (default: 1).
     */
    LineChartDemo(int width, int height, const std::string& fontPath, float lineThickness = 1.0f);

    /**
     * @brief Generates the complete chart image with two sub-charts and saves it.
     * @param outputPath Path to save the generated PPM image.
     */
    void generateChart(const std::string& outputPath);

private:
    /**
     * @brief Draws a complete chart in a specified rectangular area of the image.
     * @param chartRect The rectangle defining the area for this specific chart.
     * @param title The title of the chart.
     * @param seriesSet A vector of series data to plot.
     * @param xMinVal Minimum value for the X-axis.
     * @param xMaxVal Maximum value for the X-axis.
     * @param yMinVal Minimum value for the Y-axis.
     * @param yMaxVal Maximum value for the Y-axis.
     * @param xLabel Label for the X-axis.
     * @param yLabel Label for the Y-axis.
     */
    void drawChartArea(const Rect& chartRect, const std::string& title,
                       const std::vector<SeriesData>& seriesSet,
                       float xMinVal, float xMaxVal, float yMinVal, float yMaxVal,
                       const std::string& xLabel, const std::string& yLabel);

    /**
     * @brief Draws the X and Y axes, including ticks, labels, and grid lines.
     * @param plotArea The rectangle defining the plotting area (inside chartRect, excluding margins).
     * @param xMin Minimum value of the X-axis.
     * @param xMax Maximum value of the X-axis.
     * @param yMin Minimum value of the Y-axis.
     * @param yMax Maximum value of the Y-axis.
     */
    void drawAxes(const Rect& plotArea, float xMin, float xMax, float yMin, float yMax);

    /**
     * @brief Draws all data series lines on the chart.
     * @param plotArea The rectangle defining the plotting area.
     * @param seriesSet Vector of series data to plot.
     * @param xMin Minimum value of the X-axis.
     * @param xMax Maximum value of the X-axis.
     * @param yMin Minimum value of the Y-axis.
     * @param yMax Maximum value of the Y-axis.
     */
    void drawSeries(const Rect& plotArea, const std::vector<SeriesData>& seriesSet,
                    float xMin, float xMax, float yMin, float yMax);

    /**
     * @brief Draws the legend for the data series.
     * @param chartRect The overall rectangle for the current chart (used for positioning).
     * @param plotArea The rectangle defining the plotting area.
     * @param seriesSet Vector of series data for which to draw legend entries.
     */
    void drawLegend(const Rect& chartRect, const Rect& plotArea, const std::vector<SeriesData>& seriesSet);

    /**
     * @brief Converts world coordinates (data values) to screen coordinates (pixels).
     * @param x X-value in world coordinates.
     * @param y Y-value in world coordinates.
     * @param plotArea The rectangle defining the plotting area.
     * @param xMin Minimum world X-value visible on the plot.
     * @param xMax Maximum world X-value visible on the plot.
     * @param yMin Minimum world Y-value visible on the plot.
     * @param yMax Maximum world Y-value visible on the plot.
     * @return PointF containing screen x and y coordinates.
     */
    PointF worldToScreen(float x, float y, const Rect& plotArea,
                         float xMin, float xMax, float yMin, float yMax);

    // Image and drawing utilities
    RgbImage image_;
    PixelPainterForRgbImage pixelPainter_; // For general pixel operations if needed by text painter
    float lineThickness_; // Thickness of lines in the chart
    LinePainterForRgbImage linePainter_;   // For drawing lines (axes, series)
    RectPainterForRgbImage rectPainter_;   // For drawing rectangles (legend color boxes)
    
    uimg::BdfFont font_;
    uimg::TextPainterForBdfFont textPainter_;    // For drawing all text (title, labels, legend)

    // Style properties
    RgbColor backgroundColor_;
    RgbColor axisColor_;
    RgbColor textColor_;
    RgbColor gridColor_;

    int imageWidth_;
    int imageHeight_;
};
