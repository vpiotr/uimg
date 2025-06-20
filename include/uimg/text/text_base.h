//
// Created by piotr on 8/23/20.
// Modernized on 5/18/25.
//

#pragma once

#include <cstdint>
#include <vector>
#include <sstream>
#include <string>

namespace uimg {

using text_char_code_t = uint32_t;

/**
 * @brief Text alignment options
 */
enum class TextAlignment {
    LEFT,   ///< Align text to the left
    CENTER, ///< Center align text
    RIGHT   ///< Align text to the right
};

/**
 * @brief Abstract text source class for character iteration
 */
class TextSource {
public:
    virtual ~TextSource() = default;

    /**
     * @brief Check if there are more characters available
     * @return true if there are more characters, false otherwise
     */
    virtual bool hasNext() const {
        return false;
    }

    /**
     * @brief Get the next character code
     * @return The next character code
     */
    virtual text_char_code_t getNext() { return 0; }
};

/**
 * @brief Character source using vector of unicode code values as storage
 */
class VectorCharSource : public TextSource {
public:
    VectorCharSource() = delete;

    VectorCharSource(const VectorCharSource &src) = default;
    VectorCharSource& operator=(const VectorCharSource&) = default;

    explicit VectorCharSource(const std::vector<text_char_code_t> &data) : data_(&data), pos_(0) {}

    bool hasNext() const override {
        return pos_ < data_->size();
    }

    text_char_code_t getNext() override { 
        return (*data_)[pos_++]; 
    }

private:
    const std::vector<text_char_code_t> *data_ = nullptr;
    size_t pos_ = 0;
};

namespace detail {

    /**
     * @brief Convert value to string
     * @tparam T Type to convert
     * @param arg Value to convert
     * @return String representation of the value
     */
    template<typename T>
    std::string toString(const T &arg) {
        std::ostringstream out;
        out << arg;
        return out.str();
    }

} // namespace detail

} // namespace uimg
