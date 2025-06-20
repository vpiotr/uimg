#include "utest/utest.h"
#include "uimg/utils/cast.h"

using uimg::utils::unsigned_cast;

/**
 * @file test_unsigned_cast_failures.cpp
 * @brief Tests for unsigned_cast failure cases (negative values)
 * @author Piotr Likus
 * @date June 20, 2025
 */

UTEST_FUNC_DEF(NegativeInt_ThrowsException) {
    int negative_int = -42;
    
    bool exception_caught = false;
    try {
        unsigned_cast<unsigned int>(negative_int);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown");
}

UTEST_FUNC_DEF(NegativeLong_ThrowsException) {
    long negative_long = -12345L;
    
    bool exception_caught = false;
    try {
        unsigned_cast<unsigned long>(negative_long);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown");
}

UTEST_FUNC_DEF(NegativeLongLong_ThrowsException) {
    long long negative_long_long = -123456789LL;
    
    bool exception_caught = false;
    try {
        unsigned_cast<unsigned long long>(negative_long_long);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown");
}

UTEST_FUNC_DEF(NegativeChar_ThrowsException) {
    char negative_char = -1;
    
    bool exception_caught = false;
    try {
        unsigned_cast<unsigned char>(negative_char);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown");
}

UTEST_FUNC_DEF(NegativeFloat_ThrowsException) {
    float negative_float = -42.7f;
    
    bool exception_caught = false;
    try {
        unsigned_cast<unsigned int>(negative_float);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown");
}

UTEST_FUNC_DEF(NegativeDouble_ThrowsException) {
    double negative_double = -12345.67;
    
    bool exception_caught = false;
    try {
        unsigned_cast<unsigned long>(negative_double);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown");
}

UTEST_FUNC_DEF(NegativeLongDouble_ThrowsException) {
    long double negative_long_double = -98765.43L;
    
    bool exception_caught = false;
    try {
        unsigned_cast<unsigned long long>(negative_long_double);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown");
}

UTEST_FUNC_DEF(ExceptionContainsDetails) {
    int negative_value = -123;
    
    try {
        UNSIGNED_CAST(unsigned int, negative_value);
        UTEST_ASSERT_TRUE_MSG(false, "Expected exception was not thrown");
    } catch (const uimg::utils::UnsafeCastException& e) {
        // Check that exception message contains the negative value
        std::string message = e.what();
        UTEST_ASSERT_STR_CONTAINS(message, "-123");
        UTEST_ASSERT_STR_CONTAINS(message, "negative value");
        
        // Check that file information is present
        UTEST_ASSERT_STR_CONTAINS(e.getFile(), "test_unsigned_cast_failures.cpp");
        UTEST_ASSERT_GT(e.getLine(), 0);
        UTEST_ASSERT_STR_NOT_EQUALS(e.getFunction(), "");
    }
}

UTEST_FUNC_DEF(VeryNegativeValues) {
    // Test with very negative values
    int very_negative = -2147483647; // Near INT_MIN
    
    bool exception_caught = false;
    try {
        unsigned_cast<unsigned int>(very_negative);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown for very negative int");
    
    float very_negative_float = -999999.99f;
    
    exception_caught = false;
    try {
        unsigned_cast<unsigned int>(very_negative_float);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown for very negative float");
}

UTEST_FUNC_DEF(SmallNegativeFloat) {
    // Test with very small negative float values
    float small_negative = -0.1f;
    
    bool exception_caught = false;
    try {
        unsigned_cast<unsigned int>(small_negative);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown for small negative float");
    
    double small_negative_double = -0.00001;
    
    exception_caught = false;
    try {
        unsigned_cast<unsigned long>(small_negative_double);
    } catch (const uimg::utils::UnsafeCastException& e) {
        exception_caught = true;
    }
    UTEST_ASSERT_TRUE_MSG(exception_caught, "Expected UnsafeCastException was not thrown for small negative double");
}

int main() {
    UTEST_PROLOG();
    
    UTEST_FUNC(NegativeInt_ThrowsException);
    UTEST_FUNC(NegativeLong_ThrowsException);
    UTEST_FUNC(NegativeLongLong_ThrowsException);
    UTEST_FUNC(NegativeChar_ThrowsException);
    UTEST_FUNC(NegativeFloat_ThrowsException);
    UTEST_FUNC(NegativeDouble_ThrowsException);
    UTEST_FUNC(NegativeLongDouble_ThrowsException);
    UTEST_FUNC(ExceptionContainsDetails);
    UTEST_FUNC(VeryNegativeValues);
    UTEST_FUNC(SmallNegativeFloat);
    
    UTEST_EPILOG();
    return 0;
}
