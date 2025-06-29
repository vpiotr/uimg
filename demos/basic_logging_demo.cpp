#include <iostream>
#include "dlog/dlog.h"

int main() {
    std::cout << "Basic Logging Demo - dlog demonstration" << std::endl;
    
    DLOG_INFO("Basic logging demo started");
    DLOG_DEBUG("Debug message example");
    DLOG_INFO("Information message example");
    DLOG_WARN("Warning message example");
    DLOG_ERROR("Error message example");
    DLOG_INFO("Basic logging demo completed");
    
    std::cout << "Demo completed successfully!" << std::endl;
    return 0;
}
