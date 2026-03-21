#pragma once

#include <memory>
#include "window.h"

namespace Engine::Core {

    /// @brief Базовий клас гри. Наслідуй і реалізуй Update + Render.
    class Application {
    public:
        Application();
        virtual ~Application();

        Application(const Application&)            = delete;
        Application& operator=(const Application&) = delete;

        /// @brief Запуск головного циклу.
        void Run();

        /// @brief Ігрова логіка. dt — час кадру у секундах.
        virtual void Update(float dt) = 0;

        /// @brief Рендеринг. Викликається після Update.
        virtual void Render() = 0;

        /// @brief Зупинити гру на наступному кадрі.
        void Quit() { m_Running = false; }

        [[nodiscard]] static Application& Get()    { return *s_Instance; }
        [[nodiscard]] Window&             GetWindow() { return *m_Window; }

    private:
        static Application* s_Instance;
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
    };

    Application* CreateApplication();

} // namespace Engine::Core