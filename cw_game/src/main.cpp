#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "window.h"
#include "OrthographicCamera.h"
#include "Renderer2D.h"
#include "render_command.h"
#include "texture_2d.h"

using namespace Engine::Graphics;

int main() {
    // --- Вікно ---
    Engine::Core::WindowProps props("Renderer2D — Texture Support", 1280, 720);
    Engine::Core::Window window(props);

    // --- Ініціалізація рендерера ---
    Renderer2D::Init();

    // --- Камера (aspect ratio 1280/720 ≈ 1.777) ---
    OrthographicCamera camera(-1.777f * 3.0f, 1.777f * 3.0f, -3.0f, 3.0f);

    // --- Завантажуємо тестові текстури ---
    // Поклади будь-які PNG у res/textures/
    // Якщо файлів немає — DrawQuad з кольором все одно працює
    std::shared_ptr<Texture> texA = nullptr;
    std::shared_ptr<Texture> texB = nullptr;

    try {
        texA = std::make_shared<Texture>("res/textures/tile.png");
        texB = std::make_shared<Texture>("res/textures/logo.png");
    } catch (...) {
        std::cerr << "[WARN] Textures not found — using color-only quads\n";
    }

    // --- Головний цикл ---
    Renderer2D::ResetStats();

    while (!window.ShouldClose()) {

        float time = static_cast<float>(glfwGetTime());

        // --- Очищення ---
        RenderCommand::SetClearColor({ 0.05f, 0.05f, 0.08f, 1.0f });
        RenderCommand::Clear();

        Renderer2D::BeginScene(camera);

        // --- Сітка кольорових квадратів (використовує слот 0 = білий піксель) ---
        for (int y = -5; y < 5; ++y) {
            for (int x = -5; x < 5; ++x) {
                glm::vec2 pos(x * 0.55f, y * 0.55f);
                glm::vec4 col(
                    (x + 5) / 10.0f,
                    0.3f,
                    (y + 5) / 10.0f,
                    1.0f
                );
                float pulse = (std::sin(time * 2.0f - glm::length(pos)) * 0.5f + 0.5f)
                              * 0.35f + 0.15f;
                Renderer2D::DrawQuad(pos, { pulse, pulse }, col);
            }
        }

        // --- Текстурований квадрат A (якщо є) ---
        if (texA) {
            Renderer2D::DrawQuad(
                { -1.5f, 0.0f, 0.1f },   // z=0.1 → трохи поверх сітки
                { 1.5f, 1.5f },
                texA
            );
        }

        // --- Текстурований квадрат B з тінтом (якщо є) ---
        if (texB) {
            Renderer2D::DrawQuad(
                { 1.5f, 0.0f, 0.1f },
                { 1.5f, 1.5f },
                texB,
                { 1.0f, 0.8f, 0.5f, 1.0f }  // жовтуватий тінт
            );
        }

        Renderer2D::EndScene();

        // --- Debug статистика у заголовок вікна ---
        auto stats = Renderer2D::GetStats();
        std::string title = "Renderer2D | Draw calls: "
                          + std::to_string(stats.DrawCalls)
                          + "  Quads: "
                          + std::to_string(stats.QuadCount);
        glfwSetWindowTitle(window.GetNativeWindow(), title.c_str());
        Renderer2D::ResetStats();

        window.Update();
    }

    Renderer2D::Shutdown();
    return 0;
}