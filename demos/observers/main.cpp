#include <iostream>
#include "dlog/dlog.h"

int main() {
    std::cout << "Observers Demo - dlog library demonstration" << std::endl;
    
    DLOG_INFO("Observers demo started");
    DLOG_DEBUG("Demonstrating observer pattern");
    DLOG_INFO("Observer registered");
    DLOG_WARN("Observer warning example");
    DLOG_INFO("Observers demo completed");
    
    std::cout << "Demo completed successfully!" << std::endl;
    return 0;
}
