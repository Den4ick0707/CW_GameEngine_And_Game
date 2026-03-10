#include "application.h"

#include "logger.h"
#include "input.h"
#include "time_manager.h"

namespace Engine::Core {
    Application *Application::s_Instance = nullptr;

    Application::Application() {
        s_Instance = this;

        Logger::GetInstance().Init(LogLevel::Trace, LogTarget::Console | LogTarget::File);
        CW_INFO_LOG("Engine Systems Initializing...");

        WindowProps props("CW Engine Game", 1280, 720);
        m_Window = std::make_unique<Window>(props);

        Time::Init();

        auto &input = Input::Get();
        input.BindAction("Exit", KeyCode::Escape);

        CW_INFO_LOG("All Systems Ready.");
    }

    Application::~Application() {
        CW_INFO_LOG("Engine Shutting Down...");
    }

    void Application::Run() {
        while (m_Running) {
            Time::Update();
            float dt = Time::GetDeltaTimeFloat();

            Input::Get().Update();

            if (m_Window->ShouldClose()) {
                m_Running = false;
            }

            Update(dt);
            Render();
            m_Window->Update();
        }
    }
}
