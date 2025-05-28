#pragma once

#include "uimg/base/structs.h"
#include "uimg/images/rgb_image.h"
#include "uimg/painters/painter_for_rgb_image.h"
#include "uimg/painters/painter_for_pixels.h"
#include "uimg/fonts/bdf_font.h"
#include "uimg/fonts/painter_for_bdf_font.h"
#include "uimg/charts/chart_styles.h"

#include <vector>
#include <functional>
#include <string>
#include <memory>

namespace uimg {
namespace charts {

/**
 * @brief Class representing a data point in a chart
 */
struct DataPoint {
    float x;
    float y;
    
    DataPoint(float x, float y) : x(x), y(y) {}
};

/**
 * @brief Class representing a series of data in a chart
 */
class Series {
public:
    Series(const std::string& name, const RgbColor& color, float lineThickness = 1.0f) 
        : style_({color, lineThickness, name}) {}
    
    Series(const SeriesStyle& style) : style_(style) {}

    // Add a single data point to the series
    void addPoint(float x, float y) {
        points_.push_back({x, y});
    }
    
    // Add multiple data points from vectors of coordinates
    void addPoints(const std::vector<float>& xValues, const std::vector<float>& yValues) {
        if (xValues.size() != yValues.size()) {
            throw std::invalid_argument("X and Y value arrays must have the same size");
        }
        
        for (size_t i = 0; i < xValues.size(); i++) {
            points_.push_back({xValues[i], yValues[i]});
        }
    }
    
    // Generate data points from a function
    void generatePoints(float start, float end, float step, std::function<float(float)> function) {
        points_.clear();
        for (float x = start; x <= end; x += step) {
            points_.push_back({x, function(x)});
        }
    }
    
    // Accessors
    const std::vector<DataPoint>& getPoints() const { return points_; }
    const SeriesStyle& getStyle() const { return style_; }
    SeriesStyle& getStyle() { return style_; }
    
private:
    std::vector<DataPoint> points_;
    SeriesStyle style_;
};

/**
 * @brief The Chart class represents a single 2D chart with multiple data series
 */
class Chart {
public:
    /**
     * @brief Create a chart with title and axis configurations
     * @param title Chart title
     * @param xAxis X-axis configuration
     * @param yAxis Y-axis configuration
     * @param style Chart style configuration
     */
    Chart(const std::string& title, 
          const AxisConfig& xAxis = AxisConfig::create(0, 10, "X-axis"),
          const AxisConfig& yAxis = AxisConfig::create(0, 10, "Y-axis"),
          const ChartStyle& style = ChartStyle::createDefault()) 
        : title_(title), xAxis_(xAxis), yAxis_(yAxis), style_(style) {}
    
    // Add a series to the chart
    void addSeries(const Series& series) {
        series_.push_back(series);
    }
    
    // Create and add a new series to the chart
    Series& createSeries(const std::string& name, const RgbColor& color, float lineThickness = 1.0f) {
        series_.emplace_back(name, color, lineThickness);
        return series_.back();
    }
    
    // Accessors and mutators
    const std::string& getTitle() const { return title_; }
    void setTitle(const std::string& title) { title_ = title; }
    
    const AxisConfig& getXAxis() const { return xAxis_; }
    AxisConfig& getXAxis() { return xAxis_; }
    
    const AxisConfig& getYAxis() const { return yAxis_; }
    AxisConfig& getYAxis() { return yAxis_; }
    
    const ChartStyle& getStyle() const { return style_; }
    ChartStyle& getStyle() { return style_; }
    
    const std::vector<Series>& getSeries() const { return series_; }
    
private:
    std::string title_;
    AxisConfig xAxis_;
    AxisConfig yAxis_;
    ChartStyle style_;
    std::vector<Series> series_;
};

} // namespace charts
} // namespace uimg
