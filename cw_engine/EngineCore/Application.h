#pragma once
#include <memory>
#include "Window.h"

namespace Engine {
    class Application {
    public:
        Application();
        virtual ~Application();

        void Run(); // Головний цикл тут

        // Клієнт (гра) буде це реалізовувати
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}

    private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        float m_LastFrameTime = 0.0f;
    };

    // Визначається в клієнті (cw_game)
    Application* CreateApplication();
}