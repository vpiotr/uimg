# dlog - Micro-Logging Library

A lightweight, feature-rich C++11 header-only logging library designed for simplicity and performance.

## Features

- **Six Log Levels**: TRACE, DEBUG, INFO, WARN, ERROR, FATAL
- **Flexible Formatting**: Printf-style with positional arguments ({0}, {1}, ...)
- **Advanced Formatting**: Width, justification, precision control
- **Multiple Outputs**: Console, internal buffer, custom callbacks
- **Runtime Configuration**: Log levels, output toggles, timestamps
- **Thread-Safe**: Singleton design with thread safety
- **C++11 Compatible**: No modern C++ features required
- **Header-Only**: Single file inclusion

## Quick Start

### Basic Usage

```cpp
#include "dlog/dlog.h"

int main() {
    auto logger = dlog::Logger::getInstance();
    
    logger->info("Application started");
    logger->warn("This is a warning");
    logger->error("Error code: {0}", 404);
    
    return 0;
}
```

### Configuration

```cpp
auto logger = dlog::Logger::getInstance();

// Set minimum log level
logger->setLevel(dlog::LogLevel::DEBUG);

// Configure outputs
logger->setConsoleEnabled(true);      // Enable console output
logger->setBufferEnabled(true);       // Enable internal buffer
logger->setTimestampEnabled(true);    // Add timestamps to console

// Set buffer size
logger->setBufferLimit(500);          // Store up to 500 entries
```

## Formatting Examples

### Basic Formatting

```cpp
logger->info("User: {0}, Age: {1}", "Alice", 25);
logger->debug("Processing file: {0}", filename);
logger->error("Failed after {0} attempts", retryCount);
```

### Printf-Style Specifiers

```cpp
// Floating point precision
logger->info("Pi = {0:.3f}", 3.14159);           // "Pi = 3.142"
logger->info("Scientific: {0:.2e}", 123456.0);   // "Scientific: 1.23e+05"

// Integer formatting
logger->info("Decimal: {0:d}", 255);             // "Decimal: 255"
logger->info("Hexadecimal: {0:x}", 255);         // "Hexadecimal: ff"
logger->info("Hex uppercase: {0:X}", 255);       // "Hex uppercase: FF"

// Mixed formatting
logger->info("User {0}: Score {1:.1f}%, Rank #{2:d}", name, score, rank);
```

### Width and Justification

```cpp
// Right-justified (default)
logger->info("Name: '{0:10}' Score: '{1:6.1f}'", "Bob", 87.5);
// Output: "Name: '       Bob' Score: '  87.5'"

// Left-justified (with minus prefix)
logger->info("Name: '{0:-10}' Score: '{1:-6.1f}'", "Bob", 87.5);
// Output: "Name: 'Bob       ' Score: '87.5  '"

// Table-like formatting
logger->info("| {0:-12} | {1:8.2f} | {2:6d} |", "Product", 29.99, 15);
// Output: "| Product      |    29.99 |     15 |"
```

## Log Levels

| Level | Value | Description |
|-------|-------|-------------|
| TRACE | 0 | Detailed debugging information |
| DEBUG | 1 | General debugging information |
| INFO  | 2 | Informational messages |
| WARN  | 3 | Warning messages |
| ERROR | 4 | Error conditions |
| FATAL | 5 | Critical error conditions |
| OFF   | 6 | Disable all logging |

### Level Filtering

```cpp
logger->setLevel(dlog::LogLevel::WARN);

logger->debug("Not shown");  // Filtered out
logger->info("Not shown");   // Filtered out
logger->warn("Shown");       // Displayed
logger->error("Shown");      // Displayed
```

## Buffer Access

The internal buffer stores log entries for later retrieval and analysis.

```cpp
// Get all entries
auto allEntries = logger->getBufferEntries();

// Get only errors and above
auto errors = logger->getBufferEntries(dlog::LogLevel::ERROR);

// Process entries
for (const auto& entry : errors) {
    std::cout << dlog::Logger::levelToString(entry.level) 
              << ": " << entry.message << std::endl;
}

// Clear buffer
logger->clearBuffer();

// Get entry count
size_t errorCount = logger->getBufferCount(dlog::LogLevel::ERROR);
```

## Custom Callbacks

Register a custom function to process log messages:

```cpp
logger->setLogCallback([](dlog::LogLevel level, const std::string& message) {
    // Send to external logging system
    if (level >= dlog::LogLevel::ERROR) {
        sendToErrorReporting(message);
    }
});

// Clear callback
logger->clearLogCallback();
```

## Convenience Macros

For simple string logging without getting the logger instance:

```cpp
DLOG_TRACE("Detailed debug info");
DLOG_DEBUG("Debug message");
DLOG_INFO("Information");
DLOG_WARN("Warning");
DLOG_ERROR("Error occurred");
DLOG_FATAL("Critical failure");
```

**Note**: Macros only support simple strings. For formatted logging, use the Logger instance methods.

## Advanced Configuration

### Timestamps

```cpp
logger->setTimestampEnabled(true);
logger->info("Message with timestamp");
// Console output: "14:30:25.123 [INFO] Message with timestamp"

logger->setTimestampEnabled(false);
logger->info("Message without timestamp");
// Console output: "[INFO] Message without timestamp"
```

### Buffer Management

```cpp
// Configure buffer size
logger->setBufferLimit(1000);  // Store up to 1000 entries

// Buffer behavior: circular (oldest entries removed when full)
for (int i = 0; i < 1500; ++i) {
    logger->info("Message {0}", i);
}
// Buffer contains messages 500-1499 (1000 most recent)
```

### Output Control

```cpp
// Disable console output (useful for testing or background services)
logger->setConsoleEnabled(false);

// Disable buffer storage (saves memory)
logger->setBufferEnabled(false);

// Completely disable logging
logger->setLevel(dlog::LogLevel::OFF);
```

## Format Specification Reference

### Placeholder Syntax

- `{0}`, `{1}`, `{2}`, ... - Positional arguments
- `{0:spec}` - Argument with format specification

### Format Specifiers

#### Floating Point
- `{0:.2f}` - 2 decimal places
- `{0:.4f}` - 4 decimal places  
- `{0:.2e}` - Scientific notation with 2 decimal places
- `{0:.3E}` - Scientific notation (uppercase E)

#### Integers
- `{0:d}` - Decimal format
- `{0:x}` - Hexadecimal (lowercase)
- `{0:X}` - Hexadecimal (uppercase)
- `{0:o}` - Octal format

#### Width and Justification
- `{0:10}` - Right-justified in 10 characters
- `{0:-10}` - Left-justified in 10 characters
- `{0:08d}` - Zero-padded decimal in 8 characters

#### Combined Examples
```cpp
logger->info("Value: {0:8.2f} Hex: {1:04X}", 3.14159, 255);
// Output: "Value:     3.14 Hex: 00FF"
```

## Thread Safety

The Logger uses the Meyer's singleton pattern which is thread-safe in C++11:

```cpp
// Safe to call from multiple threads
auto logger = dlog::Logger::getInstance();
logger->info("Thread-safe logging");
```

## Performance Considerations

- **Header-only**: No linking required
- **Minimal overhead**: Level filtering happens before formatting
- **Efficient formatting**: Uses snprintf for numeric conversions
- **Circular buffer**: Automatic memory management
- **No heap allocation**: For basic string logging

```cpp
// Fast path: filtered out immediately if level too low
logger->setLevel(dlog::LogLevel::ERROR);
logger->debug("This check is very fast");  // No formatting occurs
```

## Integration

### CMake Integration

```cmake
# Add include directory
target_include_directories(your_target PRIVATE path/to/include)

# Or for header-only
target_sources(your_target PRIVATE path/to/include/dlog/dlog.h)
```

### Compiler Compatibility

- Requires C++11 or later
- Tested with GCC 4.8+, Clang 3.3+, MSVC 2013+
- No external dependencies beyond standard library

## Examples

### Application Logging

```cpp
#include "dlog/dlog.h"

class Application {
private:
    dlog::Logger* logger;
    
public:
    Application() : logger(dlog::Logger::getInstance()) {
        logger->setLevel(dlog::LogLevel::INFO);
        logger->setTimestampEnabled(true);
    }
    
    void start() {
        logger->info("Application starting...");
        
        if (!initialize()) {
            logger->fatal("Failed to initialize");
            return;
        }
        
        logger->info("Application ready");
        run();
    }
    
private:
    bool initialize() {
        logger->debug("Loading configuration...");
        // ... initialization code ...
        logger->debug("Configuration loaded successfully");
        return true;
    }
    
    void run() {
        int processed = 0;
        while (running) {
            if (processItem()) {
                processed++;
                if (processed % 100 == 0) {
                    logger->info("Processed {0} items", processed);
                }
            }
        }
        logger->info("Processing complete. Total: {0} items", processed);
    }
};
```

### Error Reporting Integration

```cpp
void setupErrorReporting() {
    auto logger = dlog::Logger::getInstance();
    
    logger->setLogCallback([](dlog::LogLevel level, const std::string& message) {
        if (level >= dlog::LogLevel::ERROR) {
            // Send to external error reporting service
            ErrorReporting::send(dlog::Logger::levelToString(level), message);
        }
        
        if (level == dlog::LogLevel::FATAL) {
            // Trigger immediate notification
            AlertSystem::sendUrgent("FATAL: " + message);
        }
    });
}
```

### Testing and Debugging

```cpp
void testLogging() {
    auto logger = dlog::Logger::getInstance();
    
    // Capture logs in buffer for testing
    logger->setConsoleEnabled(false);
    logger->setBufferEnabled(true);
    logger->setLevel(dlog::LogLevel::DEBUG);
    logger->clearBuffer();
    
    // Run test code
    functionUnderTest();
    
    // Verify log output
    auto entries = logger->getBufferEntries(dlog::LogLevel::ERROR);
    assert(entries.size() == 0);  // No errors expected
    
    auto debugEntries = logger->getBufferEntries(dlog::LogLevel::DEBUG);
    assert(debugEntries.size() > 0);  // Some debug output expected
}
```

## License

Part of the uimg project. See project license for details.
