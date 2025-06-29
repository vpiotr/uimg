/**
 * @file dlog.h
 * @brief dlog - A lightweight, feature-rich C++11 micro-logging library
 * 
 * dlog is a header-only logging library designed for simplicity and performance.
 * It provides multiple log levels, flexible formatting, runtime configuration,
 * and both console and buffer-based output.
 *
 * Features:
 * - Six log levels: TRACE, DEBUG, INFO, WARN, ERROR, FATAL
 * - Printf-style formatting with positional arguments ({0}, {1}, ...)
 * - Width and justification formatting for strings and numbers
 * - Runtime log level configuration
 * - Console output with optional timestamps
 * - Internal circular buffer for log storage and retrieval
 * - Custom callback support for external log handling
 * - Thread-safe singleton design
 * - C++11 compatible
 *
 * Basic Usage:
 * @code
 * auto logger = dlog::Logger::getInstance();
 * logger->setLevel(dlog::LogLevel::DEBUG);
 * 
 * logger->info("Simple message");
 * logger->error("Error code: {0}", 404);
 * logger->debug("User: {0}, Score: {1:.2f}", "Alice", 87.65);
 * @endcode
 *
 * Advanced Formatting:
 * @code
 * // Width and justification
 * logger->info("Name: '{0:10}' Score: '{1:6.1f}'", "Bob", 92.3);
 * logger->info("Left: '{0:-10}' Right: '{1:10}'", "text", "value");
 * 
 * // Printf-style specifiers
 * logger->info("Hex: {0:x}, Float: {1:.3f}, Scientific: {2:.2e}", 255, 3.14159, 1234.5);
 * @endcode
 *
 * Configuration:
 * @code
 * logger->setConsoleEnabled(true);     // Enable/disable console output
 * logger->setBufferEnabled(true);      // Enable/disable buffer storage
 * logger->setTimestampEnabled(true);   // Add timestamps to console output
 * logger->setBufferLimit(500);         // Set buffer size (default: 1000)
 * @endcode
 *
 * @author Generated for uimg project
 * @version 1.0
 * @date 2025
 */

#ifndef __DLOG_H__
#define __DLOG_H__

#include <iostream>
#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <cstdio>
#include <chrono>
#include <iomanip>
#include <cctype>
#include <type_traits>

/**
 * @namespace dlog
 * @brief Main namespace for the dlog logging library
 * 
 * Contains all classes, enums, and constants for the logging system.
 */
namespace dlog {

/**
 * @brief Default size for the internal log buffer
 * 
 * This constant defines the default number of log entries that can be stored
 * in the internal circular buffer before old entries are overwritten.
 */
constexpr size_t DEFAULT_LOG_BUFFER_ENTRIES = 1000;

/**
 * @enum LogLevel
 * @brief Enumeration of available log levels
 * 
 * Log levels are ordered by severity, from TRACE (lowest) to FATAL (highest).
 * The OFF level can be used to disable all logging.
 * 
 * @note Messages are only logged if their level is >= the current logger level
 */
enum class LogLevel { 
    TRACE = 0,  ///< Detailed information for debugging
    DEBUG = 1,  ///< General debugging information
    INFO = 2,   ///< Informational messages
    WARN = 3,   ///< Warning messages
    ERROR = 4,  ///< Error conditions
    FATAL = 5,  ///< Critical error conditions
    OFF = 6     ///< Disables all logging
};

/**
 * @struct LogEntry
 * @brief Represents a single log entry with timestamp, level, and message
 * 
 * This structure is used internally by the LogBuffer to store log entries.
 * Each entry captures the exact time it was created, its severity level,
 * and the formatted message content.
 */
struct LogEntry {
    std::chrono::system_clock::time_point timestamp;  ///< When the log entry was created
    LogLevel level;                                    ///< Severity level of the entry
    std::string message;                               ///< Formatted log message
    
    /**
     * @brief Construct a new LogEntry
     * @param lvl The log level for this entry
     * @param msg The message content
     */
    LogEntry(LogLevel lvl, const std::string& msg) 
        : timestamp(std::chrono::system_clock::now()), level(lvl), message(msg) {}
};

/**
 * @class LogBuffer
 * @brief Internal circular buffer for storing log entries
 * 
 * The LogBuffer maintains a fixed-size circular buffer of log entries.
 * When the buffer reaches its maximum capacity, older entries are
 * automatically removed to make room for new ones.
 * 
 * This class is used internally by the Logger and provides methods
 * for adding entries, clearing the buffer, and retrieving entries
 * filtered by log level.
 */
class LogBuffer {
private:
    std::vector<LogEntry> logs_;    ///< Storage for log entries
    size_t maxEntries_;             ///< Maximum number of entries to store

public:
    /**
     * @brief Construct a new LogBuffer
     * @param maxEntries Maximum number of entries to store (default: DEFAULT_LOG_BUFFER_ENTRIES)
     */
    LogBuffer(size_t maxEntries = DEFAULT_LOG_BUFFER_ENTRIES) : maxEntries_(maxEntries) {}
    
    /**
     * @brief Construct a new LogBuffer (compatibility constructor)
     * @param level Unused parameter for compatibility
     * @param maxEntries Maximum number of entries to store
     */
    LogBuffer(LogLevel /*level*/, size_t maxEntries = DEFAULT_LOG_BUFFER_ENTRIES) : maxEntries_(maxEntries) {}
    
    /**
     * @brief Add a new log entry to the buffer
     * @param level The log level of the entry
     * @param message The message content
     * 
     * If the buffer is at capacity, the oldest entry will be removed.
     */
    void add(LogLevel level, const std::string& message) {
        logs_.emplace_back(level, message);
        if (logs_.size() > maxEntries_) {
            logs_.erase(logs_.begin());
        }
    }
    
    /**
     * @brief Clear all entries from the buffer
     */
    void clear() {
        logs_.clear();
    }
    
    /**
     * @brief Get all entries in the buffer
     * @return Const reference to all log entries
     */
    const std::vector<LogEntry>& getEntries() const {
        return logs_;
    }
    
    /**
     * @brief Get all entries with level >= minLevel
     * @param minLevel Minimum log level to include
     * @return Vector of log entries matching the criteria
     */
    const std::vector<LogEntry> getEntries(LogLevel minLevel) const {
        std::vector<LogEntry> result;
        for (const auto& entry : logs_) {
            if (entry.level >= minLevel) {
                result.push_back(entry);
            }
        }
        return result;
    }
    
    /**
     * @brief Get count of entries with level >= minLevel
     * @param minLevel Minimum log level to include (default: TRACE)
     * @return Number of entries matching the criteria
     */
    size_t getCount(LogLevel minLevel = LogLevel::TRACE) const {
        size_t count = 0;
        for (const auto& entry : logs_) {
            if (entry.level >= minLevel) {
                count++;
            }
        }
        return count;
    }
    
    /**
     * @brief Get total number of entries in the buffer
     * @return Total number of log entries stored
     */
    size_t size() const { return logs_.size(); }
};

/**
 * @class Logger
 * @brief Main logging interface providing formatted logging with multiple outputs
 * 
 * The Logger class is implemented as a thread-safe singleton that provides:
 * - Multiple log levels with runtime filtering
 * - Printf-style formatting with positional arguments ({0}, {1}, ...)
 * - Width and justification formatting for strings and numbers
 * - Console output with optional timestamps
 * - Internal buffer storage for log retrieval
 * - Custom callback support for external log processing
 * 
 * Usage Examples:
 * @code
 * auto logger = dlog::Logger::getInstance();
 * 
 * // Basic logging
 * logger->info("User logged in");
 * logger->error("Failed to open file: {0}", filename);
 * 
 * // Advanced formatting
 * logger->debug("User: '{0:15}' Score: '{1:6.2f}' Rank: #{2:3d}", name, score, rank);
 * logger->warn("Progress: {0:6.1f}% complete", progress);
 * @endcode
 * 
 * @note This class uses the Meyer's singleton pattern for thread safety
 */
class Logger {
private:
    LogLevel level_ = LogLevel::INFO;                                       ///< Current minimum log level
    bool consoleEnabled_ = true;                                            ///< Whether to output to console
    bool bufferEnabled_ = true;                                             ///< Whether to store in internal buffer
    bool timestampEnabled_ = false;                                         ///< Whether to add timestamps to console output
    std::function<void(LogLevel, const std::string&)> callback_ = nullptr;  ///< Optional custom callback
    LogBuffer buffer_;                                                      ///< Internal log storage

    /**
     * @brief Private constructor for singleton pattern
     */
    Logger() = default;

    /**
     * @brief Format current timestamp as HH:MM:SS.mmm
     * @return Formatted timestamp string
     */
    std::string formatTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

public:
    /**
     * @brief Get the singleton Logger instance
     * @return Pointer to the global Logger instance
     * 
     * This method provides thread-safe access to the singleton Logger instance
     * using the Meyer's singleton pattern.
     */
    static Logger* getInstance() {
        static Logger instance;
        return &instance;
    }

    // Configuration Methods
    
    /**
     * @brief Set the minimum log level
     * @param level Messages below this level will be filtered out
     * 
     * Only messages with level >= the set level will be processed.
     * Use LogLevel::OFF to disable all logging.
     */
    void setLevel(LogLevel level) { level_ = level; }
    
    /**
     * @brief Get the current minimum log level
     * @return Current log level threshold
     */
    LogLevel getLevel() const { return level_; }

    /**
     * @brief Enable or disable console output
     * @param enabled true to enable console output, false to disable
     */
    void setConsoleEnabled(bool enabled) { consoleEnabled_ = enabled; }
    
    /**
     * @brief Enable or disable internal buffer storage
     * @param enabled true to enable buffer storage, false to disable
     */
    void setBufferEnabled(bool enabled) { bufferEnabled_ = enabled; }
    
    /**
     * @brief Enable or disable timestamps in console output
     * @param enabled true to add timestamps, false to omit them
     * 
     * @note Timestamps are only added to console output, not buffer storage
     */
    void setTimestampEnabled(bool enabled) { timestampEnabled_ = enabled; }
    
    /**
     * @brief Set the maximum number of entries in the internal buffer
     * @param limit Maximum number of log entries to store
     * 
     * When the limit is reached, older entries are automatically removed.
     */
    void setBufferLimit(size_t limit) { buffer_ = LogBuffer(limit); }

    /**
     * @brief Set a custom callback for log processing
     * @param callback Function to call for each log message (level, message)
     * 
     * The callback receives the log level and formatted message for each
     * log entry that passes the level filter.
     */
    void setLogCallback(std::function<void(LogLevel, const std::string&)> callback) {
        callback_ = callback;
    }

    /**
     * @brief Clear the custom callback
     * 
     * Removes any previously set custom callback function.
     */
    void clearLogCallback() {
        callback_ = nullptr;
    }

    // Utility Methods
    
    /**
     * @brief Convert log level to string representation
     * @param level The log level to convert
     * @return String name of the log level (e.g., "INFO", "ERROR")
     */
    static std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            case LogLevel::OFF: return "OFF";
            default: return "UNKNOWN";
        }
    }

    /**
     * @brief Convert string to log level
     * @param str String representation of log level
     * @return Corresponding LogLevel enum value (defaults to INFO if unknown)
     */
    static LogLevel stringToLevel(const std::string& str) {
        if (str == "TRACE") return LogLevel::TRACE;
        if (str == "DEBUG") return LogLevel::DEBUG;
        if (str == "INFO") return LogLevel::INFO;
        if (str == "WARN") return LogLevel::WARN;
        if (str == "ERROR") return LogLevel::ERROR;
        if (str == "FATAL") return LogLevel::FATAL;
        if (str == "OFF") return LogLevel::OFF;
        return LogLevel::INFO;
    }

    // Core Logging Method
    
    /**
     * @brief Log a message at the specified level
     * @param level The severity level of the message
     * @param message The formatted message to log
     * 
     * This is the core logging method that handles output routing to
     * console, buffer, and custom callbacks based on current configuration.
     */
    void log(LogLevel level, const std::string& message) {
        if (level < level_ || level_ == LogLevel::OFF) return;

        std::string finalMessage = message;
        if (timestampEnabled_) {
            finalMessage = "[" + formatTimestamp() + "] " + message;
        }

        // Write to callback if assigned
        if (callback_) {
            callback_(level, finalMessage);
        }

        // Write to console if enabled
        if (consoleEnabled_) {
            std::cout << "[" << levelToString(level) << "] " << finalMessage << std::endl;
        }

        // Write to buffer if enabled
        if (bufferEnabled_) {
            buffer_.add(level, message); // Store original message without timestamp in buffer
        }
    }

private:
    // Helper for streaming parameter substitution with format support
    template<typename... Args>
    void log_stream(LogLevel level, const std::string& format, Args&&... args) {
        if (level < level_ || level_ == LogLevel::OFF) return;
        
        std::string result = format;
        
        // Store arguments as strings with their formatters
        std::vector<std::pair<std::string, std::function<std::string(const std::string&)>>> formattedArgs;
        addArgsToVector(formattedArgs, std::forward<Args>(args)...);
        
        // Replace {0}, {1}, {0:.2f}, etc. with actual arguments
        for (size_t i = 0; i < formattedArgs.size(); ++i) {
            // Look for formatted placeholders like {0:.2f}
            std::string formatPattern = "{" + std::to_string(i) + ":";
            size_t pos = 0;
            while ((pos = result.find(formatPattern, pos)) != std::string::npos) {
                // Find the closing brace
                size_t endPos = result.find("}", pos);
                if (endPos != std::string::npos) {
                    // Extract format specifier (e.g., ".2f")
                    std::string formatSpec = result.substr(pos + formatPattern.length(), 
                                                          endPos - pos - formatPattern.length());
                    std::string placeholder = result.substr(pos, endPos - pos + 1);
                    
                    // Apply formatting using the stored formatter
                    std::string formattedValue = formattedArgs[i].second(formatSpec);
                    result.replace(pos, placeholder.length(), formattedValue);
                    pos += formattedValue.length();
                } else {
                    break;
                }
            }
            
            // Look for simple placeholders like {0}
            std::string simplePlaceholder = "{" + std::to_string(i) + "}";
            pos = 0;
            while ((pos = result.find(simplePlaceholder, pos)) != std::string::npos) {
                result.replace(pos, simplePlaceholder.length(), formattedArgs[i].first);
                pos += formattedArgs[i].first.length();
            }
        }
        
        log(level, result);
    }

    // Simple formatting functions for different types
    
    // Float/double formatting (supports precision and width)
    std::string formatValue(float value, const std::string& formatSpec) const {
        return formatDoubleValue(static_cast<double>(value), formatSpec);
    }
    
    std::string formatValue(double value, const std::string& formatSpec) const {
        return formatDoubleValue(value, formatSpec);
    }
    
    // Integer formatting (supports dec/hex/oct and width)
    std::string formatValue(int value, const std::string& formatSpec) const {
        return formatIntegerValue(static_cast<long long>(value), formatSpec);
    }
    
    std::string formatValue(long value, const std::string& formatSpec) const {
        return formatIntegerValue(static_cast<long long>(value), formatSpec);
    }
    
    std::string formatValue(long long value, const std::string& formatSpec) const {
        return formatIntegerValue(value, formatSpec);
    }
    
    std::string formatValue(unsigned int value, const std::string& formatSpec) const {
        return formatIntegerValue(static_cast<long long>(value), formatSpec);
    }
    
    std::string formatValue(unsigned long value, const std::string& formatSpec) const {
        return formatIntegerValue(static_cast<long long>(value), formatSpec);
    }
    
    std::string formatValue(unsigned long long value, const std::string& formatSpec) const {
        return formatIntegerValue(static_cast<long long>(value), formatSpec);
    }
    
    // Character formatting
    std::string formatValue(char value, const std::string& formatSpec) const {
        // If format spec looks like integer format, treat as int
        if (!formatSpec.empty() && (formatSpec.back() == 'd' || formatSpec.back() == 'x' || formatSpec.back() == 'o')) {
            return formatIntegerValue(static_cast<long long>(value), formatSpec);
        }
        
        std::string str(1, value);
        return applyStringFormatting(str, formatSpec);
    }
    
    // Boolean formatting
    std::string formatValue(bool value, const std::string& formatSpec) const {
        std::string str = value ? "true" : "false";
        return applyStringFormatting(str, formatSpec);
    }
    
    // String formatting (supports width and justification)
    std::string formatValue(const std::string& value, const std::string& formatSpec) const {
        return applyStringFormatting(value, formatSpec);
    }
    
    std::string formatValue(const char* value, const std::string& formatSpec) const {
        return applyStringFormatting(std::string(value), formatSpec);
    }
    
    // Generic fallback for other types
    template<typename T>
    std::string formatValue(const T& value, const std::string& formatSpec) const {
        std::ostringstream oss;
        oss << value;
        return applyStringFormatting(oss.str(), formatSpec);
    }
    
private:
    // Helper functions for specific type formatting
    std::string formatDoubleValue(double value, const std::string& formatSpec) const {
        if (formatSpec.empty()) {
            return std::to_string(value);
        }
        
        char buffer[256];
        std::string printfFormat = "%" + formatSpec;
        snprintf(buffer, sizeof(buffer), printfFormat.c_str(), value);
        return std::string(buffer);
    }
    
    std::string formatIntegerValue(long long value, const std::string& formatSpec) const {
        if (formatSpec.empty()) {
            return std::to_string(value);
        }
        
        char buffer[256];
        std::string printfFormat = "%" + formatSpec;
        snprintf(buffer, sizeof(buffer), printfFormat.c_str(), value);
        return std::string(buffer);
    }
    
    // Helper function for string width and justification
    std::string applyStringFormatting(const std::string& value, const std::string& formatSpec) const {
        if (formatSpec.empty()) {
            return value;
        }
        
        // Parse width and justification (e.g., "10", "-10")
        bool leftJustified = false;
        int width = 0;
        
        if (!formatSpec.empty() && formatSpec[0] == '-') {
            leftJustified = true;
            if (formatSpec.length() > 1) {
                width = std::atoi(formatSpec.substr(1).c_str());
            }
        } else {
            width = std::atoi(formatSpec.c_str());
        }
        
        if (width <= 0 || width <= static_cast<int>(value.length())) {
            return value;
        }
        
        int padding = width - static_cast<int>(value.length());
        if (leftJustified) {
            return value + std::string(static_cast<size_t>(padding), ' ');
        } else {
            return std::string(static_cast<size_t>(padding), ' ') + value;
        }
    }

    // Recursive helper to convert arguments to formatted pairs (C++11 compatible)
    template<typename T>
    void addArgsToVector(std::vector<std::pair<std::string, std::function<std::string(const std::string&)>>>& vec, T&& arg) {
        std::ostringstream oss;
        oss << arg;
        std::string stringValue = oss.str();
        
        // Create formatter function for this argument
        auto formatter = [this, arg](const std::string& formatSpec) -> std::string {
            return formatValue(arg, formatSpec);
        };
        
        vec.emplace_back(stringValue, formatter);
    }

    template<typename T, typename... Args>
    void addArgsToVector(std::vector<std::pair<std::string, std::function<std::string(const std::string&)>>>& vec, T&& first, Args&&... rest) {
        addArgsToVector(vec, std::forward<T>(first));
        addArgsToVector(vec, std::forward<Args>(rest)...);
    }

public:
    // ========== Main Logging Interface ==========
    
    /**
     * @name Simple String Logging Methods
     * @brief Log simple string messages at different levels
     * @{
     */
    
    /** @brief Log a TRACE level message */
    void trace(const std::string& message) { log(LogLevel::TRACE, message); }
    
    /** @brief Log a DEBUG level message */
    void debug(const std::string& message) { log(LogLevel::DEBUG, message); }
    
    /** @brief Log an INFO level message */
    void info(const std::string& message) { log(LogLevel::INFO, message); }
    
    /** @brief Log a WARN level message */
    void warn(const std::string& message) { log(LogLevel::WARN, message); }
    
    /** @brief Log an ERROR level message */
    void error(const std::string& message) { log(LogLevel::ERROR, message); }
    
    /** @brief Log a FATAL level message */
    void fatal(const std::string& message) { log(LogLevel::FATAL, message); }
    
    /** @} */

    /**
     * @name Formatted Logging Methods
     * @brief Log formatted messages using positional arguments and printf-style specifiers
     * 
     * These methods support:
     * - Positional placeholders: {0}, {1}, {2}, ...
     * - Printf-style formatting: {0:.2f}, {1:d}, {2:x}, etc.
     * - Width and justification: {0:10}, {1:-15}, etc.
     * 
     * Examples:
     * @code
     * logger->info("User {0} has {1} points", "Alice", 150);
     * logger->debug("Value: {0:.3f}, Hex: {1:08x}", 3.14159, 255);
     * logger->warn("Status: '{0:-10}' Progress: {1:6.1f}%", "Running", 67.8);
     * @endcode
     * @{
     */
    
    /** @brief Log a formatted TRACE level message */
    template<typename... Args>
    void trace(const std::string& format, Args&&... args) {
        log_stream(LogLevel::TRACE, format, std::forward<Args>(args)...);
    }

    /** @brief Log a formatted DEBUG level message */
    template<typename... Args>
    void debug(const std::string& format, Args&&... args) {
        log_stream(LogLevel::DEBUG, format, std::forward<Args>(args)...);
    }

    /** @brief Log a formatted INFO level message */
    template<typename... Args>
    void info(const std::string& format, Args&&... args) {
        log_stream(LogLevel::INFO, format, std::forward<Args>(args)...);
    }

    /** @brief Log a formatted WARN level message */
    template<typename... Args>
    void warn(const std::string& format, Args&&... args) {
        log_stream(LogLevel::WARN, format, std::forward<Args>(args)...);
    }

    /** @brief Log a formatted ERROR level message */
    template<typename... Args>
    void error(const std::string& format, Args&&... args) {
        log_stream(LogLevel::ERROR, format, std::forward<Args>(args)...);
    }

    /** @brief Log a formatted FATAL level message */
    template<typename... Args>
    void fatal(const std::string& format, Args&&... args) {
        log_stream(LogLevel::FATAL, format, std::forward<Args>(args)...);
    }
    
    /** @} */

    // ========== Buffer Access Methods ==========
    
    /**
     * @name Buffer Access and Management
     * @brief Methods for accessing and managing the internal log buffer
     * @{
     */
    
    /**
     * @brief Get direct access to the internal buffer
     * @return Const reference to the LogBuffer
     */
    const LogBuffer& buffer() const { return buffer_; }
    
    /**
     * @brief Get buffer entries filtered by minimum log level
     * @param minLevel Minimum log level to include (default: TRACE)
     * @return Vector of log entries matching the criteria
     * 
     * @code
     * // Get all entries
     * auto allEntries = logger->getBufferEntries();
     * 
     * // Get only errors and above
     * auto errors = logger->getBufferEntries(dlog::LogLevel::ERROR);
     * @endcode
     */
    const std::vector<LogEntry> getBufferEntries(LogLevel minLevel = LogLevel::TRACE) const {
        return buffer_.getEntries(minLevel);
    }

    /**
     * @brief Clear all entries from the internal buffer
     */
    void clearBuffer() { buffer_.clear(); }

    /**
     * @brief Get count of buffer entries at or above specified level
     * @param level Minimum log level to count (default: TRACE)
     * @return Number of entries matching the criteria
     */
    size_t getBufferCount(LogLevel level = LogLevel::TRACE) const {
        return buffer_.getCount(level);
    }
    
    /** @} */
};

/**
 * @name Convenience Logging Macros
 * @brief Global macros for quick logging access
 * 
 * These macros provide a convenient way to log messages without explicitly
 * getting the Logger instance. They automatically use the singleton instance.
 * 
 * @code
 * DLOG_INFO("Application started");
 * DLOG_ERROR("Critical error occurred");
 * @endcode
 * 
 * @note These macros only support simple string messages. For formatted 
 *       logging, use the Logger instance methods directly.
 * @{
 */

/** @brief Log TRACE message using global macro */
#define DLOG_TRACE(msg) dlog::Logger::getInstance()->trace(msg)

/** @brief Log DEBUG message using global macro */
#define DLOG_DEBUG(msg) dlog::Logger::getInstance()->debug(msg)

/** @brief Log INFO message using global macro */
#define DLOG_INFO(msg) dlog::Logger::getInstance()->info(msg)

/** @brief Log WARN message using global macro */
#define DLOG_WARN(msg) dlog::Logger::getInstance()->warn(msg)

/** @brief Log ERROR message using global macro */
#define DLOG_ERROR(msg) dlog::Logger::getInstance()->error(msg)

/** @brief Log FATAL message using global macro */
#define DLOG_FATAL(msg) dlog::Logger::getInstance()->fatal(msg)

/** @} */

} // namespace dlog

#endif // __DLOG_H__
