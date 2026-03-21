#pragma once

namespace Engine::Core {

    class Application;

    /// @brief Статичний менеджер часу (аналог Unity Time).
    /// Не викликай Init/Update вручну — це робить Application.
    class Time {
    public:
        /// @return Час з початку програми (секунди).
        [[nodiscard]] static double GetTime()           { return m_Time; }

        /// @return Delta time (секунди, double).
        [[nodiscard]] static double GetDeltaTime()      { return m_DeltaTime; }

        /// @return Delta time (секунди, float) — для ігрової логіки.
        [[nodiscard]] static float  GetDeltaTimeFloat() { return static_cast<float>(m_DeltaTime); }

        /// @return Delta time (мілісекунди, float).
        [[nodiscard]] static float  GetDeltaTimeMs()    { return static_cast<float>(m_DeltaTime * 1000.0); }

        /// @return Поточний FPS.
        [[nodiscard]] static float  GetFPS()            { return m_DeltaTime > 0.0 ? static_cast<float>(1.0 / m_DeltaTime) : 0.0f; }

    private:
        static void Init();
        static void Update();
        friend class Application;

        static double m_LastFrameTime;
        static double m_DeltaTime;
        static double m_Time;
    };

} // namespace Engine::Core