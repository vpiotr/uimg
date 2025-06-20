#ifndef __UIMG_TEXT_DEMO_H__
#define __UIMG_TEXT_DEMO_H__

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>

#include "uimg/base/structs.h"
#include "uimg/text/text_base.h"
#include "uimg/fonts/bdf_font.h"
#include "uimg/fonts/font_utils.h"
#include "uimg/fonts/painter_for_bdf_font.h"
#include "samples/test_painter_base.h"

class test_text_demo : public test_painter_base {
public:
    test_text_demo(const std::string &outFileName, const std::string &fontPath) 
        : test_painter_base(outFileName), fontPath_(fontPath), font_(nullptr) {}

    virtual ~test_text_demo() {}

protected:
    virtual Point getImageSize() {
        return {800, 600}; // Larger canvas for text demonstration
    }

    virtual void init() {
        test_painter_base::init();
        
        // Initialize and load font
        font_ = std::make_unique<uimg::BdfFont>();
        if (!uimg::FontUtils::loadFontFromFile(*font_, fontPath_)) {
            std::cerr << "Failed to load font from: " << fontPath_ << std::endl;
            exit(1);
        }
        
        // Initialize the font painter
        textPainter_ = std::make_unique<uimg::TextPainterForBdfFont>(getPainter(), getImageSize());
        textPainter_->setFont(font_.get());
    }

    virtual void paint() {
        // Set background color
        RgbImage &img = getImage();
        BackgroundPainterForRgbImage bgPainter(img);
        bgPainter.paint(RgbColor{240, 240, 240}); // Light gray background
        
        // Draw title
        drawTitle();
        
        // Draw demo text sections
        drawColoredText();
        drawFormattedText();
        drawBorder();
    }

private:
    void drawTitle() {
        RgbColor titleColor{20, 40, 120}; // Dark blue for title
        textPainter_->drawText(20, 50, "BDF Font Demo", titleColor);
    }
    
    void drawColoredText() {
        // Draw text with different colors
        std::vector<RgbColor> colors = {
            {200, 0, 0},    // Red
            {0, 150, 0},    // Green
            {0, 0, 200},    // Blue
            {200, 100, 0},  // Orange
            {150, 0, 150}   // Purple
        };
        
        std::vector<std::string> colorNames = {
            "This text is Red",
            "This text is Green",
            "This text is Blue",
            "This text is Orange",
            "This text is Purple"
        };
        
        for (size_t i = 0; i < colors.size(); ++i) {
            textPainter_->drawText(50, static_cast<unsigned int>(120 + i * 40), colorNames[i], colors[i]);
        }
    }
    
    void drawFormattedText() {
        RgbColor textColor{50, 50, 50}; // Dark gray for regular text
        
        std::vector<std::string> sampleText = {
            "uImg Text Rendering Demo",
            "====================================",
            "",
            "This is a demonstration of text rendering",
            "capabilities using BDF fonts in uImg library.",
            "",
            "Features demonstrated:",
            "- Font loading from BDF files",
            "- Text positioning and alignment",
            "- Multi-color text rendering",
            "- Basic text formatting"
        };
        
        for (size_t i = 0; i < sampleText.size(); ++i) {
            textPainter_->drawText(400, static_cast<unsigned int>(150 + i * 30), sampleText[i], textColor);
        }
    }
    
    void drawBorder() {
        PixelPainter &painter = getPainter();
        RgbColor borderColor{100, 100, 100}; // Medium gray for border
        
        // Get image size
        Point size = getImageSize();
        
        // Draw border rectangle
        for (int x = 0; x < size.x; ++x) {
            painter.putPixel(static_cast<unsigned int>(x), 0, borderColor);
            painter.putPixel(static_cast<unsigned int>(x), static_cast<unsigned int>(size.y - 1), borderColor);
        }
        
        for (int y = 0; y < size.y; ++y) {
            painter.putPixel(0, static_cast<unsigned int>(y), borderColor);
            painter.putPixel(static_cast<unsigned int>(size.x - 1), static_cast<unsigned int>(y), borderColor);
        }
    }
    
private:
    std::string fontPath_;
    std::unique_ptr<uimg::BdfFont> font_;
    std::unique_ptr<uimg::TextPainterForBdfFont> textPainter_;
};

struct text_demo_args {
    std::string outFileName;
    std::string fontPath;
    
    static text_demo_args parse(int argc, const char* const argv[]) {
        std::string outFileName = "text_demo.ppm";
        std::string fontPath;
        
        for (int i = 1; i < argc; ++i) {
            if ((strcmp(argv[i], "-out") == 0) && (i + 1 < argc)) {
                outFileName = std::string(argv[i + 1]);
            } else if ((strcmp(argv[i], "-font") == 0) && (i + 1 < argc)) {
                fontPath = std::string(argv[i + 1]);
            }
        }
        
        if (fontPath.empty()) {
            std::cerr << "Error: Font path is required. Use -font <path/to/font.bdf>" << std::endl;
            exit(1);
        }
        
        return {outFileName, fontPath};
    }
};

#endif // __UIMG_TEXT_DEMO_H__
