#pragma once

#include <cassert>
#include <algorithm>
#include <memory>

#include "uimg/base/structs.h"
#include "uimg/fonts/bdf_font.h"
#include "uimg/text/text_painter.h"
#include "uimg/painters/painter_base.h"
#include "uimg/utils/cast.h"
#include "uimg/utils/cast.h"

namespace uimg {

/**
 * @brief Specialized text painter implementation for BDF fonts
 */
class TextPainterForBdfFont : public TextPainter {
public:
    /**
     * @brief Create a text painter with canvas size
     * @param pixelPainter Pixel painter for rendering
     * @param canvasSize Size of the canvas
     */
    TextPainterForBdfFont(PixelPainter &pixelPainter, const Point &canvasSize)
        : font_(nullptr)
        , pixelPainter_(pixelPainter)
        , canvasRect_(Rect::make_rect(0, 0, canvasSize.x - 1, canvasSize.y - 1)) {}
    
    /**
     * @brief Create a text painter with specific canvas region
     * @param pixelPainter Pixel painter for rendering
     * @param canvasRect Canvas rectangle
     */
    TextPainterForBdfFont(PixelPainter &pixelPainter, const Rect &canvasRect)
        : font_(nullptr)
        , pixelPainter_(pixelPainter)
        , canvasRect_(canvasRect) {}

    /**
     * @brief Get the current font
     * @return Current font pointer
     */
    [[nodiscard]] const BdfFont *font() const noexcept { 
        return font_; 
    }
    
    /**
     * @brief Set the font to use for rendering
     * @param value Font to use
     */
    void setFont(BdfFont *value) noexcept { 
        font_ = value; 
    }

    /**
     * @brief Draw text at specified position
     * @param x X coordinate
     * @param y Y coordinate
     * @param text Text to draw
     * @param color Text color
     */
    void drawText(unsigned int x, unsigned int y, const std::string &text, const RgbColor &color) override {
        assert(font_ != nullptr);
        unsigned int idx = 0;
        unsigned int sx = UNSIGNED_CAST(unsigned int, canvasRect_.x2);
        while ((x <= sx) && (idx < text.length())) {
            x += UNSIGNED_CAST(unsigned int, drawGlyphInt(x, y, static_cast<text_char_code_t>(UNSIGNED_CAST(unsigned char, text[idx])), color));
            ++idx;
        }
    }

    /**
     * @brief Draw a single glyph
     * @param x X coordinate
     * @param y Y coordinate
     * @param char_code Character code
     * @param color Glyph color
     */
    void drawGlyph(unsigned int x, unsigned int y, text_char_code_t char_code, const RgbColor &color) override {
        drawGlyphInt(x, y, char_code, color);
    }

    /**
     * @brief Draw text from a TextSource
     * @param x X coordinate
     * @param y Y coordinate
     * @param src Source of characters
     * @param color Text color
     */
    void drawText(unsigned int x, unsigned int y, TextSource &src, const RgbColor &color) override {
        assert(font_ != nullptr);
        unsigned int sx = UNSIGNED_CAST(unsigned int, canvasRect_.x2);
        while ((x <= sx) && (src.hasNext())) {
            x += UNSIGNED_CAST(unsigned int, drawGlyphInt(x, y, src.getNext(), color));
        }
    }

    /**
     * @brief Calculate text width
     * @param text Text to measure
     * @return Width of the text
     */
    unsigned int textWidth(const std::string &text) override {
        unsigned int idx = 0;
        unsigned int width = 0;

        while (idx < text.length()) {
            width += glyphWidth(static_cast<text_char_code_t>(UNSIGNED_CAST(unsigned char, text[idx])));
            ++idx;
        }

        return width;
    }

    /**
     * @brief Calculate text width from TextSource
     * @param src Source of characters
     * @return Width of the text
     */
    unsigned int textWidth(TextSource &src) override {
        unsigned int width = 0;

        while (src.hasNext()) {
            width += glyphWidth(src.getNext());
        }

        return width;
    }

    /**
     * @brief Calculate text size
     * @param text Text to measure
     * @return Size as Point(width, height)
     */
    Point textSize(const std::string &text) override {
        size_t idx = 0;
        size_t len = text.length();
        int width = 0;
        int height = 0;
        Point p;

        while (idx < len) {
            p = glyphSize(static_cast<text_char_code_t>(UNSIGNED_CAST(unsigned char, text[idx])));
            width += p.x;
            height = std::max(height, p.y);
            ++idx;
        }

        return {width, height};
    }

    /**
     * @brief Calculate text size from TextSource
     * @param src Source of characters
     * @return Size as Point(width, height)
     */
    Point textSize(TextSource &src) override {
        unsigned int width = 0;
        int height = 0;
        Point p;

        while (src.hasNext()) {
            p = glyphSize(src.getNext());
            width += UNSIGNED_CAST(unsigned int, p.x);
            height = std::max(height, p.y);
        }

        return {static_cast<int>(width), height};
    }

    /**
     * @brief Get width of a single glyph
     * @param charCode Character code
     * @return Width of the glyph
     */
    unsigned int glyphWidth(text_char_code_t charCode) override {
        const BdfGlyph *g = get_glyph(charCode);
        assert(g != nullptr);
        return glyphWidth(g);
    }

    /**
     * @brief Get height of a single glyph
     * @param charCode Character code
     * @return Height of the glyph
     */
    unsigned int glyphHeight(text_char_code_t charCode) override {
        const BdfGlyph *g = get_glyph(charCode);
        assert(g != nullptr);
        return glyphHeight(g);
    }

    /**
     * @brief Get size of a single glyph
     * @param charCode Character code
     * @return Size as Point(width, height)
     */
    Point glyphSize(text_char_code_t charCode) override {
        const BdfGlyph *g = get_glyph(charCode);
        assert(g != nullptr);
        Point p;
        p.x = static_cast<int>(glyphWidth(g));
        p.y = static_cast<int>(glyphHeight(g));
        return p;
    }

protected:
    /**
     * @brief Get character code to use when a requested glyph is not found
     * @return Fallback character code
     */
    [[nodiscard]] virtual text_char_code_t unknown_char_code() const {
        return '?';
    }

    /**
     * @brief Calculate width of a glyph
     * @param glyph Glyph pointer
     * @return Width of the glyph
     */
    [[nodiscard]] unsigned int glyphWidth(const BdfGlyph *glyph) {
        assert(glyph != nullptr);
        unsigned int w = UNSIGNED_CAST(unsigned int, glyph->bbxSize().x + glyph->bbxOffset().x);
        return UNSIGNED_CAST(unsigned int, std::max(glyph->deviceWidth().x, static_cast<int>(w)));
    }

    /**
     * @brief Calculate height of a glyph
     * @param glyph Glyph pointer
     * @return Height of the glyph
     */
    [[nodiscard]] unsigned int glyphHeight(const BdfGlyph *glyph) {
        assert(glyph != nullptr);
        unsigned int h = UNSIGNED_CAST(unsigned int, glyph->bbxSize().y + glyph->bbxOffset().y);
        return UNSIGNED_CAST(unsigned int, std::max(glyph->deviceWidth().y, static_cast<int>(h)));
    }

private:
    /**
     * @brief Internal method to draw a glyph
     * @param x X coordinate
     * @param y Y coordinate
     * @param charCode Character code
     * @param color Glyph color
     * @return Width of the drawn glyph
     */
    int drawGlyphInt(unsigned int x, unsigned int y, text_char_code_t charCode, const RgbColor &color) {
        const BdfGlyph *g = get_glyph(charCode);
        PixelPainter *painter = checkPixelPainter();
        assert(g != nullptr);

        int h = g->bbxSize().y;
        int w = g->bbxSize().x + g->bbxOffset().x;
        int y0 = static_cast<int>(y) - h - g->bbxOffset().y;

        for (int yi = 0; yi < h; ++yi) {
            const BdfGlyph::pixel_line_t row = g->pixelData()[yi];
            BdfGlyph::pixel_line_t pixelMask = 0x80000000;

            for (int xi = 0; xi < w; ++xi, pixelMask >>= 1) {
                if (row & pixelMask) {
                    int pixel_x = static_cast<int>(x) + xi;
                    int pixel_y = y0 + yi;
                    if (pixel_x >= 0 && pixel_y >= 0) {
                        painter->putPixel(UNSIGNED_CAST(unsigned int, pixel_x), UNSIGNED_CAST(unsigned int, pixel_y), color);
                    }
                }
            }
        }
        return std::max(g->deviceWidth().x, static_cast<int>(w));
    }

    /**
     * @brief Get a glyph by character code with fallback
     * @param charCode Character code
     * @return Pointer to glyph, or fallback glyph if not found
     */
    [[nodiscard]] const BdfGlyph *get_glyph(text_char_code_t charCode) const {
        const BdfGlyph *g = font_->getGlyphByCode(charCode);
        if (g == nullptr) {
            g = font_->getGlyphByCode(unknown_char_code());
        }
        return g;
    }

    /**
     * @brief Get the pixel painter for rendering
     * @return Pixel painter pointer
     */
    virtual PixelPainter *checkPixelPainter() {
        return &pixelPainter_;
    }

private:
    const BdfFont *font_;
    PixelPainter &pixelPainter_;
    Rect canvasRect_;
};

} // namespace uimg
