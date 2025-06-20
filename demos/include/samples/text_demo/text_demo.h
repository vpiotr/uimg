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
#include "uimg/fonts/painter_for_bdf_font_ex.h"
#include "uimg/utils/cast.h"
#include "samples/demo_painter_base.h"

class text_demo : public demo_painter_base {
public:
    text_demo(const std::string &outFileName, const std::string &fontPath) 
        : demo_painter_base(outFileName), fontPath_(fontPath), font_(nullptr) {}

    virtual ~text_demo() {}

protected:
    virtual Point getImageSize() {
        return {800, 600}; // Larger canvas for text demonstration
    }

    virtual void init() {
        demo_painter_base::init();
        
        // Initialize and load font
        font_ = std::make_unique<uimg::BdfFont>();
        if (!uimg::FontUtils::loadFontFromFile(*font_, fontPath_)) {
            std::cerr << "Failed to load font from: " << fontPath_ << std::endl;
            exit(1);
        }
        
        // Initialize the font painter
        textPainter_ = std::make_unique<uimg::TextPainterForBdfFontEx>(getPainter(), getImageSize());
        textPainter_->setFont(font_.get());
    }

    virtual void paint() {
        // Set background color
        RgbImage &img = getImage();
        BackgroundPainterForRgbImage bgPainter(img);
        bgPainter.paint(RgbColor{240, 240, 240}); // Light gray background
        
        // Draw heading
        textPainter_->setScale(2.0f);
        textPainter_->setTextColor(RgbColor{20, 40, 180}); // Dark blue
        textPainter_->drawText(Point{50, 50}, "uimg Text Rendering Demo");
        
        // Reset scale for normal text
        textPainter_->setScale(1.0f);
        
        // Draw sample text in different colors and styles
        textPainter_->setTextColor(RgbColor{0, 0, 0}); // Black
        textPainter_->drawText(Point{50, 120}, "Regular black text");
        
        textPainter_->setTextColor(RgbColor{180, 0, 0}); // Red
        textPainter_->drawText(Point{50, 160}, "Red text sample");
        
        textPainter_->setTextColor(RgbColor{0, 120, 0}); // Green
        textPainter_->drawText(Point{50, 200}, "Green text sample");
        
        // Demonstrate different scales
        textPainter_->setTextColor(RgbColor{80, 80, 80}); // Gray
        textPainter_->setScale(0.75f);
        textPainter_->drawText(Point{50, 250}, "Smaller scale text (0.75)");
        
        textPainter_->setScale(1.5f);
        textPainter_->drawText(Point{50, 300}, "Larger scale text (1.5)");
        
        // Demonstrate text alignment
        textPainter_->setScale(1.0f);
        textPainter_->setTextColor(RgbColor{100, 50, 150}); // Purple
        
        // Draw a box to show alignment area
        RectPainterForRgbImage rectPainter(img);
        rectPainter.drawEmpty(400, 350, 700, 450, RgbColor{200, 200, 200});
        
        // Align text within the box
        textPainter_->setAlignment(uimg::TextAlignment::LEFT);
        textPainter_->drawText(Point{400, 380}, "Left aligned");
        
        textPainter_->setAlignment(uimg::TextAlignment::CENTER);
        textPainter_->drawText(Point{550, 410}, "Center aligned");
        
        textPainter_->setAlignment(uimg::TextAlignment::RIGHT);
        textPainter_->drawText(Point{700, 440}, "Right aligned");
        
        // Special characters demonstration
        textPainter_->setAlignment(uimg::TextAlignment::LEFT);
        textPainter_->setTextColor(RgbColor{0, 0, 0}); // Black
        textPainter_->drawText(Point{50, 500}, "Special chars: !@#$%^&*()_+-=[]{}|;:,./<>?");
        
        // Information text
        textPainter_->setScale(0.8f);
        textPainter_->setTextColor(RgbColor{100, 100, 100}); // Gray
        textPainter_->drawText(Point{50, 550}, "Font: " + fontPath_);
    }

private:
    std::string fontPath_;
    std::unique_ptr<uimg::BdfFont> font_;
    std::unique_ptr<uimg::TextPainterForBdfFontEx> textPainter_;
    
    PixelPainterForImageBase& getPainter() {
        static PixelPainterForImageBase painter(getImage());
        return painter;
    }
};

#endif
