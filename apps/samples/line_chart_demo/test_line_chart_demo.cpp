#include "samples/line_chart_demo/test_line_chart_demo.h"
#include <cmath> // For std::sin, std::cos
#include <stdexcept> // For std::runtime_error
#include <fstream>  // For std::ofstream
#include "uimg/images/ppm_image.h" // For PpmWriterForRgbImage

// Constructor
LineChartDemo::LineChartDemo(int width, int height, const std::string& fontPath)
    : image_(width, height),
      pixelPainter_(image_),
      linePainter_(image_),
      rectPainter_(image_),
      font_(),
      textPainter_(pixelPainter_, Point(width, height)),
      backgroundColor_{255, 255, 255}, // Default white background
      axisColor_{0, 0, 0},             // Default black axes
      textColor_{0, 0, 0},             // Default black text
      gridColor_{200, 200, 200},       // Default light gray grid
      imageWidth_(width),
      imageHeight_(height) {
    
    // Load the font from file
    std::ifstream fontFile(fontPath, std::ios::binary);
    if (!fontFile.is_open()) {
        throw std::runtime_error("Failed to open font file: " + fontPath);
    }
    
    uimg::BdfFontLoader loader;
    loader.load(fontFile, font_);
    textPainter_.setFont(&font_);
    
    // Initialize image with background color
    rectPainter_.drawFull(0, 0, imageWidth_ - 1, imageHeight_ - 1, backgroundColor_);
}

// Generates the complete chart image
void LineChartDemo::generateChart(const std::string& outputPath) {
    // Define areas for two charts
    // For simplicity, let's divide the canvas into two horizontal sections
    // Top chart
    Rect chart1Rect = Rect::make_rect(10, 10, imageWidth_ / 2 - 15, imageHeight_ - 20);
    // Bottom chart (or right chart if preferred)
    Rect chart2Rect = Rect::make_rect(imageWidth_ / 2 + 5, 10, imageWidth_ - 15, imageHeight_ - 20);

    // Chart 1: Basic Math Function (e.g., y = x^2)
    std::vector<SeriesData> seriesSet1;
    SeriesData series1_1;
    series1_1.name = "y = x^2";
    series1_1.color = RgbColor{255, 0, 0}; // Red
    for (float x = -10.0f; x <= 10.0f; x += 0.5f) {
        series1_1.points.push_back({x, x * x});
    }
    seriesSet1.push_back(series1_1);
    drawChartArea(chart1Rect, "Chart 1: y = x^2", seriesSet1, -10.0f, 10.0f, 0.0f, 100.0f, "X-axis", "Y-axis");

    // Chart 2: Two Series (e.g., sin(x) and cos(x))
    std::vector<SeriesData> seriesSet2;
    SeriesData series2_1;
    series2_1.name = "y = sin(x)";
    series2_1.color = RgbColor{0, 0, 255}; // Blue
    for (float x = 0.0f; x <= 6.28f; x += 0.1f) { // 0 to 2*PI
        series2_1.points.push_back({x, std::sin(x)});
    }
    seriesSet2.push_back(series2_1);

    SeriesData series2_2;
    series2_2.name = "y = cos(x)";
    series2_2.color = RgbColor{0, 128, 0}; // Green
    for (float x = 0.0f; x <= 6.28f; x += 0.1f) { // 0 to 2*PI
        series2_2.points.push_back({x, std::cos(x)});
    }
    seriesSet2.push_back(series2_2);
    drawChartArea(chart2Rect, "Chart 2: sin(x) & cos(x)", seriesSet2, 0.0f, 6.28f, -1.0f, 1.0f, "Angle (radians)", "Value");

    // Save the image - using standard PPM writer as in other demos
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        throw std::runtime_error("Failed to open output file: " + outputPath);
    }
    
    PpmWriterForRgbImage writer(outFile);
    writer.writeImage(image_);
}

// Draws a complete chart in a specified rectangular area
void LineChartDemo::drawChartArea(const Rect& chartRect, const std::string& title,
                                  const std::vector<SeriesData>& seriesSet,
                                  float xMinVal, float xMaxVal, float yMinVal, float yMaxVal,
                                  const std::string& xLabel, const std::string& yLabel) {
    // Define plot area within chartRect (with margins for title, labels, legend)
    // These margins might need adjustment based on font size and text length
    int marginTop = 40; // For title
    int marginBottom = 50; // For X-axis label and ticks
    int marginLeft = 80; // For Y-axis label and ticks, increased to avoid overlapping
    int marginRight = 20; // Right padding
    int legendWidth = 120; // Approximate width for legend if placed inside/beside plotArea

    // Adjust marginRight if legend is to be placed to the right of the plot area but within chartRect
    // For now, let's assume legend is drawn separately or above/below plot area.
    // If legend is drawn to the side, plotArea width needs to be reduced.

    Rect plotArea = Rect::make_rect(
        chartRect.x1 + marginLeft, 
        chartRect.y1 + marginTop,
        chartRect.x2 - marginRight, 
        chartRect.y2 - marginBottom
    );

    // Draw chart title
    unsigned int titleWidth = textPainter_.textWidth(title);
    int titleX = chartRect.x1 + (chartRect.width() - titleWidth) / 2;
    int titleY = chartRect.y1 + 20; // Adjust Y based on font size
    textPainter_.drawText(titleX, titleY, title, textColor_);

    // Draw axes
    drawAxes(plotArea, xMinVal, xMaxVal, yMinVal, yMaxVal);

    // Draw series
    drawSeries(plotArea, seriesSet, xMinVal, xMaxVal, yMinVal, yMaxVal);

    // Draw X-axis label
    unsigned int xLabelWidth = textPainter_.textWidth(xLabel);
    int xLabelX = plotArea.x1 + (plotArea.width() - xLabelWidth) / 2;
    int xLabelY = plotArea.y2 + 30; // Position below X-axis
    textPainter_.drawText(xLabelX, xLabelY, xLabel, textColor_);

    // Draw Y-axis label (vertical text might require special handling)
    // For simplicity, drawing vertically - stacked characters
    // Position farther to the left to avoid overlapping with tick values
    int yLabelX = chartRect.x1 + 15;
    
    // Calculate position to center the label vertically on the y-axis
    int totalYLabelHeight = yLabel.length() * 12; // 12 pixels per character
    int yLabelY = plotArea.y1 + (plotArea.height() - totalYLabelHeight) / 2;
    
    // Draw each character stacked vertically to create vertical text
    for (size_t i = 0; i < yLabel.length(); ++i) {
        textPainter_.drawText(yLabelX, yLabelY + i * 12, std::string(1, yLabel[i]), textColor_);
    }

    // Draw legend
    Rect legendArea = Rect::make_rect(
        plotArea.x2 - legendWidth, 
        plotArea.y1,
        plotArea.x2, 
        plotArea.y1 + seriesSet.size() * 25
    );
    drawLegend(chartRect, legendArea, seriesSet);
}

// Draws the X and Y axes
void LineChartDemo::drawAxes(const Rect& plotArea, float xMin, float xMax, float yMin, float yMax) {
    // Draw axes lines
    linePainter_.drawLine(plotArea.x1, plotArea.y2, plotArea.x2, plotArea.y2, axisColor_); // X-axis
    linePainter_.drawLine(plotArea.x1, plotArea.y1, plotArea.x1, plotArea.y2, axisColor_); // Y-axis

    // Draw X-axis ticks and grid
    int numXTicks = 10; // Adjust as needed
    for (int i = 0; i <= numXTicks; ++i) {
        float value = xMin + (xMax - xMin) * i / numXTicks;
        float xPos = plotArea.x1 + plotArea.width() * i / numXTicks;
        
        // Draw tick
        linePainter_.drawLine(xPos, plotArea.y2, xPos, plotArea.y2 + 5, axisColor_);
        
        // Draw grid line
        linePainter_.drawLine(xPos, plotArea.y1, xPos, plotArea.y2, gridColor_);
        
        // Draw tick label
        // Convert value to string, truncating decimal places if needed
        char buffer[20];
        sprintf(buffer, "%.1f", value);
        std::string label = buffer;
        
        unsigned int labelWidth = textPainter_.textWidth(label);
        textPainter_.drawText(xPos - labelWidth / 2, plotArea.y2 + 15, label, textColor_);
    }

    // Draw Y-axis ticks and grid
    int numYTicks = 10; // Adjust as needed
    for (int i = 0; i <= numYTicks; ++i) {
        float value = yMin + (yMax - yMin) * (numYTicks - i) / numYTicks; // Invert for screen Y
        float yPos = plotArea.y1 + plotArea.height() * i / numYTicks;
        
        // Draw tick
        linePainter_.drawLine(plotArea.x1 - 5, yPos, plotArea.x1, yPos, axisColor_);
        
        // Draw grid line
        linePainter_.drawLine(plotArea.x1, yPos, plotArea.x2, yPos, gridColor_);
        
        // Draw tick label
        // Convert value to string, truncating decimal places if needed
        char buffer[20];
        sprintf(buffer, "%.1f", value);
        std::string label = buffer;
        
        unsigned int labelWidth = textPainter_.textWidth(label);
        // Move Y-axis labels further from the axis to avoid overlapping with Y-axis label
        textPainter_.drawText(plotArea.x1 - labelWidth - 25, yPos, label, textColor_);
    }
}

// Draws all data series
void LineChartDemo::drawSeries(const Rect& plotArea, const std::vector<SeriesData>& seriesSet,
                              float xMin, float xMax, float yMin, float yMax) {
    for (const auto& series : seriesSet) {
        if (series.points.size() < 2) continue;

        for (size_t i = 0; i < series.points.size() - 1; ++i) {
            PointF p1_world = series.points[i];
            PointF p2_world = series.points[i+1];

            PointF p1_screen = worldToScreen(p1_world.x, p1_world.y, plotArea, xMin, xMax, yMin, yMax);
            PointF p2_screen = worldToScreen(p2_world.x, p2_world.y, plotArea, xMin, xMax, yMin, yMax);

            linePainter_.drawLine(static_cast<int>(p1_screen.x), static_cast<int>(p1_screen.y),
                                  static_cast<int>(p2_screen.x), static_cast<int>(p2_screen.y), series.color);
        }
    }
}

// Draws the legend
void LineChartDemo::drawLegend(const Rect& chartRect, const Rect& legendArea, const std::vector<SeriesData>& seriesSet) {
    // chartRect is for overall context, legendArea is where the legend should be drawn.
    int itemHeight = 25; // Increased height per legend item for better spacing
    int colorBoxSize = 10;
    int textPadding = 8;  // Increased padding between color box and text
    
    // For courR12.bdf font (12pt), the typical glyph height is around 12 pixels
    // We'll use this to properly align text with the color boxes
    int fontHeight = 12;
    int fontBaseline = 9;  // Most BDF fonts have baseline at ~75% of height
    
    for (size_t i = 0; i < seriesSet.size(); ++i) {
        const auto& series = seriesSet[i];
        int currentY = legendArea.y1 + i * itemHeight;
        
        // Calculate vertical positions for perfect alignment
        // Position text so its vertical center aligns with the color box's center
        int colorBoxY = currentY + (itemHeight - colorBoxSize) / 2; // Centered color box
        int textY = colorBoxY + (colorBoxSize / 2) - (fontHeight / 2) + fontBaseline;
        
        // Draw color box - vertically centered in the item space
        rectPainter_.drawFull(legendArea.x1, colorBoxY,
                            legendArea.x1 + colorBoxSize, colorBoxY + colorBoxSize,
                            series.color);
        
        // Draw series name - properly aligned with the color box
        textPainter_.drawText(legendArea.x1 + colorBoxSize + textPadding, textY,
                            series.name, textColor_);
    }
}

// Converts world coordinates to screen coordinates
PointF LineChartDemo::worldToScreen(float x, float y, const Rect& plotArea,
                                    float xMin, float xMax, float yMin, float yMax) {
    if (xMax <= xMin || yMax <= yMin) { // Avoid division by zero or negative range
        return {static_cast<float>(plotArea.x1), static_cast<float>(plotArea.y1)};
    }

    float screenX = plotArea.x1 + (x - xMin) / (xMax - xMin) * plotArea.width();
    float screenY = plotArea.y2 - (y - yMin) / (yMax - yMin) * plotArea.height(); // Y is inverted

    // Clamping to plotArea boundaries might be useful if points can go outside
    // screenX = std::max((float)plotArea.x1, std::min(screenX, (float)plotArea.x2));
    // screenY = std::max((float)plotArea.y1, std::min(screenY, (float)plotArea.y2));

    return {screenX, screenY};
}
