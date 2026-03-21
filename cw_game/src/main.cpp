#include <iostream>
#include <vector>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Підключаємо ядро рушія та EntryPoint (який містить main)
#include "application.h"
#include "entry_point.h"
#include "engine.h"

// Графіка та ECS
#include "render_command.h"
#include "Renderer2D.h"
#include "OrthographicCamera.h"
#include "registry.h"
#include "components.h"

using namespace Engine::Core;
using namespace Engine::Graphics;
using namespace Engine::Scene;

// --- Наші клієнтські компоненти ---
struct ColorComponent {
    glm::vec4 Color{ 1.0f, 1.0f, 1.0f, 1.0f };
    ColorComponent() = default;
    ColorComponent(const glm::vec4& c) : Color(c) {}
};

struct WaveAnimationComponent {
    float Offset = 0.0f;
    float Speed = 4.0f;
    WaveAnimationComponent() = default;
    WaveAnimationComponent(float offset) : Offset(offset) {}
};

// =========================================================
// КЛАС НАШОЇ ГРИ
// =========================================================
class SandboxGame : public Application {
private:
    OrthographicCamera m_Camera;
    Registry m_Registry;
    std::vector<EntityID> m_Entities;

public:
    SandboxGame()
        // Ініціалізуємо камеру в списку ініціалізації
        : m_Camera(-1.777f * 2.0f, 1.777f * 2.0f, -1.0f * 2.0f, 1.0f * 2.0f)
    {
        Renderer2D::Init();

        // Генеруємо сцену
        int gridSize = 15;
        for (int y = -gridSize; y < gridSize; ++y) {
            for (int x = -gridSize; x < gridSize; ++x) {
                EntityID entity = m_Registry.Create();

                m_Registry.Add<TransformComponent>(entity);
                auto& transform = m_Registry.Get<TransformComponent>(entity);
                transform.Position = glm::vec3(x * 0.35f, y * 0.35f, 0.0f);

                float distance = glm::length(glm::vec2(transform.Position.x, transform.Position.y));
                glm::vec4 color((x + gridSize) / (float)(gridSize * 2), 0.5f, (y + gridSize) / (float)(gridSize * 2), 1.0f);

                m_Registry.Add<ColorComponent>(entity);
                m_Registry.Get<ColorComponent>(entity).Color = color;

                m_Registry.Add<WaveAnimationComponent>(entity);
                m_Registry.Get<WaveAnimationComponent>(entity).Offset = distance;

                m_Entities.push_back(entity);
            }
        }
    }

    ~SandboxGame() {
        Renderer2D::Shutdown();
    }

    // Цей метод має викликатися рушієм кожного кадру
    void Update(float dt)  {
        RenderCommand::SetClearColor({ 0.08f, 0.08f, 0.1f, 1.0f });
        RenderCommand::Clear();

        float time = (float)glfwGetTime();

        // 1. СИСТЕМА ЛОГІКИ
        for (auto entity : m_Entities) {
            if (m_Registry.Has<TransformComponent>(entity) && m_Registry.Has<WaveAnimationComponent>(entity)) {
                auto& transform = m_Registry.Get<TransformComponent>(entity);
                auto& wave = m_Registry.Get<WaveAnimationComponent>(entity);

                float sineWave = std::sin(time * wave.Speed - wave.Offset);
                float newScale = (sineWave * 0.5f + 0.5f) * 0.25f + 0.05f;

                transform.Scale = glm::vec3(newScale, newScale, 1.0f);
                transform.Rotation = time * 2.0f + wave.Offset * 0.5f;
            }
        }

        // 2. СИСТЕМА РЕНДЕРИНГУ
        Renderer2D::BeginScene(m_Camera);
        for (auto entity : m_Entities) {
            if (m_Registry.Has<TransformComponent>(entity) && m_Registry.Has<ColorComponent>(entity)) {
                auto& transform = m_Registry.Get<TransformComponent>(entity);
                auto& colorComp = m_Registry.Get<ColorComponent>(entity);

                Renderer2D::DrawQuad(transform.Position, glm::vec2(transform.Scale.x, transform.Scale.y), colorComp.Color);
            }
        }
        Renderer2D::EndScene();
    }
    void Render()override {

    }
};

// =========================================================
// ПІДКЛЮЧЕННЯ ДО РУШІЯ (ЗВ'ЯЗОК З ENTRYPOINT)
// =========================================================
Engine::Core::Application* Engine::Core::CreateApplication() {
    // EntryPoint викличе цю функцію, щоб отримати екземпляр нашої гри
    return new SandboxGame();
}