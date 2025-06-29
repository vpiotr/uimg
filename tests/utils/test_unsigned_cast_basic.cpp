#include "utest/utest.h"
#include "uimg/utils/cast.h"
#include <limits>

using uimg::utils::unsigned_cast;

/**
 * @file test_unsigned_cast_basic.cpp
 * @brief Basic tests for unsigned_cast functionality
 * @author Piotr Likus
 * @date June 20, 2025
 */

UTEST_FUNC_DEF(PositiveToPositive_Int) {
    // Test positive int to unsigned int
    int positive_int = 42;
    unsigned int result = unsigned_cast<unsigned int>(positive_int);
    UTEST_ASSERT_EQUALS(result, 42u);
    
    // Test positive int to unsigned long
    unsigned long result_long = unsigned_cast<unsigned long>(positive_int);
    UTEST_ASSERT_EQUALS(result_long, 42ul);
}

UTEST_FUNC_DEF(PositiveToPositive_Long) {
    // Test positive long to unsigned long
    long positive_long = 12345L;
    unsigned long result = unsigned_cast<unsigned long>(positive_long);
    UTEST_ASSERT_EQUALS(result, 12345ul);
    
    // Test positive long to unsigned int (if it fits)
    long small_positive_long = 100L;
    unsigned int result_int = unsigned_cast<unsigned int>(small_positive_long);
    UTEST_ASSERT_EQUALS(result_int, 100u);
}

UTEST_FUNC_DEF(PositiveToPositive_LongLong) {
    // Test positive long long to unsigned long long
    long long positive_long_long = 123456789LL;
    unsigned long long result = unsigned_cast<unsigned long long>(positive_long_long);
    UTEST_ASSERT_EQUALS(result, 123456789ull);
}

UTEST_FUNC_DEF(PositiveToPositive_Char) {
    // Test positive char to unsigned char
    char positive_char = 65; // 'A'
    unsigned char result = unsigned_cast<unsigned char>(positive_char);
    UTEST_ASSERT_EQUALS(result, 65);
    
    // Test positive char to unsigned int
    unsigned int result_int = unsigned_cast<unsigned int>(positive_char);
    UTEST_ASSERT_EQUALS(result_int, 65u);
}

UTEST_FUNC_DEF(ZeroToZero) {
    // Test zero values for different types
    int zero_int = 0;
    unsigned int result_int = unsigned_cast<unsigned int>(zero_int);
    UTEST_ASSERT_EQUALS(result_int, 0u);
    
    long zero_long = 0L;
    unsigned long result_long = unsigned_cast<unsigned long>(zero_long);
    UTEST_ASSERT_EQUALS(result_long, 0ul);
    
    char zero_char = 0;
    unsigned char result_char = unsigned_cast<unsigned char>(zero_char);
    UTEST_ASSERT_EQUALS(result_char, 0);
}

UTEST_FUNC_DEF(PositiveToPositive_Float) {
    // Test positive float to unsigned int
    float positive_float = 42.7f;
    unsigned int result = unsigned_cast<unsigned int>(positive_float);
    UTEST_ASSERT_EQUALS(result, 42u); // Truncated
    
    // Test zero float
    float zero_float = 0.0f;
    unsigned int result_zero = unsigned_cast<unsigned int>(zero_float);
    UTEST_ASSERT_EQUALS(result_zero, 0u);
}

UTEST_FUNC_DEF(PositiveToPositive_Double) {
    // Test positive double to unsigned long
    double positive_double = 12345.67;
    unsigned long result = unsigned_cast<unsigned long>(positive_double);
    UTEST_ASSERT_EQUALS(result, 12345ul); // Truncated
    
    // Test zero double
    double zero_double = 0.0;
    unsigned long result_zero = unsigned_cast<unsigned long>(zero_double);
    UTEST_ASSERT_EQUALS(result_zero, 0ul);
}

UTEST_FUNC_DEF(PositiveToPositive_LongDouble) {
    // Test positive long double to unsigned long long
    long double positive_long_double = 98765.43L;
    unsigned long long result = unsigned_cast<unsigned long long>(positive_long_double);
    UTEST_ASSERT_EQUALS(result, 98765ull); // Truncated
    
    // Test zero long double
    long double zero_long_double = 0.0L;
    unsigned long long result_zero = unsigned_cast<unsigned long long>(zero_long_double);
    UTEST_ASSERT_EQUALS(result_zero, 0ull);
}

int main() {
    UTEST_PROLOG();
    
    UTEST_FUNC(PositiveToPositive_Int);
    UTEST_FUNC(PositiveToPositive_Long);
    UTEST_FUNC(PositiveToPositive_LongLong);
    UTEST_FUNC(PositiveToPositive_Char);
    UTEST_FUNC(ZeroToZero);
    UTEST_FUNC(PositiveToPositive_Float);
    UTEST_FUNC(PositiveToPositive_Double);
    UTEST_FUNC(PositiveToPositive_LongDouble);
    
    UTEST_EPILOG();
    return 0;
}
