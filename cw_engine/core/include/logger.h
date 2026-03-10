#ifndef CW_ENGINE_CORE_LOGGER_H
#define CW_ENGINE_CORE_LOGGER_H
#include <fstream>

namespace Engine::Core {

    /// @brief Defines the severity level of the log message.
    enum class LogLevel {
        Trace = 0, ///< Detailed debug information (variables, flow).
        Info,      ///< General operational events (startup, shutdown).
        Warn,      ///< Something unexpected happened, but execution continues.
        Error,     ///< Operation failed, functionality might be limited.
        Fatal      ///< Critical error, application cannot continue.
    };

    /// @brief Defines where the logs should be output.
    /// @details Can be combined using bitwise OR (e.g., Console | File).
    enum LogTarget {
        None    = 0,
        Console = 1 << 0, ///< Output to standard console (stdout).
        File    = 1 << 1  ///< Output to a log file.
    };

    /// @brief Singleton class for logging events.
    /// @details Handles formatted output to console and files in a thread-safe manner.
    /// @warning You must call Init() before logging to a file.
    class Logger {
    public:
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        /// @brief Access the Singleton instance of the Logger.
        /// @return Reference to the Logger instance.
        [[nodiscard]] static Logger &GetInstance() {
            static Logger instance;
            return instance;
        }

        /// @brief Initialize the logger settings.
        /// @param minLevel Messages below this level will be ignored.
        /// @param targets Where to write logs (Console | File).
        /// @param filepath Path to the log file (if File target is enabled).
        void Init(LogLevel minLevel = LogLevel::Trace, int targets = LogTarget::Console,
                  const std::string &filepath = "engine.log");

        /// @brief Log a message with a specific severity level.
        /// @note Usually called via macros (CW_INFO_LOG, etc.).
        void Log(LogLevel level, const std::string &message);

    private:
        Logger();
        ~Logger();

        /// @brief Get current system time formatted as string [HH:MM:SS].
        std::string CurrentTime();

    private:
        LogLevel m_MinLevel = LogLevel::Trace;
        int m_Targets = LogTarget::Console;
        std::ofstream m_FileStream;
        std::mutex m_Mutex;
    };


    /// @brief Formats a string using printf-style arguments.
    /// @internal This is a helper function for macros, not intended for direct use.
    template<typename... Args>
    std::string FormatString(const std::string &format, Args... args) {
        // 1. Визначаємо розмір майбутнього рядка
        // snprintf з nullptr повертає розмір, який БУВ БИ записаний (без \0)
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;

        if (size_s <= 0) { return "Error formatting log message."; }

        auto size = static_cast<size_t>(size_s);
        auto buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return std::string(buf.get(), buf.get() + size - 1);
    }

    inline std::string FormatString(const std::string &msg) { return msg; }
}


#if defined(CW_DEBUG) || defined(CW_ENABLE_LOGGING)

/// @name Logging Macros
/// @brief Macros for logging messages. Removed in Release builds unless CW_ENABLE_LOGGING is defined.
/// @{

/// @brief Log detailed debug info (Trace level).
#define CW_TRACE_LOG(...) ::Engine::Core::Logger::GetInstance().Log(::Engine::Core::LogLevel::Trace, ::Engine::Core::FormatString(__VA_ARGS__))

/// @brief Log general information (Info level).
#define CW_INFO_LOG(...)  ::Engine::Core::Logger::GetInstance().Log(::Engine::Core::LogLevel::Info,  ::Engine::Core::FormatString(__VA_ARGS__))

/// @brief Log a warning (Warn level).
#define CW_WARN_LOG(...)  ::Engine::Core::Logger::GetInstance().Log(::Engine::Core::LogLevel::Warn,  ::Engine::Core::FormatString(__VA_ARGS__))

/// @brief Log a recoverable error (Error level).
#define CW_ERROR_LOG(...) ::Engine::Core::Logger::GetInstance().Log(::Engine::Core::LogLevel::Error, ::Engine::Core::FormatString(__VA_ARGS__))

/// @brief Log a fatal crash error (Fatal level).
#define CW_FATAL_LOG(...) ::Engine::Core::Logger::GetInstance().Log(::Engine::Core::LogLevel::Fatal, ::Engine::Core::FormatString(__VA_ARGS__))

/// @}

#else


#define CW_TRACE_LOG(...)
#define CW_INFO_LOG(...)
#define CW_WARN_LOG(...)
#define CW_ERROR_LOG(...)
#define CW_FATAL_LOG(...)

#endif // CW_DEBUG || CW_ENABLE_LOGGING

#endif // CW_ENGINE_CORE_LOGGER_H