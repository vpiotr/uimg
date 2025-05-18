//
// Created by piotr on 8/23/20.
// Modernized on 5/18/25.
//

#pragma once

#include <string>
#include <string_view>

#include "uimg/base/structs.h"
#include "uimg/text/text_base.h"

namespace uimg {

/**
 * @brief Interface for text painting operations
 */
class TextPainter {
public:
    virtual ~TextPainter() = default;

    /**
     * @brief Draw text at the specified position
     * @param x X coordinate
     * @param y Y coordinate
     * @param text Text to draw
     * @param color Text color
     */
    virtual void drawText(unsigned int x, unsigned int y, const std::string &text, const RgbColor &color) = 0;

    /**
     * @brief Calculate text width
     * @param text Text to measure
     * @return Width of the text in pixels
     */
    virtual unsigned int textWidth(const std::string &text) = 0;

    /**
     * @brief Calculate text size
     * @param text Text to measure
     * @return Size of the text as a Point(width, height)
     */
    virtual Point textSize(const std::string &text) = 0;

    /**
     * @brief Draw a single glyph
     * @param x X coordinate
     * @param y Y coordinate
     * @param char_code Character code
     * @param color Glyph color
     */
    virtual void drawGlyph(unsigned int x, unsigned int y, text_char_code_t char_code, const RgbColor &color) = 0;

    /**
     * @brief Get glyph width
     * @param char_code Character code
     * @return Width of the glyph in pixels
     */
    virtual unsigned int glyphWidth(text_char_code_t char_code) = 0;

    /**
     * @brief Get glyph height
     * @param char_code Character code
     * @return Height of the glyph in pixels
     */
    virtual unsigned int glyphHeight(text_char_code_t char_code) = 0;

    /**
     * @brief Get glyph size
     * @param charCode Character code
     * @return Size of the glyph as a Point(width, height)
     */
    virtual Point glyphSize(text_char_code_t charCode) = 0;

    /**
     * @brief Draw text from a TextSource
     * @param x X coordinate
     * @param y Y coordinate
     * @param src Source of characters
     * @param color Text color
     */
    virtual void drawText(unsigned int x, unsigned int y, TextSource &src, const RgbColor &color) = 0;

    /**
     * @brief Calculate text width from a TextSource
     * @param src Source of characters
     * @return Width of the text in pixels
     */
    virtual unsigned int textWidth(TextSource &src) = 0;

    /**
     * @brief Calculate text size from a TextSource
     * @param src Source of characters
     * @return Size of the text as a Point(width, height)
     */
    virtual Point textSize(TextSource &src) = 0;
};

} // namespace uimg
