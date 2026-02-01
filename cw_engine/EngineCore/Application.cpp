#include "Application.h"
#include <GLFW/glfw3.h>

namespace Engine {
    Application::Application() {
        m_Window = std::make_unique<Window>(1280, 720, "CW Engine");
        // Тут ініціалізація подій (Events), логера, тощо
    }

    Application::~Application() {}

    void Application::Run() {
        while (m_Running) {
            float time = (float)glfwGetTime();
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            // 1. Оновлення логіки (фізика, AI)
            OnUpdate(deltaTime);

            // 2. Рендерінг
            m_Window->OnUpdate(); // SwapBuffers і PollEvents тут
            OnRender();

            if (m_Window->ShouldClose()) m_Running = false;
        }
    }
}