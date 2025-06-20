#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <iostream>
#include <fstream>

#include "uimg/base/structs.h"
#include "uimg/text/text_base.h"
#include "uimg/utils/cast.h"

namespace uimg {

/**
 * @brief Represents a single glyph in a BDF font
 */
class BdfGlyph {
public:
    using pixel_line_t = uint32_t;

    BdfGlyph() = default;
    ~BdfGlyph() = default;

    BdfGlyph(const BdfGlyph&) = default;
    BdfGlyph& operator=(const BdfGlyph&) = default;
    
    BdfGlyph(BdfGlyph&&) noexcept = default;
    BdfGlyph& operator=(BdfGlyph&&) noexcept = default;

    /**
     * @brief Get the bounding box size
     * @return Bounding box size as Point
     */
    [[nodiscard]] Point bbxSize() const noexcept {
        return bbxSize_;
    }

    /**
     * @brief Set the bounding box size
     * @param value New bounding box size
     */
    void setBbxSize(const Point &value) noexcept {
        bbxSize_ = value;
    }

    /**
     * @brief Get the bounding box offset
     * @return Bounding box offset as Point
     */
    [[nodiscard]] Point bbxOffset() const noexcept {
        return bbxOffset_;
    }

    /**
     * @brief Set the bounding box offset
     * @param value New bounding box offset
     */
    void setBbxOffset(const Point &value) noexcept {
        bbxOffset_ = value;
    }

    /**
     * @brief Get device width
     * @return Device width as Point
     */
    [[nodiscard]] Point deviceWidth() const noexcept {
        return deviceWidth_;
    }

    /**
     * @brief Set device width
     * @param value New device width
     */
    void setDeviceWidth(const Point &value) noexcept {
        deviceWidth_ = value;
    }

    /**
     * @brief Get scalable width
     * @return Scalable width as Point
     */
    [[nodiscard]] Point scalableWidth() const noexcept {
        return scalableWidth_;
    }

    /**
     * @brief Set scalable width
     * @param value New scalable width
     */
    void setScalableWidth(const Point &value) noexcept {
        scalableWidth_ = value;
    }

    /**
     * @brief Get character encoding
     * @return Character code
     */
    [[nodiscard]] text_char_code_t encoding() const noexcept {
        return encoding_;
    }

    /**
     * @brief Set character encoding
     * @param value New character code
     */
    void setEncoding(text_char_code_t value) noexcept {
        encoding_ = value;
    }

    /**
     * @brief Get glyph name
     * @return Glyph name
     */
    [[nodiscard]] const std::string& name() const noexcept {
        return name_;
    }

    /**
     * @brief Set glyph name
     * @param value New glyph name
     */
    void setName(std::string value) {
        name_ = std::move(value);
    }

    /**
     * @brief Get pixel data for rendering
     * @return Pointer to pixel data
     */
    [[nodiscard]] const pixel_line_t *pixelData() const {
        return pixelData_.data();
    }

    /**
     * @brief Get number of pixel data lines
     * @return Size of pixel data
     */
    [[nodiscard]] size_t pixelDataSize() const noexcept {
        return pixelData_.size();
    }

    /**
     * @brief Set pixel data for rendering
     * @param value New pixel data
     */
    void setPixelData(std::vector<pixel_line_t> value) {
        pixelData_ = std::move(value);
    }

private:
    Point bbxSize_{0, 0};
    Point bbxOffset_{0, 0};
    Point deviceWidth_{0, 0};
    Point scalableWidth_{0, 0};
    text_char_code_t encoding_ = 0;
    std::string name_;
    std::vector<pixel_line_t> pixelData_;
};

/**
 * @brief Represents a BDF font with multiple glyphs
 */
class BdfFont {
public:
    BdfFont() = default;
    ~BdfFont() = default;
    
    BdfFont(const BdfFont&) = default;
    BdfFont& operator=(const BdfFont&) = default;
    
    BdfFont(BdfFont&&) noexcept = default;
    BdfFont& operator=(BdfFont&&) noexcept = default;

    /**
     * @brief Add a glyph to the font
     * @param glyph Glyph to add
     */
    void addGlyph(const BdfGlyph &glyph) {
        unsigned int newPos = UNSIGNED_CAST(unsigned int, glyphs_.size());
        glyphs_.push_back(glyph);
        codeMap_.emplace(glyph.encoding(), newPos);
        nameMap_.emplace(glyph.name(), newPos);
    }

    /**
     * @brief Get a glyph by character code
     * @param code Character code to look up
     * @return Pointer to the glyph, or nullptr if not found
     */
    [[nodiscard]] const BdfGlyph *getGlyphByCode(text_char_code_t code) const {
        auto it = codeMap_.find(code);
        if (it == codeMap_.end()) {
            return nullptr;
        }
        return &(glyphs_[it->second]);
    }

    /**
     * @brief Get a glyph by name
     * @param name Glyph name to look up
     * @return Pointer to the glyph, or nullptr if not found
     */
    [[nodiscard]] const BdfGlyph *getGlyphByName(const std::string &name) const {
        auto it = nameMap_.find(name);
        if (it == nameMap_.end()) {
            return nullptr;
        }
        return &(glyphs_[it->second]);
    }

    /**
     * @brief Get the number of glyphs in the font
     * @return Number of glyphs
     */
    [[nodiscard]] size_t glyphCount() const noexcept { 
        return glyphs_.size(); 
    }

    /**
     * @brief Get font version
     * @return Font version string
     */
    [[nodiscard]] const std::string& version() const noexcept { 
        return version_; 
    }

    /**
     * @brief Set font version
     * @param value New font version
     */
    void setVersion(std::string value) {
        version_ = std::move(value);
    }

    /**
     * @brief Get font bounding box
     * @return Font bounding box
     */
    [[nodiscard]] Rect fontBoundingBox() const noexcept { 
        return fontBoundingBox_; 
    }

    /**
     * @brief Set font bounding box
     * @param value New font bounding box
     */
    void setFontBoundingBox(const Rect &value) noexcept {
        fontBoundingBox_ = value;
    }

    /**
     * @brief Get font name
     * @return Font name
     */
    [[nodiscard]] const std::string& name() const noexcept { 
        return name_; 
    }

    /**
     * @brief Set font name
     * @param value New font name
     */
    void setName(std::string value) {
        name_ = std::move(value);
    }

private:
    std::vector<BdfGlyph> glyphs_;
    std::map<text_char_code_t, unsigned int> codeMap_;
    std::map<std::string, unsigned int> nameMap_;
    std::string version_;
    Rect fontBoundingBox_{0, 0, 0, 0};
    std::string name_;
};

/**
 * @brief Class for loading BDF fonts from streams
 */
class BdfFontLoader {
public:
    /**
     * @brief Load a BDF font from an input stream
     * @param in Input stream containing BDF font data
     * @param out Font object to be populated
     */
    void load(std::istream &in, BdfFont &out) {
    	using namespace detail;

        std::string s;

        int i1, i2, i3, i4;
        unsigned int pixelShift = 0;
        int glyphLineNo = -1;
        text_char_code_t ch = 0;

        BdfFont newFont;
        BdfGlyph newGlyph;
        BdfGlyph::pixel_line_t line;
        std::vector<BdfGlyph::pixel_line_t> pixelLines;
        
        // Constants for string prefix lengths
        constexpr size_t STARTCHAR_LEN_P1 = 10; // strlen("STARTCHAR ")
        constexpr size_t FONT_LEN_P1 = 5;       // strlen("FONT ")
        constexpr size_t STARTFONT_LEN_P1 = 10; // strlen("STARTFONT ")

        while (std::getline(in, s)) {
            if (strncmp(s.c_str(), "STARTCHAR ", STARTCHAR_LEN_P1) == 0) {
                if (s.length() > STARTCHAR_LEN_P1) {
                    s = s.substr(STARTCHAR_LEN_P1);
                    newGlyph.setName(s);
                }
            }
            else if (strncmp(s.c_str(), "FONT ", FONT_LEN_P1) == 0) {
                if (s.length() > FONT_LEN_P1) {
                    s = s.substr(FONT_LEN_P1);
                    newFont.setName(s);
                }
            }
            else if (strncmp(s.c_str(), "STARTFONT ", STARTFONT_LEN_P1) == 0) {
                if (s.length() > STARTFONT_LEN_P1) {
                    s = s.substr(STARTFONT_LEN_P1);
                    newFont.setVersion(s);
                }
            }
            else if (sscanf(s.c_str(), "FONTBOUNDINGBOX %d %d %d %d", &i1, &i2, &i3, &i4) == 4) {
                Rect fontBoundingBox;
                fontBoundingBox.x1 = i1;
                fontBoundingBox.y1 = i2;
                fontBoundingBox.x2 = i3;
                fontBoundingBox.y2 = i4;

                newFont.setFontBoundingBox(fontBoundingBox);
            }
            else if (sscanf(s.c_str(), "ENCODING %ud", &ch) == 1) {
                newGlyph.setEncoding(ch);
            }
            else if (sscanf(s.c_str(), "DWIDTH %d %d", &i1, &i2) == 2) {
                Point dwidth = { i1, i2 };
                newGlyph.setDeviceWidth(dwidth);
            }
            else if (sscanf(s.c_str(), "SWIDTH %d %d", &i1, &i2) == 2) {
                Point swidth = { i1, i2 };
                newGlyph.setScalableWidth(swidth);
            }
            else if (sscanf(s.c_str(), "BBX %d %d %d %d", &i1, &i2, &i3, &i4) == 4) {
                Point bbxSize = { i1, i2 };
                Point bbxOffset = { i3, i4 };
                newGlyph.setBbxSize(bbxSize);
                newGlyph.setBbxOffset(bbxOffset);
                auto shiftA = 8 * (sizeof(BdfGlyph::pixel_line_t) - UNSIGNED_CAST(size_t, (bbxSize.x + 7) / 8));
                auto shiftB = UNSIGNED_CAST(unsigned int, bbxOffset.x);
                long shiftC = (long)shiftA - (long)shiftB;

                pixelShift = shiftC < 0 ? 0 : UNSIGNED_CAST(unsigned int, shiftC);
                glyphLineNo = -1;
                pixelLines.clear();
                pixelLines.reserve(UNSIGNED_CAST(size_t, bbxSize.y));
            }
            else if (strncmp(s.c_str(), "BITMAP", strlen("BITMAP")) == 0) {
                glyphLineNo = 0;
            }
            else if (glyphLineNo >= 0 && glyphLineNo < newGlyph.bbxSize().y
                && (sscanf(s.c_str(), "%x", &line) == 1)) {
                line <<= pixelShift;
                pixelLines.push_back(line);
                ++glyphLineNo;
            }
            else if (strncmp(s.c_str(), "ENDCHAR", strlen("ENDCHAR")) == 0) {
                if (glyphLineNo == newGlyph.bbxSize().y) {
                    newGlyph.setPixelData(pixelLines);
                    glyphLineNo = -1;
                    pixelShift = 0;
                    if (newGlyph.name().empty()) {
                        newGlyph.setName(toString(newFont.glyphCount()));
                    }
                    newFont.addGlyph(newGlyph);
                    newGlyph = BdfGlyph();
                }
            }
        }

        out = newFont;
    }
};

} // namespace uimg
