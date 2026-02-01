#include "Application.h"
#include <GLFW/glfw3.h>

namespace Engine {
    namespace Core {
        Application::Application() {
            m_Window = std::make_unique<Window>(1280, 720, "CW Engine");
        }

        Application::~Application() {
        }

        void Application::Run() {
            while (m_Running) {
                float time = (float) glfwGetTime();
                float deltaTime = time - m_LastFrameTime;
                m_LastFrameTime = time;

                OnUpdate(deltaTime);

                m_Window->OnUpdate();
                OnRender();

                if (m_Window->ShouldClose()) m_Running = false;
            }
        }
    }
}
