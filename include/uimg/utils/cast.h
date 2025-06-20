#ifndef __UIMG_UTILS_CAST_H__
#define __UIMG_UTILS_CAST_H__

/**
 * @file cast.h
 * @brief Safe casting utilities for uimg library
 * @author Piotr Likus
 * @date June 20, 2025
 * 
 * This file provides safe casting functions that perform runtime validation
 * to prevent dangerous casts that could lead to unexpected behavior.
 * 
 * Two approaches are available:
 * 
 * 1. Template function (standard C++ cast syntax):
 *    auto result = unsigned_cast<unsigned int>(signed_value);
 *    - Familiar syntax like static_cast, dynamic_cast
 *    - Location info in exceptions shows "unknown" (template definition location)
 * 
 * 2. Macro (accurate location information):
 *    auto result = UNSIGNED_CAST(unsigned int, signed_value);
 *    - Captures actual file/line/function information
 *    - Better for debugging when exceptions occur
 * 
 * Usage examples:
 * @code
 * using uimg::utils::unsigned_cast;
 * 
 * // Safe usage (positive values) - both approaches work
 * int positive = 42;
 * unsigned int result1 = unsigned_cast<unsigned int>(positive);   // Template
 * unsigned int result2 = UNSIGNED_CAST(unsigned int, positive);  // Macro
 * 
 * // Unsafe usage (negative values) - throws UnsafeCastException
 * int negative = -42;
 * unsigned int result = unsigned_cast<unsigned int>(negative);  // Throws!
 * 
 * // Performance mode (validation disabled)
 * // Compile with -DUIMG_DISABLE_CAST_VALIDATION
 * // Both approaches behave like static_cast (no validation)
 * @endcode
 */

#include <stdexcept>
#include <string>
#include <sstream>
#include <type_traits>

namespace uimg {
namespace utils {

/**
 * @brief Exception thrown when an unsafe cast is attempted
 */
class UnsafeCastException : public std::runtime_error {
public:
    /**
     * @brief Construct with full location information
     * @param message Error description
     * @param file Source file name
     * @param line Line number
     * @param function Function name
     */
    UnsafeCastException(const std::string& message, const std::string& file, 
                       int line, const std::string& function)
        : std::runtime_error(message), 
          file_(file), 
          line_(line), 
          function_(function) {}
    
    virtual ~UnsafeCastException() = default;
    
    /**
     * @brief Get the source file where the cast failed
     * @return Source file name
     */
    const std::string& getFile() const { return file_; }
    
    /**
     * @brief Get the line number where the cast failed
     * @return Line number
     */
    int getLine() const { return line_; }
    
    /**
     * @brief Get the function name where the cast failed
     * @return Function name
     */
    const std::string& getFunction() const { return function_; }

private:
    std::string file_;
    int line_;
    std::string function_;
};

// Macro to enable/disable validation (can be overridden by compiler defines)
#ifndef UIMG_DISABLE_CAST_VALIDATION
#define UIMG_ENABLE_CAST_VALIDATION 1
#else
#define UIMG_ENABLE_CAST_VALIDATION 0
#endif

namespace detail {
    /**
     * @brief Helper function to perform the actual unsigned cast with validation
     * @tparam ToType Target unsigned type
     * @tparam FromType Source type
     * @param value Value to cast
     * @param file Source file name
     * @param line Line number
     * @param function Function name
     * @return Safely cast value
     * @throws UnsafeCastException if value is negative
     */
    template<typename ToType, typename FromType>
    ToType unsigned_cast_impl(FromType value, const char* file, int line, const char* function) {
        static_assert(std::is_unsigned<ToType>::value, "ToType must be an unsigned type");
        
#if UIMG_ENABLE_CAST_VALIDATION
        if (value < 0) {
            std::ostringstream ss;
            ss << "Attempt to cast negative value (" << value 
               << ") to unsigned type. Source type: " << typeid(FromType).name()
               << ", Target type: " << typeid(ToType).name();
            throw UnsafeCastException(ss.str(), file, line, function);
        }
#endif
        return static_cast<ToType>(value);
    }
}

/**
 * @brief Safe cast to unsigned types with runtime validation 
 * 
 * This function template provides the standard cast syntax but location
 * information will be from the template definition, not the call site.
 * For accurate location information, use UNSIGNED_CAST macro instead.
 * 
 * Usage: auto result = unsigned_cast<unsigned int>(signed_value);
 */
template<typename ToType, typename FromType>
ToType unsigned_cast(FromType value) {
    return detail::unsigned_cast_impl<ToType>(value, "unknown", 0, "unknown");
}

/**
 * @brief Macro version of unsigned_cast with accurate location information
 * 
 * This macro captures file, line, and function information at the call site,
 * providing accurate location details in exception messages.
 * 
 * Usage: auto result = UNSIGNED_CAST(unsigned int, signed_value);
 */
#define UNSIGNED_CAST(ToType, value) \
    uimg::utils::detail::unsigned_cast_impl<ToType>(value, __FILE__, __LINE__, __PRETTY_FUNCTION__)

} // namespace utils
} // namespace uimg

#endif // __UIMG_UTILS_CAST_H__
