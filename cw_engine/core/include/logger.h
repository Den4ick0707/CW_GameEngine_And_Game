#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <memory>

namespace Engine::Core {

    enum class LogLevel { Trace = 0, Info, Warn, Error, Fatal };

    enum LogTarget {
        None    = 0,
        Console = 1 << 0,
        File    = 1 << 1
    };

    /// @brief Thread-safe Singleton Logger.
    /// Підтримує вивід у консоль (з кольорами) та у файл одночасно.
    class Logger {
    public:
        Logger(const Logger&)            = delete;
        Logger& operator=(const Logger&) = delete;

        static Logger& GetInstance() {
            static Logger instance;
            return instance;
        }

        /// @brief Налаштування логера. Викликай один раз на старті.
        /// @param minLevel  Мінімальний рівень для виводу.
        /// @param targets   Куди писати (Console | File).
        /// @param filepath  Шлях до лог-файлу (якщо File увімкнено).
        void Init(LogLevel           minLevel = LogLevel::Trace,
                  int                targets  = LogTarget::Console,
                  const std::string& filepath = "engine.log");

        void Log(LogLevel level, const std::string& message);

    private:
        Logger()  = default;
        ~Logger();

        std::string CurrentTime() const;
        const char* LevelToString(LogLevel level) const;
        const char* LevelToColor (LogLevel level) const;

        LogLevel      m_MinLevel = LogLevel::Trace;
        int           m_Targets  = LogTarget::Console;
        std::ofstream m_FileStream;
        mutable std::mutex m_Mutex;
    };

    // ── Helpers ───────────────────────────────────────────────────────────────

    template<typename... Args>
    std::string FormatString(const std::string& fmt, Args&&... args) {
        int sz = std::snprintf(nullptr, 0, fmt.c_str(),
                               std::forward<Args>(args)...) + 1;
        if (sz <= 0) return "Format error";
        auto buf = std::make_unique<char[]>(static_cast<size_t>(sz));
        std::snprintf(buf.get(), static_cast<size_t>(sz),
                      fmt.c_str(), std::forward<Args>(args)...);
        return { buf.get(), buf.get() + sz - 1 };
    }

    // Перевантаження для рядка без аргументів — уникаємо зайвого snprintf
    inline std::string FormatString(const std::string& msg) { return msg; }

} // namespace Engine::Core

// ── Logging Macros ────────────────────────────────────────────────────────────
#if defined(CW_DEBUG) || defined(CW_ENABLE_LOGGING)

#define CW_TRACE_LOG(...) ::Engine::Core::Logger::GetInstance().Log( \
    ::Engine::Core::LogLevel::Trace, ::Engine::Core::FormatString(__VA_ARGS__))
#define CW_INFO_LOG(...)  ::Engine::Core::Logger::GetInstance().Log( \
    ::Engine::Core::LogLevel::Info,  ::Engine::Core::FormatString(__VA_ARGS__))
#define CW_WARN_LOG(...)  ::Engine::Core::Logger::GetInstance().Log( \
    ::Engine::Core::LogLevel::Warn,  ::Engine::Core::FormatString(__VA_ARGS__))
#define CW_ERROR_LOG(...) ::Engine::Core::Logger::GetInstance().Log( \
    ::Engine::Core::LogLevel::Error, ::Engine::Core::FormatString(__VA_ARGS__))
#define CW_FATAL_LOG(...) ::Engine::Core::Logger::GetInstance().Log( \
    ::Engine::Core::LogLevel::Fatal, ::Engine::Core::FormatString(__VA_ARGS__))

#else
#define CW_TRACE_LOG(...)
#define CW_INFO_LOG(...)
#define CW_WARN_LOG(...)
#define CW_ERROR_LOG(...)
#define CW_FATAL_LOG(...)
#endif