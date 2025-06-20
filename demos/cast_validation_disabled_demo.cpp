#include "uimg/utils/cast.h"
#include "dlog/dlog.h"
#include <iostream>

using uimg::utils::unsigned_cast;

/**
 * @file cast_validation_disabled_demo.cpp
 * @brief Demonstration of unsigned_cast with validation disabled
 * @author Piotr Likus
 * @date June 20, 2025
 * 
 * Compile with: -DUIMG_DISABLE_CAST_VALIDATION
 */

int main() {
    DLOG_INFO("Starting unsigned_cast demonstration with validation disabled");
    
#if UIMG_ENABLE_CAST_VALIDATION
    DLOG_WARN("Validation is ENABLED - This demo should be compiled with -DUIMG_DISABLE_CAST_VALIDATION");
    DLOG_INFO("Current compilation has validation enabled, so negative casts will throw exceptions");
#else
    DLOG_INFO("Validation is DISABLED - Negative values will be cast without checks");
#endif
    
    // Test negative cast behavior
    int negative_int = -42;
    
    try {
        unsigned int result = unsigned_cast<unsigned int>(negative_int);
        auto logger = dlog::Logger::getInstance();
        
#if UIMG_ENABLE_CAST_VALIDATION
        logger->error("This should not be reached with validation enabled!");
#else
        logger->info("With validation disabled: negative int {0} -> unsigned int {1}", negative_int, result);
        logger->warn("Note: This is unsafe behavior - the negative value wrapped around!");
#endif
        
    } catch (const uimg::utils::UnsafeCastException& e) {
        auto logger = dlog::Logger::getInstance();
        logger->info("Exception caught (validation is enabled): {0}", e.what());
    }
    
    DLOG_INFO("Demo completed");
    return 0;
}
