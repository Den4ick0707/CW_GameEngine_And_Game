#include "application.h"
#include "logger.h"
#include "input.h"
#include "time_manager.h"

namespace Engine::Core {

    Application* Application::s_Instance = nullptr;

    Application::Application() {
        s_Instance = this;

        Logger::GetInstance().Init(
            LogLevel::Trace,
            LogTarget::Console | LogTarget::File,
            "engine.log");

        CW_INFO_LOG("=== CW Engine v0.1 Starting ===");

        m_Window = std::make_unique<Window>(WindowProps("CW Engine", 1280, 720));
        Time::Init();

        // Escape завжди закриває гру
        auto& input = Input::Get();
        input.BindAction("Exit", KeyCode::Escape);
        input.Subscribe("Exit", [this]() { Quit(); });

        CW_INFO_LOG("Application initialized.");
    }

    Application::~Application() {
        CW_INFO_LOG("=== CW Engine Shutdown ===");
    }

    void Application::Run() {
        while (m_Running) {
            Time::Update();
            float dt = Time::GetDeltaTimeFloat();

            Input::Get().Update();

            if (m_Window->ShouldClose())
                m_Running = false;

            Update(dt);
            Render();
            m_Window->Update();
        }
    }

} // namespace Engine::Core