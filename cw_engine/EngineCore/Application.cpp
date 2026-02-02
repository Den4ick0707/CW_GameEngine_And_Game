#include "Application.h"
#include "Graphics/Renderer2D.h"
#include "Input.h"

#include <GLFW/glfw3.h>

namespace Engine {
    Application::Application() {
        m_Window = std::make_unique<Window>(1280, 720, "CW Engine");
        Input::Init(m_Window->GetNativeWindow());
        Renderer2D::Init();
    }

    Application::~Application() {
        Renderer2D::Shutdown();
    }

    void Application::Run() {
        while (m_Running) {
            //  Abstract time management away from GLFW
            float time = (float) glfwGetTime();
            float deltaTime = time - m_LastFrameTime;
            m_LastFrameTime = time;

            OnUpdate(deltaTime);

            m_Window->OnUpdate();
            OnRender();

            if (m_Window->ShouldClose()) {
                m_Running = false;
            }
        }
    }
}
