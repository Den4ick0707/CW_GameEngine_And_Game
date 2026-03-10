#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "window.h"
#include "OrthographicCamera.h"
#include "Renderer2D.h"
#include "render_command.h"

using namespace Engine::Graphics;

int main() {
    Engine::Core::WindowProps props("Batch Renderer Magic!", 1280, 720);
    Engine::Core::Window window(props);

    Renderer2D::Init();

    OrthographicCamera camera(-1.777f * 2.0f, 1.777f * 2.0f, -2.0f, 2.0f);

    while (!window.ShouldClose()) {
        RenderCommand::SetClearColor({ 0.05f, 0.05f, 0.05f, 1.0f });
        RenderCommand::Clear();

        float time = static_cast<float>(glfwGetTime());

        Renderer2D::BeginScene(camera);

        for (int y = -10; y < 10; ++y) {
            for (int x = -10; x < 10; ++x) {
                glm::vec2 pos(x * 0.35f, y * 0.35f);
                float distance   = glm::length(pos);
                float scaleValue = (std::sin(time * 3.0f - distance) * 0.5f + 0.5f) * 0.25f + 0.05f;

                glm::vec4 color((x + 10) / 20.0f, 0.4f, (y + 10) / 20.0f, 1.0f);

                Renderer2D::DrawQuad(pos, { scaleValue, scaleValue }, color);
            }
        }

        Renderer2D::EndScene();

        window.Update();
    }

    Renderer2D::Shutdown();
    return 0;
}