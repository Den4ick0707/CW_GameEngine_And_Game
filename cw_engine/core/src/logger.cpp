#include "core_pch.h"
#include "logger.h"

#include <chrono>


namespace Engine::Core {
    Logger::Logger() {
        m_MinLevel = LogLevel::Trace;
        m_Targets = LogTarget::Console;
    }

    Logger::~Logger() {
        if (m_FileStream.is_open()) {
            m_FileStream.close();
        }
    }

    void Logger::Init(LogLevel minLevel, int targets, const std::string &filepath) {
        std::lock_guard<std::mutex> lock(m_Mutex);

        m_MinLevel = minLevel;
        m_Targets = targets;

        if (m_Targets & LogTarget::File) {
            m_FileStream.open(filepath, std::ios::out | std::ios::app);

            if (!m_FileStream.is_open()) {
                std::cerr << "[LOGGER] Failed to open file: " << filepath << std::endl;
                m_Targets &= ~LogTarget::File;
            }
        }
    }

    std::string Logger::CurrentTime() {
        std::time_t t = std::time(nullptr);
        std::tm tm;

#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif

        char buffer[12]; // Достатньо для "[HH:MM:SS]\0"
        std::strftime(buffer, sizeof(buffer), "[%H:%M:%S]", &tm);
        return std::string(buffer);
    }

    void Logger::Log(LogLevel level, const std::string &message) {
        if (level < m_MinLevel) return;

        std::lock_guard<std::mutex> lock(m_Mutex);

        const char *levelStr = "";
        const char *colorCode = "";
        const char *resetColor = "\033[0m";

        switch (level) {
            case LogLevel::Trace: levelStr = "[TRACE]";
                colorCode = "\033[90m";
                break;
            case LogLevel::Info: levelStr = "[INFO] ";
                colorCode = "\033[32m";
                break;
            case LogLevel::Warn: levelStr = "[WARN] ";
                colorCode = "\033[33m";
                break;
            case LogLevel::Error: levelStr = "[ERROR]";
                colorCode = "\033[31m";
                break;
            case LogLevel::Fatal: levelStr = "[FATAL]";
                colorCode = "\033[41m";
                break;
        }

        std::string timeStr = CurrentTime();

        if (m_Targets & LogTarget::Console) {
            std::cout << colorCode << timeStr << " " << levelStr << " " << message << resetColor << "\n";
        }

        if ((m_Targets & LogTarget::File) && m_FileStream.is_open()) {
            m_FileStream << timeStr << " " << levelStr << " " << message << "\n";
            m_FileStream.flush();
        }
    }
}
