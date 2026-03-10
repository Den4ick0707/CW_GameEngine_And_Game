#ifndef CW_ENGINE_CORE_TIME_H
#define CW_ENGINE_CORE_TIME_H

namespace Engine::Core {
    /// Forward declaration
    class Application;

    /// @brief Global static time management class.
    /// @details Provides access to delta time and total time from anywhere in the code.
    /// Mimics the behavior of Unity's Time class.
    class Time {
    public:

        /// @name Getters
        /// @brief Get time in different formats
        /// @{

        /// @return Time since the start of the program
        [[nodiscard]] static double GetTime() { return m_Time; }

        /// @return Delta time (double precision)
        /// @details Usually returns time in seconds
        [[nodiscard]] static double GetDeltaTime() { return m_DeltaTime; }

        /// @return Delta time (float precision)
        /// @details Most common usage for gameplay logic
        [[nodiscard]] static float GetDeltaTimeFloat() { return static_cast<float>(m_DeltaTime); }

        /// @return Delta time in milliseconds (float)
        /// @see GetDeltaTime()
        [[nodiscard]] static float GetDeltaTimeMs() { return static_cast<float>(m_DeltaTime * 1000.0); }

        /// @}

    private:
        /// @brief Initialize time (called by Application)
        static void Init();

        /// @brief Update delta time (called by Application loop)
        static void Update();

        friend class Application;

        static double m_LastFrameTime;
        static double m_DeltaTime;
        static double m_Time;
    };
}

#endif // CW_ENGINE_CORE_TIME_H