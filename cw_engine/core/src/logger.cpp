#include "core_pch.h"
#include "logger.h"

namespace Engine::Core {

    Logger::~Logger() {
        if (m_FileStream.is_open())
            m_FileStream.close();
    }

    void Logger::Init(LogLevel minLevel, int targets, const std::string& filepath) {
        std::lock_guard lock(m_Mutex);
        m_MinLevel = minLevel;
        m_Targets  = targets;

        if (m_Targets & LogTarget::File) {
            m_FileStream.open(filepath, std::ios::out | std::ios::app);
            if (!m_FileStream.is_open()) {
                std::cerr << "[LOGGER] Cannot open: " << filepath << "\n";
                m_Targets &= ~LogTarget::File;
            }
        }
    }

    std::string Logger::CurrentTime() const {
        std::time_t t  = std::time(nullptr);
        std::tm     tm = {};
#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        char buf[12];
        std::strftime(buf, sizeof(buf), "[%H:%M:%S]", &tm);
        return buf;
    }

    const char* Logger::LevelToString(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace: return "[TRACE]";
            case LogLevel::Info:  return "[INFO] ";
            case LogLevel::Warn:  return "[WARN] ";
            case LogLevel::Error: return "[ERROR]";
            case LogLevel::Fatal: return "[FATAL]";
        }
        return "[?????]";
    }

    const char* Logger::LevelToColor(LogLevel level) const {
        switch (level) {
            case LogLevel::Trace: return "\033[90m";   // сірий
            case LogLevel::Info:  return "\033[32m";   // зелений
            case LogLevel::Warn:  return "\033[33m";   // жовтий
            case LogLevel::Error: return "\033[31m";   // червоний
            case LogLevel::Fatal: return "\033[41m";   // червоний фон
        }
        return "\033[0m";
    }

    void Logger::Log(LogLevel level, const std::string& message) {
        if (level < m_MinLevel) return;

        std::lock_guard lock(m_Mutex);

        const std::string time = CurrentTime();
        const char* lvl        = LevelToString(level);

        if (m_Targets & LogTarget::Console)
            std::cout << LevelToColor(level)
                      << time << " " << lvl << " " << message
                      << "\033[0m\n";

        if ((m_Targets & LogTarget::File) && m_FileStream.is_open())
            m_FileStream << time << " " << lvl << " " << message << "\n",
            m_FileStream.flush();
    }

} // namespace Engine::Core