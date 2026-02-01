#include "Application.h"
#include "Application.h"
#include "Input.h"

namespace Engine {
    namespace Core {
        Application *Application::s_Instance = nullptr;

        Application::Application() {
            s_Instance = this;

            m_Window = std::make_unique<Window>(800, 600, "Coursework Engine");

            Input::Init(m_Window->GetNativeWindow());
        }

        Application::~Application() {
        }

        void Application::Run() {
            OnInit();

            while (m_Running) {
                float time = (float) glfwGetTime();
                float timestep = time - m_LastFrameTime;
                m_LastFrameTime = time;

                if (m_Window->ShouldClose()) {
                    m_Running = false;
                }

                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                OnUpdate(timestep);

                m_Window->OnUpdate();
            }

            OnShutdown();
        }
    }
}
