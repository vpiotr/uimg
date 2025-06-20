#include "utest/utest.h"
#include "dlog/dlog.h"

UTEST_FUNC_DEF(BasicLogging) {
    auto logger = dlog::Logger::getInstance();
    logger->setLevel(dlog::LogLevel::DEBUG);
    logger->setConsoleEnabled(false); // Disable console for test
    logger->setBufferEnabled(true);
    logger->clearBuffer();
    
    // Test that basic logging doesn't crash
    DLOG_TRACE("This is a trace message");
    DLOG_DEBUG("This is a debug message");
    DLOG_INFO("This is an info message");
    DLOG_WARN("This is a warning message");
    DLOG_ERROR("This is an error message");
    
    // Verify messages were logged to buffer
    auto entries = logger->getBufferEntries();
    UTEST_ASSERT_GT(entries.size(), 0);
}

UTEST_FUNC_DEF(LogLevels) {
    auto logger = dlog::Logger::getInstance();
    logger->setConsoleEnabled(false);
    logger->setBufferEnabled(true);
    logger->clearBuffer();
    
    // Test level filtering
    logger->setLevel(dlog::LogLevel::WARN);
    
    DLOG_DEBUG("This debug should NOT appear");
    DLOG_INFO("This info should NOT appear");
    DLOG_WARN("This warning SHOULD appear");
    DLOG_ERROR("This error SHOULD appear");
    
    auto entries = logger->getBufferEntries();
    UTEST_ASSERT_EQUALS(entries.size(), 2); // Only WARN and ERROR should appear
    
    // Verify the entries are the correct levels
    UTEST_ASSERT_EQUALS(static_cast<int>(entries[0].level), static_cast<int>(dlog::LogLevel::WARN));
    UTEST_ASSERT_EQUALS(static_cast<int>(entries[1].level), static_cast<int>(dlog::LogLevel::ERROR));
}

UTEST_FUNC_DEF(ConsoleBufferToggle) {
    auto logger = dlog::Logger::getInstance();
    logger->setLevel(dlog::LogLevel::DEBUG);
    logger->setBufferEnabled(true);
    logger->clearBuffer();
    
    // Test console disabled, buffer enabled
    logger->setConsoleEnabled(false);
    DLOG_DEBUG("This message goes to buffer only");
    
    auto bufferEntries = logger->getBufferEntries();
    UTEST_ASSERT_EQUALS(bufferEntries.size(), 1);
    UTEST_ASSERT_STR_EQUALS(bufferEntries[0].message, "This message goes to buffer only");
}

UTEST_FUNC_DEF(TemplateLogging) {
    auto logger = dlog::Logger::getInstance();
    logger->setLevel(dlog::LogLevel::DEBUG);
    logger->setConsoleEnabled(false);
    logger->setBufferEnabled(true);
    logger->clearBuffer();
    
    // Test debug_pair functionality
    logger->debug("user_id = {0}", 12345);
    logger->debug("balance = {0}", 1234.56);
    logger->debug("is_premium = {0}", true);
    
    auto entries = logger->getBufferEntries();
    UTEST_ASSERT_EQUALS(entries.size(), 3);
}

UTEST_FUNC_DEF(StringConversions) {
    // Test levelToString
    UTEST_ASSERT_STR_EQUALS(dlog::Logger::levelToString(dlog::LogLevel::DEBUG), "DEBUG");
    UTEST_ASSERT_STR_EQUALS(dlog::Logger::levelToString(dlog::LogLevel::INFO), "INFO");
    UTEST_ASSERT_STR_EQUALS(dlog::Logger::levelToString(dlog::LogLevel::WARN), "WARN");
    UTEST_ASSERT_STR_EQUALS(dlog::Logger::levelToString(dlog::LogLevel::ERROR), "ERROR");
    UTEST_ASSERT_STR_EQUALS(dlog::Logger::levelToString(dlog::LogLevel::FATAL), "FATAL");
    
    // Test stringToLevel
    UTEST_ASSERT_EQUALS(static_cast<int>(dlog::Logger::stringToLevel("DEBUG")), static_cast<int>(dlog::LogLevel::DEBUG));
    UTEST_ASSERT_EQUALS(static_cast<int>(dlog::Logger::stringToLevel("INFO")), static_cast<int>(dlog::LogLevel::INFO));
    UTEST_ASSERT_EQUALS(static_cast<int>(dlog::Logger::stringToLevel("INVALID")), static_cast<int>(dlog::LogLevel::INFO)); // Default
}

UTEST_FUNC_DEF(LogBuffer) {
    dlog::LogBuffer memLogger;
    
    memLogger.add(dlog::LogLevel::DEBUG, "Debug message");
    memLogger.add(dlog::LogLevel::INFO, "Info message");
    memLogger.add(dlog::LogLevel::WARN, "Warning message");
    
    auto allLogs = memLogger.getEntries();
    UTEST_ASSERT_EQUALS(allLogs.size(), 3);
    
    auto warnLogs = memLogger.getEntries(dlog::LogLevel::WARN);
    UTEST_ASSERT_EQUALS(warnLogs.size(), 1);
    UTEST_ASSERT_EQUALS(static_cast<int>(warnLogs[0].level), static_cast<int>(dlog::LogLevel::WARN));
}

UTEST_FUNC_DEF(BufferFunctionality) {
    auto logger = dlog::Logger::getInstance();
    logger->setLevel(dlog::LogLevel::DEBUG);
    logger->setConsoleEnabled(false);
    logger->setBufferEnabled(true);
    logger->setBufferLimit(3);
    logger->clearBuffer();
    
    // Add some messages
    DLOG_DEBUG("Message 1");
    DLOG_INFO("Message 2");
    DLOG_WARN("Message 3");
    DLOG_ERROR("Message 4"); // This should cause oldest to be removed
    
    auto buffer = logger->getBufferEntries();
    UTEST_ASSERT_EQUALS(buffer.size(), 3); // Limited to 3
    UTEST_ASSERT_STR_EQUALS(buffer[0].message, "Message 2"); // Message 1 should be removed
    UTEST_ASSERT_STR_EQUALS(buffer[2].message, "Message 4");
    
    // Test filtered buffer access
    auto warnAndAbove = logger->getBufferEntries(dlog::LogLevel::WARN);
    UTEST_ASSERT_EQUALS(warnAndAbove.size(), 2); // WARN and ERROR
}

int main() {
    UTEST_PROLOG();
    
    UTEST_FUNC(BasicLogging);
    UTEST_FUNC(LogLevels);
    UTEST_FUNC(ConsoleBufferToggle);
    UTEST_FUNC(TemplateLogging);
    UTEST_FUNC(StringConversions);
    UTEST_FUNC(LogBuffer);
    UTEST_FUNC(BufferFunctionality);
    
    UTEST_EPILOG();
}
