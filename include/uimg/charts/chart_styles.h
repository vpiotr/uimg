#pragma once

#include "uimg/base/structs.h"
#include <string>

namespace uimg {
namespace charts {

/**
 * @brief Style properties for chart series
 */
struct SeriesStyle {
    RgbColor color;                // Color of the line
    float lineThickness = 1.0f;    // Thickness of the line
    std::string name;              // Name for the legend
    
    // Factory method for creating default styles with different colors
    static SeriesStyle createDefault(const RgbColor& color, const std::string& name) {
        SeriesStyle style;
        style.color = color;
        style.name = name;
        return style;
    }
};

/**
 * @brief Style properties for the overall chart
 */
struct ChartStyle {
    // Colors
    RgbColor backgroundColor{255, 255, 255};  // Background color (white default)
    RgbColor axisColor{0, 0, 0};              // Color of axes (black default)
    RgbColor textColor{0, 0, 0};              // Text color (black default)
    RgbColor gridColor{200, 200, 200};        // Grid color (light gray default)
    
    // Margins
    int marginTop = 40;        // Space for title
    int marginBottom = 50;     // Space for X-axis labels and legend
    int marginLeft = 80;       // Space for Y-axis labels
    int marginRight = 20;      // Right padding
    int legendWidth = 120;     // Width reserved for legend
    
    // Grid options
    bool showGrid = true;      // Whether to show grid lines
    int numXTicks = 10;        // Number of X-axis ticks/grid lines
    int numYTicks = 10;        // Number of Y-axis ticks/grid lines

    // Factory method for creating a default style
    static ChartStyle createDefault() {
        return ChartStyle{};
    }
    
    // Factory method for creating a dark theme style
    static ChartStyle createDarkTheme() {
        ChartStyle style;
        style.backgroundColor = {40, 40, 40};     // Dark background
        style.axisColor = {200, 200, 200};        // Light gray axes
        style.textColor = {220, 220, 220};        // Light gray text
        style.gridColor = {80, 80, 80};           // Dark gray grid
        return style;
    }
};

/**
 * @brief Axis configuration for a chart
 */
struct AxisConfig {
    float min;                 // Minimum value on axis
    float max;                 // Maximum value on axis
    std::string label;         // Axis label text
    bool autoScale = false;    // Whether to auto-scale based on data
    
    // Create a basic axis configuration
    static AxisConfig create(float min, float max, const std::string& label) {
        AxisConfig config;
        config.min = min;
        config.max = max;
        config.label = label;
        return config;
    }
    
    // Create an auto-scaling axis configuration
    static AxisConfig createAutoScale(const std::string& label) {
        AxisConfig config;
        config.min = 0;
        config.max = 0;
        config.label = label;
        config.autoScale = true;
        return config;
    }
};

} // namespace charts
} // namespace uimg
