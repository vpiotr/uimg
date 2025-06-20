#include "samples/test_chart3d.h"
#include "utest/utest.h"
#include <iostream>
#include <exception>

int main(int /*argc*/, const char* /*argv*/[]) {
    printf("Chart3D Comprehensive Test Suite\n");
    printf("================================\n\n");
    
    try {
        // Run the comprehensive chart3d tests
        runChart3DBasicTests();
        
        printf("\n=== TEST SUMMARY ===\n");
        printf("All chart3d tests completed successfully!\n");
        
        return 0;
    } catch (const utest::AssertionException& e) {
        printf("\n=== TEST FAILURE ===\n");
        printf("Test failed: %s\n", e.what());
        printf("File: %s\n", e.getFile().c_str());
        printf("Line: %d\n", e.getLine());
        printf("Function: %s\n", e.getFunction().c_str());
        return 1;
    } catch (const std::exception& e) {
        printf("\n=== UNEXPECTED ERROR ===\n");
        printf("Error: %s\n", e.what());
        return 1;
    } catch (...) {
        printf("\n=== UNKNOWN ERROR ===\n");
        printf("An unknown error occurred during testing\n");
        return 1;
    }
}
