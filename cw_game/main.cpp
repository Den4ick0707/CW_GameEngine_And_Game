#include <EngineCore/Application.h>
#include <iostream>

class Sandbox : public Engine::Application {
public:
    Sandbox() {
        std::cout << "Game Started!" << std::endl;
    }

    void OnUpdate(float ts) override {
        // Тут буде код гри:
        // if (Engine::Input::IsKeyPressed(CW_KEY_W)) player.Move(ts);
    }

    void OnRender() override {
        // Тут малювання:
        // Renderer::DrawQuad(...);
    }
};

Engine::Application* Engine::CreateApplication() {
    return new Sandbox();
}

// А точка входу main() схована в самому рушії (наприклад в EntryPoint.h),
// або може бути тут, якщо ти не хочеш ускладнювати:
int main() {
    auto app = Engine::CreateApplication();
    app->Run();
    delete app;
}