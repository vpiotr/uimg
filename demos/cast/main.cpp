#include "uimg/utils/cast.h"
#include "dlog/dlog.h"
#include <iostream>

using uimg::utils::unsigned_cast;

/**
 * @file cast_demo.cpp
 * @brief Demonstration of unsigned_cast functionality
 * @author Piotr Likus
 * @date June 20, 2025
 */

int main() {
    DLOG_INFO("Starting unsigned_cast demonstration");
    
    // Successful casts using template function syntax
    DLOG_INFO("=== Successful casts (template function syntax) ===");
    
    try {
        int positive_int = 42;
        unsigned int result1 = unsigned_cast<unsigned int>(positive_int);
        auto logger = dlog::Logger::getInstance();
        logger->info("Positive int {0} -> unsigned int {1}", positive_int, result1);
        
        long positive_long = 12345L;
        unsigned long result2 = unsigned_cast<unsigned long>(positive_long);
        logger->info("Positive long {0} -> unsigned long {1}", positive_long, result2);
        
        float positive_float = 99.7f;
        unsigned int result3 = unsigned_cast<unsigned int>(positive_float);
        logger->info("Positive float {0:.1f} -> unsigned int {1} (truncated)", positive_float, result3);
        
        char zero_char = 0;
        unsigned char result4 = unsigned_cast<unsigned char>(zero_char);
        logger->info("Zero char {0} -> unsigned char {1}", (int)zero_char, (int)result4);
        
    } catch (const uimg::utils::UnsafeCastException& e) {
        auto logger = dlog::Logger::getInstance();
        logger->error("Unexpected exception during successful casts: {0}", e.what());
        return 1;
    }
    
    // Failed casts (demonstrating safety) - using template function
    DLOG_INFO("=== Failed casts (template function - no location info) ===");
    
    try {
        int negative_int = -42;
        unsigned int result = unsigned_cast<unsigned int>(negative_int);
        auto logger = dlog::Logger::getInstance();
        logger->error("ERROR: This should not be reached! Result: {0}", result);
        return 1;
    } catch (const uimg::utils::UnsafeCastException& e) {
        auto logger = dlog::Logger::getInstance();
        logger->info("✓ Correctly caught negative int cast: {0}", e.what());
        logger->info("  Exception location: {0}:{1} in {2}", e.getFile(), e.getLine(), e.getFunction());
    }
    
    // Failed casts using macro (with location information)
    DLOG_INFO("=== Failed casts (macro - with accurate location info) ===");
    
    try {
        float negative_float = -12.5f;
        unsigned long result = UNSIGNED_CAST(unsigned long, negative_float);
        auto logger = dlog::Logger::getInstance();
        logger->error("ERROR: This should not be reached! Result: {0}", result);
        return 1;
    } catch (const uimg::utils::UnsafeCastException& e) {
        auto logger = dlog::Logger::getInstance();
        logger->info("✓ Correctly caught negative float cast: {0}", e.what());
        logger->info("  Exception location: {0}:{1} in {2}", e.getFile(), e.getLine(), e.getFunction());
    }
    
    try {
        double very_small_negative = -0.001;
        unsigned int result = UNSIGNED_CAST(unsigned int, very_small_negative);
        auto logger = dlog::Logger::getInstance();
        logger->error("ERROR: This should not be reached! Result: {0}", result);
        return 1;
    } catch (const uimg::utils::UnsafeCastException& e) {
        auto logger = dlog::Logger::getInstance();
        logger->info("✓ Correctly caught small negative double cast: {0}", e.what());
        logger->info("  Exception location: {0}:{1} in {2}", e.getFile(), e.getLine(), e.getFunction());
    }
    
    DLOG_INFO("=== Two approaches available ===");
    DLOG_INFO("1. Template function: unsigned_cast<unsigned int>(value) - standard syntax");
    DLOG_INFO("2. Macro: UNSIGNED_CAST(unsigned int, value) - accurate location info");
    DLOG_INFO("=== Validation can be disabled ===");
    DLOG_INFO("To disable validation, compile with -DUIMG_DISABLE_CAST_VALIDATION");
    DLOG_INFO("This allows unsafe casts for performance-critical code after testing");
    
    DLOG_INFO("unsigned_cast demonstration completed successfully!");
    return 0;
}
