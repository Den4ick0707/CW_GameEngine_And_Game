#include <EngineCore/Application.h>
#include <iostream>

class Sandbox : public Engine::Core::Application {
public:
    Sandbox() {
        std::cout << "Game Started!" << std::endl;
    }

    void OnUpdate(float ts) override {

    }

    void OnRender() override {

    }
};

Engine::Core::Application* Engine::Core::CreateApplication() {
    return new Sandbox();
}

int main() {
    auto app = Engine::Core::CreateApplication();
    app->Run();
    delete app;
}