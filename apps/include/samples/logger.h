#pragma once
#include <cstdio>
#include <string>
#include <iostream>
#include <utility>
#include <algorithm>
#include <memory>

/**
 * @brief Logging levels for demo applications
 */
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    OFF = 5
};

/**
 * @brief Generic logger for all demo applications
 */
class DemoLogger {
public:
    DemoLogger(LogLevel level = LogLevel::OFF) : level_(level) {}
    
    void setLevel(LogLevel level) { level_ = level; }
    LogLevel getLevel() const { return level_; }
    bool isEnabled(LogLevel level) const { return level >= level_; }
    
    template<typename... Args>
    void trace(const char* fmt, Args... args) const {
        if (isEnabled(LogLevel::TRACE)) {
            printf("[TRACE] ");
            printf(fmt, args...);
            printf("\n");
        }
    }
    
    template<typename... Args>
    void debug(const char* fmt, Args... args) const {
        if (isEnabled(LogLevel::DEBUG)) {
            printf("[DEBUG] ");
            printf(fmt, args...);
            printf("\n");
        }
    }
    
    template<typename... Args>
    void info(const char* fmt, Args... args) const {
        if (isEnabled(LogLevel::INFO)) {
            printf("[INFO] ");
            printf(fmt, args...);
            printf("\n");
        }
    }
    
    template<typename... Args>
    void warn(const char* fmt, Args... args) const {
        if (isEnabled(LogLevel::WARN)) {
            printf("[WARN] ");
            printf(fmt, args...);
            printf("\n");
        }
    }
    
    template<typename... Args>
    void error(const char* fmt, Args... args) const {
        if (isEnabled(LogLevel::ERROR)) {
            printf("[ERROR] ");
            printf(fmt, args...);
            printf("\n");
        }
    }
    
    static std::shared_ptr<DemoLogger> getInstance() {
        static std::shared_ptr<DemoLogger> instance = std::make_shared<DemoLogger>();
        return instance;
    }
    
    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::OFF: return "OFF";
            default: return "UNKNOWN";
        }
    }
    
    static LogLevel stringToLevel(const std::string& levelStr) {
        std::string upper = levelStr;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
        
        if (upper == "TRACE") return LogLevel::TRACE;
        if (upper == "DEBUG") return LogLevel::DEBUG;
        if (upper == "INFO") return LogLevel::INFO;
        if (upper == "WARN") return LogLevel::WARN;
        if (upper == "ERROR") return LogLevel::ERROR;
        if (upper == "OFF") return LogLevel::OFF;
        
        return LogLevel::OFF; // Default to OFF for unrecognized levels
    }

private:
    LogLevel level_;
};
