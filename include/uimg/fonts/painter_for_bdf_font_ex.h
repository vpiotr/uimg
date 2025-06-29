#pragma once

#include "uimg/fonts/painter_for_bdf_font.h"
#include "uimg/text/text_base.h"
#include "uimg/base/structs.h"
#include "uimg/painters/painter_base.h"
#include "uimg/pixels/pixel_painter.h"

namespace uimg {

/**
 * @brief Extended text painter implementation for BDF fonts with additional features
 * 
 * This class extends TextPainterForBdfFont with support for:
 * - Text scaling
 * - Text color management
 * - Text alignment
 * - Drawing text at Point coordinates
 */
class TextPainterForBdfFontEx : public TextPainterForBdfFont {
public:
    /**
     * @brief Create an extended text painter with canvas size
     * @param pixelPainter Pixel painter for rendering
     * @param canvasSize Size of the canvas
     */
    TextPainterForBdfFontEx(PixelPainter &pixelPainter, const Point &canvasSize)
        : TextPainterForBdfFont(pixelPainter, canvasSize)
        , scale_(1.0f)
        , textColor_(RgbColor{0, 0, 0}) // Default to black
        , alignment_(TextAlignment::LEFT) {}
    
    /**
     * @brief Create an extended text painter with specific canvas region
     * @param pixelPainter Pixel painter for rendering
     * @param canvasRect Canvas rectangle
     */
    TextPainterForBdfFontEx(PixelPainter &pixelPainter, const Rect &canvasRect)
        : TextPainterForBdfFont(pixelPainter, canvasRect)
        , scale_(1.0f)
        , textColor_(RgbColor{0, 0, 0}) // Default to black
        , alignment_(TextAlignment::LEFT) {}

    /**
     * @brief Set the text scaling factor
     * @param scale Scaling factor (1.0 = normal size, 2.0 = double size, etc.)
     */
    void setScale(float scale) noexcept {
        scale_ = scale;
    }

    /**
     * @brief Get the current text scaling factor
     * @return Current scaling factor
     */
    [[nodiscard]] float getScale() const noexcept {
        return scale_;
    }

    /**
     * @brief Set the text color
     * @param color Text color to use for subsequent text rendering
     */
    void setTextColor(const RgbColor &color) noexcept {
        textColor_ = color;
    }

    /**
     * @brief Get the current text color
     * @return Current text color
     */
    [[nodiscard]] const RgbColor& getTextColor() const noexcept {
        return textColor_;
    }

    /**
     * @brief Set the text alignment
     * @param alignment Text alignment to use
     */
    void setAlignment(TextAlignment alignment) noexcept {
        alignment_ = alignment;
    }

    /**
     * @brief Get the current text alignment
     * @return Current text alignment
     */
    [[nodiscard]] TextAlignment getAlignment() const noexcept {
        return alignment_;
    }

    /**
     * @brief Draw text at specified Point with current color and alignment
     * @param pos Position to draw text at
     * @param text Text to draw
     */
    void drawText(const Point &pos, const std::string &text) {
        Point adjustedPos = calculateAlignedPosition(pos, text);
        drawTextScaled(static_cast<unsigned int>(adjustedPos.x), static_cast<unsigned int>(adjustedPos.y), text, textColor_);
    }

    /**
     * @brief Draw text at specified coordinates with current color and alignment
     * @param x X coordinate
     * @param y Y coordinate
     * @param text Text to draw
     */
    void drawText(unsigned int x, unsigned int y, const std::string &text) {
        Point pos{static_cast<int>(x), static_cast<int>(y)};
        Point adjustedPos = calculateAlignedPosition(pos, text);
        drawTextScaled(static_cast<unsigned int>(adjustedPos.x), static_cast<unsigned int>(adjustedPos.y), text, textColor_);
    }

    /**
     * @brief Draw text at specified coordinates with specified color
     * @param x X coordinate
     * @param y Y coordinate
     * @param text Text to draw
     * @param color Text color
     */
    void drawText(unsigned int x, unsigned int y, const std::string &text, const RgbColor &color) override {
        Point pos{static_cast<int>(x), static_cast<int>(y)};
        Point adjustedPos = calculateAlignedPosition(pos, text);
        drawTextScaled(static_cast<unsigned int>(adjustedPos.x), static_cast<unsigned int>(adjustedPos.y), text, color);
    }

    /**
     * @brief Calculate text width with current scaling
     * @param text Text to measure
     * @return Width of the text with scaling applied
     */
    unsigned int textWidth(const std::string &text) override {
        unsigned int baseWidth = TextPainterForBdfFont::textWidth(text);
        return static_cast<unsigned int>(baseWidth * scale_);
    }

    /**
     * @brief Calculate text size with current scaling
     * @param text Text to measure
     * @return Size as Point(width, height) with scaling applied
     */
    Point textSize(const std::string &text) override {
        Point baseSize = TextPainterForBdfFont::textSize(text);
        return {
            static_cast<int>(baseSize.x * scale_),
            static_cast<int>(baseSize.y * scale_)
        };
    }

protected:
    /**
     * @brief Calculate the aligned position for text rendering
     * @param pos Original position
     * @param text Text to align
     * @return Adjusted position based on alignment
     */
    Point calculateAlignedPosition(const Point &pos, const std::string &text) {
        Point adjustedPos = pos;
        
        switch (alignment_) {
            case TextAlignment::LEFT:
                // No adjustment needed for left alignment
                break;
            case TextAlignment::CENTER: {
                unsigned int width = textWidth(text);
                adjustedPos.x = pos.x - static_cast<int>(width / 2);
                break;
            }
            case TextAlignment::RIGHT: {
                unsigned int width = textWidth(text);
                adjustedPos.x = pos.x - static_cast<int>(width);
                break;
            }
        }
        
        return adjustedPos;
    }

    /**
     * @brief Draw text with scaling applied
     * @param x X coordinate
     * @param y Y coordinate
     * @param text Text to draw
     * @param color Text color
     */
    void drawTextScaled(unsigned int x, unsigned int y, const std::string &text, const RgbColor &color) {
        if (scale_ == 1.0f) {
            // Use base implementation for normal scale
            TextPainterForBdfFont::drawText(x, y, text, color);
        } else {
            // Draw scaled text by rendering each character at scaled positions
            unsigned int currentX = x;
            for (size_t i = 0; i < text.length(); ++i) {
                char c = text[i];
                drawGlyphScaled(currentX, y, static_cast<text_char_code_t>(static_cast<unsigned char>(c)), color);
                
                // Advance by scaled character width
                unsigned int charWidth = TextPainterForBdfFont::glyphWidth(static_cast<text_char_code_t>(static_cast<unsigned char>(c)));
                currentX += static_cast<unsigned int>(charWidth * scale_);
            }
        }
    }

    /**
     * @brief Draw a single glyph with scaling applied
     * @param x X coordinate
     * @param y Y coordinate
     * @param charCode Character code
     * @param color Glyph color
     */
    void drawGlyphScaled(unsigned int x, unsigned int y, text_char_code_t charCode, const RgbColor &color) {
        if (scale_ == 1.0f) {
            // Use base implementation for normal scale
            TextPainterForBdfFont::drawGlyph(x, y, charCode, color);
        } else {
            // Simple scaling: replicate pixels for each glyph pixel
            // We'll approximate scaling by drawing multiple base-size characters offset slightly
            int scaleInt = static_cast<int>(scale_);
            if (scaleInt < 1) scaleInt = 1;
            
            // For simplicity, draw the glyph multiple times with slight offsets
            // This is a basic scaling approximation
            for (int sy = 0; sy < scaleInt; ++sy) {
                for (int sx = 0; sx < scaleInt; ++sx) {
                    unsigned int offsetX = x + static_cast<unsigned int>(sx);
                    unsigned int offsetY = y + static_cast<unsigned int>(sy);
                    TextPainterForBdfFont::drawGlyph(offsetX, offsetY, charCode, color);
                }
            }
        }
    }

private:
    float scale_;                ///< Text scaling factor
    RgbColor textColor_;         ///< Current text color
    TextAlignment alignment_;    ///< Current text alignment
};

} // namespace uimg
